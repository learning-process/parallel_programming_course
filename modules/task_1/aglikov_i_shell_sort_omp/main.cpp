//  Copyright 2019 Aglikov Ilya

#include <iostream>
#include <algorithm>
#include <utility>

void insertsort(int* arr, int size);
void shellsort(int* arr, int size);
void printArr(int* arr, int size);
bool checkEquality(int* arr0, int* arr1, int size);

int main(int argc, char* argv[]) {
    int* arr0 = nullptr;
    int* arr1 = nullptr;
    int size = 10;

    if (argc > 1)   size = atoi(argv[1]);
    arr0 = new int[size];
    arr1 = new int[size];
    for (int i = 0; i < size; i++)
        arr0[i] = arr1[i] = size - i;
    std::cout << "Original array: ";
    printArr(arr0, size);

    std::cout << "Insert sort: ";
    insertsort(arr0, size);
    printArr(arr0, size);

    std::cout << "Shell sort: ";
    shellsort(arr1, size);
    printArr(arr1, size);

    if (checkEquality(arr0, arr1, size))
        std::cout << "Sort working right.\n";
    else
        std::cout << "Sort working wrong.\n";

    return 0;
}

void insertsort(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        int j = i - 1;
        int fake = arr[i];
        while ((arr[j] > fake) && (j >= 0)) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = fake;
    }
}

void shellsort(int* arr, int size) {
    int step = size / 2;
    while (step > 0) {
        for (int i = 0; i < size - step; i++) {
            int j = i;
            while (j >= 0 && arr[j] > arr[j + step]) {
                std::swap(arr[j], arr[j + step]);
                j--;
            }
        }
        step = step / 2;
    }
}

void printArr(int* arr, int size) {
    for (int i = 0; i < size; i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;
}

bool checkEquality(int* arr0, int* arr1, int size) {
    bool flag = true;
    for (int i = 0; i < size; i++)
        if (arr0[i] != arr1[i]) {
            flag = false;
            break;
        }
    return flag;
}
