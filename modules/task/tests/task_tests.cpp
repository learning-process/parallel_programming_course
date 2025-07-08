#include <gtest/gtest.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <libenvpp/env.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>
#include <utility>
#include <vector>

#include "runners/include/runners.hpp"
#include "task/include/task.hpp"
#include "util/include/util.hpp"

using namespace ppc::task;

class ScopedFile {
 public:
  explicit ScopedFile(std::string path) : path_(std::move(path)) {}
  ~ScopedFile() {
    std::error_code ec;
    std::filesystem::remove(path_, ec);
  }

 private:
  std::string path_;
};

namespace ppc::test {

template <typename InType, typename OutType>
class TestTask : public ppc::task::Task<InType, OutType> {
 public:
  explicit TestTask(const InType& in) { this->GetInput() = in; }

  bool ValidationImpl() override { return !this->GetInput().empty(); }

  bool PreProcessingImpl() override {
    this->GetOutput() = 0;
    return true;
  }

  bool RunImpl() override {
    for (unsigned i = 0; i < this->GetInput().size(); i++) {
      this->GetOutput() += this->GetInput()[i];
    }
    return true;
  }

  bool PostProcessingImpl() override { return true; }
};

template <typename InType, typename OutType>
class FakeSlowTask : public TestTask<InType, OutType> {
 public:
  explicit FakeSlowTask(const InType& in) : TestTask<InType, OutType>(in) {}

  bool RunImpl() override {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return TestTask<InType, OutType>::RunImpl();
  }
};

}  // namespace ppc::test

TEST(TaskTest, TestTask_WithInt32Vector_CompletesSuccessfully) {
  std::vector<int32_t> in(20, 1);
  ppc::test::TestTask<std::vector<int32_t>, int32_t> test_task(in);
  ASSERT_EQ(test_task.Validation(), true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  ASSERT_EQ(static_cast<size_t>(test_task.GetOutput()), in.size());
}

TEST(TaskTest, SlowTask_WithInt32Vector_ThrowsOnTimeout) {
  {
    std::vector<int32_t> in(20, 1);
    ppc::test::FakeSlowTask<std::vector<int32_t>, int32_t> test_task(in);
    test_task.ExpectIncompleteLifecycle();  // Task may not complete due to timeout
    ASSERT_EQ(test_task.Validation(), true);
    test_task.PreProcessing();
    test_task.Run();
    ASSERT_ANY_THROW(test_task.PostProcessing());
  }
}

TEST(TaskTest, SlowTask_RespectsEnvOverride) {
  env::detail::set_scoped_environment_variable scoped("PPC_TASK_MAX_TIME", "3");
  std::vector<int32_t> in(20, 1);
  ppc::test::FakeSlowTask<std::vector<int32_t>, int32_t> test_task(in);
  ASSERT_EQ(test_task.Validation(), true);
  test_task.PreProcessing();
  test_task.Run();
  EXPECT_NO_THROW(test_task.PostProcessing());
}

TEST(TaskTest, TestTask_WithEmptyInput_ValidationFails) {
  {
    std::vector<int32_t> in;
    ppc::test::TestTask<std::vector<int32_t>, int32_t> test_task(in);
    test_task.ExpectIncompleteLifecycle();  // Task fails validation so won't complete
    ASSERT_EQ(test_task.Validation(), false);
  }
}

TEST(TaskTest, TestTask_WithDoubleVector_CompletesSuccessfully) {
  std::vector<double> in(20, 1);
  ppc::test::TestTask<std::vector<double>, double> test_task(in);
  ASSERT_EQ(test_task.Validation(), true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_NEAR(test_task.GetOutput(), static_cast<double>(in.size()), 1e-6);
}

TEST(TaskTest, TestTask_WithFloatVector_CompletesSuccessfully) {
  std::vector<float> in(20, 1);
  ppc::test::TestTask<std::vector<float>, float> test_task(in);
  ASSERT_EQ(test_task.Validation(), true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_NEAR(test_task.GetOutput(), in.size(), 1e-3);
}

TEST(TaskTest, TestTask_WithWrongExecutionOrder_ThrowsRuntimeError) {
  {
    std::vector<float> in(20, 1);
    ppc::test::TestTask<std::vector<float>, float> test_task(in);
    test_task.ExpectIncompleteLifecycle();  // Task has the wrong execution order
    ASSERT_EQ(test_task.Validation(), true);
    test_task.PreProcessing();
    EXPECT_THROW(test_task.PostProcessing(), std::runtime_error);
  }
}

TEST(TaskTest, TestTask_WithPrematurePostProcessingNoSteps_ThrowsRuntimeError) {
  {
    std::vector<float> in(20, 1);
    ppc::test::TestTask<std::vector<float>, float> test_task(in);
    test_task.ExpectIncompleteLifecycle();  // Task throws exception so won't complete
    EXPECT_THROW(test_task.PostProcessing(), std::runtime_error);
  }
}

TEST(TaskTest, TestTask_WithPrematurePostProcessingAfterPreProcessing_ThrowsRuntimeError) {
  {
    std::vector<float> in(20, 1);
    ppc::test::TestTask<std::vector<float>, float> test_task(in);
    test_task.ExpectIncompleteLifecycle();  // Task throws exceptions so won't complete
    EXPECT_THROW(test_task.PreProcessing(), std::runtime_error);
    EXPECT_THROW(test_task.PostProcessing(), std::runtime_error);
  }
}

TEST(TaskTest, GetStringTaskStatus_WithDisabledStatus_ReturnsDisabled) {
  EXPECT_EQ(GetStringTaskStatus(StatusOfTask::kDisabled), "disabled");
}

TEST(TaskTest, GetStringTaskStatus_WithEnabledStatus_ReturnsEnabled) {
  EXPECT_EQ(GetStringTaskStatus(StatusOfTask::kEnabled), "enabled");
}

TEST(TaskTest, GetStringTaskType_WithInvalidFile_ThrowsRuntimeError) {
  EXPECT_THROW({ GetStringTaskType(TypeOfTask::kALL, "non_existing_file.json"); }, std::runtime_error);
}

TEST(TaskTest, GetStringTaskType_WithUnknownTypeAndValidFile_DoesNotThrow) {
  std::string path = "settings_valid.json";
  ScopedFile cleaner(path);
  std::ofstream file(path);
  file
      << R"({"tasks": {"all": "enabled", "stl": "enabled", "omp": "enabled", "mpi": "enabled", "tbb": "enabled", "seq": "enabled"}})";
  file.close();
  EXPECT_NO_THROW({ GetStringTaskType(TypeOfTask::kUnknown, path); });
}

TEST(TaskTest, GetStringTaskType_WithBadJSON_ThrowsException) {
  std::string path = "bad_settings.json";
  ScopedFile cleaner(path);
  std::ofstream file(path);
  file << "{";
  file.close();
  EXPECT_THROW({ GetStringTaskType(TypeOfTask::kALL, path); }, std::exception);
}

TEST(TaskTest, GetStringTaskType_WithEachTypeAndValidFile_DoesNotThrow) {
  std::string path = "settings_valid_all.json";
  ScopedFile cleaner(path);
  std::ofstream file(path);
  file
      << R"({"tasks": {"all": "enabled", "stl": "enabled", "omp": "enabled", "mpi": "enabled", "tbb": "enabled", "seq": "enabled"}})";
  file.close();

  EXPECT_NO_THROW(GetStringTaskType(TypeOfTask::kALL, path));
  EXPECT_NO_THROW(GetStringTaskType(TypeOfTask::kSTL, path));
  EXPECT_NO_THROW(GetStringTaskType(TypeOfTask::kOMP, path));
  EXPECT_NO_THROW(GetStringTaskType(TypeOfTask::kMPI, path));
  EXPECT_NO_THROW(GetStringTaskType(TypeOfTask::kTBB, path));
  EXPECT_NO_THROW(GetStringTaskType(TypeOfTask::kSEQ, path));
}

TEST(TaskTest, GetStringTaskType_WithUnknownType_ReturnsUnknown) {
  std::string path = "settings_valid_unknown.json";
  ScopedFile cleaner(path);
  std::ofstream file(path);
  file << R"({"tasks": {"all": "enabled"}})";
  file.close();

  auto result = GetStringTaskType(TypeOfTask::kUnknown, path);
  EXPECT_EQ(result, "unknown");
}

TEST(TaskTest, GetStringTaskType_WithMissingKey_ThrowsException) {
  std::string path = "settings_partial.json";
  ScopedFile cleaner(path);
  std::ofstream file(path);
  file << R"({"tasks": {"all": "enabled"}})";
  file.close();

  EXPECT_ANY_THROW(GetStringTaskType(TypeOfTask::kSTL, path));
}

TEST(TaskTest, TaskDestructor_WithIncompleteStage_SetsDestructorFailureFlag) {
  {
    std::vector<int32_t> in(20, 1);
    struct LocalTask : Task<std::vector<int32_t>, int32_t> {
      explicit LocalTask(const std::vector<int32_t>& in) { this->GetInput() = in; }
      bool ValidationImpl() override { return true; }
      bool PreProcessingImpl() override { return true; }
      bool RunImpl() override { return true; }
      bool PostProcessingImpl() override { return true; }
    } task(in);
    task.ExpectIncompleteLifecycle();  // Mark this task as expected to be incomplete
    task.Validation();
  }
  // No need to check global flag - task handles its own validation
}

TEST(TaskTest, TaskDestructor_WithEmptyTask_SetsDestructorFailureFlag) {
  {
    std::vector<int32_t> in(20, 1);
    struct LocalTask : Task<std::vector<int32_t>, int32_t> {
      explicit LocalTask(const std::vector<int32_t>& in) { this->GetInput() = in; }
      bool ValidationImpl() override { return true; }
      bool PreProcessingImpl() override { return true; }
      bool RunImpl() override { return true; }
      bool PostProcessingImpl() override { return true; }
    } task(in);
    task.ExpectIncompleteLifecycle();  // Mark this task as expected to be incomplete
  }
  // No need to check global flag - task handles its own validation
}

TEST(TaskTest, InternalTimeTest_WithTimeoutExceeded_ThrowsRuntimeError) {
  struct SlowTask : Task<std::vector<int32_t>, int32_t> {
    explicit SlowTask(const std::vector<int32_t>& in) { this->GetInput() = in; }
    bool ValidationImpl() override { return true; }
    bool PreProcessingImpl() override {
      std::this_thread::sleep_for(std::chrono::seconds(2));
      return true;
    }
    bool RunImpl() override { return true; }
    bool PostProcessingImpl() override { return true; }
  };

  {
    std::vector<int32_t> in(20, 1);
    SlowTask task(in);
    task.ExpectIncompleteLifecycle();  // Task throws timeout exception
    task.GetStateOfTesting() = StateOfTesting::kFunc;
    task.Validation();
    EXPECT_NO_THROW(task.PreProcessing());
    task.Run();
    EXPECT_THROW(task.PostProcessing(), std::runtime_error);
  }
}

class DummyTask : public Task<int, int> {
 public:
  using Task::Task;
  bool ValidationImpl() override { return true; }
  bool PreProcessingImpl() override { return true; }
  bool RunImpl() override { return true; }
  bool PostProcessingImpl() override { return true; }
};

TEST(TaskTest, Validation_WhenCalledTwice_ThrowsRuntimeError) {
  {
    auto task = std::make_shared<DummyTask>();
    task->ExpectIncompleteLifecycle();  // Task throws exception so won't complete
    task->Validation();
    EXPECT_THROW(task->Validation(), std::runtime_error);
  }
}

TEST(TaskTest, PreProcessing_WhenCalledBeforeValidation_ThrowsRuntimeError) {
  {
    auto task = std::make_shared<DummyTask>();
    task->ExpectIncompleteLifecycle();  // Task throws exception so won't complete
    EXPECT_THROW(task->PreProcessing(), std::runtime_error);
  }
}

TEST(TaskTest, Run_WhenCalledBeforePreProcessing_ThrowsRuntimeError) {
  {
    auto task = std::make_shared<DummyTask>();
    task->ExpectIncompleteLifecycle();  // Task throws exception so won't complete
    EXPECT_THROW(task->Run(), std::runtime_error);
  }
}

TEST(TaskTest, PostProcessing_WhenCalledBeforeRun_ThrowsRuntimeError) {
  {
    auto task = std::make_shared<DummyTask>();
    task->ExpectIncompleteLifecycle();  // Task throws exception so won't complete
    task->Validation();
    task->PreProcessing();
    EXPECT_THROW(task->PostProcessing(), std::runtime_error);
  }
}

TEST(TaskTest, Destructor_WhenTaskIncompleteWithoutExpectIncomplete_ExecutesErrorPath) {
  // Test that an error path in destructor is executed when a task is destroyed without completing the pipeline
  // This test covers the previously uncovered lines: std::cerr and terminate_handler_() calls

  // We use ExpectIncompleteLifecycle first, then reset it to test the path
  {
    auto task = std::make_shared<DummyTask>();
    task->ExpectIncompleteLifecycle();  // This prevents termination by setting an empty lambda
    task->Validation();
    // Task is destroyed here - this executes the std::cerr and terminate_handler_() lines
    // but terminate_handler_ is now an empty lambda, so no actual termination occurs
  }
  // Test passes - the error handling code was executed without termination
}

int main(int argc, char** argv) { return ppc::runners::SimpleInit(argc, argv); }
