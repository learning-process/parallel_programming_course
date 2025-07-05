#include <gtest/gtest.h>

#include <libenvpp/detail/environment.hpp>
#include <libenvpp/detail/testing.hpp>
#include <string>

#include "util/include/util.hpp"

class UtilAdditionalTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // No need to manually clear environment variables with libenvpp
  }

  void TearDown() override {
    // No need to manually clear environment variables with libenvpp
  }
};

// Tests for GetAbsoluteTaskPath - understand it creates full absolute paths
TEST_F(UtilAdditionalTest, GetAbsoluteTaskPath_ValidPaths) {
  std::string result = ppc::util::GetAbsoluteTaskPath("task1", "src/main.cpp");
  // The function adds PPC_PATH_TO_PROJECT/tasks/task1/data/src/main.cpp
  // Use platform-agnostic path checking - simplified to reduce complexity
  EXPECT_FALSE(result.empty());
  EXPECT_TRUE(result.find("tasks") != std::string::npos);
  EXPECT_TRUE(result.find("task1") != std::string::npos);
}

TEST_F(UtilAdditionalTest, GetAbsoluteTaskPath_EmptyIdPath) {
  std::string result = ppc::util::GetAbsoluteTaskPath("", "src/main.cpp");
  // The function adds PPC_PATH_TO_PROJECT/tasks/data/src/main.cpp
  EXPECT_TRUE(result.find("tasks") != std::string::npos);
  EXPECT_TRUE(result.find("data") != std::string::npos);
  EXPECT_TRUE(result.find("main.cpp") != std::string::npos);
}

TEST_F(UtilAdditionalTest, GetAbsoluteTaskPath_EmptyRelativePath) {
  std::string result = ppc::util::GetAbsoluteTaskPath("task1", "");
  // The function adds PPC_PATH_TO_PROJECT/tasks/task1/data/
  EXPECT_TRUE(result.find("tasks") != std::string::npos);
  EXPECT_TRUE(result.find("task1") != std::string::npos);
  EXPECT_TRUE(result.find("data") != std::string::npos);
}

TEST_F(UtilAdditionalTest, GetAbsoluteTaskPath_BothEmpty) {
  std::string result = ppc::util::GetAbsoluteTaskPath("", "");
  // The function adds PPC_PATH_TO_PROJECT/tasks/data/
  EXPECT_TRUE(result.find("tasks") != std::string::npos);
  EXPECT_TRUE(result.find("data") != std::string::npos);
}

// Tests for GetNumThreads - returns 1 by default if no env var, otherwise returns env var value
TEST_F(UtilAdditionalTest, GetNumThreads_EnvironmentVariableNotSet) {
  // Ensure PPC_NUM_THREADS is not set in the system environment
  env::detail::delete_environment_variable("PPC_NUM_THREADS");

  // Create a scoped environment with no PPC_NUM_THREADS set
  env::scoped_test_environment test_env({});

  int result = ppc::util::GetNumThreads();
  EXPECT_EQ(result, 1);  // Default value when no environment variable is set
}

TEST_F(UtilAdditionalTest, GetNumThreads_EnvironmentVariableSet) {
  // Create a scoped environment with PPC_NUM_THREADS=4
  env::scoped_test_environment test_env("PPC_NUM_THREADS", "4");

  int result = ppc::util::GetNumThreads();
  EXPECT_EQ(result, 4);
}

TEST_F(UtilAdditionalTest, GetNumThreads_EnvironmentVariableZero) {
  env::scoped_test_environment test_env("PPC_NUM_THREADS", "0");

  int result = ppc::util::GetNumThreads();
  EXPECT_EQ(result, 0);
}

TEST_F(UtilAdditionalTest, GetNumThreads_EnvironmentVariableNegative) {
  env::scoped_test_environment test_env("PPC_NUM_THREADS", "-1");

  int result = ppc::util::GetNumThreads();
  EXPECT_EQ(result, -1);
}

TEST_F(UtilAdditionalTest, GetNumThreads_EnvironmentVariableInvalid) {
  env::scoped_test_environment test_env("PPC_NUM_THREADS", "invalid");

  int result = ppc::util::GetNumThreads();
  EXPECT_EQ(result, 1);  // Returns default when parsing fails
}

// Tests for IsUnderMpirun - checks specific environment variables from the kMpiEnvVars array
TEST_F(UtilAdditionalTest, IsUnderMpirun_NoEnvironmentVariables) {
  // Create an empty environment to ensure no MPI vars are set
  env::scoped_test_environment test_env({});

  bool result = ppc::util::IsUnderMpirun();
  EXPECT_FALSE(result);
}

TEST_F(UtilAdditionalTest, IsUnderMpirun_OMPI_COMM_WORLD_SIZE) {
  env::scoped_test_environment test_env("OMPI_COMM_WORLD_SIZE", "4");

  bool result = ppc::util::IsUnderMpirun();
  EXPECT_TRUE(result);
}

TEST_F(UtilAdditionalTest, IsUnderMpirun_OMPI_UNIVERSE_SIZE) {
  env::scoped_test_environment test_env("OMPI_UNIVERSE_SIZE", "8");

  bool result = ppc::util::IsUnderMpirun();
  EXPECT_TRUE(result);
}

TEST_F(UtilAdditionalTest, IsUnderMpirun_PMI_SIZE) {
  env::scoped_test_environment test_env("PMI_SIZE", "2");

  bool result = ppc::util::IsUnderMpirun();
  EXPECT_TRUE(result);
}

TEST_F(UtilAdditionalTest, IsUnderMpirun_PMI_RANK) {
  env::scoped_test_environment test_env("PMI_RANK", "0");

  bool result = ppc::util::IsUnderMpirun();
  EXPECT_TRUE(result);
}

TEST_F(UtilAdditionalTest, IsUnderMpirun_PMI_FD) {
  env::scoped_test_environment test_env("PMI_FD", "3");

  bool result = ppc::util::IsUnderMpirun();
  EXPECT_TRUE(result);
}

TEST_F(UtilAdditionalTest, IsUnderMpirun_HYDRA_CONTROL_FD) {
  env::scoped_test_environment test_env("HYDRA_CONTROL_FD", "4");

  bool result = ppc::util::IsUnderMpirun();
  EXPECT_TRUE(result);
}

TEST_F(UtilAdditionalTest, IsUnderMpirun_PMIX_RANK) {
  env::scoped_test_environment test_env("PMIX_RANK", "1");

  bool result = ppc::util::IsUnderMpirun();
  EXPECT_TRUE(result);
}

TEST_F(UtilAdditionalTest, IsUnderMpirun_SLURM_PROCID) {
  env::scoped_test_environment test_env("SLURM_PROCID", "0");

  bool result = ppc::util::IsUnderMpirun();
  EXPECT_TRUE(result);
}

TEST_F(UtilAdditionalTest, IsUnderMpirun_MSMPI_RANK) {
  env::scoped_test_environment test_env("MSMPI_RANK", "2");

  bool result = ppc::util::IsUnderMpirun();
  EXPECT_TRUE(result);
}

TEST_F(UtilAdditionalTest, IsUnderMpirun_MSMPI_LOCALRANK) {
  env::scoped_test_environment test_env("MSMPI_LOCALRANK", "0");

  bool result = ppc::util::IsUnderMpirun();
  EXPECT_TRUE(result);
}

TEST_F(UtilAdditionalTest, IsUnderMpirun_MultipleEnvironmentVariables) {
  // Test with multiple MPI environment variables set
  env::scoped_test_environment test_env({{"OMPI_COMM_WORLD_SIZE", "4"}, {"PMI_SIZE", "4"}, {"SLURM_PROCID", "0"}});

  bool result = ppc::util::IsUnderMpirun();
  EXPECT_TRUE(result);
}

TEST_F(UtilAdditionalTest, IsUnderMpirun_EmptyEnvironmentVariable) {
  // Test with empty value - behavior is implementation-dependent
  env::scoped_test_environment test_env("OMPI_COMM_WORLD_SIZE", "");

  bool result = ppc::util::IsUnderMpirun();
  // Empty values may or may not be detected as "set" depending on implementation
  // verify the function doesn't crash
  (void)result;  // Suppress unused variable warning
}
