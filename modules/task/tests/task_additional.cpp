#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "task/include/task.hpp"

class TaskAdditionalTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Setup for each test
  }

  void TearDown() override {
    // Clean up after each test
  }
};

// Test TaskGetter function - unique functionality not covered elsewhere
TEST_F(TaskAdditionalTest, TaskGetter_WithBasicTask_CreatesTaskSuccessfully) {
  // Create a task to test with
  class GetterTestTask : public ppc::task::Task<std::vector<int>, std::vector<int>> {
   public:
    GetterTestTask(int value) : ppc::task::Task<std::vector<int>, std::vector<int>>(), value_(value) {}

    bool PreProcessingImpl() override { return true; }
    bool ValidationImpl() override { return true; }
    bool RunImpl() override { return true; }
    bool PostProcessingImpl() override { return true; }

    [[nodiscard]] int GetValue() const { return value_; }

   private:
    int value_;
  };

  // Test TaskGetter function
  auto getter_result = ppc::task::TaskGetter<GetterTestTask>(42);

  EXPECT_NE(getter_result, nullptr);
  EXPECT_EQ(getter_result->GetValue(), 42);
}

TEST_F(TaskAdditionalTest, TaskGetter_WithDifferentTaskTypes_CreatesTasksSuccessfully) {
  // Test TaskGetter with different task types
  class TaskType1 : public ppc::task::Task<std::vector<int>, std::vector<int>> {
   public:
    explicit TaskType1(std::string name)
        : ppc::task::Task<std::vector<int>, std::vector<int>>(), name_(std::move(name)) {}

    bool PreProcessingImpl() override { return true; }
    bool ValidationImpl() override { return true; }
    bool RunImpl() override { return true; }
    bool PostProcessingImpl() override { return true; }

    [[nodiscard]] std::string GetName() const { return name_; }

   private:
    std::string name_;
  };

  class TaskType2 : public ppc::task::Task<std::vector<int>, std::vector<int>> {
   public:
    TaskType2(double value) : ppc::task::Task<std::vector<int>, std::vector<int>>(), value_(value) {}

    bool PreProcessingImpl() override { return true; }
    bool ValidationImpl() override { return true; }
    bool RunImpl() override { return true; }
    bool PostProcessingImpl() override { return true; }

    [[nodiscard]] double GetValue() const { return value_; }

   private:
    double value_;
  };

  auto getter1 = ppc::task::TaskGetter<TaskType1>(std::string("test"));
  auto getter2 = ppc::task::TaskGetter<TaskType2>(3.14);

  EXPECT_NE(getter1, nullptr);
  EXPECT_NE(getter2, nullptr);
  EXPECT_EQ(getter1->GetName(), "test");
  EXPECT_DOUBLE_EQ(getter2->GetValue(), 3.14);
}
