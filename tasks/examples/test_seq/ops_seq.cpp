// Copyright 2023 Nesterov Alexander
#include "examples/test_seq/ops_seq.hpp"

#include <omp.h>

#include <iostream>
#include <random>
#include <string>
#include <vector>

bool TestTaskSequential::pre_processing() {
  internal_order_test();
  // Init value for input and output
  input_ = reinterpret_cast<int*>(taskData->inputs[0])[0];
  res = 0;
  return true;
}

bool TestTaskSequential::validation() {
  internal_order_test();
  // Check count elements of output
  if (taskData->inputs_count[0] == 1 && taskData->outputs_count[0]) {
    return true;
  } else {
    return false;
  }
}

bool TestTaskSequential::run() {
  internal_order_test();
  for (size_t i = 0; i < input_; i++) {
    res++;
  }
  return true;
}

bool TestTaskSequential::post_processing() {
  internal_order_test();
  reinterpret_cast<int*>(taskData->outputs[0])[0] = res;
  return true;
}
