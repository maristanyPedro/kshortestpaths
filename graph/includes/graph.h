#ifndef GRAPH_H_
#define GRAPH_H_

#include <memory> //for unique_ptr
#include <unordered_set>
#include <string>
#include <vector>

#include "../../typedefs.h"

typedef std::unordered_set<ArcId> ForbiddenArcs;
typedef std::unordered_set<Node> PathNodes;

struct Arc {
    Arc() = default;
    Arc(Node tailId, Node headId, CostType c1);

    Arc& operator=(const Arc& other) {
        this->tail = other.tail; this->head = other.head; this->c = other.c;
        return *this;
    }

    void print() const;

    //The node at which the arc starts at. tail ---> head
    Node tail{INVALID_NODE};

    //The node at which the arc points at. tail ---> head
    Node head{INVALID_NODE};
    CostType c{MAX_COST};
    //uint16_t lastProcessedPred{0};
};

struct ArcSorter{
    bool operator()(const Arc& lhs, const Arc& rhs) const {
        if (lhs.tail < rhs.tail) {
            return true;
        }
        if (lhs.tail == rhs.tail) {
            return lhs.c < rhs.c;
        }
        return false;
    }
};

typedef std::vector<ArcId> Neighborhood;

class NodeAdjacency {
public:
    NodeAdjacency();
    NodeAdjacency(Node nid, std::size_t incomingArcsCt, std::size_t outgoingArcsCt);
    //private:
    Neighborhood incomingArcs;
    Neighborhood outgoingArcs;
    Node id;
};

class Graph {
public:
    Graph(std::string name, Node nodesCount, ArcId arcsCount, std::vector<Arc>& arcs, Node source, Node target);

    [[nodiscard]] inline const Neighborhood& outgoingArcs(const Node nodeId) const {
        return this->nodes[nodeId].outgoingArcs;
    }

    [[nodiscard]] inline const Neighborhood& incomingArcs(const Node nodeId) const {
        return this->nodes[nodeId].incomingArcs;
    }

//    inline Neighborhood& outgoingArcs(const Node nodeId) {
//        return this->nodes[nodeId].outgoingArcs;
//    }
//
//    inline Neighborhood& incomingArcs(const Node nodeId) {
//        return this->nodes[nodeId].incomingArcs;
//    }

    [[nodiscard]] const NodeAdjacency& node(Node nodeId) const;
    NodeAdjacency& node(Node nodeId);

    [[nodiscard]] const Arc& arc(ArcId aId) const;
    Arc& arc(ArcId aId);

    [[maybe_unused]] void printNodeInfo(Node nodeId) const;
    void printArcs(const Neighborhood & arcs) const;

    void setNodeInfo(Node n, NeighborhoodSize inDegree, NeighborhoodSize outDegree);

    CostType calculatePathCosts(const std::vector<Node>& pathNodes) const;

    const std::string name;
    const Node nodesCount;
    const ArcId arcsCount;
    const Node source;
    const Node target;

private: //Members
    std::vector<NodeAdjacency> nodes;
    std::vector<Arc> arcs;
};

inline const NodeAdjacency& Graph::node(const Node nodeId) const {
    return this->nodes[nodeId];
}

inline NodeAdjacency& Graph::node(const Node nodeId) {
    return this->nodes[nodeId];
}

inline const Arc& Graph::arc(ArcId aId) const {
    return this->arcs[aId];
}

inline Arc& Graph::arc(ArcId aId) {
    return this->arcs[aId];
}

std::unique_ptr<Graph> setupGraph(const std::string& filename, Node sourceId, Node targetId);

//inline const Neighborhood& forwardStar(const Graph& G, const Node nodeId) {
//    return G.outgoingArcs(nodeId);
//}
//inline const Neighborhood & backwardStar(const Graph& G, const Node nodeId) {
//    return G.incomingArcs(nodeId);
//}

void split(const std::string& s, char delim, std::vector<std::string>& elems);

std::vector<std::string> split(const std::string& s, char delim);


#endif