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
