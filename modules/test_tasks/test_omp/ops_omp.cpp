// Copyright 2018 Nesterov Alexander
#include <omp.h>
#include <vector>
#include <string>
#include <random>
#include <iostream>
#include "../../../modules/test_tasks/test_omp/ops_omp.h"

std::vector<int> getRandomVector(int  sz) {
    std::random_device dev;
    std::mt19937 gen(dev());
    std::vector<int> vec(sz);
    for (int  i = 0; i < sz; i++) { vec[i] = gen() % 100; }
    return vec;
}

int getParallelOperations(std::vector<int> vec, const std::string& ops) {
    const int  sz = vec.size();
    int reduction_elem = 1;
    double start = omp_get_wtime();
    if (ops == "+") {
        #pragma omp parallel for reduction(+:reduction_elem)
        for (int  i = 0; i < sz; i++) {
            reduction_elem += vec[i];
        }
    } else if (ops == "-") {
        #pragma omp parallel for reduction(-:reduction_elem)
        for (int  i = 0; i < sz; i++) {
            reduction_elem -= vec[i];
        }
    } else if (ops == "*") {
        #pragma omp parallel for reduction(*:reduction_elem)
        for (int  i = 0; i < sz; i++) {
            reduction_elem *= vec[i];
        }
    }
    double finish = omp_get_wtime();
    std::cout << "How measure time in OpenMP: " << finish - start << std::endl;
    return reduction_elem;
}

int getSequentialOperations(std::vector<int> vec, const std::string& ops) {
    const int  sz = vec.size();
    int reduction_elem = 1;
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
