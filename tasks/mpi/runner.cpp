#include <gtest/gtest.h>

#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <utility>

class UnreadMessagesDetector : public ::testing::EmptyTestEventListener {
 public:
  UnreadMessagesDetector(boost::mpi::communicator com) : com_(std::move(com)) {}

  void OnTestEnd(const ::testing::TestInfo& test_info) override {
    com_.barrier();
    if (const auto msg = com_.iprobe(boost::mpi::any_source, boost::mpi::any_tag)) {
      fprintf(
          stderr,
          "[  PROCESS %d  ] [  FAILED  ] %s.%s: MPI message queue has an unread message from process %d with tag %d\n",
          com_.rank(), test_info.test_suite_name(), test_info.name(), msg->source(), msg->tag());
      exit(2);
    }
    com_.barrier();
  }

 private:
  boost::mpi::communicator com_;
};

class WorkerTestFailurePrinter : public ::testing::EmptyTestEventListener {
 public:
  WorkerTestFailurePrinter(std::shared_ptr<::testing::TestEventListener> base, boost::mpi::communicator com)
      : base_(std::move(base)), com_(std::move(com)) {}

  void OnTestEnd(const ::testing::TestInfo& test_info) override {
    if (test_info.result()->Passed()) {
      return;
    }
    PrintProcessRank();
    base_->OnTestEnd(test_info);
  }

  void OnTestPartResult(const ::testing::TestPartResult& test_part_result) override {
    if (test_part_result.passed() || test_part_result.skipped()) {
      return;
    }
    PrintProcessRank();
    base_->OnTestPartResult(test_part_result);
  }

 private:
  void PrintProcessRank() const { printf(" [  PROCESS %d  ] ", com_.rank()); }

  std::shared_ptr<::testing::TestEventListener> base_;
  boost::mpi::communicator com_;
};

int main(int argc, char** argv) {
  boost::mpi::environment env(argc, argv);
  boost::mpi::communicator world;

  ::testing::InitGoogleTest(&argc, argv);

  auto& listeners = ::testing::UnitTest::GetInstance()->listeners();
  if (world.rank() != 0 && (argc < 2 || argv[1] != std::string("--print-workers"))) {
    auto* listener = listeners.Release(listeners.default_result_printer());
    listeners.Append(new WorkerTestFailurePrinter(std::shared_ptr<::testing::TestEventListener>(listener), world));
  }
  listeners.Append(new UnreadMessagesDetector(world));

  return RUN_ALL_TESTS();
}
