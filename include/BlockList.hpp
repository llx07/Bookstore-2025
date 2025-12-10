#ifndef BOOKSTORE_BLOCKLIST_HPP
#define BOOKSTORE_BLOCKLIST_HPP

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "MemoryRiver.hpp"

// A persistent container that can store ordered key-value pairs like std::multimap and provides
// square root time complexity for operations.
//
// Template Args:
//   Key: The type of the key. Must be comparable using operator< and be POD.
//   T: The type of the value. Must be comparable using operator< and be POD.
template <class Key, class T>
class BlockList {
public:
    // Initializes BlockList with file_name. Will create a new file if the file doesn't exist.
    void initialise(const std::string& file_name);
    // Inserts (key, value). Do nothing if (key, value) already exists.
    void insert(const Key& key, const T& value);
    // Erases (key, value). Do nothing if (key, value) doesn't exist.
    void erase(const Key& key, const T& value);
    // Returns all values with the given key. Returns empty vectors if no such value exists.
    std::vector<T> query(const Key& key);
    // Returns all values in this.
    std::vector<T> queryAll();
    std::vector<std::pair<Key, T>> queryAllKeyValuePairs();

private:
    // The maximum number of (key, value) that can be stored in a single block
    static constexpr int BLOCK_CAPACITY = 512;
    // The aggregate struct representing a key-value pair.
    struct KeyValuePair {
        Key first;
        T second;
        auto operator<=>(const KeyValuePair&) const = default;
    };
    // The data structure of a block
    struct Block {
        // ==== header part ====
        int count;      // the number of elements in the block
        int prev_head;  // the index of the previous node
        int next_head;  // the index of next node, in file 'head'
        KeyValuePair min_elem;
        KeyValuePair max_elem;
        // ====  body  part ====
        KeyValuePair data[BLOCK_CAPACITY + 1];  // one additional space for splitting
    };

    // The MemoryRiver that store the data of all blocks.
    // The first an only info stores the starting index of the first block.
    MemoryRiver<Block, 1> file;

    // Gets or sets the index of first block
    int getFirstHead() {
        int pos;
        file.getInfo(pos, 1);
        return pos;
    }
    void setFirstHead(int pos) { file.writeInfo(pos, 1); }
    // Macro for defining getters and setters for header data of a block.
#define DEFINE_GETTER_AND_SETTER(MEMBER)                               \
    void set_##MEMBER(int index, const decltype(Block::MEMBER)& val) { \
        if (!index) return;                                            \
        file.update(val, index, offsetof(Block, MEMBER));              \
    }                                                                  \
    decltype(Block::MEMBER) get_##MEMBER(int index) {                  \
        assert(index);                                                 \
        decltype(Block::MEMBER) val;                                   \
        file.read(val, index, offsetof(Block, MEMBER));                \
        return val;                                                    \
    }
    DEFINE_GETTER_AND_SETTER(count);
    DEFINE_GETTER_AND_SETTER(prev_head);
    DEFINE_GETTER_AND_SETTER(next_head);
    DEFINE_GETTER_AND_SETTER(min_elem);
    DEFINE_GETTER_AND_SETTER(max_elem);
#undef DEFINE_GETTER_AND_SETTER

    // Reads a whole block from file.
    Block getBlock(int block_id);
    // Writes a whole block to file.
    void setBlock(int block_id, const Block& block);
    // Maintains the min and max elem in head data of a block
    void updateMinMaxElem(Block& b);
    // Creates a new block and returns its index.
    int allocateNewBlock();
    // Splits half the data block_id into a new block.
    // Prerequisite: the block must have at least two elements.
    void splitBlock(int block_id);
    // Merges block id1 and id2 together.
    // Prerequisite: the two blocks must be adjacent and in order id1 -> id2.
    void mergeBlock(int id1, int id2);
    // Inserts (key, value) into block_id.
    void insertInBlock(int block_id, const Key& key, const T& value);
    // Checks if block id1 and id2 can be merged.
    bool canMerge(int id1, int id2);
    // Erases (key, value) in block_id
    void eraseInBlock(int block_id, const Key& key, const T& value);
    // Returns all elements in a block whose key is equal to key/
    std::vector<T> extractInBlock(int block_id, const Key& key);
};

template <class Key, class T>
void BlockList<Key, T>::initialise(const std::string& file_name) {
    file.initialise(file_name);
}
template <class Key, class T>
void BlockList<Key, T>::insert(const Key& key, const T& value) {
    if (!getFirstHead()) {  // the blocklist is empty
        int pos = allocateNewBlock();
        setFirstHead(pos);
        insertInBlock(pos, key, value);
        return;
    }
    auto elem = KeyValuePair{key, value};
    int block_now = getFirstHead();
    while (true) {
        if (get_count(block_now) == 0 || !get_next_head(block_now)) break;
        if (get_max_elem(block_now) >= elem) break;
        block_now = get_next_head(block_now);
    }
    insertInBlock(block_now, key, value);
    if (get_count(block_now) >= BLOCK_CAPACITY) {
        splitBlock(block_now);
    }
}
template <class Key, class T>
void BlockList<Key, T>::erase(const Key& key, const T& value) {
    if (!getFirstHead()) return;  // the block list is empty

    auto elem = KeyValuePair{key, value};
    int block_now = getFirstHead();

    while (block_now) {
        if (get_count(block_now) == 0 || get_max_elem(block_now) < elem) {
            block_now = get_next_head(block_now);
            continue;
        }
        if (get_max_elem(block_now) >= elem) {
            break;
        }
    }
    if (!block_now) return;
    // elem may be in this block
    eraseInBlock(block_now, key, value);

    int pre = get_prev_head(block_now);
    int nxt = get_next_head(block_now);

    if (pre && canMerge(pre, block_now)) {
        mergeBlock(pre, block_now);
    } else if (nxt && canMerge(block_now, nxt)) {
        mergeBlock(block_now, nxt);
    }
}
template <class Key, class T>
std::vector<T> BlockList<Key, T>::query(const Key& key) {
    std::vector<T> results;

    if (!getFirstHead()) return results;  // the block list is empty

    int block_now = getFirstHead();
    // skip useless blocks
    while (block_now) {
        if (get_count(block_now) == 0 || get_max_elem(block_now).first < key) {
            block_now = get_next_head(block_now);
            continue;
        }
        if (get_max_elem(block_now).first >= key) {
            break;
        }
    }
    if (!block_now) return results;  // find no key
    while (block_now) {
        if (get_count(block_now) == 0) {
            block_now = get_next_head(block_now);
            continue;
        }
        if (get_min_elem(block_now).first > key) break;
        auto results_in_this_block = extractInBlock(block_now, key);
        for (auto& value : results_in_this_block) {
            results.push_back(value);
        }
        block_now = get_next_head(block_now);
    }
    return results;
}
template <class Key, class T>
std::vector<T> BlockList<Key, T>::queryAll() {
    std::vector<T> results;
    if (!getFirstHead()) return results;  // the block list is empty
    int block_now = getFirstHead();
    while (block_now) {
        const auto& block = getBlock(block_now);
        for (int i = 0; i < block.count; i++) {
            results.push_back(block.data[i].second);
        }
        block_now = get_next_head(block_now);
    }
    return results;
}
template <class Key, class T>
std::vector<std::pair<Key, T>> BlockList<Key, T>::queryAllKeyValuePairs() {
    std::vector<std::pair<Key, T>> results;
    if (!getFirstHead()) return results;  // the block list is empty
    int block_now = getFirstHead();
    while (block_now) {
        const auto& block = getBlock(block_now);
        for (int i = 0; i < block.count; i++) {
            results.emplace_back(block.data[i].first, block.data[i].second);
        }
        block_now = get_next_head(block_now);
    }
    return results;
}
template <class Key, class T>
typename BlockList<Key, T>::Block BlockList<Key, T>::getBlock(int block_id) {
    Block data;
    file.read(data, block_id);
    return data;
}
template <class Key, class T>
void BlockList<Key, T>::setBlock(int block_id, const Block& block) {
    file.update(block, block_id);
}
template <class Key, class T>
void BlockList<Key, T>::updateMinMaxElem(Block& b) {
    if (!b.count) return;  // don't modify empty block
    b.min_elem = b.data[0];
    b.max_elem = b.data[b.count - 1];
}
template <class Key, class T>
int BlockList<Key, T>::allocateNewBlock() {
    int pos = file.write(Block{});
    return pos;
}
template <class Key, class T>
void BlockList<Key, T>::splitBlock(int block_id) {
    auto b1 = getBlock(block_id);
    assert(b1.count >= 2);
    int mid = b1.count / 2;

    int new_block = allocateNewBlock();
    auto b2 = getBlock(new_block);
    // copy the latter part of the data
    for (int i = mid; i < b1.count; i++) {
        b2.data[i - mid] = b1.data[i];
    }
    // update count
    b2.count = b1.count - mid;
    b1.count = mid;
    // update the chain list
    b2.next_head = b1.next_head;
    b2.prev_head = block_id;
    set_prev_head(b1.next_head, new_block);  // also work if h is the last block
    b1.next_head = new_block;
    // update the min/max elements
    updateMinMaxElem(b1);
    updateMinMaxElem(b2);
    setBlock(block_id, b1);
    setBlock(new_block, b2);
}
template <class Key, class T>
void BlockList<Key, T>::mergeBlock(int id1, int id2) {
    auto b1 = getBlock(id1);
    auto b2 = getBlock(id2);
    assert(b1.count + b2.count <= BLOCK_CAPACITY);
    for (int i = b1.count; i < b1.count + b2.count; i++) {
        b1.data[i] = b2.data[i - b1.count];
    }
    b1.count += b2.count;

    // update the chain list (only b1, because b2 will be deleted soon)
    b1.next_head = b2.next_head;
    set_prev_head(b2.next_head, id1);
    // update the min/max elements
    updateMinMaxElem(b1);
    setBlock(id1, b1);

    // remove b2,b2 from the disk
    file.erase(id2);
}
template <class Key, class T>
void BlockList<Key, T>::insertInBlock(int block_id, const Key& key, const T& value) {
    auto b = getBlock(block_id);

    auto elem = KeyValuePair{key, value};
    int pos = std::lower_bound(b.data, b.data + b.count, elem) - b.data;
    for (int i = b.count - 1; i >= pos; --i) {
        b.data[i + 1] = b.data[i];
    }
    b.data[pos] = elem;
    ++b.count;
    updateMinMaxElem(b);
    setBlock(block_id, b);
}
template <class Key, class T>
bool BlockList<Key, T>::canMerge(int id1, int id2) {
    return get_count(id1) + get_count(id2) < BLOCK_CAPACITY;
}
template <class Key, class T>
void BlockList<Key, T>::eraseInBlock(int block_id, const Key& key, const T& value) {
    auto b = getBlock(block_id);
    auto elem = KeyValuePair{key, value};
    int pos = std::lower_bound(b.data, b.data + b.count, elem) - b.data;
    if (b.data[pos] != elem) return;
    for (int i = pos; i < b.count - 1; i++) {
        b.data[i] = b.data[i + 1];
    }
    --b.count;
    updateMinMaxElem(b);
    setBlock(block_id, b);
}
template <class Key, class T>
std::vector<T> BlockList<Key, T>::extractInBlock(int block_id, const Key& key) {
    std::vector<T> results;
    auto b = getBlock(block_id);
    for (int i = 0; i < b.count; i++) {
        auto [k, v] = b.data[i];
        if (k == key) {
            results.push_back(v);
        }
    }
    return results;
}

#endif  // BOOKSTORE_BLOCKLIST_HPP
