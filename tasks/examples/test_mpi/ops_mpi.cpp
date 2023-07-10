// Copyright 2023 Nesterov Alexander
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <functional>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>
#include "examples/test_mpi/ops_mpi.h"

std::vector<int> getRandomVector(int sz) {
    std::random_device dev;
    std::mt19937 gen(dev());
    std::vector<int> vec(sz);
    for (int  i = 0; i < sz; i++) { vec[i] = gen() % 100; }
    return vec;
}

int getSequentialOperations(std::vector<int> vec, const std::string& ops) {
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
    } else if (ops == "max") {
        reduction_elem = vec[0];
        for (int  i = 1; i < sz; i++) {
            reduction_elem = std::max(reduction_elem, vec[i]);
        }
    }
    return reduction_elem;
}

int getParallelOperations(std::vector<int> global_vec,
                          int count_size_vector, const std::string& ops) {
    boost::mpi::communicator world;
    const int delta = count_size_vector / world.size();

    if (world.rank() == 0) {
        for (int proc = 1; proc < world.size(); proc++) {
            world.send(proc, 0, global_vec.data() + proc * delta, delta);
        }
    }

    std::vector<int> local_vec(delta);
    if (world.rank() == 0) {
        local_vec = std::vector<int>(global_vec.begin(),
                                     global_vec.begin() + delta);
    } else {
        world.recv(0, 0, local_vec.data(), delta);
    }

    int global_sum = 0;
    int local_sum = getSequentialOperations(local_vec, ops);
    if (ops == "+" || ops == "-") {
        reduce(world, local_sum, global_sum, std::plus<int>(), 0);
    }
    if (ops == "max") {
        reduce(world, local_sum, global_sum, boost::mpi::maximum<int>(), 0);
    }
    return global_sum;
}
