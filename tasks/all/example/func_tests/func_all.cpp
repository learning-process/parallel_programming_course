#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stb_library.hpp>
#include <string>
#include <vector>

#include "all/example/include/ops_all.hpp"
#include "core/task/include/task.hpp"
#include "core/util/include/util.hpp"

TEST(nesterov_a_test_task_all, test_matmul_50) {
  constexpr size_t kCount = 50;

  // Create data
  std::vector<int> in(kCount * kCount, 0);

  for (size_t i = 0; i < kCount; i++) {
    in[(i * kCount) + i] = 1;
  }

  // Create Task
  nesterov_a_test_task_all::TestTaskALL test_task_all(in);
  ASSERT_EQ(test_task_all.Validation(), true);
  test_task_all.PreProcessing();
  test_task_all.Run();
  test_task_all.PostProcessing();
  EXPECT_EQ(in, test_task_all.Get());
}

TEST(nesterov_a_test_task_all, test_matmul_from_pic) {
  int width = -1;
  int height = -1;
  int channels = -1;
  std::string abs_path = ppc::util::GetAbsolutePath("all/example/data/pic_all.jpg");
  unsigned char *data = stbi_load(abs_path.c_str(), &width, &height, &channels, 0);
  EXPECT_TRUE(data != nullptr) << "Failed to load image: " << stbi_failure_reason() << '\n';
  auto img = std::vector<uint8_t>(data, data + (width * height * channels));
  stbi_image_free(data);

  EXPECT_EQ(width, height);
  const int count = (width + height) / 10;

  // Create data
  std::vector<int> in(count * count, 0);

  for (int i = 0; i < count; i++) {
    in[(i * count) + i] = 1;
  }

  // Create Task
  nesterov_a_test_task_all::TestTaskALL test_task_all(in);
  ASSERT_EQ(test_task_all.Validation(), true);
  test_task_all.PreProcessing();
  test_task_all.Run();
  test_task_all.PostProcessing();
  EXPECT_EQ(in, test_task_all.Get());
}
