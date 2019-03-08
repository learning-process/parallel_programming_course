// Copyright 2019 Usova Marina

#include <omp.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>

// FUNCTION OF SORTING HOARA

template<typename Tmass, typename Tint> void quicksort(Tmass* mass, Tint iStart, Tint iFinish) {
  if (iFinish > iStart) {
    Tint s = iStart, f = iFinish;
    // as the reference element take the middle of the array
    Tint middle = mass[(iFinish + iStart) / 2];

    // we carry out division into elements smaller than the reference
    // and larger ones of the reference, and then recursively run
    // the same function for the generated sets
    do {
      while (mass[s] < middle)
        s++;
      while (mass[f] > middle)
        f--;
      if (s <= f) {
        Tmass tmp = mass[s];
        mass[s] = mass[f];
        mass[f] = tmp;
        s++;
        f--;
      }
    } while (s < f);

    if (iStart < f)
      quicksort(mass, iStart, f);
    if (s < iFinish)
      quicksort(mass, s, iFinish);
  }
}

// SUPPORTING FUNCTIONS

void create_array(int* array, int* copy, int size) {
  srand((unsigned int)time(NULL));
  const int max_elem = 10000;
  for (int i = 0; i < size; ++i) {
    array[i] = std::rand() % max_elem;
    copy[i] = array[i];
  }
}

void print_array(int* array, int size) {
  for (int i = 0; i < size; ++i)
    std::cout << array[i] << " ";
  std::cout << std::endl;
}

bool check(int* A, int* B, int size) {
  for (int i = 0; i < size; ++i)
    if (std::abs(A[i] - B[i]) != 0) {
      return false;
    }
  return true;
}

int compare(const int *i, const int *j) {
  return *i - *j;
}

// MAIN FUNCTION

int main(int argc, char** argv) {
  int size;

  srand((unsigned int)time(NULL));

  if (argc > 1) {
    size = atoi(argv[1]);
  } else {
    size = 500 + std::rand() % 1000;
    // size = 20;
  }

  std::cout << "n = " << size << std::endl;

  int* data = new int[size];
  int* copy = new int[size];

  std::cout << "massive generating..." << std::endl;

  create_array(data, copy, size);

  std::cout << "massive is generated!" << std::endl << std::endl;

  // print_array(data, size);

  std::cout << "usual sort..." << std::endl;

  auto timeWork_ = omp_get_wtime();
  qsort(copy, size, sizeof(int), (int(*) (const void *, const void *)) compare);
  timeWork_ = omp_get_wtime() - timeWork_;

  // print_array(copy, size);

  std::cout << "sorting done!" << std::endl << std::endl;
  std::cout << "quick sort..." << std::endl;

  auto timeWork = omp_get_wtime();
  quicksort(data, 0, size - 1);
  timeWork = omp_get_wtime() - timeWork;

  // print_array(data, size);

  std::cout << "sorting done!" << std::endl << std::endl;

  std::cout.precision(17);
  if (check(copy, data, size))
    std::cout << "- good sorting: results of implemented sort and library sort coincide"
    << std::endl << "- time of library function of sorting: " << timeWork_
    << std::endl << "- time of my quick sorting: " << timeWork << std::endl;
  else
    std::cout << " error of sorting!!!" << std::endl;

  delete[]data;
  delete[]copy;
  return 0;
}
