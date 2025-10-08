#ifndef HASHMAP_H
#define HASHMAP_H

#include <string>
#include <vector>
#include <list>
#include <functional>

using namespace std;

template <typename T>
class HashMap
{
public:
    HashMap(double max_load = 0.75); // constructor

    ~HashMap(); // destructor; deletes all of the items in the hashmap

    int size() const; // return the number of associations in the hashmap

    // The insert method associates one item (key) with another (value).
    // If no association currently exists with that key, this method inserts
    // a new association into the hashmap with that key/value pair. If there is
    // already an association with that key in the hashmap, then the item
    // associated with that key is replaced by the second parameter (value).
    // Thus, the hashmap must contain no duplicate keys.
    void insert(const std::string& key, const T& value);

    // Defines the bracket operator for HashMap, so you can use your map like this:
    // your_map["david"] = 2.99;
    // If the key does not exist in the hashmap, this will create a new entry in
    // the hashmap and map it to the default value of type T (0 for builtin types).
    // It returns a reference to the newly created value in the map.
    T& operator[](const std::string& key);

    // If no association exists with the given key, return nullptr; otherwise,
    // return a pointer to the value associated with that key. This pointer can be
    // used to examine that value within the map.
    const T* find(const std::string& key) const;

    // If no association exists with the given key, return nullptr; otherwise,
    // return a pointer to the value associated with that key. This pointer can be
    // used to examine that value or modify it directly within the map.
    T* find(const std::string& key) {
        const auto& hm = *this;
        return const_cast<T*>(hm.find(key));
    }

private:
    struct Node {
        Node(const string key, const T& value)
        : m_key(key), m_value(value) {}

        string m_key;
        T m_value;
    };

    vector<list<Node>> m_buckets;
    int numItems;
    double m_loadFactor;

    size_t getBucketIndex(const string key) const;
    void rehash();

};


// PRIVATE IMPLEMENTATIONS

// BUCKET INDEX
template <typename T>
size_t HashMap<T>::getBucketIndex(const std::string key) const {
    size_t bucketIndex = std::hash<string>()(key) % m_buckets.size();
    return bucketIndex;
}


// REHASH
template <typename T>
void HashMap<T>::rehash() {
    vector<list<Node>> prevBuckets = m_buckets;
    int prevSize = prevBuckets.size();

    m_buckets.clear();
    numItems = 0;
    m_buckets.resize(prevBuckets.size() * 2);

    for (size_t i = 0; i < prevSize; i++) {
        for (typename list<Node>::iterator p = prevBuckets[i].begin(); p != prevBuckets[i].end(); p++) {
            insert(p->m_key, p->m_value);
        }
    }
}


// CONSTRUCTOR
template <typename T>
HashMap<T>::HashMap(double max_load)
: numItems(0), m_loadFactor(max_load) {
    m_buckets.resize(10);
}


// DESTRUCTOR
template <typename T>
HashMap<T>::~HashMap() {
    m_buckets.clear();
}


// SIZE
template <typename T>
int HashMap<T>::size() const {
    return numItems;
}


// INSERT
template <typename T>
void HashMap<T>::insert(const std::string &key, const T &value) {
    size_t bucketIndex = getBucketIndex(key);

    for (typename list<Node>::iterator p = m_buckets[bucketIndex].begin(); p != m_buckets[bucketIndex].end(); p++) {
        if (p->m_key == key) {
            p->m_value = value;
            return;
        }
    }

    Node newNode(key, value);
    m_buckets[bucketIndex].push_front(newNode);
    numItems++;

    if (static_cast<double>(numItems) / m_buckets.size() > m_loadFactor) {
        rehash();
    }

}


// [] OPERATOR
template <typename T>
T &HashMap<T>::operator[](const std::string &key) {
    size_t bucketIndex = getBucketIndex(key);


    for (typename list<Node>::iterator p = m_buckets[bucketIndex].begin(); p != m_buckets[bucketIndex].end(); p++) {
        if (p->m_key == key) {
            return p->m_value;
        }
    }

    insert(key, T());

    bucketIndex = getBucketIndex(key);
    for (typename list<Node>::iterator p = m_buckets[bucketIndex].begin(); p != m_buckets[bucketIndex].end(); p++) {
        if (p->m_key == key) {
            return p->m_value;
        }
    }

    return m_buckets[bucketIndex].begin()->m_value;
}


// FIND
template <typename T>
const T *HashMap<T>::find(const std::string &key) const {
    size_t bucketIndex = getBucketIndex(key);
    for (typename list<Node>::const_iterator p = m_buckets[bucketIndex].begin(); p != m_buckets[bucketIndex].end(); p++) {
        if (p->m_key == key)
            return &p->m_value;
    }
    return nullptr;
}


#endif //HASHMAP_H


