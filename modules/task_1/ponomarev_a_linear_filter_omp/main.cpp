// A sequential version of linear filtration algorithm with Gauss kernel
// Copyright 2019 Ponomarev Alexey

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <ctime>
#include <fstream>
#include <random>


// constants
const int IMAGE_WIDTH = 16;
const int IMAGE_HEIGHT = 9;
// const int IMAGE_SIZE = IMAGE_WIDTH * IMAGE_WIDTH;
// const int IMAGE_COMPONENTS_COUNT = 3;
const char* GENERATED_IMAGE_NAME = "generated image";
const char* FILTERD_IMAGE_NAME = "generated image";
// const double MATH_PI = 3.14159265359;
const int KERNEL_RADIUS = 1;
const char* KERNEL_NULL_ERROR = "ERROR: kernel is null";
const char* KERNEL_ROW_NULL_ERROR = "ERROR: nullable kernel row#";
const char* ALLOCATING_IMAGE_MEMORY_ERROR = "ERROR: error with allocating memory for image";
const char* IMAGE_NULL_ERROR = "ERROR: image is null";
const double DEFAULT_SIGMA = 0.5;

struct Pixel {
    int r;
    int g;
    int b;
};

Pixel** allocateImageMemory(int width, int height) {
    // allocate memory
    Pixel** image = new Pixel*[height];
    for (int i = 0; i < height; i++) {
        image[i] = new Pixel[width];
    }
    return image;
}

// generating random image
Pixel** generateImage(int width, int height) {
    // allocate memory
    Pixel** image = allocateImageMemory(width, height);
    // init array
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image[i][j].r = std::rand() % 255;
            image[i][j].g = std::rand() % 255;
            image[i][j].b = std::rand() % 255;
        }
    }
    return image;
}


// generating Gauss kernel
double** generateGaussKernel(int radius, double sigma = DEFAULT_SIGMA) {
    // formula:  kernel(u, v) = (1/(2*pi*r^2))*e^(-(u^2 + v^2)/(2 * r^2)))
    /* defining kernel and allocating memory for it */
    double **kernel = new double*[radius * 2 + 1];
    for (int i = 0; i < 2 * radius + 1; i++) {
        kernel[i] = new double[2 * radius + 1];
    }

    /* init coefficients */
    // double k = 2.0 * radius * radius;
    double squreSigma = sigma * sigma;
    double doulbeSquereSigma = squreSigma * 2;
    // double k = MATH_PI * squreSigma;
    double norm = 0.0;

    /* summ processing */
    for (int u = -radius; u <= radius; u++) {
        for (int v = -radius; v <= radius; v++) {
            kernel[u + radius][v + radius] = exp(-(u * u + v * v) / doulbeSquereSigma);
            norm += kernel[u + radius][v + radius];
        }
    }

    /* normalizing */
    for (int u = -radius; u <= radius; u++) {
        for (int v = -radius; v <= radius; v++) {
            kernel[u + radius][v + radius] /= norm;
        }
    }
    return kernel;
}

bool deleteKernel(double **kernel, int radius) {
    if (kernel == NULL) {
        return false;
    }
    for (int i = 0; i < 2 * radius + 1; i++) {
        if (kernel[i] == NULL) {
            return false;
        }
        delete[] kernel[i];
    }
    return true;
    delete[] kernel;
}

// null checking function for image
bool isImageNull(Pixel** image, int imHeight) {
    if (image == NULL) {
        return true;
    }
    for (int i = 0; i < imHeight; i++) {
        if (image[i] == NULL) {
            return true;
        }
    }
    return false;
}


// wrapper function for null image checking
void checkIsImageNull(Pixel** image, int imHeight,
    const char* locationMessage = "", const char* errorMessage = IMAGE_NULL_ERROR) {
    if (isImageNull(image, imHeight)) {
        std::cout << "*** " << errorMessage << " for " << locationMessage << std::endl;
    }
}


// filter for one pixel
Pixel seqPixelFiltering(Pixel** genImage, int width, int height, double** kernel, int kerRadius, int x, int y) {
    Pixel outPixel;

    outPixel.r = 0;
    outPixel.b = 0;
    outPixel.g = 0;

    for (int u = -kerRadius; u <= kerRadius; u++) {
        for (int v = -kerRadius; v <= kerRadius; v++) {
            if (x + v >= 0 && x + v < width && y + u >= 0 && y + u < height) {
                outPixel.r += static_cast<int>(kernel[u + kerRadius][v + kerRadius] *
                    static_cast<double>(genImage[y + u][x + v].r));
                outPixel.g += static_cast<int>(kernel[u + kerRadius][v + kerRadius] *
                    static_cast<double>(genImage[y + u][x + v].g));
                outPixel.b += static_cast<int>(kernel[u + kerRadius][v + kerRadius] *
                    static_cast<double>(genImage[y + u][x + v].b));
            }
        }
    }

    return outPixel;
}

// sequential version
Pixel** seqFilter(Pixel** genImage, int width, int height, double** kernel, int kerRadius) {
    Pixel** filteredImage = allocateImageMemory(width, height);
    checkIsImageNull(genImage, height, " for generated image in seq filter ");
    checkIsImageNull(filteredImage, height, " for filtered image in seq filter ", ALLOCATING_IMAGE_MEMORY_ERROR);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            filteredImage[i][j] = seqPixelFiltering(genImage, width, height, kernel, kerRadius, j, i);
        }
    }

    return filteredImage;
}


bool deleteImage(int height, Pixel** image) {
    if (image == NULL) {
        return false;
    }
    for (int i = 0; i < height; i++) {
        if (image[i] == NULL) {
            return false;
        }
        delete[] image[i];
    }
    delete[] image;
    return true;
}

void tryDeleteImage(int height, Pixel** image, const char* imageName) {
    std::cout << "*** LOG: try deleting image with name = " << imageName << " ***" << std::endl;
    if (deleteImage(height, image)) {
        std::cout << "*** " << imageName << " successfully deleted ***" << std::endl;
    } else {
        std::cout << "*** error with deleting " << imageName << " ***" << std::endl;
    }
}

void tryDeleteKernel(int radius, double** kernel) {
    std::cout << "*** LOG: try deleting kernel ***" << std::endl;
    if (deleteKernel(kernel, radius)) {
        std::cout << "*** kernel successfully deleted ***" << std::endl;
    } else {
        std::cout << "*** error with deleting kernel ***" << std::endl;
    }
}

void printImage(Pixel** image, int width, int height) {
    checkIsImageNull(image, height, " in printing function ");
    std::cout << std::endl;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            std::cout << "("
                << image[i][j].r << ", "
                << image[i][j].g << ", "
                << image[i][j].b
                << ")";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}


void printKernel(double **kernel, int radius) {
    if (kernel == NULL) {
        std::cout << KERNEL_NULL_ERROR << std::endl;
        return;
    }

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Kernel: " << std::endl;

    for (int u = -radius; u <= radius; u++) {
        if (kernel[u + radius] == NULL) {
            std::cout << KERNEL_ROW_NULL_ERROR << u << std::endl;
            return;
        }
        for (int v = -radius; v <= radius; v++) {
            std::cout << kernel[u + radius][v + radius] << ", ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << std::endl;
}


/* entry point */
int main(int argc, char* argv[]) {
    // define variables
    Pixel **genImage = NULL;  // image before filtering
    Pixel **filteredImage = NULL;  // image after filtering
    int imWidth = IMAGE_WIDTH;  // image width
    int imHeight = IMAGE_HEIGHT;  // image height
    int kerRadius = KERNEL_RADIUS;
    double **kernel = NULL;  // Gauss kernel

    /* initialize random seed: */
    srand(static_cast<int>(time(NULL)));

    genImage = generateImage(imWidth, imHeight);
    printImage(genImage, imWidth, imHeight);

    kernel = generateGaussKernel(kerRadius);
    printKernel(kernel, kerRadius);

    filteredImage = seqFilter(genImage, imWidth, imHeight, kernel, kerRadius);
    printImage(filteredImage, imWidth, imHeight);

    tryDeleteImage(imHeight, genImage, GENERATED_IMAGE_NAME);
    tryDeleteImage(imHeight, filteredImage, FILTERD_IMAGE_NAME);
    tryDeleteKernel(kerRadius, kernel);

    return 0;
}




