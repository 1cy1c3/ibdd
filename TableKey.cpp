/**
 * @file TableKey.cpp
 * @author Rune Krauss
 *
 * The hash function must be computed optimally so that the computed or unique table can guarantee
 * access to nodes in constant time. The performance depends directly on the usage factor which
 * represents the quotient of data records and table size whereby the respective triple (f, g, h)
 * identifies the key. Since hash functions are injective, the function must be determined in such
 * a way that collisions are reduced. By an optimal determination a polynomial effort for the synthesis
 * (@see Manager#ite) can be guaranteed.
 */
#include "TableKey.hpp"

/**
 * This constructor creates an empty key object, that is, there are no references to the respective triple yet.
 */
TableKey::TableKey(){}

/**
 * A key object is generated directly with the nodes from which the hash code is to be generated. This means there
 * is no need for extra assignments.
 *
 * @param f Decision variable
 * @param g High child
 * @param h Low child
 */
TableKey::TableKey(size_t f, size_t g, size_t h) : f(f), g(g), h(h) {}

/**
 * This generates a key for the unique table (@see UTable) or computed table (@see CTable) including the nodes
 * f, g, h (triple). The method identifies a so-called functional object which is treated as a function.
 * Afterwards, The key can be used to access nodes that have already been computed (represent a Boolean function).
 *
 * @return Key for accessing nodes in the table
 */
unsigned long TableKey::operator ()() const
{
    return ( (g + h) >> f);
}

/**
 * Defines the equivalence of keys. They are exactly the same when the conjunction of them is equivalent.
 *
 * @param key Key to be compared
 * @return True, if the conjunction of the nodes is equivalent, otherwise False
 */
bool TableKey::operator ==(const TableKey& key) const
{
    return ( (f == key.f) && (g == key.g) && (h == key.h) );
}
