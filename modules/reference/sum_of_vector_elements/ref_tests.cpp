// Copyright 2023 Nesterov Alexander
#include <gtest/gtest.h>
#include <vector>
#include "core/include/task.hpp"
#include "reference/sum_of_vector_elements/ref_task.hpp"

TEST(sum_of_vector_elements, check_int32_t) {
    // Create data
    std::vector<int32_t> in(1256, 1);
    std::vector<int32_t> out(1, 0);
    // Create TaskData
    std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
    taskData->inputs_count.emplace_back(in.size());
    taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskData->outputs_count.emplace_back(out.size());
    // Create Task
    ppc::reference::SumOfVectorElements<int32_t> testTask(taskData);
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.pre_processing();
    testTask.run();
    testTask.post_processing();
    ASSERT_EQ(out[0], in.size());
}

TEST(sum_of_vector_elements, check_validate_func) {
    auto test_func = []() {
        // Create data
        std::vector<int32_t> in(125, 1);
        std::vector<int32_t> out(2, 0);
        // Create TaskData
        std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
        taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
        taskData->inputs_count.emplace_back(in.size());
        taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
        taskData->outputs_count.emplace_back(out.size());
        // Create Task
        ppc::reference::SumOfVectorElements<int32_t> testTask(taskData);
        testTask.pre_processing();
        bool isValid = testTask.validation();
        ASSERT_EQ(isValid, false);
    };

    EXPECT_DEATH(test_func(), "");
}

TEST(sum_of_vector_elements, check_double) {
    // Create data
    std::vector<double> in(25680, 1);
    std::vector<double> out(1, 0);
    // Create TaskData
    std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
    taskData->inputs_count.emplace_back(in.size());
    taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskData->outputs_count.emplace_back(out.size());
    // Create Task
    ppc::reference::SumOfVectorElements<double> testTask(taskData);
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.pre_processing();
    testTask.run();
    testTask.post_processing();
    EXPECT_NEAR(out[0], in.size(), 1e-6);
}

TEST(sum_of_vector_elements, check_uint8_t) {
    // Create data
    std::vector<uint8_t> in(255, 1);
    std::vector<uint8_t> out(1, 0);
    // Create TaskData
    std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
    taskData->inputs_count.emplace_back(in.size());
    taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskData->outputs_count.emplace_back(out.size());
    // Create Task
    ppc::reference::SumOfVectorElements<uint8_t> testTask(taskData);
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.pre_processing();
    testTask.run();
    testTask.post_processing();
    ASSERT_EQ(out[0], in.size());
}

TEST(sum_of_vector_elements, check_int64_t) {
    // Create data
    std::vector<int64_t> in(75836, 1);
    std::vector<int64_t> out(1, 0);
    // Create TaskData
    std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
    taskData->inputs_count.emplace_back(in.size());
    taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskData->outputs_count.emplace_back(out.size());
    // Create Task
    ppc::reference::SumOfVectorElements<int64_t> testTask(taskData);
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.pre_processing();
    testTask.run();
    testTask.post_processing();
    ASSERT_EQ(out[0], in.size());
}

TEST(sum_of_vector_elements, check_float) {
    // Create data
    std::vector<float> in(1, 1);
    std::vector<float> out(1, 0);
    // Create TaskData
    std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
    taskData->inputs_count.emplace_back(in.size());
    taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskData->outputs_count.emplace_back(out.size());
    // Create Task
    ppc::reference::SumOfVectorElements<float> testTask(taskData);
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.pre_processing();
    testTask.run();
    testTask.post_processing();
    EXPECT_NEAR(out[0], in.size(), 1e-3);
}
