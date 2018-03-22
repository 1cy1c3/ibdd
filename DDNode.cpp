/**
 * @file DDNode.cpp
 * @author Rune Krauss
 *
 * Since an integer is used as data type for the variables, the number of possible variables
 * is limited to 2^16, i. e. an approximate 22 bytes may be assumed for the nodes. Related to
 * this, there are references to the low and high child as well as the label. There is still a
 * reference counter for garbage collection. If a node is created,  this counter holds a 1. If
 * another node is used at another place during the synthesis (@see Manager#ite), the counter is
 * incremented. If, for example, a formula and the associated nodes are cleaned up, the respective
 * counters are decremented. If the reference counter is set to 0, the node is not required and can
 * be deleted.  This class or all operations are wrapped of type "BDDNode" (@see BDDNode). This is
 * the case due to automatic garbage collection, so that the reference counter can not only be incremented
 * automatically, but also decremented, i. e. there is a pointer to it in the type "BDDNode".
 */
#include "DDNode.hpp"

/**
 * Creates a constant (leaf). A node has logically not yet been visited or selected. The ID is initially
 * set to 0 and is increased for the computed table when the address is accessed. The reference counter
 * is 1, i. e. the node only refers to itself.
 */
DDNode::DDNode() : id(1), index(0), marked(false) {};

/**
 * Initializes a node with children at one level of the BDD. The reference counter is controlled by the type
 * "BDDNode", i. e. increased or decreased.
 *
 * @param low Low child
 * @param high High child
 * @param index Label or variable
 */
DDNode::DDNode(size_t index, BDDNode low, BDDNode high)
{
    this->index = index;
    this->low = low;
    this->high = high;
    id = 1;
    marked = false;
}

/**
 * Increments the reference counter for the node. If this is 65535, the value remains and the node can no longer
 * be deleted (compromise between compactness and garbage collection).
 */
DDNode& DDNode::operator ++() {
    id++;
    return *this;
}

/**
 * Increments the reference counter for the node and calls the prefix increment operator internally for this purpose.
 */
DDNode DDNode::operator ++(int) {
    DDNode tmp = *this;
    ++(*this);
    return tmp;
}

/**
 * Decrements the reference counter for the node. If this is set to 0, the node can be cleaned up to create space for new nodes.
 */
DDNode& DDNode::operator --() {
    id--;
    return *this;
}

/**
 * Decrements the reference counter for the node and calls the prefix increment operator internally for this purpose.
 */
DDNode DDNode::operator --(int) {
    DDNode tmp = *this;
    --(*this);
    return tmp;
}

const BDDNode& DDNode::getLow() const
{
    return low;
}

void DDNode::setLow(BDDNode& low)
{
    this->low = low;
}

const BDDNode& DDNode::getHigh() const
{
    return high;
}

void DDNode::setHigh(BDDNode& high)
{
    this->high = high;
}

DDNode* DDNode::getLeaf()
{
    return DDNode::leaf;
}

void DDNode::setLeaf(DDNode* leaf)
{
    DDNode::leaf = leaf;
}

unsigned DDNode::getID() const
{
    return id;
}

void DDNode::setID(unsigned id)
{
    this->id = id;
}

unsigned DDNode::getIndex() const
{
    return index;
}

void DDNode::setIndex(unsigned index)
{
    this->index = index;
}

bool DDNode::isMarked() const
{
    return (marked == true);
}

void DDNode::setMarked(bool marked)
{
    this->marked = marked;
}
