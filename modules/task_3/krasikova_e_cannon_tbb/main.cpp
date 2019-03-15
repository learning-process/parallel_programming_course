// Copyright 2019 Krasikova Ekaterina
#include <stdlib.h>
#include <random>
#include <ctime>
#include <iostream>
#include "tbb/task_scheduler_init.h"
#include "tbb/blocked_range2d.h"
#include "tbb/parallel_for.h"
#include "tbb/tick_count.h"
void addMultOfBlocks(double *A, double *B, double* C, int N, int lda) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            for (int q = 0; q < N; ++q) {
                C[i*lda + j] += A[i*lda + q] * B[q*lda + j];
            }
        }
    }
}
void clearMatrix(double *C, int N, int lda) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i*lda + j] = 0.0;
        }
    }
}
class BlocksMultiplicator {
    double *A, *B, *C;
    int n, q;
 public:
    BlocksMultiplicator(double *a, double *b, double *c,
        int _n, int _q) : A(a), B(b), C(c), n(_n), q(_q) {}
    void operator()(const tbb::blocked_range2d<int>& r) const {
        int blocksize = n / q;
        for (int i = r.rows().begin(); i < r.rows().end(); ++i) {
            for (int j = r.cols().begin(); j < r.cols().end(); ++j) {
                clearMatrix(&C[(i*n + j)*blocksize], blocksize, n);
                for (int k = 0; k < q; ++k) {
                    addMultOfBlocks(&A[(i*n + (j + i + k) % q)*blocksize],
                        &B[(((i + j + k) % q)*n + j)*blocksize],
                        &C[(i*n + j)*blocksize], blocksize, n);
                }
            }
        }
    }
};
void cannon(double *A, double *B, double* C, int n, int q) {
    int blocksize = n / q;
    for (int i = 0; i < q; ++i) {
        for (int j = 0; j < q; ++j) {
            clearMatrix(&C[(i*n + j)*blocksize], blocksize, n);
            for (int k = 0; k < q; ++k) {
                addMultOfBlocks(&A[(i*n + (j + i + k) % q)*blocksize],
                    &B[(((i + j + k) % q)*n + j)*blocksize],
                    &C[(i*n + j)*blocksize], blocksize, n);
            }
        }
    }
}
void cannonTbb(double *A, double *B, double* C, int n, int q, int grainSize) {
    tbb::task_scheduler_init init;
    tbb::parallel_for(tbb::blocked_range2d<int>(0, q, grainSize, 0, q, grainSize),
        BlocksMultiplicator(A, B, C, n, q));
}
int main(int argc, char** argv) {
    std::uniform_real_distribution<double> unif(0, 10);
    std::default_random_engine re;
    double s;
    int n = 4, q = 2;
    if (argc >= 3) {
        n = atoi(argv[1]);
        q = atoi(argv[2]);
    }
    double *A, *B, *C1, *C2;
    A = new double[n*n];
    B = new double[n*n];
    C1 = new double[n*n];
    C2 = new double[n*n];
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i*n + j] = unif(re);
            B[i*n + j] = unif(re);
        }
    }
    tbb::tick_count t1 = tbb::tick_count::now();
    cannon(A, B, C1, n, q);
    tbb::tick_count t2 = tbb::tick_count::now();
    std::cout << "time of sequential cannon matrix multiplication: " << (t2 - t1).seconds() << '\n';
    int grainSize = 1;
    if (argc == 4) {
        grainSize = atoi(argv[3]);
    }
    t1 = tbb::tick_count::now();
    cannonTbb(A, B, C2, n, q, grainSize);
    t2 = tbb::tick_count::now();
    std::cout << "time of parallel cannon matrix multiplication: " << (t2 - t1).seconds() << '\n';
    s = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (s < fabs(C1[i*n + j] - C2[i*n + j])) s = fabs(C1[i*n + j] - C2[i*n + j]);
        }
    }
    std::cout << "the maximum difference between results of matrix multiplication(sequential and parallel cannon): "
        << s << '\n';
    return 0;
}
