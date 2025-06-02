#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <stb_library.hpp>
#include <string>
#include <utility>
#include <vector>

#include "core/util/include/test_util.hpp"
#include "core/util/include/util.hpp"
#include "example/all/include/ops_all.hpp"
#include "example/mpi/include/ops_mpi.hpp"
#include "example/omp/include/ops_omp.hpp"
#include "example/seq/include/ops_seq.hpp"
#include "example/stl/include/ops_stl.hpp"
#include "example/tbb/include/ops_tbb.hpp"

namespace nesterov_a_test_task {

class NesterovARunFuncTests : public BaseFuncTests {
  InType input_data_;

 public:
  static std::string PrintTestParam(TestType test_param) {
    return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    int width = -1;
    int height = -1;
    int channels = -1;
    // Read image
    {
      std::string abs_path = ppc::util::GetAbsolutePath("example/data/pic_all.jpg");
      auto *data = stbi_load(abs_path.c_str(), &width, &height, &channels, 0);
      if (data == nullptr) {
        throw std::runtime_error("Failed to load image: " + std::string(stbi_failure_reason()));
      }
      auto img = std::vector<uint8_t>(data, data + (static_cast<ptrdiff_t>(width * height * channels)));
      stbi_image_free(data);
      if (std::cmp_not_equal(width, height)) {
        throw std::runtime_error("width != height: ");
      }
    }

    TestType params = std::get<ppc::util::GTestParamIndex::kTestParams>(GetParam());
    const auto k_count = static_cast<const std::vector<int>::size_type>(((width + height) / std::get<0>(params)) *
                                                                        std::stoi(std::get<1>(params)));
    input_data_ = InType(k_count * k_count, 0);
    for (std::vector<int>::size_type i = 0; i < k_count; i++) {
      input_data_[(i * k_count) + i] = 1;
    }
  }

  bool CheckTestOutputData(OutType &output_data) final { return input_data_ == output_data; }

  InType GetTestInputData() final { return input_data_; }
};

namespace {

TEST_P(NesterovARunFuncTests, MatmulFromPic) { ExecuteTest(GetParam()); }

constexpr std::array<TestType, 3> kTestParam = {std::make_tuple(5, "5"), std::make_tuple(10, "10"),
                                                std::make_tuple(15, "15")};

INIT_TASK_GENERATOR(InType, kTestParam)

const auto kTestTasksList =
    std::tuple_cat(TaskListGenerator<NesterovATestTaskALL>(), TaskListGenerator<NesterovATestTaskMPI>(),
                   TaskListGenerator<NesterovATestTaskOMP>(), TaskListGenerator<NesterovATestTaskSEQ>(),
                   TaskListGenerator<NesterovATestTaskSTL>(), TaskListGenerator<NesterovATestTaskTBB>());

INSTANTIATE_TEST_SUITE_P_NOLINT(PicMatrixTests, NesterovARunFuncTests, ppc::util::ExpandToValues(kTestTasksList),
                                NesterovARunFuncTests::PrintFuncTestName<NesterovARunFuncTests>);

}  // namespace

}  // namespace nesterov_a_test_task
