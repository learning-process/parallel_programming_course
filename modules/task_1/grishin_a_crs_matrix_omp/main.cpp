// Copyright 2019 Anton Grishin

#include <omp.h>
#include <vector>
#include <cstring>
#include <cmath>
#include <iomanip>
#include <cstdlib>
#include <iostream>


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
                    if (mtx->Col[i * cntInRow + j] == mtx->Col[i * cntInRow + k])
                        f = 1;
            } while (f == 1);
        }
        // Сортируем номера столбцов в строке i
        for (j = 0; j < cntInRow - 1; j++)
            for (k = 0; k < cntInRow - 1; k++)
                if (mtx->Col[i * cntInRow + k] > mtx->Col[i * cntInRow + k + 1]) {
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

    for (i = 1; i <= B->N; i++) {
        tmp = BT.RowIndex[i];
        BT.RowIndex[i] = S;
        S = S + tmp;
    }

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



int main(int argc, char **argv) {
    double serialTime;
    int SizeM = 0, NNZRow = 0;
    crsMatrix A, B, C;

    if (argc > 2) {
        SizeM = atoi(argv[1]);
        NNZRow = atoi(argv[2]);
    } else {
        SizeM = 4;
        NNZRow = 2;
    }

    if (SizeM < NNZRow) {
        printf("Invalid input parameters\n");
        return 1;
    }


    GenerateCRS(&A, SizeM, NNZRow);
    GenerateCRS(&B, SizeM, NNZRow);
    Transp(&B);
    serialTime = omp_get_wtime();
    Multiplication(A, B, &C);

    serialTime = omp_get_wtime() - serialTime;


    std::cout << "Size of matrix = " << SizeM << "x" << SizeM << std::endl;
    std::cout << "Not NULL elements in ROW = " << NNZRow << std::endl;
    std::cout << "Serial Time: " << std::fixed << std::setprecision(8) << serialTime << std::endl;



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
    }

    DeleteMatrix(&A);
    DeleteMatrix(&B);
    DeleteMatrix(&C);

    return 0;
}
