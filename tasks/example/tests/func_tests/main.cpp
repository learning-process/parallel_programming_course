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

class NesterovARunFuncTests : public ::testing::TestWithParam<ppc::util::FuncTestParam<InType, OutType, int>> {
 public:
  static std::string CustomFuncTestName(
      const ::testing::TestParamInfo<ppc::util::FuncTestParam<InType, OutType, int>>& info) {
    return std::get<ppc::util::FuncTestParamIndex::kNameTest>(info.param) + "_" +
        std::to_string(std::get<ppc::util::FuncTestParamIndex::kAddParams>(info.param));
  }

 protected:
  void SetUp() override {
    // Read image
    std::string abs_path = ppc::util::GetAbsolutePath("example/tests/data/pic_all.jpg");
    data = stbi_load(abs_path.c_str(), &width, &height, &channels, 0);
    ASSERT_TRUE(data != nullptr) << "Failed to load image: " << stbi_failure_reason();
    img = std::vector<uint8_t>(data, data + (static_cast<ptrdiff_t>(width * height * channels)));
    stbi_image_free(data);
    ASSERT_EQ(width, height);

    const int k_count = (width + height) / std::get<ppc::util::FuncTestParamIndex::kAddParams>(GetParam());
    std::vector<int> in(static_cast<std::vector<int>::size_type>(k_count * k_count), 0);
    for (int i = 0; i < k_count; i++) {
      in[(i * k_count) + i] = 1;
    }
    task = std::get<ppc::util::FuncTestParamIndex::kTaskGetter>(GetParam())(in);
  }

  void ExecuteTest() {
    ASSERT_TRUE(task->Validation());
    task->PreProcessing();
    task->Run();
    task->PostProcessing();
    EXPECT_EQ(task->GetInput(), task->GetOutput());
  }

  ppc::core::TaskPtr<InType, OutType> task;
  int width = -1, height = -1, channels = -1;
  unsigned char* data = nullptr;
  std::vector<uint8_t> img;
};

TEST_P(NesterovARunFuncTests, MatmulFromPic) { ExecuteTest(); }

#define ADD_FUNC_TASK(TASK)                                    \
  std::make_tuple(ppc::core::TaskGetter<TASK, InType>, ppc::util::GetNamespace<TASK>(), 5), \
      std::make_tuple(ppc::core::TaskGetter<TASK, InType>, ppc::util::GetNamespace<TASK>(), 10)

INSTANTIATE_TEST_SUITE_P_NOLINT(PicMatrixTests, NesterovARunFuncTests,
                                ::testing::Values(ADD_FUNC_TASK(nesterov_a_test_task_all::TestTaskALL),
                                                  ADD_FUNC_TASK(nesterov_a_test_task_mpi::TestTaskMPI),
                                                  ADD_FUNC_TASK(nesterov_a_test_task_omp::TestTaskOMP),
                                                  ADD_FUNC_TASK(nesterov_a_test_task_seq::TestTaskSEQ),
                                                  ADD_FUNC_TASK(nesterov_a_test_task_stl::TestTaskSTL),
                                                  ADD_FUNC_TASK(nesterov_a_test_task_tbb::TestTaskTBB)),
                                NesterovARunFuncTests::CustomFuncTestName);
