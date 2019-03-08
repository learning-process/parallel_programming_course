// Copyright 2019 Romanov Alexander

#include <tbb/tbb.h>
#include <tbb/parallel_reduce.h>
#include <tbb/blocked_range.h>
#include <random>
#include <iostream>
#include <numeric>
#include <functional>
#include <ctime>
#define N 1000000
#define NTHREADS 2
#define a 0.0
#define A 2.0
#define b 0.0
#define B 2.0
#define c 0.0
#define C 2.0
double integralCalculator(double _ar[], size_t n) {
    return tbb::parallel_reduce(
        tbb::blocked_range<double*>(_ar, _ar + n),
        0.0,
        [](const tbb::blocked_range<double*>& r, double value)->double {
        return std::accumulate(r.begin(), r.end(), value);
        },
        std::plus<double>());
}
int main() {
    double sum = 0.0;
    double tseq, tpar;
    tbb::tick_count t1 = tbb::tick_count::now();
    std::random_device rd;
    std::mt19937 gen(1);
    double* rngArray = new double[N * 3];
    std::uniform_real_distribution<> dis1(a, A);
    std::uniform_real_distribution<> dis2(b, B);
    std::uniform_real_distribution<> dis3(c, C);
    #pragma omp parallel for private(gen)
    for (int i = 0; i < 3 * N; i += 3) {
        rngArray[i] = pow(dis1(gen), 2);
        rngArray[i+1] = pow(dis2(gen), 2);
        rngArray[i+2] = pow(dis3(gen), 2);
    }
    tbb::tick_count t2 = tbb::tick_count::now();
    std::cout << "Allocation and generation time = " << (t2 - t1).seconds() << std::endl;
    t1 = tbb::tick_count::now();
    for (int i = 0; i < 3 * N; i += 3) {
        sum += (rngArray[i] + rngArray[i+1] + rngArray[i+2]);
    }
    sum = (sum - 4.0 * N) * (A - a) * (B - b) * (C - c) / N;
    t2 = tbb::tick_count::now();
    tseq = (t2 - t1).seconds();
    std::cout << "Value of integral = " << sum << std::endl;
    std::cout << "Worktime of sequential algo = " << tseq << std::endl;
    std::cout << "#############################\n";
    t1 = tbb::tick_count::now();
    tbb::task_scheduler_init init(2);
    sum = integralCalculator(rngArray, 3 * N);
    init.terminate();
    sum = (sum - 4.0 * N) * (A - a) * (B - b) * (C - c) / N;
    t2 = tbb::tick_count::now();
    tpar = (t2 - t1).seconds();
    std::cout << "Value of integral = " << sum << std::endl;
    std::cout << "Worktime of parallel algo = " << tpar << std::endl;
    std::cout << "Acceleration = " << tseq / tpar << std::endl;
    delete[] rngArray;
    return 0;
}
