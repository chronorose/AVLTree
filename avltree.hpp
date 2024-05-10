#pragma once

#include <functional>
#include <iostream>
#include <optional>
#include <vector>

using namespace std;

template<typename Key, typename Val>
class AVLTree;

template<typename Key, typename Val>
struct Node {
    Key key;
    Val val;
    Node* left;
    Node* right;
    int height;
    Node(Key key, Val val): key(key), val(val), left(nullptr), right(nullptr), height(1) {}
    bool operator<(const Node<Key, Val>& other) {
        return (key < other.key) || (key == other.key && val > other.val);
    }
    bool operator>(const Node<Key, Val>& other) {
        return (key > other.key) || (key == other.key && val < other.val);
    }
    bool operator==(const Node<Key, Val>& other) {
        return (key == other.key) && (val == other.val);
    }
};

template<typename Key, typename Val>
class AVLTree {
    Node<Key, Val>* root = nullptr;

    Node<Key, Val>* balanceCheck(Node<Key, Val>* node) {
        node->height = newHeight(node);
        int balanced = balance(node);
        if (balanced >= 2) {
            if (balance(node->right) < 0) node = bigLeft(node);
            else node = leftRotate(node);
        } else if (balanced <= -2) {
            if (balance(node->left) == 1) node = bigRight(node);
            else node = rightRotate(node);
        }
        return node;
    }

    Node<Key, Val>* insertNode(Node<Key, Val>* node, Node<Key, Val>* insert_node) {
        if (node == nullptr) {
            return insert_node;
        } else if (*insert_node < *node) {
            node->left = insertNode(node->left, insert_node);
        } else if (*insert_node > *node) {
            node->right = insertNode(node->right, insert_node);
        } else if (*insert_node == *node) {
            delete insert_node;
            return node;
        }
        return balanceCheck(node);
    }

    Node<Key, Val>* deleteAux(Node<Key, Val>* node, Node<Key, Val>*& swap_node) {
        if (node == nullptr) return nullptr;
        if (node->left == nullptr) {
            node->left = swap_node->left;
            Node<Key, Val>* r_node = node->right;
            node->height = swap_node->height;
            delete swap_node;
            swap_node = node;
            return r_node;
        } else {
            node->left = deleteAux(node->left, swap_node);
        }
        return balanceCheck(node);
    }

    Node<Key, Val>* find_(Key& key, Node<Key, Val>* node) {
        if (node == nullptr) return nullptr;
        if (node->key == key) return node;
        else if (key < node->key) return find_(key, node->left);
        else return find_(key, node->right);
    }

    Node<Key, Val>* deleteNode(Node<Key, Val>* node, Node<Key, Val>& to_delete) {
        if (node == nullptr) return nullptr;
        if (*node == to_delete) {
            if (node->left == nullptr && node->right == nullptr) {
                delete node;
                return nullptr;
            } else if (node->left != nullptr && node->right == nullptr) {
                Node<Key, Val>* ret = node->left;
                delete node;
                return ret;
            } else if (node->right != nullptr && node->left == nullptr) {
                Node<Key, Val>* ret = node->right;
                delete node;
                return ret;
            } else {
                Node<Key, Val>* new_right = deleteAux(node->right, node);
                node->right = new_right;
                return balanceCheck(node);
            }
        } else if (to_delete < *node) {
            node->left = deleteNode(node->left, to_delete);
        } else {
            node->right = deleteNode(node->right, to_delete);
        }
        return balanceCheck(node);
    }


    int newHeight(Node<Key, Val>* node) {
        int holder = 0;
        if (node == nullptr) return holder;
        if (node->right != nullptr) holder += node->right->height;
        if (node->left != nullptr) holder = max(holder, node->left->height);
        return ++holder;
    }

    int balance(Node<Key, Val>* node) {
        if (node == nullptr) return 0;
        return newHeight(node->right) - newHeight(node->left);
    }

    Node<Key, Val>* rightRotate(Node<Key, Val>* node) {
        if (node == nullptr) return nullptr;
        Node<Key, Val>* l_node = node->left;
        Node<Key, Val>* lr_node = l_node->right;
        l_node->right = node;
        node->left = lr_node;
        l_node->height = newHeight(l_node);
        node->height = newHeight(node);
        return l_node;
    }
    Node<Key, Val>* leftRotate(Node<Key, Val>* node) {
        if (node == nullptr) return nullptr;
        Node<Key, Val>* r_node = node->right;
        Node<Key, Val>* rl_node = r_node->left;
        node->right = rl_node;
        r_node->left = node;
        r_node->height = newHeight(r_node);
        node->height = newHeight(node);
        return r_node;
    }

    Node<Key, Val>* bigLeft(Node<Key, Val>* node) {
        if (node == nullptr) return nullptr;
        node->right = rightRotate(node->right);
        node = leftRotate(node);
        return node;
    }
    Node<Key, Val>* bigRight(Node<Key, Val>* node) {
        if (node == nullptr) return nullptr;
        node->left = leftRotate(node->left);
        node = rightRotate(node);
        return node;
    }

    void deleteSubTree(Node<Key, Val>* node) {
        if (node == nullptr) return;
        deleteSubTree(node->left);
        deleteSubTree(node->right);
        delete node;
    }
public:
    using NodeOption = optional<reference_wrapper<Node<Key, Val>>>;
    AVLTree() {}

    NodeOption insert(Key key, Val val) {
        Node<Key, Val>* to_insert = new Node<Key, Val>(key, val);
        root = insertNode(root, to_insert);
        return *to_insert;
    }

    NodeOption find(Key& key) {
        Node<Key, Val>* node = find_(key, root);
        if (node == nullptr) return nullopt;
        else return *node;
    }

    void remove(Node<Key, Val>& node) {
        root = deleteNode(root, node);
    }
    NodeOption update(Node<Key, Val>& node, Key key, Val val) {
        remove(node);
        return insert(key, val);
    }

    ~AVLTree<Key, Val>() {
        deleteSubTree(root);
    }
};
