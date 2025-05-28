import os
import subprocess
import platform
from pathlib import Path


def init_cmd_args():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--running-type",
        required=True,
        choices=["threads", "processes", "performance", "performance-list"],
        help="Specify the execution mode. Choose 'threads' for multithreading or 'processes' for multiprocessing."
    )
    parser.add_argument(
        "--additional-mpi-args",
        required=False,
        default="",
        help="Additional MPI arguments to pass to the mpirun command (optional)."
    )
    args = parser.parse_args()
    _args_dict = vars(args)
    return _args_dict


class PPCRunner:
    def __init__(self):
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

    def setup_env(self):
        if (Path(self.__get_project_path()) / "install").exists():
            self.work_dir = Path(self.__get_project_path()) / "install" / "bin"
        else:
            self.work_dir = Path(self.__get_project_path()) / "build" / "bin"

    @staticmethod
    def __run_exec(command):
        result = subprocess.run(command, shell=True, env=os.environ)
        if result.returncode != 0:
            raise Exception(f"Subprocess return {result.returncode}.")

    @staticmethod
    def __get_gtest_settings(repeats_count, type_task):
        command = f"--gtest_repeat={repeats_count} "
        command += "--gtest_recreate_environments_when_repeating "
        command += "--gtest_color=0 "
        command += "--gtest_shuffle "
        command += f"--gtest_filter=\"*{type_task}*\" "
        return command

    def run_threads(self):
        if platform.system() == "Linux" and not os.environ.get("ASAN_RUN"):
            for task_type in ["seq", "stl"]:
                self.__run_exec(f"{self.valgrind_cmd} {self.work_dir / 'ppc_func_tests'} "
                                f"{self.__get_gtest_settings(1, '_' + task_type + '_')}")

        for task_type in ["omp", "seq", "stl", "tbb"]:
            self.__run_exec(f"{self.work_dir / 'ppc_func_tests'} {self.__get_gtest_settings(3, '_' + task_type + '_')}")

    def run_core(self):
        if platform.system() == "Linux" and not os.environ.get("ASAN_RUN"):
            self.__run_exec(f"{self.valgrind_cmd} {self.work_dir / 'core_func_tests'} "
                            f"{self.__get_gtest_settings(1, '*')}")

        self.__run_exec(f"{self.work_dir / 'core_func_tests'} {self.__get_gtest_settings(1, '*')}")

    def run_processes(self, additional_mpi_args):
        proc_count = os.environ.get("PROC_COUNT")
        if proc_count is None:
            raise EnvironmentError("Required environment variable 'PROC_COUNT' is not set.")

        mpi_running = f"{self.mpi_exec} {additional_mpi_args} -np {proc_count}"
        if not os.environ.get("ASAN_RUN"):
            for task_type in ["all", "mpi"]:
                self.__run_exec(f"{mpi_running} {self.work_dir / 'ppc_func_tests'} "
                                f"{self.__get_gtest_settings(10, '_' + task_type)}")

    def run_performance(self):
        if not os.environ.get("ASAN_RUN"):
            proc_count = os.environ.get("PROC_COUNT")
            if proc_count is None:
                raise EnvironmentError("Required environment variable 'PROC_COUNT' is not set.")
            mpi_running = f"{self.mpi_exec} -np {proc_count}"
            for task_type in ["all", "mpi"]:
                self.__run_exec(f"{mpi_running} {self.work_dir / 'ppc_perf_tests'} "
                                f"{self.__get_gtest_settings(1, '_' + task_type)}")

        for task_type in ["omp", "seq", "stl", "tbb"]:
            self.__run_exec(f"{self.work_dir / 'ppc_perf_tests'} {self.__get_gtest_settings(1, '_' + task_type)}")

    def run_performance_list(self):
        self.__run_exec(f"{self.work_dir / 'ppc_perf_tests'} --gtest_list_tests")


if __name__ == "__main__":
    args_dict = init_cmd_args()

    ppc_runner = PPCRunner()
    ppc_runner.setup_env()

    if args_dict["running_type"] in ["threads", "processes"]:
        ppc_runner.run_core()

    if args_dict["running_type"] == "threads":
        ppc_runner.run_threads()
    elif args_dict["running_type"] == "processes":
        ppc_runner.run_processes(args_dict["additional_mpi_args"])
    elif args_dict["running_type"] == "performance":
        ppc_runner.run_performance()
    elif args_dict["running_type"] == "performance-list":
        ppc_runner.run_performance_list()
    else:
        raise Exception("running-type is wrong!")
