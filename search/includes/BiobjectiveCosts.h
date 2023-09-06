#ifndef BIOBJECTIVE_COSTS_H_
#define BIOBJECTIVE_COSTS_H_

#include <vector>

#include "../../typedefs.h"

class Graph;
class Path;
class Dijkstra;

struct BiobjectiveCosts{
    CostType c{MAX_COST};
    bool inPath{false};
};

class BiobjectiveCostsVector{
public:
    explicit BiobjectiveCostsVector(const Graph& G);

    inline const BiobjectiveCosts& operator[](ArcId aId) const {
        return this->costs[aId];
    }

    void setReducedCosts(const Dijkstra& dijkstraSolution);

    void setActivePath(const Path& newPath);

    void deactivateCurrentPath();

private:

    const Graph& G;
    std::vector<BiobjectiveCosts> costs;
    const Path* path;
};

#endif