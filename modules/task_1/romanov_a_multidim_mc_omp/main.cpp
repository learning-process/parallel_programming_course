// Copyright 2019 Romanov Alexander
#include <omp.h>
#include <random>
#include <iostream>
#define N 2000000
#define a 0.0
#define A 2.0
#define b 0.0
#define B 2.0
#define c 0.0
#define C 2.0
int main() {
    double sum = 0.0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis1(a, A);
    std::uniform_real_distribution<> dis2(b, B);
    std::uniform_real_distribution<> dis3(c, C);
    double t1, t2;
    t1 = omp_get_wtime();
    for (int i = 0; i < N; ++i) {
        sum += (pow(dis1(gen), 2) + pow(dis2(gen), 2) + pow(dis3(gen), 2) - 4.0);
    }
    sum = sum * (A - a) * (B - b) * (C - c) / N;
    t2 = omp_get_wtime();
    std::cout << sum << "\t" << t2 - t1 << "\n";
    return 0;
}
