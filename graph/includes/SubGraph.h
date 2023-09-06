#ifndef SUBGRAPH_H_
#define SUBGRAPH_H_

#include <vector>

#include "../../typedefs.h"

class Graph;

class SubGraph {
public:

    SubGraph(const Graph&G);

    SubGraph(const Graph&G, ForbiddenArcs& forbiddenArcs);

    SubGraph(const SubGraph& other);

    /**
        * Deactivate all incoming arcs of n in G. Leaves outgoing arcs active.
 * @param G Original graph storing all adjacency information.
 * @param n Node in G which incoming arcs are deactivated in this SubGraph.
     * @param forbiddenArcs Tight representation of this SubGraph as a list of forbidden arcs only.
     * The vector is altered in this functions and contains the incoming arcs of n after the call.
 */
    void makeUnreachable(const Graph& G, Node n, ForbiddenArcs& forbiddenArcs);

    inline void deactivateArc(ArcId aId, ForbiddenArcs& forbiddenArcs) {
        this->activeArcs[aId] = false;
        forbiddenArcs.insert(aId);
    }

    inline void deactivateNode(Node n) {
        this->activeNodes[n] = false;
    }

    /**
     * Returns true if and only if the arc with aId is active in this subgraph.
     * @param aId The id of the arc to be checked.
     * @return True if and only if the arc with id aId is active. False otherwise.
     */
    inline bool activeArc(ArcId aId) const {
        return this->activeArcs[aId];
    }

    inline bool activeNode(Node n) const {
        return this->activeNodes[n];
    }

private:
    std::vector<bool> activeArcs;
    std::vector<bool> activeNodes;
};

#endif