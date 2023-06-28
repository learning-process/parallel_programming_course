// Copyright 2023 Nesterov Alexander
#include <gtest/gtest.h>
#include <vector>
#include "../../../modules/core/task.hpp"

template <class T>
class TestTask : public ppc::core::Task {
 public:
    explicit TestTask(std::shared_ptr<ppc::core::TaskData> taskData_) : Task(taskData_) {}
    bool pre_processing() override {
        input_ = reinterpret_cast<T*>(taskData->inputs[0]);
        output_ = reinterpret_cast<T*>(taskData->outputs[0]);
        output_[0] = 0;
        return true;
    };
    bool validation() override {
        if (taskData->outputs_count[0] == 1) {
            return true;
        } else {
            return false;
        }
    };
    bool run() override {
        for (int i = 0; i < taskData->inputs_count[0]; i++) {
            output_[0] += input_[i];
        }
        return true;
    };
    bool post_processing() override {
        return true;
    };

 private:
    T* input_{};
    T* output_{};
};

TEST(core_tests, check_int32_t) {
    // Create data
    std::vector<int32_t> in(20, 1);
    std::vector<int32_t> out(1, 0);
    // Create TaskData
    std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
    taskData->inputs_count.emplace_back(in.size());
    taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskData->outputs_count.emplace_back(out.size());
    // Create Task
    TestTask<int32_t> testTask(taskData);
    testTask.pre_processing();
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.run();
    ASSERT_EQ(out[0], in.size());
}

TEST(core_tests, check_double) {
    // Create data
    std::vector<double> in(20, 1);
    std::vector<double> out(1, 0);
    // Create TaskData
    std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
    taskData->inputs_count.emplace_back(in.size());
    taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskData->outputs_count.emplace_back(out.size());
    // Create Task
    TestTask<double> testTask(taskData);
    testTask.pre_processing();
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.run();
    EXPECT_NEAR(out[0], in.size(), 1e-6);
}

TEST(core_tests, check_uint8_t) {
    // Create data
    std::vector<uint8_t> in(20, 1);
    std::vector<uint8_t> out(1, 0);
    // Create TaskData
    std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
    taskData->inputs_count.emplace_back(in.size());
    taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskData->outputs_count.emplace_back(out.size());
    // Create Task
    TestTask<uint8_t> testTask(taskData);
    testTask.pre_processing();
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.run();
    ASSERT_EQ(out[0], in.size());
}

TEST(core_tests, check_int64_t) {
    // Create data
    std::vector<int64_t> in(20, 1);
    std::vector<int64_t> out(1, 0);
    // Create TaskData
    std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
    taskData->inputs_count.emplace_back(in.size());
    taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskData->outputs_count.emplace_back(out.size());
    // Create Task
    TestTask<int64_t> testTask(taskData);
    testTask.pre_processing();
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.run();
    ASSERT_EQ(out[0], in.size());
}

TEST(core_tests, check_float) {
    // Create data
    std::vector<float> in(20, 1);
    std::vector<float> out(1, 0);
    // Create TaskData
    std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
    taskData->inputs_count.emplace_back(in.size());
    taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskData->outputs_count.emplace_back(out.size());
    // Create Task
    TestTask<float> testTask(taskData);
    testTask.pre_processing();
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.run();
    EXPECT_NEAR(out[0], in.size(), 1e-3);
}
