#ifndef PATH_H_
#define PATH_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include "../../typedefs.h"

struct Arc;
class Graph;

struct SuffixInfo {
    size_t startArcIdInParent{std::numeric_limits<size_t>::max()};
    CostType c{MAX_COST};
};

struct SuffixManager {
    std::unordered_map<Node, SuffixInfo> suffixes;
};

class Path{
    friend class Dijkstra;
    friend class BiobjectiveSearch;
    friend class NewAlgorithm;
public:

    Path();

    Node firstNode(const Graph &G) const;

    void addArc(ArcId a);

    inline void forbidArc(ArcId a);

    [[nodiscard]] inline Node getDeviationNode() const;

    inline void setDeviationNode(Node n);

    [[nodiscard]] inline CostType getTotalCosts() const;

    inline void setTotalCosts(CostType c);

    [[nodiscard]] inline CostType getPrefixReducedCost() const {
        return this->prefixCost;
    }

    inline void setPrefixReducedCost(CostType c) {
        this->prefixCost = c;
    }

    void print(const Graph& G) const;

    void printNewAlgo(const Graph& G) const;

    void printReversed(const Graph& G, std::size_t firstArcIndex, bool firstNode) const;

    [[nodiscard]] inline const ForbiddenArcs& getForbiddenArcs() const;

    inline void setArcs(std::vector<ArcId> vec);

    [[nodiscard]] inline const std::vector<ArcId> &getArcs() const;

    inline const std::vector<ArcId>& getPrefix() const {
        return this->prefix;
    }

    inline void setPrefix(std::vector<ArcId> p) {
        this->prefix = std::move(p);
    }
private:
    void reverse();

private:
    std::vector<ArcId> arcs;
    std::vector<ArcId> prefix;
    ForbiddenArcs forbiddenArcs;
    CostType cost;
    CostType prefixCost;
    Node deviationNode;
    std::size_t parentPathIndex;

public:
    [[nodiscard]] inline size_t getParentPathIndex() const {
        return parentPathIndex;
    }

    inline void setParentPathIndex(size_t parentPathIndex) {
        Path::parentPathIndex = parentPathIndex;
    }

    inline void setForbiddenArcs(const ForbiddenArcs& forbiddenArcs) {
        this->forbiddenArcs = forbiddenArcs;
    }

     [[nodiscard]] bool valid() const {
        return this->cost != MAX_COST;
    }
};

Node Path::getDeviationNode() const {
    return this->deviationNode;
}

CostType Path::getTotalCosts() const {
    return cost;
}

void Path::setTotalCosts(CostType c) {
    this->cost = c;
}

void Path::forbidArc(ArcId a) {
    this->forbiddenArcs.insert(a);
}

inline void Path::setArcs(std::vector<ArcId> vec) {
    this->arcs = std::move(vec);
}

const std::vector<ArcId> &Path::getArcs() const {
    return arcs;
}

inline void Path::setDeviationNode(Node n) {
    this->deviationNode = n;
}

const ForbiddenArcs& Path::getForbiddenArcs() const {
    return forbiddenArcs;
}

struct PathComparator {
    bool operator()(const std::unique_ptr<Path>& lhs, const std::unique_ptr<Path>& rhs) const {
        //Inverted ordering s.t. it works with std::prio_queue.
        return lhs->getTotalCosts() > rhs->getTotalCosts();
    }
};

#endif