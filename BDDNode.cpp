/**
 * @file BDDNode.cpp
 * @author Rune Krauss
 *
 * In order for the application to work with millions of nodes, nodes must be as compact as possible.
 * The most important property for their implementation is their uniqueness in the form of ROBDDs.
 * In this context, each node represents exactly one Boolean formula with the variables that are responsible
 * for labeling subnodes. If a node v in a ROBDD has the label l(f) and as children g, h, the node v is
 * uniquely identified with ite(f, g, h) (@see Manager#ite). The variable selection thus enables the
 * algorithm to move from one node to another node. Because the DDNode (@see DDNode) is controlled by this
 * data structure, it is also possible to automatically decrement the reference counter (@see DDNode#counter)
 * whereby the memory can be optimally regulated.
 */
#include "BDDNode.hpp"
#include "Manager.hpp"

Manager* BDDNode::manager = nullptr;
DDNode* DDNode::leaf = nullptr;
BDDNode BDDNode::terminal0 = 0;
BDDNode BDDNode::terminal1 = 1;

/**
 * Creates an object or node and establishes a bidirectional connection to the DDNode which is wrapped
 * in this node, so that the reference counter can be optimally controlled.
 */
BDDNode::BDDNode() : ddNode(0) {}

/**
 * This constructor creates a node with its label and children and the respective incoming edge which
 * is regular or complementary. The constructor is called mainly during synthesis when nodes are saved
 * or queried.
 *
 * @param f Top variable
 * @param g High child
 * @param h Low child
 */
BDDNode::BDDNode(size_t f, size_t g, size_t h, edge edge)
{
    assert (f != 0 && "The node f must be referenced");
    DDNode* node = manager->findAdd(f, g, h);
    ddNode = (size_t) node + edge;
    ++(*node);
}

/**
 * This constructor creates nodes with an incoming edge but without children. This creates a leaf that
 * can be reached either by a regular or complement edge.
 *
 * @param node Wrapped node
 * @param edge Regular or complement edge
 */
BDDNode::BDDNode(DDNode* node, edge edge)
{
    assert (node != nullptr && "The node must be referenced");
    ddNode = (size_t) node + edge;
    ++(*node);
}

/**
 * Required by the manager to display a node using an ID. The reference counter is increased by 1 because
 * the node is accessed.
 *
 * @param node Node to be checked
 */
BDDNode::BDDNode(size_t node)
{
    ddNode = node;
    DDNode* ddNode = getDDNodeWithEdge();
    if (ddNode)
        ++(*ddNode);
}

/**
 * The copy constructor is responsible for copying nodes. This is adapted in such a way that it does not
 * allow constructors to delete the same memory. During copying, the reference counter is also increased
 * or incremented.
 *
 * @param node Node to be copied
 */
BDDNode::BDDNode(const BDDNode& node) : ddNode(node.ddNode)
{
    DDNode* ddNode = getDDNodeWithEdge();
    if (ddNode)
        ++(*ddNode);
}

/**
 * In the destructor, the reference counter for the type "DDNode" (@see DDNode) is decremented. If the scope
 * is outside a variable of type "BDDNode", the reference counter is decremented by its associated node.
 */
BDDNode::~BDDNode()
{
    DDNode* ddNode = getDDNodeWithEdge();
    if (ddNode)
        --(*ddNode);
}

/**
 * This method is called when a node is passed into an output stream. The respective BDD is processed
 * whereby the individual nodes or their addresses are output. If debugging mode is activated, additional
 * information is output, such as the number of references.
 * It applies <Address>[Variable, edge type, references, visitor status].
 * Finally, the number of nodes is computed.
 *
 * @param output Output stream
 * @param counter Counter for nodes
 */
void BDDNode::showInfo(std::ostream& output, size_t& counter, factor factor) const
{
    DDNode* ddNode = getDDNodeWithEdge();
    assert(ddNode != nullptr && "The node must be referenced");
    output << ddNode << " ";
#ifdef DEBUG
    output << '[' << getIndex() << ", ";
    if (factor == root)
        output << (isComplementEdge() ? '~' : '+');
    else if (factor == high)
        output << '+';
    else {
        output << (isComplementEdge() ? '~' : '-');
    }
    output << ", ";
    output << ddNode->getID() << (ddNode->isMarked() ? ", X] " : "] ");
#endif
    if ( ddNode->isMarked() ) {
        return;
    } else
        counter++;
    ddNode->setMarked(true);
    if ( !isLeaf() ) {
        output << "( ";
        ddNode->getLow().showInfo(output, counter, factor::low);
        ddNode->getHigh().showInfo(output, counter, factor::high);
        output << ") ";
    }
}

/**
 * This method is called by "countNodes" (@see countNodes) where the BDD is processed pre-order,
 * i. e. the counter is incremented by 1 for each node. Nodes that are already visited are marked
 * accordingly so that they are not counted twice.
 *
 * @param counter Counter for nodes
 */
void BDDNode::countNodesRecur(size_t& counter) const
{
    DDNode* ddNode = getDDNodeWithEdge();
    if ( ddNode->isMarked() ) {
        return;
    } else
        counter++;
    ddNode->setMarked(true);
    if ( !isLeaf() ) {
        ddNode->getLow().countNodesRecur(counter);
        ddNode->getHigh().countNodesRecur(counter);
    }
}

/**
 * There are also incompletely defined functions that are partial, i. e. not left-total.
 * Thus, there are DCs whereby their mapping must be selected in such a way that the values
 * can be optimally displayed. To minimize a BDD to this end, as well as to find the optimal
 * variable order, NP-complete, which is why one practically uses heuristics. These use a fixed
 * order whereby instances of common nodes are maximized or siblings are substituted (exist).
 *
 * @param index Variable to be quantified
 * @return BDD with quantified variable
 */
BDDNode BDDNode::exist(unsigned index)
{
    if (index == 0)
        return *this;
    return manager->existRecur(*this, index);
}

/**
 * Node information (@see showInfo) is written to this output stream and output to the console.
 *
 * @param output Output stream
 * @param node Root node
 */
std::ostream& operator <<(std::ostream& output, const BDDNode& node)
{
    size_t counter = 0;
    node.showInfo(output, counter, BDDNode::factor::root);
    node.setMarked(false);
    output << std::endl << "#Knoten: " << counter << std::endl;
    return output;
}

/**
 * This operation conjures two BDDs where and(f, g) stands for fg and can be expressed by ite(f, g, 0).
 *
 * @param other BDD g for the conjunction
 * @return Conjunction between two BDDs
 */
BDDNode BDDNode::operator *(const BDDNode& other) const
{
    return manager->ite(*this, other, BDDNode::terminal0);
}

/**
 * This operation compares two BDDs where >(f, g) stands for fg' and can be expressed by ite(f, g', 0).
 *
 * @param other BDD g for the comparison
 * @return Comparison between two BDDs
 */
BDDNode BDDNode::operator >(const BDDNode& other) const
{
    return manager->ite(*this, !other, BDDNode::terminal0);
}

/**
 * This operation compares two BDDs where <(f, g) stands for f'g and can be expressed by ite(f, 0, g).
 *
 * @param other BDD g for the comparison
 * @return Comparison between two BDDs
 */
BDDNode BDDNode::operator <(const BDDNode& other) const
{
    return manager->ite(*this, BDDNode::terminal0, other);
}

/**
 * Disambiguation between two BDDs where xor(f, g) stands for f^g and can be expressed by ite(f, g', g).
 *
 * @param other BDD g for the disambiguation
 * @return Disambiguation between two BDDs
 */
BDDNode BDDNode::operator ^(const BDDNode& other) const
{
    return manager->ite(*this, !other, other);
}

/**
 * This operation disjoints two BDDs where or(f, g) stands for f+g and can be expressed by ite(f, 1, g).
 *
 * @param other BDD g for the disjunction
 * @return Disjunction between two BDDs
 */
BDDNode BDDNode::operator +(const BDDNode& other) const
{
    return manager->ite(*this, BDDNode::terminal1, other);
}

/**
 * Two BDDs are linked to each other via "Not Or" where nor(f, g) is (f+g)' and can be described
 * by ite(f, 0, g').
 *
 * @param other BDD g for the linkage
 * @return "Not Or" linkage between two BDDs
 */
BDDNode BDDNode::operator |(const BDDNode& other) const
{
    return manager->ite(*this, BDDNode::terminal0, !other);
}

/**
 * Two BDDs are linked to each other via "=>" where xnor(f, g) is (f^g)' and can be described
 * by ite(f, g, g').
 *
 * @param other BDD g for the linkage
 * @return "Not Or" linkage between two BDDs
 */
BDDNode BDDNode::operator %(const BDDNode& other) const
{
    return manager->ite(*this, other, !other);
}

/**
 * This operator describes the negation where a node f' is not stored but an edge to f which
 * sets the complement bit.
 *
 * @return Negation of a BDD
 */
BDDNode BDDNode::operator !() const
{
    return (ddNode ^ edge::complement);
}

/**
 * Two BDDs are linked to each other via "Not And" where nand(f, g) is (fg)' and can be described
 * by ite(f, g', 1).
 *
 * @param other BDD g for the linkage
 * @return "Not Or" linkage between two BDDs
 */
BDDNode BDDNode::operator &(const BDDNode& other) const
{
    return manager->ite(*this, !other, BDDNode::terminal1);
}

/**
 * This operator allows to assign results of a synthesis to a node. Suppose a = b where a, b are
 * nodes of type "BDDNode". In that case, the reference counter of a or its associated node of type
 * "DDNode" (@see DDNode) is decreased. Afterwards, the associated node of b is increased. Subsequently,
 * the corresponding node of b is assigned to that of a.
 *
 * @param other BDD to be assigned
 * @return Assignment of the BDD or the result of a synthesis
 */
BDDNode& BDDNode::operator =(const BDDNode& other)
{
    DDNode* node = getDDNodeWithEdge();
    if (node)
        --(*node);
    ddNode = other.ddNode;
    node = getDDNodeWithEdge();
    if (node)
        ++(*node);
    return *this;
}

/**
 * The method checks two nodes to see if they are identical. Two nodes are called identical exactly
 * when they have the same DDNode (@see DDNode).
 *
 * @param other BDD to check the identity
 * @return Status whether two nodes are identical
 */
bool BDDNode::operator ==(const BDDNode& other) const
{
    return (ddNode == other.ddNode);
}

/**
 * The method checks two nodes to see if they are not identical. Two nodes are called not identical
 * exactly when they have a different DDNode (@see DDNode).
 *
 * @param other BDD to check the inequality
 * @return Status whether two nodes are not identical
 */
bool BDDNode::operator !=(const BDDNode& other) const
{
    return (ddNode != other.ddNode);
}

/**
 * Cofactor computing is a special case of composition which is about maximizing common instances in
 * terms of minimizing BDDs. A variable is replaced by a constant function (0, 1) where
 * f_{x_i} = f(...,x_{i-1},0,x_{i+1},...) is computed during traversing. If the root label is the cofactor
 * of a variable, the solution corresponds to one of the two children and a constant runtime applies.
 * Otherwise, traversing is linearly restricted by the ROBDD.
 * 
 * @param index Variable to be resolved
 * @param factor With "high" the high child is considered, with "low" the low child is considered
 * @return ROBDD after cofactor computing
 */
BDDNode BDDNode::getCofactor(unsigned index, factor factor) const
{
    assert(getDDNodeWithEdge() != nullptr && "The node must be referenced");
    if ( index > getIndex() )
        return *this;
    if ( index == getIndex() ) {
        if (factor == high)
            return isComplementEdge() ? !getHigh() : getHigh();
        else
            return isComplementEdge() ? !getLow() : getLow();
    }
    BDDNode t = (factor == high) ? getHigh().getCofactor(index, high) : getHigh().getCofactor(index, low);
    BDDNode e = (factor == high) ? getLow().getCofactor(index, high) : getLow().getCofactor(index, low);
    if (t == e)
        return isComplementEdge() ? !t : t;
    edge edgeF;
    if ( isComplementEdge() ^ t.isComplementEdge() )
        edgeF = edge::complement;
    else
        edgeF = edge::regular;
    if ( t.isComplementEdge() )
        t = !t;
        e = !e;
    return BDDNode(getIndex(), t.getDDNode(), e.getDDNode(), edgeF);
}

/**
 * This method is particularly useful for output from the relevant BDD to determine whether a node
 * has already been visited. This avoids unnecessary paths during pre-order traversing (@see showInfo)
 * so that the graph can be traversed faster.
 *
 * @param marked Visited?
 */
void BDDNode::setMarked(bool marked) const
{
    DDNode* ddNode = getDDNodeWithEdge();
    assert(ddNode != nullptr && "The node must be referenced");
    if ( !ddNode->isMarked() )
        return;
    else
        ddNode->setMarked(marked);
    if ( !isLeaf() ) {
        ddNode->getHigh().setMarked(marked);
        ddNode->getLow().setMarked(marked);
    }
}

/**
 * Determines the number of nodes from a BDD by running them recursively.
 *
 * @return Number of nodes
 */
size_t BDDNode::countNodes() const
{
    size_t counter = 0;
    countNodesRecur(counter);
    setMarked(false);
    return counter;
}

/**
 * A node f' is not stored but an edge is set to f which has the complement bit. Specifically,
 * a bitmask is set to determine whether a complementary or regular edge exists, with the information
 * coming from the DDNode (LSB of pointers are used for coding). Accordingly, one bit of the memory
 * address is reserved for displaying a complement. If the bit is set, the complement bit applies.
 *
 * @return Wrapped node with edge type
 */
DDNode* BDDNode::getDDNodeWithEdge() const
{
    return (DDNode*) (ddNode & ( (~( (size_t) 0) >> 2) << 2) );
}

size_t BDDNode::getDDNode() const
{
    return ddNode;
}

bool BDDNode::isComplementEdge() const
{
    return (ddNode & edge::complement);
}

bool BDDNode::isLeaf() const
{
    return ( getDDNodeWithEdge() == DDNode::getLeaf() );
}

const BDDNode& BDDNode::getHigh() const
{
    assert(getDDNodeWithEdge() != nullptr && "The node must be referenced");
    return getDDNodeWithEdge()->getHigh();
}

const BDDNode& BDDNode::getLow() const
{
    assert(getDDNodeWithEdge() != nullptr && "The node must be referenced");
    return getDDNodeWithEdge()->getLow();
}

unsigned BDDNode::getIndex() const
{
    return getDDNodeWithEdge()->getIndex();
}

BDDNode::edge BDDNode::getComplementEdge() {
    return edge::complement;
}

BDDNode::edge BDDNode::getRegularEdge() {
    return edge::regular;
}

BDDNode::factor BDDNode::getLowFactor() {
    return factor::low;
}

BDDNode::factor BDDNode::getHighFactor() {
    return factor::high;
}

BDDNode BDDNode::getTerminal1() {
    return BDDNode::terminal1;
}

void BDDNode::setTerminal1(const BDDNode node) {
    BDDNode::terminal1 = node;
}

BDDNode BDDNode::getTerminal0() {
    return BDDNode::terminal0;
}

void BDDNode::setTerminal0(const BDDNode node) {
    BDDNode::terminal0 = node;
}

void BDDNode::setManager(Manager* manager) {
    BDDNode::manager = manager;
}
