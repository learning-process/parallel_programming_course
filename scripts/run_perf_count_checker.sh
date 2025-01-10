#!/bin/bash

REQUIRED_TESTS_COUNT=2

get_ref_tests_number() {
  # shellcheck disable=SC2155
  local dir_number=$(find "$1" -maxdepth 1 -type d | wc -l)
  # shellcheck disable=SC2004
  local task_number=$(($dir_number-1))
  # shellcheck disable=SC2004
  local tests_coeff=$(($REQUIRED_TESTS_COUNT+1))
  # shellcheck disable=SC2004
  local ref_tests_number=$(($task_number * $tests_coeff))
  echo "$ref_tests_number"
}

# shellcheck disable=SC2034
MPI_REF_NUM="$(get_ref_tests_number "tasks/mpi" 3)"
# shellcheck disable=SC2034
OMP_REF_NUM="$(get_ref_tests_number "tasks/omp" 3)"
# shellcheck disable=SC2034
SEQ_REF_NUM="$(get_ref_tests_number "tasks/seq" 3)"
# shellcheck disable=SC2034
STL_REF_NUM="$(get_ref_tests_number "tasks/stl" 3)"
# shellcheck disable=SC2034
TBB_REF_NUM="$(get_ref_tests_number "tasks/tbb" 3)"

get_current_tests_number() {
  # shellcheck disable=SC2155
  local ref_tests_number="$("$1" --gtest_list_tests | wc -l)"
  # shellcheck disable=SC2004
  ref_tests_number=$(($ref_tests_number+0))
  echo "$ref_tests_number"
}

MPI_CURR_NUM="$(get_current_tests_number "./build/bin/mpi_perf_tests")"
OMP_CURR_NUM="$(get_current_tests_number "./build/bin/omp_perf_tests")"
SEQ_CURR_NUM="$(get_current_tests_number "./build/bin/seq_perf_tests")"
STL_CURR_NUM="$(get_current_tests_number "./build/bin/stl_perf_tests")"
TBB_CURR_NUM="$(get_current_tests_number "./build/bin/tbb_perf_tests")"

get_error_msg() {
  echo "Count of perf tests in $1 != $REQUIRED_TESTS_COUNT"
}

if [[ MPI_REF_NUM -ne MPI_CURR_NUM ]]; then
  # shellcheck disable=SC2005
  echo "$(get_error_msg MPI)"
  exit 1
fi

if [[ OMP_REF_NUM -ne OMP_CURR_NUM ]]; then
  # shellcheck disable=SC2005
  echo "$(get_error_msg OMP)"
  exit 1
fi

if [[ SEQ_REF_NUM -ne SEQ_CURR_NUM ]]; then
  # shellcheck disable=SC2005
  echo "$(get_error_msg SEQ)"
  exit 1
fi

if [[ STL_REF_NUM -ne STL_CURR_NUM ]]; then
  # shellcheck disable=SC2005
  echo "$(get_error_msg STL)"
  exit 1
fi

if [[ TBB_REF_NUM -ne TBB_CURR_NUM ]]; then
  # shellcheck disable=SC2005
  echo "$(get_error_msg TBB)"
  exit 1
fi

echo "Success - perf count tests"
