// Copyright 2018 Nesterov Alexander
#ifndef MODULES_TEST_TASKS_TEST_OMP_OPS_OMP_H_
#define MODULES_TEST_TASKS_TEST_OMP_OPS_OMP_H_

#include <vector>
#include <string>

std::vector<int> getRandomVector(int  sz);
int getParallelOperations(std::vector<int> vec, std::string ops);
int getSequentialOperations(std::vector<int> vec, std::string ops);

#endif  // MODULES_TEST_TASKS_TEST_OMP_OPS_OMP_H_
