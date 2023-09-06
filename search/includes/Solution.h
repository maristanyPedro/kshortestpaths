#ifndef SOLUTION_H_
#define SOLUTION_H_

#include <string>
#include <sstream>

#include "../../typedefs.h"

struct Solution{

    long double duration;
    CostType minCost{0};
    CostType maxCost{MAX_COST};
    std::size_t solutions{0};
    std::size_t bdaSearches{0};
    std::size_t unreachedBDAs{0};
    std::size_t expansionsSuccessBDA{0};
    std::size_t expansionsFailedBDA{0};

    [[nodiscard]] std::string print() const {
        std::stringstream output;
        size_t succeededBDA = bdaSearches - unreachedBDAs;
        double averageIterationsInSucceededBDAs = expansionsSuccessBDA/succeededBDA;
        double averageIterationsInFailedBDAs = unreachedBDAs > 0 ? expansionsFailedBDA/unreachedBDAs : 0;
        output << duration << ";" << minCost << ";" << maxCost << ";" << solutions << ";" << bdaSearches << ";" << unreachedBDAs << ";" << averageIterationsInSucceededBDAs << ";" << averageIterationsInFailedBDAs;
        return output.str();
    }
};

#endif
