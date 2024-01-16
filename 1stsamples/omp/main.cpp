// Copyright 2023 Nesterov Alexander
#include <omp.h>

#include <iostream>

int main() {
#pragma omp parallel
  {
    std::cout << "Thread number = " << omp_get_thread_num() << std::endl;
    std::cout << "Number of threads = " << omp_get_num_threads() << std::endl;
  }
  return 0;
}