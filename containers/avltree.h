#ifndef __AVLTREE_H__
#define __AVLTREE_H__

#include <iostream>
#include <mutex>
#include <assert.h>
#include "stack.h"
#include "../general/types.h"
#include "../util.h"
#include "../variadic-util.h"
#include "GeneralIterator.h"
#include "binarytree.h"

template <typename T> class NodeAvlTree;

template <typename T>
struct AVLTraitAscending {
    using value_type  = T;
    using CompareFunc = std::greater<T>;
    using Node        = NodeAvlTree<AVLTraitAscending<T>>;
};

template <typename T>
struct AVLTraitDescending {
    using value_type  = T;
    using CompareFunc = std::less<T>;
    using Node        = NodeAvlTree<AVLTraitDescending<T>>;
};

template <typename Traits>
class NodeAvlTree {
public:
    using value_type  = typename Traits::value_type;
    using  Node       = typename Traits::Node;
    using CompareFunc = typename Traits::CompareFunc;
private:
    value_type m_data;
    ref_type   m_ref;
    Node      *m_pChild[2] = {nullptr, nullptr};
public:
    Size       m_height = 0;
    NodeAvlTree() {}
    NodeAvlTree(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref) {}

    value_type  GetValue    () const { return m_data; }
    value_type &GetValueRef ()       { return m_data; }
    ref_type    GetRef      () const { return m_ref; }
    ref_type   &GetRefRef   ()       { return m_ref; }
    Node       *GetLeft     () const { return m_pChild[0]; }
    Node      *&GetLeftRef  ()       { return m_pChild[0]; }
    Node       *GetRight    () const { return m_pChild[1]; }
    Node      *&GetRightRef ()       { return m_pChild[1]; }
    
};

template <typename Traits>
class CAvlTree : public CBinaryTree<Traits> {
public:
    using  value_type  = typename Traits::value_type;
    using  CompareFunc = typename Traits::CompareFunc;
    using  Node        = typename Traits::Node;
    using  forwarditerator = BTreeForwardIterator<CBinaryTree<Traits>>;
    using  backwardditerator = BTreeBackwardIterator<CBinaryTree<Traits>>;

protected:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);
    void InternalRemove(Node *&root, const value_type &value);

public:
    Size GetHeight(Node *node) const { return node ? node->m_height : 0; }
    void UpdateHeight(Node *node);

    Size getBF( Node *root);
    void CheckBalance(Node *&root);

    Node* rotateRight(Node *root);
    Node* rotateLeft(Node *root);
    Node* rotateLR(Node* root);
    Node* rotateRL(Node* root);
    

};

template <typename Traits>
void CAvlTree<Traits>::UpdateHeight(Node *node) {
    Size leftH = GetHeight(node->GetLeft());
    Size rightH = GetHeight(node->GetRight());
    node->m_height = 1 + ( leftH > rightH ? leftH : rightH );
}

template <typename Traits>
Size CAvlTree<Traits>::getBF(Node *root) {
    return root ? GetHeight(root->GetRight()) - GetHeight(root->GetLeft()) : 0;
}

template <typename Traits>
void CAvlTree<Traits>::CheckBalance(Node *&root) {
    if ( !root ) return;
    if ( getBF(root) ==  2 ) 
        root = getBF(root->GetRight()) == -1 ? rotateLR( root ) : rotateLeft ( root );
    if ( getBF(root) == -2 ) 
        root = getBF(root->GetLeft() ) ==  1 ? rotateRL( root ) : rotateRight( root );
}

template <typename Traits>
typename CAvlTree<Traits>::Node* CAvlTree<Traits>::rotateRight( Node* root) {
    Node *newRoot = root->GetLeft();
    Node *rightSubTree = newRoot->GetRight();
    newRoot->GetRightRef() = root;
    root->GetLeftRef() = rightSubTree;

    UpdateHeight(root);
    UpdateHeight(newRoot);
    return newRoot;
}

template <typename Traits>
typename CAvlTree<Traits>::Node* CAvlTree<Traits>::rotateLeft( Node* root) {
    Node *newRoot = root->GetRight();
    Node *leftSubTree = newRoot->GetLeft();
    newRoot->GetLeftRef() = root;
    root->GetRightRef() = leftSubTree;

    UpdateHeight(root);
    UpdateHeight(newRoot);
    return newRoot;
}

template <typename Traits>
typename CAvlTree<Traits>::Node* CAvlTree<Traits>::rotateLR(Node* root) {
    root->GetLeftRef() = rotateLeft(root->GetLeft());   
    return rotateRight(root);          
}

template <typename Traits>
typename CAvlTree<Traits>::Node* CAvlTree<Traits>::rotateRL(Node* root) {
    root->GetRightRef() = rotateRight(root->GetRight()); 
    return rotateLeft(root);               
}

template <typename Traits>
void CAvlTree<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref) {
    if (!rParent) {
        rParent = new Node(val, ref);
        ++this->n_Elements;
        return;
    }
    auto path = this->comp(val, rParent->GetValue());
    InternalInsert(path ? rParent->GetRightRef() : rParent->GetLeftRef(), val, ref);
    
    CheckBalance(rParent);
}

template <typename Traits>
void CAvlTree<Traits>::InternalRemove(Node *&rParent, const value_type &value) {
    assert(this->n_Elements > 0);

    if (!rParent) return;

    if ( value != rParent->GetValue() ) {
        auto path = this->comp(value, rParent->GetValue());
        InternalRemove(path ? rParent->GetRightRef() : rParent->GetLeftRef(), value);
        CheckBalance(rParent);
        return;
    }

    Node* temp = rParent;
    if (!rParent->GetLeft() && !rParent->GetRight()) rParent = nullptr;
    else if (!rParent->GetLeft())  rParent = rParent->GetRight();
    else if (!rParent->GetRight()) rParent = rParent->GetLeft();
    else {  // En este caso optamos por el reemplazar por el predecesor del dato a eliminar
        Node* parent = rParent;
        temp = rParent->GetLeft();
        while ( temp->GetRight() ) {
            parent = temp;
            temp = temp->GetRight();
        }
        rParent->GetValueRef() = temp->GetValue();
        rParent->GetRefRef() = temp->GetRef();
        
        // Actualizamos el enlace antes del nodo anterior al predecesor del nodo a eliminar
        (parent == rParent ? parent->GetLeftRef() : parent->GetRightRef()) = temp->GetLeft();
    }
    delete temp;
    --this->n_Elements;
    CheckBalance(rParent);
}

void DemoAvlTree();

#endif