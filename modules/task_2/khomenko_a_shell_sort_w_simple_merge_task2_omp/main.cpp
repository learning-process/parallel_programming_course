// Copyright 2019 Khomenko Aleksei
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <iostream>
#include <ctime>

bool is_sorted(double* arr, int siz) {
    double prev;
    while ( --siz ) {
        prev = *arr++;
        if ( *arr < prev ) return false;
    }
    return true;
}

int increment(int inc[], int size) {
    int p1, p2, p3, s;
    p1 = p2 = p3 = 1;
    s = -1;
    do {
        if (++s % 2) {
            inc[s] = 8 * p1 - 6 * p2 + 1;
        } else {
            inc[s] = 9 * p1 - 9 * p3 + 1;
            p2 *= 2;
            p3 *= 2;
        }
        p1 *= 2;
    } while (3 * inc[s] < size);
    return s > 0 ? --s : 0;
}

void ShellSort(double * a, int shift, int size) {
    int inc, i, j, seq[40];
    int s;
    s = increment(seq, size);
    while (s >= 0) {
        inc = seq[s--];
        for (i = inc + shift; i < size + shift; i++) {
            double temp = a[i];
            for (j = i - inc; (j >= shift) && (a[j] > temp); j -= inc) {
                a[j + inc] = a[j];
            }
            a[j + inc] = temp;
        }
    }
}
void insertSort(double* arr, int i, int length, int half) {
    double temp = 0;
    int j = 0;
    for (int f = half + i; f < length; f = f + half) {
        j = f;
        while (j > i && arr[j - half] > arr[j]) {
            temp = arr[j];
            arr[j] = arr[j - half];
            arr[j - half] = temp;
            j = j - half;
        }
    }
}

void ShellSort1(double* array, int shift, int length) {
    int h;
    int i = 0;
    for (h = length / 2; h > 0; h = h / 2) {
        for (i = 0; i < h; i++) {
            insertSort(array, i + shift, length + shift, h);
        }
    }
}

void Merge(double* array, int m, int n, int it1, int it2) {
    int i = 0, j = 0, k = 0;
    int size = m + n;
    double* temp;
    temp = new double[size];
    while (i < m && j < n) {
        if (array[i + it1] <= array[j + it2]) {
            temp[k] = array[i + it1];
            i++;
        } else {
            temp[k] = array[j + it2];
            j++;
        }
        k++;
    }
    if (i < m) {
        for (int p = i; p < m; p++) {
            temp[k] = array[p + it1];
            k++;
        }
    } else {
        for (int p = j; p < n; p++) {
            temp[k] = array[p + it2];
            k++;
        }
    }
    for (i = it1; i < it2 + n; i++)
        array[i] = temp[i - it1];
}

double ShellSortOmp(double* arr, int length, int n_threads) {
    double t1_1 , t2_1 , t1_2 , t2_2;
    double totalTime1, totalTime2;
    omp_set_num_threads(n_threads);
    int grain_size = 0;
    grain_size = length / (n_threads);
    std::cout << std::endl;
    std::cout << "Threads: " << n_threads << std::endl;
    std::cout << "Length of array: " << length << std::endl;
    std::cout << "Size of one part: " << grain_size << std::endl;
    t1_1 = omp_get_wtime();
#pragma omp parallel for schedule(static) shared(arr)
    for (int f = 0; f < n_threads; f++) {
        ShellSort(arr, f*grain_size, grain_size);
    }
    t2_1 = omp_get_wtime();
    totalTime1 = t2_1 - t1_1;
    std::cout << "Parallel Shell sorting time on "
    << n_threads << " threads: " << (totalTime1) << std::endl;
    int k = 0;
    int i = 0;
    t1_2 = omp_get_wtime();
    for (i = (n_threads / 2); i > 0; i = i/2) {
        int start1 = 0, start2 = start1 +
        grain_size*static_cast<int>(pow(2.0, k));
        int size1, size2 = 0;
        size1 = grain_size*static_cast<int>(pow(2.0, k));
        size2 = grain_size*static_cast<int>(pow(2.0, k));
        omp_set_num_threads(i);
#pragma omp parallel for schedule(static)\
private(start1, start2) shared(arr, size1, size2)
        for (int f = 0; f < i; f++) {
            int my_num = omp_get_thread_num();
            start1 = my_num*grain_size*static_cast<int>(pow(2.0, k + 1));
            start2 = start1 + grain_size*static_cast<int>(pow(2.0, k));
            Merge(arr, size1, size2, start1, start2);
        }
        k++;
    }
    t2_2 = omp_get_wtime();
    totalTime2 = t2_2 - t1_2;
    std::cout<< "Parallel merge sorting time on " << n_threads
    << " threads: " << (totalTime2) << std::endl;
    std::cout << std::endl;
    double totalTime = (totalTime1 + totalTime2);
    if (length < 100) {
        for (int i = 0; i < length; i++) {
            std::cout<< arr[i] <<" ";
        }
    }
    std::cout << std::endl;
    bool y = is_sorted(arr, length);
    std::cout << "arr is sorted ? " << y << std::endl;
    return totalTime;
}

int main(int argc, const char * argv[]) {
    srand((unsigned int)time(NULL));
    double t1Serial, t2Serial;
    int l = 10;
    int newLength = l;
    int length = 0;
    int threads = 2;
    int countZero = 0;
    int notCorrect = 0;
    double* arr;
    double* arrSerial;
    double* equal;
    double* tmp = new double[l];
    if (l < threads) {
        threads = 1;
    }
    if (l%threads != 0) {
        while (newLength%(threads) != 0) {
            newLength = newLength + 1;
            countZero++;
        }
        arr = new double[newLength];
        for (int i = 0; i < newLength; i++) {
            arr[i] = 0.0;
        }
        for (int i = 0; i < l; i++) {
            tmp[i] = std::rand() % 100;
        }
        for (int i = 0; i < l; i++) {
            arr[i] = tmp[i];
        }
        std::cout << "New length: " << l << " -> " << newLength << std::endl;
        if (l < 100) {
            for (int i = 0; i < newLength; i++) {
                std::cout << arr[i] << " ";
            }
            std::cout << std::endl;
        }
        length = newLength;
    } else {
        arr = new double[l];
        for (int i = 0; i < l; i++) {
            arr[i] = std::rand() % 100;
        }
        std::cout << "Length: " << l << std::endl;
        if (l < 100) {
            for (int i = 0; i < l; i++) {
                std::cout << arr[i] << " ";
            }
            std::cout << std::endl;
        }
        length = l;
    }
    arrSerial = new double[length];
    equal = new double[length];

    for (int i = 0; i < length; i++) {
        arrSerial[i] = arr[i];
    }
    double totalParallel = ShellSortOmp(arr, length, threads);
    t1Serial = omp_get_wtime();
    ShellSort(arrSerial, 0, length);
    t2Serial = omp_get_wtime();
    for (int i = 0; i < length; i++) {
        equal[i] = arrSerial[i] - arr[i];
        if (equal[i] != 0.0) {
            notCorrect++;
        }
    }
    if (notCorrect > 0) {
        std::cout << "Not correct !" << std::endl;
    } else {
        std::cout << "Correct !" << std::endl;
    }
    double totalSerial = t2Serial - t1Serial;
    std::cout << std::endl;
    std::cout << "Total parallel time on " << threads
    << " threads: " << totalParallel << std::endl;
    std::cout << "Total serial time: " << totalSerial << std::endl;
    std::cout << "Acceleration = " << totalSerial / totalParallel << std::endl;

    return 0;
}



