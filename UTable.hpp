/**
 * @file UTable.hpp
 * @author Rune Krauss
 *
 * The Unique-Table (UT) ensures the canonicality of the BDD and allows quick access to nodes/edges.
 * Nodes are accessed via a triple f = (v, g, h). If g and h have a canonicity, f exists on condition
 * that there is an entry for (v, g, h). In contrast to the computed table, no entries that are still
 * required may be deleted, since this destroys the canonicity. Thus, there is a collision strategy
 * (concatenation of defectors in a list to save storage space). Thus, nodes and the UT are combined
 * with each other whereby the node contains a reference to the successor or such nodes are stored in
 * a common slot. This means that intermediate nodes that have already been calculated and are no longer
 * required, can be deleted (even their successors, provided there are no pointers to them). For efficient
 * implementation, the nodes (@see DDNode) have a reference counter. If, for example, this is set to 0,
 * the node can be deleted and is also decremented for all successors.
 */
#ifndef UTable_hpp
#define UTable_hpp

/**
 * This class implements the unique table to store and reuse nodes. The canonicity is ensured directly,
 * so that a reduction (redundancy and isomorphism rule) does not have to be called up constantly.
 * So that operations are possible in O(1), an equal distribution of the nodes must exist or collisions
 * must be reduced. There is a modulo method (@see TableKey) for this purpose. With regard to variable swap,
 * it also makes sense to use a UT for each variable since this no longer has to be contained in the key
 * which means that the entire table does not have to be searched. If the collision rate is too high,
 * there is a dynamic extension.
 */
template <typename K, typename E>
class UTable
{
private:
    /**
     * The nodes are stored in a vector as an image of keys (triplets) on nodes whereby the key is generated
     * as hash code itself. Vectors keep the nodes in memory close together so that a search can be performed
     * quickly. Due to the principle of location, the next block that must be loaded into the memory is
     * always estimated which also leads to an improvement in memory utilization.
     */
    std::vector<std::pair<K, E> >* items;
    
    /**
     * Specifies the size of the UT, that is, how many nodes it can contain. With regard to the modulo method,
     * a prime number should be selected here in order to enable optimum uniform distribution.
     */
    size_t size;
    
    /**
     * @brief Returns a generated key that gives access to nodes.
     */
    size_t getKey(const K&) const;
public:
    /**
     * @brief Initializes an empty UT, i. e. there are no nodes in the hash table and the size is 0.
     */
    UTable();
    
    /**
     * @brief Cleans the memory for the UT so that the nodes can be reused.
     */
    virtual ~UTable();
    
    /**
     * @brief Loads the UT according to a certain size and allocates the associated memory.
     */
    void load(size_t);
    
    /**
     * @brief Resets the UT to the origin, i. e. size 0 applies and there is not yet
     * a node in the table.
     */
    void clear();
    
    /**
     * @brief Returns True if there are no valid nodes in the UT. A node means valid exactly
     * when there is a triple for it.
     */
    bool empty();
    
    /**
     * @brief Checks whether there is a key for the respective node in the table.
     */
    bool find(const K&, E&) const;
    
    /**
     * @brief Inserts a new node into the UT which can be reused.
     */
    void add(const K&, const E&);
    
    /**
     * @brief Overloads the index operator for convenient access to the hash table.
     */
    std::vector<std::pair<K, E> >& operator [](size_t);
    
    size_t getSize() const;
    
    /**
     * This inner class describes a bidirectional iterator to pass through the UT. Only those nodes that hold
     * the reference value 0 are deleted, that is, they are no longer required.
     */
    class iterator
    {
    private:
        /**
         * The UT that stores and reuses nodes
         */
        UTable<K, E>* uTable;
        
        /**
         * Current value with respect to the node
         */
        size_t value;
        
        /**
         * Last value or the total number of nodes
         */
        size_t last;
    public:
        /**
         * @brief Creates an empty iterator for a UT.
         */
        iterator(UTable<K, E>* = 0, size_t = 0, size_t = 0);
        
        /**
         * @brief Describes the copy constructor that copies nodes of the UT.
         */
        iterator(const iterator&);
        
        /**
         * @brief Does not delete nodes in the UT since this is done by the UT itself (@see UTable#clear).
         */
        virtual ~iterator() = default;
        
        /**
         * @brief Iterates the UT forward.
         */
        iterator& operator ++();
        
        /**
         * @brief Iterates the UT forward by a given number.
         */
        iterator operator ++(int);
        
        /**
         * @brief Iterates the UT backward.
         */
        iterator& operator --();
        
        /**
         * @brief Iterates the UT backward by a given number.
         */
        iterator operator --(int);
        
        /**
         * @brief Assigns the values of an iterator to another iterator.
         */
        iterator& operator =(const iterator&);
        
        /**
         * @brief Checks if two iterators are equivalent.
         */
        bool operator ==(const iterator&) const;
        
        /**
         * @brief Checks if two iterators are not equal.
         */
        bool operator !=(const iterator&) const;
        
        /**
         * @brief Overloads the pointer operator to access values regarding their addresses.
         */
        std::pair<K, E>& operator *();
    };
    
    /**
     * @brief Returns the first or current valid element in the UT.
     */
    iterator begin() const;
    
    /**
     * @brief Returns the last element in the UT.
     */
    iterator end() const;
};

/**
 * This constructor creates an empty UT, that is, the size is 0 and no nodes are referenced yet.
 * If nodes are to be stored in it, it is first extended dynamically.
 *
 * @param size Size of the UT
 * @param items BDD nodes
 */
template <typename K, typename E>
UTable<K, E>::UTable() : items(0), size(0) {}

/**
 * This destructor cleans the memory for the UT. Thus, the size is set to 0 and the elements are all deleted,
 * whereby their pointers point to nothing.
 */
template <typename K, typename E>
UTable<K, E>::~UTable()
{
    clear();
}

/**
 * A key is generated with key `mod` size at the UT. The returned key makes it possible to access or
 * store nodes in the UT in O(1).
 *
 * @param key Key for which a hash code is generated.
 * @return Hash code
 */
template <typename K, typename E>
size_t UTable<K, E>::getKey(const K& key) const
{
    return (key() % size);
}

/**
 * Reserves the desired memory for the UT and resets the current UT with the nodes (if any).
 *
 * @param size Desired size for saving the nodes
 */
template <typename K, typename E>
void UTable<K, E>::load(size_t size)
{
    clear();
    this->size = size;
    items = new std::vector<std::pair<K, E> >[size];
}

/**
 * This sets the size to 0 and deletes all elements, if any currently exist
 * (at least one node refers to a valid value).
 */
template <typename K, typename E>
void UTable<K, E>::clear()
{
    size = 0;
    if (items != nullptr) {
        items = 0;
        delete[] items;
    }
}

/**
 * Checks whether there are valid nodes in the UT or whether the UT is empty
 * overall and therefore does not hold any nodes.
 *
 * @return True, if the UT is empty, otherwise False
 */
template <typename K, typename E>
bool UTable<K, E>::empty()
{
    for (int i = 0; i < size; i++)
        if (items[i].size() != 0)
            return false;
    return true;
}

/**
 * Checks a given key to see if there is already a node in the UT for this. This is
 * done by iterating over the number of nodes (in the corresponding slot).
 *
 * @param key Key
 * @param value Node
 * @return True, if a node for the given key is in the UT, otherwise False
 */
template <typename K, typename E>
bool UTable<K, E>::find(const K& key, E& value) const {
    size_t pos = getKey(key);
    size_t nodes = items[pos].size();
    for (int i = 0; i < nodes; i++) {
        if (items[pos][i].first == key) {
            value = items[pos][i].second;
            return true;
        }
    }
    return false;
}

/**
 * Inserts nodes into the UT in O(1). The hash code is generated using a modulo method so that the nodes
 * can be distributed equally. If a node already exists for the respective hash code, i. e. a collision
 * occurs, it is inserted in the list behind it.
 *
 * @param key Key
 * @param value Node
 */
template <typename K, typename E>
void UTable<K, E>::add(const K& key, const E& value)
{
    items[getKey(key)].push_back(std::pair<K, E>(key, value));
}

/**
 * This operator overload ensures easier access to the slots of the UT. For example, the iterator uses this
 * operator to easily access nodes via pointers.
 *
 * @param key Key
 * @return Already computed node
 */
template <typename K, typename E>
std::vector<std::pair<K, E> >& UTable<K, E>::operator [](size_t key)
{
    return items[key];
}

template <typename K, typename E>
size_t UTable<K, E>::getSize() const
{
    return size;
}

/**
 * This instantiates the iterator for the UT according to a start and end value.
 *
 * @param uTable UT for saving nodes
 * @param value First or current value
 * @param last Last value or total number
 */
template <typename K, typename E>
UTable<K, E>::iterator::iterator(UTable<K, E>* uTable, size_t value, size_t last) : uTable(uTable), value(value), last(last) {}

/**
 * The copy constructor copies the individual elements, i. e. table, value as well as total number
 * and creates a new iterator based on this. This makes it easy to copy multiple UTs.
 *
 * @param it Iterator
 */
template <typename K, typename E>
UTable<K, E>::iterator::iterator(const iterator& it) : uTable(it.uTable), value(it.value), last(it.last) {}

/**
 * The UT is iterated forward, it can be written as well as read.
 *
 * @return Reference to the advanced element
 */
template <typename K, typename E>
typename UTable<K, E>::iterator& UTable<K, E>::iterator::operator ++()
{
    if (uTable == 0)
        return *this;
    if (uTable->size < value)
        return *this;
    ++last;
    if (last >= (*uTable)[value].size()) {
        do {
            last = 0;
        } while ( (++value < uTable->size) && (*uTable)[value].empty() );
    }
    return *this;
}

/**
 * The UT is iterated forward by a value, it can be written as well as read.
 *
 * @return Reference to the advanced element
 */
template <typename K, typename E>
typename UTable<K, E>::iterator UTable<K, E>::iterator::operator ++(int)
{
    iterator it = *this;
    ++(*this);
    return it;
}

/**
 * The UT is iterated backward, it can be written as well as read.
 *
 * @return Reference to the advanced element
 */
template <typename K, typename E>
typename UTable<K, E>::iterator& UTable<K, E>::iterator::operator --()
{
    if (uTable == 0)
        return *this;
    if (last == 0) {
        if (value == 0)
            return *this;
        while ( (*uTable)[--value].empty() )
            if (value == 0)
                return *this;
        last = (*uTable)[value].size()--;
    }
    else
        last--;
    return *this;
}

/**
 * The UT is iterated backward by a value, it can be written as well as read.
 *
 * @return Reference to the advanced element
 */
template <typename K, typename E>
typename UTable<K, E>::iterator UTable<K, E>::iterator::operator --(int)
{
    iterator it = *this;
    --(*this);
    return it;
}

/**
 * Assigns the elements, i. e. UT, value and end value of an iterator to another
 * iterator with respect to UTs or overwrites them with it.
 *
 * @param it Iterator
 * @return Reference to the overwritten iterator
 */
template <typename K, typename E>
typename UTable<K, E>::iterator& UTable<K, E>::iterator::operator = (const iterator& it)
{
    if (&it == this)
        return *this;
    uTable = it.uTable;
    value = it.value;
    last = it.last;
    return *this;
}

/**
 * Checks if two iterators are equivalent. Two iterators are called equivalent exactly when
 * the UT and start as well as end values are identical.
 *
 * @param it Iterator
 * @return True, if the iterators are equivalent, otherwise False
 */
template <typename K, typename E>
bool UTable<K, E>::iterator::operator ==(const iterator& it) const
{
    return (uTable == it.uTable && value == it.value && last == it.last);
}

/**
 * Checks if two iterators are not equivalent. Two iterators are called not equivalent exactly when
 * the UT and start as well as end values are not identical.
 *
 * @param it Iterator
 * @return True, if the iterators are not equivalent, otherwise False
 */
template <typename K, typename E>
bool UTable<K, E>::iterator::operator !=(const iterator& it) const
{
    return !(it == *this);
}

/**
 * Overloads the pointer operator, allowing access to values within the iterator (external access).
 *
 * @return Reference to an element or node in the UT
 */
template <typename K, typename E>
std::pair<K, E>& UTable<K, E>::iterator::operator *()
{
    return (*uTable)[value][last];
}
/**
 * Points to the first or current element in the UT.
 *
 * @return First or current element
 */
template <typename K, typename E>
typename UTable<K, E>::iterator UTable<K, E>::begin() const
{
    if (items == 0)
        return end();
    size_t pos = 0;
    while ( items[pos].empty() )
        pos++;
    return iterator(const_cast<UTable<K, E>*>(this), pos, 0);
}

/**
 * Points to the last element in the UT. Thus, the iterator can limit the range (start/end).
 *
 * @return Last element or total number
 */
template <typename K, typename E>
typename UTable<K, E>::iterator UTable<K, E>::end() const {
    return iterator(const_cast<UTable<K, E>*>(this), size, 0);
}
#endif
