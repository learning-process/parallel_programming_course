// Copyright 2018 Nesterov Alexander
#include <assert.h>
#include <tbb/tbb.h>
#include <iostream>

int main(int argc, char* argv[]) {
    tbb::flow::graph g;
    tbb::flow::continue_node< tbb::flow::continue_msg > hello(g,
        [](const tbb::flow::continue_msg &) { std::cout << "Hello"; });

    tbb::flow::continue_node< tbb::flow::continue_msg> world(g,
        [](const tbb::flow::continue_msg &) { std::cout << " World\n"; });
    tbb::flow::make_edge(hello, world);
    hello.try_put(tbb::flow::continue_msg());
    g.wait_for_all();
    return 0;
}
