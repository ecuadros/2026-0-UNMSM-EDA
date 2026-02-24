
#ifndef __AVL_H__
#define __AVL_H__

#include "binarytree.h"

template <typename Traits>
class CAVL : public CBinaryTree<Traits> {
    using Parent     = CBinaryTree<Traits>;
    using value_type = typename Traits::value_type;
    using Node       = NodeBinaryTree<Traits>;

public:
    CAVL() {}

    void Insert(const value_type &val, ref_type ref = -1) {
        lock_guard<mutex> lock(Parent::m_mutex);
        InternalInsertAVL(Parent::m_pRoot, val, ref);
        ++Parent::m_nElements;
    }

private:
    int Height(Node *node) {
        return node ? node->m_height : 0;
    }

    void UpdateHeight(Node *node) {
        if (node)
            node->m_height = 1 + max(Height(node->m_pChild[0]),
                                     Height(node->m_pChild[1]));
    }

    int BalanceFactor(Node *node) {
        return node ? Height(node->m_pChild[0]) - Height(node->m_pChild[1]) : 0;
    }

    // Rotacion derecha (Left-Left)
    Node *RotateRight(Node *y) {
        Node *x        = y->m_pChild[0];
        y->m_pChild[0] = x->m_pChild[1];
        x->m_pChild[1] = y;
        UpdateHeight(y);
        UpdateHeight(x);
        return x;
    }

    // Rotacion izquierda (Right-Right)
    Node *RotateLeft(Node *x) {
        Node *y        = x->m_pChild[1];
        x->m_pChild[1] = y->m_pChild[0];
        y->m_pChild[0] = x;
        UpdateHeight(x);
        UpdateHeight(y);
        return y;
    }

    Node *Rebalance(Node *node) {
        UpdateHeight(node);
        int bf = BalanceFactor(node);

        if (bf > 1  && BalanceFactor(node->m_pChild[0]) >= 0)
            return RotateRight(node);

        if (bf > 1  && BalanceFactor(node->m_pChild[0]) < 0) {
            node->m_pChild[0] = RotateLeft(node->m_pChild[0]);
            return RotateRight(node);
        }

        if (bf < -1 && BalanceFactor(node->m_pChild[1]) <= 0)
            return RotateLeft(node);

        if (bf < -1 && BalanceFactor(node->m_pChild[1]) > 0) {
            node->m_pChild[1] = RotateRight(node->m_pChild[1]);
            return RotateLeft(node);
        }

        return node;
    }

    void InternalInsertAVL(Node *&rParent, const value_type &val, ref_type ref) {
        if (!rParent) {
            rParent = new Node(val, ref);
            return;
        }
        auto path = Parent::comp(val, rParent->GetValue());
        InternalInsertAVL(rParent->m_pChild[path], val, ref);
        rParent = Rebalance(rParent);
    }
};

void DemoAVL();

#endif // __AVL_H__