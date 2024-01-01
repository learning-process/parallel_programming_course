// Copyright 2023 Nesterov Alexander
#include "examples/test_std/ops_std.hpp"

#include <iostream>
#include <random>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "unapproved/unapproved.h"

std::vector<int> getRandomVector(int sz) {
  std::random_device dev;
  std::mt19937 gen(dev());
  std::vector<int> vec(sz);
  for (int i = 0; i < sz; i++) {
    vec[i] = gen() % 100;
  }
  return vec;
}

bool TestSTDTaskSequential::pre_processing() {
  internal_order_test();
  // Init vectors
  input_ = std::vector<int>(taskData->inputs_count[0]);
  auto tmp_ptr = reinterpret_cast<int *>(taskData->inputs[0]);
  for (int i = 0; i < taskData->inputs_count[0]; i++) {
    input_[i] = tmp_ptr[i];
  }
  // Init value for output
  res = 0;
  return true;
}

bool TestSTDTaskSequential::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->outputs_count[0] == 1;
}

bool TestSTDTaskSequential::run() {
  internal_order_test();
  if (ops == "+") {
    res = std::accumulate(input_.begin(), input_.end(), 0);
  } else if (ops == "-") {
    res -= std::accumulate(input_.begin(), input_.end(), 0);
  }
  return true;
}

bool TestSTDTaskSequential::post_processing() {
  internal_order_test();
  reinterpret_cast<int *>(taskData->outputs[0])[0] = res;
  return true;
}

std::mutex my_mutex;

void atomOps(std::vector<int> vec, const std::string &ops, std::promise<int> &&pr) {
  auto sz = vec.size();
  int reduction_elem = 0;
  if (ops == "+") {
    for (int i = 0; i < sz; i++) {
      std::lock_guard<std::mutex> my_lock(my_mutex);
      reduction_elem += vec[i];
    }
  } else if (ops == "-") {
    for (int i = 0; i < sz; i++) {
      std::lock_guard<std::mutex> my_lock(my_mutex);
      reduction_elem -= vec[i];
    }
  }
  pr.set_value(reduction_elem);
}

bool TestSTDTaskParallel::pre_processing() {
  internal_order_test();
  // Init vectors
  input_ = std::vector<int>(taskData->inputs_count[0]);
  auto *tmp_ptr = reinterpret_cast<int *>(taskData->inputs[0]);
  for (int i = 0; i < taskData->inputs_count[0]; i++) {
    input_[i] = tmp_ptr[i];
  }
  // Init value for output
  res = 0;
  return true;
}

bool TestSTDTaskParallel::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->outputs_count[0] == 1;
}

bool TestSTDTaskParallel::run() {
  internal_order_test();
  const auto nthreads = std::thread::hardware_concurrency();
  const auto delta = (input_.end() - input_.begin()) / nthreads;

  auto *promises = new std::promise<int>[nthreads];
  auto *futures = new std::future<int>[nthreads];
  auto *threads = new std::thread[nthreads];

  for (int i = 0; i < nthreads; i++) {
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

bool TestSTDTaskParallel::post_processing() {
  internal_order_test();
  reinterpret_cast<int *>(taskData->outputs[0])[0] = res;
  return true;
}
