#include <queue>

#include "../../graph/includes/graph.h"
#include "../../graph/includes/SubGraph.h"
#include "../../graph/includes/Path.h"

#include "../includes/Dijkstra.h"


Dijkstra::Dijkstra(const Graph& G, Node source, Node target, bool one2one, bool reversed):
    G{G}, source{source}, target{target},
    costs(G.nodesCount, MAX_COST), permanent(G.nodesCount, false),
    one2one{one2one}, reversed{reversed}, computed{false} {}

std::unique_ptr<Path> Dijkstra::run() {
    std::priority_queue<Label*, std::vector<Label*>, LabelComp> Q;
    Label* initialLabel = this->newLabel(this->source, INVALID_ARC, 0, nullptr);
    Q.push(initialLabel);
    Label* finalLabel = nullptr;

    while (!Q.empty()) {
        Label* l = Q.top();
        Q.pop();
        if (permanent[l->n]) {
            this->memPool.free(l);
            continue;
        }
        permanent[l->n] = true;
        costs[l->n] = l->c;
        if (l->n == this->target) {
            finalLabel = l;
            if (this->one2one) {
                break;
            }
        }
        const std::vector<ArcId>& arcs = this->reversed? this->G.incomingArcs(l->n) : this->G.outgoingArcs(l->n);
        for (auto& aId : arcs) {
            const Arc& arc{this->G.arc(aId)};
            Node successor = this->reversed? arc.tail : arc.head;
            if (!permanent[successor]) {
                Label* newLabel = this->newLabel(successor, aId, l->c + arc.c, l);
                Q.push(newLabel);
            }
        }
        //this->memPool.free(l);
    }
    computed = true;
    return std::move(buildPathPtr(finalLabel));
}

Path Dijkstra::run(const SubGraph& sg, const PathNodes& targetNodes) {
    std::priority_queue<Label*, std::vector<Label*>, LabelComp> Q;
    Label* initialLabel = this->newLabel(this->source, INVALID_ARC, 0, nullptr);
    Q.push(initialLabel);
    Label* finalLabel = nullptr;

    while (!Q.empty()) {
        Label* l = Q.top();
        Q.pop();
        if (permanent[l->n]) {
            this->memPool.free(l);
            continue;
        }
        permanent[l->n] = true;
        if (l->n == G.target) {
            finalLabel = l;
            break;
        }
        const std::vector<ArcId>& arcs = this->reversed? this->G.incomingArcs(l->n) : this->G.outgoingArcs(l->n);
        for (auto& aId : arcs) {
            if (!sg.activeArc(aId)) {
                continue;
            }
            const Arc& arc{this->G.arc(aId)};
            if (!permanent[arc.head]) {
                Label* newLabel = this->newLabel(arc.head, aId, l->c + arc.c, l);
                Q.push(newLabel);
            }
        }
        //this->memPool.free(l);
    }
    computed = true;
    return buildPath(finalLabel);
}

Path Dijkstra::buildPath(const Label* finalLabel) {
    Path p;
    if (finalLabel != nullptr) {
        p.setTotalCosts(finalLabel->c);
        const Label *it = finalLabel;
        while (it->predArc != INVALID_ARC) {
            p.addArc(it->predArc);
            it = it->next;
        }
        if (!this->reversed) {
            p.reverse();
        }
    }
    return p;
}

std::unique_ptr<Path> Dijkstra::buildPathPtr(const Label* finalLabel) {
    auto p = std::make_unique<Path>();
    if (finalLabel != nullptr) {
        p->setTotalCosts(finalLabel->c);
        const Label *it = finalLabel;
        while (it->predArc != INVALID_ARC) {
            p->addArc(it->predArc);
            it = it->next;
        }
        if (!this->reversed) {
            p->reverse();
        }
    }
    return std::move(p);
    //return std::move(p);
}

CostType Dijkstra::reducedCosts(const Arc& a) const {
    if (!this->computed) {
        printf("I cannot compute reduced costs because the shortest path search did not ran yet.");
        exit(1);
    }
    if (!this->reversed || this->one2one) {
        printf("Please configure your Dijkstra search to run as a one-to-all search on the REVERSED digraph! Otherwise reduced costs cannot be computed.");
        exit(1);
    }
    if (!this->permanent[a.head]) {
        return MAX_COST;
    }
    else {
        return a.c + this->costs[a.head] - this->costs[a.tail];
    }
}