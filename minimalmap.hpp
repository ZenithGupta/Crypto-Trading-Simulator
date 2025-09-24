#ifndef MINIMAL_MAP_HPP
#define MINIMAL_MAP_HPP

#include <stdexcept>
#include "MinimalList.hpp"

template <typename Key, typename Value>
class MinimalMap {
private:
    struct Node {
        Key key;
        Value value;
        Node* left;
        Node* right;

        Node(const Key& k, const Value& v) : key(k), value(v), left(nullptr), right(nullptr) {}
    };

    Node* root;
    int node_count;

    Node* insert_recursive(Node* node, const Key& key, const Value& value) {
        if (node == nullptr) {
            node_count++;
            return new Node(key, value);
        }

        if (key < node->key) {
            node->left = insert_recursive(node->left, key, value);
        } else if (key > node->key) {
            node->right = insert_recursive(node->right, key, value);
        } else {
            node->value = value;
        }
        return node;
    }

    Value* find_recursive(Node* node, const Key& key) const {
        if (node == nullptr) {
            return nullptr;
        }

        if (key == node->key) {
            return &node->value;
        }

        if (key < node->key) {
            return find_recursive(node->left, key);
        } else {
            return find_recursive(node->right, key);
        }
    }

    void get_keys_in_order(Node* node, MinimalList<Key>& keys) const {
        if (node == nullptr) {
            return;
        }
        get_keys_in_order(node->left, keys);
        keys.push_back(node->key);
        get_keys_in_order(node->right, keys);
    }
    
    Node* find_min(Node* node) {
        while(node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    Node* remove_recursive(Node* node, const Key& key) {
        if (node == nullptr) return nullptr;

        if (key < node->key) {
            node->left = remove_recursive(node->left, key);
        } else if (key > node->key) {
            node->right = remove_recursive(node->right, key);
        } else {
            if (node->left == nullptr) {
                Node* temp = node->right;
                delete node;
                node_count--;
                return temp;
            } else if (node->right == nullptr) {
                Node* temp = node->left;
                delete node;
                node_count--;
                return temp;
            }

            Node* temp = find_min(node->right);
            node->key = temp->key;
            node->value = temp->value;
            node->right = remove_recursive(node->right, temp->key);
        }
        return node;
    }

public:
    MinimalMap() : root(nullptr), node_count(0) {}

    void insert(const Key& key, const Value& value) {
        root = insert_recursive(root, key, value);
    }

    Value* find(const Key& key) const {
        return find_recursive(root, key);
    }

    void remove(const Key& key) {
        root = remove_recursive(root, key);
    }

    MinimalList<Key> get_keys() const {
        MinimalList<Key> keys;
        get_keys_in_order(root, keys);
        return keys;
    }

    int size() const {
        return node_count;
    }
};

#endif