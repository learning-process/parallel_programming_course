//  Copyright 2019 Churakov Sergey
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cmath>
#define step 1000

void RandomizeArray(double* rarray, const int& size, const int& min = -50, const int& max = 50) {
    for (int i = 0; i < size; i++) {
        *(rarray + i) = std::rand() % (max - min + 1) + min;
    }
}


void PrintPoints(double* X, double* Y, const int& Size) {
    for (int i = 0; i < Size; i++) {
        std::cout << std::setw(3) << X[i] << ", " << Y[i] << "; ";
    }
}


void ReinitEqPoints(double* X, double* Y, const int& Size, const int& min = -50, const int& max = 50) {
    for (int i = 0; i < Size - 1; i++)
        for (int j = i + 1; j < Size; j++) {
            if (X[i] == X[j] && Y[i] == Y[j]) {
                int flag = 1;
                while (flag) {
                    flag = 0;
                    X[i] = std::rand() % (max - min + 1) + min;
                    Y[i] = std::rand() % (max - min + 1) + min;
                    for (int k = 0; k < Size; k++) {
                        if (X[i] == X[k] && Y[i] == Y[k] && i != k) {
                            flag = 1;
                            break;
                        }
                    }
                }
                break;
            }
        }
}

int FindBLPoint(double* X, double* Y, const int& Size) {
    if (Size < 1) { return -1; }
    int pointInd = 0;
    for (int i = 1; i < Size; i++) {
        if (Y[i] < Y[pointInd]) {
            pointInd = i;
        } else {
            if (Y[i] == Y[pointInd] && X[i] < X[pointInd])
                pointInd = i;
        }
    }
    return pointInd;
}

int FindTRPoint(double* X, double* Y, const int& Size) {
    if (Size < 1) { return -1; }
    int pointInd = 0;
    for (int i = 1; i < Size; i++) {
        if (Y[i] > Y[pointInd]) {
            pointInd = i;
        } else {
            if (Y[i] == Y[pointInd] && X[i] > X[pointInd])
                pointInd = i;
        }
    }
    return pointInd;
}

double FindDist(const double& x1, const double& y1, const double& x2, const double& y2) {
    return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

double GetCos(const double& x1, const double& y1, const double& x2,
 const double& y2, const double& x3, const double& y3) {
    return ((x2 - x1)*(x3 - x1) + (y2 - y1)*(y3 - y1)) / (FindDist(x1, y1, x2, y2)*FindDist(x1, y1, x3, y3));
}


int FindPWithMinAngle(double* X, double* Y, const int& Size,
 const double& x1, const double& y1, const double& x2, const double& y2) {
    double maxCos = -1.5;
    int NextPointInd = -1;
    double tmp;
    for (int i = 0; i < Size; i++) {
        if (!((X[i] == x1 && Y[i] == y1) || (X[i] == x2 && Y[i] == y2))) {
            if (((x2 - x1)*(Y[i] - y2) - (X[i] - x2)*(y2 - y1)) >= 0) {
                tmp = (-1)*GetCos(x2, y2, x1, y1, X[i], Y[i]);
                if (tmp > maxCos) {
                    maxCos = tmp;
                    NextPointInd = i;
                }
            }
        }
    }
    return NextPointInd;
}

void ElimPointsOnLines(double* X, double* Y, int* Envelope, int* Size) {
    int i = 1;
    while (i != *Size) {
        if (static_cast<double>(static_cast<double>(X[Envelope[i]] - X[Envelope[i - 1]])*
            static_cast<double>(Y[Envelope[i + 1]] - Y[Envelope[i]])) ==
            static_cast<double>(static_cast<double>(X[Envelope[i + 1]] - X[Envelope[i]])*
                static_cast<double>(Y[Envelope[i]] - Y[Envelope[i - 1]]))) {
            int j = 0;
            while (i + j < *Size) {
                Envelope[i + j] = Envelope[i + j + 1];
                j++;
            }
            *Size = *Size - 1;
            i--;
        }
        i++;
    }
}

int main(int argc, char* argv[]) {
    time_t times, time_part, timef;
    clock_t clocks, clock_part, clockf;
    times = time(NULL);
    clocks = clock();
    srand((unsigned int)time(NULL));
    int Size = 100;
    if (argc != 1 && argc != 2 && argc != 4)
        return 1;
    if (argc > 1) {
        Size = atoi(argv[1]);
        if (Size < 1)
            return 1;
    }
    double* X_coord = new double[Size];
    double* Y_coord = new double[Size];
    if (argc == 4) {
        int minRand, maxRand;
        minRand = atoi(argv[2]);
        maxRand = atoi(argv[3]);
        if (maxRand < minRand)
            return 1;
        RandomizeArray(X_coord, Size, minRand, maxRand);
        RandomizeArray(Y_coord, Size, minRand, maxRand);
        //        if (Size >(maxRand - minRand + 1)*(maxRand - minRand + 1))
        //            return 1;
        //        ReinitEqPoints(X_coord, Y_coord, Size, minRand, maxRand);
    } else {
        RandomizeArray(X_coord, Size);
        RandomizeArray(Y_coord, Size);
        //        if (Size > 101 * 101)
        //            return 1;
        //        ReinitEqPoints(X_coord, Y_coord, Size);
    }
    PrintPoints(X_coord, Y_coord, Size);
    std::cout << std::endl;
    time_part = time(NULL);
    clock_part = clock();
    if (Size == 1) {
        std::cout << "Result chain of points is a single point:" << std::endl;
        std::cout << std::setw(3) << X_coord[0] << ", " << Y_coord[0] << "; ";
    } else {
        if (Size == 2) {
            std::cout << "Result chain of points is" << std::endl;
            std::cout << std::setw(3) << X_coord[0] << ", " << Y_coord[0] << "; ";
            std::cout << std::setw(3) << X_coord[1] << ", " << Y_coord[1] << "; ";
        } else {
            int dynsize = step;
            int* Envelope = static_cast<int*>(malloc(sizeof(int) * dynsize));
            int PNum = 1;
            int FirstPoint = FindBLPoint(X_coord, Y_coord, Size);
            Envelope[0] = FirstPoint;
            Envelope[1] = FindPWithMinAngle(X_coord, Y_coord, Size, X_coord[FirstPoint] - 1,
                Y_coord[FirstPoint], X_coord[FirstPoint], Y_coord[FirstPoint]);
            while (Envelope[PNum] != FirstPoint) {
                PNum++;
                if (PNum == dynsize) {
                    dynsize += step;
                    Envelope = static_cast<int*>(realloc(Envelope, sizeof(int) * (dynsize)));
                }
                Envelope[PNum] = FindPWithMinAngle(X_coord, Y_coord, Size, X_coord[Envelope[PNum - 2]],
                    Y_coord[Envelope[PNum - 2]], X_coord[Envelope[PNum - 1]], Y_coord[Envelope[PNum - 1]]);
            }
            ElimPointsOnLines(X_coord, Y_coord, Envelope, &PNum);
            if (PNum < 3) {
                std::cout << "Result chain of points is a line" << std::endl;
                int point;
                point = FindBLPoint(X_coord, Y_coord, Size);
                std::cout << std::setw(3) << X_coord[point] << ", " << Y_coord[point] << "; ";
                point = FindTRPoint(X_coord, Y_coord, Size);
                std::cout << std::setw(3) << X_coord[point] << ", " << Y_coord[point] << "; ";
            } else {
                std::cout << "Result chain of points is" << std::endl;
                for (int i = 0; i < PNum; i++) {
                    std::cout << std::setw(3) << X_coord[Envelope[i]] << ", " << Y_coord[Envelope[i]] << "; ";
                }
            }
            delete[] Envelope;
        }
    }
    timef = time(NULL);
    clockf = clock();
    //    times =
    std::cout << std::endl;
    std::cout << "time: " << difftime(timef, times) << std::endl;
    std::cout << "time without initialisation and preparations: " << difftime(time_part, timef) << std::endl;
    std::cout << "clocks: " << clockf - clocks << std::endl;
    std::cout << "clocks without initialisation and preparations: " << clockf - clock_part << std::endl;
    delete[] X_coord;
    delete[] Y_coord;
    return 0;
}
