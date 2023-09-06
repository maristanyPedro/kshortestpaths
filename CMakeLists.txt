cmake_minimum_required(VERSION 3.18)
project(kspp_new_version)

set(CMAKE_CXX_STANDARD 17)

add_executable(kspp_new_version main.cpp
        graph/src/graph.cpp graph/src/SubGraph.cpp graph/src/Path.cpp
        search/src/Dijkstra.cpp
        #search/src/Yen.cpp
        search/src/BiobjectiveCosts.cpp search/src/BiobjectiveSearch.cpp
        search/src/NewAlgorithm.cpp)
