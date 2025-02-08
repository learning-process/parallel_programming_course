import subprocess
import re
from pathlib import Path


def get_project_path():
    script_path = Path(__file__).resolve()
    script_dir = script_path.parent
    return script_dir.parent


def run_script(_script_path):
    result = subprocess.run(
        f"{sys.executable} {_script_path} --running-type=performance-list", shell=True, capture_output=True, text=True)
    if result.returncode != 0:
        raise Exception(f"Subprocess return {result.returncode}.")

    return result.stdout.splitlines()


if __name__ == "__main__":
    script_abs_path = Path(get_project_path()) / "scripts/run_tests.py"
    tests_list = run_script(script_abs_path)

    pattern = r".*all\.|.*_mpi.|.*_omp.|.*_seq.|.*_stl.|.*_tbb."
    test_matches = [test_name for test_name in tests_list if re.match(pattern, test_name)]
    if len(tests_list) != 3 * len(test_matches):
        raise Exception(f"Count of all test {len(tests_list)} != count of theoretical count test {len(test_matches)}.")
