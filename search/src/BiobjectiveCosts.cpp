#include "../../graph/includes/graph.h"
#include "../../graph/includes/Path.h"

#include "../includes/Dijkstra.h"

#include "../includes/BiobjectiveCosts.h"

BiobjectiveCostsVector::BiobjectiveCostsVector(const Graph& G):
    G{G}, costs(G.arcsCount), path{nullptr} {}

void BiobjectiveCostsVector::setReducedCosts(const Dijkstra& dijkstraSolution) {
    for (ArcId aId = 0; aId < G.arcsCount; ++aId) {
        const Arc& a = G.arc(aId);
        //printf("%u --> %u red costs %u\n", a.tail, a.head, dijkstraSolution.reducedCosts(a));
        this->costs[aId].c = dijkstraSolution.reducedCosts(a);
    }
}

void BiobjectiveCostsVector::setActivePath(const Path& newPath) {
    if (this->path != nullptr) {
        this->deactivateCurrentPath();
    }
    this->path = &newPath;
    for (ArcId aId : newPath.getArcs()) {
        this->costs[aId].inPath = true;
        //this->costs[aId].c = 0;
    }
}

void BiobjectiveCostsVector::deactivateCurrentPath() {
    for (ArcId aId : this->path->getArcs()) {
        this->costs[aId].inPath = false;
    }
}