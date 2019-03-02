// Copyright 2019 Bederdinov Daniil
#define kLength 80
#include <omp.h>
#include <iostream>
#include <ctime>
#include <cmath>


void CreateArray(int arr[], int length) {
    srand((unsigned int)time(NULL));
    for (int i = 0; i < length; i++) {
        arr[i] = std::rand() % kLength;
    }
}

void QuickSort(int *arr, int size) {
    int i = 0, j = size - 1;
    int pivot = arr[size / 2];

    do {
        while (arr[i] < pivot)
            i++;
        while (arr[j] > pivot)
            j--;

        if (i <= j) {
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;

            i++;
            j--;
        }
    } while (i <= j);

    if (j > 0)
        QuickSort(arr, j + 1);
    if (i < size)
        QuickSort(&arr[i], size - i);
}

int main(int argc, char *argv[]) {
    int array[kLength];
    CreateArray(array, kLength);
    std::cout << std::endl;
    std::cout << "array before sort:" << std::endl;
    for (int i = 0; i < kLength; i++) {
        std::cout << array[i] << " ";
    }

    std::cout << std::endl;

    QuickSort(array, kLength);
    std::cout << "array after sort:" << std::endl;
    for (int i = 0; i < kLength; i++) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}
