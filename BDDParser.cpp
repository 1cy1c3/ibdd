/**
 * @file BDDParser.cpp
 * @author Rune Krauss
 *
 * The parser has the ability to parse trace files which trigger a lot of calls
 * in the ROBDD package. The trace files themselves describe circuits that generate
 * BDDs whereby the following format applies (example for the test circuit "c17" of ISCAS85):
 *
 * MODULE c17
 * INPUT
 *   1gat,2gat,3gat,6gat,7gat;
 * OUTPUT
 *   22gat,23gat;
 * STRUCTURE
 *   10gat = nand(1gat, 3gat);
 *   11gat = nand(3gat, 6gat);
 *   16gat = nand(2gat, 11gat);
 *   19gat = nand(11gat, 7gat);
 *   22gat = nand(10gat, 16gat);
 *   23gat = nand(16gat, 19gat);
 * ENDMODULE
 *
 * The circuit has an identifier as well as primary inputs and outputs which are described at
 * the beginning. This is followed by a description of how the respective components are connected
 * to each other.
 */
#include "BDDParser.hpp"

int BDDParser::gateCounter;
std::map<std::string, int> BDDParser::gates;
std::vector<std::string> BDDParser::outputGates;
std::vector<std::string> BDDParser::logicalOperators = {"not", "xor", "nand", "nor", "and", "or"};

/**
 * This checks whether it is a valid gate which is exactly the case if there is a comma
 * since the gate follows.
 *
 * @param line Line in the trace file
 * @param token Separator for the gates
 * @return Status whether it is a gate
 */
bool BDDParser::isGate(char* line, char token)
{
    int i;
    size_t len = strlen(line);
    for (i = 0; i < len; i++) {
        if (line[i] == token)
            return true;
    }
    return false;
}

/**
 * This increases the gate counter, i. e. the names for the gates are passed through commas
 * until a semicolon is present since then all gates have been found. For each gate found,
 * the counter is increased by exactly 1.
 *
 * @param line Line between "INPUT" und "OUTPUT"
 * @param counter Counter for primary inputs
 */
void BDDParser::countInputGate(char* line, int& counter)
{
    char lineForGates[LINE_LENGTH];
    int lineForGatesLen = 0;
    size_t len = strlen(line);
    for (int i = 0; i < len; i++) {
        if (line[i] == ' ' || line[i] == '\t')
            continue;
        else if (line[i] != ',' && line[i] != ';')
            lineForGates[lineForGatesLen++] = line[i];
        else {
            lineForGates[lineForGatesLen] = '\0';
            counter++;
            lineForGatesLen = 0;
        }
    }
}

/**
 * Converts the primary inputs from the trace to a map where each gate points to a unique ID.
 *
 * @param line Line between "INPUT" und "OUTPUT"
 */
void BDDParser::convertInputGatesToMap(char* line)
{
    std::size_t lineLength = strlen(line);
    char lineForGates[LINE_LENGTH];
    int lineForGatesLen = 0;
    for (int i = 0; i < lineLength; i++) {
        if (line[i] == ' ' || line[i] == '\t')
            continue;
        else if (line[i] != ',' && line[i] != ';')
            lineForGates[lineForGatesLen++] = line[i];
        else {
            lineForGates[lineForGatesLen] = '\0';
            std::string gate(lineForGates);
            gates[gate] = ++gateCounter;
            lineForGatesLen = 0;
        }
    }
}

/**
 * Converts the primary outputs from the trace to a map where each gate points
 * to a unique ID. Furthermore, the names of the outputs are stored in a vector
 * and the BDDs for the primary outputs are generated.
 *
 * @param line Line between "OUTPUT" and "STRUCTURE"
 * @param bddNodes Vector with the respective BDD nodes
 */
void BDDParser::convertOutputGatesToMap(char* line, std::vector<BDDNode>& bddNodes)
{
    std::size_t lineLength = strlen(line);
    char lineForGates[LINE_LENGTH];
    int lineForGatesLen = 0;
    BDDNode bddNode;
    for (int i = 0; i < lineLength; i++) {
        if (line[i] == ' ' || line[i] == '\t')
            continue;
        else if (line[i] != ',' && line[i] != ';')
            lineForGates[lineForGatesLen++] = line[i];
        else {
            lineForGates[lineForGatesLen] = '\0';
            std::string gate(lineForGates);
            gates[gate] = ++gateCounter;
            outputGates.push_back(gate);
            bddNode = bddNodes[0];
            bddNodes.push_back(bddNode);
            lineForGatesLen = 0;
        }
    }
}

/**
 * Interprets the respective gates between the primary inputs as well as outputs and
 * performs the respective synthesis for each gate depending on the operation. If,
 * for example "xor" is found in a line, the names for the respective tags will be
 * extracted first. Afterwards, the corresponding BDD is determined via the ID from
 * the map for the respective tag designation and so on. Moreover, the synthesis starts.
 * If the BDD for the output of the gate does not yet exist in the vector, it will be
 * inserted there and in the map. Otherwise it will be updated.
 *
 * @param line Line between "STRUCTURE" and "ENDMODULE"
 * @param bddNodes Vector with the respective BDD nodes
 */
void BDDParser::parseGate(char* line, std::vector<BDDNode>& bddNodes)
{
    std::string gate(line);
    std::map<std::string, int>::iterator gatesIterator;
    BDDNode bddNodeInput1, bddNodeInput2, bddNodeOutput;
    char tmpGateInput1[LINE_LENGTH], tmpGateInput2[LINE_LENGTH], tmpGateOutput[LINE_LENGTH];
    std::string gateInput1, gateInput2, gateOutput;
    int type, tmpGateInput1Length;
    size_t gateLength;
    std::pair<size_t, std::string> logicalOperation = findGate(gate);
    size_t operatorPosition = logicalOperation.first;
    std::string logicalOperator = logicalOperation.second;
    // A valid operator was found in the respective line
    if (operatorPosition != -1) {
        /**
         * 1. Get the tags from the line and save them
         * 2. Use it to find out the number via the gates vector
         * 3. Assign the BDD for it
         * 4. Perform the respective synthesis with respect to the output BDD
         * 5. If the output BDD does not yet exist in the BDD vector, add it
         * 6. Afterwards, also increase the counter for the gates
         * 7. Otherwise, the respective BDD is updated
         */
        if (logicalOperator == "not") {
            sscanf(line, "%s = not(%s", tmpGateOutput, tmpGateInput1);
            tmpGateInput1[strlen(tmpGateInput1)-2] = '\0';
            gateInput1 = tmpGateInput1;
            gateOutput = tmpGateOutput;
            bddNodeInput1 = bddNodes[gates[gateInput1]];
            bddNodeOutput = !bddNodeInput1;
        } else if (logicalOperator == "xor") {
            sscanf(line, "%s = xor(%s %s", tmpGateOutput, tmpGateInput1, tmpGateInput2);
            tmpGateInput1[strlen(tmpGateInput1)-1] = '\0';
            gateInput1 = tmpGateInput1;
            tmpGateInput2[strlen(tmpGateInput2)-2] = '\0';
            gateInput2 = tmpGateInput2;
            gateOutput = tmpGateOutput;
            bddNodeInput1 = bddNodes[gates[gateInput1]];
            bddNodeInput2 = bddNodes[gates[gateInput2]];
            bddNodeOutput = bddNodeInput1 ^ bddNodeInput2;
        } else {
            strcpy(tmpGateInput2, line);
            tmpGateInput2[operatorPosition] = '\0';
            sscanf(tmpGateInput2, "%s = ", tmpGateOutput);
            gateOutput = tmpGateOutput;
            gateLength = strlen(line);
            tmpGateInput1Length = 0;
            type = 1;
            size_t distance = 0;
            if (logicalOperator == "nand")
                distance = 5;
            else if (logicalOperator == "nor")
                distance = 4;
            else if (logicalOperator == "and")
                distance = 4;
            else if (logicalOperator == "or")
                distance = 3;
            for (size_t i = operatorPosition+distance; i < gateLength; i++) {
                if (line[i] == ' ')
                    continue;
                else if (line[i] != ',' && line[i] != ')')
                    tmpGateInput1[tmpGateInput1Length++] = line[i];
                else {
                    tmpGateInput1[tmpGateInput1Length] = '\0';
                    gateInput1 = tmpGateInput1;
                    bddNodeInput1 = bddNodes[gates[gateInput1]];
                    if (type == 0) {
                        if (logicalOperator == "nand" || logicalOperator == "and")
                            bddNodeOutput = bddNodeOutput * bddNodeInput1;
                        else if (logicalOperator == "nor" || logicalOperator == "or")
                            bddNodeOutput = bddNodeOutput + bddNodeInput1;
                    } else
                        bddNodeOutput = bddNodeInput1;
                    type = 0;
                    tmpGateInput1Length = 0;
                }
            }
            if (logicalOperator == "nand" || logicalOperator == "nor")
                bddNodeOutput = !bddNodeOutput;
        }
        gatesIterator = gates.find(gateOutput);
        if (gatesIterator == gates.end()) {
            gates[gateOutput] = ++gateCounter;
            bddNodes.push_back(bddNodeOutput);
        } else
            bddNodes[gates[gateOutput]] = bddNodeOutput;
    } else if (gate.find("=") != -1) {
        // Betrifft nur den Operator '='
        sscanf(line, "%s = %s", tmpGateOutput, tmpGateInput1);
        tmpGateInput1[strlen(tmpGateInput1)-1] = '\0';
        gateInput1 = tmpGateInput1;
        gateOutput = tmpGateOutput;
        bddNodeInput1 = bddNodes[gates[gateInput1]];
        bddNodeOutput = bddNodeInput1;
        gatesIterator = gates.find(gateOutput);
        if (gatesIterator == gates.end()) {
            gates[gateOutput] = ++gateCounter;
            bddNodes.push_back(bddNodeOutput);
        } else
            bddNodes[gates[gateOutput]] = bddNodeOutput;
    }
}

/**
 * Searches a line in the trace file for a logical operator and returns its
 * position and name. If no operator is found, -1 applies.
 *
 * @param line Line in the trace file
 * @return Position and name of a logical operator in the trace file
 */
std::pair<size_t, std::string> BDDParser::findGate(std::string& line) {
    std::pair<size_t, std::string> positionOperator(-1, "");
    for (auto needle : logicalOperators) {
        size_t position = line.find(needle);
        if (position != std::string::npos) {
            positionOperator.first = position;
            positionOperator.second = needle;
            return positionOperator;
        }
    }
    return positionOperator;
}

/**
 * This function processes the trace file up to the keyword "OUTPUT" and is responsible
 * for counting the respective primary inputs required for creating variables with respect
 * to the BDD.
 *
 * @param fileName Filename (Trace)
 * @return Number of primary inputs
 */
int BDDParser::countInputGates(char* fileName)
{
    int counter = 0;
    char line[LINE_LENGTH];
    std::ifstream file(fileName, std::ios::in);
    if (!file) {
        std::cout << "The file could not be opened." << std::endl;
        return -1;
    }
    file.getline(line, LINE_LENGTH);
    for (;;) {
        file.getline(line, LINE_LENGTH);
        if (line[0] != '#')
            break;
    }
    bool gate = isGate(line, ',');
    if (gate)
        countInputGate(line+5, counter);
    for (;;) {
        file.getline(line, LINE_LENGTH);
        if (strncmp(line, "OUTPUT", 6) == 0)
            break;
        countInputGate(line, counter);
    }
    file.close();
    return counter;
}

/**
 * Here, the gates are read in whereby the primary inputs are stored in a map (Gate -> ID).
 * The primary outputs are processed by saving them to the map. In addition, they are stored
 * in a separate vector and the associated BDDs are generated which are stored in an existing
 * vector with the BDDs for the primary inputs.
 *
 * @param fileName Filename (Trace)
 * @return Number of primary inputs
 */
int BDDParser::parseGates(char* fileName, std::vector<BDDNode>& bddNodes)
{
    char line[LINE_LENGTH];
    std::ifstream file(fileName, std::ios::in);
    if (!file) {
        std::cout << "The file could not be opened." << std::endl;
        return -1;
    }
    gateCounter = -1;
    gates.clear();
    file.getline(line, LINE_LENGTH);
    for (;;) {
        file.getline(line, LINE_LENGTH);
        if (line[0] != '#')
            break;
    }
    bool gate = isGate(line, ',');
    if (gate)
        convertInputGatesToMap(line+5);
    for (;;) {
        file.getline(line, LINE_LENGTH);
        if (strncmp(line, "OUTPUT", 6) == 0)
            break;
        convertInputGatesToMap(line);
    }
    gate = isGate(line, ',');
    if (gate)
        convertOutputGatesToMap(line+6, bddNodes);
    for (;;) {
        file.getline(line, LINE_LENGTH);
        if (strcmp(line, "STRUCTURE") == 0)
            break;
        convertOutputGatesToMap(line, bddNodes);
    }
    for (;;) {
        file.getline(line, LINE_LENGTH);
        if (strcmp(line, "ENDMODULE") == 0)
            break;
        parseGate(line, bddNodes);
    }
    file.close();
    return 0;
}
std::vector<std::string> BDDParser::getOutputGates()
{
    return outputGates;
}
std::map<std::string, int> BDDParser::getGates()
{
    return gates;
}
int BDDParser::getGateCounter()
{
    return gateCounter;
}
