// Copyright 2019 Sitkin Dmitry
#include <stdlib.h>
#include <stdio.h>
#include <utility>
#include <random>
#include <ctime>
#include <queue>
#include <iostream>
#include <string>

#define INFINITI 100
#define WEIGHT 5
#define NUM_OF_VERTEX 5
// using namespace std;

/*void print_d(int* d, int size) {
    for (int i = 0; i < size; i++) {
        cout << d[i] << " ";
    }
    cout << endl;
}
void print_graph(int** G, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cout << G[i][j] << " ";
        }
        cout << endl;
    }
}*/

int* init_graph(int countEdge, int countVertex) {
    int* one_graph = new int[countVertex * countVertex];
    int** graph = new int*[countVertex];

    for (int i = 0; i < countVertex; i++) {
        graph[i] = new int[countVertex];
    }
    srand((unsigned int)std::time(NULL));

    for (int i = 0; i < countVertex; i++) {
        for (int j = 0; j < countVertex; j++) {
            if (i == j) {
                graph[i][j] = 0;
            } else {
                graph[i][j] = std::rand() % 3;
                if (graph[i][j] == 0) {
                    graph[i][j] = INFINITI;
                }
            }
        }
    }
    for (int i = 0, t = 0; i < countVertex; i++) {
        for (int j = 0; j < countVertex; j++) {
            one_graph[t] = graph[j][i];
            t++;
        }
    }
    return one_graph;
}

int* init_d(int size) {
    int* d = new int[size];
    for (int i = 0; i < size; i++) {
        d[i] = INFINITI;
    }
    return d;
}

int* dijkstra(int* graph, int start, int count_vertex) {
    int* d = init_d(count_vertex);
    d[start] = 0;
    std::priority_queue<std::pair<int, int>>  queue;
    queue.push(std::make_pair(0, start));
    while (!queue.empty()) {
        int v = queue.top().second, cur_d = queue.top().first;
        queue.pop();

        if (cur_d > d[v])  continue;

        for (int i = 0; i < count_vertex; ++i) {
            int to = i, len = graph[i * count_vertex + v];
            if (d[v] + len < d[to]) {
                d[to] = d[v] + len;
                queue.push(std::make_pair(d[to], to));
            }
        }
    }
    return d;
}
int main(int argc, char *argv[]) {
    srand((unsigned int)std::time(NULL));
    int vert = NUM_OF_VERTEX;
    int count_edge = (vert - 1) + std::rand() % ((vert * (vert - 1)) / 2);
    int start = std::rand() % (NUM_OF_VERTEX - 1);
    int *graph = init_graph(count_edge, NUM_OF_VERTEX);
    int *result = dijkstra(graph, start, NUM_OF_VERTEX);
    for (int i = 0; i < vert; i++) {
        std::cout << result[i] << " ";
    }
  //  print_d(result, NUM_OF_VERTEX);
    return 0;
}
