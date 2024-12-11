#include <iostream>
#include <cassert>
#include <algorithm>  // Include the algorithm header for std::find
#include "Graph.cpp"  // Include your Graph implementation

void testGraphCreation() {
    Graph g(5);  // Create a graph with 5 vertices
    assert(g.getNumVertices() == 5);
    assert(g.getNumEdges() == 0);
    std::cout << "testGraphCreation passed!" << std::endl;
}

void testAddEdge() {
    Graph g(5);
    
    g.addEdge(0, 1, 10.0);
    g.addEdge(0, 3, 5.0);

    assert(g.getNumEdges() == 2);
    
    auto neighbors = g.getNeighbors(0);
    bool foundEdge1 = false, foundEdge2 = false;
    for (const auto& neighbor : neighbors) {
        if (neighbor.first == 1 && neighbor.second == 10.0) foundEdge1 = true;
        if (neighbor.first == 3 && neighbor.second == 5.0) foundEdge2 = true;
    }

    assert(foundEdge1 && foundEdge2);  // Check if edges were added correctly
    std::cout << "testAddEdge passed!" << std::endl;
}

void testRemoveEdge() {
    Graph g(5);
    
    g.addEdge(0, 1, 10.0);
    g.addEdge(0, 3, 5.0);
    g.addEdge(1, 2, 7.5);

    // Remove one edge and verify
    g.removeEdge(0, 1);
    assert(g.getNumEdges() == 2);

    auto neighbors = g.getNeighbors(0);
    for (const auto& neighbor : neighbors) {
        assert(neighbor.first != 1);  // Edge 0 -> 1 should be removed
    }

    std::cout << "testRemoveEdge passed!" << std::endl;
}

void testAddDuplicateEdge() {
    Graph g(5);
    
    g.addEdge(0, 1, 10.0);
    g.addEdge(0, 1, 10.0);  // Adding the same edge again

    // Make sure duplicate edges are not added
    assert(g.getNumEdges() == 1);

    auto neighbors = g.getNeighbors(0);
    int count = 0;
    for (const auto& neighbor : neighbors) {
        if (neighbor.first == 1 && neighbor.second == 10.0) {
            count++;
        }
    }
    assert(count == 1);  // Should only have one edge 0 -> 1
    std::cout << "testAddDuplicateEdge passed!" << std::endl;
}

void testEdgeUpdate() {
    Graph g(5);
    
    g.addEdge(0, 1, 10.0);
    g.addEdge(0, 1, 15.0);  // Updating the weight of the edge

    auto neighbors = g.getNeighbors(0);
    bool foundUpdatedEdge = false;
    for (const auto& neighbor : neighbors) {
        if (neighbor.first == 1 && neighbor.second == 15.0) {
            foundUpdatedEdge = true;
        }
    }

    assert(foundUpdatedEdge);  // Check that the edge weight was updated
    assert(g.getNumEdges() == 1);  // No new edges should be added
    std::cout << "testEdgeUpdate passed!" << std::endl;
}

void testGetNeighbors() {
    Graph g(5);
    
    g.addEdge(0, 1, 10.0);
    g.addEdge(0, 2, 20.0);
    g.addEdge(0, 3, 30.0);

    auto neighbors = g.getNeighbors(0);
    assert(neighbors.size() == 3);  // There should be 3 neighbors for vertex 0

    std::vector<int> neighborVertices;
    for (const auto& neighbor : neighbors) {
        neighborVertices.push_back(neighbor.first);
    }

    std::vector<int> expectedNeighbors = {1, 2, 3};
    for (int vertex : expectedNeighbors) {
        assert(std::find(neighborVertices.begin(), neighborVertices.end(), vertex) != neighborVertices.end());
    }

    std::cout << "testGetNeighbors passed!" << std::endl;
}

void testNonExistentVertex() {
    Graph g(5);

    auto neighbors = g.getNeighbors(10);  // Vertex 10 does not exist
    assert(neighbors.empty());  // Should return an empty list for non-existent vertex

    std::cout << "testNonExistentVertex passed!" << std::endl;
}

int main() {
    testGraphCreation();
    testAddEdge();
    testRemoveEdge();
    testAddDuplicateEdge();
    testEdgeUpdate();
    testGetNeighbors();
    testNonExistentVertex();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
