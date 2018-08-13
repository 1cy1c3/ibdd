iBDD
============

# Description
A Binary Decision Diagram (BDD) is a data structure that is used to represent a Boolean function for logic synthesis and verification. Formally expressed, a BDD is a rooted, directed, acyclic graph, which consists of several decision and terminal nodes. Unlike other compressed representations, operations like conjunction, disjunction, negation (with the help of complement edges) or existential abstraction are performed directly on the compressed representation. This package based on an efficient implementation of the if-then-else (ITE) operator respectively reduced, ordered BDD (ROBDD) for manipulating Boolean functions. A hash table is used to ensure a canonical form in the ROBDD and nodes are merged with this table. Furthermore, a hash-based cache decreases the memory usage and a standardization of ITE calls increases the performance of the synthesis. Finally, there is an automatic garbage collecton for recycling memory.

## Prerequisites
+ C++14 or later
+ Doxygen for reading the documentation
+ DOT (graph description language) for visualization of the BDDs

## Installation
At first, clone or download this project. Afterwards, go to the terminal and type `make` to compile and link this application. Finally, type `./ibdd_test` to test the quality of this application.

## Usage
Catch2 is used for the unit tests. Catch2 is a multi-paradigm test framework for C++. To define single tests, the BDD-style is used. This became such a useful way of working that first class support has been added to Catch. Scenarios are specified using SCENARIO, GIVEN, WHEN and THEN macros, which map on to TEST_CASEs and SECTIONs, respectively. For more information about this, look at the file `ibddTest.cpp`.

**Note**: To see more detailed output compared to `installation`, just type `./ibdd_test` with parameters like `-r xml`.

## More information
Generate the documentation regarding the special comments with a command in your terminal, for example:

```
$ cd ibdd
$ doxygen doxygen.config
```

Afterwards, you will get a website with helpful information about the code. Furthermore, read the more detailed [documentation](https://runekrauss.com/files/papers/ibdd.pdf) about this application.
