// Copyright 2019 Khomenko Aleksei
#include <stdio.h>
#include <stdlib.h>
#include <tbb/tbb.h>
#include <math.h>
#include <ctime>
#include <iostream>

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

void Merge(double* array, int sizeOfPart, int it1, int it2) {
    int i = 0, j = 0, k = 0;
    int size = sizeOfPart + sizeOfPart;
    double* temp;
    temp = new double[size];
    while (i < sizeOfPart && j < sizeOfPart) {
        if (array[i + it1] <= array[j + it2]) {
            temp[k] = array[i + it1];
            i++;
        } else {
            temp[k] = array[j + it2];
            j++;
        }
        k++;
    }
    if (i < sizeOfPart) {
        for (int p = i; p < sizeOfPart; p++) {
            temp[k] = array[p + it1];
            k++;
        }
    } else {
        for (int p = j; p < sizeOfPart; p++) {
            temp[k] = array[p + it2];
            k++;
        }
    }
    for (i = it1; i < it2 + sizeOfPart; i++)
        array[i] = temp[i - it1];
}

double ShellSortTBB(double* arr, int length, int n_threads) {
    tbb::tick_count tInit1_1 = tbb::tick_count::now();
    tbb::task_scheduler_init init(n_threads);
    tbb::tick_count tInit2_1  = tbb::tick_count::now();
    double totalSortTime, totalMergeTime;
    int grain_size = 0;
    grain_size = length / (n_threads);
    std::cout << std::endl;
    std::cout << "Threads: " << n_threads << std::endl;
    std::cout << "Length of array: " << length << std::endl;
    std::cout << "Size of one part: " << grain_size << std::endl;
    std::cout << std::endl;
    tbb::tick_count t1_1 = tbb::tick_count::now();
    tbb::parallel_for(tbb::blocked_range<int>(0, n_threads),
                      [=, &arr](const tbb::blocked_range<int> &r) {
                          for (int f = r.begin(); f != r.end(); f++) {
                              ShellSort(arr, f*grain_size, grain_size);
                          }
                      });

    init.terminate();
    tbb::tick_count t2_1 = tbb::tick_count::now();
    totalSortTime = ((t2_1 - t1_1)+(tInit2_1 - tInit1_1)).seconds();
    std::cout << "Parallel Shell sorting time on "
    << n_threads << " threads: " << (totalSortTime) << std::endl;

    int k = 0;
    int i = (n_threads / 2);
    int size = 0;
    tbb::tick_count t1_2 = tbb::tick_count::now();
    for (i = (n_threads / 2); i > 0; i = i/2) {
        size = grain_size*static_cast<int>(pow(2.0, k));
        init.initialize(i);
        tbb::parallel_for(tbb::blocked_range<int>(0, i),
                          [=, &arr, &size](const tbb::blocked_range<int> &r) {
                              for (int f = r.begin(); f != r.end(); f++) {
                                  int start1 = f*grain_size
                                  *static_cast<int>(pow(2.0, k + 1));
                                  int start2 = start1 +
                                  grain_size*static_cast<int>(pow(2.0, k));
                                  Merge(arr, size, start1, start2);
                              }
                          });
        k++;

        init.terminate();
    }

    tbb::tick_count t2_2 = tbb::tick_count::now();
    totalMergeTime = (t2_2 - t1_2).seconds();
    std::cout<< "Parallel merge sorting time on " << n_threads
    << " threads: " << (totalMergeTime) << std::endl;
    std::cout << std::endl;

    bool y = is_sorted(arr, length);
    std::cout << "arr is sorted ? " << y << std::endl;
    double totalTime = (totalSortTime + totalMergeTime);
    return totalTime;
}

int main(int argc, const char * argv[]) {
    srand((unsigned int)time(NULL));
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
    double totalParallel =  ShellSortTBB(arr, length, threads);

    tbb::tick_count t1Serial = tbb::tick_count::now();
    ShellSort(arrSerial, 0, length);
    tbb::tick_count t2Serial = tbb::tick_count::now();

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
    double totalSerial = (t2Serial - t1Serial).seconds();

    std::cout << std::endl;
    std::cout << "Total parallel time on " << threads <<
    " threads: " << totalParallel << std::endl;
    std::cout << "Total serial time: " << totalSerial << std::endl;
    std::cout << "Acceleration = " << totalSerial / totalParallel << std::endl;

    delete[]tmp;
    delete[]equal;
    delete[]arr;
    delete[]arrSerial;

    return 0;
}
