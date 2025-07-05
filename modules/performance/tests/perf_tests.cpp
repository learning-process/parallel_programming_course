#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "performance/include/performance.hpp"
#include "task/include/task.hpp"
#include "util/include/util.hpp"

using namespace ppc::task;

namespace ppc::test {

template <typename InType, typename OutType>
class TestPerfTask : public ppc::task::Task<InType, OutType> {
 public:
  explicit TestPerfTask(const InType& in) { this->GetInput() = in; }

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
class FakePerfTask : public TestPerfTask<InType, OutType> {
 public:
  explicit FakePerfTask(const InType& in) : TestPerfTask<InType, OutType>(in) {}

  bool RunImpl() override {
    std::this_thread::sleep_for(std::chrono::seconds(11));
    return TestPerfTask<InType, OutType>::RunImpl();
  }
};

}  // namespace ppc::test

namespace ppc::performance {

namespace {

TEST(PerfTest, Pipeline_WithUint32Vector_CompletesWithinTimeLimit) {
  std::vector<uint32_t> in(2000, 1);

  auto test_task = std::make_shared<ppc::test::TestPerfTask<std::vector<uint32_t>, uint32_t>>(in);

  Perf<std::vector<uint32_t>, uint32_t> perf_analyzer(test_task);

  PerfAttr perf_attr;
  perf_analyzer.PipelineRun(perf_attr);

  perf_analyzer.PrintPerfStatistic("check_perf_pipeline");
  ASSERT_LE(perf_analyzer.GetPerfResults().time_sec, PerfResults::kMaxTime);
  EXPECT_EQ(test_task->GetOutput(), in.size());
}

TEST(PerfTest, Pipeline_WithFloatVector_CompletesWithinTimeLimit) {
  std::vector<float> in(2000, 1);

  auto test_task = std::make_shared<ppc::test::TestPerfTask<std::vector<float>, float>>(in);

  Perf<std::vector<float>, float> perf_analyzer(test_task);

  PerfAttr perf_attr;
  perf_analyzer.PipelineRun(perf_attr);

  perf_analyzer.PrintPerfStatistic("check_perf_pipeline_float");
  ASSERT_LE(perf_analyzer.GetPerfResults().time_sec, PerfResults::kMaxTime);
  EXPECT_EQ(test_task->GetOutput(), in.size());
}

TEST(PerfTest, Pipeline_WithSlowTask_ThrowsOnTimeExceeded) {
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

TEST(PerfTest, TaskRun_WithoutPriorExecution_ThrowsException) {
  std::vector<uint32_t> in(2000, 1);

  auto test_task = std::make_shared<ppc::test::TestPerfTask<std::vector<uint32_t>, uint32_t>>(in);

  Perf<std::vector<uint32_t>, uint32_t> perf_analyzer(test_task);

  ASSERT_ANY_THROW(perf_analyzer.PrintPerfStatistic("check_perf_task_exception"));

  PerfAttr perf_attr;
  perf_analyzer.TaskRun(perf_attr);
}

TEST(PerfTest, TaskRun_WithFloatVector_CompletesWithinTimeLimit) {
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
  PerfResults::TypeOfRunning input{};
  std::string expected_output;
  friend void PrintTo(const ParamTestCase& param, std::ostream* os) {
    *os << "{ input = " << static_cast<int>(param.input) << ", expected = " << param.expected_output << " }";
  }
};

class GetStringParamNameParamTest : public ::testing::TestWithParam<ParamTestCase> {};

TEST_P(GetStringParamNameParamTest, GetStringParamName_WithValidInput_ReturnsExpectedString) {
  const auto& param = GetParam();
  EXPECT_EQ(GetStringParamName(param.input), param.expected_output);
}

INSTANTIATE_TEST_SUITE_P(ParamTests, GetStringParamNameParamTest,
                         ::testing::Values(ParamTestCase{PerfResults::kTaskRun, "task_run"},
                                           ParamTestCase{PerfResults::kPipeline, "pipeline"},
                                           ParamTestCase{PerfResults::TypeOfRunning::kNone, "none"}),
                         [](const ::testing::TestParamInfo<ParamTestCase>& info) {
                           return info.param.expected_output;
                         });

struct TaskTypeTestCase {
  TypeOfTask type{};
  std::string expected;
  std::string label;
  friend void PrintTo(const TaskTypeTestCase& param, std::ostream* os) {
    *os << "{ type = " << static_cast<int>(param.type) << ", expected = " << param.expected
        << ", label = " << param.label << " }";
  }
};

class GetStringTaskTypeTest : public ::testing::TestWithParam<TaskTypeTestCase> {
 protected:
  std::string temp_path;

  void SetUp() override {
    temp_path = (std::filesystem::temp_directory_path() / "test_settings.json").string();
    auto j = ppc::util::InitJSONPtr();
    (*j)["tasks"]["all"] = "ALL";
    (*j)["tasks"]["stl"] = "STL";
    (*j)["tasks"]["omp"] = "OMP";
    (*j)["tasks"]["mpi"] = "MPI";
    (*j)["tasks"]["tbb"] = "TBB";
    (*j)["tasks"]["seq"] = "SEQ";

    std::ofstream(temp_path) << j->dump();
  }

  void TearDown() override { std::filesystem::remove(temp_path); }
};

TEST_P(GetStringTaskTypeTest, GetStringTaskType_WithValidTypeAndFile_ReturnsExpectedString) {
  const auto& param = GetParam();
  EXPECT_EQ(GetStringTaskType(param.type, temp_path), param.expected) << "Failed on: " << param.label;
}

INSTANTIATE_TEST_SUITE_P(AllTypeCases, GetStringTaskTypeTest,
                         ::testing::Values(TaskTypeTestCase{TypeOfTask::kALL, "all_ALL", "kALL"},
                                           TaskTypeTestCase{TypeOfTask::kSTL, "stl_STL", "kSTL"},
                                           TaskTypeTestCase{TypeOfTask::kOMP, "omp_OMP", "kOMP"},
                                           TaskTypeTestCase{TypeOfTask::kMPI, "mpi_MPI", "kMPI"},
                                           TaskTypeTestCase{TypeOfTask::kTBB, "tbb_TBB", "kTBB"},
                                           TaskTypeTestCase{TypeOfTask::kSEQ, "seq_SEQ", "kSEQ"}));

TEST(GetStringTaskTypeStandaloneTest, GetStringTaskType_WithMissingFile_ThrowsException) {
  std::string missing_path = "non_existent_settings.json";
  EXPECT_THROW(GetStringTaskType(TypeOfTask::kSEQ, missing_path), std::runtime_error);
}

TEST(GetStringTaskTypeStandaloneTest, GetStringTaskType_WithMissingFile_ExceptionContainsPath) {
  const std::string missing_path = "non_existent_settings.json";
  EXPECT_THROW(try { GetStringTaskType(TypeOfTask::kSEQ, missing_path); } catch (const std::runtime_error& e) {
    EXPECT_NE(std::string(e.what()).find(missing_path), std::string::npos);
    throw;
  },
               std::runtime_error);
}

TEST(GetStringTaskTypeStandaloneTest, GetStringTaskType_WithInvalidEnum_ReturnsUnknown) {
  std::string path = (std::filesystem::temp_directory_path() / "tmp_settings.json").string();
  std::ofstream(path) << R"({"tasks":{"seq":"SEQ"}})";

  auto result = GetStringTaskType(TypeOfTask::kUnknown, path);
  EXPECT_EQ(result, "unknown");

  std::filesystem::remove(path);
}

TEST(GetStringTaskTypeEdgeCases, GetStringTaskType_WithUnreadableFile_ThrowsException) {
  EXPECT_THROW(GetStringTaskType(TypeOfTask::kSEQ, "definitely_missing_file.json"), std::runtime_error);
}

TEST(GetStringTaskTypeEdgeCases, GetStringTaskType_WithMalformedJson_ThrowsException) {
  std::string path = (std::filesystem::temp_directory_path() / "bad_json.json").string();
  std::ofstream(path) << "{ this is not valid json ";
  EXPECT_THROW(GetStringTaskType(TypeOfTask::kSEQ, path), NlohmannJsonParseError);
  std::filesystem::remove(path);
}

TEST(GetStringTaskTypeEdgeCases, GetStringTaskType_WithNullJsonValue_ThrowsException) {
  std::string path = (std::filesystem::temp_directory_path() / "null_value.json").string();
  std::ofstream(path) << R"({"tasks": { "seq": null }})";

  EXPECT_THROW(GetStringTaskType(TypeOfTask::kSEQ, path), NlohmannJsonTypeError);

  std::filesystem::remove(path);
}

TEST(GetStringTaskTypeEdgeCases, GetStringTaskType_WithEnumOutOfRange_ReturnsUnknown) {
  std::string path = (std::filesystem::temp_directory_path() / "ok.json").string();
  std::ofstream(path) << R"({"tasks":{"seq":"SEQ"}})";
  auto result = GetStringTaskType(TypeOfTask::kUnknown, path);
  EXPECT_EQ(result, "unknown");
  std::filesystem::remove(path);
}

TEST(GetStringTaskStatusTest, GetStringTaskStatus_WithEnabledAndDisabled_ReturnsCorrectString) {
  EXPECT_EQ(GetStringTaskStatus(StatusOfTask::kEnabled), "enabled");
  EXPECT_EQ(GetStringTaskStatus(StatusOfTask::kDisabled), "disabled");
}

class DummyTask : public Task<int, int> {
 public:
  using Task::Task;
  bool ValidationImpl() override { return true; }
  bool PreProcessingImpl() override { return true; }
  bool RunImpl() override { return true; }
  bool PostProcessingImpl() override { return true; }
};

TEST(TaskTest, GetDynamicType_WithValidTask_ReturnsCorrectEnum) {
  DummyTask task;
  task.SetTypeOfTask(TypeOfTask::kOMP);
  task.Validation();
  task.PreProcessing();
  task.Run();
  task.PostProcessing();
  EXPECT_EQ(task.GetDynamicTypeOfTask(), TypeOfTask::kOMP);
}

TEST(TaskTest, Destructor_WithWrongOrder_TerminatesGracefully) {
  {
    DummyTask task;
    EXPECT_THROW(task.Run(), std::runtime_error);
    // This task doesn't cause destructor failure - just execution order error
  }

  // Create a new task to complete the lifecycle properly
  DummyTask task2;
  task2.Validation();
  task2.PreProcessing();
  task2.Run();
  task2.PostProcessing();
}

namespace my {
namespace nested {
struct Type {};
}  // namespace nested

class Another {};
}  // namespace my

template <typename T>
class GetNamespaceTest : public ::testing::Test {};

using TestTypes = ::testing::Types<my::nested::Type, my::Another, int>;

TYPED_TEST_SUITE(GetNamespaceTest, TestTypes);

TYPED_TEST(GetNamespaceTest, ExtractsNamespaceCorrectly) {
  std::string k_ns = ppc::util::GetNamespace<TypeParam>();

  if constexpr (std::is_same_v<TypeParam, my::nested::Type>) {
    // Different compilers may represent anonymous namespaces differently
    // Check for essential parts: ppc::performance, my, and nested
    EXPECT_TRUE(k_ns.find("ppc::performance") != std::string::npos);
    EXPECT_TRUE(k_ns.find("my") != std::string::npos);
    EXPECT_TRUE(k_ns.find("nested") != std::string::npos);
  } else if constexpr (std::is_same_v<TypeParam, my::Another>) {
    // Check for essential parts: ppc::performance and my
    EXPECT_TRUE(k_ns.find("ppc::performance") != std::string::npos);
    EXPECT_TRUE(k_ns.find("my") != std::string::npos);
    EXPECT_TRUE(k_ns.find("nested") == std::string::npos);  // Should not contain nested
  } else if constexpr (std::is_same_v<TypeParam, int>) {
    EXPECT_EQ(k_ns, "");
  } else {
    FAIL() << "Unhandled type in test";
  }
}

TEST(PerfTest, PipelineRunAndTaskRun_WithValidTask_ExecutesSuccessfully) {
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
  EXPECT_EQ(res_pipeline.type_of_running, PerfResults::kPipeline);
  EXPECT_GT(res_pipeline.time_sec, 0.0);

  EXPECT_NO_THROW(perf.TaskRun(attr));
  auto res_taskrun = perf.GetPerfResults();
  EXPECT_EQ(res_taskrun.type_of_running, PerfResults::kTaskRun);
  EXPECT_GT(res_taskrun.time_sec, 0.0);
}

TEST(PerfTest, PrintPerfStatistic_WithNoneType_ThrowsException) {
  {
    auto task_ptr = std::make_shared<DummyTask>();
    Perf<int, int> perf(task_ptr);
    EXPECT_THROW(perf.PrintPerfStatistic("test"), std::runtime_error);
  }
  EXPECT_TRUE(ppc::util::DestructorFailureFlag::Get());
  ppc::util::DestructorFailureFlag::Unset();
}

TEST(PerfTest, GetStringParamName_WithValidParameters_ReturnsCorrectString) {
  EXPECT_EQ(GetStringParamName(PerfResults::kTaskRun), "task_run");
  EXPECT_EQ(GetStringParamName(PerfResults::kPipeline), "pipeline");
  EXPECT_EQ(GetStringParamName(PerfResults::kNone), "none");
}

TEST(PerfTest, DefaultTimer_WhenCalled_ReturnsNegativeOne) { EXPECT_EQ(DefaultTimer(), -1.0); }

TEST(PerfTest, PerfAttr_WithDefaultConstructor_HasCorrectDefaultValues) {
  PerfAttr attr;
  EXPECT_EQ(attr.num_running, 5U);
  EXPECT_EQ(attr.current_timer(), -1.0);
}

TEST(PerfTest, PerfResults_WithDefaultConstructor_HasCorrectDefaultValues) {
  PerfResults results;
  EXPECT_EQ(results.time_sec, 0.0);
  EXPECT_EQ(results.type_of_running, PerfResults::kNone);
  EXPECT_EQ(PerfResults::kMaxTime, 10.0);
}

TEST(PerfTest, PerfResults_WithEnumValues_HasCorrectValues) {
  EXPECT_EQ(static_cast<uint8_t>(PerfResults::kPipeline), 0);
  EXPECT_EQ(static_cast<uint8_t>(PerfResults::kTaskRun), 1);
  EXPECT_EQ(static_cast<uint8_t>(PerfResults::kNone), 2);
}

TEST(PerfTest, PerfConstructor_WithTask_SetsTaskStateCorrectly) {
  auto task_ptr = std::make_shared<DummyTask>();
  Perf<int, int> perf(task_ptr);

  EXPECT_EQ(task_ptr->GetStateOfTesting(), ppc::task::StateOfTesting::kPerf);

  // Complete the task lifecycle to avoid destructor issues
  task_ptr->Validation();
  task_ptr->PreProcessing();
  task_ptr->Run();
  task_ptr->PostProcessing();
}

TEST(PerfTest, GetPerfResults_AfterExecution_ReturnsCorrectResults) {
  auto task_ptr = std::make_shared<DummyTask>();
  Perf<int, int> perf(task_ptr);

  // Initially should be default values
  auto initial_results = perf.GetPerfResults();
  EXPECT_EQ(initial_results.time_sec, 0.0);
  EXPECT_EQ(initial_results.type_of_running, PerfResults::kNone);

  PerfAttr attr;
  double time = 0.0;
  attr.current_timer = [&time]() {
    double t = time;
    time += 0.5;
    return t;
  };

  perf.PipelineRun(attr);
  auto pipeline_results = perf.GetPerfResults();
  EXPECT_EQ(pipeline_results.type_of_running, PerfResults::kPipeline);
  EXPECT_GT(pipeline_results.time_sec, 0.0);

  perf.TaskRun(attr);
  auto taskrun_results = perf.GetPerfResults();
  EXPECT_EQ(taskrun_results.type_of_running, PerfResults::kTaskRun);
  EXPECT_GT(taskrun_results.time_sec, 0.0);
}

TEST(PerfTest, CommonRun_WithMultipleExecutions_CalculatesAverageTime) {
  auto task_ptr = std::make_shared<DummyTask>();
  Perf<int, int> perf(task_ptr);

  PerfAttr attr;
  int call_count = 0;
  attr.num_running = 3;
  attr.current_timer = [&call_count]() {
    if (call_count == 0) {
      call_count++;
      return 0.0;  // Start time
    } else {
      return 3.0;  // End time after 3 runs
    }
  };

  perf.PipelineRun(attr);
  auto results = perf.GetPerfResults();

  // Total time should be 3 seconds, average should be 1 second (3.0-0.0) / 3
  EXPECT_DOUBLE_EQ(results.time_sec, 1.0);
}

TEST(PerfTest, PrintPerfStatistic_WithPipelineExecution_OutputsCorrectStatistics) {
  auto task_ptr = std::make_shared<DummyTask>();
  Perf<int, int> perf(task_ptr);

  PerfAttr attr;
  double time = 0.0;
  attr.current_timer = [&time]() {
    double t = time;
    time += 0.1;
    return t;
  };

  perf.PipelineRun(attr);

  testing::internal::CaptureStdout();
  perf.PrintPerfStatistic("test_pipeline");
  std::string output = testing::internal::GetCapturedStdout();

  EXPECT_NE(output.find("test_pipeline:pipeline:"), std::string::npos);
  EXPECT_NE(output.find("0.0200000000"), std::string::npos);  // 0.1/5 = 0.02
}

TEST(PerfTest, PrintPerfStatistic_WithTaskRunExecution_OutputsCorrectStatistics) {
  auto task_ptr = std::make_shared<DummyTask>();
  Perf<int, int> perf(task_ptr);

  PerfAttr attr;
  double time = 0.0;
  attr.current_timer = [&time]() {
    double t = time;
    time += 0.25;
    return t;
  };

  perf.TaskRun(attr);

  testing::internal::CaptureStdout();
  perf.PrintPerfStatistic("test_taskrun");
  std::string output = testing::internal::GetCapturedStdout();

  EXPECT_NE(output.find("test_taskrun:task_run:"), std::string::npos);
}

TEST(PerfTest, PrintPerfStatistic_WithTimeExceeded_ThrowsException) {
  auto task_ptr = std::make_shared<DummyTask>();
  Perf<int, int> perf(task_ptr);

  PerfAttr attr;
  double time = 0.0;
  attr.current_timer = [&time]() {
    double t = time;
    time += 55.0;  // Exceeds kMaxTime (10.0)
    return t;
  };

  perf.PipelineRun(attr);

  testing::internal::CaptureStdout();
  try {
    perf.PrintPerfStatistic("test_exceed_time");
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error& e) {
    std::string error_msg = e.what();
    EXPECT_NE(error_msg.find("Task execute time need to be"), std::string::npos);
    EXPECT_NE(error_msg.find("time < 10"), std::string::npos);
    EXPECT_NE(error_msg.find("Original time in secs: 11"), std::string::npos);
  }
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_NE(output.find("test_exceed_time:pipeline:-1.0000000000"), std::string::npos);
}

TEST(PerfTest, TaskRun_WithTiming_CompletesPipelineCorrectly) {
  int validation_count = 0;
  int preprocessing_count = 0;
  int run_count = 0;
  int postprocessing_count = 0;

  // Create a custom task that counts method calls
  class CountingTask : public Task<int, int> {
   public:
    int* validation_count_;
    int* preprocessing_count_;
    int* run_count_;
    int* postprocessing_count_;

    CountingTask(int* vc, int* pc, int* rc, int* ppc)
        : validation_count_(vc), preprocessing_count_(pc), run_count_(rc), postprocessing_count_(ppc) {}

    bool ValidationImpl() override {
      (*validation_count_)++;
      return true;
    }

    bool PreProcessingImpl() override {
      (*preprocessing_count_)++;
      return true;
    }

    bool RunImpl() override {
      (*run_count_)++;
      return true;
    }

    bool PostProcessingImpl() override {
      (*postprocessing_count_)++;
      return true;
    }
  };

  auto counting_task =
      std::make_shared<CountingTask>(&validation_count, &preprocessing_count, &run_count, &postprocessing_count);
  Perf<int, int> counting_perf(counting_task);

  PerfAttr attr;
  attr.num_running = 1;

  counting_perf.TaskRun(attr);

  // TaskRun should call:
  // 1. Validation + PreProcessing + Run (num_running times) + PostProcessing
  // 2. Validation + PreProcessing + Run + PostProcessing (one additional complete cycle)
  EXPECT_EQ(validation_count, 2);      // Called twice
  EXPECT_EQ(preprocessing_count, 2);   // Called twice
  EXPECT_EQ(run_count, 2);             // Called twice (once in timing, once in final cycle)
  EXPECT_EQ(postprocessing_count, 2);  // Called twice
}

namespace test_namespace {}  // namespace test_namespace

TEST(PerfTest, Template_WithDifferentTypes_InstantiatesCorrectly) {
  // Test that the Perf template can be instantiated with different types
  auto int_task = std::make_shared<DummyTask>();
  Perf<int, int> int_perf(int_task);

  auto vector_task = std::make_shared<ppc::test::TestPerfTask<std::vector<int>, int>>(std::vector<int>{1, 2, 3});
  Perf<std::vector<int>, int> vector_perf(vector_task);

  PerfAttr attr;

  EXPECT_NO_THROW(int_perf.PipelineRun(attr));
  EXPECT_NO_THROW(vector_perf.PipelineRun(attr));

  EXPECT_EQ(int_perf.GetPerfResults().type_of_running, PerfResults::kPipeline);
  EXPECT_EQ(vector_perf.GetPerfResults().type_of_running, PerfResults::kPipeline);
}

TEST(PerfTest, PerfAttr_WithCustomValues_SetsValuesCorrectly) {
  PerfAttr attr;
  attr.num_running = 10;
  attr.current_timer = []() { return 42.0; };

  EXPECT_EQ(attr.num_running, 10U);
  EXPECT_EQ(attr.current_timer(), 42.0);
}

TEST(TaskTest, Destructor_WithInvalidPipelineOrderAndPartialExecution_TerminatesGracefully) {
  {
    struct BadTask : Task<int, int> {
      bool ValidationImpl() override { return true; }
      bool PreProcessingImpl() override { return true; }
      bool RunImpl() override { return true; }
      bool PostProcessingImpl() override { return true; }
    } task;
    task.Validation();
  }
  EXPECT_TRUE(ppc::util::DestructorFailureFlag::Get());
  ppc::util::DestructorFailureFlag::Unset();
}

}  // namespace

}  // namespace ppc::performance
