#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <algorithm>

#include "graph.H"

//traverse the graph using BFS and transfer vertices into buckets based on their weights.
void Graph::graph_lb(int ranks) 
{
    std::unordered_map<int, bool> visited;
    std::queue<int> bfs_queue;
    std::unordered_map<int, std::pair<std::vector<Vertex>, double>> buckets;
  

    int current_vertex = 0;
    double max_weight = -1;
    double total_weight = 0.0;
    double bucket_weight = 0.0;
    int bucket_idx = 0;

    //search the vertices to find the vertex with the max weight. 
    for (const auto& vertex : vertices) 
    {
        if (vertex.weight > max_weight) 
        {
            current_vertex = vertex.vertex_id;
            max_weight = vertex.weight;
        }
        total_weight += vertex.weight;
    }

    // Start BFS from the vertex with the maximum weight.
    visited[current_vertex] = true;
    bfs_queue.push(current_vertex);

    std::cout << "Graph Traversal start with: " << current_vertex << "\n\n";

    while (!bfs_queue.empty()) 
    {
        current_vertex = bfs_queue.front();
        bfs_queue.pop();
        std::cout << "Visited: " << current_vertex << "\n";

        //find neighbors of currentVertex
        for (const auto& edge: edges[current_vertex])
        {
            if (!visited[edge.vertex_2])
            {
                visited[edge.vertex_2] = true;
                bfs_queue.push(edge.vertex_2);
            }
        }

        //assign current vertex to a bucket based on its weight
        // assume weights divide evenly into ranks

       if (bucket_weight < total_weight / ranks) 
       {
            buckets[bucket_idx].first.push_back(current_vertex);
            bucket_weight += vertices[current_vertex].weight;
            buckets[bucket_idx].second = bucket_weight;
        } 
        else 
        {
            bucket_weight = 0.0;
            bucket_idx++;
            buckets[bucket_idx].first.push_back(current_vertex);
            bucket_weight += vertices[current_vertex].weight;
            buckets[bucket_idx].second = bucket_weight;
        }
    }
     
    printBuckets(buckets);
}

void Graph::printBuckets(const std::unordered_map<int, std::pair<std::vector<Vertex>, double>>& buckets) const
{
    for (const auto& [bucket_idx, data] : buckets) 
    {
        std::cout << "Bucket " << bucket_idx << " (Total Weight: " << data.second << "): ";
        for (const auto& vertex : data.first) 
        {
            std::cout << vertex.vertex_id << " ";
        }
        std::cout << "\n";
    }
}

// adjust partition based on edge weights
