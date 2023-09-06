#ifndef BUCKET_HEAP_H_
#define BUCKET_HEAP_H_

#include <vector>
#include <list>
#include <cassert>

#include "../../typedefs.h"

#include "../../graph/includes/Path.h"

typedef std::list<Path*> Bucket;

class BucketHeap {
public:
    explicit BucketHeap(CostType minCost):
        buckets(100), size{0}, minCost{minCost}, currentIndex{0} {}

    Path* extractMin() {
        if (this->buckets[currentIndex].empty()) {
            this->advanceCurrentIndex();
        }
        Bucket& minBucket = this->buckets[currentIndex];
        Path* p = minBucket.front();
        minBucket.pop_front();
        --size;
        return p;
    }

    bool push(Path* p, size_t sols, size_t k) {
        Bucket& bucket = this->calculateBucket(p->getTotalCosts());
        bool insertionRequired = true;
        if (sols + this->size >= k) {
            insertionRequired = this->evaluateInsertion(p->getTotalCosts(), sols, k);
        }
        if (insertionRequired) {
            bucket.push_back(p);
            ++size;
        }
        return insertionRequired;
    }

    [[nodiscard]] bool evaluateInsertion(CostType newC, size_t existingSolutions, size_t requiredSolutions) const {
        size_t index = newC - minCost;
        size_t pathsBetween = 0;
        for (size_t i = currentIndex; i <= index; ++i) {
            pathsBetween += this->buckets[i].size();
        }
        if (pathsBetween + existingSolutions >= requiredSolutions) {
            return false;
        }
        return true;
    }

    CostType getLastRelevantCost(size_t existingSolutions, size_t neededSolutions) {
        CostType bound = MAX_COST;
        size_t solutionsCollector = existingSolutions;
        for (size_t i = currentIndex; i < this->buckets.size(); ++i) {
            solutionsCollector += this->buckets[i].size();
            if (solutionsCollector >= neededSolutions) {
                assert(this->minCost + i == this->buckets[i].front()->getTotalCosts());
                bound = this->minCost + i;
                break;
            }
        }
        return bound;
    }

    [[nodiscard]] bool empty() const {
        return this->size == 0;
    }

    [[nodiscard]] size_t currentBucketSize() const {
        return this->buckets[currentIndex].size();
    }

private:
    std::vector<Bucket> buckets;
    std::size_t size;
    CostType minCost;
    CostType currentIndex;

    Bucket& calculateBucket(CostType c) {
        size_t index = c - minCost;
        if (index >= this->buckets.size()) {
            this->buckets.resize(index + 100);
        }
        return this->buckets[index];
    }

    void advanceCurrentIndex() {
        while(this->buckets[currentIndex].empty()) {
            currentIndex++;
        }
    }
};

#endif