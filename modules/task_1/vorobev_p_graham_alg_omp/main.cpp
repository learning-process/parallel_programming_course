// Copyright 2019 Pavel Vorobev
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

struct point {
    int x;
    int y;
};

bool moreOrEqualThan(point a, point c, point b) {
    return a.x * b.y - a.y * b.x
        + b.x * c.y - b.y * c.x
        + c.x * a.y - c.y * a.x <= 0;
}

bool notEquals(point a, point b) {
    if (a.x != b.x)
        return true;
    else if (a.y != b.y)
        return true;
    else
        return false;
}

int determinant(point a, point b, point c) {
    return (b.x - a.x) * (c.y - a.y)
        - (c.x - a.x) * (b.y - a.y);
}

int partition(std::vector<point> *a, int low, int high) {
    point pivot = a->at(high);
    int i = (low - 1);
    for (int j = low; j < high; j++) {
        if (moreOrEqualThan(pivot, a->at(j), a->at(0))) {
            i++;
            point tmp = a->at(i);
            a->at(i) = a->at(j);
            a->at(j) = tmp;
        }
    }
    point tmp = a->at(i + 1);
    a->at(i + 1) = a->at(high);
    a->at(high) = tmp;

    return i + 1;
}

void quickSort(std::vector<point> *a, int low, int high) {
    if (low < high) {
        int partIndex = partition(a, low, high);
        quickSort(a, low, partIndex - 1);
        quickSort(a, partIndex + 1, high);
    }
}

int grahamScan(std::vector<point> *a) {
    point c = a->at(0);
    int m = 0;
    for (size_t i = 1; i < a->size(); i++) {
        if (a->at(i).x < c.x) {
            c = a->at(i);
            m = i;
        } else if (a->at(i).x == c.x) {
            if (a->at(i).y < c.y) {
                c = a->at(i);
                m = i;
           }
       }
    }

    point tmp = a->at(0);
    a->at(0) = a->at(m);
    a->at(m) = tmp;
    m = 1;
    quickSort(a, 0, a->size() - 1);

    for (size_t i = 1; i < a->size(); i++) {
         if (notEquals(a->at(i), a->at(m))) {
            if (m >= 1) {
                while (m >= 1 && determinant(a->at(m - 1), a->at(m),
                    a->at(i)) >= 0) {
                    m = m - 1;
                }
            }
            m = m + 1;
            point tmp = a->at(m);
            a->at(m) = a->at(i);
            a->at(i) = tmp;
        }
    }
    return m + 1;
}

char* getCmdOption(char **begin, char **end, const std::string& option) {
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
        return *itr;
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}

int myRand() {
    static int next = 3251;
    next = ((next * next) / 100) % 10000;
    return next;
}

int myRandInRange(int min, int max) {
    return myRand() % (max + 1 - min) + min;
}

int main(int argc, char *argv[]) {
    double numOfPoints = 30;
    double min = 1;
    double max = 40;
    if (cmdOptionExists(argv, argv + argc, "-a1")) {
        char *wcount = getCmdOption(argv, argv + argc, "-a1");
        numOfPoints = atof(wcount);
    }

    if (cmdOptionExists(argv, argv + argc, "-b1")) {
        char *wcount = getCmdOption(argv, argv + argc, "-b1");
        min = atof(wcount);
    }

    if (cmdOptionExists(argv, argv + argc, "-a2")) {
        char *wcount = getCmdOption(argv, argv + argc, "-a2");
        max = atof(wcount);
    }

    std::vector<point>* data = new std::vector<point>;

    for (int i = 0; i < numOfPoints; i++) {
        point p;
        p.x = myRandInRange(static_cast<int> (min), static_cast<int> (max));
        p.y = myRandInRange(static_cast<int> (min), static_cast<int> (max));
        data->push_back(p);
    }

    int hullSize = grahamScan(data);
    std::cout << "Hull!" << std::endl;
    for (int i = 0; i < hullSize; i++) {
        std::cout << "(" << data->at(i).x << " ,"
           << data->at(i).y << " )" << std::endl;
    }
}

