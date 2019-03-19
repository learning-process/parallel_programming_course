// Copyright Dubovskoy Andrey 2019

#include <ctime>
#include <iostream>
#include <cstdlib>
#include <random>


/* For qsort */
int comp(const int *i, const int *j) {
  return *i - *j;
}

int* get_randomized_array(int size) {
  int* array = new int[size];
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(0, 10000);
  for (int i = 0; i < size; i++) {
    array[i] = distribution(generator);
  }
  return array;
}

void print_array(int* array, int length) {
  std::cout << " Array: ";
  for (int i = 0; i < length; i++) {
    std::cout << " " << array[i];
  }
  std::cout << std::endl;
}

int get_max(int* a, int n) {
  int max = a[0];
  for (int i = 1; i < n; i++) {
    if (a[i] > max)
      max = a[i];
  }
  return max;
}

int* get_num(int* a, int size, int rang) {
  int *count = new int[rang];
  for (int g = 0; g < rang; g++) {
    count[g] = 0;
  }
  for (int g = 0; g < rang; g++) {
    int r = 1;
    for (int k = 0; k < g; k++) {
      r *= 10;
    }
    for (int j = 0; j < size; j++) {
      if (a[j] / r < 10 && a[j] / r > 0) {
        count[g]++;
      }
    }
  }
  return count;
}

void get_arr(int** arr, int* a, int size, int rang) {
  for (int g = 0; g < rang; g++) {
    int r = 1;
    for (int k = 0; k < g; k++) {
      r *= 10;
    }
    int N = 0;
    for (int j = 0; j < size; j++) {
      if (a[j] / r < 10 && a[j] / r > 0) {
        arr[g][N] = a[j];
        N++;
      }
    }
  }
}

void sorting(int** arr, int* count, int rang) {
  for (int i = 0; i < rang; i++) {
    qsort(arr[i], count[i], sizeof(int), (int(*) (const void *, const void *)) comp);
  }
}

int main() {
  int size = 12;
  int *a = new int[size];
  a = get_randomized_array(size);
  if (size < 20)
    print_array(a, size);
  int max = get_max(a, size);
  if (max == 0) {
    std::cout << "Array = {0}";
    return -1;
  }
  int num = 0;
  for (int i = 0; i < size; i++) {
    if (a[i] == 0) {
      num++;
    }
  }
  int rang = 0;
  while (max > 0) {
    max /= 10;
    rang++;
  }
  std::cout << "Max rang " << rang << std::endl;
  int* count = new int[rang];
  count = get_num(a, size, rang);
  std::cout << "Rangs ";
  print_array(count, rang);
  int** arr;
  arr = new int*[rang];
  for (int i = 0; i < rang; i++)
    arr[i] = new int[count[i]];
  get_arr(arr, a, size, rang);
  if (size < 20) {
    std::cout << "ARR:  ";
    for (int i = 0; i < rang; i++)
      for (int j = 0; j < count[i]; j++) {
        std::cout << arr[i][j] << " ";
      }
    std::cout << std::endl;
  }
  sorting(arr, count, rang);

  int b = 0;
  for (int i = 0; i < rang; i++) {
    for (int j = 0; j < count[i]; j++) {
      a[num + b + j] = arr[i][j];
    }
    b += count[i];
  }
  if (size < 20) {
    std::cout << "Sorted ";
    print_array(a, size);
  }
  for (int i = 1; i < size; i++) {
    if (a[i] < a[i - 1]) {
      std::cout << "Array not sorted";
      return -1;
    }
  }
  std::cout << "Array sorted" << std::endl;
}
