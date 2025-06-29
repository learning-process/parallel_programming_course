#include <gtest/gtest.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

#include "core/task/include/task.hpp"
#include "core/task/tests/test_task.hpp"

TEST(task_tests, check_int32_t) {
  std::vector<int32_t> in(20, 1);
  ppc::test::task::TestTask<std::vector<int32_t>, int32_t> test_task(in);
  ASSERT_EQ(test_task.Validation(), true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  ASSERT_EQ(static_cast<size_t>(test_task.GetOutput()), in.size());
}

TEST(task_tests, check_int32_t_slow) {
  std::vector<int32_t> in(20, 1);
  ppc::test::task::FakeSlowTask<std::vector<int32_t>, int32_t> test_task(in);
  ASSERT_EQ(test_task.Validation(), true);
  test_task.PreProcessing();
  test_task.Run();
  ASSERT_ANY_THROW(test_task.PostProcessing());
}

TEST(task_tests, check_validate_func) {
  std::vector<int32_t> in;
  ppc::test::task::TestTask<std::vector<int32_t>, int32_t> test_task(in);
  ASSERT_EQ(test_task.Validation(), false);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
}

TEST(task_tests, check_double) {
  std::vector<double> in(20, 1);
  ppc::test::task::TestTask<std::vector<double>, double> test_task(in);
  ASSERT_EQ(test_task.Validation(), true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_NEAR(test_task.GetOutput(), static_cast<double>(in.size()), 1e-6);
}

TEST(task_tests, check_float) {
  std::vector<float> in(20, 1);
  ppc::test::task::TestTask<std::vector<float>, float> test_task(in);
  ASSERT_EQ(test_task.Validation(), true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_NEAR(test_task.GetOutput(), in.size(), 1e-3);
}

TEST(task_tests, check_wrong_order_disabled_valgrind) {
  std::vector<float> in(20, 1);
  ppc::test::task::TestTask<std::vector<float>, float> test_task(in);
  ASSERT_EQ(test_task.Validation(), true);
  test_task.PreProcessing();
  EXPECT_THROW(test_task.PostProcessing(), std::runtime_error);
}

TEST(task_tests, premature_postprocessing_no_steps) {
  std::vector<float> in(20, 1);
  ppc::test::task::TestTask<std::vector<float>, float> test_task(in);
  EXPECT_THROW(test_task.PostProcessing(), std::runtime_error);
}

TEST(task_tests, premature_postprocessing_after_preprocessing) {
  std::vector<float> in(20, 1);
  ppc::test::task::TestTask<std::vector<float>, float> test_task(in);
  EXPECT_THROW(test_task.PreProcessing(), std::runtime_error);
  EXPECT_THROW(test_task.PostProcessing(), std::runtime_error);
}

TEST(TaskTest, GetStringTaskStatus_Disabled) {
  EXPECT_EQ(GetStringTaskStatus(ppc::core::StatusOfTask::kDisabled), "disabled");
}

TEST(TaskTest, GetStringTaskStatus_Enabled) {
  EXPECT_EQ(GetStringTaskStatus(ppc::core::StatusOfTask::kEnabled), "enabled");
}

TEST(TaskTest, GetStringTaskType_InvalidFileThrows) {
  EXPECT_THROW({ GetStringTaskType(ppc::core::TypeOfTask::kALL, "non_existing_file.json"); }, std::runtime_error);
}

TEST(TaskTest, GetStringTaskType_UnknownType_WithValidFile) {
  std::string path = "settings_valid.json";
  std::ofstream file(path);
  file
      << R"({"tasks": {"all": "enabled", "stl": "enabled", "omp": "enabled", "mpi": "enabled", "tbb": "enabled", "seq": "enabled"}})";
  file.close();
  EXPECT_NO_THROW({ GetStringTaskType(ppc::core::TypeOfTask::kUnknown, path); });
}

TEST(TaskTest, GetStringTaskType_ThrowsOnBadJSON) {
  std::string path = "bad_settings.json";
  std::ofstream file(path);
  file << "{";
  file.close();
  EXPECT_THROW({ GetStringTaskType(ppc::core::TypeOfTask::kALL, path); }, std::exception);
}

TEST(TaskTest, GetStringTaskType_EachType_WithValidFile) {
  std::string path = "settings_valid_all.json";
  std::ofstream file(path);
  file
      << R"({"tasks": {"all": "enabled", "stl": "enabled", "omp": "enabled", "mpi": "enabled", "tbb": "enabled", "seq": "enabled"}})";
  file.close();

  EXPECT_NO_THROW(ppc::core::GetStringTaskType(ppc::core::TypeOfTask::kALL, path));
  EXPECT_NO_THROW(ppc::core::GetStringTaskType(ppc::core::TypeOfTask::kSTL, path));
  EXPECT_NO_THROW(ppc::core::GetStringTaskType(ppc::core::TypeOfTask::kOMP, path));
  EXPECT_NO_THROW(ppc::core::GetStringTaskType(ppc::core::TypeOfTask::kMPI, path));
  EXPECT_NO_THROW(ppc::core::GetStringTaskType(ppc::core::TypeOfTask::kTBB, path));
  EXPECT_NO_THROW(ppc::core::GetStringTaskType(ppc::core::TypeOfTask::kSEQ, path));
}

TEST(TaskTest, GetStringTaskType_ReturnsUnknown_OnDefault) {
  std::string path = "settings_valid_unknown.json";
  std::ofstream file(path);
  file << R"({"tasks": {"all": "enabled"}})";
  file.close();

  auto result = ppc::core::GetStringTaskType(ppc::core::TypeOfTask::kUnknown, path);
  EXPECT_EQ(result, "unknown");
}

TEST(TaskTest, GetStringTaskType_ThrowsIfKeyMissing) {
  std::string path = "settings_partial.json";
  std::ofstream file(path);
  file << R"({"tasks": {"all": "enabled"}})";
  file.close();

  EXPECT_ANY_THROW(ppc::core::GetStringTaskType(ppc::core::TypeOfTask::kSTL, path));
}

TEST(TaskTest, TaskDestructor_ThrowsIfStageIncomplete) {
  testing::FLAGS_gtest_death_test_style = "threadsafe";
  auto destroy_func = [] {
    std::vector<int32_t> in(20, 1);
    struct LocalTask : ppc::core::Task<std::vector<int32_t>, int32_t> {
      explicit LocalTask(const std::vector<int32_t>& in) { this->GetInput() = in; }
      bool ValidationImpl() override { return true; }
      bool PreProcessingImpl() override { return true; }
      bool RunImpl() override { return true; }
      bool PostProcessingImpl() override { return true; }
    } task(in);
    task.Validation();
  };
  EXPECT_DEATH_IF_SUPPORTED(destroy_func(), ".*ORDER OF FUNCTIONS IS NOT RIGHT.*");
}

TEST(TaskTest, InternalTimeTest_ThrowsIfTimeoutExceeded) {
  struct SlowTask : ppc::core::Task<std::vector<int32_t>, int32_t> {
    explicit SlowTask(const std::vector<int32_t>& in) { this->GetInput() = in; }
    bool ValidationImpl() override { return true; }
    bool PreProcessingImpl() override {
      std::this_thread::sleep_for(std::chrono::seconds(2));
      return true;
    }
    bool RunImpl() override { return true; }
    bool PostProcessingImpl() override { return true; }
  };

  std::vector<int32_t> in(20, 1);
  SlowTask task(in);
  task.GetStateOfTesting() = ppc::core::StateOfTesting::kFunc;
  task.Validation();
  EXPECT_NO_THROW(task.PreProcessing());
  task.Run();
  EXPECT_THROW(task.PostProcessing(), std::runtime_error);
}

class DummyTask : public ppc::core::Task<int, int> {
 public:
  using Task::Task;
  bool ValidationImpl() override { return true; }
  bool PreProcessingImpl() override { return true; }
  bool RunImpl() override { return true; }
  bool PostProcessingImpl() override { return true; }
};

TEST(TaskTest, ValidationThrowsIfCalledTwice) {
  auto task = std::make_shared<DummyTask>();
  task->Validation();
  EXPECT_THROW(task->Validation(), std::runtime_error);
}

TEST(TaskTest, PreProcessingThrowsIfCalledBeforeValidation) {
  auto task = std::make_shared<DummyTask>();
  EXPECT_THROW(task->PreProcessing(), std::runtime_error);
}

TEST(TaskTest, RunThrowsIfCalledBeforePreProcessing) {
  auto task = std::make_shared<DummyTask>();
  EXPECT_THROW(task->Run(), std::runtime_error);
}

TEST(TaskTest, PostProcessingThrowsIfCalledBeforeRun) {
  auto task = std::make_shared<DummyTask>();
  task->Validation();
  task->PreProcessing();
  EXPECT_THROW(task->PostProcessing(), std::runtime_error);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
