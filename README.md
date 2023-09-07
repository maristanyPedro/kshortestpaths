# kshortestpaths
C++ implementation of a k-shortest simple path algorithm using biobjective path search as a subroutine.

## Platform

The code has been tested on modern Unix systems and on Mac OS. You need a CMake version not older than v3.18 to generate a Makefile to compile the code.

## Compiling the code

In the root directory of the project, create a new directory called 'cmake-build' and navigate to the folder. Once you are in this directory, type
```
cmake .. -DCMAKE_BUILD_TYPE=Release
```
to generate a Makefile. Then, run
```
make -j
```
to generate the program 'kspp_new_version' that will be stored in your current directory.

## Running the executable

Once you have compiled the code, you can run it using the sample instance stored as 'instances/miniGrid.gr'. To run the code, stay in the 'cmake-build' directory and run, for example,
```
./kspp_new_version ../instances/miniGrid.gr 0 4000 100
```
Here, the first argument is the name of the directed graph, the second argument is the ID of the chosen source node, the third argument is the ID of the chosen target node, and the fourth number is the value chosen for K. Note that the node IDs are expected to be defined between 0 and n-1 if n is the number of nodes. The format of the graph files is self-explanatory. In our example, a line defining an arc looks as follows:

```
a tailId headId c1 c2
```
The cost assigned to the corresponding arc in the k-ssp instance is c1. c2 is ignored!

## Reading the output

If the problem is solved successfully, the program outputs a line like this:

```
NA;miniGrid.gr;0;4000;100;0.001;189;192;100;151;13;19;5;200
```
The corresponding names of the columns are

```
'ALGO', 'GRAPH', 'SOURCE', 'TARGET', 'K', 'TIME', 'COST_MIN', 'COST_MAX', 'SOLUTIONS', 'BDA_QUERIES', 'FAILED_BDA', 'EXPANSIONS_PER_BDA', 'EXPANSIONS_PER_FAILED_BDA', 'POOL_SIZE'
```

Further explanations on the meaning of these columns can be found in the paper.
