#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include "../general/types.h"
#include "../util.h"
using namespace std;

// Traits
template <typename T>
struct TreeTraitAscending {
    using value_type  = T;
    using CompareFunc = std::greater<T>;
};

template <typename T>
struct TreeTraitDescending {
    using value_type  = T;
    using CompareFunc = std::less<T>;
};

// Node
template <typename Traits>
class NodeBinaryTree {
public:
    using value_type  = typename Traits::value_type;
    using Node        = NodeBinaryTree<Traits>;
    using CompareFunc = typename Traits::CompareFunc;

    value_type m_data;
    ref_type   m_ref;
    Node*      m_pLeft;
    Node*      m_pRight;

    NodeBinaryTree(value_type val, ref_type ref = -1)
        : m_data(val), m_ref(ref), m_pLeft(nullptr), m_pRight(nullptr) {}

    value_type  GetValue()    const { return m_data; }
    value_type& GetValueRef()       { return m_data; }
};

// Forward Iterator
template <typename Container>
class BinaryTreeForwardIterator {
public:
    using Node = typename Container::Node;
    using value_type = typename Container::value_type;

private:
    queue<Node*> m_queue;
    Node* m_pCurrent;

public:
    BinaryTreeForwardIterator(Node* root) : m_pCurrent(nullptr) {
        if (root) {
            inorderTraversal(root);
            if (!m_queue.empty()) {
                m_pCurrent = m_queue.front();
                m_queue.pop();
            }
        }
    }

    BinaryTreeForwardIterator() : m_pCurrent(nullptr) {}

    void inorderTraversal(Node* node) {
        if (!node) return;
        inorderTraversal(node->m_pLeft);
        m_queue.push(node);
        inorderTraversal(node->m_pRight);
    }

    bool operator!=(const BinaryTreeForwardIterator& other) const {
        return m_pCurrent != other.m_pCurrent;
    }

    bool operator==(const BinaryTreeForwardIterator& other) const {
        return m_pCurrent == other.m_pCurrent;
    }

    value_type& operator*() {
        return m_pCurrent->GetValueRef();
    }

    BinaryTreeForwardIterator& operator++() {
        if (!m_queue.empty()) {
            m_pCurrent = m_queue.front();
            m_queue.pop();
        } else {
            m_pCurrent = nullptr;
        }
        return *this;
    }
};

// Backward Iterator
template <typename Container>
class BinaryTreeBackwardIterator {
public:
    using Node = typename Container::Node;
    using value_type = typename Container::value_type;

private:
    queue<Node*> m_queue;
    Node* m_pCurrent;

public:
    BinaryTreeBackwardIterator(Node* root) : m_pCurrent(nullptr) {
        if (root) {
            reverseInorderTraversal(root);
            if (!m_queue.empty()) {
                m_pCurrent = m_queue.front();
                m_queue.pop();
            }
        }
    }

    BinaryTreeBackwardIterator() : m_pCurrent(nullptr) {}

    void reverseInorderTraversal(Node* node) {
        if (!node) return;
        reverseInorderTraversal(node->m_pRight);
        m_queue.push(node);
        reverseInorderTraversal(node->m_pLeft);
    }

    bool operator!=(const BinaryTreeBackwardIterator& other) const {
        return m_pCurrent != other.m_pCurrent;
    }

    bool operator==(const BinaryTreeBackwardIterator& other) const {
        return m_pCurrent == other.m_pCurrent;
    }

    value_type& operator*() {
        return m_pCurrent->GetValueRef();
    }

    BinaryTreeBackwardIterator& operator++() {
        if (!m_queue.empty()) {
            m_pCurrent = m_queue.front();
            m_queue.pop();
        } else {
            m_pCurrent = nullptr;
        }
        return *this;
    }
};

template <typename Traits>
class CBinaryTree{
public:
    using  value_type  = typename Traits::value_type;
    using  Node        = typename NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;
private:
    Node *m_pRoot = nullptr;
    CompareFunc comp;

public:
    CBinaryTree(){}
    // TODO: Copy constructor
    CBinaryTree(CBinaryTree &another){

    }
    // TODO: Move constructor
    CBinaryTree(CBinaryTree &&another){

    }
    virtual ~CBinaryTree(){

    }
private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
        if( !rParent ){
            rParent = new Node(val, ref);
            return;
        }
        auto path = comp(val, rParent->GetValue());
        InternalInsert(rParent->m_pChild[path], val, ref);
    }
public:
    void Insert(const value_type &val, ref_type ref){
        InternalInsert(m_pRoot, val, ref);
    }
};


#endif // __BINARYTREE_H__
