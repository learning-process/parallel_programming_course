// Copyright 2019 Usova Marina

#include <omp.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <cmath>

// FUNCTION OF SORTING HOARA

void quicksort(int* mass, int iStart, int iFinish) {
  if (iFinish > iStart) {
    int s = iStart, f = iFinish;

    // as the reference element take the middle of the array
    int middle = mass[(iFinish + iStart) / 2];

    // we carry out division into elements smaller than the reference
    // and larger ones of the reference, and then recursively run
    // the same function for the generated sets
    do {
      while (mass[s] < middle)
        s++;
      while (mass[f] > middle)
        f--;
      if (s <= f) {
        int tmp = mass[s];
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

// FUNCTION OF ODD-EVEN MERGE

// function that performs splitting into even and odd elements
void split(const int *mass_1, int size_1, const int *mass_2, int size_2, bool isEven, std::vector<int> *Result) {
  Result->reserve(size_1 + size_2);

  int i, j;
  if (isEven) {
    i = 0, j = 0;
  } else {
    i = 1, j = 1;
  }
  for (; (i < size_1) && (j < size_2);) {
    if (mass_1[i] <= mass_2[j]) {
      Result->push_back(mass_1[i]);
      i += 2;
    } else {
      Result->push_back(mass_2[j]);
      j += 2;
    }
  }
  for (; i < size_1; i += 2) {
      Result->push_back(mass_1[i]);
  }
  for (; j < size_2; j += 2) {
    Result->push_back(mass_2[j]);
  }
}

void unsplit(const std::vector<int> vector1, const std::vector<int> vector2, int* Result) {
  int size_1 = vector1.size(), size_2 = vector2.size();

  int i = 0, j = 0;
  for (; (i < size_1) && (j < size_2); i++, j++) {
    Result[i + j] = vector1[i];
    Result[i + j + 1] = vector2[j];
  }
  for (; i < size_1; i++) {
    Result[size_1 + i] = vector1[i];
  }
  for (; j < size_2; j++) {
    Result[size_2 + j] = vector2[j];
  }
}

void compare_exchange(int* x1, int* x2) {
  if (*x1 > *x2) {
    int tmp = *x1;
    *x1 = *x2;
    *x2 = tmp;
  }
}

// function merges ordered arrays
void oddEvenMerge(const std::vector<int> vector1, const std::vector<int> vector2, int* Result) {
  int size_1 = vector1.size(), size_2 = vector2.size();

  if (size_1 + size_2 > 2) {
    unsplit(vector1, vector2, Result);
    for (int i = 1; i < size_1 + size_2 - 1; i++) {
      compare_exchange(&Result[i], &Result[i + 1]);
    }
  } else {
    if (size_1 + size_2 == 2)
      compare_exchange(&Result[0], &Result[1]);
  }
}

void parallelQuicksort(int* mass, int threads, int size) {
  std::vector<int> *Temp = new std::vector<int>[threads];

  int step;
  int *shift_array = new int[threads];
  int *array_of_sizes = new int[threads];

#pragma omp parallel shared(mass, step, shift_array, array_of_sizes, Temp) num_threads(threads)
  {
    int threadID;                         // current thread id
    int thread_index;                     // necessary shift to get companion thread

    threadID = omp_get_thread_num();

    // distribute parts of the source array by thread and sort

    shift_array[threadID] = threadID * (size / threads);

    if (threadID == threads - 1) {
      array_of_sizes[threadID] = (size / threads) + (size % threads);
    } else {
      array_of_sizes[threadID] = size / threads;
    }

    quicksort(mass + shift_array[threadID], 0, array_of_sizes[threadID] - 1);
#pragma omp barrier  // wait until all threads sort their part of the source array

    step = 1;

    while (step < threads) {
      // at each step, we select even and odd elements from paired streams
      thread_index = static_cast<int>(pow(2, step - 1));

      if (threadID % (thread_index * 2) == 0) {
        split(mass + shift_array[threadID], array_of_sizes[threadID], mass + shift_array[threadID +
          thread_index], array_of_sizes[threadID + thread_index], 1, &Temp[threadID]);
      } else if (threadID % thread_index == 0) {
        split(mass + shift_array[threadID], array_of_sizes[threadID], mass + shift_array[threadID -
          thread_index], array_of_sizes[threadID - thread_index], 0, &Temp[threadID]);
      }

#pragma omp barrier  // we expect execution of this part by all threads

      // Batcher merge on paired threads
      if (threadID % (thread_index * 2) == 0) {
        oddEvenMerge(Temp[threadID], Temp[threadID + thread_index], mass + shift_array[threadID]);
        array_of_sizes[threadID] += array_of_sizes[threadID + thread_index];
        Temp[threadID].clear();
        Temp[threadID].shrink_to_fit();
        Temp[threadID + thread_index].clear();
        Temp[threadID + thread_index].shrink_to_fit();
      }
#pragma omp single
      {
        step *= 2;  // proceed to the next step by the first free thread
      }
#pragma omp barrier  // waiting while all threads execute merging and sorting
    }
  }
  delete[] Temp;
  delete[] array_of_sizes;
  delete[] shift_array;
}

// SUPPORTING FUNCTIONS

void create_array(int* array, int* copy, int size) {
  srand((unsigned int)time(NULL));
  const int max_elem = 1000;
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
  int size, threads = 2;

  srand((unsigned int)time(NULL));

  if (argc > 1) {
    size = atoi(argv[1]);
  } else {
    // size = 500 + std::rand() % 1000;
    // size = 12;
    size = 10000000;
  }

  std::cout << "n = " << size << std::endl;

  int* data = new int[size];
  int* copy = new int[size];

  // int data[12] = {3, 7, 10, 1, 10, 17, 4, 8, 5, 9, 11, 10};
  // int copy[12] = { 3, 7, 10, 1, 10, 17, 4, 8, 5, 9, 11, 10 };

  std::cout << "massive generating..." << std::endl;

  create_array(data, copy, size);

  std::cout << "massive is generated!" << std::endl << std::endl;

  // print_array(data, size);

  std::cout << "sequential sort..." << std::endl;

  auto timeWork_ = omp_get_wtime();
  quicksort(copy, 0, size - 1);
  timeWork_ = omp_get_wtime() - timeWork_;

  // print_array(copy, size);

  std::cout << "sorting done!" << std::endl << std::endl;
  std::cout << "parallel sort..." << std::endl;

  auto timeWork = omp_get_wtime();
  parallelQuicksort(data, threads, size);
  timeWork = omp_get_wtime() - timeWork;

  // print_array(data, size);

  std::cout << "sorting done!" << std::endl << std::endl;

  std::cout.precision(17);
  if (check(copy, data, size))
    std::cout << "- good sorting: results of implemented sort and library sort coincide"
    << std::endl << "- time of sequential sorting: " << timeWork_
    << std::endl << "- time of parallel sorting: " << timeWork
    << std::endl << "- acceleration of parallel sorting: " << timeWork_ / timeWork << std::endl;
  else
    std::cout << " error of sorting!!!" << std::endl;

  delete[]data;
  delete[]copy;

  return 0;
}
