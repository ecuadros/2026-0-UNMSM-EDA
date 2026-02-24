// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <mutex>
#include <vector>
#include <utility>
#include <sstream>
#include "BTreePage.h"
#include "../util.h"

#define DEFAULT_BTREE_ORDER 3

// ForwardIterator
template <typename keyType, typename ObjIDType>
struct BTreeFwdIterator {
       using BTPage     = CBTreePage<keyType, ObjIDType>;
       using ObjectInfo = tagObjectInfo<keyType, ObjIDType>;
       vector<pair<BTPage*, int>> m_stack;

       BTreeFwdIterator() {}
       virtual ~BTreeFwdIterator() {}

       void descendLeft(BTPage* page) {
               while (page) {
                       m_stack.push_back({page, 0});
                       page = page->m_SubPages[0];
               }
       }

       BTreeFwdIterator& operator++() {
               if (m_stack.empty()) return *this;
               auto& top = m_stack.back();
               top.second++;
               if (top.first->m_SubPages[top.second]) {
                       BTPage* child = top.first->m_SubPages[top.second];
                       while (child) {
                               m_stack.push_back({child, 0});
                               child = child->m_SubPages[0];
                       }
               } else {
                       while (!m_stack.empty() &&
                              m_stack.back().second >= m_stack.back().first->m_KeyCount) {
                               m_stack.pop_back();
                       }
               }
               return *this;
       }

       bool operator!=(const BTreeFwdIterator& other) const {
               if (m_stack.empty() && other.m_stack.empty()) return false;
               if (m_stack.empty() || other.m_stack.empty()) return true;
               return m_stack.back() != other.m_stack.back();
       }

       keyType& operator*() {
               return m_stack.back().first->m_Keys[m_stack.back().second].key;
       }
};

// BackwardIterator
template <typename keyType, typename ObjIDType>
struct BTreeBwdIterator {
       using BTPage     = CBTreePage<keyType, ObjIDType>;
       using ObjectInfo = tagObjectInfo<keyType, ObjIDType>;
       vector<pair<BTPage*, int>> m_stack;

       BTreeBwdIterator() {}
       virtual ~BTreeBwdIterator() {}

       void descendRight(BTPage* page) {
               while (page) {
                       int pos = page->m_KeyCount - 1;
                       m_stack.push_back({page, pos});
                       page = page->m_SubPages[page->m_KeyCount];
               }
       }

       BTreeBwdIterator& operator++() {
               if (m_stack.empty()) return *this;
               auto& top = m_stack.back();
               if (top.first->m_SubPages[top.second]) {
                       BTPage* child = top.first->m_SubPages[top.second];
                       top.second--;
                       while (child) {
                               int pos = child->m_KeyCount - 1;
                               m_stack.push_back({child, pos});
                               child = child->m_SubPages[child->m_KeyCount];
                       }
               } else {
                       top.second--;
                       while (!m_stack.empty() && m_stack.back().second < 0) {
                               m_stack.pop_back();
                       }
               }
               return *this;
       }

       bool operator!=(const BTreeBwdIterator& other) const {
               if (m_stack.empty() && other.m_stack.empty()) return false;
               if (m_stack.empty() || other.m_stack.empty()) return true;
               return m_stack.back() != other.m_stack.back();
       }

       keyType& operator*() {
               return m_stack.back().first->m_Keys[m_stack.back().second].key;
       }
};

template <typename keyType, typename ObjIDType = long>
class BTree 
// this is the full version of the BTree
{
       typedef CBTreePage <keyType, ObjIDType> BTNode;// useful shorthand
       /*struct ObjectInfo
       {
               keyType first;
               long    second;
               ObjectInfo *&operator->() { return this; }
       };*/

public:
       //typedef ObjectInfo iterator;
       typedef typename BTNode::lpfnForEach2    lpfnForEach2;
       typedef typename BTNode::lpfnForEach3    lpfnForEach3;
       typedef typename BTNode::lpfnFirstThat2  lpfnFirstThat2;
       typedef typename BTNode::lpfnFirstThat3  lpfnFirstThat3;
       typedef typename BTNode::ObjectInfo      ObjectInfo;

       // iterator types
       using forward_iterator  = BTreeFwdIterator<keyType, ObjIDType>;
       using backward_iterator = BTreeBwdIterator<keyType, ObjIDType>;
       using iterator          = forward_iterator;

public:
       // constructor
       BTree(int order = DEFAULT_BTREE_ORDER, bool unique = true);

       // copy constructor
       BTree(const BTree &otro);

       // move constructor
       BTree(BTree &&otro) noexcept;

       // destructor seguro
       virtual ~BTree();
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
       ObjectInfo*     FirstThat( lpfnFirstThat2 lpfn, void *pExtra1 );
       ObjectInfo*     FirstThat( lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2);
       //typedef               ObjectInfo iterator;

       // forward iterator begin
       forward_iterator begin() {
               forward_iterator it;
               if (m_NumKeys > 0) it.descendLeft(&m_Root);
               return it;
       }
       forward_iterator end() { return forward_iterator(); }

       // backward iterator rbegin
       backward_iterator rbegin() {
               backward_iterator it;
               if (m_NumKeys > 0) it.descendRight(&m_Root);
               return it;
       }
       backward_iterator rend() { return backward_iterator(); }

       // inorder
       template <typename ObjFunc, typename ...Args>
       void Inorder(ObjFunc of, Args... args) {
               lock_guard<mutex> lock(m_mtx);
               InternalInorder(&m_Root, of, args...);
       }

       // preorder
       template <typename ObjFunc, typename ...Args>
       void Preorder(ObjFunc of, Args... args) {
               lock_guard<mutex> lock(m_mtx);
               InternalPreorder(&m_Root, of, args...);
       }

       // foreach
       template <typename ObjFunc, typename ...Args>
       void Foreach(ObjFunc of, Args... args) {
               ::Foreach(*this, of, args...);
       }

       // firstthat
       template <typename ObjFunc, typename ...Args>
       auto FirstThatV(ObjFunc of, Args... args) {
               return ::FirstThat(*this, of, args...);
       }

       // operator<<
       friend ostream& operator<<(ostream &os, BTree &container) {
               os << "BTree: size = " << container.size()
                  << ", height = " << container.height()
                  << ", order = " << container.GetOrder() << endl;
               container.m_Root.Print(os);
               return os;
       }

       // operator>>
       friend istream& operator>>(istream &is, BTree &container) {
               size_t sz;
               if (is >> sz) {
                       keyType key;
                       ObjIDType ref;
                       for (size_t i = 0; i < sz; ++i) {
                               is >> key >> ref;
                               container.Insert(key, ref);
                       }
               }
               return is;
       }

protected:
       BTNode          m_Root;
       long            m_NumKeys; // number of keys
       bool            m_Unique;  // Accept the elements only once ?
       int             m_Order;   // order of tree
       int             m_Height;  // height of tree
       mutable mutex   m_mtx;

       // deep copy
       BTNode* InternalDeepCopy(BTNode* pSrc);

       // inorder
       template <typename ObjFunc, typename ...Args>
       void InternalInorder(BTNode *pPage, ObjFunc of, Args... args) {
               if (!pPage) return;
               for (int i = 0; i < pPage->m_KeyCount; i++) {
                       if (pPage->m_SubPages[i])
                               InternalInorder(pPage->m_SubPages[i], of, args...);
                       of(pPage->m_Keys[i].key, args...);
               }
               if (pPage->m_SubPages[pPage->m_KeyCount])
                       InternalInorder(pPage->m_SubPages[pPage->m_KeyCount], of, args...);
       }

       // preorder
       template <typename ObjFunc, typename ...Args>
       void InternalPreorder(BTNode *pPage, ObjFunc of, Args... args) {
               if (!pPage) return;
               for (int i = 0; i < pPage->m_KeyCount; i++)
                       of(pPage->m_Keys[i].key, args...);
               for (int i = 0; i <= pPage->m_KeyCount; i++) {
                       if (pPage->m_SubPages[i])
                               InternalPreorder(pPage->m_SubPages[i], of, args...);
               }
       }
};

const int MaxHeight = 5;
template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::BTree(int order, bool unique)
                               : m_Root(2 * order  + 1, unique),
                                 m_NumKeys(0),
                                 m_Unique(unique),
                                 m_Order(order)
{
       m_Root.SetMaxKeysForChilds(order);
       m_Height = 1;
}

// copy constructor
template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::BTree(const BTree &otro)
                                : m_Root(otro.m_Root.m_MaxKeys, otro.m_Unique),
                                  m_NumKeys(0),
                                  m_Unique(otro.m_Unique),
                                  m_Order(otro.m_Order),
                                  m_Height(otro.m_Height)
{
       lock_guard<mutex> lock(otro.m_mtx);
       m_Root.SetMaxKeysForChilds(otro.m_Root.m_MaxKeysForChilds);
       for (int i = 0; i < otro.m_Root.m_KeyCount; i++) {
               m_Root.m_Keys[i]     = otro.m_Root.m_Keys[i];
               m_Root.m_SubPages[i] = InternalDeepCopy(otro.m_Root.m_SubPages[i]);
       }
       m_Root.m_SubPages[otro.m_Root.m_KeyCount] = InternalDeepCopy(otro.m_Root.m_SubPages[otro.m_Root.m_KeyCount]);
       m_Root.m_KeyCount = otro.m_Root.m_KeyCount;
       m_NumKeys = otro.m_NumKeys;
}

// move constructor
template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::BTree(BTree &&otro) noexcept
                                : m_Root(otro.m_Root.m_MaxKeys, otro.m_Unique),
                                  m_NumKeys(otro.m_NumKeys),
                                  m_Unique(otro.m_Unique),
                                  m_Order(otro.m_Order),
                                  m_Height(otro.m_Height)
{
       lock_guard<mutex> lock(otro.m_mtx);
       m_Root.m_Keys     = std::move(otro.m_Root.m_Keys);
       m_Root.m_SubPages = std::move(otro.m_Root.m_SubPages);
       m_Root.m_KeyCount = otro.m_Root.m_KeyCount;
       m_Root.m_MaxKeysForChilds = otro.m_Root.m_MaxKeysForChilds;
       otro.m_Root.Create();
       otro.m_NumKeys = 0;
       otro.m_Height  = 1;
}

// destructor seguro
template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::~BTree()
{
       lock_guard<mutex> lock(m_mtx);
}

template <typename keyType, typename ObjIDType>
bool BTree<keyType, ObjIDType>::Insert(const keyType key, const int ObjID)
{
       lock_guard<mutex> lock(m_mtx);
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
bool BTree<keyType, ObjIDType>::Remove (const keyType key, const int ObjID)
{
       lock_guard<mutex> lock(m_mtx);
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
       lock_guard<mutex> lock(m_mtx);
       ObjIDType ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}


template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::ForEach(lpfnForEach2 lpfn, void *pExtra1)
{
       lock_guard<mutex> lock(m_mtx);
       m_Root.ForEach(lpfn, 0, pExtra1);
}

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::ForEach(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2)
{
       lock_guard<mutex> lock(m_mtx);
       m_Root.ForEach(lpfn, 0, pExtra1, pExtra2);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::ObjectInfo *
BTree<keyType, ObjIDType>::FirstThat(lpfnFirstThat2 lpfn, void *pExtra1)
{
       lock_guard<mutex> lock(m_mtx);
       return m_Root.FirstThat(lpfn, 0, pExtra1);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::ObjectInfo *
BTree<keyType, ObjIDType>::FirstThat(lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2)
{
       lock_guard<mutex> lock(m_mtx);
       return m_Root.FirstThat(lpfn, 0, pExtra1, pExtra2);
}

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::Print(ostream &os){
       lock_guard<mutex> lock(m_mtx);
       m_Root.Print(os);
}

// deep copy
template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::BTNode*
BTree<keyType, ObjIDType>::InternalDeepCopy(BTNode* pSrc)
{
       if (!pSrc) return nullptr;
       BTNode* pDest = new BTNode(pSrc->m_MaxKeys, pSrc->m_Unique);
       pDest->SetMaxKeysForChilds(pSrc->m_MaxKeysForChilds);
       for (int i = 0; i < pSrc->m_KeyCount; i++) {
               pDest->m_Keys[i]     = pSrc->m_Keys[i];
               pDest->m_SubPages[i] = InternalDeepCopy(pSrc->m_SubPages[i]);
       }
       pDest->m_SubPages[pSrc->m_KeyCount] = InternalDeepCopy(pSrc->m_SubPages[pSrc->m_KeyCount]);
       pDest->m_KeyCount = pSrc->m_KeyCount;
       return pDest;
}

void DemoBTree();

#endif