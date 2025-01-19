#include "all/example/include/ops_all.hpp"

#include <cmath>
#include <thread>

#include "core/util/util.hpp"

namespace {
void MatMul(const std::vector<int> &in_vec, int rc_size, std::vector<int> &out_vec) {
  for (int i = 0; i < rc_size; ++i) {
    for (int j = 0; j < rc_size; ++j) {
      for (int k = 0; k < rc_size; ++k) {
        out_vec[(i * rc_size) + j] += in_vec[(i * rc_size) + k] * in_vec[(k * rc_size) + j];
      }
    }
  }
}
void MatMulElse(const std::vector<int> &in_vec, int rc_size, std::vector<int> &out_vec) {
  for (int k = 0; k < rc_size; ++k) {
    for (int j = 0; j < rc_size; ++j) {
      for (int i = 0; i < rc_size; ++i) {
        out_vec[(i * rc_size) + j] += in_vec[(i * rc_size) + k] * in_vec[(k * rc_size) + j];
      }
    }
  }
}
}  // namespace

bool nesterov_a_test_task_all::TestTaskALL::PreProcessingImpl() {
  // Init value for input and output
  unsigned int input_size = task_data->inputs_count[0];
  auto *in_ptr = reinterpret_cast<int *>(task_data->inputs[0]);
  input_ = std::vector<int>(in_ptr, in_ptr + input_size);

  unsigned int output_size = task_data->outputs_count[0];
  output_ = std::vector<int>(output_size, 0);

  rc_size_ = static_cast<int>(std::sqrt(input_size));
  return true;
}

bool nesterov_a_test_task_all::TestTaskALL::ValidationImpl() {
  // Check equality of counts elements
  return task_data->inputs_count[0] == task_data->outputs_count[0];
}

bool nesterov_a_test_task_all::TestTaskALL::RunImpl() {
  if (world_.rank() == 0) {
#pragma omp parallel
    { MatMul(input_, rc_size_, output_); }
  } else if (world_.rank() == 1) {
    const int num_threads = ppc::util::GetPPCNumThreads();
    std::vector<std::thread> threads(num_threads);
    for (int i = 0; i < num_threads; i++) {
      threads[i] = std::thread(MatMul, std::cref(input_), rc_size_, std::ref(output_));
      threads[i].join();
    }
  } else if (world_.rank() == 2) {
    oneapi::tbb::task_arena arena;
    arena.execute([&] { MatMul(input_, rc_size_, output_); });
  } else {
    MatMulElse(input_, rc_size_, output_);
  }
  world_.barrier();
  return true;
}

bool nesterov_a_test_task_all::TestTaskALL::PostProcessingImpl() {
  for (size_t i = 0; i < output_.size(); i++) {
    reinterpret_cast<int *>(task_data->outputs[0])[i] = output_[i];
  }
  return true;
}
