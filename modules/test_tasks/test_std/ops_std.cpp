// Copyright 2018 Nesterov Alexander
#include <vector>
#include <string>
#include <utility>
#include <random>
#include <ctime>
#include <iostream>
#include "../../../modules/test_tasks/test_std/ops_std.h"
#include "../../../3rdparty/unapproved/unapproved.h"

std::vector<int> getRandomVector(int  sz) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::vector<int> vec(sz);
    for (int  i = 0; i < sz; i++) { vec[i] = gen() % 100; }
    return vec;
}

void atomOps(std::vector<int> vec, std::string ops, std::promise<int> &&pr) {
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
    }
    pr.set_value(reduction_elem);
}

int getParallelOperations(std::vector<int> vec, std::string ops) {
    const int nthreads = std::thread::hardware_concurrency();
    const int delta = (vec.end() - vec.begin()) / nthreads;

    std::promise<int> *promises = new std::promise<int>[nthreads];
    std::future<int> *futures = new std::future<int>[nthreads];
    std::thread *threads = new std::thread[nthreads];

    int reduction_elem = 0;
    for (int i = 0; i < nthreads; i++) {
        futures[i] = promises[i].get_future();
        std::vector<int> tmp_vec(
            vec.begin() + i * delta,
            vec.begin() + (i + 1) * delta);
        threads[i] = std::thread(atomOps, tmp_vec, ops, std::move(promises[i]));
        threads[i].join();
        reduction_elem += futures[i].get();
    }

    delete []promises;
    delete []futures;
    delete []threads;
    return reduction_elem;
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
    }
    return reduction_elem;
}
