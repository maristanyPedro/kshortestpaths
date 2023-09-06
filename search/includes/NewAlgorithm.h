#ifndef NEW_ALGORITHM_H_
#define NEW_ALGORITHM_H_

#include <cstddef>
#include <unordered_set>
#include <vector>

#include "../../graph/includes/Path.h"
#include "./Solution.h"

#include "./BiobjectiveCosts.h"

class Graph;
class BiobjectiveCosts;
class BucketHeap;
class BiobjectiveSearch;

typedef std::unique_ptr<Path> PathPtr;

class NewAlgorithm{
public:
    NewAlgorithm(const Graph& G, size_t k);
    void run();

    void printPaths() const;

private:
    void secondShortestPathSearch(
            Path* shortestPath, size_t shortestPathIndex,
            BiobjectiveSearch& secondShortestSearch, CostType upperBound,
            BucketHeap& heap);
    Path* storePathPtr(PathPtr& pPtr);
    void buildPrefix(Path* p);
    std::unique_ptr<SuffixManager> generateSuffixes(Path* p) const;
    void processSolution(PathPtr& path, Path* parentPath, size_t parentPathIndex, size_t expansions, BucketHeap& heap);

    void updatePseudoTree(size_t parentIndex, Path* parentPath, Path* newPath);
private:
    const Graph& G;
    BiobjectiveCostsVector currentCosts;
    Solution solution;
    const size_t k;

    std::vector<std::unique_ptr<Path>> pathsPool;
    std::vector<Path*> solutions;
    std::vector<bool> relevantForDeviations;
    std::vector<std::unique_ptr<SuffixManager>> suffixes;
public:
    [[nodiscard]] const std::vector<Path*> &getSolutions() const {
        return solutions;
    }
};

#endif