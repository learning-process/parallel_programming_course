#include <gtest/gtest.h>

#include <array>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <libenvpp/detail/environment.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "performance/include/performance.hpp"
#include "task/include/task.hpp"
#include "util/include/util.hpp"

using ppc::task::StatusOfTask;
using ppc::task::Task;
using ppc::task::TypeOfTask;

namespace ppc::test {

template <typename InType, typename OutType>
class TestPerfTask : public ppc::task::Task<InType, OutType> {
 public:
  explicit TestPerfTask(const InType &in) {
    this->GetInput() = in;
  }

 protected:
  bool ValidationImpl() override {
    return !this->GetInput().empty();
  }

  bool PreProcessingImpl() override {
    this->GetOutput() = 0;
    return true;
  }

  bool RunImpl() override {
    for (const auto &value : this->GetInput()) {
      this->GetOutput() += value;
    }
    return true;
  }

  bool PostProcessingImpl() override {
    return true;
  }
};

template <typename InType, typename OutType>
class FakePerfTask : public TestPerfTask<InType, OutType> {
 public:
  explicit FakePerfTask(const InType &in) : TestPerfTask<InType, OutType>(in) {}

 protected:
  bool RunImpl() override {
    std::this_thread::sleep_for(std::chrono::seconds(11));
    return TestPerfTask<InType, OutType>::RunImpl();
  }
};

}  // namespace ppc::test

namespace ppc::performance {

TEST(PerfTests, CheckPerfPipeline) {
  std::vector<uint32_t> in(2000, 1);

  auto test_task = std::make_shared<ppc::test::TestPerfTask<std::vector<uint32_t>, uint32_t>>(in);

  Perf<std::vector<uint32_t>, uint32_t> perf_analyzer(test_task);

  PerfAttr perf_attr;
  perf_analyzer.PipelineRun(perf_attr);

  perf_analyzer.PrintPerfStatistic("check_perf_pipeline");
  ASSERT_LE(perf_analyzer.GetPerfResults().time_sec, PerfResults::kMaxTime);
  EXPECT_EQ(test_task->GetOutput(), in.size());
}

TEST(PerfTests, CheckPerfPipelineFloat) {
  std::vector<float> in(2000, 1);

  auto test_task = std::make_shared<ppc::test::TestPerfTask<std::vector<float>, float>>(in);

  Perf<std::vector<float>, float> perf_analyzer(test_task);

  PerfAttr perf_attr;
  perf_analyzer.PipelineRun(perf_attr);

  perf_analyzer.PrintPerfStatistic("check_perf_pipeline_float");
  ASSERT_LE(perf_analyzer.GetPerfResults().time_sec, PerfResults::kMaxTime);
  EXPECT_EQ(test_task->GetOutput(), in.size());
}

TEST(PerfTests, CheckPerfPipelineUint8tSlowTest) {
  std::vector<uint8_t> in(128, 1);

  auto test_task = std::make_shared<ppc::test::FakePerfTask<std::vector<uint8_t>, uint8_t>>(in);

  Perf<std::vector<uint8_t>, uint8_t> perf_analyzer(test_task);

  PerfAttr perf_attr;
  perf_attr.num_running = 1;

  const auto t0 = std::chrono::high_resolution_clock::now();
  perf_attr.current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };
  perf_analyzer.PipelineRun(perf_attr);

  ASSERT_ANY_THROW(perf_analyzer.PrintPerfStatistic("check_perf_pipeline_uint8_t_slow_test"));
}

TEST(PerfTests, SlowPerfRespectsEnvOverride) {
  env::detail::set_scoped_environment_variable scoped("PPC_PERF_MAX_TIME", "12");
  std::vector<uint8_t> in(128, 1);
  auto test_task = std::make_shared<ppc::test::FakePerfTask<std::vector<uint8_t>, uint8_t>>(in);
  Perf<std::vector<uint8_t>, uint8_t> perf_analyzer(test_task);
  PerfAttr perf_attr;
  perf_attr.num_running = 1;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perf_attr.current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };
  perf_analyzer.PipelineRun(perf_attr);
  EXPECT_NO_THROW(perf_analyzer.PrintPerfStatistic("slow_perf_respects_env_override"));
}

TEST(PerfTests, CheckPerfTaskException) {
  std::vector<uint32_t> in(2000, 1);

  auto test_task = std::make_shared<ppc::test::TestPerfTask<std::vector<uint32_t>, uint32_t>>(in);

  Perf<std::vector<uint32_t>, uint32_t> perf_analyzer(test_task);

  ASSERT_ANY_THROW(perf_analyzer.PrintPerfStatistic("check_perf_task_exception"));

  PerfAttr perf_attr;
  perf_analyzer.TaskRun(perf_attr);
}

TEST(PerfTests, CheckPerfTaskFloat) {
  std::vector<float> in(2000, 1);

  auto test_task = std::make_shared<ppc::test::TestPerfTask<std::vector<float>, float>>(in);

  Perf<std::vector<float>, float> perf_analyzer(test_task);

  PerfAttr perf_attr;
  perf_analyzer.TaskRun(perf_attr);

  perf_analyzer.PrintPerfStatistic("check_perf_task_float");
  ASSERT_LE(perf_analyzer.GetPerfResults().time_sec, PerfResults::kMaxTime);
  EXPECT_EQ(test_task->GetOutput(), in.size());
}

struct ParamTestCase {
  PerfResults::TypeOfRunning input;
  std::string_view expected_output;
};

namespace {

constexpr std::array<ParamTestCase, 3> kParamTestCases = {
    {{.input = PerfResults::TypeOfRunning::kTaskRun, .expected_output = "task_run"},
     {.input = PerfResults::TypeOfRunning::kPipeline, .expected_output = "pipeline"},
     {.input = PerfResults::TypeOfRunning::kNone, .expected_output = "none"}}};

}  // namespace

TEST(GetStringParamNameParamTest, ReturnsExpectedString) {
  for (const auto &param : kParamTestCases) {
    EXPECT_EQ(GetStringParamName(param.input), std::string(param.expected_output));
  }
}

struct TaskTypeTestCase {
  TypeOfTask type;
  std::string_view expected;
  std::string_view label;
};

class GetStringTaskTypeTest : public ::testing::Test {
 protected:
  std::string temp_path;

  void SetUp() override {
    temp_path = (std::filesystem::temp_directory_path() / "test_settings.json").string();
    auto j = ppc::util::InitJSONPtr();
    *j = {{"tasks", {{"all", "ALL"}, {"stl", "STL"}, {"omp", "OMP"}, {"mpi", "MPI"}, {"tbb", "TBB"}, {"seq", "SEQ"}}}};

    std::ofstream(temp_path) << j->dump();
  }

  void TearDown() override {
    std::filesystem::remove(temp_path);
  }
};

namespace {

constexpr std::array<TaskTypeTestCase, 6> kTaskTypeTestCases = {
    {{.type = TypeOfTask::kALL, .expected = "all_ALL", .label = "kALL"},
     {.type = TypeOfTask::kSTL, .expected = "stl_STL", .label = "kSTL"},
     {.type = TypeOfTask::kOMP, .expected = "omp_OMP", .label = "kOMP"},
     {.type = TypeOfTask::kMPI, .expected = "mpi_MPI", .label = "kMPI"},
     {.type = TypeOfTask::kTBB, .expected = "tbb_TBB", .label = "kTBB"},
     {.type = TypeOfTask::kSEQ, .expected = "seq_SEQ", .label = "kSEQ"}}};

}  // namespace

TEST_F(GetStringTaskTypeTest, ReturnsExpectedString) {
  for (const auto &param : kTaskTypeTestCases) {
    EXPECT_EQ(GetStringTaskType(param.type, temp_path), std::string(param.expected)) << "Failed on: " << param.label;
  }
}

TEST(GetStringTaskTypeStandaloneTest, ThrowsIfFileMissing) {
  std::string missing_path = "non_existent_settings.json";
  EXPECT_THROW(GetStringTaskType(TypeOfTask::kSEQ, missing_path), std::runtime_error);
}

TEST(GetStringTaskTypeStandaloneTest, ExceptionMessageContainsPath) {
  const std::string missing_path = "non_existent_settings.json";
  EXPECT_THROW(try { GetStringTaskType(TypeOfTask::kSEQ, missing_path); } catch (const std::runtime_error &e) {
    EXPECT_NE(std::string(e.what()).find(missing_path), std::string::npos);
    throw;
  },
               std::runtime_error);
}

TEST(GetStringTaskTypeStandaloneTest, ReturnsUnknownForInvalidEnum) {
  std::string path = (std::filesystem::temp_directory_path() / "tmp_settings.json").string();
  std::ofstream(path) << R"({"tasks":{"seq":"SEQ"}})";

  auto result = GetStringTaskType(TypeOfTask::kUnknown, path);
  EXPECT_EQ(result, "unknown");

  std::filesystem::remove(path);
}

TEST(GetStringTaskTypeEdgeCases, ThrowsIfFileCannotBeOpened) {
  EXPECT_THROW(GetStringTaskType(TypeOfTask::kSEQ, "definitely_missing_file.json"), std::runtime_error);
}

TEST(GetStringTaskTypeEdgeCases, ThrowsIfJsonIsMalformed) {
  std::string path = (std::filesystem::temp_directory_path() / "bad_json.json").string();
  std::ofstream(path) << "{ this is not valid json ";
  EXPECT_THROW(GetStringTaskType(TypeOfTask::kSEQ, path), NlohmannJsonParseError);
  std::filesystem::remove(path);
}

TEST(GetStringTaskTypeEdgeCases, ThrowsIfJsonValueIsNull) {
  std::string path = (std::filesystem::temp_directory_path() / "null_value.json").string();
  std::ofstream(path) << R"({"tasks": { "seq": null }})";

  EXPECT_THROW(GetStringTaskType(TypeOfTask::kSEQ, path), NlohmannJsonTypeError);

  std::filesystem::remove(path);
}

TEST(GetStringTaskTypeEdgeCases, ReturnsUnknownIfEnumOutOfRange) {
  std::string path = (std::filesystem::temp_directory_path() / "ok.json").string();
  std::ofstream(path) << R"({"tasks":{"seq":"SEQ"}})";
  auto result = GetStringTaskType(TypeOfTask::kUnknown, path);
  EXPECT_EQ(result, "unknown");
  std::filesystem::remove(path);
}

TEST(GetStringTaskStatusTest, HandlesEnabledAndDisabled) {
  EXPECT_EQ(GetStringTaskStatus(StatusOfTask::kEnabled), "enabled");
  EXPECT_EQ(GetStringTaskStatus(StatusOfTask::kDisabled), "disabled");
}

class DummyTask : public Task<int, int> {
 public:
  using Task::Task;

 protected:
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

TEST(TaskTest, GetDynamicTypeReturnsCorrectEnum) {
  DummyTask task;
  task.SetTypeOfTask(TypeOfTask::kOMP);
  task.Validation();
  task.PreProcessing();
  task.Run();
  task.PostProcessing();
  EXPECT_EQ(task.GetDynamicTypeOfTask(), TypeOfTask::kOMP);
}

TEST(TaskTest, DestructorTerminatesIfWrongOrder) {
  DummyTask task;
  EXPECT_THROW(task.Run(), std::runtime_error);
}

namespace my {
namespace nested {
struct Type {};
}  // namespace nested

class Another {};
}  // namespace my

TEST(GetNamespaceTest, ExtractsNestedNamespaceCorrectly) {
  EXPECT_EQ(ppc::util::GetNamespace<my::nested::Type>(), "ppc::performance::my::nested");
}

TEST(GetNamespaceTest, ExtractsParentNamespaceCorrectly) {
  EXPECT_EQ(ppc::util::GetNamespace<my::Another>(), "ppc::performance::my");
}

TEST(GetNamespaceTest, ReturnsEmptyStringForGlobalNamespaceType) {
  EXPECT_EQ(ppc::util::GetNamespace<int>(), "");
}

TEST(PerfTest, PipelineRunAndTaskRun) {
  auto task_ptr = std::make_shared<DummyTask>();
  Perf<int, int> perf(task_ptr);

  PerfAttr attr;
  double time = 0.0;
  attr.num_running = 2;
  attr.current_timer = [&time]() {
    double t = time;
    time += 1.0;
    return t;
  };

  EXPECT_NO_THROW(perf.PipelineRun(attr));
  auto res_pipeline = perf.GetPerfResults();
  EXPECT_EQ(res_pipeline.type_of_running, PerfResults::TypeOfRunning::kPipeline);
  EXPECT_GT(res_pipeline.time_sec, 0.0);

  EXPECT_NO_THROW(perf.TaskRun(attr));
  auto res_taskrun = perf.GetPerfResults();
  EXPECT_EQ(res_taskrun.type_of_running, PerfResults::TypeOfRunning::kTaskRun);
  EXPECT_GT(res_taskrun.time_sec, 0.0);
}

TEST(PerfTest, PrintPerfStatisticThrowsOnNone) {
  {
    auto task_ptr = std::make_shared<DummyTask>();
    Perf<int, int> perf(task_ptr);
    EXPECT_THROW(perf.PrintPerfStatistic("test"), std::runtime_error);
  }
  EXPECT_TRUE(ppc::util::DestructorFailureFlag::Get());
  ppc::util::DestructorFailureFlag::Unset();
}

TEST(PerfTest, GetStringParamNameTest) {
  EXPECT_EQ(GetStringParamName(PerfResults::TypeOfRunning::kTaskRun), "task_run");
  EXPECT_EQ(GetStringParamName(PerfResults::TypeOfRunning::kPipeline), "pipeline");
  EXPECT_EQ(GetStringParamName(PerfResults::TypeOfRunning::kNone), "none");
}

TEST(TaskTest, DestructorInvalidPipelineOrderTerminatesPartialPipeline) {
  {
    struct BadTask : Task<int, int> {
     protected:
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
    } task;
    task.Validation();
  }
  EXPECT_TRUE(ppc::util::DestructorFailureFlag::Get());
  ppc::util::DestructorFailureFlag::Unset();
}

}  // namespace ppc::performance
