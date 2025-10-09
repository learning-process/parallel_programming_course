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

using ppc::task::StateOfTesting;
using ppc::task::StatusOfTask;
using ppc::task::Task;
using ppc::task::TypeOfTask;

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
  explicit TestTask(const InType &in) {
    this->GetInput() = in;
  }

  bool ValidationImpl() override {
    return !this->GetInput().empty();
  }

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

  bool PostProcessingImpl() override {
    return true;
  }
};

template <typename InType, typename OutType>
class FakeSlowTask : public TestTask<InType, OutType> {
 public:
  explicit FakeSlowTask(const InType &in) : TestTask<InType, OutType>(in) {}

  bool RunImpl() override {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return TestTask<InType, OutType>::RunImpl();
  }
};

}  // namespace ppc::test

TEST(TaskTests, CheckInt32t) {
  std::vector<int32_t> in(20, 1);
  ppc::test::TestTask<std::vector<int32_t>, int32_t> test_task(in);
  ASSERT_EQ(test_task.Validation(), true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  ASSERT_EQ(static_cast<size_t>(test_task.GetOutput()), in.size());
}

TEST(TaskTests, CheckInt32tSlow) {
  std::vector<int32_t> in(20, 1);
  ppc::test::FakeSlowTask<std::vector<int32_t>, int32_t> test_task(in);
  ASSERT_EQ(test_task.Validation(), true);
  test_task.PreProcessing();
  test_task.Run();
  ASSERT_ANY_THROW(test_task.PostProcessing());
}

TEST(TaskTests, SlowTaskRespectsEnvOverride) {
  env::detail::set_scoped_environment_variable scoped("PPC_TASK_MAX_TIME", "3");
  std::vector<int32_t> in(20, 1);
  ppc::test::FakeSlowTask<std::vector<int32_t>, int32_t> test_task(in);
  ASSERT_EQ(test_task.Validation(), true);
  test_task.PreProcessing();
  test_task.Run();
  EXPECT_NO_THROW(test_task.PostProcessing());
}

TEST(TaskTests, CheckValidateFunc) {
  std::vector<int32_t> in;
  ppc::test::TestTask<std::vector<int32_t>, int32_t> test_task(in);
  ASSERT_EQ(test_task.Validation(), false);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
}

TEST(TaskTests, CheckDouble) {
  std::vector<double> in(20, 1);
  ppc::test::TestTask<std::vector<double>, double> test_task(in);
  ASSERT_EQ(test_task.Validation(), true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_NEAR(test_task.GetOutput(), static_cast<double>(in.size()), 1e-6);
}

TEST(TaskTests, CheckFloat) {
  std::vector<float> in(20, 1);
  ppc::test::TestTask<std::vector<float>, float> test_task(in);
  ASSERT_EQ(test_task.Validation(), true);
  test_task.PreProcessing();
  test_task.Run();
  test_task.PostProcessing();
  EXPECT_NEAR(test_task.GetOutput(), in.size(), 1e-3);
}

TEST(TaskTests, CheckWrongOrderDisabledValgrind) {
  std::vector<float> in(20, 1);
  ppc::test::TestTask<std::vector<float>, float> test_task(in);
  ASSERT_EQ(test_task.Validation(), true);
  test_task.PreProcessing();
  EXPECT_THROW(test_task.PostProcessing(), std::runtime_error);
}

TEST(TaskTests, PrematurePostprocessingNoSteps) {
  std::vector<float> in(20, 1);
  ppc::test::TestTask<std::vector<float>, float> test_task(in);
  EXPECT_THROW(test_task.PostProcessing(), std::runtime_error);
}

TEST(TaskTests, PrematurePostprocessingAfterPreprocessing) {
  std::vector<float> in(20, 1);
  ppc::test::TestTask<std::vector<float>, float> test_task(in);
  EXPECT_THROW(test_task.PreProcessing(), std::runtime_error);
  EXPECT_THROW(test_task.PostProcessing(), std::runtime_error);
}

TEST(TaskTest, GetStringTaskStatusDisabled) {
  EXPECT_EQ(GetStringTaskStatus(StatusOfTask::kDisabled), "disabled");
}

TEST(TaskTest, GetStringTaskStatusEnabled) {
  EXPECT_EQ(GetStringTaskStatus(StatusOfTask::kEnabled), "enabled");
}

TEST(TaskTest, GetStringTaskTypeInvalidFileThrows) {
  EXPECT_THROW({ GetStringTaskType(TypeOfTask::kALL, "non_existing_file.json"); }, std::runtime_error);
}

TEST(TaskTest, GetStringTaskTypeUnknownTypeWithValidFile) {
  std::string path = "settings_valid.json";
  ScopedFile cleaner(path);
  std::ofstream file(path);
  file
      << R"({"tasks": {"all": "enabled", "stl": "enabled", "omp": "enabled", "mpi": "enabled", "tbb": "enabled", "seq": "enabled"}})";
  file.close();
  EXPECT_NO_THROW({ GetStringTaskType(TypeOfTask::kUnknown, path); });
}

TEST(TaskTest, GetStringTaskTypeThrowsOnBadJSON) {
  std::string path = "bad_settings.json";
  ScopedFile cleaner(path);
  std::ofstream file(path);
  file << "{";
  file.close();
  EXPECT_THROW({ GetStringTaskType(TypeOfTask::kALL, path); }, std::exception);
}

TEST(TaskTest, GetStringTaskTypeEachTypeWithValidFile) {
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

TEST(TaskTest, GetStringTaskTypeReturnsUnknownOnDefault) {
  std::string path = "settings_valid_unknown.json";
  ScopedFile cleaner(path);
  std::ofstream file(path);
  file << R"({"tasks": {"all": "enabled"}})";
  file.close();

  auto result = GetStringTaskType(TypeOfTask::kUnknown, path);
  EXPECT_EQ(result, "unknown");
}

TEST(TaskTest, GetStringTaskTypeThrowsIfKeyMissing) {
  std::string path = "settings_partial.json";
  ScopedFile cleaner(path);
  std::ofstream file(path);
  file << R"({"tasks": {"all": "enabled"}})";
  file.close();

  EXPECT_ANY_THROW(GetStringTaskType(TypeOfTask::kSTL, path));
}

TEST(TaskTest, TaskDestructorThrowsIfStageIncomplete) {
  {
    std::vector<int32_t> in(20, 1);
    struct LocalTask : Task<std::vector<int32_t>, int32_t> {
      explicit LocalTask(const std::vector<int32_t> &in) {
        this->GetInput() = in;
      }
      bool ValidationImpl() override {
        return true;
      }
      bool PreProcessingImpl() override {
        return true;
      }
      bool RunImpl() override {
        return true;
      }
      bool PostProcessingImpl() override {
        return true;
      }
    } task(in);
    task.Validation();
  }
  EXPECT_TRUE(ppc::util::DestructorFailureFlag::Get());
  ppc::util::DestructorFailureFlag::Unset();
}

TEST(TaskTest, TaskDestructorThrowsIfEmpty) {
  {
    std::vector<int32_t> in(20, 1);
    struct LocalTask : Task<std::vector<int32_t>, int32_t> {
      explicit LocalTask(const std::vector<int32_t> &in) {
        this->GetInput() = in;
      }
      bool ValidationImpl() override {
        return true;
      }
      bool PreProcessingImpl() override {
        return true;
      }
      bool RunImpl() override {
        return true;
      }
      bool PostProcessingImpl() override {
        return true;
      }
    } task(in);
  }
  EXPECT_TRUE(ppc::util::DestructorFailureFlag::Get());
  ppc::util::DestructorFailureFlag::Unset();
}

TEST(TaskTest, InternalTimeTestThrowsIfTimeoutExceeded) {
  struct SlowTask : Task<std::vector<int32_t>, int32_t> {
    explicit SlowTask(const std::vector<int32_t> &in) {
      this->GetInput() = in;
    }
    bool ValidationImpl() override {
      return true;
    }
    bool PreProcessingImpl() override {
      std::this_thread::sleep_for(std::chrono::seconds(2));
      return true;
    }
    bool RunImpl() override {
      return true;
    }
    bool PostProcessingImpl() override {
      return true;
    }
  };

  std::vector<int32_t> in(20, 1);
  SlowTask task(in);
  task.GetStateOfTesting() = StateOfTesting::kFunc;
  task.Validation();
  EXPECT_NO_THROW(task.PreProcessing());
  task.Run();
  EXPECT_THROW(task.PostProcessing(), std::runtime_error);
}

class DummyTask : public Task<int, int> {
 public:
  using Task::Task;
  bool ValidationImpl() override {
    return true;
  }
  bool PreProcessingImpl() override {
    return true;
  }
  bool RunImpl() override {
    return true;
  }
  bool PostProcessingImpl() override {
    return true;
  }
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

int main(int argc, char **argv) {
  return ppc::runners::SimpleInit(argc, argv);
}
