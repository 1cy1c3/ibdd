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

/**
 * This method marks the starting point of this application where individual
 * operations of this application can be demonstrated.
 *
 * @return Status of processing
 */
int main()
{
    /*
     * Create variables and load UT as well as CT
     * It applies the following order: 4 < 3 < 2 < 1
     */
    Manager manager(4, 521, 521);
    BDDNode a( manager.createVariable(1) );
    BDDNode b( manager.createVariable(2) );
    BDDNode c( manager.createVariable(3) );
    BDDNode d( manager.createVariable(4) );
    // Create BDD by combinations (synthesis)
    BDDNode g = (a * b) ^ (!c | d);
    // Compute the high child of the first variable
    BDDNode h = g.getCofactor( 1, BDDNode::getHighFactor() );
    // Quantify variable 3 existentially
    BDDNode f = (g ^ h).exist(3);
    /**
     * Show information to the BDD
     * Compiling with DEBUG also displays
     * information about references
     */
    std::cout << f;
    // Visualize the BDD
    std::ofstream file("f.dot");
    manager.printNode(f, "f", file);
    system("dot -o f.png -T png f.dot");
    // Perform a manual garbage collection
    manager.clear();
    return 0;
}
