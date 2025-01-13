#include "omp/example/include/ops_omp.hpp"

#include <omp.h>

#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

bool nesterov_a_test_task_omp::TestOMPTaskSequential::PreProcessingImpl() {
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

bool nesterov_a_test_task_omp::TestOMPTaskSequential::ValidationImpl() {
  // Check count elements of output
  return task_data->outputs_count[0] == 1;
}

bool nesterov_a_test_task_omp::TestOMPTaskSequential::RunImpl() {
  if (ops_ == "+") {
    res_ = std::accumulate(input_.begin(), input_.end(), 1);
  } else if (ops_ == "-") {
    res_ -= std::accumulate(input_.begin(), input_.end(), 0);
  } else if (ops_ == "*") {
    res_ = std::accumulate(input_.begin(), input_.end(), 1, std::multiplies<>());
  }
  return true;
}

bool nesterov_a_test_task_omp::TestOMPTaskSequential::PostProcessingImpl() {
  reinterpret_cast<int*>(task_data->outputs[0])[0] = res_;
  return true;
}

bool nesterov_a_test_task_omp::TestOMPTaskParallel::PreProcessingImpl() {
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

bool nesterov_a_test_task_omp::TestOMPTaskParallel::ValidationImpl() {
  // Check count elements of output
  return task_data->outputs_count[0] == 1;
}

bool nesterov_a_test_task_omp::TestOMPTaskParallel::RunImpl() {
  double start = omp_get_wtime();
  auto temp_res = res_;
  if (ops_ == "+") {
#pragma omp parallel for reduction(+ : temp_res)
    for (int i = 0; i < static_cast<int>(input_.size()); i++) {
      temp_res += input_[i];
    }
  } else if (ops_ == "-") {
#pragma omp parallel for reduction(- : temp_res)
    for (int i = 0; i < static_cast<int>(input_.size()); i++) {
      temp_res -= input_[i];
    }
  } else if (ops_ == "*") {
#pragma omp parallel for reduction(* : temp_res)
    for (int i = 0; i < static_cast<int>(input_.size()); i++) {
      temp_res *= input_[i];
    }
  }
  res_ = temp_res;
  double finish = omp_get_wtime();
  std::cout << "How measure time in OpenMP: " << finish - start << '\n';
  return true;
}

bool nesterov_a_test_task_omp::TestOMPTaskParallel::PostProcessingImpl() {
  reinterpret_cast<int*>(task_data->outputs[0])[0] = res_;
  return true;
}
