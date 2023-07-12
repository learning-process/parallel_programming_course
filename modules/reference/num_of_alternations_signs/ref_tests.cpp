// Copyright 2023 Nesterov Alexander
#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include "core/include/task.hpp"
#include "reference/num_of_alternations_signs/ref_task.hpp"

TEST(num_of_alternations_signs, check_int32_t) {
    // Create data
    std::vector<int32_t> in(1256, 1);
    std::vector<int32_t> out(1, 0);
    for (int i = 0; i < in.size(); i++) {
        if (i % 2 == 0) { in[i] *= -1; }
    }

    // Create TaskData
    std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
    taskData->inputs_count.emplace_back(in.size());
    taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskData->outputs_count.emplace_back(out.size());

    // Create Task
    ppc::reference::NumOfAlternationsSigns<int32_t, uint64_t> testTask(taskData);
    testTask.pre_processing();
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.run();
    testTask.post_processing();
    ASSERT_EQ(out[0], in.size() - 1);
}

TEST(num_of_alternations_signs, check_validate_func) {
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
    ppc::reference::NumOfAlternationsSigns<int32_t, uint64_t> testTask(taskData);
    testTask.pre_processing();
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, false);
}

TEST(num_of_alternations_signs, check_double) {
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
    ppc::reference::NumOfAlternationsSigns<double, uint64_t> testTask(taskData);
    testTask.pre_processing();
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.run();
    testTask.post_processing();
    EXPECT_NEAR(out[0], in.size(), 1e-6);
}

TEST(num_of_alternations_signs, check_uint8_t) {
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
    ppc::reference::NumOfAlternationsSigns<uint8_t, uint64_t> testTask(taskData);
    testTask.pre_processing();
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.run();
    testTask.post_processing();
    ASSERT_EQ(out[0], in.size());
}

TEST(num_of_alternations_signs, check_int64_t) {
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
    ppc::reference::NumOfAlternationsSigns<int64_t, uint64_t> testTask(taskData);
    testTask.pre_processing();
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.run();
    testTask.post_processing();
    ASSERT_EQ(out[0], in.size());
}

TEST(num_of_alternations_signs, check_float) {
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
    ppc::reference::NumOfAlternationsSigns<float, uint64_t> testTask(taskData);
    testTask.pre_processing();
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.run();
    testTask.post_processing();
    EXPECT_NEAR(out[0], in.size(), 1e-3);
}
