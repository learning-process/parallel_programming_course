#include <gtest/gtest.h>

#include <boost/mpi/timer.hpp>
#include <vector>

#include "core/perf/include/perf.hpp"
#include "mpi/example/include/ops_mpi.hpp"

TEST(mpi_example_perf_test, test_pipeline_run) {
  boost::mpi::communicator world;
  std::vector<int> global_vec;
  std::vector<int32_t> global_sum(1, 0);
  // Create TaskData
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  int count_size_vector;
  if (world.rank() == 0) {
    count_size_vector = 120;
    global_vec = std::vector<int>(count_size_vector, 1);
    task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t*>(global_vec.data()));
    task_data_par->inputs_count.emplace_back(global_vec.size());
    task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t*>(global_sum.data()));
    task_data_par->outputs_count.emplace_back(global_sum.size());
  }

  auto test_mpi_task_parallel = std::make_shared<nesterov_a_test_task_mpi::TestMPITaskParallel>(task_data_par, "+");
  ASSERT_EQ(test_mpi_task_parallel->validation(), true);
  test_mpi_task_parallel->pre_processing();
  test_mpi_task_parallel->run();
  test_mpi_task_parallel->post_processing();

  // Create Perf attributes
  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();
  perf_attr->num_running = 10;
  const boost::mpi::timer current_timer;
  perf_attr->current_timer = [&] { return current_timer.elapsed(); };

  // Create and init perf results
  auto perf_results = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perf_analyzer = std::make_shared<ppc::core::Perf>(test_mpi_task_parallel);
  perf_analyzer->pipeline_run(perf_attr, perf_results);
  if (world.rank() == 0) {
    ppc::core::Perf::print_perf_statistic(perf_results);
    ASSERT_EQ(count_size_vector, global_sum[0]);
  }
}

TEST(mpi_example_perf_test, test_task_run) {
  boost::mpi::communicator world;
  std::vector<int> global_vec;
  std::vector<int32_t> global_sum(1, 0);
  // Create TaskData
  auto task_data_par = std::make_shared<ppc::core::TaskData>();
  int count_size_vector;
  if (world.rank() == 0) {
    count_size_vector = 120;
    global_vec = std::vector<int>(count_size_vector, 1);
    task_data_par->inputs.emplace_back(reinterpret_cast<uint8_t*>(global_vec.data()));
    task_data_par->inputs_count.emplace_back(global_vec.size());
    task_data_par->outputs.emplace_back(reinterpret_cast<uint8_t*>(global_sum.data()));
    task_data_par->outputs_count.emplace_back(global_sum.size());
  }

  auto test_mpi_task_parallel = std::make_shared<nesterov_a_test_task_mpi::TestMPITaskParallel>(task_data_par, "+");
  ASSERT_EQ(test_mpi_task_parallel->validation(), true);
  test_mpi_task_parallel->pre_processing();
  test_mpi_task_parallel->run();
  test_mpi_task_parallel->post_processing();

  // Create Perf attributes
  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();
  perf_attr->num_running = 10;
  const boost::mpi::timer current_timer;
  perf_attr->current_timer = [&] { return current_timer.elapsed(); };

  // Create and init perf results
  auto perf_results = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perf_analyzer = std::make_shared<ppc::core::Perf>(test_mpi_task_parallel);
  perf_analyzer->task_run(perf_attr, perf_results);
  if (world.rank() == 0) {
    ppc::core::Perf::print_perf_statistic(perf_results);
    ASSERT_EQ(count_size_vector, global_sum[0]);
  }
}
