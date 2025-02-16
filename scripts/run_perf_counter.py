import subprocess
import re
import sys
from pathlib import Path


def init_cmd_args():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--required-tests-number",
        required=True,
        type=int,
        help="Specify the number of tests to run (must be an integer)."
    )
    args = parser.parse_args()
    _args_dict = vars(args)
    return _args_dict


def get_project_path():
    script_path = Path(__file__).resolve()
    script_dir = script_path.parent
    return script_dir.parent


def run_script(_script_path):
    result = subprocess.run(
        f"{sys.executable} {_script_path} --running-type=performance-list", shell=True, capture_output=True, text=True)
    if result.returncode != 0:
        raise Exception(f"Subprocess return {result.returncode}.")

    print(result.stdout)
    print(result.stderr)
    return result.stdout.splitlines()


if __name__ == "__main__":
    args_dict = init_cmd_args()
    tests_list = run_script(Path(get_project_path()) / "scripts/run_tests.py")
    tests_number = len(tests_list)

    pattern = r".*all\.|.*_mpi\.|.*_omp\.|.*_seq\.|.*_stl\.|.*_tbb\."
    test_matches = [test_name for test_name in tests_list if re.match(pattern, test_name)]
    required_tests_number = (args_dict["required_tests_number"] + 1) * len(test_matches)

    if tests_number != required_tests_number:
        raise Exception(f"Count of all tests {tests_number} != count of required tests {required_tests_number}.")
