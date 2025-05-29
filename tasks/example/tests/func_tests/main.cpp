#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <stb_library.hpp>
#include <string>
#include <vector>

#include "core/util/include/test_util.hpp"
#include "core/util/include/util.hpp"
#include "example/all/include/ops_all.hpp"
#include "example/mpi/include/ops_mpi.hpp"
#include "example/omp/include/ops_omp.hpp"
#include "example/seq/include/ops_seq.hpp"
#include "example/stl/include/ops_stl.hpp"
#include "example/tbb/include/ops_tbb.hpp"

using InType = std::vector<int>;
using OutType = std::vector<int>;
using TestType = int;

class NesterovARunFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
  InType input_data_;

 protected:
  void SetUp() override {
    int width = -1;
    int height = -1;
    int channels = -1;
    // Read image
    {
      std::string abs_path = ppc::util::GetAbsolutePath("example/tests/data/pic_all.jpg");
      auto *data = stbi_load(abs_path.c_str(), &width, &height, &channels, 0);
      if (data == nullptr) {
        throw std::runtime_error("Failed to load image: " + std::string(stbi_failure_reason()));
      }
      auto img = std::vector<uint8_t>(data, data + (static_cast<ptrdiff_t>(width * height * channels)));
      stbi_image_free(data);
      if (width != height) {
        throw std::runtime_error("width != height: ");
      }
    }

    const int k_count = (width + height) / std::get<ppc::util::TestParamIndex::kTestParams>(GetParam());
    input_data_ = InType(static_cast<std::vector<int>::size_type>(k_count * k_count), 0);
    for (int i = 0; i < k_count; i++) {
      input_data_[(i * k_count) + i] = 1;
    }
  }

  bool CheckTestOutputData(OutType &output_data) final { return input_data_ == output_data; }

  InType GetTestInputData() final { return input_data_; }
};

namespace {

TEST_P(NesterovARunFuncTests, MatmulFromPic) { ExecuteTest(GetParam()); }

constexpr std::array<TestType, 3> kTestParam = {5, 10, 15};

INIT_TASK_GENERATOR(InType, kTestParam)

const auto kTestTasksList = std::tuple_cat(TaskListGenerator<nesterov_a_test_task_all::NesterovATestTaskALL>(),
                                           TaskListGenerator<nesterov_a_test_task_mpi::NesterovATestTaskMPI>(),
                                           TaskListGenerator<nesterov_a_test_task_omp::NesterovATestTaskOMP>(),
                                           TaskListGenerator<nesterov_a_test_task_seq::NesterovATestTaskSEQ>(),
                                           TaskListGenerator<nesterov_a_test_task_stl::NesterovATestTaskSTL>(),
                                           TaskListGenerator<nesterov_a_test_task_tbb::NesterovATestTaskTBB>());

INSTANTIATE_TEST_SUITE_P_NOLINT(PicMatrixTests, NesterovARunFuncTests, ppc::util::ExpandToValues(kTestTasksList),
                                [](const auto &info) {
                                  std::string test_name = std::get<ppc::util::TestParamIndex::kNameTest>(info.param);
                                  TestType test_param = std::get<ppc::util::TestParamIndex::kTestParams>(info.param);
                                  return test_name + "_" + std::to_string(test_param);
                                });

}  // namespace
