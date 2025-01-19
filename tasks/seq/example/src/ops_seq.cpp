#include "seq/example/include/ops_seq.hpp"

#include <thread>

bool nesterov_a_test_task_seq::TestTaskSequential::PreProcessingImpl() {
  // Init value for input and output
  unsigned int input_size = task_data->inputs_count[0];
  auto *in_ptr = reinterpret_cast<int*>(task_data->inputs[0]);
  input_ = std::vector<int>(in_ptr, in_ptr + input_size);

  unsigned int output_size = task_data->outputs_count[0];
  output_ = std::vector<int>(output_size, 0);

  rc_size = static_cast<int>(sqrt(input_size));
  return true;
}

bool nesterov_a_test_task_seq::TestTaskSequential::ValidationImpl() {
  // Check equality of counts elements
  return task_data->inputs_count[0] == task_data->outputs_count[0];
}

bool nesterov_a_test_task_seq::TestTaskSequential::RunImpl() {
  // Multiply matrices
  for (int i = 0; i < rc_size; ++i) {
    for (int j = 0; j < rc_size; ++j) {
      for (int k = 0; k < rc_size; ++k) {
        output_[(i * rc_size) + j] += input_[(i * rc_size) + k] * input_[(k * rc_size) + j];
      }
    }
  }
  return true;
}

bool nesterov_a_test_task_seq::TestTaskSequential::PostProcessingImpl() {
  for (size_t i = 0; i < output_.size(); i++) {
    reinterpret_cast<int *>(task_data->outputs[0])[i] = output_[i];
  }
  return true;
}
