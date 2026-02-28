#ifndef __AVLTREE_H__
#define __AVLTREE_H__

#include <algorithm>
#include "binarytree.h"

template <typename Traits>
class CAVLTree : public CBinaryTree<Traits>{
public:
    using value_type = typename Traits::value_type;
    using Node = NodeBinaryTree<Traits>;
    using CompareFunc = typename Traits::CompareFunc;
    using size_type = size_t;
protected:
    //alturas y fatores
    int Height(Node* pNode){
        return pNode ? pNode->m_height : 0;
    }
    int BalanceFactor(Node* pNode){
        return pNode ? Height(pNode->m_pChild[0]) - Height(pNode->m_pChild[1]) : 0;
    }
    void UpdateHeight(Node* pNode){
        if(pNode){
            pNode->m_height = std::max(Height(pNode->m_pChild[0]), Height(pNode->m_pChild[1])) + 1;

        }
    }
    //rotaciones
    void RotateRight(Node*& root){
        Node* newRoot = root->m_pChild[0];
        root->m_pChild[0] = newRoot->m_pChild[1];
        newRoot->m_pChild[1] = root;
        UpdateHeight(root);
        UpdateHeight(newRoot);
        root = newRoot;

    }
    void RotateLeft(Node*& root){
        Node* newRoot = root->m_pChild[1];
        root->m_pChild[1] = newRoot->m_pChild[0];
        newRoot->m_pChild[0] = root;

        UpdateHeight(root);
        UpdateHeight(newRoot);
        root = newRoot;
    }
    void Balance(Node*& root){
     if(!root) return;
     UpdateHeight(root);
     int bf = BalanceFactor(root);
     if(bf > 1){
         if(BalanceFactor(root->m_pChild[0]) < 0){
          RotateLeft(root->m_pChild[0]);
         }
         RotateRight(root);
         } else if(bf < -1){
         if(BalanceFactor(root->m_pChild[1]) > 0){
             RotateRight(root->m_pChild[1]);
         }  RotateLeft(root);
        }
     }
     //Insert y remove
     void InternalInsert(Node*& rParent, const value_type& val, int ref) {
         if (!rParent) {
             rParent = new Node(val, ref);
             return;
         }
         if (val == rParent->GetValue()) return;

         auto path = this->comp(val, rParent->GetValue());
         InternalInsert(rParent->m_pChild[path], val, ref);

         Balance(rParent);
     }
    void InternalRemove(Node*& rParent, const value_type& val) {
        if (!rParent) return;

        if (val == rParent->GetValue()) {
            if (!rParent->m_pChild[0] && !rParent->m_pChild[1]) {
                delete rParent;
                rParent = nullptr;
            } else if (!rParent->m_pChild[0]) {
                Node *temp = rParent;
                rParent = rParent->m_pChild[1];
                delete temp;
            } else if (!rParent->m_pChild[1]) {
                Node *temp = rParent;
                rParent = rParent->m_pChild[0];
                delete temp;
            } else {
                Node *&replace = FindMin(rParent->m_pChild[1]);
                rParent->GetValueRef() = replace->GetValue();
                InternalRemove(replace, replace->GetValue());
            }
        } else {
            auto path = this->comp(val, rParent->GetValue());
            InternalRemove(rParent->m_pChild[path], val);
        }
        if (rParent) Balance(rParent);
    }
    Node*& FindMin(Node*& node) {
        if (!node->m_pChild[0]) return node;
        return FindMin(node->m_pChild[0]);
    }
public:
    CAVLTree() : CBinaryTree<Traits>() {}
    void Insert(const value_type& val, int ref = 0) {
        std::lock_guard <std::mutex> lock(this->mutex);
        InternalInsert(this->m_pRoot, val, ref);
    }
    void Remove(const value_type& val) {
        std::lock_guard <std::mutex> lock(this->mutex);
        InternalRemove(this->m_pRoot, val);
    }
};
#endif