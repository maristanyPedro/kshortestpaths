#include <queue>
#include <cstdio>
#include <ctime>
#include <chrono>

#include "../../graph/includes/graph.h"

#include "../includes/BucketHeap.h"
#include "../includes/Dijkstra.h"
#include "../includes/BiobjectiveSearch.h"
#include "../includes/NewAlgorithm.h"

NewAlgorithm::NewAlgorithm(const Graph &G, size_t k):
    G{G}, currentCosts{G}, k{k}, relevantForDeviations(k, true) {
    this->solutions.reserve(k);
}

std::unique_ptr<SuffixManager> NewAlgorithm::generateSuffixes(Path* p) const {
    std::unique_ptr<SuffixManager> newSuffixes = std::make_unique<SuffixManager>();
    const std::vector<ArcId> &arcs = p->getArcs();
    CostType c = 0;
    for (std::vector<int>::size_type arcIterator = arcs.size() - 1;
         arcIterator != (std::vector<int>::size_type) -1; arcIterator--) {
        ArcId aId = arcs[arcIterator];
        const Arc &a = this->G.arc(aId);
        newSuffixes->suffixes.insert({a.head, {arcIterator + 1, c}});
        c += this->currentCosts[aId].c;
    }
    return std::move(newSuffixes);
}

//void NewAlgorithm::calculatePseudoTreeRelation(Path& parent, Path& child) {
//    size_t parentPathIterator = parent.
//}

void NewAlgorithm::updatePseudoTree(size_t parentIndex, Path* parentPath, Path* newPath) {
    newPath->setParentPathIndex(parentIndex);
    const auto& parentArcs = parentPath->getArcs();
    const auto& pathArcs = newPath->getArcs();

    CostType deviationReducedCosts = newPath->getTotalCosts();
    size_t deviationArcIndex = 0;
    size_t sourceArcInParentPath = parentIndex == 0 ? 0 : 1;
    //No need for range check! They must deviate from each other. Otherwise the path returned by the biobjective search is wrong.
    while (parentArcs[sourceArcInParentPath + deviationArcIndex] == pathArcs[deviationArcIndex]) {
            deviationReducedCosts -= this->currentCosts[pathArcs[deviationArcIndex]].c;
            ++deviationArcIndex;
    }
    auto firstSuffixArcIterator = pathArcs.begin() + deviationArcIndex;
    std::vector<ArcId> newPathDeviation(firstSuffixArcIterator, pathArcs.end());
    parentPath->forbidArc(*firstSuffixArcIterator);
    newPath->setArcs(newPathDeviation);
    CostType costCorrection = 0;
    for (size_t parentArcIndex = deviationArcIndex + sourceArcInParentPath; parentArcIndex < parentArcs.size(); ++parentArcIndex) {
        costCorrection += this->currentCosts[parentArcs[parentArcIndex]].c;
    }
    newPath->setTotalCosts(deviationReducedCosts + parentPath->getTotalCosts() - costCorrection);
}

void NewAlgorithm::printPaths() const {
    size_t sol = 0;
    for (const Path* p : this->solutions) {
        printf("Solution: %lu %u\n\t", sol++, p->getTotalCosts());
        p->printNewAlgo(G);
    }
}

Path* NewAlgorithm::storePathPtr(PathPtr& pPtr) {
    this->pathsPool.push_back(std::move(pPtr));
    return &*this->pathsPool.back();
}

void NewAlgorithm::processSolution(PathPtr& path, Path* parentPath, size_t parentPathIndex, size_t expansions, BucketHeap& heap) {
    if (path->valid()) {
        solution.expansionsSuccessBDA += expansions;
        Path* newPath = this->storePathPtr(path);
        this->updatePseudoTree(parentPathIndex, parentPath, newPath);
        bool inserted = heap.push(newPath, this->solutions.size(), this->k);
        if (!inserted) {
            this->relevantForDeviations[parentPathIndex] = false;
        }
    }
    else {
        this->relevantForDeviations[parentPathIndex] = false;
        ++solution.unreachedBDAs;
        solution.expansionsFailedBDA += expansions;
    }
}

void NewAlgorithm::secondShortestPathSearch(
        Path* shortestPath, size_t shortestPathIndex,
        BiobjectiveSearch& secondShortestSearch, CostType upperBound,
        BucketHeap& heap) {
    if (this->relevantForDeviations[shortestPathIndex]) {
        this->currentCosts.setActivePath(*shortestPath);
        secondShortestSearch.reset(*shortestPath);
        PathPtr newCandidatePath2Ptr = secondShortestSearch.run(*shortestPath,
                                                                *this->suffixes[shortestPathIndex],
                                                                shortestPathIndex, upperBound);
        ++solution.bdaSearches;
        this->processSolution(newCandidatePath2Ptr, shortestPath, shortestPathIndex, secondShortestSearch.expansions, heap);
    }
}

void NewAlgorithm::run() {
    std::clock_t c_start = std::clock();
    Pool<BiobjectiveLabel> labelsPool;
    Dijkstra spp(G, this->G.target, this->G.source, false, true);
    PathPtr dijkstraSolution = spp.run();
    Path* shortestPath = this->storePathPtr(dijkstraSolution);
    shortestPath->setPrefixReducedCost(0);
    this->solutions.push_back(shortestPath);
    BucketHeap heap(shortestPath->getTotalCosts());
    this->currentCosts.setReducedCosts(spp);
    this->currentCosts.setActivePath(*shortestPath);
    this->suffixes.push_back(std::move(this->generateSuffixes(shortestPath)));

    BiobjectiveSearch secondShortestSearch(this->G, this->currentCosts, *shortestPath, labelsPool);
    PathPtr ssp = secondShortestSearch.run(*shortestPath, *this->suffixes[0], 0);
    ++solution.bdaSearches;
    if (!ssp->valid()) {
        ++solution.unreachedBDAs;
        solution.expansionsFailedBDA += secondShortestSearch.expansions;
    }
    else {
        solution.expansionsSuccessBDA += secondShortestSearch.expansions;
    }
    Path* secondShortestPath = this->storePathPtr(ssp);
    this->updatePseudoTree(0, shortestPath, secondShortestPath);

    heap.push(secondShortestPath, this->solutions.size(), this->k);
    CostType upperBound = MAX_COST;
    while (this->solutions.size() < this->k) {
        if (heap.empty()) {
            break;
        }
        if (this->solutions.size() > k/2) {
            upperBound = heap.getLastRelevantCost(this->solutions.size(), this->k);
            //assert(upperBound != MAX_COST);
        }
        Path* lastPath = heap.extractMin();
        this->solutions.push_back(lastPath);
        this->suffixes.push_back(std::move(this->generateSuffixes(lastPath)));
        //printf("c= %u\n", lastPath.getTotalCosts());
        if (lastPath->firstNode(this->G) != this->G.source && lastPath->getPrefix().empty()) {
             this->buildPrefix(lastPath);
        }
        if (this->solutions.size() == this->k) {
            break;
        }
        if (heap.currentBucketSize() + this->solutions.size() >= this->k) {
            continue;
        }
        //////////////////////START PROCESSING EXTRACTED PATH////////////////////////////////
        if (lastPath->getArcs().size() > 1) {
            this->secondShortestPathSearch(lastPath, this->solutions.size() - 1, secondShortestSearch, upperBound, heap);
        }
        //////////////////////START PROCESSING PARENT OF EXTRACTED PATH////////////////////////////////
        this->secondShortestPathSearch(this->solutions[lastPath->getParentPathIndex()], lastPath->getParentPathIndex(), secondShortestSearch, upperBound, heap);
    }
    std::clock_t c_end = std::clock();
//    this->printPaths();
//    std::vector<size_t> children(2*this->k, 0);
//    for (size_t i = 1; i < solutions.size(); ++i) {
//        size_t parentIndex = this->solutions[i].parentPathIndex;
//        children[parentIndex]++;
//    }
//    for (size_t childrenCount : children) {
//        if (childrenCount > 0)
//            printf("Children: %lu \n", childrenCount);
//    }
    solution.minCost = this->solutions.front()->cost;
    solution.maxCost = this->solutions.back()->cost;
    solution.solutions = this->solutions.size();
    solution.duration = (1000 * (c_end - c_start) / CLOCKS_PER_SEC) / 1000.;
    printf("NA;%s;%u;%u;%lu;%s;%lu\n", this->G.name.c_str(), this->G.source, this->G.target, this->k, solution.print().c_str(), labelsPool.size());
    //printf("Labels pool has %lu labels!\n", labelsPool.size());
}

void NewAlgorithm::buildPrefix(Path* p) {
    const Path* parentPath = this->solutions[p->getParentPathIndex()];
    //assert(parentPath.firstNode(G) == this->G.source);
    Node lastPrefixNode = p->firstNode(G);
    CostType prefixCost = parentPath->getPrefixReducedCost();
    std::vector<ArcId> result = parentPath->getPrefix();
    for (ArcId aId : parentPath->getArcs()) {
        result.push_back(aId);
        prefixCost += this->currentCosts[aId].c;
        const Arc& a = G.arc(aId);
        if (a.head == lastPrefixNode) {
            break;
        }
    }
    p->setPrefixReducedCost(prefixCost);
    p->setPrefix(result);
}
