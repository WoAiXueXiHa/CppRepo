// 我要写一个红黑树，要求能够进行插入、删除、查找、遍历等操作
#include <iostream>
#include <functional>

using namespace std;
using namespace std::placeholders;  // 占位符

enum Color { RED, BLACK };

struct Node {
    int value;
    Color color;
    Node* left;
    Node* right;
    Node* parent;
    
    Node(int val) : value(val), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

class RedBlackTree {
    private:
    Node* root;
    
    void leftRotate(Node* node) {
        Node* right_child = node->right;
        node->right = right_child->left;
        if (right_child->left != nullptr) {
            right_child->left->parent = node;
        }
        right_child->parent = node->parent;
        if (node->parent == nullptr) {
            root = right_child;
        } else if (node == node->parent->left) {
            node->parent->left = right_child;
        } else {
            node->parent->right = right_child;
        }
        right_child->left = node;
        node->parent = right_child;
    }
    
    void rightRotate(Node* node) {
        Node* left_child = node->left;
        node->left = left_child->right;
        if (left_child->right != nullptr) {
            left_child->right->parent = node;
        }
        left_child->parent = node->parent;
        if (node->parent == nullptr) {
            root = left_child;
        } else if (node == node->parent->right) {
            node->parent->right = left_child;
        } else {
            node->parent->left = left_child;
        }
        left_child->right = node;
        node->parent = left_child;
    }
    
    void traverse(Node* node) {
        if (node == nullptr) return;
        traverse(node->left);
        cout << node->value << "(" << (node->color == RED ? "R" : "B") << ") ";
        traverse(node->right);
    }
    
    public:
    RedBlackTree() {
        root = nullptr;
    }
    void insert(int value) {
        if (root == nullptr) {
            root = new Node(value);
            root->color = BLACK;
            return;
        }
        Node* node = root;
        while (node != nullptr) {
            if (value < node->value) {
                if (node->left == nullptr) {
                    node->left = new Node(value);
                    node->left->parent = node;
                    node->left->color = RED;
                    break;
                }
                node = node->left;
            } else {
                if (node->right == nullptr) {
                    node->right = new Node(value);
                    node->right->parent = node;
                    node->right->color = RED;
                    break;
                }
                node = node->right;
            }
        }
        fixInsert(node);
    }
    void fixInsert(Node* node) {
        while (node->parent != nullptr && node->parent->color == RED) {
            if (node->parent == node->parent->parent->left) {
                Node* uncle = node->parent->parent->right;
                if (uncle != nullptr && uncle->color == RED) {
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    node->parent->parent->color = RED;
                    node = node->parent->parent;
                } else {
                    if (node == node->parent->right) {  
                        leftRotate(node->parent);
                        node = node->left;
                    }
                    node->parent->color = BLACK;
                    node->parent->parent->color = RED;
                    rightRotate(node->parent->parent);
                }
            } else {
                Node* uncle = node->parent->parent->left;
                if (uncle != nullptr && uncle->color == RED) {
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    node->parent->parent->color = RED;
                    node = node->parent->parent;
                } else {
                    if (node == node->parent->left) {
                        rightRotate(node->parent);
                        node = node->right;
                    }
                    node->parent->color = BLACK;    
                    node->parent->parent->color = RED;
                    leftRotate(node->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }
    void traverse() {
        traverse(root);
        cout << endl;
    }
};

int main() {
    RedBlackTree tree;
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(40);
    tree.insert(50);
    tree.traverse();
    return 0;
}