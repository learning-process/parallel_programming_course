#include "mpi/example/include/ops_mpi.hpp"

#include <algorithm>
#include <functional>
#include <random>
#include <string>
#include <thread>
#include <vector>

bool nesterov_a_test_task_mpi::TestMPITaskSequential::PreProcessingImpl() {
  // Init vectors
  input_ = std::vector<int>(task_data->inputs_count[0]);
  auto* tmp_ptr = reinterpret_cast<int*>(task_data->inputs[0]);
  for (unsigned i = 0; i < task_data->inputs_count[0]; i++) {
    input_[i] = tmp_ptr[i];
  }
  // Init value for output
  res_ = 0;
  return true;
}

bool nesterov_a_test_task_mpi::TestMPITaskSequential::ValidationImpl() {
  // Check count elements of output
  return task_data->outputs_count[0] == 1;
}

bool nesterov_a_test_task_mpi::TestMPITaskSequential::RunImpl() {
  if (ops_ == "+") {
    res_ = std::accumulate(input_.begin(), input_.end(), 0);
  } else if (ops_ == "-") {
    res_ = -std::accumulate(input_.begin(), input_.end(), 0);
  } else if (ops_ == "max") {
    res_ = *std::ranges::max_element(input_);
  }
  return true;
}

bool nesterov_a_test_task_mpi::TestMPITaskSequential::PostProcessingImpl() {
  reinterpret_cast<int*>(task_data->outputs[0])[0] = res_;
  return true;
}

bool nesterov_a_test_task_mpi::TestMPITaskParallel::PreProcessingImpl() {
  unsigned int delta = 0;
  if (world_.rank() == 0) {
    delta = task_data->inputs_count[0] / world_.size();
  }
  broadcast(world_, delta, 0);

  if (world_.rank() == 0) {
    // Init vectors
    input_ = std::vector<int>(task_data->inputs_count[0]);
    auto* tmp_ptr = reinterpret_cast<int*>(task_data->inputs[0]);
    for (unsigned i = 0; i < task_data->inputs_count[0]; i++) {
      input_[i] = tmp_ptr[i];
    }
    for (int proc = 1; proc < world_.size(); proc++) {
      world_.send(proc, 0, input_.data() + (proc * delta), delta);
    }
  }
  local_input_ = std::vector<int>(delta);
  if (world_.rank() == 0) {
    local_input_ = std::vector<int>(input_.begin(), input_.begin() + delta);
  } else {
    world_.recv(0, 0, local_input_.data(), delta);
  }
  // Init value for output
  res_ = 0;
  return true;
}

bool nesterov_a_test_task_mpi::TestMPITaskParallel::ValidationImpl() {
  if (world_.rank() == 0) {
    // Check count elements of output
    return task_data->outputs_count[0] == 1;
  }
  return true;
}

bool nesterov_a_test_task_mpi::TestMPITaskParallel::RunImpl() {
  int local_res;
  if (ops_ == "+") {
    local_res = std::accumulate(local_input_.begin(), local_input_.end(), 0);
  } else if (ops_ == "-") {
    local_res = -std::accumulate(local_input_.begin(), local_input_.end(), 0);
  } else if (ops_ == "max") {
    local_res = *std::ranges::max_element(local_input_);
  }

  if (ops_ == "+" || ops_ == "-") {
    reduce(world_, local_res, res_, std::plus(), 0);
  } else if (ops_ == "max") {
    reduce(world_, local_res, res_, boost::mpi::maximum<int>(), 0);
  }
  return true;
}

bool nesterov_a_test_task_mpi::TestMPITaskParallel::PostProcessingImpl() {
  if (world_.rank() == 0) {
    reinterpret_cast<int*>(task_data->outputs[0])[0] = res_;
  }
  return true;
}
