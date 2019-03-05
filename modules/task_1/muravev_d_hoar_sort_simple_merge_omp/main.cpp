// Copyright 2019 Muravev Denis
// Hoar sorting with simple merge
#include <ctime>
#include <iostream>
#include <climits>
#include <cstring>
#define TYPE_MAS double
#define TYPE_SIZE unsigned int

TYPE_MAS* Create_mas(TYPE_SIZE _size) {
    TYPE_MAS* _mas = new TYPE_MAS[_size];
    TYPE_SIZE left = 0, right = INT_MAX;
    TYPE_SIZE i;

    if (_size < 1)
        return NULL;

    for (i = 0; i < _size; i++)
        _mas[i] = std::rand() % (left - right + 1) + left;
    return  _mas;
}

void Show_mas(TYPE_MAS* _mas, TYPE_SIZE _size) {
    TYPE_SIZE i;
    int delta = 7;

    if (_mas == NULL || _size < 1)
        return;

    std::cout << "Array [" << _size << "]" << std::endl;
    if (_size < 50) {
        for (i = 0; i < _size; i++) {
            std::cout.width(delta);
            std::cout << _mas[i];
        }
    }
}

int Check_not_decreasing(TYPE_MAS* _mas, TYPE_SIZE _size) {
    TYPE_SIZE i;
    for (i = 1; i < _size; i++)
        if (_mas[i - 1] > _mas[i])
            return 0;
    return 1;
}

void Quick_sort(TYPE_MAS * mas, TYPE_SIZE l, TYPE_SIZE r) {
    TYPE_SIZE i = l, j = r;
    TYPE_MAS e, t;
    e = mas[(r + l) / 2];

    while (i <= j) {
        while (mas[i] < e)
            i++;
        while (mas[j] > e)
            j--;

        if (i <= j) {
            if (i < j) {
                t = mas[i];
                mas[i] = mas[j];
                mas[j] = t;
            }
            i++;
            j--;
        }
    }

    if (j > l)
        Quick_sort(mas, l, j);
    if (r > i)
        Quick_sort(mas, i, r);
}

int main(int argc, char * argv[]) {
    TYPE_MAS* mas = NULL;
    TYPE_SIZE size = 100000;
    double time_sort = 0;
    if (argc == 3)
        if (strcmp(argv[1], "-size") == 0)
            size = atoi(argv[2]);

    std::srand((unsigned)time(NULL));
    mas = Create_mas(size);
    if (mas == NULL) {
        std::cout << "Error! Incorrect input data for array";
        return -1;
    }

    time_sort = clock();
    Quick_sort(mas, 0, size - 1);
    time_sort = (clock() - time_sort) / static_cast<double>(CLOCKS_PER_SEC);
    std::cout << "Spend time algorithm (Sequence version Hoar sorting): " << time_sort << "sec" << std::endl;

    if (Check_not_decreasing(mas, size))
        std::cout << "Array is sorted by not decreasing" << std::endl;
    else
        std::cout << "Array isn't sorted by not decreasing" << std::endl;

    Show_mas(mas, size);
    delete[] mas;
    return 0;
}
