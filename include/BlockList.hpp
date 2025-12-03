#ifndef BOOKSTORE_BLOCKLIST_HPP
#define BOOKSTORE_BLOCKLIST_HPP

#include <MemoryRiver.hpp>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

template <class Key, class T>
class Blocklist {
private:
    static constexpr int BLOCK_CAPACITY = 128;
    struct KeyValuePair {
        Key first;
        T second;
        auto operator<=>(const KeyValuePair&) const = default;
    };
    // Metadata of a block in file 'head'.
    struct HeadNode {
        int body_index;  // the index of the block in the file 'body'
        int count;       // the number of elements in the block
        int prev_head;   // the index of the previous node
        int next_head;   // the index of next node, in file 'head'
        KeyValuePair min_elem;
        KeyValuePair max_elem;
    };
    // the real data of the block, in file 'body'
    struct BodyNode {
        // one additional space for splitting block afterward
        KeyValuePair data[BLOCK_CAPACITY + 1];
    };

    MemoryRiver<HeadNode, 1> head;  // info: first head
    MemoryRiver<BodyNode, 0> body;  // no info

    int get_first_head() {
        int pos;
        head.get_info(pos, 1);
        return pos;
    }
    void set_first_head(int pos) { head.write_info(pos, 1); }

#define DEFINE_GETTER_AND_SETTER(MEMBER)                                  \
    void set_##MEMBER(int index, const decltype(HeadNode::MEMBER)& val) { \
        if (!index) return;                                               \
        head.update(val, index, offsetof(HeadNode, MEMBER));              \
    }                                                                     \
    decltype(HeadNode::MEMBER) get_##MEMBER(int index) {                  \
        assert(index);                                                    \
        decltype(HeadNode::MEMBER) val;                                   \
        head.read(val, index, offsetof(HeadNode, MEMBER));                \
        return val;                                                       \
    }

    /* getter and setter of head node */
    DEFINE_GETTER_AND_SETTER(count);
    DEFINE_GETTER_AND_SETTER(prev_head);
    DEFINE_GETTER_AND_SETTER(next_head);
    DEFINE_GETTER_AND_SETTER(min_elem);
    DEFINE_GETTER_AND_SETTER(max_elem);

    /* getter and setter of a whole block */
    std::pair<HeadNode, BodyNode> get_block(int block_id) {
        HeadNode head_node;
        head.read(head_node, block_id);
        BodyNode data;
        body.read(data, head_node.body_index);
        return {head_node, data};
    }
    void set_block(int block_id, const std::pair<HeadNode, BodyNode>& block) {
        head.update(block.first, block_id);
        body.update(block.second, block.first.body_index);
    }

    void update_minmax_elem(HeadNode& h, BodyNode& b) {
        if (!h.count) return;  // don't modify empty block
        h.min_elem = b.data[0];
        h.max_elem = b.data[h.count - 1];
    }

    int allocate_new_block() {
        int body_pos = body.write(BodyNode{});
        HeadNode init_head{};
        init_head.body_index = body_pos;
        return head.write(init_head);
    }

    // split half the data block_id into the next block
    // prerequisite: the block is not empty
    void split_block(int block_id) {
        auto [h1, b1] = get_block(block_id);
        assert(h1.count >= 2);
        int mid = h1.count / 2;

        int new_block = allocate_new_block();
        auto [h2, b2] = get_block(new_block);
        // copy the latter part of the data
        for (int i = mid; i < h1.count; i++) {
            b2.data[i - mid] = b1.data[i];
        }
        // update count
        h2.count = h1.count - mid;
        h1.count = mid;
        // update the chain list
        h2.next_head = h1.next_head;
        h2.prev_head = block_id;
        set_prev_head(h1.next_head, new_block);  // also work if h is the last block
        h1.next_head = new_block;
        // update the min/max elements
        update_minmax_elem(h1, b1);
        update_minmax_elem(h2, b2);
        set_block(block_id, {h1, b1});
        set_block(new_block, {h2, b2});
    }

    void merge_block(int id1, int id2) {
        auto [h1, b1] = get_block(id1);
        auto [h2, b2] = get_block(id2);
        assert(h1.count + h2.count <= BLOCK_CAPACITY);
        for (int i = h1.count; i < h1.count + h2.count; i++) {
            b1.data[i] = b2.data[i - h1.count];
        }
        h1.count += h2.count;

        // update the chain list (only h1, because h2 will be deleted soon)
        h1.next_head = h2.next_head;
        set_prev_head(h2.next_head, id1);
        // update the min/max elements
        update_minmax_elem(h1, b1);
        set_block(id1, {h1, b1});

        // remove h2,b2 from the disk
        body.Delete(h2.body_index);
        head.Delete(id2);
    }

    void insert_in_block(int block_id, const Key& key, const T& value) {
        // TODO(llx) binary search in the block to speed up
        auto [h, b] = get_block(block_id);
        int pos = h.count;
        auto elem = KeyValuePair{key, value};
        for (int i = 0; i < h.count; i++) {
            if (b.data[i] == elem) return;
            if (b.data[i] > elem) {
                pos = i;
                break;
            }
        }
        for (int i = h.count - 1; i >= pos; --i) {
            b.data[i + 1] = b.data[i];
        }
        b.data[pos] = elem;
        ++h.count;
        update_minmax_elem(h, b);
        set_block(block_id, {h, b});
    }

    bool can_merge(int id1, int id2) { return get_count(id1) + get_count(id2) < BLOCK_CAPACITY; }
    void erase_in_block(int block_id, const Key& key, const T& value) {
        // TODO(llx) binary search in the block to speed up
        auto [h, b] = get_block(block_id);
        auto elem = KeyValuePair{key, value};
        int pos = -1;
        for (int i = 0; i < h.count; i++) {
            if (b.data[i] == elem) {
                pos = i;
                break;
            }
        }
        if (pos == -1) return;
        for (int i = pos; i < h.count - 1; i++) {
            b.data[i] = b.data[i + 1];
        }
        --h.count;
        update_minmax_elem(h, b);
        set_block(block_id, {h, b});
    }
    // Return all elements in a block whose key is equal to key
    std::vector<T> extract_in_block(int block_id, const Key& key) {
        std::vector<T> results;
        auto [head_node, body_data] = get_block(block_id);
        for (int i = 0; i < head_node.count; i++) {
            auto [k, v] = body_data.data[i];
            if (k == key) {
                results.push_back(v);
            }
        }
        return results;
    }

public:
    void initialise(const std::string& file_name) {
        head.initialise(file_name + "_head");
        body.initialise(file_name + "_body");
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
