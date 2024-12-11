class Vertex {
public:
    int id;
    Vertex(int id) : id(id) {}
    
    bool operator==(const Vertex& other) const {
        return this->id == other.id;
    }
};