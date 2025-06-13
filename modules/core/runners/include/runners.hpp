#pragma once

#include <gtest/gtest.h>

namespace ppc::core {

class UnreadMessagesDetector : public ::testing::EmptyTestEventListener {
 public:
  UnreadMessagesDetector() = default;
  void OnTestEnd(const ::testing::TestInfo& /*test_info*/) override;

 private:
};

class WorkerTestFailurePrinter : public ::testing::EmptyTestEventListener {
 public:
  explicit WorkerTestFailurePrinter(std::shared_ptr<::testing::TestEventListener> base) : base_(std::move(base)) {}
  void OnTestEnd(const ::testing::TestInfo& test_info) override;
  void OnTestPartResult(const ::testing::TestPartResult& test_part_result) override;

 private:
  static void PrintProcessRank();
  std::shared_ptr<::testing::TestEventListener> base_;
};

int Init(int argc, char** argv);

}  // namespace ppc::core
