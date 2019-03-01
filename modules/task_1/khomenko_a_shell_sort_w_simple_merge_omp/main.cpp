// Copyright 2019 Khomenko Aleksei
#include <stdlib.h>
#include <omp.h>
#include <iostream>
#include <ctime>




void ShellSort(int length, double array[]) {
    int i, j, step;
    double tmp;
    for (step = length / 2; step > 0; step /= 2)
        for (i = step; i < length; i++) {
            tmp = array[i];
            for (j = i; j >= step; j -= step) {
                if (tmp < array[j - step])
                    array[j] = array[j - step];
                else
                    break;
            }
            array[j] = tmp;
        }
}

void InsertSort(double arr[], int i, int length, int half) {
    double tmp = 0;
    int j = 0;
    for (int f = half + i; f < length; f = f + half) {
        j = f;
        while (j > i && arr[j - half] > arr[j]) {
            tmp = arr[j];
            arr[j] = arr[j - half];
            arr[j - half] = tmp;
            j = j - half;
        }
    }
}

void ShellSort_v2(int length, double array[]) {
    int h;
    int i = 0;
    for (h = length / 2; h > 0; h = h / 2) {
        for (i = 0; i < h; i++) {
            InsertSort(array, i, length, h);
        }
    }
}


void CreateArray(double arr[], int lenght) {
    srand((unsigned int)time(NULL));
    for (int i = 0; i < lenght; i++) {
        arr[i] = std::rand() % 10;
    }
}

int main(int argc, char * argv[]) {
    double firstArray[10];
    double secondArray[10];
    CreateArray(firstArray, 10);
    for (int i = 0; i < 10 ; ++i) {
        secondArray[i] = firstArray[i];
    }
    ShellSort_v2(10, firstArray);
    std::cout <<"firstArray after ShellSort_v2: ";
    for (int i = 0; i < 10; i++) {
        std::cout << firstArray[i] << " ";
    }
    std::cout <<std::endl;
    ShellSort(10, secondArray);
    std::cout <<"secondArray after ShellSort:   ";
    for (int i = 0; i < 10; i++) {
        std::cout << secondArray[i] << " ";
    }
    std::cout <<std::endl;
    return 0;
}

