/**
 * @file BDDNode.hpp
 * @author Rune Krauss
 *
 * @brief This data structure wraps the DDNode (@see DDNode) due to the automatic garbage collection
 * and therefore offers operations like the conjunction or cofactor calculation on nodes. Furthermore,
 * in a common BDD (SBDD) the negation of a Boolean function is always represented by means of complement
 * edges, i. e. the negation can be calculated in O(1).
 */
#ifndef BDDNode_hpp
#define BDDNode_hpp

#include <cstddef>
#include <iostream>

class DDNode;
class Manager;

/**
 * This class manages the properties of nodes by defining operations on them. All Boolean operators can
 * be traced back to the ternary operator (@see Manager#ite). Thus, for example, the conjunction
 * ite(f, g, 0) = fg+f'0 = fg. The Shannon decomposition is used to determine the cofactors during
 * synthesis and to check whether regular or complement edges exist in this respect. For this purpose,
 * the LSB of the pointer addresses of nodes is used (@see getDDNode). If it is set, a complement edge
 * applies, otherwise a regular edge. To ensure canonicity, the rules (@see Manager#standardize) apply
 * that there is only a 1-leaf and the output inverters are only allowed on low edges. An exception here
 * are the root nodes for which output inverters may also be used.
 */
class BDDNode
{
private:
    /**
     * Manages all operations related to the BDD such as the creation of variables.
     */
    static Manager* manager;
    
    /**
     * To display the complement, it is possible to use the fact that pointer addresses are a
     * multiplier of 4. The two least significant bits are used to capture the edge information.
     * The smallest bit of it is used for the inversion. This variable contains data of the type
     * "DDNode" (@see getDDNodeWithEdge) and an edge information whether a regular or complement
     * edge exists.
     */
    size_t ddNode;
    
    /**
     * Stands for a regular or complement edge.
     */
    enum edge
    {
        regular = 0,
        complement = 1,
    };
    
    /**
     * Indicates the cofactor to be considered or, as a special case, the root node required
     * for the output (@see showInfo).
     */
    enum factor {
        low = 0,
        high = 1,
        root = 2
    };
    
    /**
     * Indicates a leaf that has an incoming regular edge.
     */
    static BDDNode terminal1;
    
    /**
     * Indicates a leaf that has an incoming complement edge.
     * To maintain canonicity, a 1-leaf is also described here.
     */
    static BDDNode terminal0;
    
    /**
     * @brief This method specifies information such as the number of nodes for the BDD.
     */
    void showInfo(std::ostream&, size_t&, factor) const;
    
    /**
     * @brief Iterates through the graph recursively and counts the respective nodes.
     */
    void countNodesRecur(size_t&) const;
public:
    /**
     * @brief Initializes the node and establishes the bidirectional relationship to the DDNode (@see DDNode).
     */
    BDDNode();
    
    /**
     * @brief Determines a node using an ID from the hash tables.
     */
    BDDNode(size_t);
    
    /**
     * @brief Creates a new triplet of nodes with an incoming edge.
     */
    BDDNode(size_t, size_t, size_t, edge);
    
    /**
     * @brief Creates a node with an incoming edge that is either complementary or regular.
     */
    BDDNode(DDNode*, edge);
    
    /**
     * @brief Creates a copy of a node but it must not be empty.
     */
    BDDNode(const BDDNode&);
    
    /**
     * Main location where garbage collection is activated and the reference counter is decremented.
     */
    ~BDDNode();
    
    /*
     * This method is used to execute an existential quantification, i. e. a substitution of
     * siblings in relation to a variable.
     */
    BDDNode exist(unsigned);
    
    /**
     * @brief Represents the AND operator.
     */
    BDDNode operator *(const BDDNode&) const;
    
    /**
     * @brief Represents the "More than" operator.
     */
    BDDNode operator >(const BDDNode&) const;
    
    /**
     * @brief Represents the "Less than" operator.
     */
    BDDNode operator <(const BDDNode&) const;
    
    /**
     * @brief Represents the XOR operator.
     */
    BDDNode operator ^(const BDDNode&) const;
    
    /**
     * @brief Represents the OR operator.
     */
    BDDNode operator +(const BDDNode&) const;
    
    /**
     * @brief Represents the NOR operator.
     */
    BDDNode operator |(const BDDNode&) const;
    
    /**
     * @brief Represents the XNOR operator.
     */
    BDDNode operator %(const BDDNode&) const;
    
    /**
     * @brief Represents the NOT operator.
     */
    BDDNode operator !() const;
    
    /**
     * @brief Represents the NAND operator.
     */
    BDDNode operator &(const BDDNode&) const;
    
    /**
     * @brief Creates a copy of a node.
     */
    BDDNode& operator =(const BDDNode&);
    
    /**
     * @brief Finds out whether two nodes are identical.
     */
    bool operator ==(const BDDNode&) const;
    
    /**
     * @brief Determines whether two nodes are not equal.
     */
    bool operator !=(const BDDNode&) const;
    
    /**
     * @brief The overload of the I/O operator ensures that the information of the respective BDD
     * is written to the console in an output stream.
     */
    friend std::ostream& operator <<(std::ostream&, const BDDNode&);
    
    /**
     * @brief Each node stores a triple with a variable v which breaks it down. The corresponding
     * children result recursively from an ITE call (@see Manager#ite) of the cofactors which can
     * be computed with this method.
     */
    BDDNode getCofactor(const unsigned, factor) const;
    
    /**
     * @brief Sets the visitor status at the respective node during traversing.
     */
    void setMarked(bool) const;
    
    /**
     * @brief Counts the number of nodes in a decision graph.
     */
    size_t countNodes() const;
    
    /**
     * @brief Returns the DDNode with the respective edge information.
     */
    DDNode* getDDNodeWithEdge() const;
    
    size_t getDDNode() const;
    
    bool isComplementEdge() const;
    
    bool isLeaf() const;
    
    const BDDNode& getHigh() const;
    
    const BDDNode& getLow() const;
    
    unsigned getIndex() const;
    
    static edge getComplementEdge();
    
    static edge getRegularEdge();
    
    static factor getLowFactor();
    
    static factor getHighFactor();
    
    static BDDNode getTerminal1();
    
    static void setTerminal1(const BDDNode);
    
    static BDDNode getTerminal0();
    
    static void setTerminal0(const BDDNode);
    
    static void setManager(Manager*);
};
#endif
