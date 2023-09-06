#include <queue>
#include <cstdio>

#include "../../graph/includes/graph.h"
#include "../../graph/includes/SubGraph.h"

#include "../includes/Dijkstra.h"
#include "../includes/Yen.h"

using namespace std;

Yen::Yen(const Graph &G, size_t k):
    G{G}, k{k} {}

Path& Yen::storeSolution(const Path& p){
    this->solutions.push_back(p);
    return this->solutions.back();
}

void Yen::addSuffix(const Path& parentPath, Path& newPath, size_t deviationArcIndexInParent) {
    Node hitNode = this->G.arc(newPath.getArcs().back()).head;
    if (hitNode == this->G.target) {
        return;
    }
    const auto& parentArcs = parentPath.getArcs();
    //Initially, the tail of parentPathIterator is the initial node of the search that lead to newPath.
    while (deviationArcIndexInParent < parentArcs.size()) {
        ArcId parentArcIterator = parentArcs[deviationArcIndexInParent];
        Node currentHead = G.arc(parentArcIterator).head;
        if (currentHead == hitNode) {
            break;
        }
        ++deviationArcIndexInParent;
    }
    ++deviationArcIndexInParent;
    newPath.arcs.insert(newPath.getArcs().end(), parentArcs.begin()+deviationArcIndexInParent, parentArcs.end());
    CostType suffixCost = 0;
    for (auto it = parentArcs.begin()+deviationArcIndexInParent; it != parentArcs.end(); ++it) {
        suffixCost += this->G.arc(*it).c;
    }
    newPath.setTotalCosts(newPath.getTotalCosts() + suffixCost);
}

void Yen::run() {
    Dijkstra spp(G, this->G.source, this->G.target, true, false);
    Path sp = spp.run();
    sp.setDeviationNode(this->G.source);
    sp.setParentPathIndex(numeric_limits<size_t>::max());
    sp.setIndexOfLastArcInParent(sp.getArcs().size()-1);
    sp.setPrefixCost(0);
    std::priority_queue<Path, std::vector<Path>, PathComparator> candidatePaths;
    candidatePaths.push(sp);
    size_t iterationCounter = 0;

    while (iterationCounter < this->k) {
        if (candidatePaths.empty()) {
            printf("After %lu iterations, heap is empty! Can't find %lu s-t-paths!\n", iterationCounter, this->k);
            break;
        }
        Path& lastPath = this->storeSolution(candidatePaths.top());
        PathNodes& lastPathNodes = lastPath.getNodesWithoutSource(G);
        candidatePaths.pop();
        //printf("%lu: solution:\n\t\t", iterationCounter);
        //lastPath.print(this->G);
        ++iterationCounter;

        CostType prefixCost = lastPath.getPrefixCost();
        ForbiddenArcs forbiddenArcs = lastPath.getForbiddenArcs();
        SubGraph subGraph(this->G, forbiddenArcs);
        subGraph.makeUnreachable(this->G, lastPath.getDeviationNode(), forbiddenArcs);

        for (size_t i = 0; i < lastPath.getArcs().size(); ++i) {
            ArcId aId = lastPath.getArcs()[i];
            const Arc& arc = this->G.arc(aId);
            Node currentDeviationNode = arc.tail;
            //Now deactivate arcs incrementally!
            subGraph.deactivateArc(aId, forbiddenArcs);
            //Dijkstra with deactivated arcs from currentDeviationNode to Target!
            Dijkstra suffixSearch(this->G, currentDeviationNode, this->G.target, true, false);
            Path newSuffix = suffixSearch.run(subGraph, lastPathNodes);
            if (newSuffix.valid()) {
                this->addSuffix(lastPath, newSuffix, i);
                newSuffix.setTotalCosts(prefixCost + newSuffix.getTotalCosts());
                newSuffix.setPrefixCost(prefixCost);

                assert(this->G.arc(newSuffix.getArcs().front()).tail == currentDeviationNode);
                newSuffix.setDeviationNode(currentDeviationNode);
                newSuffix.setParentPathIndex(this->solutions.size() - 1);
                newSuffix.setIndexOfLastArcInParent(i);
                newSuffix.setForbiddenArcs(forbiddenArcs);
//                printf("CANDIDATE:\n\t\t");
//                newSuffix.print(this->G);
                candidatePaths.push(newSuffix);
            }
            lastPathNodes.erase(arc.head);
            subGraph.makeUnreachable(this->G, arc.head, forbiddenArcs);
            prefixCost += arc.c;
        }
    }
}

void Yen::printPaths() const {
    for (size_t i = 0; i < solutions.size(); ++i) {
        const Path* p = &solutions[i];
        printf("Printing %lu shortest path with cost: %u  and parent %lu --> ", i, p->getTotalCosts(), p->getParentPathIndex());
        p->printReversed(this->G, p->getArcs().size()-1, true);
        while (this->G.arc(p->getArcs().front()).tail != this->G.source) {
            size_t devIndex = p->getIndexOfLastArcInParent();
            p = &solutions[p->getParentPathIndex()];
            p->printReversed(this->G, devIndex-1, false);
        }
        printf("\n");
    }
}