// Copyright 2019 Bederdinov Daniil
#define kLength 100000
#include <omp.h>
#include <cmath>
#include <ctime>
#include <iostream>

void shuffle(int *arr, int len) {
    srand((unsigned int)time(NULL));
    int i = len, j, temp;
    while (--i > 0) {
        j = std::rand() % kLength;
        temp = arr[j];
        arr[j] = arr[i];
        arr[i] = temp;
    }
}

void fillArray(int *arr, int len) {
    for (int i = 0; i < len; i++) {
        arr[i] = i;
    }
}

void quickSort(int *arr, int size) {
    int i = 0, j = size - 1;
    int pivot = arr[size / 2];

    do {
        while (arr[i] < pivot) {
            i++;
        }
        while (arr[j] > pivot) {
            j--;
        }

        if (i <= j) {
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;

            i++;
            j--;
        }
    } while (i <= j);

    if (j > 0)
        quickSort(arr, j + 1);
    if (i < size)
        quickSort(&arr[i], size - i);
}

bool check(int *A, int *B, int size) {
    for (int i = 0; i < size; ++i)
        if (A[i] == B[i]) {
            return false;
        }
    return true;
}

int compare(const int *i, const int *j) {
    return *i - *j;
}

int main(int argc, char *argv[]) {
    int *copy = new int[kLength];
    int *array = new int[kLength];

    fillArray(array, kLength);
    shuffle(array, kLength);

    for (int i = 0; i < kLength; i++)
        copy[i] = array[i];

    if (kLength < 100) {
        std::cout << std::endl;
        std::cout << "Array before sort:" << std::endl;
        for (int i = 0; i < kLength; i++) {
            std::cout << array[i] << " ";
        }
    }

    qsort(copy, kLength, sizeof(int), (int (*)(const void *, const void *))compare);

    std::cout << std::endl;
    quickSort(array, kLength);

    if (kLength < 100) {
        std::cout << "Array after sort:" << std::endl;
        for (int i = 0; i < kLength; i++) {
            std::cout << array[i] << " ";
        }
        std::cout << std::endl;
    }

    if (check(copy, array, kLength))
        std::cout << "Arrays are equal" << std::endl;
    else
        std::cout << "Arrays are NOT equal" << std::endl;

    delete[] copy;
    delete[] array;
    return 0;
}
