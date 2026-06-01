#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "example_processes_3/common/include/common.hpp"
#include "example_processes_3/mpi/include/ops_mpi.hpp"
#include "example_processes_3/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace nesterov_a_test_task_processes_3 {

class NesterovARunFuncTestsProcesses3 : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void RunTestCase(const ppc::util::FuncTestParam<InType, OutType, TestType> &test_param) {
    const std::string &test_name =
        std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kNameTest)>(test_param);
    if (IsTestDisabled(test_name) || ShouldSkipNonMpiTask(test_name)) {
      return;
    }

    SetInputData();
    ExecuteTest(test_param);
  }

  void SetInputData() {
    int width = -1;
    int height = -1;
    int channels = -1;
    std::vector<uint8_t> img;
    // Read image in RGB to ensure consistent channel count
    {
      std::string abs_path = ppc::util::GetAbsoluteTaskPath(PPC_ID_example_processes_3, "pic.ppm");
      auto *data = stbi_load(abs_path.c_str(), &width, &height, &channels, STBI_rgb);
      if (data == nullptr) {
        throw std::runtime_error("Failed to load image: " + std::string(stbi_failure_reason()));
      }
      channels = STBI_rgb;
      img = std::vector<uint8_t>(data, data + (static_cast<ptrdiff_t>(width * height * channels)));
      stbi_image_free(data);
      if (std::cmp_not_equal(width, height)) {
        throw std::runtime_error("width != height: ");
      }
    }

    input_data_ = width - height + std::min(std::accumulate(img.begin(), img.end(), 0), channels);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return (input_data_ == output_data);
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_ = 0;
};

namespace {

const std::array<TestType, 3> kTestParam = {std::make_tuple(3, "3"), std::make_tuple(5, "5"), std::make_tuple(7, "7")};

const auto kTestTasksList =
    std::tuple_cat(ppc::util::AddFuncTask<NesterovATestTaskMPI, InType>(kTestParam, PPC_SETTINGS_example_processes_3),
                   ppc::util::AddFuncTask<NesterovATestTaskSEQ, InType>(kTestParam, PPC_SETTINGS_example_processes_3));

}  // namespace

TEST_F(NesterovARunFuncTestsProcesses3, MatmulFromPic) {
  std::apply([this](const auto &...test_params) { (RunTestCase(test_params), ...); }, kTestTasksList);
}

}  // namespace nesterov_a_test_task_processes_3
