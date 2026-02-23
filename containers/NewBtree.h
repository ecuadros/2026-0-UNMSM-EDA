#ifndef BNEWTREE_H
#define BNEWTREE_H

#include <iostream>
#include <mutex>
#include <vector>
#include "NewBTreePage.h"
#include "general/types.h"

using namespace std;

const Size DEFAULT_BTREE_ORDER = 3;

template <typename keyType, typename ObjIDType = long>
class BNEWTree 
{
       typedef CBNEWTreePage <keyType, ObjIDType> BTNode;

public:
       typedef typename BTNode::ObjectInfo ObjectInfo;

       template <typename... Args>
       using lpfnForEach = void (*)(ObjectInfo &info, Size level, Args... args);

       template <typename... Args>
       using lpfnFirstThat = ObjectInfo *(*)(ObjectInfo &info, Size level, Args... args);

       // --- Forward Iterator y Backward Iterator (Requeridos) ---
       class ForwardIterator {
       private:
           struct Frame { BTNode* node; int index; }; 
           vector<Frame> s;
           ObjectInfo* current;

           void pushLeft(BTNode* n) {
               while(n && n->GetNumberOfKeys() > 0) { 
                   s.push_back({n, 0}); 
                   n = n->m_SubPages[0]; 
               }
           }
       public:
           ForwardIterator(BTNode* root) : current(nullptr) { 
               if(root && root->GetNumberOfKeys() > 0) { 
                   pushLeft(root); advance(); 
               } 
           }
           ForwardIterator() : current(nullptr) {}
           
           void advance() {
               if (s.empty()) { current = nullptr; return; }
               auto top = s.back(); 
               s.pop_back();
               current = &top.node->m_Keys[top.index];
               
               top.index++;
               if (top.index < static_cast<int>(top.node->GetNumberOfKeys())) {
                   s.push_back(top);
               }
               if (top.index <= static_cast<int>(top.node->GetNumberOfKeys()) && top.node->m_SubPages[top.index]) {
                   pushLeft(top.node->m_SubPages[top.index]);
               }
           }
           
           ForwardIterator& operator++() { advance(); return *this; }
           ObjectInfo& operator*() { return *current; }
           ObjectInfo* operator->() { return current; }
           bool operator!=(const ForwardIterator& other) const { return current != other.current; }
           bool operator==(const ForwardIterator& other) const { return current == other.current; }
       };

       class BackwardIterator {
       private:
           struct Frame { BTNode* node; int index; }; 
           vector<Frame> s;
           ObjectInfo* current;

           void pushRight(BTNode* n) {
               while(n && n->GetNumberOfKeys() > 0) { 
                   s.push_back({n, static_cast<int>(n->GetNumberOfKeys() - 1)}); 
                   n = n->m_SubPages[n->GetNumberOfKeys()]; 
               }
           }
       public:
           BackwardIterator(BTNode* root) : current(nullptr) { 
               if(root && root->GetNumberOfKeys() > 0) { 
                   pushRight(root); advance(); 
               } 
           }
           BackwardIterator() : current(nullptr) {}
           
           void advance() {
               if (s.empty()) { current = nullptr; return; }
               auto top = s.back(); 
               s.pop_back();
               current = &top.node->m_Keys[top.index];
               
               top.index--;
               if (top.index >= 0) {
                   s.push_back(top);
               }
               if (top.index + 1 >= 0 && top.node->m_SubPages[top.index + 1]) {
                   pushRight(top.node->m_SubPages[top.index + 1]);
               }
           }
           
           BackwardIterator& operator++() { advance(); return *this; }
           ObjectInfo& operator*() { return *current; }
           ObjectInfo* operator->() { return current; }
           bool operator!=(const BackwardIterator& other) const { return current != other.current; }
           bool operator==(const BackwardIterator& other) const { return current == other.current; }
       };

public:
       BNEWTree(Size order = DEFAULT_BTREE_ORDER, bool unique = true);
       
       // --- Copy Constructor y Move Constructor (Requeridos) ---
       BNEWTree(const BNEWTree& other);
       BNEWTree& operator=(const BNEWTree& other);
       BNEWTree(BNEWTree&& other) noexcept;
       BNEWTree& operator=(BNEWTree&& other) noexcept;

       // --- Destructor Virtual (Requerido) ---
       virtual ~BNEWTree(); 

       // --- Métodos de Forward Iterator y Backward Iterator (Requeridos) ---
       ForwardIterator begin() { return ForwardIterator(&m_Root); }
       ForwardIterator end()   { return ForwardIterator(); }
       BackwardIterator rbegin() { return BackwardIterator(&m_Root); }
       BackwardIterator rend()   { return BackwardIterator(); }

       bool            Insert (const keyType key, const ObjIDType ObjID);
       bool            Remove (const keyType key, const ObjIDType ObjID);
       ObjIDType       Search (const keyType key);
       Size            size()  const { std::lock_guard<std::mutex> lock(Block); return m_NumKeys; }
       Size            height() const { std::lock_guard<std::mutex> lock(Block); return m_Height; }
       Size            GetOrder() const { std::lock_guard<std::mutex> lock(Block); return m_Order; }

       // --- Recorridos Variadic (Requeridos) ---
       template <typename... Args>
       void            InOrder( lpfnForEach<Args...> lpfn, Args... args );

       template <typename... Args>
       void            PreOrder( lpfnForEach<Args...> lpfn, Args... args );

       template <typename... Args>
       void            PostOrder( lpfnForEach<Args...> lpfn, Args... args );

       template <typename... Args>
       void            ForEach( lpfnForEach<Args...> lpfn, Args... args ) { InOrder(lpfn, args...); }

       // --- FirstThat Variadic (Requerido) ---
       template <typename... Args>
       ObjectInfo* FirstThat( lpfnFirstThat<Args...> lpfn, Args... args );

       // --- Operador << (Requerido) ---
       friend ostream& operator<<(ostream& os, BTree& tree) {
           std::lock_guard<std::mutex> lock(tree.Block);
           auto printNode = [](ObjectInfo &info, Size level, ostream& out) {
               for(Size i = 0; i < level; i++) out << "\t";
               out << info.key << "->" << info.ObjID << "\n";
           };
           tree.m_Root.template InOrder<ostream&>(printNode, 0, os);
           return os;
       }

       // --- Operador >> (Requerido) ---
       friend istream& operator>>(istream& is, BTree& tree) {
           keyType key;
           if (is >> key) {
               static long long auto_id = 1; 
               tree.Insert(key, static_cast<ObjIDType>(auto_id++));
           }
           return is;
       }

protected:
       // --- Concurrencia Mutex (Requerida) ---
       mutable std::mutex Block; 

       BTNode          m_Root;
       Size            m_NumKeys;
       bool            m_Unique;  
       Size            m_Order;   
       Size            m_Height;  
};

const Size MaxHeight = 5;

template <typename keyType, typename ObjIDType>
BNEWTree<keyType, ObjIDType>::BNEWTree(Size order, bool unique)
                               : m_Root(2 * order  + 1, unique),
                                 m_NumKeys(0),
                                 m_Unique(unique),
                                 m_Order(order)
{
       m_Root.SetMaxKeysForChilds(order);
       m_Height = 1;
}

// --- Implementación: Copy Constructor y Move Constructor (Requeridos) ---

template <typename keyType, typename ObjIDType>
BNEWTree<keyType, ObjIDType>::BNEWTree(const BNEWTree& other) 
    : m_Root(other.m_Order * 2 + 1, other.m_Unique), 
      m_NumKeys(0), m_Unique(other.m_Unique), m_Order(other.m_Order), m_Height(1) 
{
    m_Root.SetMaxKeysForChilds(other.m_Order);
    std::lock_guard<std::mutex> lock(other.Block);
    m_Root = other.m_Root; 
    m_NumKeys = other.m_NumKeys;
    m_Height = other.m_Height;
}

template <typename keyType, typename ObjIDType>
BNEWTree<keyType, ObjIDType>& BNEWTree<keyType, ObjIDType>::operator=(const BNEWTree& other) 
{
    if (this != &other) {
        std::lock_guard<std::mutex> lockThis(Block);
        std::lock_guard<std::mutex> lockOther(other.Block);
        m_Root = other.m_Root;
        m_NumKeys = other.m_NumKeys;
        m_Unique = other.m_Unique;
        m_Order = other.m_Order;
        m_Height = other.m_Height;
    }
    return *this;
}

template <typename keyType, typename ObjIDType>
BNEWTree<keyType, ObjIDType>::BNEWTree(BNEWTree&& other) noexcept
    : m_Root(other.m_Order * 2 + 1, other.m_Unique), 
      m_NumKeys(0), m_Unique(other.m_Unique), m_Order(other.m_Order), m_Height(1) 
{
    m_Root.SetMaxKeysForChilds(other.m_Order);
    std::lock_guard<std::mutex> lock(other.Block);
    m_Root = std::move(other.m_Root);
    m_NumKeys = other.m_NumKeys;
    m_Height = other.m_Height;
    
    other.m_NumKeys = 0;
    other.m_Height = 0;
}

template <typename keyType, typename ObjIDType>
BNEWTree<keyType, ObjIDType>& BNEWTree<keyType, ObjIDType>::operator=(BNEWTree&& other) noexcept 
{
    if (this != &other) {
        std::lock_guard<std::mutex> lockThis(Block);
        std::lock_guard<std::mutex> lockOther(other.Block);
        m_Root = std::move(other.m_Root);
        m_NumKeys = other.m_NumKeys;
        m_Unique = other.m_Unique;
        m_Order = other.m_Order;
        m_Height = other.m_Height;
        
        other.m_NumKeys = 0;
        other.m_Height = 0;
    }
    return *this;
}

template <typename keyType, typename ObjIDType>
BNEWTree<keyType, ObjIDType>::~BNEWTree()
{
}

template <typename keyType, typename ObjIDType>
bool BNEWTree<keyType, ObjIDType>::Insert(const keyType key, const ObjIDType ObjID)
{
       std::lock_guard<std::mutex> lock(Block);
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
bool BNEWTree<keyType, ObjIDType>::Remove (const keyType key, const ObjIDType ObjID)
{
       std::lock_guard<std::mutex> lock(Block);
       bt_ErrorCode error = m_Root.Remove(key, ObjID);
       if( error == bt_duplicate || error == bt_nofound )
               return false;
       m_NumKeys--;

       if( error == bt_rootmerged )
               m_Height--;
       return true;
}

template <typename keyType, typename ObjIDType>
ObjIDType BNEWTree<keyType, ObjIDType>::Search (const keyType key)
{
       std::lock_guard<std::mutex> lock(Block);
       ObjIDType ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}

// --- Implementación: Recorridos Variadic (Requeridos) ---

template <typename keyType, typename ObjIDType>
template <typename... Args>
void BNEWTree<keyType, ObjIDType>::InOrder(lpfnForEach<Args...> lpfn, Args... args)
{
       std::lock_guard<std::mutex> lock(Block);
       m_Root.template InOrder<Args...>(lpfn, 0, args...);
}

template <typename keyType, typename ObjIDType>
template <typename... Args>
void BNEWTree<keyType, ObjIDType>::PreOrder(lpfnForEach<Args...> lpfn, Args... args)
{
       std::lock_guard<std::mutex> lock(Block);
       m_Root.template PreOrder<Args...>(lpfn, 0, args...);
}

template <typename keyType, typename ObjIDType>
template <typename... Args>
void BNEWTree<keyType, ObjIDType>::PostOrder(lpfnForEach<Args...> lpfn, Args... args)
{
       std::lock_guard<std::mutex> lock(Block);
       m_Root.template PostOrder<Args...>(lpfn, 0, args...);
}

// --- Implementación: FirstThat Variadic (Requerido) ---

template <typename keyType, typename ObjIDType>
template <typename... Args>
typename BNEWTree<keyType, ObjIDType>::ObjectInfo *
BNEWTree<keyType, ObjIDType>::FirstThat(lpfnFirstThat<Args...> lpfn, Args... args)
{
       std::lock_guard<std::mutex> lock(Block);
       return m_Root.template FirstThat<Args...>(lpfn, 0, args...);
}

void DemoBTree();

#endif