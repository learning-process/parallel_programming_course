// Copyright 2019 Kaganov Dmitryi
// Radix sort with simple merge (int)
#include <omp.h>
#include <ctime>
#include <cmath>
#include <utility>
#include <iostream>

unsigned int *sortMerge(unsigned int *firstArray, int firstSize,
                        unsigned int *secondArray, int secondSize) {
    unsigned int *array = new unsigned int[firstSize + secondSize];
    int i = 0, j = 0, k = 0;
    unsigned int tmp1 = 0;
    unsigned int tmp2 = 0;
    while (i < firstSize && j < secondSize) {
        tmp1 = firstArray[i];
        tmp2 = secondArray[j];
        if (tmp1 <= tmp2) {
            array[k] = tmp1;
            i++;
        } else {
            array[k] = tmp2;
            j++;
        }
        k++;
    }
    while (i < firstSize)
        array[k++] = firstArray[i++];
    while (j < secondSize)
        array[k++] = secondArray[j++];
    return array;
}

void lsdSort(unsigned int* A, unsigned int count) {
    unsigned int* B = new unsigned int[count];
    unsigned int index[4][256] = { {0} };
    unsigned int x, y, z;
    for (unsigned int i = 0; i < count; i++) {
        x = A[i];
        for (int j = 0; j < 4; j++) {
            index[j][static_cast<int>(x & 0xff)]++;
            x >>= 8;
        }
    }
    for (int i = 0; i < 4; i++) {
        y = 0;
        for (int j = 0; j < 256; j++) {
            z = index[i][j];
            index[i][j] = y;
            y += z;
        }
    }
    for (int i = 0; i < 4; i++) {
        for (unsigned int j = 0; j < count; j++) {
            x = A[j];
            y = static_cast<int>(x >> (i << 3)) & 0xff;
            B[index[i][y]++] = x;
        }
        std::swap(A, B);
    }
    delete[] B;
}

void printArray(unsigned int *array, int size) {
    if (size < 11) {
        for (int i = 0; i < size; i++) {
            std::cout << array[i] << " ";
        }
        std::cout << std::endl;
    }
}

void check(unsigned int* R, unsigned int size) {
    for (unsigned int i = 0; i < size - 1; i++) {
        if (R[i] > R[i + 1]) {
            std::cout << "\nError! Array not sorted!\n";
            return;
        }
    }
    std::cout << "\nArray is sorted\n";
}

int main(int argc, char** argv) {
    std::srand(static_cast<int>(time(NULL)));
    int mergeNum = 0;                   // number of mergers
    unsigned int arrSize = 0;           // array size
    unsigned int* A = NULL;
    unsigned int* R = NULL;
    unsigned int* buff = NULL;          // auxiliary array for merge
    unsigned int* lastBuff = NULL;      // remainder array
    double timeSort = 0;

    if (argc > 2) {
        arrSize = atoi(argv[1]);
        mergeNum = atoi(argv[2]);
    } else {
        arrSize = 10;
        mergeNum = 2;
    }

    // rounding for proper array division
    int size = (static_cast<int>(arrSize) + static_cast<int>(mergeNum) - 1) / static_cast<int>(mergeNum);
    std::cout << size << std::endl;

    std::cout << "\nArray size : " << arrSize << std::endl;
    std::cout << "Number of divisions : " << mergeNum << std::endl;
    A = new unsigned int[arrSize];
    R = new unsigned int[arrSize];
    buff = new unsigned int[size];
    lastBuff = new unsigned int[arrSize % size];

    for (unsigned int i = 0; i < arrSize; i++)
        A[i] = std::rand() % 200;

    std::cout << "\nInput array:\n";
    printArray(A, arrSize);

    // RADIX SORT
    timeSort = clock();
    for (unsigned int i = 0; i < arrSize; i += (size)) {
        // merge for remainder
        if ((i == arrSize - arrSize % size) && (arrSize % mergeNum != 0)) {
            for (unsigned int j = i, k = 0; j < i + (arrSize % size); j++, k++)
                lastBuff[k] = A[j];

            lsdSort(lastBuff, arrSize % size);
            printArray(lastBuff, arrSize % size);
            R = sortMerge(R, i, lastBuff, arrSize % size);

        // merge for without remainder
        } else {
            for (unsigned int j = i, k = 0; j < i + size; j++, k++)
                buff[k] = A[j];

            lsdSort(buff, size);
            printArray(buff, size);

            // if the  buff is first, then it is placed in the resulting (R)
            if (i == 0) {
                for (int j = 0; j < size; j++) {
                    R[j] = buff[j];
                }
            } else {                                        // for further merging with him
                R = sortMerge(R, i, buff, size);
            }
        }
    }
    timeSort = (clock() - timeSort) / static_cast<double>(CLOCKS_PER_SEC);

    std::cout << "\nSorted array:\n";
    printArray(R, arrSize);

    check(R, arrSize);
    std::cout << "Time: " << timeSort << std::endl;

    delete[] A;
    delete[] R;
    delete[] buff;
    delete[] lastBuff;

    return 0;
}
