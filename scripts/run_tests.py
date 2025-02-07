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
        choices=["threads", "processes"],
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


def get_project_path():
    script_path = Path(__file__).resolve()  # Absolute path of the script
    script_dir = script_path.parent  # Directory containing the script
    return script_dir.parent


def source_script(script_path):
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


def setup_env():
    ocv_script_path = "build/ppc_opencv/install/bin/setup_vars_opencv4.sh"
    if os.path.isfile(Path(get_project_path()) / ocv_script_path):
        _executable_path = Path(get_project_path()) / "build/bin"
        env_vars = source_script(Path(get_project_path()) / ocv_script_path)
    else:
        _executable_path = Path(get_project_path()) / "install/bin"
        env_vars = source_script(Path(_executable_path) / "setup_vars_opencv4.sh")

    os.environ.update(env_vars)
    return _executable_path


def run_exec_file(command):
    result = subprocess.run(command, shell=True, env=os.environ)
    if result.returncode != 0:
        raise Exception(f"Subprocess return {result.returncode}.")


def get_common_gtest_settings(repeats_count):
    command = "--gtest_also_run_disabled_tests "
    command += f"--gtest_repeat={repeats_count} "
    command += "--gtest_recreate_environments_when_repeating "
    command += "--gtest_color=0 "
    return command


def run_threads():
    print(123)


def run_common(install_bin_dir):
    valgrind_running = "valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all"

    if platform.system() == "Linux" and not os.environ.get("ASAN_RUN"):
        run_exec_file(f"{valgrind_running} {Path(install_bin_dir) / 'core_func_tests'} {get_common_gtest_settings(1)}")
        run_exec_file(f"{valgrind_running} {Path(install_bin_dir) / 'ref_func_tests'} {get_common_gtest_settings(1)}")

    run_exec_file(f"{Path(install_bin_dir) / 'core_func_tests'} {get_common_gtest_settings(10)}")
    run_exec_file(f"{Path(install_bin_dir) / 'ref_func_tests'}  {get_common_gtest_settings(10)}")


def run_processes(install_bin_dir, additional_mpi_args):
    proc_count = os.environ.get("PROC_COUNT")
    if proc_count is None:
        raise EnvironmentError("Required environment variable 'PROC_COUNT' is not set.")

    mpi_running = f"mpirun {additional_mpi_args} -np {proc_count}"
    if not os.environ.get("ASAN_RUN"):
        run_exec_file(f"{mpi_running} {Path(install_bin_dir) / 'all_func_tests'} {get_common_gtest_settings(10)}")
        run_exec_file(f"{mpi_running} {Path(install_bin_dir) / 'mpi_func_tests'} {get_common_gtest_settings(10)}")


if __name__ == "__main__":
    args_dict = init_cmd_args()
    executable_path = setup_env()
    run_common(executable_path)
    if args_dict["running_type"] == "threads":
        run_threads()
    elif args_dict["running_type"] == "processes":
        run_processes(executable_path, args_dict["additional_mpi_args"])
    else:
        raise Exception("running-type is wrong!")
