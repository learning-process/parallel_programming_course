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
            self.ocv_script_name = "setup_vars_opencv4.cmd"
            self.ocv_script_path = Path("build/ppc_opencv/install/") / self.ocv_script_name
        else:
            self.ocv_script_name = "setup_vars_opencv4.sh"
            self.ocv_script_path = Path("build/ppc_opencv/install/bin/") / self.ocv_script_name

        if platform.system() == "Windows":
            self.mpi_exec = "mpiexec"
        else:
            self.mpi_exec = "mpirun"

    @staticmethod
    def __get_project_path():
        script_path = Path(__file__).resolve()  # Absolute path of the script
        script_dir = script_path.parent  # Directory containing the script
        return script_dir.parent

    @staticmethod
    def __source_script(script_path):
        if platform.system() == "Windows":
            return
        command = f"bash -c 'source {script_path} && env'"
        result = subprocess.run(command, stdout=subprocess.PIPE, shell=True, text=True)
        if result.returncode == 0:
            # Parse the output environment variables
            env_vars = {}
            for line in result.stdout.splitlines():
                if '=' in line:
                    key, value = line.split("=", 1)
                    env_vars[key] = value
            return env_vars
        else:
            print(f"Failed to source script: {script_path}")
            return {}

    def setup_env(self):
        if os.path.isfile(Path(self.__get_project_path()) / self.ocv_script_path):
            _work_dir = Path(self.__get_project_path()) / "build/bin"
            env_vars = self.__source_script(Path(self.__get_project_path()) / self.ocv_script_path)
        else:
            _work_dir = Path(self.__get_project_path()) / "install/bin"
            env_vars = self.__source_script(Path(_work_dir) / self.ocv_script_name)

        self.work_dir = Path(_work_dir)
        if not platform.system() == "Windows":
            os.environ.update(env_vars)

    @staticmethod
    def __run_exec(command):
        result = subprocess.run(command, shell=True, env=os.environ)
        if result.returncode != 0:
            raise Exception(f"Subprocess return {result.returncode}.")

    @staticmethod
    def __get_gtest_settings(repeats_count):
        command = "--gtest_also_run_disabled_tests "
        command += f"--gtest_repeat={repeats_count} "
        command += "--gtest_recreate_environments_when_repeating "
        command += "--gtest_color=0 "
        return command

    def run_threads(self):
        if platform.system() == "Linux" and not os.environ.get("ASAN_RUN"):
            self.__run_exec(f"{self.valgrind_cmd} {self.work_dir / 'seq_func_tests'} {self.__get_gtest_settings(1)}")
            self.__run_exec(f"{self.valgrind_cmd} {self.work_dir / 'stl_func_tests'} {self.__get_gtest_settings(1)}")

        self.__run_exec(f"{self.work_dir / 'seq_func_tests'} {self.__get_gtest_settings(3)}")
        self.__run_exec(f"{self.work_dir / 'stl_func_tests'} {self.__get_gtest_settings(3)}")
        self.__run_exec(f"{self.work_dir / 'tbb_func_tests'} {self.__get_gtest_settings(3)}")

        if os.environ.get("CLANG_BUILD") == "1":
            return
        self.__run_exec(f"{self.work_dir / 'omp_func_tests'} {self.__get_gtest_settings(3)}")

    def run_core(self):
        if platform.system() == "Linux" and not os.environ.get("ASAN_RUN"):
            self.__run_exec(f"{self.valgrind_cmd} {self.work_dir / 'core_func_tests'} {self.__get_gtest_settings(1)}")
            self.__run_exec(f"{self.valgrind_cmd} {self.work_dir / 'ref_func_tests'} {self.__get_gtest_settings(1)}")

        self.__run_exec(f"{self.work_dir / 'core_func_tests'} {self.__get_gtest_settings(1)}")
        self.__run_exec(f"{self.work_dir / 'ref_func_tests'}  {self.__get_gtest_settings(1)}")

    def run_processes(self, additional_mpi_args):
        if os.environ.get("CLANG_BUILD") == "1":
            return

        proc_count = os.environ.get("PROC_COUNT")
        if proc_count is None:
            raise EnvironmentError("Required environment variable 'PROC_COUNT' is not set.")

        mpi_running = f"{self.mpi_exec} {additional_mpi_args} -np {proc_count}"
        if not os.environ.get("ASAN_RUN"):
            self.__run_exec(f"{mpi_running} {self.work_dir / 'all_func_tests'} {self.__get_gtest_settings(10)}")
            self.__run_exec(f"{mpi_running} {self.work_dir / 'mpi_func_tests'} {self.__get_gtest_settings(10)}")

    def run_performance(self):
        if not os.environ.get("ASAN_RUN"):
            mpi_running = ""
            if platform.system() in ("Linux", "Windows"):
                mpi_running = f"{self.mpi_exec} -np 4"
            elif platform.system() == "Darwin":
                mpi_running = f"{self.mpi_exec} -np 2"
            self.__run_exec(f"{mpi_running} {self.work_dir / 'all_perf_tests'} {self.__get_gtest_settings(1)}")
            self.__run_exec(f"{mpi_running} {self.work_dir / 'mpi_perf_tests'} {self.__get_gtest_settings(1)}")

        self.__run_exec(f"{self.work_dir / 'omp_perf_tests'} {self.__get_gtest_settings(1)}")
        self.__run_exec(f"{self.work_dir / 'seq_perf_tests'} {self.__get_gtest_settings(1)}")
        self.__run_exec(f"{self.work_dir / 'stl_perf_tests'} {self.__get_gtest_settings(1)}")
        self.__run_exec(f"{self.work_dir / 'tbb_perf_tests'} {self.__get_gtest_settings(1)}")

    def run_performance_list(self):
        for task_type in ["all", "mpi", "omp", "seq", "stl", "tbb"]:
            self.__run_exec(f"{self.work_dir / f'{task_type}_perf_tests'} --gtest_list_tests")


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
