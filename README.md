iBDD
============

# Description
A Binary Decision Diagram (BDD) is a data structure that is used to represent a Boolean function for logic synthesis and verification. Formally expressed, a BDD is a rooted, directed, acyclic graph, which consists of several decision and terminal nodes. Unlike other compressed representations, operations like conjunction, disjunction, negation (with the help of complement edges) or existential abstraction are performed directly on the compressed representation. This package based on an efficient implementation of the if-then-else (ITE) operator respectively reduced, ordered BDD (ROBDD) for manipulating Boolean functions. A hash table is used to ensure a canonical form in the ROBDD and nodes are merged with this table. Furthermore, a hash-based cache decreases the memory usage and a standardization of ITE calls increases the performance of the synthesis. Finally, there is an automatic garbage collecton for recycling memory.

## Prerequisites
+ C++14 or later
+ Doxygen for reading the documentation
+ DOT (graph description language) for visualization of the BDDs

## Installation
At first, clone or download this project. Afterwards, go to the terminal and type `make` to compile and link this application. Finally, type `./ibdd` to test an example.

**Note**: There are also unit tests and benchmarks. To checkout the unit tests, type `git checkout test` in your terminal. To get the benchmarks, type `git checkout benchmark`. For more information, see their *README*.

## Usage
At first, include and initialize the manager with the commands `include "manager.hpp"` and `Manager manager(4, 521, 521)`. The first parameter stands for the supported variables and the next parameters for the sizes regarding the hash table and cache. It is recommended to use prime numbers because of using a modulo process for the generation of keys. For creating  single nodes, use the command `BDDNode a( manager.createVariable(1) )`. In this context, there are many overloaded operators which deal with the manipulation of Boolean functions, e. g. `BDDNode g = !a` stands for a negation. For more information, look at the class `BDDNode`. For getting information about nodes, use the output operator `std::cout << a;` and to visualize nodes, use the command `manager.printNode(a, "a", file)`. Finally, the command `manager.clear()` executes a manual garbage collection.

## More information
Generate the documentation regarding the special comments with a command in your terminal, for example:

```
$ cd ibdd
$ doxygen doxygen.config
```

Afterwards, you will get a website with helpful information about the code. Furthermore, read the more detailed [documentation](https://runekrauss.com/files/papers/ibdd.pdf) about this application.
