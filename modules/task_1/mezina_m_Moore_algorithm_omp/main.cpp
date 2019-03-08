// Copyright 2019 Mezina Margarita

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>

int main(int argc, char** argv) {
    // Graph params
    unsigned int vertex_count;
    unsigned int edges_count;
    unsigned char** graph_matrix;
    bool writing_work = 0;
    unsigned int start_vertex;

    // Enter graph params
    /*std::cout << "Enter count of vertexes: ";
    std::cin >> vertex_count;
    std::cout << "Enter count of edges: ";
    std::cin >> edges_count;
    std::cout << "Enter start vertex: ";
    std::cin >> start_vertex;
    start_vertex--;
    std::cout << "Enter 1, if should print intermidiate information, else enter any other value: ";
    string s;
    std::cin >> s;
    if (s == "1") writing_work = 1;*/
    vertex_count = 5;
    edges_count = 10;
    start_vertex = 0;
    writing_work = 1;

    // Memory allocation matrix
    srand(static_cast<unsigned int>(time(0)));
    graph_matrix = new unsigned char*[vertex_count];
    for (unsigned int i = 0; i < vertex_count; ++i) {
        graph_matrix[i] = new unsigned char[vertex_count];
    }

    // Generate matrix
    if (edges_count <= vertex_count * (vertex_count - 1) / 2) {
        for (unsigned int i = 0; i < vertex_count; ++i) {
            for (unsigned int j = 0; j < vertex_count; ++j)
                graph_matrix[i][j] = 0;
        }
        for (unsigned int i = 0; i < edges_count; ++i) {
            unsigned int x, y;
            do {
                x = std::rand() % vertex_count;
                y = std::rand() % vertex_count;
            } while (x == y || graph_matrix[x][y] != 0);
            unsigned char value = std::rand() % 100 + 1;
            graph_matrix[x][y] = value;
        }
    }   else {
        for (unsigned int x = 0; x < vertex_count; ++x) {
            for (unsigned int y = 0; y < vertex_count; ++y)
                graph_matrix[x][y] = (x != y) ? std::rand() % 100 + 1 : 0;
        }
        for (unsigned int i = 0; i < vertex_count * (vertex_count - 1) - edges_count; ++i) {
            unsigned int x, y;
            do {
                x = std::rand() % vertex_count;
                y = std::rand() % vertex_count;
            } while (graph_matrix[x][y] == 0);
            graph_matrix[x][y] = 0;
        }
    }
    if (writing_work) {
        std::cout << std::endl << "GENERATE MATRIX:" << std::endl;
        for (unsigned int x = 0; x < vertex_count; ++x) {
            for (unsigned int y = 0; y < vertex_count; ++y)
                std::cout << "\t" << static_cast<unsigned int>(graph_matrix[x][y]);
            std::cout << std::endl;
        }
    }

    // Starting time
    std::cout << "START WORK!" << std::endl;
    unsigned int start_time, finish_time;
    start_time = clock();

    // Work params
    unsigned int** distance_matrix;
    unsigned int* prev_vertex;
    const unsigned int PATH_INFINITY = ~(unsigned int)0;
    bool check_compare;
    unsigned int iter;
    std::vector<unsigned int>* path;

    // Memory allocation params
    distance_matrix = new unsigned int*[vertex_count + 1];
    for (unsigned int i = 0; i <= vertex_count; ++i)
        distance_matrix[i] = new unsigned int[vertex_count];
    prev_vertex = new unsigned int[vertex_count];
    for (unsigned int i = 0; i < vertex_count; ++i) {
        distance_matrix[0][i] = PATH_INFINITY;
        prev_vertex[i] = PATH_INFINITY;
    }
    distance_matrix[0][start_vertex] = 0;
    prev_vertex[start_vertex] = start_vertex;
    path = new std::vector<unsigned int>[vertex_count];

    // Algorithm
    iter = 0;
    check_compare = 0;
    if (writing_work) {
        std::cout << "WORK ITERATION: "<< std::endl;
        std::cout << "\tIteration 0:\t";
        for (unsigned int i = 0; i < vertex_count; ++i) {
            if (distance_matrix[0][i] == PATH_INFINITY)
                std::cout << "inf\t";
            else
                std::cout << distance_matrix[0][i] << "\t";
        }
        std::cout << std::endl;
    }
    while (!check_compare && iter < vertex_count) {
        check_compare = 1;
        // Copy elements
        for (unsigned int vertex = 0; vertex < vertex_count; ++vertex)
            distance_matrix[iter + 1][vertex] = distance_matrix[iter][vertex];
        // Update distance for vertexes
        for (unsigned int from = 0; from < vertex_count; ++from) {
            if (distance_matrix[iter][from] == PATH_INFINITY)
                continue;
            if (iter != 0 && distance_matrix[iter][from] == distance_matrix[iter - 1][from])
                continue;
            for (unsigned int to = 0; to < vertex_count; ++to) {
                if (graph_matrix[from][to] == 0)
                    continue;
                if (distance_matrix[iter + 1][to] > distance_matrix[iter][from] + graph_matrix[from][to]) {
                    check_compare = 0;
                    distance_matrix[iter + 1][to] = distance_matrix[iter][from] + graph_matrix[from][to];
                    prev_vertex[to] = from;
                }
            }
        }
        iter++;
        if (writing_work) {
            std::cout << "\tIteration " << iter << ":\t";
            for (unsigned int i = 0; i < vertex_count; ++i) {
                if (distance_matrix[iter][i] == PATH_INFINITY)
                    std::cout << "inf\t";
                else
                    std::cout << distance_matrix[iter][i] << "\t";
            }
            std::cout << std::endl;
        }
    }

    // Find pathes
    for (unsigned int vertex = 0; vertex < vertex_count; ++vertex) {
        if (distance_matrix[iter][vertex] == PATH_INFINITY)
            continue;
        unsigned int cur_vertex = vertex;
        do {
            path[vertex].push_back(cur_vertex);
            cur_vertex = prev_vertex[cur_vertex];
        } while (cur_vertex != start_vertex);
        if (path[vertex].back() != start_vertex)
            path[vertex].push_back(start_vertex);
        reverse(path[vertex].begin(), path[vertex].end());
    }

    // Print results
    if (writing_work) {
        std::cout << "RESULTS: "<< std::endl;
        for (unsigned int vertex = 0; vertex < vertex_count; ++vertex) {
            std::cout << "\t" << vertex + 1 << ")\t";
            if (distance_matrix[iter][vertex] == PATH_INFINITY) {
                std::cout << "not path" << std::endl;
                continue;
            }
            std::cout << "Distance: " << distance_matrix[iter][vertex] << std::endl;
            std::cout << "\t\tPath: ";
            std::cout << path[vertex][0] + 1;
            for (unsigned int i = 1; i < path[vertex].size(); ++i)
                std::cout << "->" << path[vertex][i] + 1;
            std::cout << std::endl;
        }
    }

    // Finish time
    finish_time = clock();
    std::cout << "TIME: " << static_cast<double>(finish_time - start_time) / 1000 << std::endl;

    // system("pause");
    return 0;
}
