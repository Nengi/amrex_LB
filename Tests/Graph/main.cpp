#include <iostream>
#include <vector>
#include <unordered_map>    
#include <algorithm>

#include "graph.H"

int main() {

    // generate graph and test
    
    std::vector<int> vertex_ids = {0, 1, 2, 3, 4};
    std::vector<int> vertex_weights = {1, 1, 4, 3, 1};

    Graph g(vertex_ids, vertex_weights);

    g.addEdge(0, 1, 1.0);
    g.addEdge(0, 2, 1.0);
    g.addEdge(0, 3, 1.0);
    g.addEdge(0, 4, 1.0);

    g.addEdge(1, 2, 1.0);

    g.addEdge(3, 4, 1.0);



    std::cout << "=== Graph with Node and Edge Weights ===\n";

    g.print();

    g.graph_lb(2);
    
    return 0;
}

