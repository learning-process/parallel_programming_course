#include <gtest/gtest.h>

#include <compare>
#include <cstddef>
#include <cstdint>
#include <stb_library.hpp>
#include <string>
#include <utility>
#include <vector>

#include "core/util/include/func_test_util.hpp"
#include "core/util/include/util.hpp"
#include "example/all/include/ops_all.hpp"
#include "example/mpi/include/ops_mpi.hpp"
#include "example/omp/include/ops_omp.hpp"
#include "example/seq/include/ops_seq.hpp"
#include "example/stl/include/ops_stl.hpp"
#include "example/tbb/include/ops_tbb.hpp"

namespace nesterov_a_test_task {

class NesterovARunFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    int width = -1;
    int height = -1;
    int channels = -1;
    std::vector<uint8_t> img;
    // Read image
    {
      std::string abs_path = ppc::util::GetAbsolutePath("example/data/pic.jpg");
      auto *data = stbi_load(abs_path.c_str(), &width, &height, &channels, 0);
      if (data == nullptr) {
        throw std::runtime_error("Failed to load image: " + std::string(stbi_failure_reason()));
      }
      img = std::vector<uint8_t>(data, data + (static_cast<ptrdiff_t>(width * height * channels)));
      stbi_image_free(data);
      if (std::cmp_not_equal(width, height)) {
        throw std::runtime_error("width != height: ");
      }
    }

    TestType params = std::get<ppc::util::GTestParamIndex::kTestParams>(GetParam());
    input_data_ = width - height + std::min(std::accumulate(img.begin(), img.end(), 0), channels);
  }

  bool CheckTestOutputData(OutType &output_data) final { return (input_data_ == output_data); }

  InType GetTestInputData() final { return input_data_; }

 private:
  InType input_data_ = 0;
};

namespace {

TEST_P(NesterovARunFuncTests, MatmulFromPic) { ExecuteTest(GetParam()); }

const std::array<TestType, 3> kTestParam = {std::make_tuple(3, "3"), std::make_tuple(5, "5"), std::make_tuple(7, "7")};

INIT_TASK_GENERATOR(InType, kTestParam, PPC_SETTINGS_example)

const auto kTestTasksList =
    std::tuple_cat(ADD_FUNC_TASK_PROCESS(NesterovATestTaskALL), ADD_FUNC_TASK_PROCESS(NesterovATestTaskMPI),
                   ADD_FUNC_TASK_THREADS(NesterovATestTaskOMP), ADD_FUNC_TASK_THREADS(NesterovATestTaskSEQ),
                   ADD_FUNC_TASK_THREADS(NesterovATestTaskSTL), ADD_FUNC_TASK_THREADS(NesterovATestTaskTBB));

INSTANTIATE_TEST_SUITE_P_NOLINT(PicMatrixTests, NesterovARunFuncTests, ppc::util::ExpandToValues(kTestTasksList),
                                NesterovARunFuncTests::PrintFuncTestName<NesterovARunFuncTests>);

}  // namespace

}  // namespace nesterov_a_test_task
