#include<iostream>
#include<omp.h>
double oneDimensionalIntegral(double x, double func(double, double), double a, double b, int n);
double Trapezoid(double func(double,double), double ax, double bx, double ay, double by, unsigned int n) {
    double step = (bx - ax) / n;
    double result=0;
    
    for (int i = 1; i < n-1; i++) {
        result += oneDimensionalIntegral(ax + i * step, func, ay, by, n);
    }
    result *= 2;
    result += oneDimensionalIntegral(ax + ax, func, ay, by, n) + oneDimensionalIntegral(ax + bx, func, ay, by, n);
    result *= (step / 2);
    return result;
}
double oneDimensionalIntegral(double x, double func(double, double), double a, double b, int n) {
    double step = (b-a) / n;
    double result = 0;
    for (int i = 1; i <= n-1; i++) {
        result += func(x, a + i * step);
    }
    result *= 2;
    result += func(x, a) + func(x, b);
    result *= (step / 2);
    return result;
}
double test_function1(double x, double y) {
    return x + y;
}

int main() {
    double start = omp_get_wtime();
    std::cout << Trapezoid(test_function1, 0, 10, 0, 10, 10000);
    std::cout << "\nTime: " << omp_get_wtime() - start;
}