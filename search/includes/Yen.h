#ifndef YEN_H_
#define YEN_H_

#include <cstddef>
#include <vector>

#include "../../graph/includes/Path.h"

class Graph;

class Yen {
public:
    Yen(const Graph& G, size_t k);
    void run();

    void printPaths() const;

private:
    Path& storeSolution(const Path& p);
    void addSuffix(const Path& parentPath, Path& newPath, size_t deviationArcIndexInParent);
private:
    const Graph& G;
    const size_t k;

    std::vector<Path> solutions;
public:
    [[nodiscard]] const std::vector<Path> &getSolutions() const {
        return solutions;
    }
};
#endif