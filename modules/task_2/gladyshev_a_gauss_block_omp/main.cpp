// Copyright 2019 Gladyshev Alexey
#include <omp.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <cmath>
#include <algorithm>

typedef struct { int r; int g; int b; } myColor;
typedef struct { int x; int y; } myPoint;
typedef struct { myPoint start; myPoint finish; } myIndex;

int clamp(int value, int min, int max);
float* createGaussianKernel(int radius, float sigma);
myColor calculateNewPixelColor(myColor* sourceImage, int width, int height,
                             float* kernel, int radius, int i, int j);
void createRandomPicture(myColor* arrayImage, int width, int height);
myPoint getDecomposition(int n);
int* getLength(int length, int qElements);
void getIndexes(myIndex* indexArray, int width, int height, int threads);
void ompProcessImage_block(myColor* sourceImage, myColor* resultImage,
  int width, int height, float* kernel, int kernelRadius, myIndex* indexArray);
// Serial version
void processImage(myColor* sourceImage, myColor* resultImage, int width,
    int height, float* kernel, int kernelRadius);

int main() {
    srand(static_cast<unsigned int>(time(NULL)));
    int width = 3123, height = 4967;
    int radius = 1;
    float sigma = 6;
    int threads = 4;
    double start, ompTotal, serialTotal;
    float* kernel = createGaussianKernel(radius, sigma);

    myColor* sourceArrayImage = new myColor[width * height];
    myColor* ompResultArrayImage = new myColor[width * height];
    myColor* serialResultArrayImage = new myColor[width * height];
    createRandomPicture(sourceArrayImage, width, height);

    // Parallel version
    omp_set_num_threads(threads);
    myIndex* indexArray = new myIndex[threads];
    getIndexes(indexArray, width, height, threads);

    start = omp_get_wtime();
    ompProcessImage_block(sourceArrayImage, ompResultArrayImage,
                          width, height, kernel, radius, indexArray);
    ompTotal = omp_get_wtime() - start;

    // Serial version
    start = omp_get_wtime();
    processImage(sourceArrayImage, ompResultArrayImage,
                 width, height, kernel, radius);
    serialTotal = omp_get_wtime() - start;

    std::cout << std::endl << "(width, height): (" << width << ", ";
    std::cout << height << ")";
    std::cout << std::endl << "Threads:                " << threads;
    std::cout << std::endl << "Kernel radius:          " << radius;
    std::cout << std::endl << "Filtering time(paral) : " << ompTotal * 1000;
    std::cout << " (ms)";
    std::cout << std::endl << "Filtering time(serial): " << serialTotal * 1000;
    std::cout << " (ms)";
    std::cout << std::endl << "Acceleration:           ";
    std::cout << serialTotal / ompTotal << std::endl;

    delete[]sourceArrayImage;
    delete[]ompResultArrayImage;
    delete[]serialResultArrayImage;
    delete[]kernel;
    delete[]indexArray;

    return 0;
}

int clamp(int value, int min, int max) {
    if (value < min)    return min;
    if (value > max)    return max;
    return value;
}

float* createGaussianKernel(int radius, float sigma) {
    int size = 2 * radius + 1;
    float* kernel = new float[size * size];
    float norm = 0;

    for (int i = -radius; i <= radius; i++)
        for (int j = -radius; j <= radius; j++) {
            kernel[(i + radius) * size + (j + radius)] =
                static_cast<float>(
                    (std::exp(-(i * i + j * j) / (sigma * sigma))));
            norm += kernel[(i + radius) * size + (j + radius)];
        }

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            kernel[i * size + j] /= norm;

    return kernel;
}

myColor calculateNewPixelColor(myColor* sourceImage, int width, int height,
                             float* kernel, int radius, int i, int j) {
    float resultR = 0;
    float resultG = 0;
    float resultB = 0;

    for (int l = -radius; l <= radius; l++) {
        for (int k = -radius; k <= radius; k++) {
            int idX = clamp(i + l, 0, height - 1);
            int idY = clamp(j + k, 0, width - 1);

            myColor neighborColor = sourceImage[idX * width + idY];

            resultR += neighborColor.r *
                kernel[(k + radius) * radius + (l + radius)];
            resultG += neighborColor.g *
                kernel[(k + radius) * radius + (l + radius)];
            resultB += neighborColor.b *
                kernel[(k + radius) * radius + (l + radius)];
        }
    }

    myColor result;
    result.r = clamp(static_cast<int>(resultR), 0, 255);
    result.g = clamp(static_cast<int>(resultG), 0, 255);
    result.b = clamp(static_cast<int>(resultB), 0, 255);
    return result;
}

void createRandomPicture(myColor* arrayImage, int width, int height) {
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++) {
            arrayImage[i * width + j].r = std::rand() % 256;
            arrayImage[i * width + j].g = std::rand() % 256;
            arrayImage[i * width + j].b = std::rand() % 256;
        }
}

myPoint getDecomposition(int n) {
    //  n = k * m
    // |k - m| -> min
    myPoint result;
    int m, k;

    m = static_cast<int>(sqrt(n));
    while (n % m != 0) m--;
    k = n / m;

    result.x = std::max(k, m);
    result.y = std::min(k, m);
    return result;
}

int* getLength(int length, int qElements) {
    int eachLength = length / qElements;
    int tailLength = length % qElements;
    int* lengthArray = new int[qElements];

    for (int i = 0; i < qElements; i++)
        lengthArray[i] = eachLength;

    int i = 0;
    while (tailLength != 0) {
        lengthArray[i % qElements]++;
        tailLength--;
        i++;
    }

    return lengthArray;
}

void getIndexes(myIndex* indexArray, int width, int height, int threads) {
    myPoint decomposition = getDecomposition(threads);
    int* heightLength = getLength(height, decomposition.x);
    int* widthLength = getLength(width, decomposition.y);

    int currentHeight = 0;
    for (int i = 0; i < decomposition.x; i++) {
        int currentWidth = 0;
        for (int j = 0; j < decomposition.y; j++) {
            myPoint start, finish;
            start.x = currentWidth;
            start.y = currentHeight;

            finish.x = currentWidth + widthLength[j];
            finish.y = currentHeight + heightLength[i];

            indexArray[i * decomposition.y + j].start = start;
            indexArray[i * decomposition.y + j].finish = finish;

            currentWidth += widthLength[j];
        }
        currentHeight += heightLength[i];
    }

    delete[]heightLength;
    delete[]widthLength;
}

void ompProcessImage_block(myColor* sourceImage, myColor* resultImage,
                           int width, int height, float* kernel,
                           int kernelRadius, myIndex* indexArray) {
#pragma omp parallel
        {
            int myid = omp_get_thread_num();
            int startHeight = indexArray[myid].start.y;
            int finishHeight = indexArray[myid].finish.y;
            int startWidth = indexArray[myid].start.x;
            int finishWidth = indexArray[myid].finish.x;

            for (int i = startHeight; i < finishHeight; i++)
                for (int j = startWidth; j < finishWidth; j++)
                    resultImage[i * width + j] =
                        calculateNewPixelColor(sourceImage, width, height,
                                               kernel, kernelRadius, i, j);
        }
}

// Serial version
void processImage(myColor* sourceImage, myColor* resultImage, int width,
    int height, float* kernel, int kernelRadius) {
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            resultImage[i * width + j] =
            calculateNewPixelColor(sourceImage, width, height,
                kernel, kernelRadius, i, j);
}
