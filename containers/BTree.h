// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <vector>
#include "BTreePage.h"
#include "../foreach.h"
#include "../general/types.h"

#define DEFAULT_BTREE_ORDER 3

template <typename keyType, typename ObjIDType = LSize>
class BTree 
// this is the full version of the BTree
{
       using BTNode = CBTreePage<keyType, ObjIDType>; // useful shorthand
       /*struct ObjectInfo
       {
               keyType first;
               long    second;
               ObjectInfo *&operator->() { return this; }
       };*/

public:
       using key_type = keyType;
       using obj_id_type = ObjIDType;
       using size_type = Size;
       using long_size_type = LSize;
       using forward_iterator = ForwardBTreeIterator<keyType, ObjIDType>;
       using backward_iterator = BackwardBTreeIterator<keyType, ObjIDType>;

       //typedef ObjectInfo iterator;
       typedef typename BTNode::lpfnForEach2    lpfnForEach2;
       typedef typename BTNode::lpfnForEach3    lpfnForEach3;
       typedef typename BTNode::lpfnFirstThat2  lpfnFirstThat2;
       typedef typename BTNode::lpfnFirstThat3  lpfnFirstThat3;
       typedef typename BTNode::ObjectInfo      ObjectInfo;

public:
       BTree(Size order = DEFAULT_BTREE_ORDER, bool unique = true);
       ~BTree();
       //int           Open (char * name, int mode);
       //int           Create (char * name, int mode);
       //int           Close ();
       bool            Insert (const keyType key, const ObjIDType ObjID);
       bool            Remove (const keyType key, const ObjIDType ObjID);
       ObjIDType       Search (const keyType key);
       LSize           size()  { return m_NumKeys; }
       LSize           height() { return m_Height;      }
       LSize           GetOrder() { return m_Order;     }

       void            Print (ostream &os);
       void            ForEach( lpfnForEach2 lpfn, void *pExtra1 );
       void            ForEach( lpfnForEach3 lpfn, void *pExtra1, void *pExtra2);
       ObjectInfo*     FirstThat( lpfnFirstThat2 lpfn, void *pExtra1 );
       ObjectInfo*     FirstThat( lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2);

       template <typename Func, typename ...Args>
       void            ForEach( forward_iterator start, forward_iterator end, Func func, Args&& ...args );
       template <typename Func, typename ...Args>
       ObjectInfo*     FirstThat( forward_iterator start, forward_iterator end, Func func, Args&& ...args);

       forward_iterator begin();
       forward_iterator end();
       backward_iterator rbegin();
       backward_iterator rend();

protected:
       BTNode          m_Root;
       LSize           m_NumKeys; // number of keys
       bool            m_Unique;  // Accept the elements only once ?
       LSize           m_Order;   // order of tree
       LSize           m_Height;  // height of tree

       friend class ForwardBTreeIterator<keyType, ObjIDType>;
       friend class BackwardBTreeIterator<keyType, ObjIDType>;
};

const Size MaxHeight = 5;
template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::BTree(Size order, bool unique)
                               : m_Root(2 * order  + 1, unique),
                                 m_NumKeys(0),
                                 m_Unique(unique),
                                 m_Order(order)
{
       m_Root.SetMaxKeysForChilds(order);
       m_Height = 1;
}

template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::~BTree()
{
}

template<typename keyType, typename ObjIDType>
template<typename Func, typename ...Args>
void BTree<keyType, ObjIDType>::ForEach(forward_iterator start, forward_iterator end, Func func, Args &&...args) {
       ::Foreach(start, end, func, std::forward<Args>(args)...);
}

template<typename keyType, typename ObjIDType>
template<typename Func, typename... Args>
typename BTree<keyType, ObjIDType>::ObjectInfo *
BTree<keyType, ObjIDType>::FirstThat(forward_iterator start, forward_iterator end, Func func, Args &&... args) {
       auto it = ::FirstThat(start, end, func, std::forward<Args>(args)...);
       return (it == end) ? nullptr : it.operator->();
}

template <typename keyType, typename ObjIDType>
bool BTree<keyType, ObjIDType>::Insert(const keyType key, const ObjIDType ObjID)
{
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

template <typename keyType, typename ObjIDType>
bool BTree<keyType, ObjIDType>::Remove (const keyType key, const ObjIDType ObjID)
{
       bt_ErrorCode error = m_Root.Remove(key, ObjID);
       if( error == bt_duplicate || error == bt_nofound )
               return false;
       m_NumKeys--;

       if( error == bt_rootmerged )
               m_Height--;
       return true;
}

template <typename keyType, typename ObjIDType>
ObjIDType BTree<keyType, ObjIDType>::Search (const keyType key)
{
       ObjIDType ObjID{};  // esto antes usaba -1
       m_Root.Search(key, ObjID);
       return ObjID;
}


template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::ForEach(lpfnForEach2 lpfn, void *pExtra1)
{
       m_Root.ForEach(lpfn, 0, pExtra1);
}

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::ForEach(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2)
{
       m_Root.ForEach(lpfn, 0, pExtra1, pExtra2);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::ObjectInfo *
BTree<keyType, ObjIDType>::FirstThat(lpfnFirstThat2 lpfn, void *pExtra1)
{
       return m_Root.FirstThat(lpfn, 0, pExtra1);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::ObjectInfo *
BTree<keyType, ObjIDType>::FirstThat(lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2)
{
       return m_Root.FirstThat(lpfn, 0, pExtra1, pExtra2);
}

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::Print(ostream &os){
       m_Root.Print(os);
}

template <typename keyType, typename ObjIDType>
class ForwardBTreeIterator {
       using Tree = BTree<keyType, ObjIDType>;
       using Page = CBTreePage<keyType, ObjIDType>;
public:
       using ObjectInfo = typename Tree::ObjectInfo;
private:
       struct Frame {
               Page *page;
               Size keyIndex;
       };

       Tree *m_tree;
       vector<Frame> m_stack;
       ObjectInfo *m_current = nullptr;

       void push_leftmost(Page *page) {
               while (page) {
                       m_stack.push_back({page, 0});
                       page = page->m_SubPages[0];
               }
       }

       void settle() {
               while (!m_stack.empty()) {
                       Frame &f = m_stack.back();
                       if (f.keyIndex < f.page->m_KeyCount) {
                               m_current = &f.page->m_Keys[f.keyIndex];
                               return;
                       }
                       m_stack.pop_back();
               }
               m_current = nullptr;
       }

public:
       ForwardBTreeIterator(Tree *tree, bool is_end) : m_tree(tree) {
               if (!is_end && m_tree && m_tree->m_Root.m_KeyCount > 0) {
                       push_leftmost(&m_tree->m_Root);
                       settle();
               }
       }

       ObjectInfo &operator*() const { return *m_current; }
       ObjectInfo *operator->() const { return m_current; }

       ForwardBTreeIterator &operator++() {
               if (!m_current || m_stack.empty()) return *this;
               Frame &f = m_stack.back();
               const Size keyIndex = f.keyIndex;
               Page *page = f.page;

               f.keyIndex++;
               if (page->m_SubPages[keyIndex + 1]) {
                       push_leftmost(page->m_SubPages[keyIndex + 1]);
               }
               settle();
               return *this;
       }

       ForwardBTreeIterator operator++(int) {
               ForwardBTreeIterator tmp = *this;
               ++(*this);
               return tmp;
       }

       bool operator==(const ForwardBTreeIterator &other) const {
               return m_tree == other.m_tree && m_current == other.m_current;
       }

       bool operator!=(const ForwardBTreeIterator &other) const {
               return !(*this == other);
       }
};

template <typename keyType, typename ObjIDType>
class BackwardBTreeIterator {
       using Tree = BTree<keyType, ObjIDType>;
       using Page = CBTreePage<keyType, ObjIDType>;
public:
       using ObjectInfo = typename Tree::ObjectInfo;
private:
       struct Frame {
               Page *page;
               Size keyIndex;
       };

       Tree *m_tree;
       vector<Frame> m_stack;
       ObjectInfo *m_current = nullptr;

       void push_rightmost(Page *page) {
               while (page) {
                       m_stack.push_back({page, page->m_KeyCount - 1});
                       page = page->m_SubPages[page->m_KeyCount];
               }
       }

       void settle() {
               while (!m_stack.empty()) {
                       Frame &f = m_stack.back();
                       if (f.keyIndex >= 0) {
                               m_current = &f.page->m_Keys[f.keyIndex];
                               return;
                       }
                       m_stack.pop_back();
               }
               m_current = nullptr;
       }

public:
       BackwardBTreeIterator(Tree *tree, bool is_end) : m_tree(tree) {
               if (!is_end && m_tree && m_tree->m_Root.m_KeyCount > 0) {
                       push_rightmost(&m_tree->m_Root);
                       settle();
               }
       }

       ObjectInfo &operator*() const { return *m_current; }
       ObjectInfo *operator->() const { return m_current; }

       BackwardBTreeIterator &operator++() {
               if (!m_current || m_stack.empty()) return *this;
               Frame &f = m_stack.back();
               const Size keyIndex = f.keyIndex;
               Page *page = f.page;

               f.keyIndex--;
               if (page->m_SubPages[keyIndex]) {
                       push_rightmost(page->m_SubPages[keyIndex]);
               }
               settle();
               return *this;
       }

       BackwardBTreeIterator operator++(int) {
               BackwardBTreeIterator tmp = *this;
               ++(*this);
               return tmp;
       }

       bool operator==(const BackwardBTreeIterator &other) const {
               return m_tree == other.m_tree && m_current == other.m_current;
       }

       bool operator!=(const BackwardBTreeIterator &other) const {
               return !(*this == other);
       }
};

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::forward_iterator BTree<keyType, ObjIDType>::begin() {
       return forward_iterator(this, false);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::forward_iterator BTree<keyType, ObjIDType>::end() {
       return forward_iterator(this, true);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::backward_iterator BTree<keyType, ObjIDType>::rbegin() {
       return backward_iterator(this, false);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::backward_iterator BTree<keyType, ObjIDType>::rend() {
       return backward_iterator(this, true);
}

void DemoBTree();

#endif
