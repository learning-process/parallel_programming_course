// Copyright 2019 Anton Grishin

#include <omp.h>
#include <vector>
#include <cstring>
#include <cmath>
#include <iomanip>
#include <cstdlib>
#include <iostream>
#include <algorithm>

#define MAX_NZ 1000000
#define ZERO_IN_CRS 0.000001
#define EPS 0.01

size_t nzA, nzB;


struct crsMatrix {
    int N;  //размер матрицы N*N
    int NZ;  //количество ненулевых элементов
    double* Value;  //массив значений (размеров NZ)
    int* Col;  //массив номеров столбцов (размером NZ)
    int* RowIndex;  //массив индексов строк (размером N+1)
};

void InitializeMatrix(int N, int NZ, crsMatrix *mtx) {
    mtx->N = N;
    mtx->NZ = NZ;
    mtx->Value = new double[NZ];
    mtx->Col = new int[NZ];
    mtx->RowIndex = new int[N + 1];
}

void DeleteMatrix(crsMatrix *mtx) {
    delete[] mtx->Value;
    delete[] mtx->Col;
    delete[] mtx->RowIndex;
}

void GenerateCRS(crsMatrix *mtx, int n, int cntInRow) {
    int i, j, k, f, tmp, notNull, c;
    notNull = cntInRow * n;
    InitializeMatrix(n, notNull, mtx);
    for (i = 0; i < n; i++) {
        // Формируем номера столбцов в строке i
        for (j = 0; j < cntInRow; j++) {
            do {
                mtx->Col[i * cntInRow + j] = std::rand() % n;
                f = 0;
                for (k = 0; k < j; k++)
                    if (mtx->Col[i * cntInRow + j]
                        == mtx->Col[i * cntInRow + k])
                        f = 1;
            } while (f == 1);
        }
        // Сортируем номера столбцов в строке i
        for (j = 0; j < cntInRow - 1; j++)
            for (k = 0; k < cntInRow - 1; k++)
                if (mtx->Col[i * cntInRow + k]
                    > mtx->Col[i * cntInRow + k + 1]) {
                    tmp = mtx->Col[i * cntInRow + k];
                    mtx->Col[i * cntInRow + k] = mtx->Col[i * cntInRow + k + 1];
                    mtx->Col[i * cntInRow + k + 1] = tmp;
                }
    }
    for (i = 0; i < cntInRow * n; i++)
        mtx->Value[i] = std::rand() % 9 + 1;
    c = 0;
    for (i = 0; i <= n; i++) {
        mtx->RowIndex[i] = c;
        c += cntInRow;
    }
}

void PrintMatrix(crsMatrix *mtx) {
    int i;
    int k = mtx->NZ;
    int N = mtx->N + 1;

    printf("Matrix in CRS: \n");
    printf("\n Value: ");
    for (i = 0; i < k; i++) {
        printf(" %.1f", mtx->Value[i]);
    }
    printf("\n Col: ");
    for (i = 0; i < k; i++) {
        printf(" %d", mtx->Col[i]);
    }
    printf("\n RowIndex: ");
    for (i = 0; i < N; i++) {
        printf(" %d", mtx->RowIndex[i]);
    }
    printf("\n");
    fflush(stdout);
}



void Transp(crsMatrix *B) {
    // BT - траспонированная матрица В
    crsMatrix BT;
    int tmp = 0, S = 0, IIndex = 0, RIndex = 0, i, j;
    double V = 0.0;
    InitializeMatrix(B->N, B->NZ, &BT);

    memset(BT.RowIndex, 0, ((B->N) + 1) * sizeof(int));
    for (i = 0; i < B->NZ; i++)
        BT.RowIndex[B->Col[i] + 1]++;
    // храним кол-во элементов в столбце j-1 матрицы B

    // теперь сделаем так, что RI[i] хранит индекс начала i-1 строки
    for (i = 1; i <= B->N; i++) {
        tmp = BT.RowIndex[i];
        BT.RowIndex[i] = S;
        S = S + tmp;
    }

    //Поместим значения в корректную позицию (RI известно).
    //Зная, что j строка начинается с RI(j+1), добавляем
    // V и i в Value и Col соответственно, после чего
    //увеличим RI(j+1) на единицу (теперь элемент будет хранить начало j+1)
    //строки
    for (i = 0; i < B->N; i++) {
        int j1 = B->RowIndex[i];
        int j2 = B->RowIndex[i + 1];
        int Col = i;  // Столбец в AT - строка в А
        for (j = j1; j < j2; j++) {
            V = B->Value[j];  // Значение
            RIndex = B->Col[j];  // Строка в AT
            IIndex = BT.RowIndex[RIndex + 1];
            BT.Value[IIndex] = V;
            BT.Col[IIndex] = Col;
            BT.RowIndex[RIndex + 1]++;
        }
    }

    DeleteMatrix(B);

    InitializeMatrix(BT.N, BT.NZ, B);

    for (i = 0; i < BT.NZ; i++) {
        B->Col[i] = BT.Col[i];
        B->Value[i] = BT.Value[i];
    }

    for (i = 0; i < (BT.N + 1); i++) {
        B->RowIndex[i] = BT.RowIndex[i];
    }

    DeleteMatrix(&BT);
}

void Multiplication(const crsMatrix &A, const crsMatrix &B, crsMatrix *C) {
    int AN = A.N, BN = B.N, rowNZ, i, j, k, L;
    std::vector<int> columns;
    std::vector<double> values;
    std::vector<int> row_index;


    row_index.push_back(0);
    for (i = 0; i < AN; i++) {
        rowNZ = 0;
        for (j = 0; j < BN; j++) {
            double sum = 0;
            for (k = A.RowIndex[i]; k < A.RowIndex[i + 1]; k++) {
                for (L = B.RowIndex[j]; L < B.RowIndex[j + 1]; L++) {
                    if (A.Col[k] == B.Col[L]) {
                        sum += A.Value[k] * B.Value[L];
                        break;
                    }
                }
            }
            if (fabs(sum) > ZERO_IN_CRS) {
                columns.push_back(j);
                values.push_back(sum);
                rowNZ++;
            }
        }
        row_index.push_back(rowNZ + row_index[i]);
    }
    InitializeMatrix(AN, columns.size(), C);
    for (unsigned int j = 0; j < columns.size(); j++) {
        C->Col[j] = columns[j];
        C->Value[j] = values[j];
    }
    for (int i = 0; i <= AN; i++)
        C->RowIndex[i] = row_index[i];
}


void MultiplicationCompare(const crsMatrix &A,
                            const crsMatrix &B, crsMatrix *C) {
    int N = A.N;
    std::vector<int> columns;
    std::vector<double> values;
    std::vector<int> row_index;
    int NZ = 0;
    row_index.push_back(0);
    for (int i = 0; i < A.N; i++) {
        for (int j = 0; j < B.N; j++) {
            // Умножаем строку i матрицы A и столбец j матрицы B
            double sum = 0;
            int ks = A.RowIndex[i];
            int ls = B.RowIndex[j];
            int kf = A.RowIndex[i + 1] - 1;
            int lf = B.RowIndex[j + 1] - 1;
            while ((ks <= kf) && (ls <= lf)) {
                if (A.Col[ks] < B.Col[ls]) {
                    ks++;
                } else {
                    if (A.Col[ks] > B.Col[ls]) {
                        ls++;
                    } else {
                        sum += A.Value[ks] * B.Value[ls];
                        ks++;
                        ls++;
                    }
                }
            }
            if (fabs(sum) > ZERO_IN_CRS) {
                columns.push_back(j);
                values.push_back(sum);
                NZ++;
            }
        }
        row_index.push_back(NZ);
    }
    InitializeMatrix(N, NZ, C);

    for (unsigned int j = 0; j < columns.size(); j++) {
        C->Col[j] = columns[j];
        C->Value[j] = values[j];
    }
    for (int i = 0; i <= N; i++)
        C->RowIndex[i] = row_index[i];
}


void MultiplicationOMP(const crsMatrix &A,
    const crsMatrix &B, crsMatrix *C) {
    int N = A.N;
    // Продублируем все необходимые элементы по кол-ву строк,
    // для решения проблемы с Push_back
    // После всех вычислений соберем необходимые
    // данные в нужные элементы решения
    std::vector<int>* columns = new std::vector<int>[N];
    std::vector<double>* values = new std::vector<double>[N];
    int *row_index = new int[N + 1];
    int i = 0, j = 0, ks = 0, ls = 0, kf = 0, lf = 0;

    memset(row_index, 0, sizeof(int) * N);

#pragma omp parallel
    {
#pragma omp for private (j, ks, ls, kf, lf)
        for (i = 0; i < A.N; i++) {
            for (j = 0; j < B.N; j++) {
                // Умножаем строку i матрицы A и столбец j матрицы B
                double sum = 0;
                ks = A.RowIndex[i];
                ls = B.RowIndex[j];
                kf = A.RowIndex[i + 1] - 1;
                lf = B.RowIndex[j + 1] - 1;
                while ((ks <= kf) && (ls <= lf)) {
                    if (A.Col[ks] < B.Col[ls]) {
                        ks++;
                    } else {
                        if (A.Col[ks] > B.Col[ls]) {
                            ls++;
                        } else {
                            sum += A.Value[ks] * B.Value[ls];
                            ks++;
                            ls++;
                        }
                    }
                }
                if (fabs(sum) > ZERO_IN_CRS) {
                    columns[i].push_back(j);
                    values[i].push_back(sum);
                    row_index[i]++;
                }
            }
        }
    }

    int NZ = 0;
    for (i = 0; i < N; i++) {
        int tmp = row_index[i];
        row_index[i] = NZ;
        NZ += tmp;
    }
    row_index[N] = NZ;

    InitializeMatrix(N, NZ, C);

    int count = 0;
    for (i = 0; i < N; i++) {
        int size = columns[i].size();
        memcpy(&(*C).Col[count], &columns[i][0], size * sizeof(int));
        memcpy(&(*C).Value[count], &values[i][0], size * sizeof(double));
        count += size;
    }

    memcpy(C->RowIndex, &row_index[0], (N + 1) * sizeof(int));
    delete[] row_index;
    delete[] columns;
    delete[] values;
}


int Validate(const crsMatrix &A, const crsMatrix &B) {
    int i, NZ = A.NZ;
    if ((A.NZ != B.NZ) || (A.N != B.N))
        return 0;
    for (i = 0; i < NZ; i++)
        if ((A.Col[i] != B.Col[i]) || (fabs(A.Value[i] - B.Value[i]) > EPS))
            return 0;
    return 1;
}

int main(int argc, char **argv) {
    double serialTime, serialTime2, parallelTime;
    int num_threads;
    int SizeM = 0, NNZRow = 0;
    crsMatrix A, B, C, D, C_omp;

    if (argc > 3) {
        SizeM = atoi(argv[1]);
        NNZRow = atoi(argv[2]);
        num_threads = atoi(argv[3]);
    } else {
        SizeM = 4;
        NNZRow = 2;
        num_threads = 4;
    }

    if (SizeM < NNZRow) {
        printf("Invalid input parameters\n");
        return 1;
    }

    std::cout << "Generate CRS matrix..." << std::endl;
    GenerateCRS(&A, SizeM, NNZRow);
    GenerateCRS(&B, SizeM, NNZRow);
    std::cout << "Transpose B matrix..." << std::endl;
    Transp(&B);
    std::cout << "Starting first serial alghorithm..." << std::endl;
    serialTime = omp_get_wtime();
    Multiplication(A, B, &C);
    serialTime = omp_get_wtime() - serialTime;
    std::cout << "Starting second serial alghorithm..." << std::endl;
    serialTime2 = omp_get_wtime();
    MultiplicationCompare(A, B, &D);
    serialTime2 = omp_get_wtime() - serialTime2;

    std::cout << "Starting parallel alghorithm..." << std::endl;
    // parallel version
    omp_set_num_threads(num_threads);
    parallelTime = omp_get_wtime();
    MultiplicationOMP(A, B, &C_omp);
    parallelTime = omp_get_wtime() - parallelTime;

    std::cout << "Num threads = " << num_threads << std::endl;
    std::cout << "Size of matrix = " << SizeM << "x" << SizeM << std::endl;
    std::cout << "Not NULL elements in ROW = " << NNZRow << std::endl;
    std::cout << "Serial Time: " <<
        std::fixed << std::setprecision(8) << serialTime << std::endl;
    std::cout << "Serial Time (another func): " <<
        std::fixed << std::setprecision(8) << serialTime2 << std::endl;
    std::cout << "Parallel Time: " <<
        std::fixed << std::setprecision(8) << parallelTime << std::endl;
    std::cout << "Acceleration: " <<
        std::fixed << std::setprecision(8) <<
        serialTime2/parallelTime << std::endl;


    std::cout << "Validating in process..." << std::endl;
    if (Validate(C, D) == 1)
        std::cout << "Serial solves are equal!" << std::endl;
    else
        std::cout << "Serial solves are not equal!" << std::endl;


    if (Validate(D, C_omp) == 1)
        std::cout << "Serial and parallel solve are equal!" << std::endl;
    else
        std::cout << "Serial and parallel solve are not equal!" << std::endl;

    if (SizeM < 10) {
        std::cout << "A_________________________" << std::endl;
        PrintMatrix(&A);
        std::cout << "Transp B__________________" << std::endl;
        PrintMatrix(&B);
        Transp(&B);
        std::cout << "B_________________________" << std::endl;
        PrintMatrix(&B);
        std::cout << "Serial Result_____________" << std::endl;
        PrintMatrix(&C);
        std::cout << "Parallel Result_____________" << std::endl;
        PrintMatrix(&C_omp);
    }

    DeleteMatrix(&A);
    DeleteMatrix(&B);
    DeleteMatrix(&C);

    return 0;
}
