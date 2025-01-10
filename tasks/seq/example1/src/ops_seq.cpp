#include "seq/example1/include/ops_seq.hpp"

#include <thread>

bool nesterov_a_test_task_seq1::TestTaskSequential::pre_processing_impl() {
  // Init value for input and output
  input_ = reinterpret_cast<int*>(taskData->inputs[0])[0];
  res = 0;
  return true;
}

bool nesterov_a_test_task_seq1::TestTaskSequential::validation_impl() {
  // Check count elements of output
  return taskData->inputs_count[0] == 1 && taskData->outputs_count[0] == 1;
}

bool nesterov_a_test_task_seq1::TestTaskSequential::run_impl() {
  for (int i = 0; i < input_; i++) {
    res++;
  }
  return true;
}

bool nesterov_a_test_task_seq1::TestTaskSequential::post_processing_impl() {
  reinterpret_cast<int*>(taskData->outputs[0])[0] = res;
  return true;
}
