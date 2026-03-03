// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <mutex>
#include <vector>
#include <memory>
#include "BTreePage.h"

#define DEFAULT_BTREE_ORDER 3

// Traits
template <typename T>
struct BTreeTraitAscending {
    using value_type  = T;
    using CompareFunc = std::greater<T>; 
};

template <typename T>
struct BTreeTraitDescending {
    using value_type  = T;
    using CompareFunc = std::less<T>;
};


template <typename Traits>
class BTree
{
    using keyType   = typename Traits::value_type;
    using CompareFunc=typename Traits::CompareFunc;
    using ObjIDType = long;
    typedef CBTreePage<Traits> BTNode;
    typedef typename BTNode::ObjectInfo    ObjectInfo;

public:
       BTree(int order = DEFAULT_BTREE_ORDER, bool unique = true);
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
       //void            ForEach( lpfnForEach2 lpfn, void *pExtra1 );
       //void            ForEach( lpfnForEach3 lpfn, void *pExtra1, void *pExtra2);
       //ObjectInfo*     FirstThat( lpfnFirstThat2 lpfn, void *pExtra1 );
       //ObjectInfo*     FirstThat( lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2);
       //typedef         ObjectInfo iterator;

protected:
       BTNode          m_Root;
       long            m_NumKeys; // number of keys
       bool            m_Unique;  // Accept the elements only once ?
       int             m_Order;   // order of tree
       int             m_Height;  // height of tree
       mutable std::mutex m_mutex;
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

template <typename Traits>
BTree<Traits>::~BTree()
{
}

template <typename Traits>
bool BTree<Traits>::Insert(const keyType key, const int ObjID)
{      std::lock_guard<std::mutex> lock(m_mutex);
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
{      std::lock_guard<std::mutex> lock(m_mutex);
       bt_ErrorCode error = m_Root.Remove(key, ObjID);
       if( error == bt_duplicate || error == bt_nofound )
               return false;
       m_NumKeys--;

       if( error == bt_rootmerged )
               m_Height--;
       return true;
}

template <typename Traits>
ObjIDType BTree<Traits>::Search (const keyType key)
{      std::lock_guard<std::mutex> lock(m_mutex);
       ObjIDType ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}


template <typename Traits>
template <typename Func, typename... Args>
void BTree<Traits>::ForEach(Func fn, Args... args) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_Root.ForEach(fn, 0, args...);
}


template <typename Traits>
template <typename Func, typename... Args>
typename BTree<Traits>::ObjectInfo*
BTree<Traits>::FirstThat(Func fn, Args... args) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_Root.FirstThat(fn, 0, args...);
}

template <typename Traits>
void BTree<Traits>::Print(ostream &os){
       m_Root.Print(os);
}

void DemoBTree();

#endif
