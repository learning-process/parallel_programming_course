// Copyright 2023 Nesterov Alexander
#include <gtest/gtest.h>
#include <vector>
#include "core/include/task.hpp"
#include "reference/sum_values_by_rows_matrix/ref_task.hpp"

TEST(sum_values_by_rows_matrix, check_int32_t) {
    // Create data
    std::vector<int32_t> in(1369, 2);
    std::vector<uint64_t> in_index(2, 37);
    std::vector<int32_t> out(37, 0);

    // Create TaskData
    std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
    taskData->inputs_count.emplace_back(in.size());
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in_index.data()));
    taskData->inputs_count.emplace_back(in_index.size());
    taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskData->outputs_count.emplace_back(out.size());

    // Create Task
    ppc::reference::SumValuesByRowsMatrix<int32_t, uint64_t> testTask(taskData);
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.pre_processing();
    testTask.run();
    testTask.post_processing();
    for (int i = 0; i < in_index[0]; i++) {
        ASSERT_EQ(out[0], 2 * in_index[0]);
    }
}

TEST(sum_values_by_rows_matrix, check_validate_func1) {
    // Create data
    std::vector<int32_t> in(1369, 2);
    std::vector<uint64_t> in_index(2, 37);
    std::vector<int32_t> out(39, 0);

    // Create TaskData
    std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
    taskData->inputs_count.emplace_back(in.size());
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in_index.data()));
    taskData->inputs_count.emplace_back(in_index.size());
    taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskData->outputs_count.emplace_back(out.size());

    // Create Task
    ppc::reference::SumValuesByRowsMatrix<int32_t, uint64_t> testTask(taskData);
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, false);
}

TEST(sum_values_by_rows_matrix, check_validate_func2) {
    // Create data
    std::vector<int32_t> in(1369, 2);
    std::vector<uint64_t> in_index(3, 37);
    std::vector<int32_t> out(37, 0);

    // Create TaskData
    std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
    taskData->inputs_count.emplace_back(in.size());
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in_index.data()));
    taskData->inputs_count.emplace_back(in_index.size());
    taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskData->outputs_count.emplace_back(out.size());

    // Create Task
    ppc::reference::SumValuesByRowsMatrix<int32_t, uint64_t> testTask(taskData);
    auto isValid = testTask.validation();
    ASSERT_EQ(isValid, false);
}

TEST(sum_values_by_rows_matrix, check_double) {
    // Create data
    std::vector<double> in(1406);
    std::vector<uint64_t> in_index = {37, 38};
    std::vector<double> out(37, 0);
    for (auto i = 0; i < in.size(); ++i) {
        in[i] = i % 2 ? -2 : 2;
    }


    // Create TaskData
    std::shared_ptr<ppc::core::TaskData> taskData = std::make_shared<ppc::core::TaskData>();
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in.data()));
    taskData->inputs_count.emplace_back(in.size());
    taskData->inputs.emplace_back(reinterpret_cast<uint8_t*>(in_index.data()));
    taskData->inputs_count.emplace_back(in_index.size());
    taskData->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskData->outputs_count.emplace_back(out.size());

    // Create Task
    ppc::reference::SumValuesByRowsMatrix<double, uint64_t> testTask(taskData);
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.pre_processing();
    testTask.run();
    testTask.post_processing();
    for (int i = 0; i < in_index[0]; i++) {
        EXPECT_NEAR(out[i], 0.0, 1e-6);
    }
}

TEST(sum_values_by_rows_matrix, check_uint8_t) {
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
    ppc::reference::SumValuesByRowsMatrix<uint8_t, uint64_t> testTask(taskData);
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.pre_processing();
    testTask.run();
    testTask.post_processing();
    ASSERT_EQ(out[0], in.size());
}

TEST(sum_values_by_rows_matrix, check_int64_t) {
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
    ppc::reference::SumValuesByRowsMatrix<int64_t, uint64_t> testTask(taskData);
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.pre_processing();
    testTask.run();
    testTask.post_processing();
    ASSERT_EQ(out[0], in.size());
}

TEST(sum_values_by_rows_matrix, check_float) {
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
    ppc::reference::SumValuesByRowsMatrix<float, uint64_t> testTask(taskData);
    bool isValid = testTask.validation();
    ASSERT_EQ(isValid, true);
    testTask.pre_processing();
    testTask.run();
    testTask.post_processing();
    EXPECT_NEAR(out[0], in.size(), 1e-3);
}
