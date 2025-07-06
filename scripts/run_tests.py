#!/usr/bin/env python3
"""
Test runner script for a PPC project.

This script provides functionality to run tests in different modes:
- threads: for multithreading tests
- processes: for multiprocessing tests
- processes_coverage: for multiprocessing tests with coverage collection
- performance: for performance testing
"""

import argparse
import os
import shlex
import subprocess
import platform
from pathlib import Path


def init_cmd_args():
    """Initialize and parse command line arguments."""
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--running-type",
        required=True,
        choices=["threads", "processes", "processes_coverage", "performance"],
        help="Specify the execution mode. Choose 'threads' for multithreading, "
             "'processes' for multiprocessing, 'processes_coverage' for multiprocessing "
             "with coverage, or 'performance' for performance testing."
    )
    parser.add_argument(
        "--additional-mpi-args",
        required=False,
        default="",
        help="Additional MPI arguments to pass to the mpirun command (optional)."
    )
    parser.add_argument(
        "--counts",
        nargs="+",
        type=int,
        help="List of process/thread counts to run sequentially"
    )
    args = parser.parse_args()
    _args_dict = vars(args)
    return _args_dict


class PPCRunner:
    """Runner class for PPC test execution in different modes."""

    def __init__(self):
        self.__ppc_num_threads = None
        self.__ppc_num_proc = None
        self.__ppc_env = None
        self.work_dir = None

        self.valgrind_cmd = "valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all"

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
        """Setup environment variables and working directory."""
        self.__ppc_env = ppc_env

        self.__ppc_num_threads = self.__ppc_env.get("PPC_NUM_THREADS")
        if self.__ppc_num_threads is None:
            raise EnvironmentError("Required environment variable 'PPC_NUM_THREADS' is not set.")
        self.__ppc_env["OMP_NUM_THREADS"] = self.__ppc_num_threads

        self.__ppc_num_proc = self.__ppc_env.get("PPC_NUM_PROC")
        if self.__ppc_num_proc is None:
            raise EnvironmentError("Required environment variable 'PPC_NUM_PROC' is not set.")

        if (Path(self.__get_project_path()) / "install").exists():
            self.work_dir = Path(self.__get_project_path()) / "install" / "bin"
        else:
            self.work_dir = Path(self.__get_project_path()) / "build" / "bin"

    def __run_exec(self, command):
        result = subprocess.run(command, shell=False, env=self.__ppc_env, check=False)
        if result.returncode != 0:
            raise subprocess.CalledProcessError(result.returncode, command)

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
        """Run tests in threading mode."""
        if (platform.system() == "Linux" and
                not self.__ppc_env.get("PPC_ASAN_RUN") and
                not self.__ppc_env.get("PPC_DISABLE_VALGRIND")):
            for task_type in ["seq", "stl"]:
                self.__run_exec(
                    shlex.split(self.valgrind_cmd)
                    + [str(self.work_dir / 'ppc_func_tests')]
                    + self.__get_gtest_settings(1, '_' + task_type + '_')
                )

        for task_type in ["omp", "seq", "stl", "tbb"]:
            self.__run_exec(
                [str(self.work_dir / 'ppc_func_tests')] +
                self.__get_gtest_settings(3, '_' + task_type + '_')
            )

    def run_core(self):
        """Run core functionality tests."""
        if (platform.system() == "Linux" and
                not self.__ppc_env.get("PPC_ASAN_RUN") and
                not self.__ppc_env.get("PPC_DISABLE_VALGRIND")):
            self.__run_exec(
                shlex.split(self.valgrind_cmd)
                + [str(self.work_dir / 'core_func_tests')]
                + self.__get_gtest_settings(1, '*')
                + ["--gtest_filter=*:-*_disabled_valgrind"]
            )

        self.__run_exec(
            [str(self.work_dir / 'core_func_tests')] + self.__get_gtest_settings(1, '*')
        )

    def run_processes(self, additional_mpi_args):
        """Run tests in multiprocessing mode."""
        ppc_num_proc = self.__ppc_env.get("PPC_NUM_PROC")
        if ppc_num_proc is None:
            raise EnvironmentError("Required environment variable 'PPC_NUM_PROC' is not set.")

        mpi_running = [self.mpi_exec] + shlex.split(additional_mpi_args) + ["-np", ppc_num_proc]
        if not self.__ppc_env.get("PPC_ASAN_RUN"):
            for task_type in ["all", "mpi"]:
                self.__run_exec(
                    mpi_running
                    + [str(self.work_dir / 'ppc_func_tests')]
                    + self.__get_gtest_settings(10, '_' + task_type)
                )

    def __create_coverage_wrapper(self, template_name, replacements):
        """Create a coverage wrapper script from the template."""
        template_path = (
            Path(self.__get_project_path()) / "scripts" / "templates" / template_name
        )
        wrapper_path = (
            Path(self.__get_project_path()) / "build" / template_name.replace('.template', '')
        )

        # Read template
        with open(template_path, 'r', encoding='utf-8') as template_file:
            content = template_file.read()

        # Replace placeholders
        for key, value in replacements.items():
            content = content.replace(f"{{{key}}}", value)

        # Write a wrapper script
        wrapper_path.write_text(content)
        wrapper_path.chmod(0o755)

        return wrapper_path

    def run_processes_coverage(self, additional_mpi_args):
        """Run tests in multiprocessing mode with a coverage collection."""
        ppc_num_proc = self.__ppc_env.get("PPC_NUM_PROC")
        if ppc_num_proc is None:
            raise EnvironmentError("Required environment variable 'PPC_NUM_PROC' is not set.")

        mpi_running = [self.mpi_exec] + shlex.split(additional_mpi_args) + ["-np", ppc_num_proc]

        # Set up coverage environment for MPI processes
        if not self.__ppc_env.get("PPC_ASAN_RUN"):
            # Check if we're using LLVM coverage or gcov
            llvm_profile_file = self.__ppc_env.get("LLVM_PROFILE_FILE")

            if llvm_profile_file:
                # LLVM coverage setup
                wrapper_script = self.__create_coverage_wrapper(
                    "mpi_llvm_coverage_wrapper.sh.template",
                    {"llvm_profile_base": llvm_profile_file.replace('%p', '%p').replace('%m', '%m')}
                )

                # Run tests with the LLVM coverage wrapper
                for task_type in ["all", "mpi"]:
                    test_command = (
                        mpi_running
                        + [str(wrapper_script)]
                        + [str(self.work_dir / 'ppc_func_tests')]
                        + self.__get_gtest_settings(10, '_' + task_type)
                    )
                    self.__run_exec(test_command)
            else:
                # Original gcov coverage setup
                # Enable coverage data collection for each MPI process
                self.__ppc_env["GCOV_PREFIX_STRIP"] = "0"
                # Use MPI rank to create unique coverage directories for each process
                gcov_base_dir = Path(self.__get_project_path()) / "build" / "gcov_data"
                gcov_base_dir.mkdir(parents=True, exist_ok=True)

                # Set GCOV_PREFIX to include MPI rank - this creates separate directories
                # for each MPI process at runtime
                self.__ppc_env["GCOV_PREFIX"] = str(
                    gcov_base_dir / "rank_${PMI_RANK:-${OMPI_COMM_WORLD_RANK:-${SLURM_PROCID:-0}}}"
                )

                # Create a wrapper script to set a unique prefix per process
                wrapper_script = self.__create_coverage_wrapper(
                    "mpi_gcov_coverage_wrapper.sh.template",
                    {"gcov_base_dir": str(gcov_base_dir)}
                )

                # Run tests with a coverage wrapper
                for task_type in ["all", "mpi"]:
                    test_command = (
                        mpi_running
                        + [str(wrapper_script)]
                        + [str(self.work_dir / 'ppc_func_tests')]
                        + self.__get_gtest_settings(10, '_' + task_type)
                    )
                    self.__run_exec(test_command)

    def run_performance(self):
        """Run performance tests."""
        if not self.__ppc_env.get("PPC_ASAN_RUN"):
            mpi_running = [self.mpi_exec, "-np", self.__ppc_num_proc]
            for task_type in ["all", "mpi"]:
                self.__run_exec(
                    mpi_running
                    + [str(self.work_dir / 'ppc_perf_tests')]
                    + self.__get_gtest_settings(1, '_' + task_type)
                )

        for task_type in ["omp", "seq", "stl", "tbb"]:
            self.__run_exec(
                [str(self.work_dir / 'ppc_perf_tests')] +
                self.__get_gtest_settings(1, '_' + task_type)
            )


def _execute(args_dict_, env):
    """Execute tests based on the provided arguments."""
    runner = PPCRunner()
    runner.setup_env(env)

    if args_dict_["running_type"] in ["threads", "processes", "processes_coverage"]:
        runner.run_core()

    if args_dict_["running_type"] == "threads":
        runner.run_threads()
    elif args_dict_["running_type"] == "processes":
        runner.run_processes(args_dict_["additional_mpi_args"])
    elif args_dict_["running_type"] == "processes_coverage":
        runner.run_processes_coverage(args_dict_["additional_mpi_args"])
    elif args_dict_["running_type"] == "performance":
        runner.run_performance()
    else:
        raise ValueError(f"Invalid running-type: {args_dict_['running_type']}")


if __name__ == "__main__":
    args_dict = init_cmd_args()
    counts = args_dict.get("counts")

    if counts:
        for count in counts:
            env_copy = os.environ.copy()

            if args_dict["running_type"] == "threads":
                env_copy["PPC_NUM_THREADS"] = str(count)
                env_copy.setdefault("PPC_NUM_PROC", "1")
            elif args_dict["running_type"] in ["processes", "processes_coverage"]:
                env_copy["PPC_NUM_PROC"] = str(count)
                env_copy.setdefault("PPC_NUM_THREADS", "1")

            print(f"Executing with {args_dict['running_type']} count: {count}")
            _execute(args_dict, env_copy)
    else:
        _execute(args_dict, os.environ.copy())
