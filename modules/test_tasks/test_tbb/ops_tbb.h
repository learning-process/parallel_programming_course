// Copyright 2018 Nesterov Alexander
#ifndef MODULES_TEST_TASKS_TEST_TBB_OPS_TBB_H_
#define MODULES_TEST_TASKS_TEST_TBB_OPS_TBB_H_

#include <vector>
#include <string>

std::vector<int> getRandomVector(int sz);
int getParallelOperations(std::vector<int> vec, const std::string& ops);
int getSequentialOperations(std::vector<int> vec, const std::string& ops);

#endif  // MODULES_TEST_TASKS_TEST_TBB_OPS_TBB_H_
