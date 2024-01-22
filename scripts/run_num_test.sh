# shellcheck disable=SC2034
# shellcheck disable=SC2164
BASEDIR=$(pwd)
echo "--------------------------------"
echo core_func_tests
echo "--------------------------------"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/core_func_tests"
echo "--------------------------------"
echo ref_func_tests
echo "--------------------------------"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/ref_func_tests"
echo "--------------------------------"
echo mpi_func_tests
echo "--------------------------------"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/mpi_func_tests"
echo "--------------------------------"
echo omp_func_tests
echo "--------------------------------"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/omp_func_tests"
echo "--------------------------------"
echo seq_func_tests
echo "--------------------------------"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/seq_func_tests"
echo "--------------------------------"
echo stl_func_tests
echo "--------------------------------"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/stl_func_tests"
echo "--------------------------------"
echo tbb_func_tests
echo "--------------------------------"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/tbb_func_tests"

echo "--------------------------------"
echo mpi_perf_tests
echo "--------------------------------"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/mpi_perf_tests"
echo "--------------------------------"
echo omp_perf_tests
echo "--------------------------------"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/omp_perf_tests"
echo "--------------------------------"
echo seq_perf_tests
echo "--------------------------------"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/seq_perf_tests"
echo "--------------------------------"
echo stl_perf_tests
echo "--------------------------------"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/stl_perf_tests"
echo "--------------------------------"
echo tbb_perf_tests
echo "--------------------------------"
python3 "$BASEDIR/scripts/check_number_tests.py" "$BASEDIR/build/bin/tbb_perf_tests"
