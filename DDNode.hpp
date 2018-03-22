/**
 * @file DDNode.hpp
 * @author Rune Krauss
 *
 * @brief A node represents an ordered BDD (OBDD) or reduced OBDD (ROBDD). Each ROBDD is canonical according
 * to Bryant's theorem, i. e. a unique representation of a Boolean function. With regard to the nodes,
 * there can be different start nodes which means there is a common BDD (SBDD). So there is a collection
 * of nodes that have the same variable order. This saves storage space and prevents multiple identical computings.
 */
#ifndef DDNode_hpp
#define DDNode_hpp

#include "BDDNode.hpp"

/**
 * This class represents the wrapped node that implements the properties such as references to successors or
 * the reference counter. The node is wrapped because the actual operations such as cofactor computings or memory
 * cleanup are performed by the BDDNode (@see BDDNode).
 */
class DDNode
{
private:
    /*
     * The low child of a node corresponds to the else-case during synthesis and can also represent a root node
     * in the SBDD (@see Manager#ite).
     */
    BDDNode low;
    
    /*
     * The low child of a node corresponds to the then-case during synthesis and can also represent a root node
     * in the SBDD (@see Manager#ite).
     */
    BDDNode high;
    
    /*
     * A leaf has no successors, that is, there are no references to children in the BDD which implements it as
     * a constant or terminal.
     */
    static DDNode* leaf;
    
    /**
     * The reference counter contains 2 bytes to keep the OBDD nodes as small as possible. From a reference
     * number of 65535, the node would no longer be deleted. This is a compromise between memory consumption and compactness.
     */
    unsigned id: 16;
    
    /**
     * The variable is represented by a bit field which holds 2 bytes. This means the respective level in the graph
     * which will play a role above all for a variable exchange and with regard to algorithms for finding the optimal variable order.
     */
    size_t index: 16;
    
    /**
     * This flag is directly related to the selected variables that refer to the node. If a node is visited, it is noted here.
     * Furthermore, this flag will also play an important role for algorithms for finding the optimal variable order,
     * for example, if the longest paths are to be found.
     */
    bool marked;
public:
    /**
     * @brief Creates a node consisting of a leaf.
     */
    DDNode();
    
    /**
     * @brief Initializes a node with children at one level of the graph.
     */
    DDNode(size_t, BDDNode, BDDNode);
    
    /**
     * @brief If this object is destroyed, no memory must be freed (controlled by BDDNode and performed automatically).
     */
    virtual ~DDNode() = default;
    
    /**
     * @brief Increases the reference counter by 1 (prefix).
     */
    DDNode& operator ++();
    
    /**
     * @brief Increases the reference counter by 1 (postfix).
     */
    DDNode operator ++(int);
    
    /**
     * @brief Decreases the reference counter by 1 (prefix).
     */
    DDNode& operator --();
    
    /**
     * @brief Decreases the reference counter by 1 (postfix).
     */
    DDNode operator --(int);
    
    const BDDNode& getLow() const;
    
    void setLow(BDDNode&);
    
    const BDDNode& getHigh() const;
    
    void setHigh(BDDNode&);
    
    static DDNode* getLeaf();
    
    static void setLeaf(DDNode*);
    
    unsigned getID() const;
    
    void setID(unsigned);
    
    unsigned getIndex() const;
    
    void setIndex(unsigned);
    
    bool isMarked() const;
    
    void setMarked(bool);
};
#endif
