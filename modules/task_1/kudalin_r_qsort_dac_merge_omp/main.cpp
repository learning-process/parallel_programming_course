//  Copyright 2019 Kudalin Roman
#define _SCL_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <utility>
#include <ctime>
#include <functional>

enum SortType { DESC, ASC };

int QuickSplit(int* arr, int left, int right,
    int (*comparator)(int arg1, int arg2)) {
    int pivot = arr[(left + right) / 2];
    int i = left, j = right;
    while (i <= j) {
        while (comparator(arr[i], pivot) < 0) {
            i++;
        }
        while (comparator(arr[j], pivot) > 0) {
            j--;
        }
        if (i >= j) {
            break;
        }
        std::swap(arr[i++], arr[j--]);
    }
    return j;
}

void QuickSort(int *arr, int left, int right,
    int (*comparator)(int arg1, int arg2)) {
    if (left < right) {
        int p = QuickSplit(arr, left, right, comparator);
        QuickSort(arr, left, p, comparator);
        QuickSort(arr, p + 1, right, comparator);
    }
}


int* GenerateArray(int size) {
    int* arr = new int[size];
    std::generate(arr, arr + size, []() { return std::rand() % 100; });
    return arr;
}

int* CopyArr(int* src, int size) {
    int* copy = new int[size];
    std::copy(src, src + size, copy);
    return copy;
}

void PrintArr(int* arr, int size) {
    std::for_each(arr, arr + size, [](int elem) { std::cout << elem << " "; });
    std::cout << std::endl;
}

void SortAndCompare(int* arr1, int* arr2, int size, SortType type) {
    std::random_shuffle(arr1, arr1 + size);
    std::random_shuffle(arr2, arr2 + size);

    auto descending = [](int arg1, int arg2) {
        if (arg1 < arg2) {
            return 1;
        }
        if (arg1 > arg2) {
            return -1;
        }
        return 0;
    };

    auto ascending = [](int arg1, int arg2) {
        if (arg1 < arg2) {
            return -1;
        }
        if (arg1 > arg2) {
            return 1;
        }
        return 0;
    };

    switch (type) {
      case DESC:
        QuickSort(arr1, 0, size - 1, descending);
        std::sort(arr2, arr2 + size, std::greater<int>());
        break;
      case ASC:
        QuickSort(arr1, 0, size - 1, ascending);
        std::sort(arr2, arr2 + size, std::less<int>());
        break;
      default:
        break;
    }

    if (std::equal(arr1, arr1 + size, arr2)) {
        std::cout << "Arrays are equal" << std::endl;
    } else {
        std::cout << "Arrays aren't equal" << std::endl;
    }
}

int main(int argc, char** argv) {
    std::srand((unsigned int)std::time(nullptr));
    const int kSize = 10;
    int* arr = GenerateArray(kSize);
    int* copy_arr = CopyArr(arr, kSize);

    SortAndCompare(arr, copy_arr, kSize, ASC);
    SortAndCompare(arr, copy_arr, kSize, DESC);

    delete[] arr;
    delete[] copy_arr;
    return 0;
}
