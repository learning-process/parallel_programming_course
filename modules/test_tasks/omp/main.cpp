// Copyright 2018 Nesterov Alexander
#include <assert.h>
#include <omp.h>
#include <iostream>

int main(int argc, char* argv[]) {
    #pragma omp parallel
    {
        #pragma omp critical
        {
            std::cout << "Thread ID = " << omp_get_thread_num() << '\n';
        }
    }
    return 0;
}
