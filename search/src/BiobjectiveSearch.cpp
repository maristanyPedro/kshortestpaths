#include <queue>
#include <cstdio>

#include "../../graph/includes/graph.h"
#include "../../graph/includes/Path.h"

#include "../includes/MemoryPool.h"
#include "../includes/BiobjectiveSearch.h"

BiobjectiveSearch::BiobjectiveSearch(const Graph& G, const BiobjectiveCostsVector& costs, Path& shortestPath, Pool<BiobjectiveLabel>& pool):
        G{G}, c{costs}, lastEfficient(G.nodesCount, MAX_COST), forbiddenArcs{&shortestPath.getForbiddenArcs()}, permanents(G.nodesCount), labelsPool{pool}, shortestPathCosts{shortestPath.getTotalCosts()} {
    lastEfficient.shrink_to_fit();
    //labelsPool.freeAll();
}

void BiobjectiveSearch::reset(Path& newParentPath) {
    this->forbiddenArcs = &newParentPath.getForbiddenArcs();
    std::fill(this->lastEfficient.begin(), this->lastEfficient.end(), MAX_COST);
    this->expansions = 0;
    //permanents.clear();
}

PathPtr BiobjectiveSearch::run(const Path& parentPath, const SuffixManager& suffixes, size_t parentPathIndex, CostType bound) {
    this->blockPrefix(parentPath);
    Node sourceNode{INVALID_NODE};
    size_t initialC2Value;
    CostType deviationArcReducedCosts = 0;
    if (parentPathIndex == 0) {
        sourceNode = this->G.source;
        assert(parentPath.firstNode(this->G) == this->G.source);
        initialC2Value = 0; //<< No arc fixed!
    }
    else {
        Node deviationNode = parentPath.firstNode(this->G);
        this->lastEfficient[deviationNode] = parentPath.getPrefix().size();
//        BiobjectiveLabel* blockingLabel = this->labelsPool.newItem();
//        blockingLabel->update(this->c, 0, deviationNode, INVALID_ARC, parentPath.getPrefix().size(), nullptr, false);
//        this->permanents[deviationNode].push_back(blockingLabel);
        sourceNode = this->G.arc(parentPath.getArcs()[0]).head;
        deviationArcReducedCosts = this->c[parentPath.getArcs()[0]].c;
        initialC2Value = parentPath.getPrefix().size() + 1; //<< The whole prefix AND the first arc fixed!
    }

    BiobjectiveLabel* initialLabel = labelsPool.newItem();
    initialLabel->update(this->c, 0, sourceNode, INVALID_ARC, initialC2Value, nullptr, false);

    std::priority_queue<BiobjectiveLabel*, std::vector<BiobjectiveLabel*>, BiobjectiveLabelComp> Q;
    Q.push(initialLabel);
    BiobjectiveLabel* extractedLabel = nullptr;
    BiobjectiveLabel* finalLabel = nullptr;
    CostType costEstimationToSourceNode = this->shortestPathCosts + parentPath.getPrefixReducedCost() + deviationArcReducedCosts;
    while (!Q.empty()) {
//        if (!suffixReached && iterations > parentPath.getArcs().size()*100) {
//            break;
//        }
        extractedLabel = Q.top();
        Q.pop();
        Node currentNode = extractedLabel->n;
        CostType costToTargetEstimation = costEstimationToSourceNode + extractedLabel->c;
        if (costToTargetEstimation >= bound) {
            break;
        }

        auto suffixIt = suffixes.suffixes.find(currentNode);
        if (extractedLabel->deviated && currentNode != this->G.target && suffixIt != suffixes.suffixes.end()) {
//            if (this->dominated(extractedLabel)) {
//                printf("Bro, que esto está dominado!\n");
//            }
            BiobjectiveLabel* shortcutLabel = labelsPool.newItem();
            shortcutLabel->update(this->c, extractedLabel->c + suffixIt->second.c, this->G.target, INVALID_ARC, extractedLabel->pathArcs, extractedLabel, true);
            Q.push(shortcutLabel);
            this->permanents[currentNode].push_back(extractedLabel);
            this->lastEfficient[currentNode] = extractedLabel->pathArcs;
            ++this->expansions;
            continue;
        }

        //STOPPING CONDITION!
        if (extractedLabel->deviated && currentNode==this->G.target) {
            assert(!this->dominated(currentNode, extractedLabel->pathArcs));
            //printf("The current label lies on a node on the path!!!\n");
            finalLabel = extractedLabel;
            //reached = true;
            break;
        }
        if (this->dominated(currentNode, extractedLabel->pathArcs)) {
            labelsPool.free(extractedLabel);
            continue;
        }
        ++this->expansions;
        bool success = false;
        for (ArcId aId : this->G.outgoingArcs(currentNode)) {
            if (this->forbiddenArcs->find(aId) != this->forbiddenArcs->end()) {
                continue;
            }
            const Arc& a{this->G.arc(aId)};
            const BiobjectiveCosts& aCosts{this->c[aId]};
            Node successorNode = a.head;
            uint16_t newPathArcs = extractedLabel->pathArcs + aCosts.inPath;
//            if (!this->permanents[successorNode].empty() && this->permanents[successorNode].back()->pathArcs <= newPathArcs) {
            if (this->dominated(successorNode, newPathArcs)) {
                continue;
            }
            else {
                success = true;
                CostType newPathCosts = extractedLabel->c + aCosts.c;
                BiobjectiveLabel* successorLabel = labelsPool.newItem();
                successorLabel->update(this->c, newPathCosts, successorNode, aId, newPathArcs, extractedLabel, false);
                Q.push(successorLabel);
            }
        }
        if (success) {
            this->permanents[currentNode].push_back(extractedLabel);
            this->lastEfficient[currentNode] = extractedLabel->pathArcs;
        }
        else {
            labelsPool.free(extractedLabel);
        }
    }
//    printf("%lu iterations on a path with %lu arcs. Ratio: %.2f\n",
//           iterations, parentPath.getArcs().size(), (float)iterations/parentPath.getArcs().size());
//    if (Q.empty()) {
//        printf("Stopped b/c empty after %lu iterations!\n", iterations);
//        printf("The finalLabel is null: %s\n", finalLabel == nullptr? "YES":"NO");
//    }
    PathPtr newPath = std::move(buildPath(finalLabel, parentPath, suffixes));
    labelsPool.freeAll();
//    if (!reached)
//        printf("ParentPath Arcs: %lu. Did %lu iterations. Reached: %s. Suff Reached %s\n", parentPath.getArcs().size(), iterations, reached?"YES":"NO", suffixReached?"YES":"NO");
    return newPath;
}

PathPtr BiobjectiveSearch::buildPath(const BiobjectiveLabel* l, const Path& parentPath, const SuffixManager& suffixes) {
    PathPtr newPath = std::make_unique<Path>();
    if (l != nullptr) {
        const BiobjectiveLabel *currentLabel = nullptr;
        if (l->suffixShortcut) {
            currentLabel = l->next;
        }
        else {
            currentLabel = l;
        }
        while (currentLabel->next != nullptr) {
            newPath->addArc(currentLabel->predArc);
            currentLabel = currentLabel->next;
        }
        newPath->reverse();
        if (l->suffixShortcut) {
            Node lastNodeBeforeSuffix = l->next->n;
            const SuffixInfo& suffixInfo = suffixes.suffixes.find(lastNodeBeforeSuffix)->second;
            const auto& parentArcs = parentPath.getArcs();
            auto firstSuffixArcIt = parentArcs.begin() + suffixInfo.startArcIdInParent;
            newPath->arcs.insert(newPath->arcs.end(), firstSuffixArcIt, parentArcs.end());
        }
        newPath->setTotalCosts(l->c);
    }
    return std::move(newPath);
}

void BiobjectiveSearch::blockPrefix(const Path& p) {
    assert(p.firstNode(G) == this->G.source || !p.getPrefix().empty());
    if (p.firstNode(G) == this->G.source) {
        assert(p.getPrefix().empty());
        return;
    }
    else {
        assert(!p.getPrefix().empty());
        size_t prefixArcs = 0;
        for (ArcId aId : p.getPrefix()) {
            const Arc& a = this->G.arc(aId);
            this->lastEfficient[a.tail] = prefixArcs++;
        }
        Node lastNode = this->G.arc(p.getPrefix().back()).head;
        this->lastEfficient[lastNode] = prefixArcs++;
    }
}