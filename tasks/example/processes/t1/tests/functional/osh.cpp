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

#include "example/common/include/common.hpp"
#include "example/processes/t1/osh/include/ops_osh.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace example_processes_t1 {

class NesterovARunFuncTestsProcessesOSH : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void RunTestCase(const ppc::util::FuncTestParam<InType, OutType, TestType> &test_param) override {
    if (ShouldSkipTestCase(test_param)) {
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
    {
      std::string abs_path = ppc::util::GetAbsoluteTaskPath(PPC_ID_example, "pic.ppm");
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
    ppc::util::AddFuncTask<NesterovATestTaskOSH, InType>(kTestParam, PPC_SETTINGS_example, "processes.t1");

}  // namespace

TEST_F(NesterovARunFuncTestsProcessesOSH, MatmulFromPicOSHEnabled) {
  RunTestCasesWithTag(kTestTasksList, "osh");
}

}  // namespace example_processes_t1
