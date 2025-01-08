#include <gtest/gtest.h>

#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>

class UnreadMessagesDetector : public ::testing::EmptyTestEventListener {
 public:
  UnreadMessagesDetector(boost::mpi::communicator world) : world_(std::move(world)) {}

  void OnTestEnd(const ::testing::TestInfo& test_info) override {
    world_.barrier();
    if (const auto msg = world_.iprobe(boost::mpi::any_source, boost::mpi::any_tag)) {
      fprintf(
          stderr,
          "[  PROCESS %d  ] [  FAILED  ] %s.%s: MPI message queue has an unread message from process %d with tag %d\n",
          world_.rank(), test_info.test_suite_name(), test_info.name(), msg->source(), msg->tag());
      exit(2);
    }
    world_.barrier();
  }

 private:
  boost::mpi::communicator world_;
};

int main(int argc, char** argv) {
  boost::mpi::environment env(argc, argv);
  boost::mpi::communicator world;

  ::testing::InitGoogleTest(&argc, argv);
  auto& listeners = ::testing::UnitTest::GetInstance()->listeners();
  if (world.rank() != 0) {
    delete listeners.Release(listeners.default_result_printer());
  }
  listeners.Append(new UnreadMessagesDetector(world));
  return RUN_ALL_TESTS();
}
