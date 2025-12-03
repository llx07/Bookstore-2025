#ifndef BOOKSTORE_BLOCKLIST_HPP
#define BOOKSTORE_BLOCKLIST_HPP

#include <pstl/algorithm_fwd.h>

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "MemoryRiver.hpp"

template <class Key, class T>
class Blocklist {
private:
    static constexpr int BLOCK_CAPACITY = 512;
    struct KeyValuePair {
        Key first;
        T second;
        auto operator<=>(const KeyValuePair&) const = default;
    };
    struct Block {
        int count;       // the number of elements in the block
        int prev_head;   // the index of the previous node
        int next_head;   // the index of next node, in file 'head'
        KeyValuePair min_elem;
        KeyValuePair max_elem;
        // one additional space for splitting block afterward
        KeyValuePair data[BLOCK_CAPACITY + 1];
    };
    // the real data of the block, in file 'body'

    MemoryRiver<Block, 1> file;  // info: first block

    int get_first_head() {
        int pos;
        file.get_info(pos, 1);
        return pos;
    }
    void set_first_head(int pos) { file.write_info(pos, 1); }

#define DEFINE_GETTER_AND_SETTER(MEMBER)                                  \
    void set_##MEMBER(int index, const decltype(Block::MEMBER)& val) { \
        if (!index) return;                                               \
        file.update(val, index, offsetof(Block, MEMBER));              \
    }                                                                     \
    decltype(Block::MEMBER) get_##MEMBER(int index) {                  \
        assert(index);                                                    \
        decltype(Block::MEMBER) val;                                   \
        file.read(val, index, offsetof(Block, MEMBER));                \
        return val;                                                       \
    }

    /* getter and setter of head node */
    DEFINE_GETTER_AND_SETTER(count);
    DEFINE_GETTER_AND_SETTER(prev_head);
    DEFINE_GETTER_AND_SETTER(next_head);
    DEFINE_GETTER_AND_SETTER(min_elem);
    DEFINE_GETTER_AND_SETTER(max_elem);

    /* getter and setter of a whole block */
    Block get_block(int block_id) {
        Block data;
        file.read(data, block_id);
        return data;
    }
    void set_block(int block_id, const Block& block) {
        file.update(block, block_id);
    }

    void update_minmax_elem(Block& b) {
        if (!b.count) return;  // don't modify empty block
        b.min_elem = b.data[0];
        b.max_elem = b.data[b.count - 1];
    }

    int allocate_new_block() {
        int pos = file.write(Block{});
        return pos;
    }

    // split half the data block_id into the next block
    // prerequisite: the block is not empty
    void split_block(int block_id) {
        auto b1 = get_block(block_id);
        assert(b1.count >= 2);
        int mid = b1.count / 2;

        int new_block = allocate_new_block();
        auto b2 = get_block(new_block);
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
        update_minmax_elem(b1);
        update_minmax_elem(b2);
        set_block(block_id, b1);
        set_block(new_block, b2);
    }

    void merge_block(int id1, int id2) {
        auto b1 = get_block(id1);
        auto b2 = get_block(id2);
        assert(b1.count + b2.count <= BLOCK_CAPACITY);
        for (int i = b1.count; i < b1.count + b2.count; i++) {
            b1.data[i] = b2.data[i - b1.count];
        }
        b1.count += b2.count;

        // update the chain list (only b1, because b2 will be deleted soon)
        b1.next_head = b2.next_head;
        set_prev_head(b2.next_head, id1);
        // update the min/max elements
        update_minmax_elem(b1);
        set_block(id1, b1);

        // remove b2,b2 from the disk
        file.Delete(id2);
    }

    void insert_in_block(int block_id, const Key& key, const T& value) {
        auto b = get_block(block_id);

        auto elem = KeyValuePair{key, value};
        int pos = std::lower_bound(b.data, b.data + b.count, elem) - b.data;
        for (int i = b.count - 1; i >= pos; --i) {
            b.data[i + 1] = b.data[i];
        }
        b.data[pos] = elem;
        ++b.count;
        update_minmax_elem(b);
        set_block(block_id, b);
    }

    bool can_merge(int id1, int id2) { return get_count(id1) + get_count(id2) < BLOCK_CAPACITY; }
    void erase_in_block(int block_id, const Key& key, const T& value) {
        auto b = get_block(block_id);
        auto elem = KeyValuePair{key, value};
        int pos = std::lower_bound(b.data, b.data + b.count, elem) - b.data;
        if (b.data[pos] != elem) return;
        for (int i = pos; i < b.count - 1; i++) {
            b.data[i] = b.data[i + 1];
        }
        --b.count;
        update_minmax_elem(b);
        set_block(block_id, b);
    }
    // Return all elements in a block whose key is equal to key
    std::vector<T> extract_in_block(int block_id, const Key& key) {
        std::vector<T> results;
        auto b = get_block(block_id);
        for (int i = 0; i < b.count; i++) {
            auto [k, v] = b.data[i];
            if (k == key) {
                results.push_back(v);
            }
        }
        return results;
    }

public:
    void initialise(const std::string& file_name) {
        file.initialise(file_name);
    }
    void insert(const Key& key, const T& value) {
        if (!get_first_head()) {  // the blocklist is empty
            int pos = allocate_new_block();
            set_first_head(pos);
            insert_in_block(pos, key, value);
            return;
        }
        auto elem = KeyValuePair{key, value};
        int block_now = get_first_head();
        while (true) {
            if (get_count(block_now) == 0 || !get_next_head(block_now)) break;
            if (get_max_elem(block_now) >= elem) break;
            block_now = get_next_head(block_now);
        }
        insert_in_block(block_now, key, value);
        if (get_count(block_now) >= BLOCK_CAPACITY) {
            split_block(block_now);
        }
    }
    void erase(const Key& key, const T& value) {
        if (!get_first_head()) return;  // the block list is empty

        auto elem = KeyValuePair{key, value};
        int block_now = get_first_head();

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
        erase_in_block(block_now, key, value);

        int pre = get_prev_head(block_now);
        int nxt = get_next_head(block_now);

        if (pre && can_merge(pre, block_now)) {
            merge_block(pre, block_now);
        } else if (nxt && can_merge(block_now, nxt)) {
            merge_block(block_now, nxt);
        }
    }
    std::vector<T> query(const Key& key) {
        std::vector<T> results;

        if (!get_first_head()) return results;  // the block list is empty

        int block_now = get_first_head();
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
            auto results_in_this_block = extract_in_block(block_now, key);
            for (auto& value : results_in_this_block) {
                results.push_back(value);
            }
            block_now = get_next_head(block_now);
        }
        return results;
    }
};

#endif  // BOOKSTORE_BLOCKLIST_HPP
