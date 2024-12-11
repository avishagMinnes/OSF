#include "Vertex.cpp"

class Edge {
public:
    Vertex source;
    Vertex destination;
    double weight;
    
    Edge(Vertex source, Vertex destination, double weight) 
        : source(source), destination(destination), weight(weight) {}
};
