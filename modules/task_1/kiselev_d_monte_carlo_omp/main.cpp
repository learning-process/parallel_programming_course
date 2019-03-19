// Copyright Kiselev Denis 2019

#include <cfloat>
#include <iostream>
#include <random>

#define DEFAULT_NPOINTS 10000

// Ellipsoid options
#define ELLPS_A 1.0
#define ELLPS_B 2.0
#define ELLPS_C 3.0

// Limits of integration
#define X1 -1.0
#define X2 1.0

#define Y1 -2.0
#define Y2 2.0

#define Z1 -3.0
#define Z2 3.0

// This is a simple elipsoid function, which we
// will integrate using the Monte Carlo method.
double ellipsoid(double x, double y, double z) {
    return - 1.0 + pow(x / ELLPS_A, 2)
                 + pow(y / ELLPS_B, 2)
                 + pow(z / ELLPS_C, 2);
}

int main(int argc, char *argv[]) {
    // Creating a generator and distributions
    // for random double numbers in the ranges
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<> xAxisDistr(X1, X2);
    std::uniform_real_distribution<> yAxisDistr(Y1, Y2);
    std::uniform_real_distribution<> zAxisDistr(Z1, Z2);

    int nPoints = (argc == 1) ? DEFAULT_NPOINTS : atoi(argv[1]);

    // Ellipsoid hit counting
    int nHitsOnEllipsoid = 0;
    for (int i = 0; i < nPoints; i++) {
        double value = ellipsoid(xAxisDistr(generator),
                                 yAxisDistr(generator),
                                 zAxisDistr(generator));
        if (value <= 0) nHitsOnEllipsoid++;
    }

    // Calculation of the result according to the Monte Carlo method
    double avgValueOfHits = static_cast<double>(nHitsOnEllipsoid) / nPoints;
    double res = (X2 - X1) * (Y2 - Y1) * (Z2 - Z1) * avgValueOfHits;
    double realRes = 4.0 / 3.0 * std::acos(-1) * ELLPS_A * ELLPS_B * ELLPS_C;

    std::cout.precision(8);
    std::cout << "Result: " << std::fixed << res << std::endl
              << "Real result: " << std::fixed << realRes << std::endl
              << "Error: " << std::fixed << res - realRes << std::endl;
    return 0;
}
