#pragma once

#include <gtest/gtest.h>

#include <boost/mpi/collectives.hpp>
#include <boost/mpi/communicator.hpp>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "core/task/include/task.hpp"

namespace nesterov_a_test_task_mpi {

std::vector<int> getRandomVector(int sz);

class TestMPITaskSequential : public ppc::core::Task {
 public:
  explicit TestMPITaskSequential(ppc::core::TaskDataPtr taskData_, std::string ops_)
      : Task(std::move(taskData_)), ops(std::move(ops_)) {}
  bool pre_processing_impl() override;
  bool validation_impl() override;
  bool run_impl() override;
  bool post_processing_impl() override;

 private:
  std::vector<int> input_;
  int res{};
  std::string ops;
};

class TestMPITaskParallel : public ppc::core::Task {
 public:
  explicit TestMPITaskParallel(ppc::core::TaskDataPtr taskData_, std::string ops_)
      : Task(std::move(taskData_)), ops(std::move(ops_)) {}
  bool pre_processing_impl() override;
  bool validation_impl() override;
  bool run_impl() override;
  bool post_processing_impl() override;

 private:
  std::vector<int> input_, local_input_;
  int res{};
  std::string ops;
  boost::mpi::communicator world;
};

}  // namespace nesterov_a_test_task_mpi