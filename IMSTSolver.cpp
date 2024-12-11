#include <vector>
#include <list>
#include <utility> // for std::pair

class IMSTSolver {
public:
    virtual std::list<std::pair<int, int>> solve(int numVertices, 
                                                 const std::vector<std::pair<int, std::pair<int, double>>>& edges) = 0;
    virtual ~IMSTSolver() = default;
};
