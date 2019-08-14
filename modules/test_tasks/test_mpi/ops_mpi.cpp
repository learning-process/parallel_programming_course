// Copyright 2018 Nesterov Alexander
#include <mpi.h>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include "../../../modules/test_tasks/test_mpi/ops_mpi.h"


std::vector<int> getRandomVector(int sz) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::vector<int> vec(sz);
    for (int  i = 0; i < sz; i++) { vec[i] = gen() % 100; }
    return vec;
}

int getSequentialOperations(std::vector<int> vec, std::string ops) {
    const int  sz = vec.size();
    int reduction_elem = 0;
    if (ops == "+") {
        for (int  i = 0; i < sz; i++) {
            reduction_elem += vec[i];
        }
    } else if (ops == "-") {
        for (int  i = 0; i < sz; i++) {
            reduction_elem -= vec[i];
        }
    } else if (ops == "*") {
        for (int  i = 0; i < sz; i++) {
            reduction_elem *= vec[i];
        }
    }
    return reduction_elem;
}
