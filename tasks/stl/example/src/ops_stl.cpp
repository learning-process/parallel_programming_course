// Copyright 2023 Nesterov Alexander
#include "stl/example/include/ops_stl.hpp"

#include <future>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <thread>
#include <utility>
#include <vector>

using namespace std::chrono_literals;

std::vector<int> nesterov_a_test_task_stl::getRandomVector(int sz) {
  std::random_device dev;
  std::mt19937 gen(dev());
  std::vector<int> vec(sz);
  for (int i = 0; i < sz; i++) {
    vec[i] = static_cast<int>(gen()) % 100;
  }
  return vec;
}

bool nesterov_a_test_task_stl::TestSTLTaskSequential::pre_processing() {
  internal_order_test();
  // Init vectors
  input_ = std::vector<int>(taskData->inputs_count[0]);
  auto *tmp_ptr = reinterpret_cast<int *>(taskData->inputs[0]);
  for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
    input_[i] = tmp_ptr[i];
  }
  // Init value for output
  res = 0;
  return true;
}

bool nesterov_a_test_task_stl::TestSTLTaskSequential::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->outputs_count[0] == 1;
}

bool nesterov_a_test_task_stl::TestSTLTaskSequential::run() {
  internal_order_test();
  if (ops == "+") {
    res = std::accumulate(input_.begin(), input_.end(), 0);
  } else if (ops == "-") {
    res -= std::accumulate(input_.begin(), input_.end(), 0);
  }
  std::this_thread::sleep_for(20ms);
  return true;
}

bool nesterov_a_test_task_stl::TestSTLTaskSequential::post_processing() {
  internal_order_test();
  reinterpret_cast<int *>(taskData->outputs[0])[0] = res;
  return true;
}

std::mutex my_mutex;

void atomOps(std::vector<int> vec, const std::string &ops, std::promise<int> &&pr) {
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

bool nesterov_a_test_task_stl::TestSTLTaskParallel::pre_processing() {
  internal_order_test();
  // Init vectors
  input_ = std::vector<int>(taskData->inputs_count[0]);
  auto *tmp_ptr = reinterpret_cast<int *>(taskData->inputs[0]);
  for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
    input_[i] = tmp_ptr[i];
  }
  // Init value for output
  res = 0;
  return true;
}

bool nesterov_a_test_task_stl::TestSTLTaskParallel::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->outputs_count[0] == 1;
}

bool nesterov_a_test_task_stl::TestSTLTaskParallel::run() {
  internal_order_test();
  const auto nthreads = std::thread::hardware_concurrency();
  const auto delta = (input_.end() - input_.begin()) / nthreads;

  auto *promises = new std::promise<int>[nthreads];
  auto *futures = new std::future<int>[nthreads];
  auto *threads = new std::thread[nthreads];

  for (unsigned i = 0; i < nthreads; i++) {
    futures[i] = promises[i].get_future();
    std::vector<int> tmp_vec(input_.begin() + i * delta, input_.begin() + (i + 1) * delta);
    threads[i] = std::thread(atomOps, tmp_vec, ops, std::move(promises[i]));
    threads[i].join();
    res += futures[i].get();
  }

  delete[] promises;
  delete[] futures;
  delete[] threads;
  return true;
}

bool nesterov_a_test_task_stl::TestSTLTaskParallel::post_processing() {
  internal_order_test();
  reinterpret_cast<int *>(taskData->outputs[0])[0] = res;
  return true;
}
