#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <chrono>
#include "valgrind/callgrind.h"

#include "./graph/includes/graph.h"
#include "./graph/includes/Path.h"
#include "./search/includes/Dijkstra.h"
//#include "./search/includes/Yen.h"
#include "./search/includes/NewAlgorithm.h"

using namespace std;

int main(int argc, char *argv[]) {

    Node sourceId = INVALID_NODE;
    Node targetId = INVALID_NODE;
    size_t solutionsCount = numeric_limits<size_t>::max();
    stringstream(argv[2]) >> sourceId;
    stringstream(argv[3]) >> targetId;
    stringstream(argv[4]) >> solutionsCount;

    unique_ptr<Graph> G_ptr = setupGraph(argv[1], sourceId, targetId);
    Graph& G = *G_ptr;

//    printf("Graph %s with %u nodes and %u arcs. Instance from %u to %u\n",
//           G.name.c_str(), G.nodesCount, G.arcsCount, G.source, G.target);

    if (G.nodesCount < sourceId || G.nodesCount < targetId) {
        throw;
    }

//        Yen yen(G, solutionsCount);
//        //yen.run();

        NewAlgorithm na(G, solutionsCount);
        CALLGRIND_START_INSTRUMENTATION;
        na.run();
        CALLGRIND_STOP_INSTRUMENTATION;
        CALLGRIND_DUMP_STATS;





    //yen.printPaths();




//    const auto& yenSolutions = yen.getSolutions();
//    const auto& naSolutions = na.getSolutions();
//    bool equal = true;
//    for (size_t i = 0; i < naSolutions.size(); ++i) {
//        if (yenSolutions[i].getTotalCosts() != naSolutions[i].getTotalCosts()) {
//            cout << "\033[1;31mError at index \033[0m" << i << endl;
//            equal = false;
//            break;
//        }
//    }
//    if (equal) {
//        cout << "\033[1;32mCongratulations, both algorithms coincide!\033[0m\n";
//    }

    return 0;
}
