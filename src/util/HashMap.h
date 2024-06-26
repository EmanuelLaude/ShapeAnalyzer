#ifndef ShapeAnalyzer_HashMap_h
#define ShapeAnalyzer_HashMap_h

#include <unordered_map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <utility>

using namespace std;

namespace util {

template <class KEY, class VALUE>

///
/// \brief HashMap is a wrapper class for unordered_map with some extra features,
/// e.g. a proper contains function and a random subset of the map can be generated.
/// \details A HashMap<KEY, VALUE> maps objects from type KEY to objects from type value.
/// It manages a unordered_map<KEY, VALUE> object and has some special features. The first one
/// is that the [] operator does not add keys to the hash map when you try to access them
/// although they did not exist. Additionally you can get random subsets of the elements, keys or values.
/// @tparam KEY type used as the key in the hash map
/// @tparam VALUE type of the mapped values
/// \author Emanuel Laude and Zorah Lähner
///
class HashMap {
public:
    typedef typename unordered_map<KEY, VALUE>::const_iterator const_iterator;
    typedef typename unordered_map<KEY, VALUE>::iterator iterator;
    
    /// @{
    /// \brief Empty Constructor.
    HashMap();
    /// \brief Reserves memory for n elements.
    HashMap(int n);
    /// \brief Copy Constructor.
    HashMap(const HashMap<KEY, VALUE>&);
    /// \brief Maps all keys to the given value.
    HashMap(const vector<KEY>&, const VALUE&);
    /// \brief Empty Destructor.
    ~HashMap() {}
    ///@}
    
    /// @{
    /// \brief Inserts the value with the given key.
    /// \details If the key already exists the corresponding value is replaced.
    void insert(const KEY&, const VALUE&);
    /// \brief Maps all given keys to the given value.
    /// \details If any of the keys already exist the corresponding value is replaced.
    void insert(const vector<KEY>&, const VALUE&);
    /// \brief Inserts all key plus value pairs.
    /// \details If any of the keys already exist the corresponding value is replaced.
    void insert(const vector<pair<KEY, VALUE> >&);
    /// @}
    
    /// @{
    /// \brief If the key exists, its corresponding value is removed from the hash map.
    bool remove(const KEY&);
    /// \brief Calls remove(KEY) on every element in the vector.
    bool remove(const vector<KEY>&);
    /// \brief Clears the whole hash map.
    void clear();
    /// @}
    
    /// @{
    /// \brief Returns iterator pointing to the first element of the hashmap.
    const_iterator begin() const;
    /// \brief Returns iterator pointing to the end of the hash map.
    const_iterator end() const;

    /// \brief Returns iterator pointing to the first element of the hashmap.
    iterator begin();
    /// \brief Returns iterator pointing to the end of the hash map.
    iterator end();
    /// @}
    
    /// \brief Returns if the given key exists in the hash map.
    /// \details This will not cause the key to be added to the hash map.
    /// @return true if key exists, false otherwise
    bool containsKey(const KEY&) const;
    /// \brief Returns the value corresponding to the key.
    /// \details If the key does not exist, the result will be a null pointer.
    /// This will not cause the key to be added to the hash map.
    /// This expression is not assignable since it is constant.
    /// If it is not guaranteed that the KEY exists in the map check for existence of the key using containsKey() first otherwise an exception of type std::out_of_range is thrown.
    /// @return pointer to the value corresponding to the key.
    const VALUE& operator[](const KEY&) const;

    /// This will not cause the key to be added to the hash map.
    /// This expression is assignable since it is a reference to the value.
    /// If it is not guaranteed that the KEY exists in the map check for existence of the key using containsKey() first otherwise an exception of type std::out_of_range is thrown.
    /// @return pointer to the value corresponding to the key.
    VALUE& operator[](const KEY&);
    
    /// \brief Returns the number of elements in the hash map.
    unsigned size() const;
    
    /// \brief Stores a random subset of size 'size' in the given hash map.
    /// \details Keys that were in the given hashmap before are not deleted, but might get a new value.
    /// If size is larger than the number of elements all elements are returned.
    /// Reserving enough memory for the subset beforehand will speed up the process.
    /// @param size size of the random subset, if it is larger than the size of the hash map, all values are returned
    /// @param hashmap pointer to valid hashmap object to which the subset will be added
    void getRandomSample(unsigned size, HashMap<KEY, VALUE>& hashmap) const;
    /// \brief Stores a random subset of size 'size' of the keys in the given vector.
    /// \details Elements that were in the vector before are not deleted.
    /// If size is larger than the number of elements all keys are returned.
    /// Reserving enough memory for the subset beforehand will speed up the process.
    /// @param size size of the random subset, if it is larger than the size of the hash map, all keys are returned
    /// @param vector pointer to valid vector object to which the subset will be added
    void getRandomSampleKeys(unsigned size, vector<KEY>& vector) const;
    /// \brief Stores a random subset of size 'size' of the values in the given vector.
    /// \details Elements that were in the vector before are not deleted.
    /// If size is larger than the number of elements all values are returned.
    /// Reserving enough memory for the subset beforehand will speed up the process.
    /// @param size size of the random subset, if it is larger than the size of the hash map, all values are returned
    /// @param vector pointer to valid vector object to which the subset will be added
    void getRandomSampleValues(unsigned size, vector<VALUE>& vector) const;
    
    /// \brief Returns a readonly const reference to the underlying unordered_map.
    const unordered_map<KEY, VALUE>& getEntries() const;

    /// \brief Returns a writeable reference to the underlying unordered_map.
    unordered_map<KEY, VALUE>& getEntries();
    
    /// \brief Stores all values in the given vector.
    /// \details The result is not checked for double entries.
    void getValues(vector<VALUE>&) const;
    /// \brief Stores all keys in the given vector.
    void getKeys(vector<KEY>&) const;
    
private:
    unordered_map<KEY, VALUE> entries_;
    
};

///////////////////////////////////////////////////////////////////////////////
// Constructors
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
HashMap<KEY, VALUE>::HashMap() {
    entries_ = unordered_map<KEY, VALUE>(100);
}


///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
HashMap<KEY, VALUE>::HashMap(int n) {
    entries_ = unordered_map<KEY, VALUE>(n);
}

///////////////////////////////////////////////////////////////////////////////
// copy contructor
template<class KEY, class VALUE>
HashMap<KEY, VALUE>::HashMap(const HashMap<KEY, VALUE>& map) {
    entries_ = unordered_map<KEY, VALUE>(map.getEntries());
}


///////////////////////////////////////////////////////////////////////////////
// all keys are added with the same value
template<class KEY, class VALUE>
HashMap<KEY, VALUE>::HashMap(const vector<KEY>& keys, const VALUE& value) {
    entries_ = unordered_map<KEY, VALUE>(keys.size());
    for(auto key : keys) {
        entries_.insert(make_pair(key, value));
    }
}


///////////////////////////////////////////////////////////////////////////////
// Adding and Deleting Correspondences
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
void HashMap<KEY, VALUE>::insert(const KEY& key, const VALUE& value) {
    entries_.insert(make_pair(key, value));
}


///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
void HashMap<KEY, VALUE>::insert(const vector<KEY>& keys, const VALUE& value) {
    // reserve enough memory for new keys
    entries_.rehash(entries_.size() + keys.size());
    
    for (auto it = keys.begin(); it != keys.end(); it++) {
        entries_.insert(make_pair<KEY, VALUE>(*it, value));
    }
}


///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
void HashMap<KEY, VALUE>::insert(const vector<pair<KEY, VALUE> >& entries) {
    // reserve enough memory for new keys
    entries_.rehash(entries_.size() + entries.size());
    
    for (auto it = entries.begin(); it != entries.end(); it++) {
        entries_.insert(*it);
    }
}


///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
bool HashMap<KEY, VALUE>::remove(const KEY& key) {
    return entries_.erase(key) != 0;
}

///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
bool HashMap<KEY, VALUE>::remove(const vector<KEY>& keys) {
    bool success = true;
    for (auto it = keys.begin(); it != keys.end(); it++) {
        if (!remove(*it)) {
            success = false;
        }
    }
    return success;
}


///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
void HashMap<KEY, VALUE>::clear() {
    entries_.clear();
}

///////////////////////////////////////////////////////////////////////////////
// Getters
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
unsigned HashMap<KEY, VALUE>::size() const {
    return entries_.size();
}


///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
typename HashMap<KEY, VALUE>::iterator HashMap<KEY, VALUE>::begin() {
    return entries_.begin();
}


///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
typename HashMap<KEY, VALUE>::iterator HashMap<KEY, VALUE>::end() {
    return entries_.end();
}

///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
typename HashMap<KEY, VALUE>::const_iterator HashMap<KEY, VALUE>::begin() const {
    return entries_.begin();
}


///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
typename HashMap<KEY, VALUE>::const_iterator HashMap<KEY, VALUE>::end() const {
    return entries_.end();
}

///////////////////////////////////////////////////////////////////////////////
// returns true if the input was found in the map, false otherwise
template<class KEY, class VALUE>
bool HashMap<KEY, VALUE>::containsKey(const KEY& key) const {
    return entries_.find(key) != entries_.end();
}

///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
const unordered_map<KEY, VALUE>& HashMap<KEY, VALUE>::getEntries() const {
    return entries_;
}

///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
unordered_map<KEY, VALUE>& HashMap<KEY, VALUE>::getEntries() {
    return entries_;
}

///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
const VALUE& HashMap<KEY, VALUE>::operator[](const KEY& key) const {
    return entries_.at(key);
}

///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
VALUE& HashMap<KEY, VALUE>::operator[](const KEY& key) {
    return entries_.at(key);
}

template<class KEY, class VALUE>
void HashMap<KEY, VALUE>::getValues(vector<VALUE>& values) const {
    for(auto entry : entries_) {
        values.push_back(entry.second);
    }
}

template<class KEY, class VALUE>
void HashMap<KEY, VALUE>::getKeys(vector<KEY>& keys) const {
    for(auto entry : entries_) {
        keys.push_back(entry.first);
    }
}

///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
void HashMap<KEY, VALUE>::getRandomSample(unsigned size, HashMap<KEY, VALUE>& sampleMap) const {
    
    // if size is larger than the number elements in the map, all keys are
    // returned
    if (size >= this->size()) {
        
        // copy this map
        for (auto it = begin(); it != end(); it++) {
            sampleMap.add(it->first, it->second);
        }
        
    } else { // size smaller that number of entries
        vector<unsigned int> permutation(entries_.size());
        for(int i = 0; i < entries_.size(); i++) {
            permutation[i] = i;
        }
        random_shuffle(permutation.begin(), permutation.end());
        
        // obtain an ordered sequence of all keys
        vector<KEY> keys;
        getKeys(keys);
        
        // write size keys of random order into subsetKeys
        for (int i = 0; i < size; i++) {
            sampleMap.add(keys[permutation[i]], entries_[keys[permutation[i]]]);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
void HashMap<KEY, VALUE>::getRandomSampleKeys(unsigned size, vector<KEY>& sampleKeys) const {
    
    // if size is larger than the number elements in the map, all keys are
    // returned
    if (size >= this->size()) {
        
        // copy this map
        for (auto it = begin(); it != end(); it++) {
            sampleKeys.push_back(it->first);
        }
        
    } else { // size smaller that number of entries
        
        vector<unsigned int> permutation(entries_.size());
        for(int i = 0; i < entries_.size(); i++) {
            permutation[i] = i;
        }
        random_shuffle(permutation.begin(), permutation.end());
        
        // obtain an ordered sequence of all keys
        vector<KEY> keys;
        getKeys(keys);
        
        // write size keys of random order into subsetKeys
        for (int i = 0; i < size; i++) {
            sampleKeys.push_back(keys[permutation[i]]);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
template<class KEY, class VALUE>
void HashMap<KEY, VALUE>::getRandomSampleValues(unsigned size, vector<VALUE>& sampleValues) const {
    
    // if size is larger than the number elements in the map, all keys are
    // returned
    if (size >= this->size()) {
        
        // copy this map
        for (auto it = begin(); it != end(); it++) {
            sampleValues.push_back(it->second);
        }
        
    } else { // size smaller that number of entries
        vector<unsigned int> permutation(entries_.size());
        for(int i = 0; i < entries_.size(); i++) {
            permutation[i] = i;
        }
        random_shuffle(permutation.begin(), permutation.end());
        
        // obtain an ordered sequence of all keys
        vector<KEY> keys;
        getKeys(keys);
        
        // write size keys of random order into subsetKeys
        for (int i = 0; i < size; i++) {
            sampleValues.push_back(entries_[keys[permutation[i]]]);
        }
    }
}

}

#endif
