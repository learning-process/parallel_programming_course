#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string_view>
#include <vector>

#include "core/performance/include/performance.hpp"
#include "core/performance/tests/test_task.hpp"
#include "core/task/include/task.hpp"
#include "core/util/include/util.hpp"

TEST(perf_tests, check_perf_pipeline) {
  std::vector<uint32_t> in(2000, 1);

  auto test_task = std::make_shared<ppc::test::perf::TestTask<std::vector<uint32_t>, uint32_t>>(in);

  ppc::core::Perf<std::vector<uint32_t>, uint32_t> perf_analyzer(test_task);

  ppc::core::PerfAttr perf_attr;
  perf_analyzer.PipelineRun(perf_attr);

  perf_analyzer.PrintPerfStatistic("check_perf_pipeline");
  ASSERT_LE(perf_analyzer.GetPerfResults().time_sec, ppc::core::PerfResults::kMaxTime);
  EXPECT_EQ(test_task->GetOutput(), in.size());
}

TEST(perf_tests, check_perf_pipeline_float) {
  std::vector<float> in(2000, 1);

  auto test_task = std::make_shared<ppc::test::perf::TestTask<std::vector<float>, float>>(in);

  ppc::core::Perf<std::vector<float>, float> perf_analyzer(test_task);

  ppc::core::PerfAttr perf_attr;
  perf_analyzer.PipelineRun(perf_attr);

  perf_analyzer.PrintPerfStatistic("check_perf_pipeline_float");
  ASSERT_LE(perf_analyzer.GetPerfResults().time_sec, ppc::core::PerfResults::kMaxTime);
  EXPECT_EQ(test_task->GetOutput(), in.size());
}

TEST(perf_tests, check_perf_pipeline_uint8_t_slow_test) {
  std::vector<uint8_t> in(128, 1);

  auto test_task = std::make_shared<ppc::test::perf::FakePerfTask<std::vector<uint8_t>, uint8_t>>(in);

  ppc::core::Perf<std::vector<uint8_t>, uint8_t> perf_analyzer(test_task);

  ppc::core::PerfAttr perf_attr;
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

TEST(perf_tests, check_perf_task_exception) {
  std::vector<uint32_t> in(2000, 1);

  auto test_task = std::make_shared<ppc::test::perf::TestTask<std::vector<uint32_t>, uint32_t>>(in);

  ppc::core::Perf<std::vector<uint32_t>, uint32_t> perf_analyzer(test_task);

  ASSERT_ANY_THROW(perf_analyzer.PrintPerfStatistic("check_perf_task_exception"));

  ppc::core::PerfAttr perf_attr;
  perf_analyzer.TaskRun(perf_attr);
}

TEST(perf_tests, check_perf_task_float) {
  std::vector<float> in(2000, 1);

  auto test_task = std::make_shared<ppc::test::perf::TestTask<std::vector<float>, float>>(in);

  ppc::core::Perf<std::vector<float>, float> perf_analyzer(test_task);

  ppc::core::PerfAttr perf_attr;
  perf_analyzer.TaskRun(perf_attr);

  perf_analyzer.PrintPerfStatistic("check_perf_task_float");
  ASSERT_LE(perf_analyzer.GetPerfResults().time_sec, ppc::core::PerfResults::kMaxTime);
  EXPECT_EQ(test_task->GetOutput(), in.size());
}

struct ParamTestCase {
  ppc::core::PerfResults::TypeOfRunning input;
  std::string expected_output;
  friend void PrintTo(const ParamTestCase& param, std::ostream* os) {
    *os << "{ input = " << static_cast<int>(param.input) << ", expected = " << param.expected_output << " }";
  }
};

class GetStringParamNameParamTest : public ::testing::TestWithParam<ParamTestCase> {};

TEST_P(GetStringParamNameParamTest, ReturnsExpectedString) {
  const auto& param = GetParam();
  EXPECT_EQ(ppc::core::GetStringParamName(param.input), param.expected_output);
}

INSTANTIATE_TEST_SUITE_P(ParamTests, GetStringParamNameParamTest,
                         ::testing::Values(ParamTestCase{ppc::core::PerfResults::kTaskRun, "task_run"},
                                           ParamTestCase{ppc::core::PerfResults::kPipeline, "pipeline"},
                                           ParamTestCase{ppc::core::PerfResults::TypeOfRunning::kNone, "none"}),
                         [](const ::testing::TestParamInfo<ParamTestCase>& info) {
                           return info.param.expected_output;
                         });

struct TaskTypeTestCase {
  ppc::core::TypeOfTask type;
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

    std::ofstream(temp_path) << (*j).dump();
  }

  void TearDown() override { std::filesystem::remove(temp_path); }
};

TEST_P(GetStringTaskTypeTest, ReturnsExpectedString) {
  const auto& param = GetParam();
  EXPECT_EQ(GetStringTaskType(param.type, temp_path), param.expected) << "Failed on: " << param.label;
}

INSTANTIATE_TEST_SUITE_P(AllTypeCases, GetStringTaskTypeTest,
                         ::testing::Values(TaskTypeTestCase{ppc::core::TypeOfTask::kALL, "all_ALL", "kALL"},
                                           TaskTypeTestCase{ppc::core::TypeOfTask::kSTL, "stl_STL", "kSTL"},
                                           TaskTypeTestCase{ppc::core::TypeOfTask::kOMP, "omp_OMP", "kOMP"},
                                           TaskTypeTestCase{ppc::core::TypeOfTask::kMPI, "mpi_MPI", "kMPI"},
                                           TaskTypeTestCase{ppc::core::TypeOfTask::kTBB, "tbb_TBB", "kTBB"},
                                           TaskTypeTestCase{ppc::core::TypeOfTask::kSEQ, "seq_SEQ", "kSEQ"}));

TEST(GetStringTaskTypeStandaloneTest, ThrowsIfFileMissing) {
  std::string missing_path = "non_existent_settings.json";
  EXPECT_THROW(GetStringTaskType(ppc::core::TypeOfTask::kSEQ, missing_path), std::runtime_error);
}

TEST(GetStringTaskTypeStandaloneTest, ExceptionMessageContainsPath) {
  const std::string missing_path = "non_existent_settings.json";
  EXPECT_THROW(
      try { GetStringTaskType(ppc::core::TypeOfTask::kSEQ, missing_path); } catch (const std::runtime_error& e) {
        EXPECT_NE(std::string(e.what()).find(missing_path), std::string::npos);
        throw;
      },
      std::runtime_error);
}

TEST(GetStringTaskTypeStandaloneTest, ReturnsUnknownForInvalidEnum) {
  std::string path = (std::filesystem::temp_directory_path() / "tmp_settings.json").string();
  std::ofstream(path) << R"({"tasks":{"seq":"SEQ"}})";

  auto result = GetStringTaskType(ppc::core::TypeOfTask::kUnknown, path);
  EXPECT_EQ(result, "unknown");

  std::filesystem::remove(path);
}

TEST(GetStringTaskTypeEdgeCases, ThrowsIfFileCannotBeOpened) {
  EXPECT_THROW(GetStringTaskType(ppc::core::TypeOfTask::kSEQ, "definitely_missing_file.json"), std::runtime_error);
}

TEST(GetStringTaskTypeEdgeCases, ThrowsIfJsonIsMalformed) {
  std::string path = (std::filesystem::temp_directory_path() / "bad_json.json").string();
  std::ofstream(path) << "{ this is not valid json ";
  EXPECT_THROW(GetStringTaskType(ppc::core::TypeOfTask::kSEQ, path), NlohmannJsonParseError);
  std::filesystem::remove(path);
}

TEST(GetStringTaskTypeEdgeCases, ThrowsIfJsonValueIsNull) {
  std::string path = (std::filesystem::temp_directory_path() / "null_value.json").string();
  std::ofstream(path) << R"({"tasks": { "seq": null }})";

  EXPECT_THROW(GetStringTaskType(ppc::core::TypeOfTask::kSEQ, path), NlohmannJsonTypeError);

  std::filesystem::remove(path);
}

TEST(GetStringTaskTypeEdgeCases, ReturnsUnknownIfEnumOutOfRange) {
  std::string path = (std::filesystem::temp_directory_path() / "ok.json").string();
  std::ofstream(path) << R"({"tasks":{"seq":"SEQ"}})";
  auto result = GetStringTaskType(ppc::core::TypeOfTask::kUnknown, path);
  EXPECT_EQ(result, "unknown");
  std::filesystem::remove(path);
}

TEST(GetStringTaskStatusTest, HandlesEnabledAndDisabled) {
  EXPECT_EQ(GetStringTaskStatus(ppc::core::StatusOfTask::kEnabled), "enabled");
  EXPECT_EQ(GetStringTaskStatus(ppc::core::StatusOfTask::kDisabled), "disabled");
}

class DummyTask : public ppc::core::Task<int, int> {
 public:
  using Task::Task;
  bool ValidationImpl() override { return true; }
  bool PreProcessingImpl() override { return true; }
  bool RunImpl() override { return true; }
  bool PostProcessingImpl() override { return true; }
};

TEST(TaskTest, GetDynamicTypeReturnsCorrectEnum) {
  DummyTask task;
  task.SetTypeOfTask(ppc::core::TypeOfTask::kOMP);
  task.Validation();
  task.PreProcessing();
  task.Run();
  task.PostProcessing();
  EXPECT_EQ(task.GetDynamicTypeOfTask(), ppc::core::TypeOfTask::kOMP);
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

template <typename T>
class GetNamespaceTest : public ::testing::Test {};

using TestTypes = ::testing::Types<my::nested::Type, my::Another, int>;

TYPED_TEST_SUITE(GetNamespaceTest, TestTypes);

TYPED_TEST(GetNamespaceTest, ExtractsNamespaceCorrectly) {
  std::string k_ns = ppc::util::GetNamespace<TypeParam>();

  if constexpr (std::is_same_v<TypeParam, my::nested::Type>) {
    EXPECT_EQ(k_ns, "my::nested");
  } else if constexpr (std::is_same_v<TypeParam, my::Another>) {
    EXPECT_EQ(k_ns, "my");
  } else if constexpr (std::is_same_v<TypeParam, int>) {
    EXPECT_EQ(k_ns, "");
  } else {
    FAIL() << "Unhandled type in test";
  }
}

TEST(PerfTest, PipelineRunAndTaskRun) {
  auto task_ptr = std::make_shared<DummyTask>();
  ppc::core::Perf<int, int> perf(task_ptr);

  ppc::core::PerfAttr attr;
  double time = 0.0;
  attr.num_running = 2;
  attr.current_timer = [&time]() {
    double t = time;
    time += 1.0;
    return t;
  };

  EXPECT_NO_THROW(perf.PipelineRun(attr));
  auto res_pipeline = perf.GetPerfResults();
  EXPECT_EQ(res_pipeline.type_of_running, ppc::core::PerfResults::kPipeline);
  EXPECT_GT(res_pipeline.time_sec, 0.0);

  EXPECT_NO_THROW(perf.TaskRun(attr));
  auto res_taskrun = perf.GetPerfResults();
  EXPECT_EQ(res_taskrun.type_of_running, ppc::core::PerfResults::kTaskRun);
  EXPECT_GT(res_taskrun.time_sec, 0.0);
}

TEST(PerfTest, PrintPerfStatisticThrowsOnNone) {
  testing::FLAGS_gtest_death_test_style = "threadsafe";
  auto test_func = [&] {
    auto task_ptr = std::make_shared<DummyTask>();
    ppc::core::Perf<int, int> perf(task_ptr);
    EXPECT_THROW(perf.PrintPerfStatistic("test"), std::runtime_error);
  };
  ASSERT_DEATH_IF_SUPPORTED({ test_func(); }, "");
}

TEST(PerfTest, GetStringParamNameTest) {
  EXPECT_EQ(GetStringParamName(ppc::core::PerfResults::kTaskRun), "task_run");
  EXPECT_EQ(GetStringParamName(ppc::core::PerfResults::kPipeline), "pipeline");
  EXPECT_EQ(GetStringParamName(ppc::core::PerfResults::kNone), "none");
}

TEST(TaskTest, Destructor_InvalidPipelineOrderTerminates_PartialPipeline) {
  testing::FLAGS_gtest_death_test_style = "threadsafe";
  auto test_func = [&] {
    struct BadTask : ppc::core::Task<int, int> {
      bool ValidationImpl() override { return true; }
      bool PreProcessingImpl() override { return true; }
      bool RunImpl() override { return true; }
      bool PostProcessingImpl() override { return true; }
    } task;
    task.Validation();
  };
  ASSERT_DEATH_IF_SUPPORTED({ test_func(); }, "ORDER OF FUNCTIONS IS NOT RIGHT");
}
