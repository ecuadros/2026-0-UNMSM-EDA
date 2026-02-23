// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <mutex>
#include "BTreePage.h"
#include "stack.h"
#include "../general/types.h"
#include "../foreach.h"
#include "GeneralIterator.h"

#define DEFAULT_BTREE_ORDER 3

template <typename Node>
struct StackFrame {
    Node *page;
    Size  keyIndex;
};

template <typename T>
struct TreeTraitAscending {
    using keyType  = T;
    using ObjIDType = ref_type;
    using CompareFunc = std::less<T>;
};

template <typename T>
struct TreeTraitDescending {
    using keyType  = T;
    using ObjIDType = ref_type;
    using CompareFunc = std::greater<T>;
};

template <typename Container>
class BTreeForwardIterator : public GeneralIterator<Container>
{
    using Parent     = GeneralIterator<Container>;
    using Node       = typename Container::Node;
    using value_type = typename Container::value_type;
    using StackF     = StackFrame<Node>;
    CStack<StackF> stack;

    void pushLeftmost(Node* page) {
        while (page) {
            stack.push({page, 0}, 0);
            page = page->m_SubPages[0];
        }
    }

public:
    BTreeForwardIterator(Container* pContainer, Node* root, Size pos = 0)
        : Parent(pContainer, pos, root) {
        pushLeftmost(root);
        if (stack.empty()) Parent::m_data = nullptr;
        else               Parent::m_data = &stack.top().page->m_Keys[0];
    }
    BTreeForwardIterator(BTreeForwardIterator<Container>& another)
        : Parent(another), stack(another.stack) {}

    BTreeForwardIterator<Container>& operator++() {
        if (stack.empty()) {
            Parent::m_data = nullptr;
            return *this;
        }

        StackF& top = stack.top();
        Node*       page = top.page;
        Size         idx  = top.keyIndex;

        if (page->m_SubPages[idx + 1])
            pushLeftmost(page->m_SubPages[idx + 1]);
        top.keyIndex++;
        if (stack.top().keyIndex >= stack.top().page->m_KeyCount)
            stack.pop();
        if (!stack.empty())
            Parent::m_data = &stack.top().page->m_Keys[stack.top().keyIndex];
        else
            Parent::m_data = nullptr;

        ++Parent::m_pos;
        return *this;
    }

    value_type& operator*() {
        return *Parent::m_data;
    }
    value_type* operator->() {
        return Parent::m_data;
    }
};

template <typename Container>
class BTreeBackwardIterator : public GeneralIterator<Container>
{
    using Parent     = GeneralIterator<Container>;
    using Node       = typename Container::Node;
    using value_type = typename Container::value_type;
    using StackF     = StackFrame<Node>;
    CStack<StackF> stack;

    void pushRightmost(Node* page) {
        while (page) {
            stack.push({page, (Size)(page->m_KeyCount - 1)}, 0);
            page = page->m_SubPages[page->m_KeyCount];
        }
    }

public:
    BTreeBackwardIterator(Container* pContainer, Node* root, Size pos = 0)
        : Parent(pContainer, pos, root) {
        pushRightmost(root);
        if (stack.empty()) Parent::m_data = nullptr;
        else               Parent::m_data = &stack.top().page->m_Keys[stack.top().keyIndex];
    }
    BTreeBackwardIterator(BTreeBackwardIterator<Container>& another)
        : Parent(another), stack(another.stack) {}

    BTreeBackwardIterator<Container>& operator++() {
        if (stack.empty()) {
            Parent::m_data = nullptr;
            return *this;
        }

        StackF& top = stack.top();
        Node*       page = top.page;
        int         idx  = top.keyIndex;

        if (page->m_SubPages[idx])
            pushRightmost(page->m_SubPages[idx]);
        top.keyIndex--;
        if (stack.top().keyIndex < 0)
            stack.pop();
        if (!stack.empty())
            Parent::m_data = &stack.top().page->m_Keys[stack.top().keyIndex];
        else
            Parent::m_data = nullptr;
        --Parent::m_pos;
        return *this;
    }

    value_type& operator*() {
        return *Parent::m_data;
    }
    value_type* operator->() {
        return Parent::m_data;
    }
};

template <typename Traits>
class BTree 
// this is the full version of the BTree
{
       typedef CBTreePage<Traits> BTNode; // useful shorthand
       using keyType   = typename Traits::keyType;
       using ObjIDType = typename Traits::ObjIDType;

public:
       using Node       = CBTreePage<Traits>;
       using value_type = tagObjectInfo<keyType, ObjIDType>;
       using  forward_iterator = BTreeForwardIterator<BTree<Traits>>;
       using  backward_iterator = BTreeBackwardIterator<BTree<Traits>>;
       /*struct ObjectInfo
       {
               keyType first;
               long    second;
               ObjectInfo *&operator->() { return this; }
       };*/
       //typedef ObjectInfo iterator;
       typedef typename BTNode::lpfnForEach2    lpfnForEach2;
       typedef typename BTNode::lpfnForEach3    lpfnForEach3;
       typedef typename BTNode::lpfnFirstThat2  lpfnFirstThat2;
       typedef typename BTNode::lpfnFirstThat3  lpfnFirstThat3;
       typedef typename BTNode::ObjectInfo      ObjectInfo;

       forward_iterator begin()
       { return forward_iterator(this, &m_Root, 0); }
       forward_iterator end()
       { return forward_iterator(this, nullptr, m_NumKeys); }
       backward_iterator rbegin()
       { return backward_iterator(this, &m_Root, m_NumKeys - 1); }
       backward_iterator rend()
       { return backward_iterator(this, nullptr, -1); }       

       BTree(int order = DEFAULT_BTREE_ORDER, bool unique = true);
       BTree(const BTree &another);
       ~BTree();
       //int           Open (char * name, int mode);
       //int           Create (char * name, int mode);
       //int           Close ();
       bool            Insert (const keyType key, const int ObjID);
       bool            Remove (const keyType key, const int ObjID);
       ObjIDType       Search (const keyType key);
       long            size()  { return m_NumKeys; }
       long            height() { return m_Height;      }
       long            GetOrder() { return m_Order;     }

       void            Print (ostream &os);
       void            ForEach( lpfnForEach2 lpfn, void *pExtra1 );
       void            ForEach( lpfnForEach3 lpfn, void *pExtra1, void *pExtra2);
       // ObjectInfo*     FirstThat( lpfnFirstThat2 lpfn, void *pExtra1 );
       // ObjectInfo*     FirstThat( lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2);
       //typedef               ObjectInfo iterator;

       template <typename ObjFunc, typename ...Args>
       auto FirstThat(ObjFunc of, Args... args){
           return ::FirstThat(*this, of, args...);
       }
       template <typename Func, typename... Args>
       void preorden(Func fn, Args... args) {
           m_Root.internalpreorden(&m_Root, fn, args...);
       }
       template <typename Func, typename... Args>
       void inorden(Func fn, Args... args) {
           m_Root.internalinorden(&m_Root, fn, args...);
       }
       template <typename Func, typename... Args>
       void postorden(Func fn, Args... args) {
           m_Root.internalpostorden(&m_Root, fn, args...);
       }
       
       friend ostream &operator<<(ostream &os, BTree &BTree) {
       
       os << "Tamaño del CBinaryTree: " << BTree.size() << endl << endl;
       BTree.inorden( [&os]( auto &val ) { os << "("<< val.key<< " : " << val.ObjID << ")" << " "; } );
       return os;
       }

protected:
       BTNode          m_Root;
       long            m_NumKeys; // number of keys
       bool            m_Unique;  // Accept the elements only once ?
       int             m_Order;   // order of tree
       int             m_Height;  // height of tree
       mutable mutex   mtx;

};

const int MaxHeight = 5;
template <typename Traits>
BTree<Traits>::BTree(int order, bool unique)
                               : m_Root(2 * order  + 1, unique),
                                 m_NumKeys(0),
                                 m_Unique(unique),
                                 m_Order(order)
{
       m_Root.SetMaxKeysForChilds(order);
       m_Height = 1;
}

// Move constructor
template <typename Traits>
BTree<Traits>::BTree(const BTree &another)
    : m_Root(2 * another.m_Order + 1, another.m_Unique),
      m_NumKeys(0),
      m_Unique(another.m_Unique),
      m_Order(another.m_Order),
      m_Height(another.m_Height)
{
    lock_guard<mutex> lock(another.mtx); // bloquea el otro árbol durante la copia
    m_Root.SetMaxKeysForChilds(another.m_Order);
    m_Root = &another.m_Root;
    m_NumKeys = another.m_NumKeys;
}

template <typename Traits>
BTree<Traits>::~BTree()
{
}

template <typename Traits>
bool BTree<Traits>::Insert(const keyType key, const int ObjID)
{
       lock_guard<mutex> lock(mtx);
       bt_ErrorCode error = m_Root.Insert(key, ObjID);
       if( error == bt_duplicate )
               return false;
       m_NumKeys++;
       if( error == bt_overflow )
       {
               m_Root.SplitRoot();
               m_Height++;
       }
       return true;
}

template <typename Traits>
bool BTree<Traits>::Remove (const keyType key, const int ObjID)
{
       bt_ErrorCode error = m_Root.Remove(key, ObjID);
       if( error == bt_duplicate || error == bt_nofound )
               return false;
       m_NumKeys--;

       if( error == bt_rootmerged )
               m_Height--;
       return true;
}

template <typename Traits>
typename BTree<Traits>::ObjIDType BTree<Traits>::Search (const keyType key)
{
       ObjIDType ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}


template <typename Traits>
void BTree<Traits>::ForEach(lpfnForEach2 lpfn, void *pExtra1)
{
       m_Root.ForEach(lpfn, 0, pExtra1);
}

template <typename Traits>
void BTree<Traits>::ForEach(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2)
{
       m_Root.ForEach(lpfn, 0, pExtra1, pExtra2);
}

// template <typename Traits>
// typename BTree<Traits>::ObjectInfo *
// BTree<Traits>::FirstThat(lpfnFirstThat2 lpfn, void *pExtra1)
// {
//        return m_Root.FirstThat(lpfn, 0, pExtra1);
// }

// template <typename Traits>
// typename BTree<Traits>::ObjectInfo *
// BTree<Traits>::FirstThat(lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2)
// {
//        return m_Root.FirstThat(lpfn, 0, pExtra1, pExtra2);
// }

template <typename Traits>
void BTree<Traits>::Print(ostream &os){
       m_Root.Print(os);
}

void DemoBTree();

#endif