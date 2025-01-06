#include <omp.h>

#include <iostream>

int main() {
#pragma omp parallel
  {
    std::cout << "Thread number = " << omp_get_thread_num() << '\n';
    std::cout << "Number of threads = " << omp_get_num_threads() << '\n';
  }
  return 0;
}