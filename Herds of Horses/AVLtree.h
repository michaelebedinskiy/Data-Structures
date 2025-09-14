#include <iostream>

#ifndef DS_HW1_AVLTREE_H
#define DS_HW1_AVLTREE_H

template<typename T, typename Compare = std::less<T>>
class AVLTree {
public:
    struct Node {
        T data;
        Node* left;
        Node* right;
        int height;

        Node(const T &value) : data(value), left(nullptr), right(nullptr), height(1) {}
    };
private:
    Compare comp;
    Node* root;

    int getHeight(Node* node) const {
        return node ? node->height : 0;
    }

    void updateHeight(Node* node) {
        if (node) {
            node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
        }
    }

    int getBalanceFactor(Node* node) const {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    Node* rotateLeft(Node* node) {
        Node* newRoot = node->right;
        node->right = newRoot->left;
        newRoot->left = node;
        updateHeight(node);
        updateHeight(newRoot);
        return newRoot;
    }

    Node* rotateRight(Node* node) {
        Node* newRoot = node->left;
        node->left = newRoot->right;
        newRoot->right = node;
        updateHeight(node);
        updateHeight(newRoot);
        return newRoot;
    }

    Node* balance(Node* node) {
        if (!node) return nullptr;

        updateHeight(node);

        if (getBalanceFactor(node) >= 2) {
            if (getBalanceFactor(node->left) <= -1) {
                node->left = rotateLeft(node->left); //LR
            }
            node = rotateRight(node); //LL
        }

        if (getBalanceFactor(node) <= -2) {
            if (getBalanceFactor(node->right) >= 1) {
                node->right = rotateRight(node->right); //RL
            }
            node = rotateLeft(node); //RR
        }

        return node;
    }

    Node* auxInsert(Node* node, const T& value) {
        if (!node) return new Node(value);

        if (comp(value, node->data)) {
            node->left = auxInsert(node->left, value);
        } else if (comp(node->data, value)) {
            node->right = auxInsert(node->right, value);
        } else {
            return node;
        }

        return balance(node);
    }

    Node* auxRemove(Node* node, const T& value) {
        if (!node) return nullptr;

        if (comp(value, node->data)) {
            node->left = auxRemove(node->left, value);
        } else if (comp(node->data, value)) {
            node->right = auxRemove(node->right, value);
        } else {
            if (!node->left && !node->right) {
                delete node;
                return nullptr;
            } else if (!node->left) {
                Node *temp = node->right;
                delete node;
                return temp;
            } else if (!node->right) {
                Node *temp = node->left;
                delete node;
                return temp;
            } else {
                Node* temp = node->right;
                while (temp && temp->left) { //find the smallest in the right subtree
                    temp = temp->left;
                }
                node->data = temp->data;
                node->right = auxRemove(node->right, temp->data);
            }
        }
        return balance(node);
    }

    void destroyTree(Node *node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

public:
    AVLTree() : root(nullptr) {}

    ~AVLTree() {
        destroyTree(root);
    }

    void insert(const T& value) {
        root = auxInsert(root, value);
    }

    void remove(const T& value) {
        root = auxRemove(root, value);
    }

    const Node* find(const T& value) const {
        Node* current = root;
        while (current) {
            if (comp(value, current->data)) {
                current = current->left;
            } else if (comp(current->data, value)) {
                current = current->right;
            } else {
                return current;
            }
        }
        return nullptr;
    }

    Node* getRoot() const {
        return root;
    }
};


#endif //DS_HW1_AVLTREE_H