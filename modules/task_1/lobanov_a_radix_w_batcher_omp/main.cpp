//  Copyright 2019 Lobanov Andrey
#define nSize 1000
#define nAmount 20

#include <iostream>
#include <cstdlib>
#include <ctime>

const int getMax(int arr[], int const len) {
    int max = arr[0];
    for (int i = 1; i < len; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

void countSort(int arr[], int len, int exp) {
    int* output = new int[len];
    int i, count[10] = { 0 };

    for (i = 0; i < len; i++) {
        count[(arr[i] / exp) % 10]++;
    }
    for (i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }
    for (i = len - 1; i >= 0; i--) {
        output[count[(arr[i] / exp) % 10] - 1] = arr[i];
        count[(arr[i] / exp) % 10]--;
    }

    for (i = 0; i < len; i++) {
        arr[i] = output[i];
    }
}

void radixSort(int arr[], int len) {
    int exp, m;
    m = getMax(arr, len);

    for (exp = 1; m / exp > 0; exp *= 10) {
        countSort(arr, len, exp);
    }
}

int main(int argc, char* argv[]) {
    int len = nAmount;
    int *arr = new int[len];

    srand((unsigned int)time(NULL));
    for (int i = 0; i < len; i++) {
    arr[i] = std::rand() % nSize;
        std::cout << "\nValue " << i + 1 << ": [" << arr[i] << "]";
    }

    radixSort(arr, len);

    std::cout << "\n\nSorted array ";
    for (int i = 0; i < len; i++) {
    std::cout << "[" << arr[i] << "]";
    }
return 0;
}
