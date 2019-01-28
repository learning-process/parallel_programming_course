// Copyright 2018 Nesterov Alexander
#include <assert.h>
#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
#pragma omp parallel for
    for (size_t i = 0; i < 10; i++) {
        std::cout << i << " ";
    }
    std::cout << '\n';
    return 0;
}
