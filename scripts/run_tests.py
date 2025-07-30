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
        choices=["threads", "processes", "performance", "processes_coverage"],
        help="Specify the execution mode. Choose 'threads' for multithreading, "
        "'processes' for multiprocessing, or 'processes_coverage' for coverage generation.",
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
    parser.add_argument(
        "--llvm-version",
        default="20",
        help="LLVM version for coverage tools (default: 20)",
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
                + self.__get_gtest_settings(3, "_" + task_type + "_")
            )

    def run_core(self):
        if platform.system() == "Linux" and not self.__ppc_env.get("PPC_ASAN_RUN"):
            self.__run_exec(
                shlex.split(self.valgrind_cmd)
                + [str(self.work_dir / "core_func_tests")]
                + self.__get_gtest_settings(1, "*")
                + ["--gtest_filter=*:-*_disabled_valgrind"]
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

        mpi_running = (
            [self.mpi_exec] + shlex.split(additional_mpi_args) + ["-np", ppc_num_proc]
        )
        if not self.__ppc_env.get("PPC_ASAN_RUN"):
            for task_type in ["all", "mpi"]:
                self.__run_exec(
                    mpi_running
                    + [str(self.work_dir / "ppc_func_tests")]
                    + self.__get_gtest_settings(10, "_" + task_type)
                )

    def run_performance(self):
        if not self.__ppc_env.get("PPC_ASAN_RUN"):
            mpi_running = [self.mpi_exec, "-np", self.__ppc_num_proc]
            for task_type in ["all", "mpi"]:
                self.__run_exec(
                    mpi_running
                    + [str(self.work_dir / "ppc_perf_tests")]
                    + self.__get_gtest_settings(1, "_" + task_type)
                )

        for task_type in ["omp", "seq", "stl", "tbb"]:
            self.__run_exec(
                [str(self.work_dir / "ppc_perf_tests")]
                + self.__get_gtest_settings(1, "_" + task_type)
            )

    def generate_coverage(self, llvm_version="20"):
        """Generate LLVM coverage report after running tests."""

        # Find llvm-profdata and llvm-cov
        if llvm_version:
            profdata_names = [f"llvm-profdata-{llvm_version}", "llvm-profdata"]
            cov_names = [f"llvm-cov-{llvm_version}", "llvm-cov"]
        else:
            profdata_names = ["llvm-profdata"]
            cov_names = ["llvm-cov"]

        llvm_profdata = None
        llvm_cov = None

        for name in profdata_names:
            result = subprocess.run(["which", name], capture_output=True, text=True)
            if result.returncode == 0:
                llvm_profdata = name
                break

        for name in cov_names:
            result = subprocess.run(["which", name], capture_output=True, text=True)
            if result.returncode == 0:
                llvm_cov = name
                break

        if not llvm_profdata or not llvm_cov:
            raise Exception("Could not find llvm-profdata or llvm-cov in PATH")

        build_dir = self.work_dir.parent
        output_dir = build_dir / "coverage"
        output_dir.mkdir(exist_ok=True)

        # Find all .profraw files
        profraw_files = list(build_dir.glob("**/*.profraw"))
        if not profraw_files:
            raise Exception(
                "No .profraw files found. Make sure to run tests with LLVM_PROFILE_FILE set."
            )

        print(f"Found {len(profraw_files)} .profraw files")

        # Merge profiles
        profdata_file = output_dir / "coverage.profdata"
        cmd = (
            [llvm_profdata, "merge", "-sparse"]
            + [str(f) for f in profraw_files]
            + ["-o", str(profdata_file)]
        )
        if self.verbose:
            print("Executing:", " ".join(shlex.quote(part) for part in cmd))
        result = subprocess.run(cmd)
        if result.returncode != 0:
            raise Exception("Failed to merge coverage profiles")

        # Find executables
        executables = []
        for f in self.work_dir.iterdir():
            if f.is_file() and os.access(f, os.X_OK) and not f.suffix == ".txt":
                executables.append(str(f))

        if not executables:
            raise Exception("No executables found in bin directory")

        print(f"Found {len(executables)} executables")

        # Get project root
        project_root = build_dir.parent

        # Generate LCOV report
        lcov_file = output_dir / "coverage.lcov"
        cmd = (
            [llvm_cov, "export"]
            + executables
            + [
                "--format=lcov",
                "--ignore-filename-regex=.*3rdparty/.*|/usr/.*|.*tests/.*|"
                ".*tasks/.*|.*modules/runners/.*|.*modules/util/include/perf_test_util.hpp|"
                ".*modules/util/include/func_test_util.hpp|.*modules/util/src/func_test_util.cpp",
                f"--instr-profile={profdata_file}",
            ]
        )

        with open(lcov_file, "w") as f:
            result = subprocess.run(cmd, stdout=f, stderr=subprocess.PIPE, text=True)
            if result.returncode != 0:
                raise Exception(f"Error generating LCOV report: {result.stderr}")

        # Post-process LCOV file to use relative paths
        with open(lcov_file, "r") as f:
            lcov_content = f.read()

        lcov_content = lcov_content.replace(str(project_root) + "/", "")

        with open(lcov_file, "w") as f:
            f.write(lcov_content)

        print(f"Generated LCOV report: {lcov_file}")

        # Generate HTML report
        html_dir = output_dir / "html"
        cmd = (
            [llvm_cov, "show"]
            + executables
            + [
                "--format=html",
                f"--output-dir={html_dir}",
                "--ignore-filename-regex=.*3rdparty/.*|/usr/.*|.*tests/.*|"
                ".*tasks/.*|.*modules/runners/.*|.*modules/util/include/perf_test_util.hpp|"
                ".*modules/util/include/func_test_util.hpp|.*modules/util/src/func_test_util.cpp",
                f"--instr-profile={profdata_file}",
            ]
        )

        if self.verbose:
            print("Executing:", " ".join(shlex.quote(part) for part in cmd))
        result = subprocess.run(cmd)
        if result.returncode != 0:
            raise Exception("Failed to generate HTML coverage report")

        print(f"Generated HTML report: {html_dir}/index.html")

        # Generate summary
        cmd = (
            [llvm_cov, "report"]
            + executables
            + [
                f"--instr-profile={profdata_file}",
                "--ignore-filename-regex=.*3rdparty/.*|/usr/.*|.*tasks/.*/tests/.*|.*modules/.*/tests/.*|"
                ".*tasks/common/runners/.*|.*modules/runners/.*|.*modules/util/include/perf_test_util.hpp|"
                ".*modules/util/include/func_test_util.hpp|.*modules/util/src/func_test_util.cpp",
            ]
        )

        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode == 0:
            print("\nCoverage Summary:")
            print(result.stdout)


def _execute(args_dict, env):
    runner = PPCRunner(verbose=args_dict.get("verbose", False))
    runner.setup_env(env)

    if args_dict["running_type"] in ["threads", "processes", "processes_coverage"]:
        runner.run_core()

    if args_dict["running_type"] == "threads":
        runner.run_threads()
    elif args_dict["running_type"] == "processes":
        runner.run_processes(args_dict["additional_mpi_args"])
    elif args_dict["running_type"] == "processes_coverage":
        # Run processes tests
        runner.run_processes(args_dict["additional_mpi_args"])
        # Generate coverage report
        runner.generate_coverage(args_dict.get("llvm_version", "20"))
    elif args_dict["running_type"] == "performance":
        runner.run_performance()
    else:
        raise Exception("running-type is wrong!")


if __name__ == "__main__":
    args_dict = init_cmd_args()
    counts = args_dict.get("counts")

    if args_dict["running_type"] == "processes_coverage":
        # For coverage, set environment variables for profiling
        env_copy = os.environ.copy()
        env_copy["LLVM_PROFILE_FILE"] = "coverage-%p-%m.profraw"
        env_copy["PPC_NUM_PROC"] = "2"
        env_copy["PPC_NUM_THREADS"] = "2"
        env_copy["OMPI_ALLOW_RUN_AS_ROOT"] = "1"
        env_copy["OMPI_ALLOW_RUN_AS_ROOT_CONFIRM"] = "1"

        # Run threads tests with different counts
        print("Running thread tests with coverage...")
        threads_args = args_dict.copy()
        threads_args["running_type"] = "threads"
        for count in [1, 2, 3, 4]:
            env_threads = env_copy.copy()
            env_threads["PPC_NUM_THREADS"] = str(count)
            env_threads["PPC_NUM_PROC"] = "1"
            print(f"Executing with threads count: {count}")
            try:
                _execute(threads_args, env_threads)
            except Exception as e:
                print(f"Warning: Thread tests with count {count} failed: {e}")

        # Now run the process coverage tests
        print("\nRunning process tests with coverage...")
        _execute(args_dict, env_copy)
    elif counts:
        for count in counts:
            env_copy = os.environ.copy()

            if args_dict["running_type"] == "threads":
                env_copy["PPC_NUM_THREADS"] = str(count)
                env_copy.setdefault("PPC_NUM_PROC", "1")
            elif args_dict["running_type"] == "processes":
                env_copy["PPC_NUM_PROC"] = str(count)
                env_copy.setdefault("PPC_NUM_THREADS", "1")

            print(f"Executing with {args_dict['running_type']} count: {count}")
            _execute(args_dict, env_copy)
    else:
        _execute(args_dict, os.environ.copy())
