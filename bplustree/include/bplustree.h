#pragma once

#include <vector>
#include <memory>

//B+ Tree 
template<typename K, typename V, std::size_t ORDER = 4>
class BPlusTree {
    static_assert(ORDER >= 3, "ORDER must be at least 3");

    struct Node {
        bool isLeaf;
        std::vector<K> keys;
        std::vector<std::shared_ptr<Node>> children; // internal: size = keys+1
        std::vector<V> values;                       // leaf: size = keys
        std::shared_ptr<Node> next;                  // leaf-level forward link
        std::shared_ptr<Node> prev;                  // leaf-level backward link

        explicit Node(bool leaf) : isLeaf(leaf) {}
    };

    using NodePtr = std::shared_ptr<Node>;
    NodePtr root;

    static constexpr std::size_t MAX_KEYS = ORDER - 1;

public:
    BPlusTree() : root(std::make_shared<Node>(true)) {}

    // Single-value search
    bool search(const K& key, V& out) const {
        NodePtr node = descendToLeaf(key);
        for (std::size_t i = 0; i < node->keys.size(); ++i) {
            if (node->keys[i] == key) {
                out = node->values[i];
                return true;
            }
        }
        return false;
    }

    // Return all values for this key
    std::vector<V> searchAll(const K& key) const {
        std::vector<V> result;
        NodePtr leaf = descendToLeaf(key);

        // Find any occurrence in this leaf
        std::size_t pos = leaf->keys.size();
        for (std::size_t i = 0; i < leaf->keys.size(); ++i) {
            if (leaf->keys[i] == key) {
                pos = i;
                break;
            }
        }
        if (pos == leaf->keys.size()) {
            return result; // not present in this leaf
        }

        // Move left to earliest occurrence across leaves
        NodePtr curr = leaf;
        std::size_t idx = pos;

        // Step left within same leaf
        while (idx > 0 && curr->keys[idx - 1] == key) {
            --idx;
        }

        // Step into previous leaves if they end with same key
        while (idx == 0 && curr->prev) {
            NodePtr prevLeaf = curr->prev;
            if (prevLeaf->keys.empty() || prevLeaf->keys.back() != key) {
                break;
            }

            std::size_t j = 0;
            while (j < prevLeaf->keys.size() && prevLeaf->keys[j] != key) {
                ++j;
            }

            curr = prevLeaf;
            idx = j;
        }

        // Collect all matches going right
        for (NodePtr node = curr; node; node = node->next) {
            for (std::size_t i = (node == curr ? idx : 0); i < node->keys.size(); ++i) {
                if (node->keys[i] == key) {
                    result.push_back(node->values[i]);
                } else if (node->keys[i] > key) {
                    return result;
                }
            }
        }

        return result;
    }

    void insert(const K& key, const V& value) {
        K upKey{};
        NodePtr newChild;
        bool split = insertInternal(root, key, value, upKey, newChild);
        if (split) {
            NodePtr newRoot = std::make_shared<Node>(false);
            newRoot->keys.push_back(upKey);
            newRoot->children.push_back(root);
            newRoot->children.push_back(newChild);
            root = newRoot;
        }
    }

private:
    NodePtr descendToLeaf(const K& key) const {
        NodePtr node = root;
        while (!node->isLeaf) {
            std::size_t i = 0;
            while (i < node->keys.size() && key >= node->keys[i]) {
                ++i;
            }
            node = node->children[i];
        }
        return node;
    }

    bool insertInternal(NodePtr node,
                        const K& key,
                        const V& value,
                        K& outKey,
                        NodePtr& outNode) {
        if (node->isLeaf) {
            // Find insert position
            std::size_t pos = 0;
            while (pos < node->keys.size() && key > node->keys[pos]) {
                ++pos;
            }
            node->keys.insert(node->keys.begin() + static_cast<long>(pos), key);
            node->values.insert(node->values.begin() + static_cast<long>(pos), value);

            if (node->keys.size() <= MAX_KEYS) {
                return false; // no split
            }

            // Split leaf
            std::size_t mid = node->keys.size() / 2;
            NodePtr sibling = std::make_shared<Node>(true);

            sibling->keys.assign(node->keys.begin() + static_cast<long>(mid), node->keys.end());
            sibling->values.assign(node->values.begin() + static_cast<long>(mid), node->values.end());

            node->keys.erase(node->keys.begin() + static_cast<long>(mid), node->keys.end());
            node->values.erase(node->values.begin() + static_cast<long>(mid), node->values.end());

            // leaf links
            sibling->next = node->next;
            if (sibling->next) sibling->next->prev = sibling;
            sibling->prev = node;
            node->next = sibling;

            outKey = sibling->keys.front();
            outNode = sibling;
            return true;
        } else {
            // Internal: descend into child
            std::size_t i = 0;
            while (i < node->keys.size() && key >= node->keys[i]) {
                ++i;
            }
            NodePtr child = node->children[i];

            K newKey{};
            NodePtr newChild;
            bool split = insertInternal(child, key, value, newKey, newChild);
            if (!split) {
                return false;
            }

            // Child split; insert promoted key & pointer
            node->keys.insert(node->keys.begin() + static_cast<long>(i), newKey);
            node->children.insert(node->children.begin() + static_cast<long>(i + 1), newChild);

            if (node->keys.size() <= MAX_KEYS) {
                return false;
            }

            // Split internal
            std::size_t midIndex = node->keys.size() / 2;
            K upKey = node->keys[midIndex];

            NodePtr sibling = std::make_shared<Node>(false);

            sibling->keys.assign(node->keys.begin() + static_cast<long>(midIndex + 1), node->keys.end());
            node->keys.erase(node->keys.begin() + static_cast<long>(midIndex), node->keys.end());

            sibling->children.assign(node->children.begin() + static_cast<long>(midIndex + 1), node->children.end());
            node->children.erase(node->children.begin() + static_cast<long>(midIndex + 1), node->children.end());

            outKey = upKey;
            outNode = sibling;
            return true;
        }
    }
};
