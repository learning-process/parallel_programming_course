#!/usr/bin/env python3

import os
import platform
import shlex
import shutil
import subprocess
from pathlib import Path


def init_cmd_args():
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--running-type",
        required=True,
        choices=["threads", "processes", "performance"],
        help="Specify the execution mode. Choose 'threads' for multithreading or 'processes' for multiprocessing.",
    )
    parser.add_argument(
        "--additional-mpi-args",
        required=False,
        default="",
        help="Additional MPI arguments to pass to the mpirun command (optional).",
    )
    parser.add_argument(
        "--counts",
        nargs="+",
        type=int,
        help="List of process/thread counts to run sequentially",
    )
    parser.add_argument(
        "--build-dir",
        default="build",
        help=(
            "Path to the CMake build directory (or its 'bin' subdirectory). "
            "Relative paths are resolved from the project root. "
            "Default: 'build'."
        ),
    )
    parser.add_argument(
        "--verbose", action="store_true", help="Print commands executed by the script"
    )
    args = parser.parse_args()
    _args_dict = vars(args)
    return _args_dict


class PPCRunner:
    def __init__(self, build_dir="build", verbose=False):
        self.__ppc_num_threads = None
        self.__ppc_num_proc = None
        self.__ppc_env = None
        self.__build_dir_path = None
        self.work_dir = None
        self.build_dir = build_dir
        self.verbose = verbose

        self.valgrind_cmd = (
            "valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all"
        )

        if platform.system() == "Windows":
            self.mpi_exec = "mpiexec"
            self.osh_exec = "mpiexec"
        else:
            self.mpi_exec = "mpirun"
            self.osh_exec = "oshrun"
        self.platform = platform.system()

        # Detect MPI implementation to choose compatible flags
        self.mpi_env_mode = "unknown"  # one of: openmpi, mpich, unknown
        self.mpi_np_flag = "-np"
        if self.platform == "Windows":
            # MSMPI uses -env and -n
            self.mpi_env_mode = "mpich"
            self.mpi_np_flag = "-n"
        else:
            self.mpi_env_mode, self.mpi_np_flag = self.__detect_mpi_impl()

    @staticmethod
    def __get_project_path():
        script_path = Path(__file__).resolve()  # Absolute path of the script
        script_dir = script_path.parent  # Directory containing the script
        return script_dir.parent

    def setup_env(self, ppc_env):
        self.__ppc_env = ppc_env

        self.__ppc_num_threads = self.__ppc_env.get("PPC_NUM_THREADS")
        if self.__ppc_num_threads is None:
            raise EnvironmentError(
                "Required environment variable 'PPC_NUM_THREADS' is not set."
            )
        self.__ppc_env["OMP_NUM_THREADS"] = self.__ppc_num_threads

        self.__ppc_num_proc = self.__ppc_env.get("PPC_NUM_PROC")
        if self.__ppc_num_proc is None:
            raise EnvironmentError(
                "Required environment variable 'PPC_NUM_PROC' is not set."
            )

        project_path = Path(self.__get_project_path())
        build_dir = Path(self.build_dir)
        if not build_dir.is_absolute():
            build_dir = project_path / build_dir
        self.__build_dir_path = build_dir

        install_bin_dir = project_path / "install" / "bin"
        if install_bin_dir.exists():
            self.work_dir = install_bin_dir
            self.__select_mpi_launcher()
            return

        bin_dir = build_dir if build_dir.name == "bin" else build_dir / "bin"
        if not bin_dir.exists():
            raise FileNotFoundError(
                f"Test binaries directory not found: '{bin_dir}'. "
                "Build the project or pass a correct '--build-dir' (e.g. 'build', 'build_seq', or 'build/bin')."
            )
        self.work_dir = bin_dir
        self.__select_mpi_launcher()

    def __run_exec(self, command, extra_env=None):
        if self.verbose:
            print("Executing:", " ".join(shlex.quote(part) for part in command))
        run_env = self.__ppc_env.copy()
        if extra_env:
            run_env.update(extra_env)
        result = subprocess.run(command, shell=False, env=run_env)
        if result.returncode != 0:
            raise Exception(f"Subprocess return {result.returncode}.")

    def __detect_mpi_impl(self):
        """Detect MPI implementation and return (env_mode, np_flag).
        env_mode: 'openmpi' -> use '-x VAR', 'mpich' -> use '-genvlist VAR1,VAR2', 'unknown' -> pass no env flags.
        np_flag: '-np' for OpenMPI/unknown, '-n' for MPICH-family.
        """
        probes = (["--version"], ["-V"], ["-v"], ["--help"], ["-help"])
        out = ""
        for args in probes:
            try:
                proc = subprocess.run(
                    [self.mpi_exec] + list(args),
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT,
                    text=True,
                )
                out = (proc.stdout or "").lower()
                if out:
                    break
            except Exception:
                continue

        if "open mpi" in out or "ompi" in out:
            return "openmpi", "-np"
        if (
            "hydra" in out
            or "mpich" in out
            or "intel(r) mpi" in out
            or "intel mpi" in out
        ):
            return "mpich", "-n"
        return "unknown", "-np"

    def __select_mpi_launcher(self):
        if self.platform == "Windows" or self.__build_dir_path is None:
            return

        project_path = Path(self.__get_project_path())
        mpi_candidates = []
        if self.work_dir is not None:
            mpi_candidates.append(self.work_dir / "mpirun")
        mpi_candidates.extend(
            [
                self.__build_dir_path / "ppc_openmpi" / "install" / "bin" / "mpirun",
                project_path / "install" / "bin" / "mpirun",
            ]
        )
        for launcher in mpi_candidates:
            if launcher.exists():
                self.mpi_exec = str(launcher)
                self.mpi_env_mode, self.mpi_np_flag = self.__detect_mpi_impl()
                break

        osh_candidates = []
        if self.work_dir is not None:
            osh_candidates.append(self.work_dir / "oshrun")
        osh_candidates.extend(
            [
                self.__build_dir_path / "ppc_openmpi" / "install" / "bin" / "oshrun",
                project_path / "install" / "bin" / "oshrun",
            ]
        )
        for launcher in osh_candidates:
            if launcher.exists():
                self.osh_exec = str(launcher)
                return
        self.osh_exec = self.mpi_exec

    def __supports_osh(self):
        return self.platform == "Linux"

    def __build_mpi_cmd(
        self, ppc_num_proc, additional_mpi_args, extra_env=None, launcher=None
    ):
        mpi_env = self.__ppc_env.copy()
        if extra_env:
            mpi_env.update(extra_env)
        additional_args = shlex.split(additional_mpi_args)
        if self.mpi_env_mode == "mpich":
            additional_args = [
                arg
                for arg in additional_args
                if arg not in ("--oversubscribe", "-oversubscribe")
            ]
        base = [launcher or self.mpi_exec] + additional_args
        forwarded_env = [
            "PPC_NUM_THREADS",
            "OMP_NUM_THREADS",
            "PPC_BENCHMARK_OUT",
            "PPC_BENCHMARK_FILTER",
            "PPC_PERF_IMPL_FILTER",
            "PPC_PERF_CATEGORY_FILTER",
        ]

        if self.platform == "Windows":
            # MS-MPI style
            env_args = []
            for env_name in forwarded_env:
                if env_name in mpi_env:
                    env_args += ["-env", env_name, mpi_env[env_name]]
            np_args = ["-n", ppc_num_proc]
            return base + env_args + np_args

        # Non-Windows
        if self.mpi_env_mode == "openmpi":
            env_args = []
            for env_name in forwarded_env:
                if env_name in mpi_env:
                    env_args += ["-x", env_name]
            np_flag = "-np"
        elif self.mpi_env_mode == "mpich":
            # Explicitly set env variables for all ranks
            env_args = []
            for env_name in forwarded_env:
                if env_name in mpi_env:
                    env_args += ["-env", env_name, mpi_env[env_name]]
            np_flag = "-n"
        else:
            # Unknown MPI flavor: rely on environment inheritance and default to -np
            env_args = []
            np_flag = "-np"

        return base + env_args + [np_flag, ppc_num_proc]

    def __benchmark_output_dir(self):
        if self.__build_dir_path is None:
            raise RuntimeError("Build directory is not initialized.")
        return self.__build_dir_path / "perf_stat_dir" / "benchmarks"

    def __get_performance_gtest_settings(self):
        return [
            "--gtest_repeat=1",
            "--gtest_recreate_environments_when_repeating",
            "--gtest_color=0",
            "--gtest_filter=*RunPerf*",
        ]

    def __get_benchmark_env(self, category, task_type):
        output_dir = self.__benchmark_output_dir()
        output_dir.mkdir(parents=True, exist_ok=True)
        return {
            "PPC_PERF_CATEGORY_FILTER": f"_{category}_",
            "PPC_PERF_IMPL_FILTER": f"_{task_type}_",
            "PPC_BENCHMARK_OUT": str(
                output_dir / f"benchmark_{category}_{task_type}.json"
            ),
        }

    @staticmethod
    def __get_gtest_settings(repeats_count, type_task):
        type_task_patterns = {
            "_all_": ["_all_", "AllEnabled"],
            "_mpi_": ["_mpi_", "MpiEnabled"],
            "_osh_": ["_osh_", "OSHEnabled"],
            "_omp_": ["_omp_", "OmpEnabled"],
            "_seq_": ["_seq_", "SeqEnabled"],
            "_stl_": ["_stl_", "StlEnabled"],
            "_tbb_": ["_tbb_", "TbbEnabled"],
        }
        filter_patterns = type_task_patterns.get(type_task, [type_task])
        command = [
            f"--gtest_repeat={repeats_count}",
            "--gtest_recreate_environments_when_repeating",
            "--gtest_color=0",
            "--gtest_shuffle",
            "--gtest_filter=" + ":".join(f"*{pattern}*" for pattern in filter_patterns),
        ]
        return command

    def run_threads(self):
        if platform.system() == "Linux" and not self.__ppc_env.get("PPC_ASAN_RUN"):
            for task_type in ["seq", "stl"]:
                self.__run_exec(
                    shlex.split(self.valgrind_cmd)
                    + [str(self.work_dir / "ppc_func_tests")]
                    + self.__get_gtest_settings(1, "_" + task_type + "_")
                )

        for task_type in ["omp", "seq", "stl", "tbb"]:
            self.__run_exec(
                [str(self.work_dir / "ppc_func_tests")]
                + self.__get_gtest_settings(1, "_" + task_type + "_")
            )

    def run_core(self):
        if platform.system() == "Linux" and not self.__ppc_env.get("PPC_ASAN_RUN"):
            self.__run_exec(
                shlex.split(self.valgrind_cmd)
                + [str(self.work_dir / "core_func_tests")]
                + self.__get_gtest_settings(1, "*")
                + ["--gtest_filter=*:-*DisabledValgrind"]
            )

        self.__run_exec(
            [str(self.work_dir / "core_func_tests")] + self.__get_gtest_settings(1, "*")
        )

    def run_processes(self, additional_mpi_args):
        ppc_num_proc = self.__ppc_env.get("PPC_NUM_PROC")
        if ppc_num_proc is None:
            raise EnvironmentError(
                "Required environment variable 'PPC_NUM_PROC' is not set."
            )
        mpi_running = self.__build_mpi_cmd(ppc_num_proc, additional_mpi_args)
        if not self.__ppc_env.get("PPC_ASAN_RUN"):
            for task_type in ["all", "mpi"]:
                self.__run_exec(
                    mpi_running
                    + [str(self.work_dir / "ppc_func_tests")]
                    + self.__get_gtest_settings(1, "_" + task_type + "_")
                )
            if self.__supports_osh():
                osh_running = self.__build_mpi_cmd(
                    ppc_num_proc, additional_mpi_args, launcher=self.osh_exec
                )
                self.__run_exec(
                    osh_running
                    + [str(self.work_dir / "ppc_func_tests")]
                    + self.__get_gtest_settings(1, "_osh_")
                )

    def run_performance(self):
        output_dir = self.__benchmark_output_dir()
        if output_dir.exists():
            shutil.rmtree(output_dir)

        if not self.__ppc_env.get("PPC_ASAN_RUN"):
            perf_jobs = [
                ("threads", "all"),
                ("processes", "mpi"),
                ("processes", "seq"),
            ]
            if self.__supports_osh():
                perf_jobs.insert(2, ("processes", "osh"))
            for category, task_type in perf_jobs:
                extra_env = self.__get_benchmark_env(category, task_type)
                launcher = (
                    self.osh_exec
                    if category == "processes" and task_type == "osh"
                    else None
                )
                runner = self.__build_mpi_cmd(
                    self.__ppc_num_proc, "", extra_env, launcher
                )
                self.__run_exec(
                    runner
                    + [str(self.work_dir / "ppc_perf_tests")]
                    + self.__get_performance_gtest_settings(),
                    extra_env,
                )

        for task_type in ["omp", "seq", "stl", "tbb"]:
            extra_env = self.__get_benchmark_env("threads", task_type)
            self.__run_exec(
                [str(self.work_dir / "ppc_perf_tests")]
                + self.__get_performance_gtest_settings(),
                extra_env,
            )


def _execute(args_dict, env):
    runner = PPCRunner(
        build_dir=args_dict.get("build_dir", "build"),
        verbose=args_dict.get("verbose", False),
    )
    runner.setup_env(env)

    if args_dict["running_type"] in ["threads", "processes"]:
        runner.run_core()

    if args_dict["running_type"] == "threads":
        runner.run_threads()
    elif args_dict["running_type"] == "processes":
        runner.run_processes(args_dict["additional_mpi_args"])
    elif args_dict["running_type"] == "performance":
        runner.run_performance()
    else:
        raise Exception("running-type is wrong!")


if __name__ == "__main__":
    args_dict = init_cmd_args()
    counts = args_dict.get("counts")

    if counts:
        for count in counts:
            env_copy = os.environ.copy()

            if args_dict["running_type"] == "threads":
                env_copy["PPC_NUM_THREADS"] = str(count)
                env_copy.setdefault("PPC_NUM_PROC", "1")
            elif args_dict["running_type"] == "processes":
                env_copy["PPC_NUM_PROC"] = str(count)
                env_copy.setdefault("PPC_NUM_THREADS", "1")

            print(
                f"Executing with {args_dict['running_type']} count: {count}", flush=True
            )
            _execute(args_dict, env_copy)
    else:
        _execute(args_dict, os.environ.copy())
