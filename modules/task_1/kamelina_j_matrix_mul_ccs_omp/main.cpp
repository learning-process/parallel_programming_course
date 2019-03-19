//  Copyright 2019 Kamelina Julia

#include <stdlib.h>
#include <vector>
#include <iostream>
#include <complex>
#include <random>

using std::vector;
using std::complex;

struct Matrix {
  vector<complex<double> > val;
  vector<int> rows;
  vector<int> pointer;
};

Matrix createMatrix(int N) {
  std::default_random_engine gen;
  std::uniform_int_distribution<int> dist(0, 10);

  Matrix new_matrix;
  new_matrix.pointer.resize(N + 1, -1);

  int k = 0;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      double re = (dist(gen) % 2)*dist(gen);
      double im = re*dist(gen);

      std::cout << re << " + " << im << "i ";

      if (re != 0 || im != 0) {
        new_matrix.val.push_back(complex<double>(re, im));
        new_matrix.rows.push_back(j);

        if (new_matrix.pointer[i] < 0) {
          new_matrix.pointer[i] = k;
        }
        k++;
      }
    }
    std::cout << std::endl;
  }
  new_matrix.pointer[N] = k;

  for (int i = N; i >= 1; i--) {
    if (new_matrix.pointer[i - 1] < 0) {
      new_matrix.pointer[i - 1] = new_matrix.pointer[i];
    }
  }

  return new_matrix;
}

void Transp(Matrix *a, int col) {
  Matrix a_T;
  a_T.pointer.resize(col + 1, 0);
  size_t nz_size = a->val.size();
  a_T.val.resize(nz_size, complex<double>(0, 0));
  a_T.rows.resize(nz_size);

  for (size_t i = 0; i < nz_size; i++) {
    a_T.pointer[a->rows[i] + 1]++;
  }
  for (int i = 1; i < col + 1; i++) {
    a_T.pointer[i] += a_T.pointer[i - 1];
  }

  for (int i = 0; i < col; i++) {
    int j = a->pointer[i];
    int k = a->pointer[i + 1];

    for (; j < k; j++) {
      int pos = a_T.pointer[a->rows[j]];

      while (a_T.val[pos] != complex<double>(0, 0)) {
        pos++;
      }
      a_T.val[pos] = a->val[j];
      a_T.rows[pos] = i;
    }
  }
  a->val = a_T.val;
  a->pointer = a_T.pointer;
  a->rows = a_T.rows;
}

void linMult(const Matrix &a, const Matrix &b, Matrix *c, int N) {
  c->pointer.resize(N + 1, 0);
  for (int i = 0; i < N; i++) {    //  cols b
    for (int j = 0; j < N; j++) {  //  cols a_T
      complex<double> sum = complex<double>(0, 0);

      for (int k = b.pointer[i]; k < b.pointer[i + 1]; k++) {
        for (int p = a.pointer[j]; p < a.pointer[j + 1]; p++) {
          if (a.rows[p] == b.rows[k]) {
            sum += a.val[p] * b.val[k];
            break;
          }
        }
      }

      std::cout << sum.real() << " + " << sum.imag() << "i ";

      if (sum != complex<double>(0, 0)) {
        c->val.push_back(sum);
        c->rows.push_back(j);
        c->pointer[i + 1]++;
      }
    }
    std::cout << std::endl;

    c->pointer[i + 1] += c->pointer[i];
  }
}

int main(int argc, char** argv) {
  int N = 5;
  if (argc == 2) {
    N = atoi(argv[1]);
  }
  std::cout << "Matrix A (printed as transposed)" << std::endl;
  Matrix a = createMatrix(N);
  std::cout << "Matrix B (printed as transposed)" << std::endl;
  Matrix b = createMatrix(N);
  Transp(&a, N);
  Matrix c;
  std::cout << "Matrix C (printed as transposed)" << std::endl;
  linMult(a, b, &c, N);
  return 0;
}
