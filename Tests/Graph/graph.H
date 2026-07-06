#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>


struct Vertex 
{
    int vertex_id;
    double weight;
    Vertex(int id, double weight = 1.0)
        : vertex_id(id), weight(weight) {}
};

//structure for edges
struct Edge 
{
    int    vertex_1;
    int    vertex_2;
    double weight;   
    Edge(int from, int to, double weight = 1.0)
        : vertex_1(from), vertex_2(to), weight(weight) {}
};

//structure for graph
struct Graph 
{
    //Create data structure for node and edges with their respective weights
    std::vector<Vertex> vertices; 
    std::unordered_map<int, std::vector<Edge>> edges;

    Graph(std::vector<int> v_ids, std::vector<int> v_wgts);

    // methods for graph
    void addVertex(int,double);
    void addEdge(int,int,double);
    void print() const;
    void graph_lb(int);
    void printBuckets(const std::unordered_map<int, std::pair<std::vector<Vertex>, double>>& buckets) const;

};




