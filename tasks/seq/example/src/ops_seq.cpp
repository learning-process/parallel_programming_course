// Copyright 2023 Nesterov Alexander
#include "seq/example/include/ops_seq.hpp"

#include <thread>

auto awake_time() {
  using std::chrono::operator"" ms;
  return std::chrono::steady_clock::now() + 20ms;
}

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
  return taskData->inputs_count[0] == 1 && taskData->outputs_count[0] == 1;
}

bool TestTaskSequential::run() {
  internal_order_test();
  for (size_t i = 0; i < input_; i++) {
    res++;
  }
  std::this_thread::sleep_until(awake_time());
  return true;
}

bool TestTaskSequential::post_processing() {
  internal_order_test();
  reinterpret_cast<int*>(taskData->outputs[0])[0] = res;
  return true;
}
