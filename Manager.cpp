/**
 * @file Manager.cpp
 * @author Rune Krauss
 *
 * The manager performs administrative tasks and provides operations that affect nodes.
 * For example, there is the ITE operator which represents the core of the package and identifies the
 * synthesis. Furthermore, the manager provides techniques that can also be used in synthesis to
 * increase performance in terms of runtime and memory. In addition to the corresponding terminal cases
 * which lead to an early termination of the synthesis, there are also so-called standard triplets. This
 * Combinations that lead to the same result are combined. Afterwards, A representative stands for the
 * result. Finally, I/O operations are also provided to visualize BDDs graphically.
 */
#include <fstream>
#include <sstream>
#include "Manager.hpp"

/**
 * Creates a Manager object and initializes the tables and support for the respective variables
 * stored in a vector. If no values are specified, the default settings apply, i.e. the unique and
 * computed table initially contain a maximum of 5003 nodes and the number of variables is limited
 * to 16.
 *
 * @param variables Number of variables
 * @param uTableSize Size of the unique table
 * @param cTableSize Size of the computed table
 */
Manager::Manager(unsigned variables, size_t uTableSize, size_t cTableSize)
{
    uTable.load(uTableSize);
    cTable.load(cTableSize);
    BDDNode::setManager(this);
    DDNode::setLeaf( findAdd(0, 0, 0) );
    BDDNode::setTerminal1( BDDNode(DDNode::getLeaf(), BDDNode::getRegularEdge()) );
    BDDNode::setTerminal0( BDDNode(DDNode::getLeaf(), BDDNode::getComplementEdge()) );
    variableCounter.reserve(variables + 1);
    variableCounter.push_back( BDDNode::getTerminal1() );
    int i = 1;
    while (i <= variables) {
        variableCounter.push_back( BDDNode(i, BDDNode::getTerminal1().getDDNode(), BDDNode::getTerminal0().getDDNode(), BDDNode::getRegularEdge()) );
        i++;
    }
}

/**
 * The destructor triggers the cleanup of the memory (@see clear) for the tables and variables.
 */
Manager::~Manager()
{
    clear();
}

/**
 * This method standardizes ambiguous ITE calls. There are parameters f1, f2, f3 and g1, g2, g3, so
 * ite(f1, f2, f3) = ite(g1, g2, g3). If an i exists, so that fi is not equal to gi, there is an
 * equivalence relation. There are combinations that lead to the same result. These can be grouped into
 * equivalence classes. Then, a combination is chosen that acts as a representative. For example,
 * parameters such as ite(f, g, f') are first transformed to ite(f, g, 1), then
 * ite(f, g, 1) = ite(g', f', 1) is determined as representative. The latter transformation is favoured
 * by the existence of complementary edges (negation can be calculated in constant time). This makes it
 * possible to keep the computed table small or to avoid redundant computings. As a result, there are
 * different rules that must be defined. With respect to complement edges, for example, f and g have
 * regular edges.
 *
 * @param f Top variable
 * @param g High child
 * @param h Low child
 * @param complementEdge Specifies whether a complement edge exists.
 */
void Manager::standardize(BDDNode &f, BDDNode &g, BDDNode &h, bool& complementEdge)
{
    // Identical rules
    if (f == g)
        g = BDDNode::getTerminal1();
    else if (f == h)
        h = BDDNode::getTerminal0();
    else if (f == !h)
        h = BDDNode::getTerminal1();
    else if (f == !g)
        g = BDDNode::getTerminal0();
    // Symmetrical rules
    if ( g == BDDNode::getTerminal1() ) {
        if ( f.getIndex() > h.getIndex() )
            swap(f, h);
    } else if ( g == BDDNode::getTerminal0() ) {
        if ( f.getIndex() > h.getIndex() ) {
            swap(f, h);
            f = !f;
            h = !h;
        }
    } else if (g == !h) {
        if ( f.getIndex() > g.getIndex() ) {
            swap(f, g);
            h = !g;
        }
    } else if ( h == BDDNode::getTerminal1() ) {
        if ( f.getIndex() > g.getIndex() ) {
            swap(f, g);
            f = !f;
            g = !g;
        }
    } else if ( h == BDDNode::getTerminal0() ) {
        if ( f.getIndex() > g.getIndex() )
            swap(f, g);
    }
    // Complementary rules
    if ( f.isComplementEdge() ) {
        swap(g, h);
        f = !f;
    }
    if ( g.isComplementEdge() ) {
        g = !g;
        h = !h;
        complementEdge = !complementEdge;
    }
}

/**
 * This method swaps neighboring variables with each other and is the basic operation for many
 * procedures whereby only the pointers are moved to nodes. This concerns, for example, the execution
 * of a permutation on an existing ROBDD. The respective OBDD is not affected by this. It should be
 * added that this is a local operation, i. e. only the nodes labeled with the two variables are
 * considered.
 * 
 * @param f OBDD node
 * @param g OBDD node
 */
void Manager::swap(BDDNode& f, BDDNode& g)
{
    BDDNode tmp = f;
    f = g;
    g = tmp;
}

/**
 * This method is used in the ITE algorithm and serves to check whether terminal cases exist. For
 * example, ite(1, f, g) = ite(0, g, f) = ite(f, 1, 0) = f is a terminal case. In this case, The result
 * can be returned immediately.
 *
 * @param f Top variable
 * @param g High child
 * @param h Low child
 * @param res Result of the specific case that is returned
 * @return Status whether a terminal case is exists
 */
bool Manager::isTerminal(const BDDNode& f, const BDDNode& g, const BDDNode& h, BDDNode& res)
{
    if ( f == BDDNode::getTerminal1() ) {
        res = g;
        return true;
    } else if ( f == BDDNode::getTerminal0() ) {
        res = h;
        return true;
    } else if ( h == BDDNode::getTerminal0() && g == BDDNode::getTerminal1() ) {
        res = f;
        return true;
    } else if (g == h) {
        res = g;
        return true;
    }
    return false;
}

/**
 * The respective BDD is processed whereby the information of the nodes is written to an output
 * stream or a file. This concerns information such as references or the respective designation.
 * The format corresponds to DOT, a description language for visualizing graphs. Dotted arrows
 * correspond to low edges while solid arrows correspond to positive edges. If there is a
 * circle on a low edge, a complement edge applies. The square also stands for the 1-leaf and is
 * therefore a terminal.
 *
 * @param node BDD to be visualized
 * @param file Filename
 */
void Manager::printNodeRecur(BDDNode& node, std::ofstream& file) const
{
    DDNode* ddNode = node.getDDNodeWithEdge();
    if ( ddNode->isMarked() )
        return;
    ddNode->setMarked(true);
    if( node.isLeaf() )
        return;
    BDDNode high = node.getHigh();
    BDDNode low = node.getLow();
    file << "\t { rank=same; " "\"" << printIndex(node) << "\"; }\n";
    file << "\t \"" << printIndex(node) << "\" -> \"" << printIndex(low) << "\" [style=dotted]";
    file << ( (low.isComplementEdge()) ? " [arrowhead=odot]" : ";") << std::endl;
    file << "\t \"" << printIndex(node) << "\" -> \"" << printIndex(high) << "\"";
    file << ";" << std::endl;
    printNodeRecur(low, file);
    printNodeRecur(high, file);
}

/**
 * Returns the node name for the visualization (@see printNodeRecur).
 * 
 * @param node Node for which the description is to be determined
 * @return Label of the node
 */
std::string Manager::printIndex(BDDNode& node) const
{
    if ( node.isLeaf() )
        return "terminal";
    std::ostringstream index;
    index << node.getDDNodeWithEdge();
    return index.str();
}

/**
 * This method is responsible for garbage collection, i. e. the respective memory for the tables or
 * variable reservations is cleaned up automatically. With regard to the unique table, this applies
 * only nodes that refer to themselves but are not used anywhere else in the SBDD. With the computed
 * table, this case does not have to be considered since the canonicity is not endangered. For this
 * reason, there is no need to search there.
 */
void Manager::clear()
{
    uTable.clear();
    cTable.clear();
    UTable::iterator it = uTable.begin();
    for (auto i = uTable.begin(); i != uTable.end(); i++)
        delete (*i).second;
    variableCounter.clear();
}

/**
 * Creates a variable and stores it in a vector. The higher the index, the further forward the
 * variable is in the order and is accordingly queried first during the synthesis.
 * 
 * @param variable Index of the variable
 * @return Support for a node
 */
const BDDNode& Manager::createVariable(unsigned variable) const
{
    assert(variable <= variableCounter.size()-1 && "There is no support for this variable.");
    return variableCounter[variable];
}

/**
 * The ITE algorithm represents the universal synthesis operator and computes ite(f, g, h) = fg + f'h.
 * Since f is a decision variable, this procedure corresponds exactly to the Shannon decomposition.
 * All Boolean operators can be returned to this ternary operator where the arguments from the recursive
 * formulation are obtained as a result of the decomposition. The corresponding cofactor computing is
 * linearly limited by the ROBDD whereby a computing is also possible in constant time if the root node
 * marking is considered since the solution corresponds to one of the two children. An isomorphism check
 * is possible in O(1) since only the root nodes t, e must be compared. By using the unique and computed
 * table and the associated modulo method (@see TableKey), the determination and storage of nodes in the
 * best case is also possible in O(1). Each operator is also called no more than once for each
 * combination of nodes. For this reason, O(|f|||g||h|) exists in total.
 * 
 * @param f Top variable
 * @param g High child
 * @param h Low child
 * @return BDD by a combination of BDDs
 */
BDDNode Manager::ite(BDDNode f, BDDNode g, BDDNode h)
{
    bool complementEdge = false;
    standardize(f, g, h, complementEdge);
    BDDNode resT;
    // Check terminal cases
    if ( isTerminal(f, g, h, resT) ) {
        if (complementEdge)
            resT = !resT;
        return resT;
    }
    TableKey key( f.getDDNode(), g.getDDNode(), h.getDDNode() );
    size_t resC;
    // Check if there is already a node with this parameters in the computed table
    if ( cTable.hasNext(key, resC) ) {
        if (complementEdge)
            resC = resC ^ BDDNode::getComplementEdge();
        return resC;
    }
    unsigned top = f.getIndex();
    if (g.getIndex() > top)
        top = g.getIndex();
    if (h.getIndex() > top)
        top = h.getIndex();
    // Determine the cofactors of f, g, h
    BDDNode fl = f.getCofactor( top, BDDNode::getHighFactor() );
    BDDNode gl = g.getCofactor( top, BDDNode::getHighFactor() );
    BDDNode hl = h.getCofactor( top, BDDNode::getHighFactor() );
    BDDNode f0 = f.getCofactor( top, BDDNode::getLowFactor() );
    BDDNode g0 = g.getCofactor( top, BDDNode::getLowFactor() );
    BDDNode h0 = h.getCofactor( top, BDDNode::getLowFactor() );
    /**
     * Use the cofactors to create two subproblems t, e
     * Select the root label that is first in the order
     */
    BDDNode t = ite(fl, gl, hl);
    BDDNode e = ite(f0, g0, h0);
    // Check for isomorphism
    if (t == e) {
        if (complementEdge)
            t = !t;
        return t;
    }
    /**
     * Create nodes in the unique table only if they do not yet exist
     * Otherwise, just return a reference to the node
     */
    DDNode* node = findAdd( top, t.getDDNode(), e.getDDNode() );
    resC = (size_t) node;
    // Save the computing in the computed table
    cTable.insert(key, resC);
    if (complementEdge)
        resC = resC ^ BDDNode::getComplementEdge();
    return resC;
}

/**
 * This method is called by the ITE operator (@see ite) and the algorithm for existential quantification
 * (@see existRecur) to determine whether a triple is already in the unique table (@see UTable). If
 * there is no triple, a new node will be created.
 *
 * @param f Top variable
 * @param g High child
 * @param h Low child
 * @return Node from the unique table
 */
DDNode* Manager::findAdd(size_t f, size_t g, size_t h)
{
    DDNode* ddNode = nullptr;
    TableKey key(f, g, h);
    if ( !uTable.find(key, ddNode) ) {
        ddNode = new DDNode(f, h, g);
        uTable.add(key, ddNode);
    }
    return ddNode;
}

/**
 * During the computing of the cofactors (@see getCofactor) variables are replaced by constants whereby
 * here the truth value is indifferent. Therefore, a quantification can be done whereby e. g. CTL
 * formulas can be represented. The following applies:
 * \exists{x_i}: f(x_1,...,x_n) = f_{x_i=0} + f_{x_i=1} where f depends on X_n.
 * This method itself can be used as a pre-computing step because it allows conclusions to be drawn
 * about the output thus reducing the input size. The effort corresponds to O(|f|^2) since a
 * quantification corresponds to a parallel traversing by the decision graph f.
 *
 * @param index Variable to be quantified
 * @return BDD with the quantified variable
 */
BDDNode Manager::existRecur(BDDNode& node, unsigned index)
{
    if ( node.isLeaf() )
        return node;
    unsigned currentIndex = node.getIndex();
    if (currentIndex < index)
        return node;
    BDDNode high = node.getCofactor( currentIndex, BDDNode::getHighFactor() );
    BDDNode low = node.getCofactor( currentIndex, BDDNode::getLowFactor() );
    TableKey k(node.getDDNode(), high.getDDNode(), low.getDDNode());
    size_t next;
    if (cTable.hasNext(k, next)) {
        return next;
    }
    if (index == currentIndex) {
        BDDNode res = low + high;
        cTable.insert(k, res.getDDNode());
        return res;
    }
    bool complementEdge = false;
    BDDNode t = existRecur(high, index);
    BDDNode e = existRecur(low, index);
    if (t == e) {
        cTable.insert(k, t.getDDNode());
        return t;
    }
    if ( t.isComplementEdge() ) {
        t = !t;
        e = !e;
        complementEdge = true;
    }
    BDDNode res(findAdd( currentIndex, t.getDDNode(), e.getDDNode()), BDDNode::getRegularEdge() );
    if (complementEdge)
        res = !res;
    cTable.insert(k, res.getDDNode());
    return res;
}

/**
 * Prepares the visualization of a decision graph, that is, properties for the shape of the nodes or
 * leaves are specified and the root nodes are written. Afterwards, there is a traversing through the
 * graph to represent the internal nodes (@see printNodeRecur).
 *
 * @param node BDD to be visualized
 * @param name Name of the node (function name)
 * @param file Filename
 */
void Manager::printNode(BDDNode& node, const std::string& name, std::ofstream& file) const
{
    file << "digraph {" << std::endl;
    file << "\t node [shape=plaintext];" << std::endl;
    file << "\t terminal [label=\"1\", shape=square];" << std::endl;
    file << "\t { rank=source; \"" << name << "\"; }" << std::endl;
    file << "\t node [shape=oval];" << std::endl;
    file << "\t \"" << name << "\" -> \"" << printIndex(node) << "\"";
    file << (node.isComplementEdge() ? " [arrowhead=odot]" : ";") << std::endl;
    printNodeRecur(node, file);
    node.setMarked(false);
    file << "\t { rank=same; \"terminal\"; }" << std::endl;
    file << "}" << std::endl;
}

/**
 * The current number of nodes and the elapsed time for the synthesis are displayed here.
 *
 * @param comparedTime Time needed for synthesis
 */
void Manager::showInfo(const double comparedTime, std::vector<BDDNode>& bddNodes) const
{
    struct rusage r_usage;
    getrusage(RUSAGE_SELF, &r_usage);
    int bddNodeCount = 0;
    std::for_each(bddNodes.rbegin(), bddNodes.rend(), [&bddNodeCount](BDDNode bddNode) { bddNodeCount += bddNode.countNodes(); });
    std::cout << "Nodes count: " << bddNodeCount << std::endl;
    std::cout << "Time in seconds: " << comparedTime << std::endl;
    std::cout << "Memory usage: " << r_usage.ru_maxrss << std::endl;
}
