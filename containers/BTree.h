#ifndef BTREE_H
#define BTREE_H
#include <iostream>
#include <vector>
#include <mutex>
#include <stdexcept>
#include <type_traits>
#include "BTraits.h"   
#include "BTreePage.h"    
#include "../general/types.h"

#define DEFAULT_BTREE_ORDER 3

template <typename keyType,
          typename ObjIDType = long,
          typename Traits    = BTreeTraits<keyType>>
class BTree
{

    static_assert(has_btree_traits<keyType>::value,
                  "No hay BTreeTraits especializado para este tipo de clave. "
                  "Agrega una especialización en BTreeTraits.h");

    typedef CBTreePage<keyType, ObjIDType> BTNode;

public:
    typedef typename BTNode::lpfnForEach2   lpfnForEach2;
    typedef typename BTNode::lpfnForEach3   lpfnForEach3;
    typedef typename BTNode::lpfnFirstThat2 lpfnFirstThat2;
    typedef typename BTNode::lpfnFirstThat3 lpfnFirstThat3;
    typedef typename BTNode::ObjectInfo     ObjectInfo;
    typedef Traits                          traits_type;

    explicit BTree( T1 order = DEFAULT_BTREE_ORDER, bool unique = true)
        : m_Root(2 * order + 1, unique),
          m_NumKeys(0),
          m_Unique(unique),
          m_Order(order),
          m_Height(1)
    {
        m_Root.SetMaxKeysForChilds(order);
    }

    BTree(BTree&& other) noexcept
        : m_Root(std::move(other.m_Root)),
          m_NumKeys(other.m_NumKeys),
          m_Unique(other.m_Unique),
          m_Order(other.m_Order),
          m_Height(other.m_Height)
    {
        other.m_NumKeys = 0;
        other.m_Height  = 1;
    }

    BTree& operator=(BTree&& other) noexcept
    {
        if (this != &other) {
            std::lock_guard<std::mutex> lk(m_Mutex);
            m_Root    = std::move(other.m_Root);
            m_NumKeys = other.m_NumKeys;
            m_Unique  = other.m_Unique;
            m_Order   = other.m_Order;
            m_Height  = other.m_Height;
            other.m_NumKeys = 0;
            other.m_Height  = 1;
        }
        return *this;
    }

    BTree(const BTree&)            = delete;
    BTree& operator=(const BTree&) = delete;

    ~BTree() = default;

    bool Insert(const keyType& key, const ObjIDType ObjID)
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        bt_ErrorCode error = m_Root.Insert(key, ObjID);
        if (error == bt_duplicate) return false;
        m_NumKeys++;
        if (error == bt_overflow) {
            m_Root.SplitRoot();
            m_Height++;
        }
        return true;
    }

    bool Remove(const keyType& key, const ObjIDType ObjID)
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        bt_ErrorCode error = m_Root.Remove(key, ObjID);
        if (error == bt_duplicate || error == bt_nofound) return false;
        m_NumKeys--;
        if (error == bt_rootmerged) m_Height--;
        return true;
    }

    ObjIDType Search(const keyType& key)
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        ObjIDType ObjID = static_cast<ObjIDType>(-1);
        m_Root.Search(key, ObjID);
        return ObjID;
    }

    long size()     const { return m_NumKeys; }
    long height()   const { return m_Height;  }
    long GetOrder() const { return m_Order;   }

    void ForEach(lpfnForEach2 lpfn, void* pExtra1)
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        m_Root.ForEach(lpfn, 0, pExtra1);
    }
    void ForEach(lpfnForEach3 lpfn, void* pExtra1, void* pExtra2)
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        m_Root.ForEach(lpfn, 0, pExtra1, pExtra2);
    }
    void ForEachPreOrder(lpfnForEach2 lpfn, void* pExtra1)
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        m_Root.ForEachPreOrder(lpfn, 0, pExtra1);
    }
    void ForEachPreOrder(lpfnForEach3 lpfn, void* pExtra1, void* pExtra2)
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        m_Root.ForEachPreOrder(lpfn, 0, pExtra1, pExtra2);
    }
    void ForEachPostOrder(lpfnForEach2 lpfn, void* pExtra1)
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        m_Root.ForEachPostOrder(lpfn, 0, pExtra1);
    }
    void ForEachPostOrder(lpfnForEach3 lpfn, void* pExtra1, void* pExtra2)
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        m_Root.ForEachPostOrder(lpfn, 0, pExtra1, pExtra2);
    }
    ObjectInfo* FirstThat(lpfnFirstThat2 lpfn, void* pExtra1)
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        return m_Root.FirstThat(lpfn, 0, pExtra1);
    }
    ObjectInfo* FirstThat(lpfnFirstThat3 lpfn, void* pExtra1, void* pExtra2)
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        return m_Root.FirstThat(lpfn, 0, pExtra1, pExtra2);
    }
    friend std::ostream& operator<<(std::ostream& os, BTree& tree)
    {
        std::lock_guard<std::mutex> lk(tree.m_Mutex);
        tree.m_Root.Print(os);
        return os;
    }
    friend std::istream& operator>>(std::istream& is, BTree& tree)
    {
        keyType   key;
        ObjIDType oid;
        while (is >> key >> oid)
            tree.Insert(key, oid);
        return is;
    }
    void Print(std::ostream& os)
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        m_Root.Print(os);
    }
    struct Snapshot {
        std::vector<ObjectInfo> data;
    };

private:
    static void collectInOrder(CBTreePage<keyType,ObjIDType>* page,
                               std::vector<ObjectInfo>& out)
    {
        if (!page) return;
        T1 n = page->GetNumberOfKeys();
        for ( T1 i = 0; i < n; i++) {
        }
 
    }

    static void collectHelper(ObjectInfo& info,  T1/*level*/, void* pExtra)
    {
        auto* vec = reinterpret_cast<std::vector<ObjectInfo>*>(pExtra);
        vec->push_back(info);
    }

    std::vector<ObjectInfo> makeSnapshot()
    {
        std::vector<ObjectInfo> snap;
        snap.reserve(static_cast<size_t>(m_NumKeys));
        lpfnForEach2 fn = &BTree::collectHelper;
        m_Root.ForEach(fn, 0, &snap);
        return snap;
    }

public:
    class iterator {
    public:
        using value_type        = ObjectInfo;
        using pointer           = ObjectInfo*;
        using reference         = ObjectInfo&;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;

        iterator() = default;
        iterator(std::vector<ObjectInfo>* snap, std::size_t idx)
            : m_snap(snap), m_idx(idx) {}

        reference operator*()  { return (*m_snap)[m_idx]; }
        pointer   operator->() { return &(*m_snap)[m_idx]; }

        iterator& operator++() { ++m_idx; return *this; }
        iterator  operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
        iterator& operator--() { --m_idx; return *this; }
        iterator  operator--(int) { iterator tmp = *this; --(*this); return tmp; }

        bool operator==(const iterator& o) const { return m_idx == o.m_idx; }
        bool operator!=(const iterator& o) const { return !(*this == o); }

    private:
        std::vector<ObjectInfo>* m_snap = nullptr;
        std::size_t              m_idx  = 0;
    };

    using reverse_iterator = std::reverse_iterator<iterator>;

    iterator begin()
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        m_Snapshot = makeSnapshot();
        return iterator(&m_Snapshot, 0);
    }
    iterator end()
    {
        return iterator(&m_Snapshot, m_Snapshot.size());
    }

    reverse_iterator rbegin() { return reverse_iterator(end());   }
    reverse_iterator rend()   { return reverse_iterator(begin()); }
    std::mutex& getMutex() { return m_Mutex; }

protected:
    BTNode     m_Root;
    long       m_NumKeys;
    bool       m_Unique;
     T1       m_Order;
     T1       m_Height;

    mutable std::mutex m_Mutex;
    std::vector<ObjectInfo> m_Snapshot;
};

void DemoBTree();

#endif