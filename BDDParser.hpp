/**
 * @file BDDParser.hpp
 * @author Rune Krauss
 *
 * @brief This parser reads circuits in trace format and creates a BDD from them. It can be used,
 * for example, to read in more complex circuits such as "c6288" from ISCAS85 and to measure
 * the corresponding performance of this package.
 */
#ifndef BDDParser_hpp
#define BDDParser_hpp

#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include "BDDNode.hpp"

/**
 * This class implements a parser for trace files that can be analyzed in this respect. It is
 * responsible in particular for splitting and converting the trace files into BDDs. Furthermore,
 * the system displays how much memory was required or how long the process took.
 */
class BDDParser
{
private:
    /**
     * Line length for the traces
     */
    static int const LINE_LENGTH = 1024;
    
    /**
     * Counts the respective gates.
     */
    static int gateCounter;
    
    /**
     * Mapping the tags to a unique ID.
     */
    static std::map<std::string, int> gates;
    
    /**
     * Contains the primary outputs.
     */
    static std::vector<std::string> outputGates;
    
    /**
     * Represents the supported logical operators.
     */
    static std::vector<std::string> logicalOperators;
    
    /**
     * @brief The constructor is private, i. e. no parser object can be created
     * which allows direct calls to the operations.
     */
    BDDParser();
    
    /**
     * @brief Processes the respective gate and increments the gate counter.
     */
    static void countInputGate(char*, int&);
    
    /**
     * @brief Checks if it is a gate.
     */
    static bool isGate(char*, char);
    
    /**
     * @brief Converts primary inputs to a map.
     */
    static void convertInputGatesToMap(char*);
    
    /**
     * @brief Converts primary outputs to a map.
     */
    static void convertOutputGatesToMap(char*, std::vector<BDDNode>&);
    
    /**
     * @brief Reads the respective gate in the circuit and generates the corresponding BDD.
     */
    static void parseGate(char*, std::vector<BDDNode>&);
    
    /**
     * @brief Searches a line for a logical operator and returns it.
     */
    static std::pair<size_t, std::string> findGate(std::string&);
public:
    /**
     * @brief Counts how many inputs there are in the respective circuit.
     */
    static int countInputGates(char*);
    
    /**
     * @brief Reads the listed gates and generates the respective BDDs.
     */
    static int parseGates(char*, std::vector<BDDNode>&);
    
    static std::vector<std::string> getOutputGates();
    
    static std::map<std::string, int> getGates();
    
    static int getGateCounter();
};
#endif
