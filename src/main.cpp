
#include <iostream>
#include "Graph/GraphManager.hpp"
using namespace Moonlight::Graph;

void TestGraphManager(){
    std::cout << "Start TestGraphManager" << std::endl;
    auto& graph = GraphManager<>::Instance().GetAEmptyGraph();
    graph.AddVertex();
}

int main(){
    TestGraphManager();

    return 0;
}