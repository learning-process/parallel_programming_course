// Copyright 2019 Chernenko Valery
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>

int* getRandImage(int width, int height) {
    int* image = new int[width * height];
    for (int i = 0; i < width*height; i++)
        image[i] = std::rand() % 256;
    return image;
}

int clamp(int x, int min, int max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

int getResultPixel(int* image, int width, int height, int x, int y, float* filter, float coeff) {
    int resultPixel = 0;
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
            float res = image[clamp(x + i, 0, width - 1) + clamp(y + j, 0, height - 1) * width] * filter[i+1 + (j+1)*3];
            resultPixel += static_cast<int>(res);
        }
    resultPixel = clamp(static_cast<int>(resultPixel * coeff), 0, 255);
    return resultPixel;
}

int* getResultImage(int* image, int width, int height, float* filter, float coeff) {
    int* resultImage = new int[width * height];
    for (int i = 0; i < width; i++)
        for (int j = 0; j < height; j++) {
            resultImage[i + j*width] = getResultPixel(image, width, height, i, j, filter, coeff);
        }
    return resultImage;
}

int main(int argv, char** argc) {
    srand((unsigned int)time(0));
    setlocale(LC_ALL, "Russian");

    int* imageR;
    int* imageG;
    int* imageB;
    int width, height;

    float* filter = new float[9];
    float coeff = 0;
    filter[0] = 1; filter[1] = 1; filter[2] = 1;
    filter[3] = 1; filter[4] = 1; filter[5] = 1;
    filter[6] = 1; filter[7] = 1; filter[8] = 1;
    for (int i = 0; i < 9; i++)
        coeff += filter[i];
    coeff = 1 / coeff;

    std::ifstream fin;
    fin.open("startImageText.txt");
    if (!fin.is_open()) {
        std::cout << "File not found! Generate rand image 900 x 600" << std::endl;
        width = 900;
        height = 600;
        imageR = getRandImage(width, height);
        imageG = getRandImage(width, height);
        imageB = getRandImage(width, height);
        std::cout << "Image generated" << std::endl;
    } else {
        fin >> width >> height;
        imageR = new int[width*height];
        imageG = new int[width*height];
        imageB = new int[width*height];
        for (int i = 0; i < width * height; i++)
            fin >> imageR[i] >> imageG[i] >> imageB[i];
    }
    fin.close();

    clock_t startTime = clock();
    std::cout << "Start work" << std::endl;
    int* ResultImageR = getResultImage(imageR, width, height, filter, coeff);
    int* ResultImageG = getResultImage(imageG, width, height, filter, coeff);
    int* ResultImageB = getResultImage(imageB, width, height, filter, coeff);
    clock_t finishTime = clock();
    std::cout << "Finish work. Elapsed time = " << finishTime - startTime << " ms" << std::endl;

    /*
    std::ofstream fout;
    fout.open("endImageText.txt");
    fout << width << " " << height << std::endl;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            fout << ResultImageR[i + j*width] << " ";
            fout << ResultImageG[i + j*width] << " ";
            fout << ResultImageB[i + j*width] << " ";
        }
        fout << std::endl;
    }
    fout.close();
    */

    delete imageR;
    delete imageG;
    delete imageB;
    delete ResultImageR;
    delete ResultImageG;
    delete ResultImageB;
}
