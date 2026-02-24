// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <mutex>
#include <vector>
#include <memory>
#include "BTreePage.h"

#define DEFAULT_BTREE_ORDER 3

template <typename keyType, typename ObjIDType = long>
class BTree
{
    typedef CBTreePage<keyType, ObjIDType> BTNode;

public:
    typedef typename BTNode::lpfnForEach2   lpfnForEach2;
    typedef typename BTNode::lpfnForEach3   lpfnForEach3;
    typedef typename BTNode::lpfnFirstThat2 lpfnFirstThat2;
    typedef typename BTNode::lpfnFirstThat3 lpfnFirstThat3;
    typedef typename BTNode::ObjectInfo     ObjectInfo;

    // Forward Iterator
    class ForwardIterator {
        std::shared_ptr<std::vector<ObjectInfo*>> m_items;
        int m_pos;
    public:
        ForwardIterator(std::shared_ptr<std::vector<ObjectInfo*>> items, int pos)
            : m_items(items), m_pos(pos) {}
        ObjectInfo &operator*()       { return *(*m_items)[m_pos]; }
        ForwardIterator &operator++() { ++m_pos; return *this; }
        bool operator!=(const ForwardIterator &o) const { return m_pos != o.m_pos; }
    };

    // Backward Iterator
    class BackwardIterator {
        std::shared_ptr<std::vector<ObjectInfo*>> m_items;
        int m_pos;
    public:
        BackwardIterator(std::shared_ptr<std::vector<ObjectInfo*>> items, int pos)
            : m_items(items), m_pos(pos) {}
        ObjectInfo &operator*()        { return *(*m_items)[m_pos]; }
        BackwardIterator &operator++() { --m_pos; return *this; }
        bool operator!=(const BackwardIterator &o) const { return m_pos != o.m_pos; }
    };

public:
    BTree(int order = DEFAULT_BTREE_ORDER, bool unique = true);
    BTree(BTree &&another);           // Move Constructor
    BTree(const BTree &) = delete;
    virtual ~BTree();

    bool      Insert(const keyType key, const int ObjID);
    bool      Remove(const keyType key, const int ObjID);
    ObjIDType Search(const keyType key);
    long      size()     { return m_NumKeys; }
    long      height()   { return m_Height;  }
    long      GetOrder() { return m_Order;   }

    void Print(ostream &os);

    void ForEach(lpfnForEach2 lpfn, void *pExtra1);
    void ForEach(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2);

    // ForEach variadic
    template <typename FuncObj, typename ...Args>
    void ForEach(FuncObj fn, Args... args);

    ObjectInfo* FirstThat(lpfnFirstThat2 lpfn, void *pExtra1);
    ObjectInfo* FirstThat(lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2);

    // FirstThat variadic
    template <typename FuncObj, typename ...Args>
    ObjectInfo* FirstThat(FuncObj fn, Args... args);

    template <typename FuncObj, typename ...Args>
    void Inorden(FuncObj fn, Args... args);

    template <typename FuncObj, typename ...Args>
    void Preorden(FuncObj fn, Args... args);

    template <typename FuncObj, typename ...Args>
    void Postorden(FuncObj fn, Args... args);

    ForwardIterator  begin();
    ForwardIterator  end();
    BackwardIterator rbegin();
    BackwardIterator rend();

    // Operator <<
    friend ostream &operator<<(ostream &os, BTree<keyType, ObjIDType> &bt) {
        bt.Print(os);
        return os;
    }

    // Operator >>
    friend istream &operator>>(istream &is, BTree<keyType, ObjIDType> &bt) {
        keyType key; int id;
        while (is >> key >> id)
            bt.Insert(key, id);
        return is;
    }

protected:
    BTNode  m_Root;
    long    m_NumKeys;
    bool    m_Unique;
    int     m_Order;
    int     m_Height;
    mutable std::mutex m_mutex;  // Concurrencia

private:
    void CollectInorden(BTNode *page, std::vector<ObjectInfo*> &items);

    template <typename FuncObj, typename ...Args>
    void InternalInorden(BTNode *page, FuncObj fn, Args... args);

    template <typename FuncObj, typename ...Args>
    void InternalPreorden(BTNode *page, FuncObj fn, Args... args);

    template <typename FuncObj, typename ...Args>
    void InternalPostorden(BTNode *page, FuncObj fn, Args... args);

    template <typename FuncObj, typename ...Args>
    ObjectInfo* InternalFirstThat(BTNode *page, FuncObj fn, Args... args);
};

// Constructor
template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::BTree(int order, bool unique)
    : m_Root(2 * order + 1, unique),
      m_NumKeys(0),
      m_Unique(unique),
      m_Order(order),
      m_Height(1)
{
    m_Root.SetMaxKeysForChilds(order);
}

// Move Constructor
template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::BTree(BTree &&another)
    : m_Root(2 * another.m_Order + 1, another.m_Unique),
      m_NumKeys(another.m_NumKeys),
      m_Unique(another.m_Unique),
      m_Order(another.m_Order),
      m_Height(another.m_Height)
{
    another.m_NumKeys = 0;
    another.m_Height  = 0;
}

// Destructor
template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::~BTree() {}

template <typename keyType, typename ObjIDType>
bool BTree<keyType, ObjIDType>::Insert(const keyType key, const int ObjID) {
    std::lock_guard<std::mutex> lock(m_mutex);
    bt_ErrorCode error = m_Root.Insert(key, ObjID);
    if (error == bt_duplicate) return false;
    m_NumKeys++;
    if (error == bt_overflow) {
        m_Root.SplitRoot();
        m_Height++;
    }
    return true;
}

template <typename keyType, typename ObjIDType>
bool BTree<keyType, ObjIDType>::Remove(const keyType key, const int ObjID) {
    std::lock_guard<std::mutex> lock(m_mutex);
    bt_ErrorCode error = m_Root.Remove(key, ObjID);
    if (error == bt_duplicate || error == bt_nofound) return false;
    m_NumKeys--;
    if (error == bt_rootmerged) m_Height--;
    return true;
}

template <typename keyType, typename ObjIDType>
ObjIDType BTree<keyType, ObjIDType>::Search(const keyType key) {
    std::lock_guard<std::mutex> lock(m_mutex);
    ObjIDType ObjID = -1;
    m_Root.Search(key, ObjID);
    return ObjID;
}

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::Print(ostream &os) {
    m_Root.Print(os);
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

void DemoBTree();


#endif






