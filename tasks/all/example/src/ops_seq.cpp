#include "seq/example/include/ops_seq.hpp"

#include <thread>

bool nesterov_a_test_task_seq::TestTaskSequential::PreProcessingImpl() {
  // Init value for input and output
  input_ = reinterpret_cast<int*>(task_data->inputs[0])[0];
  res_ = 0;
  return true;
}

bool nesterov_a_test_task_seq::TestTaskSequential::ValidationImpl() {
  // Check count elements of output
  return task_data->inputs_count[0] == 1 && task_data->outputs_count[0] == 1;
}

bool nesterov_a_test_task_seq::TestTaskSequential::RunImpl() {
  for (int i = 0; i < input_; i++) {
    res_++;
  }
  return true;
}

bool nesterov_a_test_task_seq::TestTaskSequential::PostProcessingImpl() {
  reinterpret_cast<int*>(task_data->outputs[0])[0] = res_;
  return true;
}
