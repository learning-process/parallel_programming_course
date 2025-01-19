#include "all/example/include/ops_tbb.hpp"

#include <tbb/tbb.h>

#include <functional>
#include <numeric>
#include <random>
#include <string>
#include <thread>
#include <vector>

bool nesterov_a_test_task_tbb::TestTBBTaskSequential::PreProcessingImpl() {
  // Init vectors
  input_ = std::vector<int>(task_data->inputs_count[0]);
  auto* tmp_ptr = reinterpret_cast<int*>(task_data->inputs[0]);
  for (unsigned i = 0; i < task_data->inputs_count[0]; i++) {
    input_[i] = tmp_ptr[i];
  }
  // Init value for output
  res_ = 1;
  return true;
}

bool nesterov_a_test_task_tbb::TestTBBTaskSequential::ValidationImpl() {
  // Check count elements of output
  return task_data->outputs_count[0] == 1;
}

bool nesterov_a_test_task_tbb::TestTBBTaskSequential::RunImpl() {
  if (ops_ == "+") {
    res_ = std::accumulate(input_.begin(), input_.end(), 1);
  } else if (ops_ == "-") {
    res_ -= std::accumulate(input_.begin(), input_.end(), 0);
  } else if (ops_ == "*") {
    res_ = std::accumulate(input_.begin(), input_.end(), 1, std::multiplies<>());
  }
  return true;
}

bool nesterov_a_test_task_tbb::TestTBBTaskSequential::PostProcessingImpl() {
  reinterpret_cast<int*>(task_data->outputs[0])[0] = res_;
  return true;
}

bool nesterov_a_test_task_tbb::TestTBBTaskParallel::PreProcessingImpl() {
  // Init vectors
  input_ = std::vector<int>(task_data->inputs_count[0]);
  auto* tmp_ptr = reinterpret_cast<int*>(task_data->inputs[0]);
  for (unsigned i = 0; i < task_data->inputs_count[0]; i++) {
    input_[i] = tmp_ptr[i];
  }
  // Init value for output
  res_ = 1;
  return true;
}

bool nesterov_a_test_task_tbb::TestTBBTaskParallel::ValidationImpl() {
  // Check count elements of output
  return task_data->outputs_count[0] == 1;
}

bool nesterov_a_test_task_tbb::TestTBBTaskParallel::RunImpl() {
  if (ops_ == "+") {
    res_ += oneapi::tbb::parallel_reduce(
        oneapi::tbb::blocked_range<std::vector<int>::iterator>(input_.begin(), input_.end()), 0,
        [](tbb::blocked_range<std::vector<int>::iterator> r, int running_total) {
          running_total += std::accumulate(r.begin(), r.end(), 0);
          return running_total;
        },
        std::plus<>());
  } else if (ops_ == "-") {
    res_ -= oneapi::tbb::parallel_reduce(
        oneapi::tbb::blocked_range<std::vector<int>::iterator>(input_.begin(), input_.end()), 0,
        [](tbb::blocked_range<std::vector<int>::iterator> r, int running_total) {
          running_total += std::accumulate(r.begin(), r.end(), 0);
          return running_total;
        },
        std::plus<>());
  } else if (ops_ == "*") {
    res_ *= oneapi::tbb::parallel_reduce(
        oneapi::tbb::blocked_range<std::vector<int>::iterator>(input_.begin(), input_.end()), 1,
        [](tbb::blocked_range<std::vector<int>::iterator> r, int running_total) {
          running_total *= std::accumulate(r.begin(), r.end(), 1, std::multiplies<>());
          return running_total;
        },
        std::multiplies<>());
  }
  return true;
}

bool nesterov_a_test_task_tbb::TestTBBTaskParallel::PostProcessingImpl() {
  reinterpret_cast<int*>(task_data->outputs[0])[0] = res_;
  return true;
}
