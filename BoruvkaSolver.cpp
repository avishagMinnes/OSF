#include <iostream>
#include <vector>
#include <list>
#include "IMSTSolver.cpp"

class BoruvkaSolver : public IMSTSolver {
public:
    std::list<std::pair<int, int>> solve(int numVertices, 
                                         const std::vector<std::pair<int, std::pair<int, double>>>& edges) override {
        std::list<std::pair<int, int>> mstEdges;

        // Borůvka's algorithm logic (simplified pseudocode for demo purposes)
        std::vector<int> component(numVertices);  // Initially, each vertex is its own component
        for (int i = 0; i < numVertices; ++i)
            component[i] = i;

        int numComponents = numVertices;

        while (numComponents > 1) {
            std::vector<int> cheapestEdge(numVertices, -1);

            // Find the cheapest edge for each component
            for (const auto& edge : edges) {
                int u = edge.first;
                int v = edge.second.first;
                double weight = edge.second.second;
                
                if (component[u] != component[v]) {
                    if (cheapestEdge[component[u]] == -1 || weight < edges[cheapestEdge[component[u]]].second.second) {
                        cheapestEdge[component[u]] = edge.first;
                    }
                    if (cheapestEdge[component[v]] == -1 || weight < edges[cheapestEdge[component[v]]].second.second) {
                        cheapestEdge[component[v]] = edge.first;
                    }
                }
            }

            // Add the cheapest edges to the MST and merge components
            for (int i = 0; i < numVertices; ++i) {
                if (cheapestEdge[i] != -1) {
                    int u = edges[cheapestEdge[i]].first;
                    int v = edges[cheapestEdge[i]].second.first;
                    double weight = edges[cheapestEdge[i]].second.second;

                    if (component[u] != component[v]) {
                        mstEdges.push_back({u, v});
                        numComponents--;

                        // Merge components
                        int oldComponent = component[v], newComponent = component[u];
                        for (int j = 0; j < numVertices; ++j) {
                            if (component[j] == oldComponent)
                                component[j] = newComponent;
                        }
                    }
                }
            }
        }

        std::cout << "Boruvka's Algorithm executed\n";
        return mstEdges;
    }
};
