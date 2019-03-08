// Copyright 2019 Krasikova Ekaterina
#include <stdlib.h>
#include <omp.h>
#include <random>
#include <ctime>
#include <iostream>
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
void cannon(double *A, double *B, double* C, int n, int q) {
    int blocksize = n / q;
    for (int i = 0; i < q; ++i) {
        for (int j = 0; j < q; ++j) {
            clearMatrix(&C[(i*n + j)*blocksize], blocksize, n);
            for (int k = 0; k < q; ++k) {
                addMultOfBlocks(&A[(i*n + (j+i+k)%q)*blocksize],
                    &B[(((i+j+k)%q)*n + j)*blocksize],
                    &C[(i*n + j)*blocksize], blocksize, n);
            }
        }
    }
}
void cannonOmp(double *A, double *B, double* C, int n, int q) {
    int blocksize = n / q;
    omp_set_nested(1);
#pragma omp parallel for
    for (int i = 0; i < q; ++i) {
#pragma omp parallel for
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
int main(int argc, char** argv) {
    std::uniform_real_distribution<double> unif(0, 10);
    std::default_random_engine re;
    double t1, t2, s;
    int n = 4, q = 2;
    if (argc == 3) {
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
            C1[i*n + j] = 0.0;
        }
    }
    t1 = omp_get_wtime();
    addMultOfBlocks(A, B, C1, n, n);
    t2 = omp_get_wtime();
    std::cout << "time of sequential trivial matrix multiplication: " << t2-t1 << '\n';
    t1 = omp_get_wtime();
    cannon(A, B, C2, n, q);
    t2 = omp_get_wtime();
    std::cout << "time of sequential cannon matrix multiplication: " << t2 - t1 << '\n';
    s = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (s < fabs(C1[i*n + j] - C2[i*n + j])) s = fabs(C1[i*n + j] - C2[i*n + j]);
        }
    }
    std::cout << "the maximum difference between results of matrix" <<
        " multiplication(trivial and sequential cannon): " << s << '\n';
    t1 = omp_get_wtime();
    cannonOmp(A, B, C1, n, q);
    t2 = omp_get_wtime();
    std::cout << "time of parallel cannon matrix multiplication: " << t2 - t1 << '\n';
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
