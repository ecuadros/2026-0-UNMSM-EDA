#ifndef __AVL_H__
#define __AVL_H__
#include <iostream>
#include <mutex>
#include <algorithm>
#include "binarytree.h"
using namespace std;

// Node AVL
template <typename Traits>
class NodeAVL : public NodeBinaryTree<Traits> {
public:
    using value_type = typename Traits::value_type;
    using Node       = NodeAVL<Traits>;
    using BaseNode   = NodeBinaryTree<Traits>;

    int m_height;

    NodeAVL(value_type val, ref_type ref = -1)
        : BaseNode(val, ref), m_height(1) {}

    Node* getLeft() { return static_cast<Node*>(this->m_pLeft); }
    Node* getRight() { return static_cast<Node*>(this->m_pRight); }
};

// AVL Tree
template <typename Traits>
class CAVLTree : public CBinaryTree<Traits> {
public:
    using value_type = typename Traits::value_type;
    using BaseTree   = CBinaryTree<Traits>;
    using Node       = NodeAVL<Traits>;
    using BaseNode   = NodeBinaryTree<Traits>;

private:
    int height(BaseNode* node) {
        if (!node) return 0;
        return static_cast<Node*>(node)->m_height;
    }

    int getBalance(BaseNode* node) {
        if (!node) return 0;
        return height(node->m_pLeft) - height(node->m_pRight);
    }

    void updateHeight(BaseNode* node) {
        if (node) {
            static_cast<Node*>(node)->m_height = 
                1 + max(height(node->m_pLeft), height(node->m_pRight));
        }
    }

    // Rotacion simple derecha (LL)
    BaseNode* rotateRight(BaseNode* y) {
        BaseNode* x = y->m_pLeft;
        BaseNode* T2 = x->m_pRight;

        x->m_pRight = y;
        y->m_pLeft = T2;

        updateHeight(y);
        updateHeight(x);

        return x;
    }

    // Rotacion simple izquierda (RR)
    BaseNode* rotateLeft(BaseNode* x) {
        BaseNode* y = x->m_pRight;
        BaseNode* T2 = y->m_pLeft;

        y->m_pLeft = x;
        x->m_pRight = T2;

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    // Rotacion doble izquierda-derecha (LR)
    BaseNode* rotateLeftRight(BaseNode* node) {
        node->m_pLeft = rotateLeft(node->m_pLeft);
        return rotateRight(node);
    }

    // Rotacion doble derecha-izquierda (RL)
    BaseNode* rotateRightLeft(BaseNode* node) {
        node->m_pRight = rotateRight(node->m_pRight);
        return rotateLeft(node);
    }

    BaseNode* InternalInsertAVL(BaseNode* node, const value_type& val, ref_type ref) {
        if (!node) {
            return new Node(val, ref);
        }

        if (val < node->m_data) {
            node->m_pLeft = InternalInsertAVL(node->m_pLeft, val, ref);
        } else if (val > node->m_data) {
            node->m_pRight = InternalInsertAVL(node->m_pRight, val, ref);
        } else {
            return node;
        }

        updateHeight(node);

        int balance = getBalance(node);

        // LL
        if (balance > 1 && val < node->m_pLeft->m_data) {
            return rotateRight(node);
        }

        // RR
        if (balance < -1 && val > node->m_pRight->m_data) {
            return rotateLeft(node);
        }

        // LR
        if (balance > 1 && val > node->m_pLeft->m_data) {
            return rotateLeftRight(node);
        }

        // RL
        if (balance < -1 && val < node->m_pRight->m_data) {
            return rotateRightLeft(node);
        }

        return node;
    }

public:
    // Constructor
    CAVLTree() : BaseTree() {}

    // Constructor copia
    CAVLTree(const CAVLTree& other) : BaseTree(other) {}

    // Move Constructor
    CAVLTree(CAVLTree&& other) noexcept : BaseTree(std::move(other)) {}

    // Destructor
    virtual ~CAVLTree() {}

    // Insert con balanceo
    void Insert(const value_type& val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(this->m_mutex);
        this->m_pRoot = InternalInsertAVL(this->m_pRoot, val, ref);
    }
};

#endif // __AVL_H__
