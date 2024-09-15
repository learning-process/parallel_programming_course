// Copyright 2023 Nesterov Alexander
#include "omp/example/include/ops_omp.hpp"

#include <omp.h>

#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

std::vector<int> nesterov_a_test_task_omp::getRandomVector(int sz) {
  std::random_device dev;
  std::mt19937 gen(dev());
  std::vector<int> vec(sz);
  for (int i = 0; i < sz; i++) {
    vec[i] = gen() % 100 + 1;
  }
  return vec;
}

bool nesterov_a_test_task_omp::TestOMPTaskSequential::pre_processing() {
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

bool nesterov_a_test_task_omp::TestOMPTaskSequential::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->outputs_count[0] == 1;
}

bool nesterov_a_test_task_omp::TestOMPTaskSequential::run() {
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

bool nesterov_a_test_task_omp::TestOMPTaskSequential::post_processing() {
  internal_order_test();
  reinterpret_cast<int*>(taskData->outputs[0])[0] = res;
  return true;
}

bool nesterov_a_test_task_omp::TestOMPTaskParallel::pre_processing() {
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

bool nesterov_a_test_task_omp::TestOMPTaskParallel::validation() {
  internal_order_test();
  // Check count elements of output
  return taskData->outputs_count[0] == 1;
}

bool nesterov_a_test_task_omp::TestOMPTaskParallel::run() {
  internal_order_test();
  double start = omp_get_wtime();
  auto temp_res = res;
  if (ops == "+") {
#pragma omp parallel for reduction(+ : temp_res)
    for (int i = 0; i < static_cast<int>(input_.size()); i++) {
      temp_res += input_[i];
    }
  } else if (ops == "-") {
#pragma omp parallel for reduction(- : temp_res)
    for (int i = 0; i < static_cast<int>(input_.size()); i++) {
      temp_res -= input_[i];
    }
  } else if (ops == "*") {
#pragma omp parallel for reduction(* : temp_res)
    for (int i = 0; i < static_cast<int>(input_.size()); i++) {
      temp_res *= input_[i];
    }
  }
  res = temp_res;
  double finish = omp_get_wtime();
  std::cout << "How measure time in OpenMP: " << finish - start << std::endl;
  return true;
}

bool nesterov_a_test_task_omp::TestOMPTaskParallel::post_processing() {
  internal_order_test();
  reinterpret_cast<int*>(taskData->outputs[0])[0] = res;
  return true;
}
