// Copyright 2019 Ermachenko Boris
#include <math.h>
#include <ctime>
#include <iostream>
// 1
const double PI = 3.1415;
double** getNewArr(int width, int height) {
    double** new_arrImage = new double*[height];
    for (int i = 0; i < height; i++) {
        new_arrImage[i] = new double[width];
    }
    return new_arrImage;
}
double** FilterGauss(double** arrImage, double** new_arrImage, int width, int height) {
    // double sum = 0;
    double koef1 = 1.398;
    double koef2 = 2 * koef1*koef1;
    int n = 3;
    double koef3;
    double color;
    double** w = getNewArr(n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            koef3 = (i*i + j * j) / koef2;
            w[i][j] = 1 / (sqrt(PI*koef2))*exp(koef3*(-1.0));
            // sum += w[i][j];
        }
        // for (int j = 0; j < n; j++)
        // {
        // w[i][j] = w[i][j] /sum;
        // }
    }
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            new_arrImage[i][j] = arrImage[i][j];
        }
    }
    for (int yi = n; yi < height - n; yi++) {
        for (int xj = n; xj < width - n; xj++) {
            for (int i = 0; i < n; i++) {
                color = 0;
                for (int j = 0; j < n; j++) {
                    color += w[i][j] * arrImage[static_cast<int>(i + yi)][static_cast<int>(j + xj)];
                }
            }
            new_arrImage[static_cast<int>(yi)][static_cast<int>(xj)] = color;
        }
    }
    return new_arrImage;
}

double** getImage(int width, int height) {
    srand((unsigned int)time(0));
    double** arrImage = getNewArr(width, height);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            arrImage[i][j] = static_cast<int>(std::rand()) % 256;
        }
    }
    return arrImage;
}
void ShowArr(double** arr, int width, int height) {
    // std::cout.precision(0);
    std::cout << "\n";
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // cout.precision(0);
            std::cout << arr[i][j] << " ";
        }
        std::cout << "\n";
    }
}
int main() {
    int height = 20;
    int width = 10;
    double** arrImage = getImage(width, height);
    double** new_arrImage = getNewArr(width, height);
    new_arrImage = FilterGauss(arrImage, new_arrImage, width, height);
    ShowArr(arrImage, width, height);
    ShowArr(new_arrImage, width, height);
    return 0;
}
