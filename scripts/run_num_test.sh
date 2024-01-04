# shellcheck disable=SC2034
# shellcheck disable=SC2164
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
echo $SCRIPTPATH
python3 "$SCRIPTPATH/check_number_tests.py" "$SCRIPTPATH/../build/bin/core_func_tests"
python3 "$SCRIPTPATH/check_number_tests.py" "$SCRIPTPATH/../build/bin/ref_func_tests"
python3 "$SCRIPTPATH/check_number_tests.py" "$SCRIPTPATH/../build/bin/mpi_func_tests"
python3 "$SCRIPTPATH/check_number_tests.py" "$SCRIPTPATH/../build/bin/omp_func_tests"
python3 "$SCRIPTPATH/check_number_tests.py" "$SCRIPTPATH/../build/bin/seq_func_tests"
python3 "$SCRIPTPATH/check_number_tests.py" "$SCRIPTPATH/../build/bin/stl_func_tests"
python3 "$SCRIPTPATH/check_number_tests.py" "$SCRIPTPATH/../build/bin/tbb_func_tests"
