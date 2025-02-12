#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <opencv2/opencv.hpp>
#include <vector>

#include "all/example/include/ops_all.hpp"
#include "core/task/include/task.hpp"
#include "core/util/include/util.hpp"

TEST(nesterov_a_test_task_all, test_matmul_50) {
  constexpr size_t kCount = 50;

  // Create data
  std::vector<int> in(kCount * kCount, 0);
  std::vector<int> out(kCount * kCount, 0);

  for (size_t i = 0; i < kCount; i++) {
    in[(i * kCount) + i] = 1;
  }

  // Create task_data
  auto task_data_all = std::make_shared<ppc::core::TaskData>();
  task_data_all->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  task_data_all->inputs_count.emplace_back(in.size());
  task_data_all->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  task_data_all->outputs_count.emplace_back(out.size());

  // Create Task
  nesterov_a_test_task_all::TestTaskALL test_task_all(task_data_all);
  ASSERT_EQ(test_task_all.Validation(), true);
  test_task_all.PreProcessing();
  test_task_all.Run();
  test_task_all.PostProcessing();
  EXPECT_EQ(in, out);
}

TEST(nesterov_a_test_task_all, test_matmul_from_pic) {
  cv::Mat img = cv::imread(ppc::util::GetAbsolutePath("all/example/data/pic_all.jpg"));
  EXPECT_EQ(img.rows, img.cols);
  const int count = (img.rows + img.cols) / 10;

  // Create data
  std::vector<int> in(count * count, 0);
  std::vector<int> out(count * count, 0);

  for (int i = 0; i < count; i++) {
    in[(i * count) + i] = 1;
  }

  // Create task_data
  auto task_data_all = std::make_shared<ppc::core::TaskData>();
  task_data_all->inputs.emplace_back(reinterpret_cast<uint8_t *>(in.data()));
  task_data_all->inputs_count.emplace_back(in.size());
  task_data_all->outputs.emplace_back(reinterpret_cast<uint8_t *>(out.data()));
  task_data_all->outputs_count.emplace_back(out.size());

  // Create Task
  nesterov_a_test_task_all::TestTaskALL test_task_all(task_data_all);
  ASSERT_EQ(test_task_all.Validation(), true);
  test_task_all.PreProcessing();
  test_task_all.Run();
  test_task_all.PostProcessing();
  EXPECT_EQ(in, out);
}
