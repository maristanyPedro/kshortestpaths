#ifndef BIOBJECTIVE_SEARCH_H_
#define BIOBJECTIVE_SEARCH_H_

#include <unordered_map>

#include "../../typedefs.h"

#include "./MemoryPool.h"
#include "./BiobjectiveCosts.h"

class Graph;
class SuffixManager;

struct BiobjectiveLabel {

    void update(const BiobjectiveCostsVector& bcv, CostType cNew, Node nNew, ArcId predArcNew, uint16_t pathArcsNew, BiobjectiveLabel* predLabelNew, bool shortcut) {
        this->c = cNew; this->n = nNew; this->predArc = predArcNew; this->pathArcs = pathArcsNew; this->next = predLabelNew;
        this->suffixShortcut = shortcut;
        if (predArcNew == INVALID_ARC && !shortcut) { ///< Only happens for the initial label.
            this->deviated = false;
            this->deviationNode = INVALID_NODE;
        }
        else {
            assert(predLabelNew != nullptr);
            if (predLabelNew->deviated) {
                this->deviated = true;
                this->deviationNode = predLabelNew->deviationNode;
            }
            else {
                this->deviated = !bcv[predArcNew].inPath;
                if (deviated) {
                    this->deviationNode = this->n;
                }
            }
        }
    }

    CostType c{MAX_COST};
    Node n{INVALID_NODE};
    ArcId predArc{INVALID_ARC};
    uint16_t pathArcs{std::numeric_limits<uint16_t>::max()};
    Node deviationNode{INVALID_NODE};
    BiobjectiveLabel* next{nullptr};
    bool deviated{false};
    bool suffixShortcut{false};
};

struct BiobjectiveLabelComp {
    bool operator()(BiobjectiveLabel* lhs, BiobjectiveLabel* rhs) const {
        if (lhs->c > rhs->c) {
            return true;
        }
        else if (lhs->c == rhs->c) {
            return lhs->pathArcs > rhs->pathArcs;
        }
        else {
            return false;
        }
    }
};


typedef std::vector<BiobjectiveLabel*> Permanents;
typedef std::unique_ptr<Path> PathPtr;

class BiobjectiveSearch{
public:
    BiobjectiveSearch(const Graph& G, const BiobjectiveCostsVector& costs, Path& shortestPath, Pool<BiobjectiveLabel>& pool);

    PathPtr run(const Path& parentPath, const SuffixManager& suffixes, size_t parentPathIndex, CostType bound = MAX_COST);

    void reset(Path& newParentPath);

    void reset();

private:

    void blockPrefix(const Path& p);

    inline bool dominated(Node n, uint16_t pathArcs) {
        return this->lastEfficient[n] <= pathArcs;
    }

    static PathPtr buildPath(const BiobjectiveLabel* l, const Path& parentPath, const SuffixManager& suffixes);

    [[nodiscard]] bool inParentPath(Node n) const;

    const Graph& G;
    const BiobjectiveCostsVector& c;
    std::vector<CostType> lastEfficient;
    Pool<BiobjectiveLabel>& labelsPool;
    const ForbiddenArcs* forbiddenArcs;
    //const PathNodes& targetNodes;
//    std::unordered_map<Node, Permanents> permanents;
    std::vector<Permanents> permanents;
    const CostType shortestPathCosts;
public:
    std::size_t expansions{0};
};

#endif