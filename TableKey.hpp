/**
 * @file TableKey.hpp
 * @author Rune Krauss
 *
 * @brief Generates a hash code based on nodes f, g, h which can be used to store nodes in the computed/unique
 * table or to check whether these nodes have already been saved.
 */
#ifndef TableKey_hpp
#define TableKey_hpp

#include <vector>

/**
 * This class implements a modulo method h(f, g, h) = ((g + h) >> f) `mod` m as a hash function. It should be
 * mentioned that the ALU division block is complex with regard to the operation. A right shift by i digits
 * corresponds to a divisor 2^i depending on the word width. Overall, a prime number must be selected for m
 * (size of the respective table) so that there is an equal distribution of the nodes. Otherwise, the higher
 * bits for computing are ignored and there are more frequent collisions. The type "size_t" is also used for
 * the respective nodes because this application works with memory limits.
 */
class TableKey
{
private:
    /**
     * Top variable
     */
    size_t f;
    
    /**
     * High child regarding the then-case
     */
    size_t g;
    
    /**
     * Low child regarding the else-case
     */
    size_t h;
public:
    /**
     * @brief Standard constructor that instantiates a key object.
     */
    TableKey();
    
    /**
     * @brief Constructor that assigns nodes directly to this class.
     */
    TableKey(size_t, size_t, size_t);
    
    /**
     * @brief Destructor that does not need to free memory due to automatic garbage collection.
     */
    virtual ~TableKey() = default;
    
    /**
     * @brief This can be used to generate a key. The nodes or the triple (f, g, h) are considered.
     */
    size_t operator ()() const;
    
    /**
     * @brief Overloads the operator "==" which defines when keys are equivalent.
     */
    bool operator ==(const TableKey&) const;
};
#endif
