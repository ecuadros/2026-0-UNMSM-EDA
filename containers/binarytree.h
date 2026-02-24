#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include "../general/types.h"
#include "../util.h"


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

template <typename Traits>
class NodeBinaryTree{
    using  value_type  = typename Traits::value_type;
    using  Node        = typename NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pChild[2]  = nullptr;
public:
    NodeBinaryTree(){}
    NodeBinaryTree( value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){   }
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }
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