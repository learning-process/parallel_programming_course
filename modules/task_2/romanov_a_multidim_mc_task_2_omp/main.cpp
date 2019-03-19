// Copyright 2019 Romanov Alexander

#include <omp.h>
#include <random>
#include <iostream>
#define N 20000000
#define NTHREADS 2
#define a 0.0
#define A 2.0
#define b 0.0
#define B 2.0
#define c 0.0
#define C 2.0
int main() {
    double sum = 0.0;
    double t1, t2;
    double tsec, tpar;
    t1 = omp_get_wtime();
    std::random_device rd;
    std::mt19937 gen(1);
    double* x_ar = new double[N * 3];
    double* y_ar = x_ar + N;
    double* z_ar = x_ar + 2 * N;
    std::uniform_real_distribution<> dis1(a, A);
    std::uniform_real_distribution<> dis2(b, B);
    std::uniform_real_distribution<> dis3(c, C);
    #pragma omp parallel for private(gen)
    for (int i = 0; i < N; ++i) {
        x_ar[i] = pow(dis1(gen), 2);
        y_ar[i] = pow(dis2(gen), 2);
        z_ar[i] = pow(dis3(gen), 2);
    }
    t2 = omp_get_wtime();
    std::cout << "Allocation and generation time = " << t2 - t1 << std::endl;
    t1 = omp_get_wtime();
    for (int i = 0; i < N; ++i) {
        sum += (x_ar[i] + y_ar[i] + z_ar[i] - 4.0);
    }
    sum = sum * (A - a) * (B - b) * (C - c) / N;
    t2 = omp_get_wtime();
    tsec = t2 - t1;
    std::cout << "Value of integral = " << sum << std::endl;
    std::cout << "Worktime of sequential algo = " << tsec << std::endl;
    std::cout << "#############################\n";
    omp_set_num_threads(NTHREADS);
    t1 = omp_get_wtime();
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < N; ++i) {
        sum += (x_ar[i] + y_ar[i] + z_ar[i] - 4.0);
    }
    sum = sum * (A - a) * (B - b) * (C - c) / N;
    t2 = omp_get_wtime();
    tpar = t2 - t1;
    std::cout << "Value of integral = " << sum << std::endl;
    std::cout << "Worktime of parallel algo = " << tpar << std::endl;
    std::cout << "Acceleration = " << tsec / tpar << std::endl;
    delete[] x_ar;
    return 0;
}
