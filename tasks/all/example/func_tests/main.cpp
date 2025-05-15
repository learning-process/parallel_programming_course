#include <gtest/gtest.h>

#include <cstdint>
#include <stb_library.hpp>
#include <string>
#include <vector>

#include "all/example/include/ops_all.hpp"
#include "core/util/include/util.hpp"

class NesterovATestTaskAll : public ::testing::TestWithParam<int> {
 protected:
  void SetUp() override {
    width = height = channels = -1;
    std::string abs_path = ppc::util::GetAbsolutePath("all/example/data/pic_all.jpg");
    data = stbi_load(abs_path.c_str(), &width, &height, &channels, 0);
    ASSERT_TRUE(data != nullptr) << "Failed to load image: " << stbi_failure_reason();
    img = std::vector<uint8_t>(data, data + (width * height * channels));
    stbi_image_free(data);

    ASSERT_EQ(width, height);
  }

  int width = -1, height = -1, channels = -1;
  unsigned char* data = nullptr;
  std::vector<uint8_t> img;
};

TEST_P(NesterovATestTaskAll, MatmulFromPic) {
  int divider = GetParam();
  const int k_count = (width + height) / divider;

  std::vector<int> in(k_count * k_count, 0);
  for (int i = 0; i < k_count; i++) {
    in[(i * k_count) + i] = 1;
  }

  nesterov_a_test_task_all::TestTaskALL test_task_all(in);
  ASSERT_TRUE(test_task_all.Validation());
  test_task_all.PreProcessing();
  test_task_all.Run();
  test_task_all.PostProcessing();
  EXPECT_EQ(in, test_task_all.Get());
}

TEST_P(NesterovATestTaskAll, MatMulUtilFromPic) {
  int divider = GetParam();
  const int k_count = (width + height) / divider;

  std::vector<int> in(k_count * k_count, 0);
  for (int i = 0; i < k_count; i++) {
    in[(i * k_count) + i] = 1;
  }
  std::vector<int> out(k_count * k_count, 0);
  nesterov_a_test_task_all::MatMul(in, static_cast<int>(k_count), out);

  EXPECT_EQ(in, out);
}

TEST_P(NesterovATestTaskAll, MatMulTBBUtilFromPic) {
  int divider = GetParam();
  const int k_count = (width + height) / divider;

  std::vector<int> in(k_count * k_count, 0);
  for (int i = 0; i < k_count; i++) {
    in[(i * k_count) + i] = 1;
  }
  std::vector<int> out(k_count * k_count, 0);
  nesterov_a_test_task_all::MatMulTBB(in, static_cast<int>(k_count), out);

  EXPECT_EQ(in, out);
}

INSTANTIATE_TEST_SUITE_P_NOLINT(PicMatrixTests, NesterovATestTaskAll, ::testing::Values(5, 10));
