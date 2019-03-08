// Copyright 2019 Bederdinov Daniil
#define kLength 80
#include <omp.h>
#include <cmath>
#include <ctime>
#include <iostream>
#include <vector>

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

enum elemType {
    EVEN,
    ODD
};

void quickSort(int *arr, int size) {
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
        quickSort(arr, j + 1);
    if (i < size)
        quickSort(&arr[i], size - i);
}

void mergeAndSort(const std::vector<int> vec1, const std::vector<int> vec2, int *write_to) {
    int i = 0, j = 0;
    int size1 = vec1.size(), size2 = vec2.size();

    while (i < size1 && j < size2) {
        write_to[i + j] = vec1[i];
        write_to[i + j + 1] = vec2[j];
        i++;
        j++;
    }

    while (i < size1) {
        write_to[size2 + i] = vec1[i];
        i++;
    }
    while (j < size2) {
        write_to[size1 + j] = vec2[j];
        j++;
    }

    i = 1;  // Первый элемент проверять не нужно
    while (i < size1 + size2 - 1) {
        if (write_to[i] > write_to[i + 1]) {
            j = write_to[i];
            write_to[i] = write_to[i + 1];
            write_to[i + 1] = j;
        }
        i++;
    }
}

void selectElements(elemType type, const int *arr1, int size1, const int *arr2, int size2, std::vector<int> *result) {
    int i, j;
    if (type == EVEN)
        i = 0, j = 0;
    else
        i = 1, j = 1;
    result->reserve(size1 + size2);
    while (i < size1 && j < size2) {
        if (arr1[i] <= arr2[j]) {
            result->push_back(arr1[i]);
            i += 2;
        } else {
            result->push_back(arr2[j]);
            j += 2;
        }
    }
    if (i >= size1) {
        while (j < size2) {
            result->push_back(arr2[j]);
            j += 2;
        }
    } else {
        while (i < size1) {
            result->push_back(arr1[i]);
            i += 2;
        }
    }
}

void printArray(int *array, const int size) {
    for (int i = 0; i < size; i++) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {
    // if (argc != 2) {
    //     std::cout << "Use ./main [threads]" << std::endl;
    //     return 1;
    // }

    int size = kLength, threads = 4;  // atoi(argv[1]);
    int arr[kLength];
    fillArray(arr, kLength);
    shuffle(arr, kLength);
    std::cout << "Unsorted array:" << std::endl;
    printArray(arr, kLength);
    std::cout << std::endl;

    double time = omp_get_wtime();
    int step;  // Переменная для хранения шага (step = 2^(N-1))
    std::vector<int> *tempArray = new std::vector<int>[threads];
    int *shift = new int[threads];  // shift - массив сдвигов
    int *chunk = new int[threads];  // chunk - массив, содержащий размеры частей массива
#pragma omp parallel shared(arr, step, shift, chunk, tempArray) num_threads(threads)
    {
        int tid, thread_index;  // tid - переменная для хранения ID текущего потока,
                                // thread_index - определяет необходимый сдвиг для получения
                                // парного потока (на шаге 1 = 1, на шаге 2 = 2 и т.д.)
        tid = omp_get_thread_num();

        /* Распределение частей исходного массива по потокам и сортировка данных частей */

        shift[tid] = tid * (size / threads);
        chunk[tid] = (tid == threads - 1) ? (size / threads) +
        (size % threads) : size / threads;
        quickSort(arr + shift[tid], chunk[tid]);
#pragma omp barrier  // Ожидаем, пока все потоки отсортируют свою часть массива

        step = 1;
        while (step < threads) {
            /* На каждом шаге выбираем четные и нечетные элементы из парных потоков, записываем в tempArray */
            thread_index = static_cast<int>(pow(2, step - 1));

            if (tid % (thread_index * 2) == 0) {
                selectElements(EVEN, arr + shift[tid], chunk[tid], arr + shift[tid +
                thread_index], chunk[tid + thread_index], &tempArray[tid]);
            } else if (tid % thread_index == 0) {
                selectElements(ODD, arr + shift[tid], chunk[tid], arr +
                shift[tid - thread_index], chunk[tid - thread_index], &tempArray[tid]);
            }
#pragma omp barrier  // Ожидаем выполнения данной части всеми потоками
                     /* Производим слияние и сортировку tempArray в парных потоках */
            if (tid % (thread_index * 2) == 0) {
                mergeAndSort(tempArray[tid], tempArray[tid + thread_index], arr + shift[tid]);
                chunk[tid] += chunk[tid + thread_index];
                tempArray[tid].clear();
                tempArray[tid].shrink_to_fit();
                tempArray[tid + thread_index].clear();
                tempArray[tid + thread_index].shrink_to_fit();
            }
#pragma omp single
            {
                step *= 2;  // Переходим на следующий шаг первым освободившимся потоком
            }
#pragma omp barrier  // Ждем пока слияние и сортировку выполнят все потоки
        }
    }
    delete[] tempArray;
    delete[] chunk;
    delete[] shift;

    time = omp_get_wtime() - time;
    printf("OpenMP time: %f\n", time);

    printArray(arr, kLength);
    return 0;
}
