#include "../includes/graph.h"

#include "../includes/SubGraph.h"

using namespace std;

SubGraph::SubGraph(const Graph&G):
    activeArcs(G.arcsCount, true), activeNodes(G.nodesCount, true) {}

SubGraph::SubGraph(const Graph&G, ForbiddenArcs& forbiddenArcs):
    activeArcs(G.arcsCount, true), activeNodes(G.nodesCount, true) {
    for (ArcId aId : forbiddenArcs) {
        this->activeArcs[aId] = false;
    }
}

SubGraph::SubGraph(const SubGraph& other):
    activeArcs(other.activeArcs), activeNodes(other.activeNodes) {}

void SubGraph::makeUnreachable(const Graph& G, Node n, ForbiddenArcs& forbiddenArcs) {
    for (auto& arcId : G.incomingArcs(n)) {
        this->deactivateArc(arcId, forbiddenArcs);
    }
}