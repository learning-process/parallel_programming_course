//  Copyright 2019 Senina Anastasia
#include <omp.h>
#include <iostream>
#include <vector>
#include <random>


struct ccsMatrix {
    std::vector<double> value;
    std::vector<size_t> row;
    std::vector<size_t> indexCol;
    ccsMatrix(int N, int countNZ);
    explicit ccsMatrix(std::vector<std::vector<double>>);
    ccsMatrix() {}
    bool operator==(const ccsMatrix& B);
};

bool ccsMatrix::operator==(const ccsMatrix& B) {
    return (value == B.value) && (row == B.row) && (indexCol == B.indexCol);
}

ccsMatrix::ccsMatrix(int N, int countNZ) {
    value.resize(countNZ);
    row.resize(countNZ);
    indexCol.resize(N + 1);
}

ccsMatrix::ccsMatrix(std::vector<std::vector<double>> matrix) {
    size_t n = matrix.size();
    int countNZ = 0;
    for (size_t j = 0; j < n; ++j) {
        indexCol.push_back(value.size());
        for (size_t i = 0; i < n; ++i) {
            if (matrix[i][j] != 0) {
                value.push_back(matrix[i][j]);
                row.push_back(i);
                countNZ++;
            }
        }
    }
    indexCol.push_back(countNZ);
}

std::vector<std::vector<double>> generateMatrix(size_t N) {
    std::vector<std::vector<double>> matrix;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, 10.0);
    for (size_t i = 0; i < N; ++i) {
        std::vector<double> row;
        for (size_t j = 0; j < N; ++j) {
            double v = distribution(generator);
            if (v > 3.5) {
                row.push_back(0);
            } else {
                row.push_back(v);
            }
        }
        matrix.push_back(row);
    }
    return matrix;
}

ccsMatrix transpon(const ccsMatrix& A) {
    ccsMatrix AT(A.indexCol.size() - 1, A.value.size());
    int r = 0;
    for (size_t i = 0; i < A.indexCol.size() - 1; ++i) {
        AT.indexCol[i] = r;
        for (size_t j = 0; j < A.row.size(); ++j) {
            if (A.row[j] == i) {
                AT.value[r] = A.value[j];
                size_t k = 0;
                for (; k < A.indexCol.size(); ++k)
                    if (j < A.indexCol[k]) {
                        --k;
                        break;
                    }
                AT.row[r] = k;
                r++;
            }
        }
    }
    *(AT.indexCol.end() - 1) = *(A.indexCol.end() - 1);
    return AT;
}
void mulccsMatrix(const ccsMatrix& A, const ccsMatrix& B, ccsMatrix* resultC) {
    double t1 = omp_get_wtime();
    ccsMatrix AT = transpon(A);
    double t2 = omp_get_wtime();
    std::cout << " time transpose : " << t2 - t1 << std::endl;
    size_t m = B.indexCol.size(), n = AT.indexCol.size();
    size_t r = 0;
    for (size_t j = 0; j < m - 1; ++j) {
        for (size_t i = 0; i < n - 1; ++i) {
            double sum = 0;
            size_t x = AT.indexCol[i + 1], y = B.indexCol[j + 1];
            for (size_t k = AT.indexCol[i]; k < x; ++k) {
                for (size_t l = B.indexCol[j]; l < y; ++l) {
                    if (AT.row[k] == B.row[l]) {
                        sum += AT.value[k] * B.value[l];
                        break;
                    }
                }
            }
            if (sum != 0) {
                resultC->value.push_back(sum);
                resultC->row.push_back(i);
                resultC->indexCol[j + 1]++;
                ++r;
            }
        }
        resultC->indexCol[j + 1] += resultC->indexCol[j];
    }
}
std::vector<std::vector<double>> mulMatrix(std::vector<std::vector<double>> A,
    std::vector<std::vector<double>> B) {
    size_t N = A.size();
    std::vector<std::vector<double>> C(N);

    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            C[i].push_back(0);
            for (size_t k = 0; k < N; ++k)
                C[i][j] += A[i][k] * B[k][j];
        }
    }
    return C;
}

int main(int argc, char** argv) {
    size_t size = 500;
    if (argc == 2) {
        size = atoi(argv[1]);
    }
    std::vector<std::vector<double>> A = generateMatrix(size);
    std::cout << " matrix A was generated " << std::endl;
    ccsMatrix ccsA(A);
    std::vector<std::vector<double>> B = generateMatrix(size);
    std::cout << " matrix B was generated " << std::endl;
    ccsMatrix ccsB(B);

    std::cout << " start A * B .." << std::endl;
    double t1 = omp_get_wtime();
    std::vector<std::vector<double>> C = mulMatrix(A, B);
    double t2 = omp_get_wtime();
    std::cout << " time A * B : " << t2 - t1 << std::endl;

    ccsMatrix res_ccs(size, 0);
    std::cout << " start ccsA * ccsB .." << std::endl;
    t1 = omp_get_wtime();
    mulccsMatrix(ccsA, ccsB, &res_ccs);
    t2 = omp_get_wtime();
    std::cout << " time ccsA * ccsB : " << t2 - t1 << std::endl;
    ccsMatrix ccsCtrue(C);
    std::cout << (ccsCtrue == res_ccs) << std::endl;

    return EXIT_SUCCESS;
}
