// Copyright 2023 Nesterov Alexander
#include "tbb/example/include/ops_tbb.hpp"

#include <tbb/tbb.h>

#include <functional>
#include <numeric>
#include <random>
#include <string>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

std::vector<int> nesterov_a_test_task_tbb::getRandomVector(int sz) {
  std::random_device dev;
  std::mt19937 gen(dev());
  std::vector<int> vec(sz);
  for (int i = 0; i < sz; i++) {
    vec[i] = gen() % 20 + 1;
  }
  return vec;
}

bool nesterov_a_test_task_tbb::TestTBBTaskSequential::pre_processing() {
  internal_order_test();
  // Init vectors
  input_ = std::vector<int>(taskData->inputs_count[0]);
  auto* tmp_ptr = reinterpret_cast<int*>(taskData->inputs[0]);
  for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
    input_[i] = tmp_ptr[i];
  }
  // Init value for output
  res = 1;
  return true;
}

bool nesterov_a_test_task_tbb::TestTBBTaskSequential::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->outputs_count[0] == 1;
}

bool nesterov_a_test_task_tbb::TestTBBTaskSequential::run() {
  internal_order_test();
  if (ops == "+") {
    res = std::accumulate(input_.begin(), input_.end(), 1);
  } else if (ops == "-") {
    res -= std::accumulate(input_.begin(), input_.end(), 0);
  } else if (ops == "*") {
    res = std::accumulate(input_.begin(), input_.end(), 1, std::multiplies<>());
  }
  std::this_thread::sleep_for(20ms);
  return true;
}

bool nesterov_a_test_task_tbb::TestTBBTaskSequential::post_processing() {
  internal_order_test();
  reinterpret_cast<int*>(taskData->outputs[0])[0] = res;
  return true;
}

bool nesterov_a_test_task_tbb::TestTBBTaskParallel::pre_processing() {
  internal_order_test();
  // Init vectors
  input_ = std::vector<int>(taskData->inputs_count[0]);
  auto* tmp_ptr = reinterpret_cast<int*>(taskData->inputs[0]);
  for (unsigned i = 0; i < taskData->inputs_count[0]; i++) {
    input_[i] = tmp_ptr[i];
  }
  // Init value for output
  res = 1;
  return true;
}

bool nesterov_a_test_task_tbb::TestTBBTaskParallel::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->outputs_count[0] == 1;
}

bool nesterov_a_test_task_tbb::TestTBBTaskParallel::run() {
  internal_order_test();
  if (ops == "+") {
    res += oneapi::tbb::parallel_reduce(
        oneapi::tbb::blocked_range<std::vector<int>::iterator>(input_.begin(), input_.end()), 0,
        [](tbb::blocked_range<std::vector<int>::iterator> r, int running_total) {
          running_total += std::accumulate(r.begin(), r.end(), 0);
          return running_total;
        },
        std::plus<>());
  } else if (ops == "-") {
    res -= oneapi::tbb::parallel_reduce(
        oneapi::tbb::blocked_range<std::vector<int>::iterator>(input_.begin(), input_.end()), 0,
        [](tbb::blocked_range<std::vector<int>::iterator> r, int running_total) {
          running_total += std::accumulate(r.begin(), r.end(), 0);
          return running_total;
        },
        std::plus<>());
  } else if (ops == "*") {
    res *= oneapi::tbb::parallel_reduce(
        oneapi::tbb::blocked_range<std::vector<int>::iterator>(input_.begin(), input_.end()), 1,
        [](tbb::blocked_range<std::vector<int>::iterator> r, int running_total) {
          running_total *= std::accumulate(r.begin(), r.end(), 1, std::multiplies<>());
          return running_total;
        },
        std::multiplies<>());
  }
  return true;
}

bool nesterov_a_test_task_tbb::TestTBBTaskParallel::post_processing() {
  internal_order_test();
  reinterpret_cast<int*>(taskData->outputs[0])[0] = res;
  return true;
}
