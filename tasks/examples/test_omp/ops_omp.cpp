// Copyright 2023 Nesterov Alexander
#include "examples/test_omp/ops_omp.hpp"

#include <omp.h>

#include <iostream>
#include <random>
#include <string>
#include <vector>

std::vector<int> getRandomVector(int sz) {
  std::random_device dev;
  std::mt19937 gen(dev());
  std::vector<int> vec(sz);
  for (int i = 0; i < sz; i++) {
    vec[i] = gen() % 100 + 1;
  }
  return vec;
}

bool TestOMPTaskSequential::pre_processing() {
  internal_order_test();
  // Init vectors
  input_ = std::vector<int>(taskData->inputs_count[0]);
  auto tmp_ptr = reinterpret_cast<int*>(taskData->inputs[0]);
  for (int i = 0; i < taskData->inputs_count[0]; i++) {
    input_[i] = tmp_ptr[i];
  }
  // Init value for output
  res = 1;
  return true;
}

bool TestOMPTaskSequential::validation() {
  internal_order_test();
  // Check count elements of output
  if (taskData->outputs_count[0] == 1) {
    return true;
  } else {
    return false;
  }
}

bool TestOMPTaskSequential::run() {
  internal_order_test();
  if (ops == "+") {
    res = std::accumulate(input_.begin(), input_.end(), 1);
  } else if (ops == "-") {
    res -= std::accumulate(input_.begin(), input_.end(), 0);
  } else if (ops == "*") {
    res = std::accumulate(input_.begin(), input_.end(), 1, std::multiplies<>());
  }
  return true;
}

bool TestOMPTaskSequential::post_processing() {
  internal_order_test();
  reinterpret_cast<int*>(taskData->outputs[0])[0] = res;
  return true;
}

bool TestOMPTaskParallel::pre_processing() {
  internal_order_test();
  // Init vectors
  input_ = std::vector<int>(taskData->inputs_count[0]);
  auto tmp_ptr = reinterpret_cast<int*>(taskData->inputs[0]);
  for (int i = 0; i < taskData->inputs_count[0]; i++) {
    input_[i] = tmp_ptr[i];
  }
  // Init value for output
  res = 1;
  return true;
}

bool TestOMPTaskParallel::validation() {
  internal_order_test();
  // Check count elements of output
  if (taskData->outputs_count[0] == 1) {
    return true;
  } else {
    return false;
  }
}

bool TestOMPTaskParallel::run() {
  internal_order_test();
  double start = omp_get_wtime();
  if (ops == "+") {
#pragma omp parallel for reduction(+ : res)
    for (int i = 0; i < input_.size(); i++) {
      res += input_[i];
    }
  } else if (ops == "-") {
#pragma omp parallel for reduction(- : res)
    for (int i = 0; i < input_.size(); i++) {
      res -= input_[i];
    }
  } else if (ops == "*") {
#pragma omp parallel for reduction(* : res)
    for (int i = 0; i < input_.size(); i++) {
      res *= input_[i];
    }
  }
  double finish = omp_get_wtime();
  std::cout << "How measure time in OpenMP: " << finish - start << std::endl;
  return true;
}

bool TestOMPTaskParallel::post_processing() {
  internal_order_test();
  reinterpret_cast<int*>(taskData->outputs[0])[0] = res;
  return true;
}
