#!/bin/bash

if [[ -f build/ppc_opencv/install/bin/setup_vars_opencv4.sh ]]; then
  INSTALL_BIN_DIR="build/bin"
  source build/ppc_opencv/install/bin/setup_vars_opencv4.sh
else
  INSTALL_BIN_DIR="install/bin"
  source $INSTALL_BIN_DIR/setup_vars_opencv4.sh
fi

REQUIRED_TESTS_COUNT=2

get_ref_tests_number() {
  # shellcheck disable=SC2155
  local dir_number=$(find "$1" -maxdepth 1 -type d ! -name "*_disabled" | wc -l)
  # shellcheck disable=SC2004
  local task_number=$(($dir_number-1))
  # shellcheck disable=SC2004
  local tests_coeff=$(($REQUIRED_TESTS_COUNT+1))
  # shellcheck disable=SC2004
  local ref_tests_number=$(($task_number * $tests_coeff))
  echo "$ref_tests_number"
}

get_current_tests_number() {
  # shellcheck disable=SC2155
  local ref_tests_number="$("$1" --gtest_list_tests | wc -l)"
  # shellcheck disable=SC2004
  ref_tests_number=$(($ref_tests_number+0))
  echo "$ref_tests_number"
}

get_error_msg() {
  echo "Count of perf tests in $1 != $REQUIRED_TESTS_COUNT"
}

run_check() {
  # shellcheck disable=SC2155
  # shellcheck disable=SC2004
  local ref_num="$(get_ref_tests_number "tasks/$1" $(($REQUIRED_TESTS_COUNT+1)))"
  # shellcheck disable=SC2155
  local curr_num="$(get_current_tests_number "$INSTALL_BIN_DIR/$1_perf_tests")"

  if [[ ref_num -ne curr_num ]]; then
    # shellcheck disable=SC2005
    echo "$(get_error_msg "$1")"
    exit 1
  fi
}

run_check "all"
run_check "mpi"
run_check "omp"
run_check "seq"
run_check "stl"
run_check "tbb"

echo "Success - perf count tests"
