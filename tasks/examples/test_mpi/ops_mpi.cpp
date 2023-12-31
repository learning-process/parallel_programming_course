// Copyright 2023 Nesterov Alexander
#include "examples/test_mpi/ops_mpi.h"

#include <algorithm>
#include <functional>
#include <random>
#include <string>
#include <vector>

std::vector<int> getRandomVector(int sz) {
  std::random_device dev;
  std::mt19937 gen(dev());
  std::vector<int> vec(sz);
  for (int i = 0; i < sz; i++) {
    vec[i] = gen() % 100;
  }
  return vec;
}

int getSequentialOperations(std::vector<int> vec, const std::string& ops) {
  const int sz = vec.size();
  int reduction_elem = 0;
  if (ops == "+") {
    for (int i = 0; i < sz; i++) {
      reduction_elem += vec[i];
    }
  } else if (ops == "-") {
    for (int i = 0; i < sz; i++) {
      reduction_elem -= vec[i];
    }
  } else if (ops == "max") {
    reduction_elem = vec[0];
    for (int i = 1; i < sz; i++) {
      reduction_elem = std::max(reduction_elem, vec[i]);
    }
  }
  return reduction_elem;
}

int getParallelOperations(std::vector<int> global_vec, int count_size_vector,
                          const std::string& ops) {
  boost::mpi::communicator world;
  const int delta = count_size_vector / world.size();

  if (world.rank() == 0) {
    for (int proc = 1; proc < world.size(); proc++) {
      world.send(proc, 0, global_vec.data() + proc * delta, delta);
    }
  }

  std::vector<int> local_vec(delta);
  if (world.rank() == 0) {
    local_vec =
        std::vector<int>(global_vec.begin(), global_vec.begin() + delta);
  } else {
    world.recv(0, 0, local_vec.data(), delta);
  }

  int global_sum = 0;
  int local_sum = getSequentialOperations(local_vec, ops);
  if (ops == "+" || ops == "-") {
    reduce(world, local_sum, global_sum, std::plus<int>(), 0);
  }
  if (ops == "max") {
    reduce(world, local_sum, global_sum, boost::mpi::maximum<int>(), 0);
  }
  return global_sum;
}

bool TestMPITaskSequential::pre_processing() {
  internal_order_test();
  // Init vectors
  input_ = std::vector<int>(taskData->inputs_count[0]);
  auto tmp_ptr = reinterpret_cast<int*>(taskData->inputs[0]);
  for (int i = 0; i < taskData->inputs_count[0]; i++) {
    input_[i] = tmp_ptr[i];
  }
  // Init value for output
  res = 0;
  return true;
}

bool TestMPITaskSequential::validation() {
  internal_order_test();
  // Check count elements of output
  if (taskData->outputs_count[0] == 1) {
    return true;
  } else {
    return false;
  }
}

bool TestMPITaskSequential::run() {
  internal_order_test();
  if (ops == "+") {
    res = std::accumulate(input_.begin(), input_.end(), 0);
  } else if (ops == "-") {
    res = - std::accumulate(input_.begin(), input_.end(), 0);
  } else if (ops == "max") {
    res = *std::max_element(input_.begin(), input_.end());
  }
  return true;
}

bool TestMPITaskSequential::post_processing() {
  internal_order_test();
  reinterpret_cast<int*>(taskData->outputs[0])[0] = res;
  return true;
}

bool TestMPITaskParallel::pre_processing() {
  internal_order_test();
  unsigned int delta = 0;
  if (world.rank() == 0) {
    delta = taskData->inputs_count[0] / world.size();
  }
  broadcast(world, delta, 0);

  if (world.rank() == 0) {
    // Init vectors
    input_ = std::vector<int>(taskData->inputs_count[0]);
    auto tmp_ptr = reinterpret_cast<int*>(taskData->inputs[0]);
    for (int i = 0; i < taskData->inputs_count[0]; i++) {
      input_[i] = tmp_ptr[i];
    }
    for (int proc = 1; proc < world.size(); proc++) {
      world.send(proc, 0, input_.data() + proc * delta, delta);
    }
  }
  local_input_ = std::vector<int>(delta);
  if (world.rank() == 0) {
    local_input_ = std::vector<int>(input_.begin(), input_.begin() + delta);
  } else {
    world.recv(0, 0, local_input_.data(), delta);
  }
  // Init value for output
  res = 0;
  return true;
}

bool TestMPITaskParallel::validation() {
  internal_order_test();
  if (world.rank() == 0) {
    // Check count elements of output
    if (taskData->outputs_count[0] == 1) {
      return true;
    } else {
      return false;
    }
  }
  return true;
}

bool TestMPITaskParallel::run() {
  internal_order_test();
  int local_res;
  if (ops == "+") {
    local_res = std::accumulate(local_input_.begin(), local_input_.end(), 0);
  } else if (ops == "-") {
    local_res = - std::accumulate(local_input_.begin(), local_input_.end(), 0);
  } else if (ops == "max") {
    local_res = *std::max_element(local_input_.begin(), local_input_.end());
  }

  if (ops == "+" || ops == "-") {
    reduce(world, local_res, res, std::plus(), 0);
  } else if (ops == "max") {
    reduce(world, local_res, res, boost::mpi::maximum<int>(), 0);
  }
  return true;
}

bool TestMPITaskParallel::post_processing() {
  internal_order_test();
  if (world.rank() == 0) {
    reinterpret_cast<int*>(taskData->outputs[0])[0] = res;
  }
  return true;
}
