// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <utility>
#include <algorithm>
#include <mutex>
#include <memory>

#include "BTreePage.h"
#include "BTreeIterator.h"

#define DEFAULT_BTREE_ORDER 3

template <typename keyType, typename ObjIDType = long>
class BTree
// Full version of the BTree
{
       typedef CBTreePage<keyType, ObjIDType> BTNode;

public:
       typedef typename BTNode::lpfnForEach2    lpfnForEach2;
       typedef typename BTNode::lpfnForEach3    lpfnForEach3;
       typedef typename BTNode::lpfnFirstThat2  lpfnFirstThat2;
       typedef typename BTNode::lpfnFirstThat3  lpfnFirstThat3;
       typedef typename BTNode::ObjectInfo      ObjectInfo;

       typedef BTreeSnapshotIterator<ObjectInfo> iterator;
       typedef std::reverse_iterator<iterator>   reverse_iterator;

public:
       BTree(int order = DEFAULT_BTREE_ORDER, bool unique = true);
       ~BTree();

       // Move semantics
       BTree(BTree&& other) noexcept;
       BTree& operator=(BTree&& other) noexcept;

       // Disable copy (important with raw pointers inside pages)
       BTree(const BTree&) = delete;
       BTree& operator=(const BTree&) = delete;

       bool      Insert(const keyType key, const ObjIDType ObjID);
       bool      Remove(const keyType key, const ObjIDType ObjID);
       ObjIDType Search(const keyType key);

       template <typename... Pairs>
       void InsertMany(const Pairs&... pairs);

       bool operator<(const BTree& other) const;
       bool operator>(const BTree& other) const;

       long size() const
       {
              std::lock_guard<std::recursive_mutex> lock(m_Mutex);
              return m_NumKeys;
       }

       long height() const
       {
              std::lock_guard<std::recursive_mutex> lock(m_Mutex);
              return m_Height;
       }

       long GetOrder() const
       {
              std::lock_guard<std::recursive_mutex> lock(m_Mutex);
              return (long)m_Order;
       }

       void Print(std::ostream &os);

       void ForEach(lpfnForEach2 lpfn, void *pExtra1);
       void ForEach(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2);

       iterator begin();
       iterator end();
       reverse_iterator rbegin();
       reverse_iterator rend();

       ObjectInfo* FirstThat(lpfnFirstThat2 lpfn, void *pExtra1);
       ObjectInfo* FirstThat(lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2);

       // Variadic traversals
       template <typename Func, typename... Args>
       void InOrder(Func&& func, Args&&... args);

       template <typename Func, typename... Args>
       void PreOrder(Func&& func, Args&&... args);

       template <typename Func, typename... Args>
       void PostOrder(Func&& func, Args&&... args);

       template <typename Pred, typename... Args>
       ObjectInfo* FirstThatV(Pred&& pred, Args&&... args);

protected:
       mutable std::recursive_mutex m_Mutex;

       BTNode m_Root;
       long   m_NumKeys;  // number of keys
       bool   m_Unique;   // accept repeated keys?
       int    m_Order;    // tree order
       int    m_Height;   // tree height

       // Snapshot for iterators
       mutable std::shared_ptr<std::vector<ObjectInfo> > m_IterSnapshot;

       void BuildIteratorSnapshot();
       void InvalidateIteratorSnapshot();

       static void CollectSnapshotItem(ObjectInfo &info, int level, void *pExtra1);
};

const int MaxHeight = 5;

// -----------------------------------------------------------------------------
// Constructor / Destructor
// -----------------------------------------------------------------------------

template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::BTree(int order, bool unique)
       : m_Root(2 * order + 1, unique),
         m_NumKeys(0),
         m_Unique(unique),
         m_Order(order),
         m_Height(1)
{
       if (order < 3)
              throw std::invalid_argument("BTree order must be >= 3");

       m_Root.SetMaxKeysForChilds(order);
}

template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::~BTree()
{
}

// -----------------------------------------------------------------------------
// Move semantics
// -----------------------------------------------------------------------------

template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::BTree(BTree&& other) noexcept
       : m_Root(2 * other.m_Order + 1, other.m_Unique),
         m_NumKeys(0),
         m_Unique(other.m_Unique),
         m_Order(other.m_Order),
         m_Height(1)
{
       std::lock_guard<std::recursive_mutex> lock(other.m_Mutex);

       m_Root = std::move(other.m_Root);
       m_NumKeys = other.m_NumKeys;
       m_Height = other.m_Height;
       m_IterSnapshot = std::move(other.m_IterSnapshot);

       // leave source in valid state
       other.m_NumKeys = 0;
       other.m_Height = 1;
       other.m_IterSnapshot.reset();
}

template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>&
BTree<keyType, ObjIDType>::operator=(BTree&& other) noexcept
{
       if (this == &other)
              return *this;

       std::scoped_lock<std::recursive_mutex, std::recursive_mutex> lock(m_Mutex, other.m_Mutex);

       m_Root = std::move(other.m_Root);
       m_NumKeys = other.m_NumKeys;
       m_Unique = other.m_Unique;
       m_Order = other.m_Order;
       m_Height = other.m_Height;
       m_IterSnapshot = std::move(other.m_IterSnapshot);

       other.m_NumKeys = 0;
       other.m_Height = 1;
       other.m_IterSnapshot.reset();

       return *this;
}

// -----------------------------------------------------------------------------
// Basic operations
// -----------------------------------------------------------------------------

template <typename keyType, typename ObjIDType>
bool BTree<keyType, ObjIDType>::Insert(const keyType key, const ObjIDType ObjID)
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);

       bt_ErrorCode error = m_Root.Insert(key, ObjID);
       if (error == bt_duplicate)
              return false;

       m_NumKeys++;
       InvalidateIteratorSnapshot();

       if (error == bt_overflow)
       {
              m_Root.SplitRoot();
              m_Height++;
       }

       return true;
}

template <typename keyType, typename ObjIDType>
bool BTree<keyType, ObjIDType>::Remove(const keyType key, const ObjIDType ObjID)
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);

       bt_ErrorCode error = m_Root.Remove(key, ObjID);
       if (error == bt_duplicate || error == bt_nofound)
              return false;

       m_NumKeys--;
       InvalidateIteratorSnapshot();

       if (error == bt_rootmerged)
              m_Height--;

       return true;
}

template <typename keyType, typename ObjIDType>
ObjIDType BTree<keyType, ObjIDType>::Search(const keyType key)
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);

       ObjIDType ObjID{};
       m_Root.Search(key, ObjID);
       return ObjID;
}

// -----------------------------------------------------------------------------
// Traversals (classic callbacks from professor code)
// -----------------------------------------------------------------------------

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::ForEach(lpfnForEach2 lpfn, void *pExtra1)
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);
       m_Root.ForEach(lpfn, 0, pExtra1);
}

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::ForEach(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2)
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);
       m_Root.ForEach(lpfn, 0, pExtra1, pExtra2);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::ObjectInfo*
BTree<keyType, ObjIDType>::FirstThat(lpfnFirstThat2 lpfn, void *pExtra1)
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);
       return m_Root.FirstThat(lpfn, 0, pExtra1);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::ObjectInfo*
BTree<keyType, ObjIDType>::FirstThat(lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2)
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);
       return m_Root.FirstThat(lpfn, 0, pExtra1, pExtra2);
}

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::Print(std::ostream &os)
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);
       m_Root.Print(os);
}

// -----------------------------------------------------------------------------
// Iterator snapshot helpers
// -----------------------------------------------------------------------------

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::InvalidateIteratorSnapshot()
{
       m_IterSnapshot.reset();
}

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::CollectSnapshotItem(ObjectInfo &info, int /*level*/, void *pExtra1)
{
       std::vector<ObjectInfo> *pVec = (std::vector<ObjectInfo> *)pExtra1;
       pVec->push_back(info);
}

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::BuildIteratorSnapshot()
{
       if (m_IterSnapshot)
              return; // already built

       m_IterSnapshot = std::shared_ptr<std::vector<ObjectInfo> >(new std::vector<ObjectInfo>());
       if (m_NumKeys > 0)
              m_IterSnapshot->reserve((size_t)m_NumKeys);

       // reuse professor inorder traversal
       m_Root.ForEach(&BTree<keyType, ObjIDType>::CollectSnapshotItem, 0, m_IterSnapshot.get());
}

// -----------------------------------------------------------------------------
// Iterators
// -----------------------------------------------------------------------------

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::iterator BTree<keyType, ObjIDType>::begin()
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);
       BuildIteratorSnapshot();
       return iterator(m_IterSnapshot, 0);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::iterator BTree<keyType, ObjIDType>::end()
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);
       BuildIteratorSnapshot();
       return iterator(m_IterSnapshot, m_IterSnapshot->size());
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::reverse_iterator BTree<keyType, ObjIDType>::rbegin()
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);
       BuildIteratorSnapshot();
       return reverse_iterator(iterator(m_IterSnapshot, m_IterSnapshot->size()));
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::reverse_iterator BTree<keyType, ObjIDType>::rend()
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);
       BuildIteratorSnapshot();
       return reverse_iterator(iterator(m_IterSnapshot, 0));
}

// -----------------------------------------------------------------------------
// Variadic traversals
// -----------------------------------------------------------------------------

template <typename keyType, typename ObjIDType>
template <typename Func, typename... Args>
void BTree<keyType, ObjIDType>::InOrder(Func&& func, Args&&... args)
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);
       m_Root.InOrderV(func, 0, args...);
}

template <typename keyType, typename ObjIDType>
template <typename Func, typename... Args>
void BTree<keyType, ObjIDType>::PreOrder(Func&& func, Args&&... args)
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);
       m_Root.PreOrderV(func, 0, args...);
}

template <typename keyType, typename ObjIDType>
template <typename Func, typename... Args>
void BTree<keyType, ObjIDType>::PostOrder(Func&& func, Args&&... args)
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);
       m_Root.PostOrderV(func, 0, args...);
}

template <typename keyType, typename ObjIDType>
template <typename Pred, typename... Args>
typename BTree<keyType, ObjIDType>::ObjectInfo*
BTree<keyType, ObjIDType>::FirstThatV(Pred&& pred, Args&&... args)
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);
       return m_Root.FirstThatV(pred, 0, args...);
}

// -----------------------------------------------------------------------------
// Variadic insert
// -----------------------------------------------------------------------------

template <typename keyType, typename ObjIDType>
template <typename... Pairs>
void BTree<keyType, ObjIDType>::InsertMany(const Pairs&... pairs)
{
       std::lock_guard<std::recursive_mutex> lock(m_Mutex);
       (Insert(pairs.first, pairs.second), ...); // recursive_mutex allows nested lock
}

// -----------------------------------------------------------------------------
// Comparison operators
// -----------------------------------------------------------------------------

template <typename keyType, typename ObjIDType>
bool BTree<keyType, ObjIDType>::operator<(const BTree& other) const
{
       if (this == &other)
              return false;

       std::scoped_lock<std::recursive_mutex, std::recursive_mutex> lock(m_Mutex, other.m_Mutex);

       // begin/end are non-const because they build snapshots; build snapshots directly
       BTree<keyType, ObjIDType>& selfRef  = const_cast<BTree<keyType, ObjIDType>&>(*this);
       BTree<keyType, ObjIDType>& otherRef = const_cast<BTree<keyType, ObjIDType>&>(other);

       selfRef.BuildIteratorSnapshot();
       otherRef.BuildIteratorSnapshot();

       return std::lexicographical_compare(
              selfRef.m_IterSnapshot->begin(), selfRef.m_IterSnapshot->end(),
              otherRef.m_IterSnapshot->begin(), otherRef.m_IterSnapshot->end(),
              [](const ObjectInfo& a, const ObjectInfo& b)
              {
                     if (a.key < b.key) return true;
                     if (b.key < a.key) return false;
                     return a.ObjID < b.ObjID;
              }
       );
}

template <typename keyType, typename ObjIDType>
bool BTree<keyType, ObjIDType>::operator>(const BTree& other) const
{
       return other < *this;
}

#endif // BTREE_H