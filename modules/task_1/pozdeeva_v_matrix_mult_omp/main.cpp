// Copyright 2019 Pozdeeva Varvara
#include <iostream>
#include <ctime>
#include <random>
double* CreateRandMatrix(int size) {
    double* tmp = new double[size*size];
    for (int i = 0; i < size*size; i++)
        tmp[i] = (std::rand() % 10000) / 1000.0f;
    return tmp;
}
void PrintMatrix(double* A, int N) {
    for (int i = 0; i < N*N; i += N) {
        for (int j = 0; j< N; j++)
            std::cout << A[i + j] << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
void MultiMatrix(double* A, double* B, double* C, int n, int bSize) {
    for (int i = 0; i < bSize; ++i)
        for (int j = 0; j < bSize; ++j)
            for (int k = 0; k < bSize; ++k) {
                C[i * n + j] += A[i * n + k] * B[k * n + j];
            }
}
void Foxs(double* A, double* B, double* C, int n, int q) {
    int bSize = n / q;
    for (int i = 0; i < q; i++)
        for (int j = 0; j < q; j++)
            for (int k = 0; k < q; k++) {
                MultiMatrix(&A[(i*n+j)*bSize], &B[(j*n + k)*bSize], &C[(i*n + k)*bSize], n, bSize);
            }
}
int main(int argc, char** argv) {
    double *A, *B, *C;
    int N = 6, q = 2;
    if (argc == 3) {
        N = atoi(argv[1]);
        q = atoi(argv[2]);
    }
    srand((unsigned int)time(0));
    A = CreateRandMatrix(N);
    B = CreateRandMatrix(N);
    PrintMatrix(A, N);
    PrintMatrix(B, N);
    C = new double[N*N];
    for (int i = 0; i < N*N; i++)
        C[i] = 0.0;
    Foxs(A, B, C, N, q);
    PrintMatrix(C, N);
    return 0;
}
