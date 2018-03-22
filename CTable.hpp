/**
 * @file CTable.hpp
 * @author Rune Krauss
 *
 * The computed table (CT) is used as a cache to reduce the computing effort or storage space
 * of the ITE algorithm (@see Manager#ite). This means that isomorphic subgraphs do not have to
 * be viewed more often. Results that have already been computed are stored in the CT in order
 * to allow new accesses in constant time. To achieve this, a hash function is selected which uses
 * the multiplication method. In contrast to the unique table, entries are not stored until they are
 * deleted since this does not have to guarantee canonicity. As a result, there are no searches in this
 * regard and entries are simply overwritten, i. e. there is no collision strategy. Furthermore, the CT
 * is dynamically created which further saves storage space.
 */
#ifndef CTable_hpp
#define CTable_hpp

/**
 * This class represents the cache in the form of a hash table of this library and is intended to avoid
 * redundant computings during synthesis.
 */
template<typename K, typename E>
class CTable
{
private:
    /**
     * A pair identifies a mapping of keys to nodes. The key is composed using a
     * multiplication procedure (@see TableKey).
     */
    std::pair<K, E>* items;
    
    /**
     * Size of the cache in the CT. This should - for an optimal runtime - be able to
     * hold approximately 500,000 entries. It should also be a prime number for the modulo process.
     */
    size_t size;
    
    /**
     * @brief Returns a generated key that gives access to nodes.
     */
    size_t getKey(const K&) const;
public:
    /**
     * @brief Initializes a CT with size 0.
     */
    CTable();
    
    /**
     * @brief Initializes a CT with a given size.
     */
    CTable(const size_t);
    
    /**
     * @brief Resets a CT to the factory settings.
     */
    virtual ~CTable();
    
    /**
     * @brief Loads the cache according to a certain size.
     */
    void load(const size_t);
    
    /**
     * Resets a CT to its default settings.
     */
    void clear();
    
    /**
     * @brief Checks whether there is a node for a particular key or whether there
     * has already been a computing of this node.
     */
    bool hasNext(const K&, E&) const;
    
    /**
     * @brief Writes a computed node with a key to the cache. If the key is already in the cache,
     * the node will be overwritten.
     */
    void insert(const K&, const E&);
    
    /**
     * @brief Returns True if there are no nodes in the CT.
     */
    bool empty();
    
    /**
     * @brief Overloads the index operator for convenient access to the cache.
     */
    std::pair<K, E>& operator [](const size_t);
    
    size_t getSize() const;
};

/**
 * A key is generated with key `mod` size of the CT. This determined value is used
 * as a key to find or access nodes.
 *
 * @param key Key for which a hash code is generated
 * @return Hash code
 */
template <typename K, typename E>
size_t CTable<K, E>::getKey(const K& key) const
{
    return (key() % size);
}

/**
 * Initializes a CT with default values. Thus, the size is 0 and there is no node in the cache yet.
 */
template<typename K, typename E>
CTable<K, E>::CTable() : items(0), size(0) {}

/**
 * Cleans an object of the CT, i. e. the size is set to 0 and if cache memory has been allocated, it will be cleaned.
 */
template<typename K, typename E>
CTable<K, E>::~CTable()
{
    clear();
}

/**
 * Loads the cache based on a certain size, i. e. the current CT is reset or overwritten with the new data.
 * This method is called mainly when the manager is initialized.
 *
 * @param size Size of the cache
 */
template<typename K, typename E>
CTable<K, E>::CTable(const size_t size)
{
    load(size);
}

/**
 * Loads the cache according to a certain size.
 *
 * @param size Desired size
 */
template<typename K, typename E>
void CTable<K, E>::load(const size_t size)
{
    clear();
    this->size = size;
    items = new std::pair<K, E>[size];
}

/**
 * Resets a CT to its default settings. This sets the size to 0. If memory has been allocated in
 * relation to the cache, this will be cleaned. Afterwards, the null pointer applies, i.e. nothing
 * more is referenced.
 */
template<typename K, typename E>
void CTable<K, E>::clear()
{
    size = 0;
    if (items != nullptr) {
        delete[] items;
        items = nullptr;
    }
}

/**
 * Determines directly from the search value the index of the data set in which the nodes are located.
 * The location of the nodes is computed using a multiplication method (@see TableKey). Only if there
 * is a cache miss, false will be returned.
 *
 * @param key Key
 * @param node Corresponding node
 * @return True, if the node is already in the table, otherwise False
 */
template<typename K, typename E>
bool CTable<K, E>::hasNext(const K& key, E& node) const
{
    size_t pos = getKey(key);
    if (key == items[pos].first) {
        node = items[pos].second;
        return true;
    }
    return false;
}

/**
 * Writes a computed node to the cache. The location of the node is determined using a multiplication
 * method (@see TableKey). First, the key is saved, then the corresponding node. If the key already exists,
 * the node will be overwritten.
 *
 * @param key Key
 * @param node Corresponding node
 */
template<typename K, typename E>
void CTable<K, E>::insert(const K& key, const E& node)
{
    size_t pos = getKey(key);
    items[pos].first = key;
    items[pos].second = node;
}

/**
 * Checks whether nodes are in the CT or whether the CT is empty overall and therefore does not hold any nodes.
 *
 * @return True, if the CT is empty, otherwise False
 */
template <typename K, typename E>
bool CTable<K, E>::empty()
{
    return (size == 0 ? true : false);
}

/**
 * Overloads the operator [] so that the cache can be treated as an array. Thus, a node can be assigned directly
 * via a code in this operator.
 *
 * @param pos Position in the table where the respective node is located
 */
template<typename K, typename E>
std::pair<K, E>& CTable<K, E>::operator [] (const size_t pos)
{
    return items[pos];
}

template <typename K, typename E>
size_t CTable<K, E>::getSize() const
{
    return size;
}
#endif
