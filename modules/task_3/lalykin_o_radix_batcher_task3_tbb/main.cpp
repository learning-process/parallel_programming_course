// Copyright 2019 Lalykin Oleg

#include <tbb/tbb.h>
#include <iostream>
#include <string>
#include <cstdint>
#include <limits>

union Int32 {
    int x;
    unsigned char bytes[4];
};

class parRadix {
 public:
     union Int32* arr;
     int* step;
     parRadix(union Int32* tarr, int* tstep) :arr(tarr), step(tstep) {}

     void radix_sort_byte(union Int32* arr, int left, int right, int numByte, const int base) const {
         union Int32 *tmp = new union Int32[right - left];
         int* count = new int[base];
         for (int i = 0; i < base + 2; i++) {
             count[i] = 0;
         }
         for (int i = left; i < right; i++) {
             count[arr[i].bytes[numByte] + 1]++;
         }
         for (int i = 1; i < base; i++) {
             count[i] += count[i - 1];
         }
         for (int i = left; i < right; i++) {
             tmp[count[arr[i].bytes[numByte]]++] = arr[i];
         }
         for (int i = left; i < right; i++) {
             arr[i] = tmp[i - left];
         }
     }

     void radix_sort(union Int32* arr, int left, int right) const {
         int i;
         unsigned *subArr = reinterpret_cast<unsigned*>(arr);
         for (i = left; i < right; i++) {
             subArr[i] = subArr[i] ^ (-2147483647 - 1);
         }
         for (i = 0; i < 4; i++) {
             radix_sort_byte(arr, left, right, i, 256);
         }
         for (i = left; i < right; i++) {
             subArr[i] = subArr[i] ^ (-2147483647 - 1);
         }
     }

     void operator() (const tbb::blocked_range<int>& range) const {
         for (int i = range.begin(); i != range.end(); i++) {
             radix_sort(arr, step[i], step[i + 1]);
         }
     }
};

class parSortSubsequence {
 public:
    union Int32* arr;
    int* step;
    union Int32* tmp;

    parSortSubsequence(union Int32* tarr, int* tstep, union Int32* ttmp) :arr(tarr),
        step(tstep), tmp(ttmp) {}

    void  SortSubsequence(int sign, union Int32* arr, union Int32* tmp, int left, int right) const {
        for (int i = sign; i < left; i += 2) {
            tmp[i] = arr[i];
        }
        union Int32* rightArr = arr + left;

        int a = sign;
        int b = sign;
        int i = sign;

        while ((a < left) && (b < right)) {
            if (tmp[a].x <= rightArr[b].x) {
                arr[i] = tmp[a];
                a += 2;
            } else {
                arr[i] = rightArr[b];
                b += 2;
            }
            i += 2;
        }
        if (a == left) {
            for (int j = b; j < right; j += 2, i += 2) {
                arr[i] = rightArr[j];
            }
        } else {
            for (int j = a; j < left; j += 2, i += 2) {
                arr[i] = tmp[j];
            }
        }
    }

    void operator() (const tbb::blocked_range<int>& range) const {
        for (int i = range.begin(); i != range.end(); i++) {
            int t = i % 2;
            SortSubsequence(t, arr + step[i - t], tmp + step[i],
                step[i + (1 - t)] - step[i - t], step[i + 1 + (1 - t)] - step[i + (1 - t)]);
        }
    }
};

class parPairCom {
 public:
    union Int32* arr;
    int size;
    int* step;
    parPairCom(union Int32* tarr, int tsize, int* tstep) :arr(tarr), size(tsize), step(tstep) {}

    void PairComparison(union Int32* arr, int size) const {
        for (int i = 1; i < (size + 1) / 2; i++) {
            if (arr[2 * i].x < arr[2 * i - 1].x) {
                int tmp = arr[2 * i - 1].x;
                arr[2 * i - 1] = arr[2 * i];
                arr[2 * i].x = tmp;
            }
        }
    }

    void operator() (const tbb::blocked_range<int>& range) const {
        for (int i = range.begin(); i != range.end(); i++) {
        if (i % 2 != 0) {
            PairComparison(arr + step[i - 1], step[i + 1] - step[i - 1]);
        }
        }
    }
};
class Task {
 public:
    union Int32 *list;
    union Int32 *listS;
    int *Step;
    int Size;
    int threads;

    void initialization(char** argv) {
        int i;
        Size = atoi(argv[1]);

        list = new union Int32[Size];
        listS = new union Int32[Size];

        for (i = 0; i < Size; i++) {
            listS[i].x = list[i].x = std::rand() % 50 - 10;
        }

        threads = atoi(argv[2]);
        Step = new int[threads + 1];

        for (i = 0; i < threads; i++) {
            Step[i] = i * Size / threads + (i * Size / threads) % 2;
        }
        Step[threads] = Size;
    }

    void Destructor() {
        delete[] Step;
        delete[] list;
        delete[] listS;
    }

    void merge(union Int32* list, int size, int * step, int threads) {
        union Int32* tmp = new union Int32[size + 1];
        int level = threads;
        int *restep = new int[threads + 1];
        for (int i = 0; i < threads; i++) {
            step[i] = i * size / threads + (i * size / threads) % 2;
        }
        step[threads] = size;
        while ((level != 1)) {
            tbb::task_scheduler_init init(threads);
            tbb::parallel_for(tbb::blocked_range<int>(0, level), parSortSubsequence(list, step, tmp));
            init.terminate();
            init.initialize(threads);
            tbb::parallel_for(tbb::blocked_range<int>(0, level), parPairCom(list, size, step));
            init.terminate();

            level = level / 2;
            restep = new int[level];
            restep[0] = step[0];
            for (int i = 1; i < level; i++) {
                restep[i] = step[i * 2];
            }
            step = new int[level];
            for (int i = 0; i < level; i++) {
                step[i] = restep[i];
            }
            step[level] = size;
        }
    }

    void Check(union Int32* arr1, union Int32* arr2, int size) {
        bool f = true;
        for (int i = 0; i < size - 1; i++) {
            if ((arr1[i].x > arr1[i + 1].x)|| (arr2[i].x > arr2[i + 1].x)) {
                f = false;
            }
        }
        if (f) {
            std::cout << "\n\n CORRECT \n";
        } else {
            std::cout << "\n\n FALSE \n";
        }
    }

    void PrintList(std::string str, union Int32* arr, int size) {
        if (size < 33) {
            std::cout << "\n\n" << str << "\n";
            for (int i = 0; i < size; i++) {
                std::cout << arr[i].x << "|";
            }
        }
    }

    void PrintSteps(int * step, int threads) {
        std::cout << "\n\n" << " Steps:" << "\n";
        for (int j = 0; j < threads + 1; j++) {
            std::cout << "=>" << step[j];
        }
    }

    void PrintTimeRes(double ts, double tp) {
        std::cout << "\n\n Time Serial Radix: " << ts;
        std::cout << "\n\n Time Parallel Radix with Batcher: " << tp;
        std::cout << "\n\n Jump: " << ts / tp;
    }

    void RunTask(char **argv) {
        initialization(argv);
        PrintList(" Input: ", list, Size);
        PrintSteps(Step, threads);

        parRadix radixS(listS, Step);

        tbb::tick_count startS = tbb::tick_count::now();

        radixS.radix_sort(listS, 0, Size);

        tbb::tick_count finishS = tbb::tick_count::now();
        double TimeS = (finishS - startS).seconds();

        PrintList(" After serial radix: ", listS, Size);


        parRadix radix(list, Step);

        tbb::tick_count startP = tbb::tick_count::now();

        tbb::task_scheduler_init init(threads);
        tbb::parallel_for(tbb::blocked_range<int>(0, threads), radix);
        init.terminate();


        PrintList(" After parallel radix: ", list, Size);

        if (threads > 1) {
            merge(list, Size, Step, threads);
        }
        tbb::tick_count finishP = tbb::tick_count::now();
        double TimeP = (finishP - startP).seconds();

        PrintList(" After merge: ", list, Size);

        PrintTimeRes(TimeS, TimeP);
        Check(listS, list, Size);
        Destructor();
    }

    void RunDefault() {
        int i;
        Size = 40;

        list = new union Int32[Size];
        listS = new union Int32[Size];

        for (i = 0; i < Size; i++) {
            listS[i].x = list[i].x = std::rand() % 50 - 10;
        }

        threads = 4;
        Step = new int[threads + 1];

        for (i = 0; i < threads; i++) {
            Step[i] = i * Size / threads + (i * Size / threads) % 2;
        }
        Step[threads] = Size;

        PrintList(" Input: ", list, Size);
        PrintSteps(Step, threads);

        parRadix radixS(listS, Step);

        tbb::tick_count startS = tbb::tick_count::now();

        radixS.radix_sort(listS, 0, Size);

        tbb::tick_count finishS = tbb::tick_count::now();
        double TimeS = (finishS - startS).seconds();

        PrintList(" After serial radix: ", listS, Size);

        parRadix radix(list, Step);

        tbb::tick_count startP = tbb::tick_count::now();

        tbb::task_scheduler_init init(threads);
        tbb::parallel_for(tbb::blocked_range<int>(0, threads), radix);
        init.terminate();

        PrintList(" After parallel radix: ", list, Size);

        if (threads > 1) {
            merge(list, Size, Step, threads);
        }
        tbb::tick_count finishP = tbb::tick_count::now();
        double TimeP = (finishP - startP).seconds();

        PrintList(" After merge: ", list, Size);

        PrintTimeRes(TimeS, TimeP);
        Check(listS, list, Size);
        Destructor();
    }
};
int main(int argc, char **argv) {
    Task t;
    if (argc == 3) {
        t.RunTask(argv);
    } else {
        t.RunDefault();
    }
    return 0;
}
