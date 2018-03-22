/**
 * @file main.cpp
 * @author Rune Krauss
 *
 * Binary decision diagrams (BDDs) indicate an efficient representation of
 * Boolean functions and are an example of multi-stage circuits as well as
 * a data structure for logic synthesis and verification.
 * In contrast to e. g. normal forms, they can also be used in practice for many inputs.
 * In this context, it is particularly important to optimize the storage space since this
 * can already decide whether a computing is completed or not.
 * Formally, BDDs define an acyclic graph which is generated using variables
 * and at most one low and one high child. Leaves indicate the constants.
 * This package typically has a unique and computed table that are dynamic.
 * Furthermore, complement edges, a standardization and dynamic garbage collection are supported.
 * There are also some useful operations such as ITE or existential quantification.
 */
#include <iostream>
#include <fstream>
#include "Manager.hpp"
#include "BDDParser.hpp"
#include <sys/times.h>
#include <unistd.h>

/**
 * This method marks the starting point of this application where the performance of the
 * packet can be measured by means of different circuits.
 *
 * @param argc Number of arguments
 * @param argv Trace file
 * @return Status of processing
 */
int main(int argc, char **argv)
{
    struct tms start, stop;
    std::cout << "AGRABDD Package: Version 1.0 (March 18, 2018)" << std::endl;
    // Not enough arguments available
    if ( argc != 2 ) {
        std::cout << "Usage: ./agrabdd <file>" << std::endl;
        return -1;
    }
    // Count the primary inputs
    int inputGatesCounter = BDDParser::countInputGates(argv[1]);
    if (inputGatesCounter == -1)
        return -1;
    // Start by measuring time
    times(&start);
    // Create manager and vector for nodes
    Manager manager(inputGatesCounter, 500009, 500009);
    std::vector<BDDNode> bddNodes;
    BDDNode bddNode;
    // Create as many BDDs as there are primary inputs
    for (int i = 1; i <= inputGatesCounter; i++) {
        bddNode = manager.createVariable(i);
        bddNodes.push_back(bddNode);
    }
    // Iterate through the trace file and also create BDDs according to tags/operations
    int status = BDDParser::parseGates(argv[1], bddNodes);
    // For example. the file could not be opened
    if (status == -1)
        return -1;
    // Determine the number of primary outputs
    static std::vector<std::string> outputGates = BDDParser::getOutputGates();
    // Create 1-terminals for the primary outputs
    for (int i = 0; i < outputGates.size(); i++)
        BDDNode bddNode(DDNode::getLeaf(), BDDNode::getRegularEdge());
    // Stop and compare the times
    times(&stop);
    double compareTimes = (double) (stop.tms_utime - start.tms_utime + stop.tms_stime - start.tms_stime) / sysconf(_SC_CLK_TCK);
    std::cout << "Number of inputs: " << inputGatesCounter << std::endl;
    manager.showInfo(compareTimes, bddNodes);
    bddNodes.clear();
    outputGates.clear();
    manager.clear();
    return 0;
}

