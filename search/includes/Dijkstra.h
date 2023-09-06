#ifndef DIJKSTRA_H_
#define DIJKSTRA_H_

#include <memory>

#include "../../typedefs.h"

#include "./MemoryPool.h"

class Graph;
class SubGraph;
class Path;

struct Label {
    Node n{INVALID_NODE};
    ArcId predArc{INVALID_ARC};
    CostType c{MAX_COST};
    Label* next{nullptr}; //< For memory pool OR path reconstruction!
};

struct LabelComp {
    bool operator()(const Label* l, const Label* r) const {
        return l->c > r->c;
    }
};

class Dijkstra {
public:
    Dijkstra(const Graph& G, Node source, Node target, bool one2one, bool reversed);
    std::unique_ptr<Path> run();
    Path run(const SubGraph& sg, const PathNodes& targetNodes);

    [[nodiscard]] CostType reducedCosts(const Arc& a) const;

private:
    inline Label* newLabel(Node n, ArcId predArc, CostType c, Label* pred);

    Path buildPath(const Label* finalLabel) ;

    std::unique_ptr<Path> buildPathPtr(const Label* finalLabel);

private:
    const Graph& G;
    Pool<Label> memPool;
    const Node source;
    const Node target;
    std::vector<CostType> costs;
    std::vector<bool> permanent;
    const bool one2one;
    const bool reversed;
    bool computed;
};

Label* Dijkstra::newLabel(Node n, ArcId predArc, CostType c, Label* pred) {
    Label* l = this->memPool.newItem();
    l->n = n;
    l->predArc = predArc;
    l->c = c;
    l->next = pred;
    return l;
}

#endif