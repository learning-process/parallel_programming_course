// Copyright 2019 Gladyshev Alexey
#include <tbb/tbb.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <cmath>
#include <algorithm>

typedef struct { int r; int g; int b; } myColor;
typedef struct { int x; int y; } myPoint;

int clamp(int value, int min, int max);
float* createGaussianKernel(int radius, float sigma);
myColor calculateNewPixelColor(myColor* sourceImage, int width, int height,
                             float* kernel, int radius, int i, int j);
void createRandomPicture(myColor* arrayImage, int width, int height);
myPoint getDecomposition(int n);
// Serial version
void processImage(myColor* sourceImage, myColor* resultImage, int width,
    int height, float* kernel, int kernelRadius);
void tbbProcessImage(myColor* sourceImage, myColor* resultImage, int width,
    int height, float* kernel, int kernelRadius, int threads);

int main() {
    srand(static_cast<unsigned int>(time(NULL)));
    int width = 3123, height = 4967;
    int radius = 1;
    float sigma = 6;
    int threads = 4;
    double serialTotal, tbbTotal;
    float* kernel = createGaussianKernel(radius, sigma);

    myColor* sourceArrayImage = new myColor[width * height];
    myColor* serialResultArrayImage = new myColor[width * height];
    myColor* tbbResultArrayImage = new myColor[width * height];
    createRandomPicture(sourceArrayImage, width, height);

    // tbb version
    tbb::task_scheduler_init init(threads);
    tbb::tick_count tbbStart = tbb::tick_count::now();
    tbbProcessImage(sourceArrayImage, tbbResultArrayImage,
        width, height, kernel, radius, threads);
    tbbTotal = (tbb::tick_count::now() - tbbStart).seconds();
    init.terminate();

    // serial version
    tbb::tick_count start = tbb::tick_count::now();
    processImage(sourceArrayImage, serialResultArrayImage,
        width, height, kernel, radius);
    serialTotal = (tbb::tick_count::now() - start).seconds();

    std::cout << std::endl << "(width, height): (" << width << ", ";
    std::cout << height << ")";
    std::cout << std::endl << "Threads:                " << threads;
    std::cout << std::endl << "Kernel radius:          " << radius;
    std::cout << std::endl << "Filtering time(serial): ";
    std::cout << serialTotal * 1000 << " (ms)";
    std::cout << std::endl << "Filtering time(tbb)   : ";
    std::cout << tbbTotal * 1000 << " (ms)";
    std::cout << std::endl << "Acceleration(tbb)     : ";
    std::cout << serialTotal / tbbTotal << std::endl;

    delete[]sourceArrayImage;
    delete[]tbbResultArrayImage;
    delete[]serialResultArrayImage;
    delete[]kernel;

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

    result.x = (k > m) ? k : m;
    result.y = (k < m) ? k : m;
    return result;
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
void tbbProcessImage(myColor* sourceImage, myColor* resultImage, int width,
    int height, float* kernel, int kernelRadius, int threads) {
    myPoint dec = getDecomposition(threads);
    size_t grainsize1 = height / dec.x;
    size_t grainsize2 = width / dec.y;

    tbb::parallel_for(
        tbb::blocked_range2d<int>(0, height, grainsize1, 0, width, grainsize2),
        [&](tbb::blocked_range2d<int> &r) {
        int i, j;
        for (i = r.rows().begin(); i != r.rows().end(); ++i) {
            for (j = r.cols().begin(); j != r.cols().end(); ++j) {
                resultImage[i * width + j] =
                    calculateNewPixelColor(sourceImage, width, height,
                        kernel, kernelRadius, i, j);
            }
        }
    });
}
