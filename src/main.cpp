#define __PRINT_DEBUG_INFO__ 1

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "Graph/GraphManager.hpp"
using namespace Moonlight::Graph;

void TestGraphManager(){

    std::cout << "Start TestGraphManager" << std::endl;
    auto& graph = GraphManager<>::Instance().GetAEmptyGraph();
    graph.AddVertex();
}
#define shift(xs, xs_sz) ( xs_sz <= 0 ? (fprintf(stderr, "%s:%d: OUT OF BOUNDS BRUUH", __FILE__, __LINE__), abort()) : (void)0, --xs_sz, *xs++)

int main(int argc, char **argv){
    // TestGraphManager();
    // while(argc > 0){
    //     printf("%s\n", shift(argv, argc));
    // }
    // shift(argv, argc);

    return 0;
}