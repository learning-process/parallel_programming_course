// Copyright 2018 Nesterov Alexander
#include <tbb/tbb.h>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <functional>
#include <numeric>
#include "../../../modules/test_tasks/test_tbb/ops_tbb.h"

std::vector<int> getRandomVector(int sz) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::vector<int> vec(sz);
    for (int i = 0; i < sz; i++) { vec[i] = gen() % 100; }
    return vec;
}

struct Sum {
    int value;
    Sum() : value(0) {}
    Sum(Sum&s, tbb::split) : value(0) {}
    void operator()(const tbb::blocked_range<std::vector<int>::iterator>& r) {
        value = std::accumulate(r.begin(), r.end(), value);
    }
    void join(const Sum& rhs) { value += rhs.value; }
};

struct Mult {
    int value;
    Mult() : value(1) {}
    Mult(Mult&s, tbb::split) : value(1) {}
    void operator()(const tbb::blocked_range<std::vector<int>::iterator>& r) {
        value = std::accumulate(r.begin(), r.end(), value, std::multiplies<int>{});
    }
    void join(const Mult& rhs) { value *= rhs.value; }
};

int getParallelOperations(std::vector<int> vec, std::string ops) {
    int reduction_elem = 1;
    if (ops == "+") {
        Sum sum;
        tbb::parallel_reduce(
            tbb::blocked_range<std::vector<int>::iterator>(
                vec.begin(), vec.end()), sum);
        reduction_elem += sum.value;
    } else if (ops == "-") {
        Sum diff;
        tbb::parallel_reduce(
            tbb::blocked_range<std::vector<int>::iterator>(
                vec.begin(), vec.end()), diff);
        reduction_elem -= diff.value;
    } else if (ops == "*") {
        Mult mult;
        tbb::parallel_reduce(
            tbb::blocked_range<std::vector<int>::iterator>(
                vec.begin(), vec.end()), mult);
        reduction_elem *= mult.value;
    }
    return reduction_elem;
}

int getSequentialOperations(std::vector<int> vec, std::string ops) {
    const int sz = vec.size();
    int reduction_elem = 1;
    if (ops == "+") {
        for (int i = 0; i < sz; i++) {
            reduction_elem += vec[i];
        }
    } else if (ops == "-") {
        for (int i = 0; i < sz; i++) {
            reduction_elem -= vec[i];
        }
    } else if (ops == "*") {
        for (int i = 0; i < sz; i++) {
            reduction_elem *= vec[i];
        }
    }
    return reduction_elem;
}
