// Copyright 2018 Nesterov Alexander
#ifndef TASKS_TEST_TASKS_TEST_OMP_OPS_OMP_H_
#define TASKS_TEST_TASKS_TEST_OMP_OPS_OMP_H_

#include <vector>
#include <string>

std::vector<int> getRandomVector(int  sz);
int getParallelOperations(std::vector<int> vec, const std::string& ops);
int getSequentialOperations(std::vector<int> vec, const std::string& ops);

#endif  // TASKS_TEST_TASKS_TEST_OMP_OPS_OMP_H_
