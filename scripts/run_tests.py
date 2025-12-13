#!/usr/bin/env python3

import os
import shlex
import subprocess
import platform
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
        "--verbose", action="store_true", help="Print commands executed by the script"
    )
    args = parser.parse_args()
    _args_dict = vars(args)
    return _args_dict


class PPCRunner:
    def __init__(self, verbose=False):
        self.__ppc_num_threads = None
        self.__ppc_num_proc = None
        self.__ppc_env = None
        self.work_dir = None
        self.verbose = verbose

        self.valgrind_cmd = (
            "valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all"
        )

        if platform.system() == "Windows":
            self.mpi_exec = "mpiexec"
        else:
            self.mpi_exec = "mpirun"
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

        if (Path(self.__get_project_path()) / "install").exists():
            self.work_dir = Path(self.__get_project_path()) / "install" / "bin"
        else:
            self.work_dir = Path(self.__get_project_path()) / "build" / "bin"

    def __run_exec(self, command):
        if self.verbose:
            print("Executing:", " ".join(shlex.quote(part) for part in command))
        result = subprocess.run(command, shell=False, env=self.__ppc_env)
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

    def __build_mpi_cmd(self, ppc_num_proc, additional_mpi_args):
        base = [self.mpi_exec] + shlex.split(additional_mpi_args)

        if self.platform == "Windows":
            # MS-MPI style
            env_args = [
                "-env",
                "PPC_NUM_THREADS",
                self.__ppc_env["PPC_NUM_THREADS"],
                "-env",
                "OMP_NUM_THREADS",
                self.__ppc_env["OMP_NUM_THREADS"],
            ]
            np_args = ["-n", ppc_num_proc]
            return base + env_args + np_args

        # Non-Windows
        if self.mpi_env_mode == "openmpi":
            env_args = [
                "-x",
                "PPC_NUM_THREADS",
                "-x",
                "OMP_NUM_THREADS",
            ]
            np_flag = "-np"
        elif self.mpi_env_mode == "mpich":
            # Explicitly set env variables for all ranks
            env_args = [
                "-env",
                "PPC_NUM_THREADS",
                self.__ppc_env["PPC_NUM_THREADS"],
                "-env",
                "OMP_NUM_THREADS",
                self.__ppc_env["OMP_NUM_THREADS"],
            ]
            np_flag = "-n"
        else:
            # Unknown MPI flavor: rely on environment inheritance and default to -np
            env_args = []
            np_flag = "-np"

        return base + env_args + [np_flag, ppc_num_proc]

    @staticmethod
    def __get_gtest_settings(repeats_count, type_task):
        command = [
            f"--gtest_repeat={repeats_count}",
            "--gtest_recreate_environments_when_repeating",
            "--gtest_color=0",
            "--gtest_shuffle",
            f"--gtest_filter=*{type_task}*",
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

    def run_performance(self):
        if not self.__ppc_env.get("PPC_ASAN_RUN"):
            mpi_running = self.__build_mpi_cmd(self.__ppc_num_proc, "")
            # Collect perf stats for all implementations, including seq as baseline.
            for task_type in ["all", "mpi", "seq"]:
                self.__run_exec(
                    mpi_running
                    + [str(self.work_dir / "ppc_perf_tests")]
                    + self.__get_gtest_settings(1, "_" + task_type + "_")
                )

        for task_type in ["omp", "seq", "stl", "tbb"]:
            self.__run_exec(
                [str(self.work_dir / "ppc_perf_tests")]
                + self.__get_gtest_settings(1, "_" + task_type + "_")
            )


def _execute(args_dict, env):
    runner = PPCRunner(verbose=args_dict.get("verbose", False))
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
