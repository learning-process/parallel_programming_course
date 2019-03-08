// Copyright Kiselev Denis 2019

#include <cfloat>
#include <iostream>
#include <random>

#define DEFAULT_NPOINTS 10000
#define DEFAULT_LEFT_BORDER 0.0
#define DEFAULT_RIGHT_BORDER 1.0

/* This is a simple parabola function, which we
   will integrate using the Monte Carlo method. */
double parabola(double x) {
    return x * x;
}

int main(int argc, char *argv[]) {
    int nPoints;
    double leftBorder, rightBorder;

    if (argc == 1) {
        nPoints = DEFAULT_NPOINTS;
        leftBorder = DEFAULT_LEFT_BORDER;
        rightBorder = DEFAULT_RIGHT_BORDER;
    } else {
        nPoints = atoi(argv[1]);
        leftBorder = atof(argv[2]);
        rightBorder = atof(argv[3]);
    }

    // Creating a generator and distribution
    // for random double numbers in the range
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> distribution(
        leftBorder, std::nextafter(rightBorder, DBL_MAX));

    // The sum of the random values of our parabola
    double sum = 0.0;
    for (int i = 0; i < nPoints; i++) {
        double randX = distribution(generator);
        sum += parabola(randX);
    }

    // Calculation of the result according to the Monte Carlo method
    double avgValue = sum / nPoints;
    double result = (rightBorder - leftBorder) * avgValue;

    std::cout.precision(8);
    std::cout << "Result: " << std::fixed << result << std::endl;
    return 0;
}
