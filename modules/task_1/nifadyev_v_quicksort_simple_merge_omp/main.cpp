// Copyright Nifadyev Vadim 2019
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <utility>

// QuickSort - linear algorithm

int Partition(int* array, int low, int high);
void QuickSort(int* array, int low, int high);
void PrintArray(int* array, const int size);
bool IsCorrectlySorted(int* customSortedArray, int* stdSortedArray,
                       const int size);

int main() {
    const int SIZE = 50;
    int array[SIZE] = {0};
    int copiedArray[SIZE] = {0};

    std::generate(array, array + SIZE, []() { return std::rand() % 100; });
    std::copy(array, array + SIZE, copiedArray);

    std::cout << "Initial array: ";
    PrintArray(array, SIZE);

    QuickSort(array, 0, SIZE - 1);
    // Sort copy of initial array by default qsort to compare sorted arrays
    qsort(copiedArray, SIZE, sizeof(int), [](const void* a, const void* b) {
        return (*reinterpret_cast<const int*>(a) -
                *reinterpret_cast<const int*>(b));
    });

    if (IsCorrectlySorted(array, copiedArray, SIZE)) {
        std::cout << "Sorted array:  ";
        PrintArray(array, SIZE);
    } else {
        std::cout << "Quick sort failed. Array is not sorted" << std::endl;
    }

    return 0;
}

/* This function takes array[high] as pivot, places
   the pivot element at its correct position in sorted
   array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot.

   array[] --> Array to be sorted,
   low  --> Starting index,
   high  --> Ending index. */
int Partition(int* array, int low, int high) {
    int pivot = array[high];
    int i = (low - 1);  // Index of smaller element

    for (int j = low; j <= high - 1; j++) {
        if (array[j] <= pivot) {
            i++;
            std::swap(array[i], array[j]);
        }
    }

    std::swap(array[i + 1], array[high]);

    return (i + 1);
}

/* QuickSort implementation.

   array[] --> Array to be sorted,
   low  --> Starting index,
   high  --> Ending index. */
void QuickSort(int* array, int low, int high) {
    if (low < high) {
        // array[partitioningIndex] is now at right place
        int partitioningIndex = Partition(array, low, high);

        /* Separately sort elements before
        Partition and after Partition */
        QuickSort(array, low, partitioningIndex - 1);
        QuickSort(array, partitioningIndex + 1, high);
    }
}

void PrintArray(int* array, const int size) {
    for (int i = 0; i < size; i++) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
}

/* Compare array sorted by QuickSort function to
array sorted by qsort() from standard library.

   customSortedArray[] --> Array sorted by QuickSort,
   stdSortedArray[]  --> Array sorted by qsort,
   size  --> Size of both arrays. */
bool IsCorrectlySorted(int* customSortedArray, int* stdSortedArray,
                       const int size) {
    for (int i = 0; i < size; i++) {
        if (customSortedArray[i] != stdSortedArray[i]) {
            return false;
        }
    }

    return true;
}
