// Copyright 2019 Zinoviev Vladimir

#include <omp.h>
#include <iostream>
#include <random>

inline double* matrix_sum(double* A, double* B, int size) {
  double * C = new double[size];
  for (int i = 0; i < size; ++i) {
    C[i] = A[i] + B[i];
  }
  return C;
}

inline double* matrix_sub(double* A, double* B, int size) {
  double * C = new double[size];
  for (int i = 0; i < size; ++i) {
    C[i] = A[i] - B[i];
  }
  return C;
}

inline double* matrix_mul(double* A, double* B, int n) {
  int size = n * n;
  double * C = new double[size];
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      C[i*n + j] = 0;
      for (int k = 0; k < n; ++k)
        C[i*n + j] += A[i*n + k] * B[k*n + j];
    }
  }
  return C;
}

double* shtrassen_mul(double* A, double* B, int n) {
  if (n > 64) {
    int size = n * n, n_2 = n / 2, size_4 = size / 4;
    double ** A_ = new double*[4];
    double ** B_ = new double*[4];

    for (int k = 0; k < 4; ++k) {
      A_[k] = new double[size_4];
      B_[k] = new double[size_4];
    }

    for (int i = 0; i < n_2; ++i) {
      for (int j = 0; j < n_2; ++j) {
        A_[0][i*n_2 + j] = A[i*n + j];                      // A_11
        A_[1][i*n_2 + j] = A[i*n + j + n / 2];              // A_12
        A_[2][i*n_2 + j] = A[(i + n / 2) * n + j];          // A_21
        A_[3][i*n_2 + j] = A[(i + n / 2) * n + j + n / 2];  // A_22
        B_[0][i*n_2 + j] = B[i*n + j];                      // B_11
        B_[1][i*n_2 + j] = B[i*n + j + n / 2];              // B_12
        B_[2][i*n_2 + j] = B[(i + n / 2) * n + j];          // B_21
        B_[3][i*n_2 + j] = B[(i + n / 2) * n + j + n / 2];  // B_22
      }
    }

    double ** P = new double*[7];
    for (int k = 0; k < 7; ++k) {
      P[k] = new double[size_4];
    }

    P[0] = shtrassen_mul(matrix_sum(A_[0], A_[3], size_4),
      matrix_sum(B_[0], B_[3], size_4), n_2);
    P[1] = shtrassen_mul(matrix_sum(A_[2], A_[3], size_4),
      B_[0], n_2);
    P[2] = shtrassen_mul(A_[0],
      matrix_sub(B_[1], B_[3], size_4), n_2);
    P[3] = shtrassen_mul(A_[3],
      matrix_sub(B_[2], B_[0], size_4), n_2);
    P[4] = shtrassen_mul(matrix_sum(A_[0], A_[1], size_4),
      B_[3], n_2);
    P[5] = shtrassen_mul(matrix_sub(A_[2], A_[0], size_4),
      matrix_sum(B_[0], B_[1], size_4), n_2);
    P[6] = shtrassen_mul(matrix_sub(A_[1], A_[3], size_4),
      matrix_sum(B_[2], B_[3], size_4), n_2);
    double** C = new double*[4];
    for (int k = 0; k < 4; ++k) {
      C[k] = new double[size_4];
    }

    for (int i = 0; i < size_4; ++i) {
      C[0][i] = P[0][i] + P[3][i] - P[4][i] + P[6][i];
      C[1][i] = P[2][i] + P[4][i];
      C[2][i] = P[1][i] + P[3][i];
      C[3][i] = P[0][i] - P[1][i] + P[2][i] + P[5][i];
    }

    double* res = new double[size];

    for (int i = 0; i < n_2; ++i)
      for (int j = 0; j < n_2; ++j) {
        res[i*n + j] = C[0][i*n_2 + j];                      // C_11
        res[i*n + j + n / 2] = C[1][i*n_2 + j];              // C_12
        res[(i + n / 2) * n + j] = C[2][i*n_2 + j];          // C_21
        res[(i + n / 2) * n + j + n / 2] = C[3][i*n_2 + j];  // C_22
      }
    return res;
  } else {
    return matrix_mul(A, B, n);
  }
}

double* shtrassen_mul_omp(double* A, double* B, int n) {
  if (n > 64) {
    int size = n * n, n_2 = n / 2, size_4 = size / 4;
    double ** A_ = new double*[4];
    double ** B_ = new double*[4];

    for (int k = 0; k < 4; ++k) {
      A_[k] = new double[size_4];
      B_[k] = new double[size_4];
    }

    for (int i = 0; i < n_2; ++i) {
      for (int j = 0; j < n_2; ++j) {
        A_[0][i*n_2 + j] = A[i*n + j];                      // A_11
        A_[1][i*n_2 + j] = A[i*n + j + n / 2];              // A_12
        A_[2][i*n_2 + j] = A[(i + n / 2) * n + j];          // A_21
        A_[3][i*n_2 + j] = A[(i + n / 2) * n + j + n / 2];  // A_22
        B_[0][i*n_2 + j] = B[i*n + j];                      // B_11
        B_[1][i*n_2 + j] = B[i*n + j + n / 2];              // B_12
        B_[2][i*n_2 + j] = B[(i + n / 2) * n + j];          // B_21
        B_[3][i*n_2 + j] = B[(i + n / 2) * n + j + n / 2];  // B_22
      }
    }

    double ** P = new double*[7];
    for (int k = 0; k < 7; ++k) {
      P[k] = new double[size_4];
    }
#pragma omp parallel sections
    {
#pragma omp section
      P[0] = shtrassen_mul(matrix_sum(A_[0], A_[3], size_4),
        matrix_sum(B_[0], B_[3], size_4), n_2);
#pragma omp section
      P[1] = shtrassen_mul(matrix_sum(A_[2], A_[3], size_4),
        B_[0], n_2);
#pragma omp section
      P[2] = shtrassen_mul(A_[0],
        matrix_sub(B_[1], B_[3], size_4), n_2);
#pragma omp section
      P[3] = shtrassen_mul(A_[3],
        matrix_sub(B_[2], B_[0], size_4), n_2);
#pragma omp section
      P[4] = shtrassen_mul(matrix_sum(A_[0], A_[1], size_4),
        B_[3], n_2);
#pragma omp section
      P[5] = shtrassen_mul(matrix_sub(A_[2], A_[0], size_4),
        matrix_sum(B_[0], B_[1], size_4), n_2);
#pragma omp section
      P[6] = shtrassen_mul(matrix_sub(A_[1], A_[3], size_4),
        matrix_sum(B_[2], B_[3], size_4), n_2);
    }
    double** C = new double*[4];
    for (int k = 0; k < 4; ++k) {
      C[k] = new double[size_4];
    }

    for (int i = 0; i < size_4; ++i) {
      C[0][i] = P[0][i] + P[3][i] - P[4][i] + P[6][i];
      C[1][i] = P[2][i] + P[4][i];
      C[2][i] = P[1][i] + P[3][i];
      C[3][i] = P[0][i] - P[1][i] + P[2][i] + P[5][i];
    }

    double* res = new double[size];

    for (int i = 0; i < n_2; ++i)
      for (int j = 0; j < n_2; ++j) {
        res[i*n + j] = C[0][i*n_2 + j];                      // C_11
        res[i*n + j + n / 2] = C[1][i*n_2 + j];              // C_12
        res[(i + n / 2) * n + j] = C[2][i*n_2 + j];          // C_21
        res[(i + n / 2) * n + j + n / 2] = C[3][i*n_2 + j];  // C_22
      }
    return res;
  } else {
    return matrix_mul(A, B, n);
  }
}

double* expand_shtrassen(double* A, double* B, int n) {
  if (n > 450) {
    int check_pow[8] = { 128, 256, 512, 1024, 2048, 4096, 8192, 16384 };
    int new_size = 0;
    for (int i = 0; i < n; ++i) {
      if (check_pow[i] == n) {
        return shtrassen_mul(A, B, n);
      }
      if (n < check_pow[i]) {
        new_size = check_pow[i];
        break;
      }
    }

    double* new_A = new double[new_size*new_size];
    double* new_B = new double[new_size*new_size];

    for (int i = 0; i < new_size; ++i) {
      for (int j = 0; j < new_size; ++j) {
        if (i < n && j < n) {
          new_A[i*new_size + j] = A[i*n + j];
          new_B[i*new_size + j] = B[i*n + j];
        } else {
          new_A[i*new_size + j] = 0.0;
          new_B[i*new_size + j] = 0.0;
        }
      }
    }
    double* res_s = nullptr;
    res_s = shtrassen_mul(new_A, new_B, new_size);

    double* aligned_mat = new double[n*n];
    for (int i = 0; i < n; ++i)
      for (int j = 0; j < n; ++j)
        aligned_mat[i*n + j] = res_s[i*new_size + j];

    return aligned_mat;
  } else {
    return matrix_mul(A, B, n);
  }
}

double* expand_shtrassen_omp(double* A, double* B, int n) {
  if (n > 64) {
    int check_pow[8] = { 128, 256, 512, 1024, 2048, 4096, 8192, 16384 };
    int new_size = 0;
    for (int i = 0; i < n; ++i) {
      if (check_pow[i] == n) {
        return shtrassen_mul_omp(A, B, n);
      }
      if (n < check_pow[i]) {
        new_size = check_pow[i];
        break;
      }
    }

    double* new_A = new double[new_size*new_size];
    double* new_B = new double[new_size*new_size];
#pragma omp parallel for
    for (int i = 0; i < new_size; ++i) {
      for (int j = 0; j < new_size; ++j) {
        if (i < n && j < n) {
          new_A[i*new_size + j] = A[i*n + j];
          new_B[i*new_size + j] = B[i*n + j];
        } else {
          new_A[i*new_size + j] = 0.0;
          new_B[i*new_size + j] = 0.0;
        }
      }
    }
    double* res_s = nullptr;
    res_s = shtrassen_mul_omp(new_A, new_B, new_size);

    double* aligned_mat = new double[n*n];
#pragma omp parallel for
    for (int i = 0; i < n; ++i)
      for (int j = 0; j < n; ++j)
        aligned_mat[i*n + j] = res_s[i*new_size + j];

    return aligned_mat;
  } else {
    return matrix_mul(A, B, n);
  }
}

void matrix_out(double* A, int n) {
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j)
      std::cout << A[i*n + j] << " ";
    std::cout << std::endl;
  }
}

bool check(double* A, double* B, int size, int* num, double eps) {
  for (int i = 0; i < size; ++i)
    if (fabs(A[i] - B[i]) > eps) {
      *num = i;
      return false;
    }
  return true;
}

template < typename T >
T random(T lower_bound, T upper_bound) {
  return T();
}

template <>
int random<int>(int lower_bound, int upper_bound) {
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_int_distribution<int> uni(lower_bound, upper_bound);
  return uni(rng);
}

template <>
double random<double>(double lower_bound, double upper_bound) {
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_real_distribution<double> uni(lower_bound, upper_bound);
  return uni(rng);
}

int main(int argc, char** argv) {
  int n = 0;
  if (argc > 1) {
    n = atoi(argv[1]);
  } else {
    n = random<int>(500, 1000);
  }
  std::cout << "n = " << n << std::endl;
  int size = n * n;
  std::cout << "size = " << n << "x" << n << " = " << size << std::endl << std::endl;
  double* A = new double[size], *B = new double[size];
  double eps = 1e-2;
  std::cout << "matrix generating..." << std::endl;
  for (int i = 0; i < size; ++i) {
    A[i] = random<double>(0, 10000);
    B[i] = random<double>(0, 10000);
  }
  std::cout << "matrix is generated" << std::endl << std::endl;

  double* res_mul = nullptr;
  double* res_shtrass_seq = nullptr, *res_shtrass_omp = nullptr;
  double mul_t = 0, shtrass_seq_t = 0, shtrass_omp_t = 0;


  std::cout << "usual multiplicating..." << std::endl;
  double tmp = omp_get_wtime();
  res_mul = matrix_mul(A, B, n);
  mul_t = omp_get_wtime() - tmp;
  std::cout << "done!" << std::endl << std::endl;

  std::cout << "shtrassen multiplicating..." << std::endl;
  tmp = omp_get_wtime();
  res_shtrass_seq = expand_shtrassen(A, B, n);
  shtrass_seq_t = omp_get_wtime() - tmp;
  std::cout << "done!" << std::endl << std::endl;

  std::cout << "shtrassen omp parallel multiplicating..." << std::endl;
  tmp = omp_get_wtime();
  res_shtrass_omp = expand_shtrassen_omp(A, B, n);
  shtrass_omp_t = omp_get_wtime() - tmp;
  std::cout << "done!" << std::endl << std::endl;


  if (n < 10) {
    matrix_out(res_mul, n);
    matrix_out(res_shtrass_seq, n);
  }

  int bad1 = -1, bad2 = -1;
  std::cout.precision(17);
  if (check(res_mul, res_shtrass_seq, size, &bad1, eps) && check(res_mul, res_shtrass_omp, size, &bad2, eps)) {
    std::cout << "good multiplication!" << std::endl << std::endl
      << "time of usual multiplication: " << mul_t << std::endl
      << "time of sequent shtrassen:" << shtrass_seq_t << std::endl
      << "time of parallel omp shtrassen:" << shtrass_omp_t << std::endl;
  } else {
    if (bad1 != -1) {
      std::cout << "bad multiplication seq! " << bad1 << " elem: "
        << res_mul[bad1] << "!=" << res_shtrass_seq[bad1] << std::endl << std::endl;
    } else {
      std::cout << "bad multiplication omp! " << bad1 << " elem: "
        << res_mul[bad2] << "!=" << res_shtrass_omp[bad2] << std::endl << std::endl;
    }
    std::cout << "time of usual multiplication: " << mul_t << std::endl
      << "time of sequent shtrassen:" << shtrass_seq_t << std::endl
      << "time of parallel omp shtrassen:" << shtrass_omp_t << std::endl;
  }
}
