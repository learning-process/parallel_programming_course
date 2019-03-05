// Copyright 2019 Panova Elena
#include <omp.h>
#include <iostream>
#include <random>

const int N = 500;  // default matrix size
const double EPS = 1e-5;  // default error

class Vector {
    int size = 0;
    double *data = 0;

 public:
    Vector() {
        data = 0;
    }
    explicit Vector(int _size) {
        Initialize(_size);
    }
    Vector(const Vector& v) {
        size = v.size;
        data = new double[size];
        for (int i = 0; i < size; i++)
            data[i] = v.data[i];
    }
    ~Vector() {
        Clear();
    }
    void Initialize(int _size) {
        size = _size;
        Clear();
        data = new double[size];
        for (int i = 0; i < size; i++)
            data[i] = 0;
    }
    void Clear() {
        if (data)
            delete[] data;
        data = 0;
    }
    int getSize() const {
        return size;
    }
    double& operator[](int i) {
        return data[i];
    }
    double get(int i) const {
        return data[i];
    }
    Vector& operator=(const Vector& v) {
        if (this != &v) {
            if (v.size != size) {
                Clear();
                data = new double[v.size];
                size = v.size;
            }
            for (int i = 0; i < size; i++)
                data[i] = v.data[i];
        }
        return *this;
    }
    Vector operator+(const Vector& b) const {
        Vector res(size);
        for (int i = 0; i < size; i++)
            res[i] = data[i] + b.data[i];
        return res;
    }
    friend Vector operator*(double a, const Vector& b) {
        Vector res(b.size);
        for (int i = 0; i < b.size; i++)
            res[i] = a * b.data[i];
        return res;
    }
    Vector operator-(const Vector& b) const {
        return (*this) + (-1)*b;
    }
    friend double Dot(const Vector& a, const Vector& b) {
        double res = 0;
        for (int i = 0; i < a.size; i++)
            res += a.data[i] * b.data[i];
        return res;
    }
    double Norm() const {
        double max = 0;
        for (int i = 0; i < size; i++)
            if (max < std::abs(data[i]))
                max = std::abs(data[i]);
        return max;
    }
};

class Matrix {
    int size = 0;
    double *data = 0;

 public:
    Matrix() {
        data = 0;
    }
    explicit Matrix(int _size) {
        Initialize(_size);
    }
    ~Matrix() {
        Clear();
    }
    void Initialize(int _size) {
        size = _size;
        Clear();
        data = new double[size*size];
        for (int i = 0; i < size*size; i++)
            data[i] = 0;
    }
    void Clear() {
        if (data)
            delete[] data;
        data = 0;
    }
    Matrix& operator=(const Matrix& v) {
        if (this != &v) {
            if (v.size != size) {
                Clear();
                data = new double[v.size*v.size];
                size = v.size;
            }
            for (int i = 0; i < size; i++)
                for (int j = 0; j < size; j++)
                    data[i] = v.data[i];
        }
        return *this;
    }
    double& operator()(int i, int j) const {
        return data[i*size + j];
    }
    Vector operator*(const Vector& v) const {
        Vector res(size);
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                res[i] += (*this)(i, j)*v.get(j);
        return res;
    }
};

Vector conjGradientMethod(const Matrix& A, const Vector& b, const Vector& x0,
    double eps, Vector* rn, int* counter) {
    Vector x = x0;
    Vector r = b - A * x0;
    Vector z = r;
    while (r.Norm() / b.Norm() > eps) {
        Vector Az = A * z;
        double alpha = Dot(r, r) / Dot(Az, z);
        x = x + alpha * z;
        r = r - alpha * Az;
        Vector r1 = b - A*x;
        double beta = -Dot(Az, r) / Dot(Az, z);
        z = r + beta * z;
        (*counter)++;
    }
    *rn = r;
    return x;
}

bool parseArgs(int argc, char* argv[], Matrix* A, Vector* b, double* eps) {
    int size = 0;
    if (argc < 3) {
        size = N;
        *eps = EPS;
    } else {
        size = atoi(argv[1]);
        *eps = atof(argv[2]);
    }
    A->Initialize(size);
    b->Initialize(size);
    if (argc > 3) {  // read matrix and vector
        if (argc - 3 != size * size + size) {
            std::cout << "wrong args\n";
            return false;
        }
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                (*A)(i, j) = atof(argv[i*size + j + 3]);
        for (int i = 0; i < size; i++)
            (*b)[i] = atof(argv[size*size + i + 3]);
    } else {  // random matrix and vector
        std::uniform_real_distribution<double> undist(0, 1);
        std::default_random_engine re;
        for (int i = 0; i < size; i++)
            for (int j = 0; j <= i; j++)
                (*A)(j, i) = (*A)(i, j) = undist(re);
        for (int i = 0; i < size; i++)
            (*b)[i] = undist(re);
    }
    return true;
}

void printRes(const Vector& res, const Matrix& A, const Vector& b,
    const Vector& r, int c, int argc) {
    std::cout << "Matrix size is " << b.getSize() << "\n";
    std::cout << "Relative discrepancy is " <<
        r.Norm() / b.Norm() << "\n";
    std::cout << "Number of iterations is " << c << "\n";
    if (argc > 3) {
        for (int i = 0; i < res.getSize(); i++)
            std::cout << res.get(i) << " ";
        std::cout << "\n";
    }
}

int main(int argc, char* argv[]) {
    Matrix A;
    Vector b;
    double eps;
    if (parseArgs(argc, argv, &A, &b, &eps)) {
        Vector x0(b.getSize());
        int counter = 0;
        Vector r(b.getSize());
        double t1 = omp_get_wtime();
        Vector res = conjGradientMethod(A, b, x0, eps, &r, &counter);
        double t2 = omp_get_wtime();
        std::cout << "Time of sequential version is " << t2 - t1 << std::endl;
        printRes(res, A, b, r, counter, argc);
    }
}
