#include <cassert>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <utility>
#include <algorithm>
#include <set>

#include "../includes/graph.h"

using namespace std;

unique_ptr<Graph> setupGraph(const string& filename, Node sourceId, Node targetId) {
    assert (sourceId != INVALID_NODE && targetId != INVALID_NODE);
//    std::vector<Node> nodeFilter = {0,1,2,3,100,101,102,103,200,201,202,300,301,302, 203, 303};
//    std::set<Node> nodeFilterSet(nodeFilter.begin(), nodeFilter.end());
    ifstream infile(filename);
    string line;
    size_t nodesCount =0, arcsCount = 0;
    bool shiftIndices = false;
    //Parse file until information about number of nodes and number of arcs is reached.
    while (getline(infile, line)) {
        vector<string> splittedLine{split(line, ' ')};
        if (splittedLine[0] == "p") {
            nodesCount = stoi(splittedLine[2]);
            arcsCount = stoi(splittedLine[3]);
            if (splittedLine[1] == "ksppAntonio") {
                shiftIndices = true;
            }
            break;
        }
    }
    if (nodesCount == 0 || arcsCount == 0) {
        printf("Could not determine the size of the graph %s. Abort.\n", filename.c_str());
        exit(1);
    }
 //   arcsCount = 48;
    std::vector<NeighborhoodSize> inDegree(nodesCount, 0);
    std::vector<NeighborhoodSize> outDegree(nodesCount, 0);
    size_t addedArcs = 0;
    std::vector<Arc> arcs(arcsCount);
    while (getline(infile, line)) {
        vector<string> splittedLine{split(line, ' ')};
        if (splittedLine[0] == "a") {
            assert(splittedLine.size() >= 4);
            Node tailId;
            std::stringstream(splittedLine[1]) >> tailId;
            Node headId;
            std::stringstream(splittedLine[2]) >> headId;
            if (shiftIndices) {
                tailId--;
                headId--;
            }
//            if (nodeFilterSet.find(tailId) == nodeFilterSet.end() || nodeFilterSet.find(headId) == nodeFilterSet.end()) {
//                continue;
//            }
            ++outDegree[tailId];
            assert(outDegree[tailId] != MAX_DEGREE);
            ++inDegree[headId];
            assert(inDegree[headId] != MAX_DEGREE);
            arcs[addedArcs] = Arc(tailId, headId, stoi(splittedLine[3]));
            //printf("%u --> %u with c = %u\n", tailId, headId, stoi(splittedLine[3]));
            ++addedArcs;
        }
    }
//    printf("Warning!!! Graph building manipulated for debug!!\n");
    arcsCount = addedArcs;
    //assert(addedArcs == arcsCount);
    std::sort(arcs.begin(), arcs.end(), ArcSorter());
    unique_ptr<Graph> G = make_unique<Graph>(split(filename, '/').back(), nodesCount, arcsCount, arcs,sourceId, targetId);
    for (Node i = 0; i < nodesCount; ++i) {
        G->setNodeInfo(i, inDegree[i], outDegree[i]);
    }
    vector<NeighborhoodSize> incomingArcsPerNode(nodesCount, 0);
    vector<NeighborhoodSize> outgoingArcsPerNode(nodesCount, 0);
    for (size_t i = 0; i < G->arcsCount; ++i) {
        const Arc& arc = G->arc(i);
        NodeAdjacency& tail = G->node(arc.tail);
        NodeAdjacency& head = G->node(arc.head);
        tail.id = arc.tail;
        head.id = arc.head;
        tail.outgoingArcs[outgoingArcsPerNode[tail.id]++] = i;
        head.incomingArcs[incomingArcsPerNode[head.id]++] = i;
    }
    for (Node i = 0; i < nodesCount; ++i) {
        assert(inDegree[i] == incomingArcsPerNode[i]);
        assert(outDegree[i] == outgoingArcsPerNode[i]);
    }
    //printf("Graph with %lu nodes and %lu arcs!\n", G->nodesCount, G->arcsCount);
    return G;
}

void Graph::setNodeInfo(Node n, NeighborhoodSize inDegree, NeighborhoodSize outDegree) {
    NodeAdjacency& currentNode = this->nodes[n];
    currentNode.id = n;
    currentNode.incomingArcs.resize(inDegree);
    currentNode.outgoingArcs.resize(outDegree);
}

CostType Graph::calculatePathCosts(const std::vector<Node>& pathNodes) const {
    CostType c = 0;
    for (size_t i = 0; i < pathNodes.size()-1; ++i) {
        const auto& outgoingArcs = this->outgoingArcs(pathNodes[i]);
        for (ArcId arcId : outgoingArcs) {
            const Arc& arc = this->arc(arcId);
            if (arc.head == pathNodes[i+1]) {
                c += arc.c;
                break;
            }
        }
    }
    return c;
}

void Graph::printNodeInfo(const Node nodeId) const {
    printf("Analyzing node: %u\n", nodeId);
    printf("OUTGOING ARCS\n");
    printArcs(this->outgoingArcs(nodeId));
    printf("INCOMING ARCS\n");
    printArcs(this->incomingArcs(nodeId));
}

void Graph::printArcs(const Neighborhood& neighArcs) const {
    for (ArcId aId : neighArcs) {
        this->arcs[aId].print();
    }
}

Arc::Arc(Node tailId, Node headId, CostType c1):
        tail{tailId}, head{headId}, c{c1} {}

void Arc::print() const {
    printf("Arc costs: %d\n", c);
}

Graph::Graph(std::string name, Node nodesCount, ArcId arcsCount, std::vector<Arc>& arcs, Node source, Node target):
        name{std::move(name)},
        nodesCount{nodesCount},
        arcsCount{arcsCount},
        arcs{std::move(arcs)},
        source{source},
        target{target},
        nodes(nodesCount) {
    //We need that many entries in our ncl-vectors in BDA.
    //In case this assertion fails, just change the ArcId typedef in typedefs.h
    assert(INVALID_ARC >= 2*arcsCount);
}

void split(const string& s, char delim, vector<string>& elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

vector<string> split(const string& s, char delim) {
    vector<string> elems;
    if (delim == ' ') {
        istringstream iss(s);
        elems = {istream_iterator<string>{iss}, istream_iterator<string>{}};
    } else {
        split(s, delim, elems);
    }

    for (auto& elem : elems) {
        if (elem.back() == '\n') {
            elem = elem.substr(0, elem.size() - 1);
        }
    }
    return elems;
}

NodeAdjacency::NodeAdjacency(): id{INVALID_NODE} {}

NodeAdjacency::NodeAdjacency(const Node nid, size_t incomingArcsCt, size_t outgoingArcsCt):
        incomingArcs(incomingArcsCt),
        outgoingArcs(outgoingArcsCt),
        id{nid} {

    incomingArcs.shrink_to_fit();
    outgoingArcs.shrink_to_fit();
}
