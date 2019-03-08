// Copyright Bogatova Margarita
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <set>

using DstType = int;
using GraphType = std::vector< std::vector <DstType> >;

void PrintGraph(const GraphType& g);
GraphType RandomGenerateGraph(std::size_t n);
DstType FindDistance(int start, int finish, std::size_t n, const GraphType& d);

int main(int argc, char** argv) {
    GraphType g;
    DstType dst;
    std::size_t n;
    clock_t s_time, f_time;
    int start, finish;

//   int response;
//   if (argc > 1) {
//       n = std::atoi(argv[1]);
//   } else {
//       std::cout << "Enter the number of vertices: ";
//       std::cin >> n;
//   }
//   std::cout << "To generate an adjacency matrix randomly\t"
//       << "press 1" << std::endl;
//   std::cout << "To enter the adjacency matrix from the keyboard\t"
//       << "press 2" << std::endl;
//   std::cout << "Your input: ";
//   std::cin >> response;
//   if (response == 2) {
//       g.resize(n);
//       for (int i = 0; i < static_cast<int>(n); ++i) {
//           for (int j = 0; j < static_cast<int>(n); ++j) {
//               DstType d;
//               std::cin >> d;
//               g[i].push_back(d);
//           }
//       }
//   } else {
//       g = RandomGenerateGraph(n);
//       PrintGraph(g);
//   }
//   std::cout << "Enter start  vertex: ";
//   std::cin >> start;
//   std::cout << "Enter finish vertex: ";
//   std::cin >> finish;
//   s_time = clock();
//   try {
//       dst = FindDistance(start, finish, n, g);
//   } catch(std::out_of_range e) {
//       std::cout << e.what() << std::endl;
//       exit(EXIT_FAILURE);
//   }
//   f_time = clock();
    n = std::rand() % 30 + 1;
    g = RandomGenerateGraph(n);
    PrintGraph(g);
    start = 1; finish = n;
    s_time = clock();
    dst = FindDistance(start, finish, n, g);
    f_time = clock();
    std::cout << "Shortest distance between " << start << " and " << finish <<
        " is " << dst << std::endl;
    std::cout << "The running time of the algorithm: ";
    std::cout<< (f_time - s_time)<< " ms " << std::endl;
    return 0;
}

DstType FindDistance(int start, int finish, std::size_t n, const GraphType& d) {
    if (start < 1 || finish > static_cast<int>(n))
        throw std::out_of_range("Incorrect start or finish vertex");

    DstType* dst = new DstType[n];

    // Initially, the distance to all vertices is infinity
    for (int i = 0; i < static_cast<int>(n); ++i) dst[i] = -1;

    /* Lambda function that returns true if the distance to the
    vertex a is less than the distance to the vertex b    */
    auto set_function = [dst](DstType a, DstType b) {
        return (dst[a] < dst[b]) ||
            ((dst[a] == dst[b]) && (a < b));
    };

    // Set of unvisited vertices, ordered by non-decreasing distances to them
    std::set<int, decltype(set_function) > dst_queue(set_function);

    dst[start - 1] = 0;
    dst_queue.insert(start - 1);

    while (!dst_queue.empty()) {
        int next_vertex = *dst_queue.begin();
        dst_queue.erase(dst_queue.begin());
        for (int i = 0; i < static_cast<int>(n); ++i) {
            if (!d[next_vertex][i] || d[next_vertex][i] == -1)
                continue;
            if (dst[i] > dst[next_vertex] + d[next_vertex][i] || dst[i] == -1) {
                dst_queue.erase(i);
                dst[i] = dst[next_vertex] + d[next_vertex][i];
                dst_queue.insert(i);
            }
        }
    }
    return dst[finish - 1];
}

void PrintGraph(const GraphType& g) {
    std::cout << "Adjacency matrix: " << std::endl;
    for (const auto& s : g) {
        for (const auto& c : s) {
            std::cout << c << "\t";
        }
        std::cout << std::endl;
    }
}

GraphType RandomGenerateGraph(std::size_t n) {
    GraphType g;
    srand(static_cast<unsigned int>(time(0)));
    g.resize(n);
    for (int i = 0; i < static_cast<int>(n); ++i) {
        for (int j = 0; j < static_cast<int>(n); ++j) {
            if (i != j)
                g[i].push_back(std::rand() % 10 + 1);
            else
                g[i].push_back(0);
        }
    }
    return g;
}
