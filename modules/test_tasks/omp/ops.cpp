// Copyright 2018 Nesterov Alexander
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include "ops.h"

std::vector<int> getRandomVector(size_t sz)
{
    std::mt19937 gen;
    gen.seed(time(0));
    std::vector<int> vec(sz);
    for (size_t i = 0; i < sz; i++) { vec[i] = gen() % 100; }
    return vec;
}

int getParallelOperations(std::vector<int> vec, std::string ops)
{
    const size_t sz = vec.size();
    int reduction_elem = 1;
    if (ops == "+") {
        #pragma omp parallel for reduction(+:reduction_elem)
        for (size_t i = 0; i < sz; i++) {
            reduction_elem += vec[i];
        }
    } else if (ops == "-") {
        #pragma omp parallel for reduction(-:reduction_elem)
        for (size_t i = 0; i < sz; i++) {
            reduction_elem -= vec[i];
        }
    } else if (ops == "*") {
        #pragma omp parallel for reduction(*:reduction_elem)
        for (size_t i = 0; i < sz; i++) {
            reduction_elem *= vec[i];
        }
    }
    return reduction_elem;
}

int getSequentialOperations(std::vector<int> vec, std::string ops)
{
    const size_t sz = vec.size();
    int reduction_elem = 1;
    if (ops == "+") {
        for (size_t i = 0; i < sz; i++) {
            reduction_elem += vec[i];
        }
    } else if (ops == "-") {
        for (size_t i = 0; i < sz; i++) {
            reduction_elem -= vec[i];
        }
    } else if (ops == "*") {
        for (size_t i = 0; i < sz; i++) {
            reduction_elem *= vec[i];
        }
    }
    return reduction_elem;
}
