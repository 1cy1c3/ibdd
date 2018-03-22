iBDD
============

# Description
A Binary Decision Diagram (BDD) is a data structure that is used to represent a Boolean function for logic synthesis and verification. Formally expressed, a BDD is a rooted, directed, acyclic graph, which consists of several decision and terminal nodes. Unlike other compressed representations, operations like conjunction, disjunction, negation (with the help of complement edges) or existential abstraction are performed directly on the compressed representation. This package based on an efficient implementation of the if-then-else (ITE) operator respectively reduced, ordered BDD (ROBDD) for manipulating Boolean functions. A hash table is used to ensure a canonical form in the ROBDD and nodes are merged with this table. Furthermore, a hash-based cache decreases the memory usage and a standardization of ITE calls increases the performance of the synthesis. Finally, there is an automatic garbage collecton for recycling memory.

## Prerequisites
+ C++14 or later
+ Doxygen for reading the documentation
+ DOT (graph description language) for visualization of the BDDs

## Installation
At first, clone or download this project. Afterwards, go to the terminal and type `make` to compile and link this application. Finally, type `./ibdd_benchmark` to test the power of this application.

## Usage
To build BDDs from circuits, there is a folder named *trace* with different circuit descriptions. In this context, just type the following command in your terminal to read in a circuit:

```
$ ./ibdd_benchmark trace/c17.trace
AGRABDD Package: Version 1.0 (March 18, 2018)
Number of inputs: 5
...
Time in seconds: 0.00
```

Afterwards, you will see different information like number of inputs, memory usage and so on regarding the used circuit. The circuits have the following meaning:

* C17: Freely invented circuit
* C432: 27-channel interrupt controller
* C499/C1355: 32-bit SEC circuit
* C880: 8-Bit ALU
* C1908: 16-bit SEC/DED circuit
* C2670: 12-bit ALU and controller
* C3540: 8-bit ALU with BCD arithmetic
* C5315: 9-bit ALU with parity computing
* C6288-X: X-bit multiplier

## More information
Generate the documentation regarding the special comments with a command in your terminal, for example:

```
$ cd ibdd
$ doxygen doxygen.config
```

Afterwards, you will get a website with helpful information about the code. Furthermore, read the more detailed [documentation](https://runekrauss.com/pdf/ibdd.pdf) about this application.
