#pragma once

#include <gtest/gtest.h>

#include <memory>
#include <utility>

namespace ppc::runners {

/// @brief GTest event listener that checks for unread MPI messages after each test.
/// @note Used to detect unexpected inter-process communication leftovers.
class UnreadMessagesDetector : public ::testing::EmptyTestEventListener {
 public:
  UnreadMessagesDetector() = default;
  /// @brief Called by GTest after a test ends. Checks for unread messages.
  void OnTestEnd(const ::testing::TestInfo & /*test_info*/) override;

 private:
};

/// @brief GTest event listener that prints additional information on test failures in worker processes.
/// @details Includes MPI rank info in failure output for debugging.
class WorkerTestFailurePrinter : public ::testing::EmptyTestEventListener {
 public:
  /// @brief Constructs the listener with a base listener for delegation.
  /// @param base A shared pointer to another GTest event listener.
  explicit WorkerTestFailurePrinter(std::shared_ptr<::testing::TestEventListener> base) : base_(std::move(base)) {}
  /// @brief Called after a test ends. Passes call base listener and print failures with rank.
  void OnTestEnd(const ::testing::TestInfo &test_info) override;
  /// @brief Called when a test part fails. Prints MPI rank info along with the failure.
  void OnTestPartResult(const ::testing::TestPartResult &test_part_result) override;

 private:
  /// @brief Prints the MPI rank of the current process to stderr.
  static void PrintProcessRank();
  std::shared_ptr<::testing::TestEventListener> base_;
};

/// @brief Initializes the testing environment (e.g., MPI, logging).
/// @param argc Argument count.
/// @param argv Argument vector.
/// @return Exit code from RUN_ALL_TESTS or MPI error code if initialization/
///         finalization fails.
int Init(int argc, char **argv);

/// @brief Initializes the testing environment only for gtest.
/// @param argc Argument count.
/// @param argv Argument vector.
/// @return Exit code from RUN_ALL_TESTS.
int SimpleInit(int argc, char **argv);

}  // namespace ppc::runners
