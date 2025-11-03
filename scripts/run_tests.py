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

        mpi_running = (
            [self.mpi_exec] + shlex.split(additional_mpi_args) + ["-np", ppc_num_proc]
        )
        if not self.__ppc_env.get("PPC_ASAN_RUN"):
            for task_type in ["all", "mpi"]:
                self.__run_exec(
                    mpi_running
                    + [str(self.work_dir / "ppc_func_tests")]
                    + self.__get_gtest_settings(1, "_" + task_type + "_")
                )

    def run_performance(self):
        if not self.__ppc_env.get("PPC_ASAN_RUN"):
            mpi_running = [self.mpi_exec, "-np", self.__ppc_num_proc]
            for task_type in ["all", "mpi"]:
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
