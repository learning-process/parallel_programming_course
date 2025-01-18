#include "all/example/include/ops_stl.hpp"

#include <future>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <utility>
#include <vector>

bool nesterov_a_test_task_stl::TestSTLTaskSequential::PreProcessingImpl() {
  // Init vectors
  input_ = std::vector<int>(task_data->inputs_count[0]);
  auto *tmp_ptr = reinterpret_cast<int *>(task_data->inputs[0]);
  for (unsigned i = 0; i < task_data->inputs_count[0]; i++) {
    input_[i] = tmp_ptr[i];
  }
  // Init value for output
  res_ = 0;
  return true;
}

bool nesterov_a_test_task_stl::TestSTLTaskSequential::ValidationImpl() {
  // Check count elements of output
  return task_data->outputs_count[0] == 1;
}

bool nesterov_a_test_task_stl::TestSTLTaskSequential::RunImpl() {
  if (ops_ == "+") {
    res_ = std::accumulate(input_.begin(), input_.end(), 0);
  } else if (ops_ == "-") {
    res_ -= std::accumulate(input_.begin(), input_.end(), 0);
  }
  return true;
}

bool nesterov_a_test_task_stl::TestSTLTaskSequential::PostProcessingImpl() {
  reinterpret_cast<int *>(task_data->outputs[0])[0] = res_;
  return true;
}

namespace {
std::mutex my_mutex;

void AtomOps(std::vector<int> vec, const std::string &ops, std::promise<int> &&pr) {
  auto sz = vec.size();
  int reduction_elem = 0;
  if (ops == "+") {
    for (size_t i = 0; i < sz; i++) {
      std::lock_guard<std::mutex> my_lock(my_mutex);
      reduction_elem += vec[i];
    }
  } else if (ops == "-") {
    for (size_t i = 0; i < sz; i++) {
      std::lock_guard<std::mutex> my_lock(my_mutex);
      reduction_elem -= vec[i];
    }
  }
  pr.set_value(reduction_elem);
}
}  // namespace

bool nesterov_a_test_task_stl::TestSTLTaskParallel::PreProcessingImpl() {
  // Init vectors
  input_ = std::vector<int>(task_data->inputs_count[0]);
  auto *tmp_ptr = reinterpret_cast<int *>(task_data->inputs[0]);
  for (unsigned i = 0; i < task_data->inputs_count[0]; i++) {
    input_[i] = tmp_ptr[i];
  }
  // Init value for output
  res_ = 0;
  return true;
}

bool nesterov_a_test_task_stl::TestSTLTaskParallel::ValidationImpl() {
  // Check count elements of output
  return task_data->outputs_count[0] == 1;
}

bool nesterov_a_test_task_stl::TestSTLTaskParallel::RunImpl() {
  const auto nthreads = std::thread::hardware_concurrency();
  const auto delta = (input_.end() - input_.begin()) / nthreads;

  auto *promises = new std::promise<int>[nthreads];
  auto *futures = new std::future<int>[nthreads];
  auto *threads = new std::thread[nthreads];

  for (unsigned i = 0; i < nthreads; i++) {
    futures[i] = promises[i].get_future();
    std::vector<int> tmp_vec(input_.begin() + i * delta, input_.begin() + (i + 1) * delta);
    threads[i] = std::thread(AtomOps, tmp_vec, ops_, std::move(promises[i]));
    threads[i].join();
    res_ += futures[i].get();
  }

  delete[] promises;
  delete[] futures;
  delete[] threads;
  return true;
}

bool nesterov_a_test_task_stl::TestSTLTaskParallel::PostProcessingImpl() {
  reinterpret_cast<int *>(task_data->outputs[0])[0] = res_;
  return true;
}
