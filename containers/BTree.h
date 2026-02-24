// btree.h - BTree con iteradores, traversals variadic, operadores, y concurrencia

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <iomanip>
#include <mutex>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "BTreePage.h"
#include "../foreach.h"
#include "../general/types.h"

using namespace std;

#define DEFAULT_BTREE_ORDER 3


template <typename keyType, typename ObjIDType>
class BTree;

template <typename keyType, typename ObjIDType>
class ForwardBTreeIterator;

template <typename keyType, typename ObjIDType>
class BackwardBTreeIterator;

template <typename keyType, typename ObjIDType>
ostream &operator<<(ostream &os, BTree<keyType, ObjIDType> &tree);

template <typename keyType, typename ObjIDType>
istream &operator>>(istream &is, BTree<keyType, ObjIDType> &tree);


template <typename keyType, typename ObjIDType = long>
class BTree 
{
       typedef CBTreePage<keyType, ObjIDType> BTNode;

public:
       
       using key_type = keyType;
       using obj_id_type = ObjIDType;
       using size_type = Size;
       using long_size_type = long;
       using forward_iterator = ForwardBTreeIterator<keyType, ObjIDType>;
       using backward_iterator = BackwardBTreeIterator<keyType, ObjIDType>;

       
       typedef typename BTNode::lpfnForEach2    lpfnForEach2;
       typedef typename BTNode::lpfnForEach3    lpfnForEach3;
       typedef typename BTNode::lpfnFirstThat2  lpfnFirstThat2;
       typedef typename BTNode::lpfnFirstThat3  lpfnFirstThat3;
       typedef typename BTNode::ObjectInfo      ObjectInfo;

public:
       
       BTree(Size order = DEFAULT_BTREE_ORDER, bool unique = true);
       BTree(BTree &&another) noexcept;                    // Move constructor
       ~BTree();

       
       bool            Insert (const keyType key, const ObjIDType ObjID);
       bool            Remove (const keyType key, const ObjIDType ObjID);
       ObjIDType       Search (const keyType key);
       
      
       long            size();
       long            height();
       long            GetOrder();

       
       void            Print (ostream &os);

       
       void            ForEach( lpfnForEach2 lpfn, void *pExtra1 );
       void            ForEach( lpfnForEach3 lpfn, void *pExtra1, void *pExtra2);
       ObjectInfo*     FirstThat( lpfnFirstThat2 lpfn, void *pExtra1 );
       ObjectInfo*     FirstThat( lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2);

       
       template <typename Func, typename ...Args>
       void            ForEach( forward_iterator start, forward_iterator end, Func func, Args&& ...args );
       
       template <typename Func, typename ...Args>
       ObjectInfo*     FirstThat( forward_iterator start, forward_iterator end, Func func, Args&& ...args);
       
       
       template <typename Func, typename ...Args>
       void            inorderTraversal(Func func, Args&& ...args);
       
       template <typename Func, typename ...Args>
       void            preorderTraversal(Func func, Args&& ...args);
       
       template <typename Func, typename ...Args>
       void            postorderTraversal(Func func, Args&& ...args);

       // Iteradores
       forward_iterator begin();
       forward_iterator end();
       backward_iterator rbegin();
       backward_iterator rend();

protected:
       mutable mutex   m_Mutex;
       BTNode          m_Root;
       long            m_NumKeys;
       bool            m_Unique;
       long            m_Order;
       long            m_Height;

       friend class ForwardBTreeIterator<keyType, ObjIDType>;
       friend class BackwardBTreeIterator<keyType, ObjIDType>;
       friend ostream &operator<< <>(ostream &os, BTree<keyType, ObjIDType> &tree);
       friend istream &operator>> <>(istream &is, BTree<keyType, ObjIDType> &tree);

private:
       // Métodos privados
       bool _insert_unlocked(keyType key, ObjIDType ObjID);
       void _clear_unlocked();
       static void _serialize_page(ostream &os, BTNode *page, bool &isFirst);
       
       template <typename Func, typename ...Args>
       static void _inorderTraversal(BTNode *page, Func &func, Args&&...args);
       
       template <typename Func, typename ...Args>
       static void _preorderTraversal(BTNode *page, Func &func, Args&&...args);
       
       template <typename Func, typename ...Args>
       static void _postorderTraversal(BTNode *page, Func &func, Args&&...args);
};



const Size MaxHeight = 5;

// Constructor normal
template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::BTree(Size order, bool unique)
       : m_Root(2 * order  + 1, unique),
         m_NumKeys(0),
         m_Unique(unique),
         m_Order(order)
{
       m_Root.SetMaxKeysForChilds(static_cast<int>(order));
       m_Height = 1;
}

// Move constructor
template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::BTree(BTree &&another) noexcept 
       : m_Root(2 * (int)DEFAULT_BTREE_ORDER + 1, true),
         m_NumKeys(0),
         m_Unique(true),
         m_Order(DEFAULT_BTREE_ORDER),
         m_Height(1)
{
       lock_guard<mutex> lock(another.m_Mutex);
       
      
       m_NumKeys = exchange(another.m_NumKeys, 0);
       m_Unique = exchange(another.m_Unique, true);
       m_Order = exchange(another.m_Order, DEFAULT_BTREE_ORDER);
       m_Height = exchange(another.m_Height, 1);

       
       swap(m_Root.m_MinKeys, another.m_Root.m_MinKeys);
       swap(m_Root.m_MaxKeys, another.m_Root.m_MaxKeys);
       swap(m_Root.m_MaxKeysForChilds, another.m_Root.m_MaxKeysForChilds);
       swap(m_Root.m_Unique, another.m_Root.m_Unique);
       swap(m_Root.m_isRoot, another.m_Root.m_isRoot);
       swap(m_Root.m_Keys, another.m_Root.m_Keys);
       swap(m_Root.m_SubPages, another.m_Root.m_SubPages);
       swap(m_Root.m_KeyCount, another.m_Root.m_KeyCount);
}

// Destructor
template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::~BTree()
{
       lock_guard<mutex> lock(m_Mutex);
       _clear_unlocked();
}


template <typename keyType, typename ObjIDType>
long BTree<keyType, ObjIDType>::size() {
       lock_guard<mutex> lock(m_Mutex);
       return m_NumKeys;
}

template <typename keyType, typename ObjIDType>
long BTree<keyType, ObjIDType>::height() {
       lock_guard<mutex> lock(m_Mutex);
       return m_Height;
}

template <typename keyType, typename ObjIDType>
long BTree<keyType, ObjIDType>::GetOrder() {
       lock_guard<mutex> lock(m_Mutex);
       return m_Order;
}


template<typename keyType, typename ObjIDType>
template<typename Func, typename ...Args>
void BTree<keyType, ObjIDType>::ForEach(forward_iterator start, forward_iterator end, Func func, Args &&...args) {
       ::Foreach(start, end, func, forward<Args>(args)...);
}


template<typename keyType, typename ObjIDType>
template<typename Func, typename... Args>
typename BTree<keyType, ObjIDType>::ObjectInfo *
BTree<keyType, ObjIDType>::FirstThat(forward_iterator start, forward_iterator end, Func func, Args &&... args) {
       auto it = ::FirstThat(start, end, func, forward<Args>(args)...);
       return (it == end) ? nullptr : &(*it);
}


template <typename keyType, typename ObjIDType>
template <typename Func, typename ...Args>
void BTree<keyType, ObjIDType>::inorderTraversal(Func func, Args &&...args) {
       lock_guard<mutex> lock(m_Mutex);
       _inorderTraversal(&m_Root, func, forward<Args>(args)...);
}

template <typename keyType, typename ObjIDType>
template <typename Func, typename ...Args>
void BTree<keyType, ObjIDType>::preorderTraversal(Func func, Args &&...args) {
       lock_guard<mutex> lock(m_Mutex);
       _preorderTraversal(&m_Root, func, forward<Args>(args)...);
}

template <typename keyType, typename ObjIDType>
template <typename Func, typename ...Args>
void BTree<keyType, ObjIDType>::postorderTraversal(Func func, Args &&...args) {
       lock_guard<mutex> lock(m_Mutex);
       _postorderTraversal(&m_Root, func, forward<Args>(args)...);
}


template <typename keyType, typename ObjIDType>
template <typename Func, typename ...Args>
void BTree<keyType, ObjIDType>::_inorderTraversal(BTNode *page, Func &func, Args&&...args) {
       if (!page || page->m_KeyCount == 0) return;
       
       for (Size i = 0; i < (Size)page->m_KeyCount; ++i) {
               _inorderTraversal(page->m_SubPages[i], func, forward<Args>(args)...);
               func(page->m_Keys[i], forward<Args>(args)...);
       }
       _inorderTraversal(page->m_SubPages[page->m_KeyCount], func, forward<Args>(args)...);
}

template <typename keyType, typename ObjIDType>
template <typename Func, typename ...Args>
void BTree<keyType, ObjIDType>::_preorderTraversal(BTNode *page, Func &func, Args&&...args) {
       if (!page || page->m_KeyCount == 0) return;
       
       for (Size i = 0; i < (Size)page->m_KeyCount; ++i) {
               func(page->m_Keys[i], forward<Args>(args)...);
       }
       for (Size i = 0; i <= (Size)page->m_KeyCount; ++i) {
               _preorderTraversal(page->m_SubPages[i], func, forward<Args>(args)...);
       }
}

template <typename keyType, typename ObjIDType>
template <typename Func, typename ...Args>
void BTree<keyType, ObjIDType>::_postorderTraversal(BTNode *page, Func &func, Args&&...args) {
       if (!page || page->m_KeyCount == 0) return;
       
       for (Size i = 0; i <= (Size)page->m_KeyCount; ++i) {
               _postorderTraversal(page->m_SubPages[i], func, forward<Args>(args)...);
       }
       for (Size i = 0; i < (Size)page->m_KeyCount; ++i) {
               func(page->m_Keys[i], forward<Args>(args)...);
       }
}


template <typename keyType, typename ObjIDType>
bool BTree<keyType, ObjIDType>::Insert(const keyType key, const ObjIDType ObjID)
{
       lock_guard<mutex> lock(m_Mutex);
       return _insert_unlocked(key, ObjID);
}


template <typename keyType, typename ObjIDType>
bool BTree<keyType, ObjIDType>::_insert_unlocked(const keyType key, const ObjIDType ObjID) {
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
       lock_guard<mutex> lock(m_Mutex);
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
       lock_guard<mutex> lock(m_Mutex);
       long ObjID = 0;
       m_Root.Search(key, ObjID);
       return static_cast<ObjIDType>(ObjID);
}


template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::ForEach(lpfnForEach2 lpfn, void *pExtra1)
{
       lock_guard<mutex> lock(m_Mutex);
       m_Root.ForEach(lpfn, 0, pExtra1);
}

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::ForEach(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2)
{
       lock_guard<mutex> lock(m_Mutex);
       m_Root.ForEach(lpfn, 0, pExtra1, pExtra2);
}


template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::ObjectInfo *
BTree<keyType, ObjIDType>::FirstThat(lpfnFirstThat2 lpfn, void *pExtra1)
{
       lock_guard<mutex> lock(m_Mutex);
       return m_Root.FirstThat(lpfn, 0, pExtra1);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::ObjectInfo *
BTree<keyType, ObjIDType>::FirstThat(lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2)
{
       lock_guard<mutex> lock(m_Mutex);
       return m_Root.FirstThat(lpfn, 0, pExtra1, pExtra2);
}


template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::Print(ostream &os){
       lock_guard<mutex> lock(m_Mutex);
       m_Root.Print(os);
}


template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::_clear_unlocked() {
       m_Root.Reset();
       m_NumKeys = 0;
       m_Height = 1;
       m_Root.SetMaxKeysForChilds(static_cast<int>(m_Order));
}


template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::_serialize_page(ostream &os, BTNode *page, bool &isFirst) {
       if (!page || page->m_KeyCount == 0) return;
       
       for (Size i = 0; i < (Size)page->m_KeyCount; ++i) {
               _serialize_page(os, page->m_SubPages[i], isFirst);
               if (!isFirst) os << ",";
               os << "(";
               if constexpr (is_same_v<keyType, string>) os << quoted(page->m_Keys[i].key);
               else os << page->m_Keys[i].key;
               os << ":";
               if constexpr (is_same_v<ObjIDType, string>) os << quoted(page->m_Keys[i].ObjID);
               else os << page->m_Keys[i].ObjID;
               os << ")";
               isFirst = false;
       }
       _serialize_page(os, page->m_SubPages[page->m_KeyCount], isFirst);
}


template <typename keyType, typename ObjIDType>
ostream &operator<<(ostream &os, BTree<keyType, ObjIDType> &tree) {
       lock_guard<mutex> lock(tree.m_Mutex);
       os << "BTree[";
       bool isFirst = true;
       BTree<keyType, ObjIDType>::_serialize_page(os, &tree.m_Root, isFirst);
       os << "]";
       return os;
}

// Operator >>
template <typename keyType, typename ObjIDType>
istream &operator>>(istream &is, BTree<keyType, ObjIDType> &tree) {
       if (!is) return is;

       string bar;
       getline(is, bar, '[');
       lock_guard<mutex> lock(tree.m_Mutex);
       tree._clear_unlocked();

       is >> ws;
       if (!is) {
               tree._clear_unlocked();
               return is;
       }
       if (is.peek() == ']') {
               is.get();
               return is;
       }

       while (is) {
               is >> ws;
               char ch = 0;
               if (!is.get(ch) || ch != '(') {
                       is.setstate(ios::failbit);
                       break;
               }
               keyType key {};
               ObjIDType objId {};
               if constexpr (is_same_v<keyType, string>) is >> quoted(key);
               else is >> key;
               if (!is) break;
               is >> ws;
               if (!is.get(ch) || ch != ':') {
                       is.setstate(ios::failbit);
                       break;
               }
               if constexpr (is_same_v<ObjIDType, string>) is >> quoted(objId);
               else is >> objId;
               if (!is) break;
               is >> ws;
               if (!is.get(ch) || ch != ')') {
                       is.setstate(ios::failbit);
                       break;
               }

               if (!tree._insert_unlocked(key, objId)) {
                       is.setstate(ios::failbit);
                       break;
               }

               is >> ws;
               const int next = is.peek();
               if (next == ',') {
                       is.get();
                       continue;
               }
               if (next == ']') {
                       is.get();
                       break;
               }
               is.setstate(ios::failbit);
               break;
       }

       if (!is) tree._clear_unlocked();
       return is;
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
                       if (f.keyIndex < (int)f.page->m_KeyCount) {
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
               const int keyIndex = f.keyIndex;
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
                       m_stack.push_back({page, (int)page->m_KeyCount - 1});
                       page = page->m_SubPages[page->m_KeyCount];
               }
       }

       void settle() {
               while (!m_stack.empty()) {
                       Frame &f = m_stack.back();
                       if ((int)f.keyIndex >= 0) {
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
               const int keyIndex = f.keyIndex;
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