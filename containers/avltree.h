#ifndef __AVLTREE_H__
#define __AVLTREE_H__

#include <iostream>
#include <algorithm> 
#include <utility>
#include "binarytree.h"

template <typename Traits>
class CAVLTree : public CBinaryTree<Traits> {
public:
    using Parent      = CBinaryTree<Traits>;
    using value_type  = typename Traits::value_type;
    using Node        = typename Parent::Node;

public:
    CAVLTree() : Parent() {}
    virtual ~CAVLTree() {}

    void Insert(const value_type &val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(this->m_mtx);
        InternalAVLInsert(this->m_pRoot, val, ref);
    }

    void Insert(value_type &&val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(this->m_mtx);
        InternalAVLInsertMove(this->m_pRoot, std::move(val), ref);
    }

private:
    Size GetHeight(Node *pNode) {
        if (!pNode) 
            return 0;
        return pNode->m_height;
    }

    Size GetBalance(Node *pNode) {
        if (!pNode) 
            return 0;
        return GetHeight(pNode->m_pChild[0]) - GetHeight(pNode->m_pChild[1]);
    }

    void UpdateHeight(Node *pNode) {
        if (pNode) {
            Size leftH = GetHeight(pNode->m_pChild[0]);
            Size rightH = GetHeight(pNode->m_pChild[1]);
            pNode->m_height = 1 + std::max(leftH, rightH);
        }
    }

    void RotateRight(Node *&rRoot) {
        Node *newRoot = rRoot->m_pChild[0];
        rRoot->m_pChild[0] = newRoot->m_pChild[1];
        newRoot->m_pChild[1] = rRoot;

        UpdateHeight(rRoot);
        UpdateHeight(newRoot);
        rRoot = newRoot;
    }

    void RotateLeft(Node *&rRoot) {
        Node *newRoot = rRoot->m_pChild[1];
        rRoot->m_pChild[1] = newRoot->m_pChild[0];
        newRoot->m_pChild[0] = rRoot;

        UpdateHeight(rRoot);
        UpdateHeight(newRoot);
        rRoot = newRoot;
    }

    void Balance(Node *&pNode) {
        if (!pNode) 
            return;

        UpdateHeight(pNode);
        Size balance = GetBalance(pNode);

        if (balance > 1) {
            if (GetBalance(pNode->m_pChild[0]) < 0)
                RotateLeft(pNode->m_pChild[0]); 
            RotateRight(pNode); 
        }
        else if (balance < -1) {
            if (GetBalance(pNode->m_pChild[1]) > 0)
                RotateRight(pNode->m_pChild[1]); 
            RotateLeft(pNode); 
        }
    }

    void InternalAVLInsert(Node *&rParent, const value_type &val, ref_type ref) {
        if (!rParent) {
            rParent = new Node(val, ref);
            return;
        }
        
        bool path = this->comp(val, rParent->GetValue()); 
        InternalAVLInsert(rParent->m_pChild[path], val, ref);

        Balance(rParent); 
    }

    void InternalAVLInsertMove(Node *&rParent, value_type &&val, ref_type ref) {
        if (!rParent) {
            rParent = new Node(std::move(val), ref); 
            return;
        }
        
        bool path = this->comp(val, rParent->GetValue());
        InternalAVLInsertMove(rParent->m_pChild[path], std::move(val), ref);

        Balance(rParent); 
    }
};

#endif // __AVLTREE_H__
