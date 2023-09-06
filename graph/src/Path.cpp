#include <algorithm>
#include <iostream>
#include <cassert>
#include "../includes/graph.h"

#include "../includes/Path.h"

Path::Path():
    cost{MAX_COST}, prefixCost{MAX_COST},
    deviationNode{INVALID_NODE}, parentPathIndex{std::numeric_limits<size_t>::max()} {}

Node Path::firstNode(const Graph &G) const {
    return G.arc(this->getArcs()[0]).tail;
}

void Path::addArc(ArcId a) {
    this->arcs.push_back(a);
}

void Path::reverse() {
    std::reverse(this->arcs.begin(), this->arcs.end());
}

void Path::print(const Graph& G) const {
    std::cout << "c: " << this->getTotalCosts() << "; parent: " << this->getParentPathIndex() << "; devNode: " << this->getDeviationNode() << "; Path:" << G.arc(this->arcs[0]).tail;
    for (ArcId aId : this->arcs) {
        std::cout << " " << G.arc(aId).head;
    }
    std::cout << std::endl;
}

void Path::printNewAlgo(const Graph& G) const {
    if (!this->prefix.empty()) {
        std::cout << "c: " << this->getTotalCosts() << "; parent: " << this->getParentPathIndex() << "; Path:"
                  << G.arc(this->prefix[0]).tail;
        for (ArcId aId: this->prefix) {
            std::cout << " " << G.arc(aId).head;
        }
    }
    else {
        std::cout << "c: " << this->getTotalCosts() << "; parent: " << this->getParentPathIndex() << "; Path:"
                  << G.arc(this->arcs[0]).tail << ",";
    }
    for (ArcId aId : this->arcs) {
        std::cout << "," << G.arc(aId).head;
    }
    std::cout << std::endl;
}

void Path::printReversed(const Graph& G, std::size_t firstArcIndex, bool firstNode) const {
    if (firstNode) {
        printf("%u ", G.arc(this->arcs[firstArcIndex]).head);
    }
    for (size_t i = firstArcIndex; i != std::numeric_limits<size_t>::max(); --i) {
        printf("%u ", G.arc(this->arcs[i]).tail);
    }
    //printf("\n");
}
