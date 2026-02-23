#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <mutex>
#include "BTreePage.h"

template <typename keyType, typename ObjIDType = long>
class BTree {
    typedef CBTreePage<keyType, ObjIDType> BTNode;
    typedef tagObjectInfo<keyType, ObjIDType> ObjectInfo;

public:
    BTree(int order, bool unique = true) 
        : m_pRoot(nullptr), m_Order(order), m_Unique(unique), m_NumKeys(0), m_Height(0) {}

    ~BTree() {
        if (m_pRoot) delete m_pRoot;
    }

    BTree(BTree&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mtx);
        m_pRoot = other.m_pRoot;
        m_NumKeys = other.m_NumKeys;
        m_Unique = other.m_Unique;
        m_Order = other.m_Order;
        m_Height = other.m_Height;
        other.m_pRoot = nullptr;
    }

    bool Insert(const keyType& key, const ObjIDType& ObjID) {
        std::lock_guard<std::mutex> lock(m_mtx);
        
        if (!m_pRoot) {
            m_pRoot = new BTNode(m_Order, m_Unique);
        }

        bt_ErrorCode error = m_pRoot->Insert(key, ObjID);
        
        if (error == bt_duplicate) return false;
        
        if (error == bt_overflow) {
            BTNode* pNewRoot = new BTNode(m_Order, m_Unique);
            pNewRoot->m_SubPages[0] = m_pRoot;
            pNewRoot->SplitChild(0); 
            
            m_pRoot = pNewRoot;
            m_Height++;
        }
        
        m_NumKeys++;
        return true;
    }

    bool Search(const keyType& key, ObjIDType& id) {
        std::lock_guard<std::mutex> lock(m_mtx);
        if (!m_pRoot) return false;
        return m_pRoot->Search(key, id);
    }

    template<typename Func, typename... Args>
    void InOrder(Func f, Args... args) {
        std::lock_guard<std::mutex> lock(m_mtx);
        if (m_pRoot) m_pRoot->InOrderVariadic(f, 0, args...);
    }

    friend std::ostream& operator<<(std::ostream& os, BTree& tree) {
        tree.InOrder([&os](ObjectInfo& info, int level) {
            os << info.key << " ";
        });
        return os;
    }

private:
    BTNode* m_pRoot;
    int m_Order;
    bool m_Unique;
    int m_NumKeys;
    int m_Height;
    mutable std::mutex m_mtx;
};

#endif