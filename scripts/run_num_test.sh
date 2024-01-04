# shellcheck disable=SC2034
# shellcheck disable=SC2164
BASEDIR=$(pwd)
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/core_func_tests"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/ref_func_tests"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/mpi_func_tests"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/omp_func_tests"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/seq_func_tests"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/stl_func_tests"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/tbb_func_tests"
