#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <mutex>
#include <vector>
#include <functional>
#include "BTreePage.h"

#define DEFAULT_BTREE_ORDER 3

template <typename keyType, typename ObjIDType = long>
class BTree {
    typedef CBTreePage<keyType, ObjIDType> BTNode;

public:
    typedef typename BTNode::lpfnForEach2   lpfnForEach2;
    typedef typename BTNode::lpfnForEach3   lpfnForEach3;
    typedef typename BTNode::lpfnFirstThat2 lpfnFirstThat2;
    typedef typename BTNode::lpfnFirstThat3 lpfnFirstThat3;
    typedef typename BTNode::ObjectInfo     ObjectInfo;

protected:
    BTNode     m_Root;
    long       m_NumKeys;
    bool       m_Unique;
    int        m_Order;
    int        m_Height;
    std::mutex m_mutex;

public:
    // Constructor por defecto
    BTree(int order = DEFAULT_BTREE_ORDER, bool unique = true)
        : m_Root(2 * order + 1, unique),
          m_NumKeys(0),
          m_Unique(unique),
          m_Order(order),
          m_Height(1)
    {
        m_Root.SetMaxKeysForChilds(order);
    }

    // Move constructor
    BTree(BTree&& other) noexcept
        : m_Root(2 * other.m_Order + 1, other.m_Unique),
          m_NumKeys(other.m_NumKeys),
          m_Unique(other.m_Unique),
          m_Order(other.m_Order),
          m_Height(other.m_Height)
    {
        other.m_NumKeys = 0;
        other.m_Height  = 1;
    }

    // Destructor
    ~BTree() {}

    // Insert
    bool Insert(const keyType key, const int ObjID) {
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

    // Remove
    bool Remove(const keyType key, const int ObjID) {
        std::lock_guard<std::mutex> lock(m_mutex);
        bt_ErrorCode error = m_Root.Remove(key, ObjID);
        if (error == bt_duplicate || error == bt_nofound) return false;
        m_NumKeys--;
        if (error == bt_rootmerged) m_Height--;
        return true;
    }

    // Search
    ObjIDType Search(const keyType key) {
        ObjIDType ObjID = -1;
        m_Root.Search(key, ObjID);
        return ObjID;
    }

    long size()     { return m_NumKeys; }
    long height()   { return m_Height;  }
    long GetOrder() { return m_Order;   }

    void Print(std::ostream& os) { m_Root.Print(os); }

    // ForEach original
    void ForEach(lpfnForEach2 lpfn, void* pExtra1) {
        m_Root.ForEach(lpfn, 0, pExtra1);
    }
    void ForEach(lpfnForEach3 lpfn, void* pExtra1, void* pExtra2) {
        m_Root.ForEach(lpfn, 0, pExtra1, pExtra2);
    }

    // ForEach variadic con lambda
    template <typename Func, typename... Args>
    void ForEachVariadic(Func func, Args&&... args) {
        auto cache = ToVector();
        for (auto& obj : cache)
            func(obj, std::forward<Args>(args)...);
    }

    // FirstThat original
    ObjectInfo* FirstThat(lpfnFirstThat2 lpfn, void* pExtra1) {
        return m_Root.FirstThat(lpfn, 0, pExtra1);
    }
    ObjectInfo* FirstThat(lpfnFirstThat3 lpfn, void* pExtra1, void* pExtra2) {
        return m_Root.FirstThat(lpfn, 0, pExtra1, pExtra2);
    }

    // FirstThat variadic con lambda
    template <typename Func, typename... Args>
    ObjectInfo* FirstThatVariadic(Func func, Args&&... args) {
        auto cache = ToVector();
        for (auto& obj : cache)
            if (func(obj, std::forward<Args>(args)...))
                return &obj;
        return nullptr;
    }

    // operator<< inserta una clave
    BTree& operator<<(const keyType& key) {
        Insert(key, -1);
        return *this;
    }

    // operator>> busca y retorna ObjID
    BTree& operator>>(keyType& key) {
        key = Search(key);
        return *this;
    }

    // operator<< para imprimir
    friend std::ostream& operator<<(std::ostream& os, BTree& bt) {
        bt.Print(os);
        return os;
    }

    // Convierte el arbol a vector de ObjectInfo (inorden via ForEach)
    std::vector<ObjectInfo> ToVector() {
        std::vector<ObjectInfo> result;
        m_Root.ForEach(
            [](ObjectInfo& info, int level, void* extra) {
                static_cast<std::vector<ObjectInfo>*>(extra)->push_back(info);
            },
            0,
            &result
        );
        return result;
    }

    // Iteradores basados en vector cacheado
    class iterator {
        std::vector<ObjectInfo> m_cache;
        size_t m_index;
    public:
        iterator(std::vector<ObjectInfo> cache, size_t index)
            : m_cache(std::move(cache)), m_index(index) {}

        ObjectInfo& operator*()  { return m_cache[m_index]; }
        iterator& operator++()   { ++m_index; return *this; }
        bool operator!=(const iterator& other) const {
            return m_index != other.m_index;
        }
    };

    class reverse_iterator {
        std::vector<ObjectInfo> m_cache;
        int m_index;
    public:
        reverse_iterator(std::vector<ObjectInfo> cache, int index)
            : m_cache(std::move(cache)), m_index(index) {}

        ObjectInfo& operator*()          { return m_cache[m_index]; }
        reverse_iterator& operator++()   { --m_index; return *this; }
        bool operator!=(const reverse_iterator& other) const {
            return m_index != other.m_index;
        }
    };

    iterator begin() {
        auto cache = ToVector();
        return iterator(std::move(cache), 0);
    }
    iterator end() {
    auto cache = ToVector();
    size_t sz = cache.size();
    return iterator(cache, sz);
    }

    reverse_iterator rbegin() {
        auto cache = ToVector();
        int sz = (int)cache.size();
        return reverse_iterator(std::move(cache), sz - 1);
    }
    reverse_iterator rend() {
        auto cache = ToVector();
        return reverse_iterator(std::move(cache), -1);
    }
};

void DemoBTree();

#endif