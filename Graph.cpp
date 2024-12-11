#include <unordered_map>
#include <vector>
#include <list>
#include <iostream>

class Graph {
private:
    // Adjacency list: each vertex maps to a list of pairs (neighbor vertex, weight)
    std::unordered_map<int, std::list<std::pair<int, double>>> adjList;
    int numVertices;
    int numEdges;

public:
    Graph(int numVertices) : numVertices(numVertices), numEdges(0) {}

    // Add a directed edge from vertex u to vertex v with weight w
    void addEdge(int u, int v, double w) {
        adjList[u].push_back(std::make_pair(v, w));
        numEdges++;
    }

    // Remove an edge from u to v
    void removeEdge(int u, int v) {
        auto& neighbors = adjList[u];
        neighbors.remove_if([v](const std::pair<int, double>& edge) {
            return edge.first == v;
        });
        numEdges--;
    }

    // Get all the neighbors of a given vertex
    std::list<std::pair<int, double>> getNeighbors(int u) const {
        if (adjList.find(u) != adjList.end()) {
            return adjList.at(u);
        }
        return std::list<std::pair<int, double>>();  // Return empty list if u is not found
    }

    // Get the number of vertices in the graph
    int getNumVertices() const {
        return numVertices;
    }

    // Get the number of edges in the graph
    int getNumEdges() const {
        return numEdges;
    }
};

//example usage
int main() {
    Graph graph(5);  // Create a graph with 5 vertices

    // Add directed edges with weights
    graph.addEdge(0, 1, 10.0);
    graph.addEdge(0, 3, 5.0);
    graph.addEdge(1, 2, 1.0);
    graph.addEdge(3, 4, 2.0);

    // Retrieve neighbors of vertex 0
    for (const auto& neighbor : graph.getNeighbors(0)) {
        std::cout << "Vertex 0 has neighbor " << neighbor.first << " with weight " << neighbor.second << std::endl;
    }

    return 0;
}

