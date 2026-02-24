
#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <mutex>
#include <tuple>
#include "BTreePage.h"

#define DEFAULT_BTREE_ORDER 3

template <typename keyType, typename ObjIDType = long>
class BTree 
{
    typedef CBTreePage<keyType, ObjIDType> BTNode;

public:
    typedef typename BTNode::lpfnForEach2    lpfnForEach2;
    typedef typename BTNode::lpfnForEach3    lpfnForEach3;
    typedef typename BTNode::lpfnFirstThat2  lpfnFirstThat2;
    typedef typename BTNode::lpfnFirstThat3  lpfnFirstThat3;
    typedef typename BTNode::ObjectInfo      ObjectInfo;

    // ============================================
    // FORWARD ITERATOR (simple)
    // ============================================
    class iterator {
    private:
        ObjectInfo *m_pCurrent;
    public:
        iterator(ObjectInfo *p = nullptr) : m_pCurrent(p) {}
        ObjectInfo& operator*() { return *m_pCurrent; }
        ObjectInfo* operator->() { return m_pCurrent; }
        bool operator!=(const iterator &other) const { 
            return m_pCurrent != other.m_pCurrent; 
        }
    };

protected:
    BTNode  m_Root;
    long    m_NumKeys;
    bool    m_Unique;
    int     m_Order;
    int     m_Height;
    mutable std::mutex m_mutex;

public:
    // ============================================
    // CONSTRUCTOR
    // ============================================
    BTree(int order = DEFAULT_BTREE_ORDER, bool unique = true)
        : m_Root(2 * order + 1, unique),
          m_NumKeys(0),
          m_Unique(unique),
          m_Order(order),
          m_Height(1)
    {
        m_Root.SetMaxKeysForChilds(order);
    }
    
    // ============================================
    // CONSTRUCTOR COPIA
    // ============================================
    BTree(const BTree &other)
        : m_Root(2 * other.m_Order + 1, other.m_Unique),
          m_NumKeys(0),
          m_Unique(other.m_Unique),
          m_Order(other.m_Order),
          m_Height(1)
    {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_Root.SetMaxKeysForChilds(m_Order);
        
        // Copiar todos los elementos
        const_cast<BTree&>(other).ForEach(
            [](ObjectInfo &info, int level, void *pThis) {
                BTree *pTree = (BTree*)pThis;
                pTree->Insert(info.key, info.ObjID);
            }, 
            this
        );
    }
    
    // ============================================
    // MOVE CONSTRUCTOR
    // ============================================
    BTree(BTree &&other) noexcept
        : m_Root(2 * other.m_Order + 1, other.m_Unique),
          m_NumKeys(other.m_NumKeys),
          m_Unique(other.m_Unique),
          m_Order(other.m_Order),
          m_Height(other.m_Height)
    {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_Root = std::move(other.m_Root);
        other.m_NumKeys = 0;
        other.m_Height = 1;
    }
    
    // ============================================
    // DESTRUCTOR
    // ============================================
    virtual ~BTree() {}
    
    // ============================================
    // INSERT
    // ============================================
    bool Insert(const keyType key, const ObjIDType ObjID)
    {
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
    
    // ============================================
    // REMOVE
    // ============================================
    bool Remove(const keyType key, const ObjIDType ObjID)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        bt_ErrorCode error = m_Root.Remove(key, ObjID);
        if (error == bt_duplicate || error == bt_nofound) return false;
        m_NumKeys--;
        if (error == bt_rootmerged) m_Height--;
        return true;
    }
    
    // ============================================
    // SEARCH
    // ============================================
    ObjIDType Search(const keyType key)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        ObjIDType ObjID = -1;
        m_Root.Search(key, ObjID);
        return ObjID;
    }
    
    // ============================================
    // ITERADORES (begin, end)
    // ============================================
    iterator begin() { 
        // Para B-Tree, retornamos un iterador simple
        return iterator(nullptr); 
    }
    
    iterator end() { 
        return iterator(nullptr); 
    }
    
    // ============================================
    // BACKWARD ITERADORES (rbegin, rend)
    // ============================================
    iterator rbegin() { 
        return iterator(nullptr); 
    }
    
    iterator rend() { 
        return iterator(nullptr); 
    }
    
    // ============================================
    // INORDEN CON VARIADICO
    // ============================================
    template<typename Func, typename... Args>
    void Inorden(Func fn, Args... args)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        // B-Tree usa ForEach que ya recorre en orden
        m_Root.ForEach(
            [](ObjectInfo &info, int level, void *pExtra) {
                auto *pPair = (std::pair<Func, std::tuple<Args...>>*)pExtra;
                std::apply([&](Args... args) {
                    pPair->first(info, args...);
                }, pPair->second);
            },
            0,
            &std::make_pair(fn, std::make_tuple(args...))
        );
    }
    
    // Sobrecarga sin argumentos adicionales
    void Inorden(lpfnForEach2 lpfn, void *pExtra1)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Root.ForEach(lpfn, 0, pExtra1);
    }
    
    // ============================================
    // PREORDEN CON VARIADICO
    // ============================================
    template<typename Func, typename... Args>
    void Preorden(Func fn, Args... args)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Root.ForEach(
            [](ObjectInfo &info, int level, void *pExtra) {
                auto *pPair = (std::pair<Func, std::tuple<Args...>>*)pExtra;
                std::apply([&](Args... args) {
                    pPair->first(info, args...);
                }, pPair->second);
            },
            0,
            &std::make_pair(fn, std::make_tuple(args...))
        );
    }
    
    void Preorden(lpfnForEach2 lpfn, void *pExtra1)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Root.ForEach(lpfn, 0, pExtra1);
    }
    
    // ============================================
    // POSORDEN CON VARIADICO
    // ============================================
    template<typename Func, typename... Args>
    void Posorden(Func fn, Args... args)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Root.ForEach(
            [](ObjectInfo &info, int level, void *pExtra) {
                auto *pPair = (std::pair<Func, std::tuple<Args...>>*)pExtra;
                std::apply([&](Args... args) {
                    pPair->first(info, args...);
                }, pPair->second);
            },
            0,
            &std::make_pair(fn, std::make_tuple(args...))
        );
    }
    
    void Posorden(lpfnForEach2 lpfn, void *pExtra1)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Root.ForEach(lpfn, 0, pExtra1);
    }
    
    // ============================================
    // FOREACH (original del profesor)
    // ============================================
    void ForEach(lpfnForEach2 lpfn, void *pExtra1)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Root.ForEach(lpfn, 0, pExtra1);
    }
    
    void ForEach(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Root.ForEach(lpfn, 0, pExtra1, pExtra2);
    }
    
    // ============================================
    // FIRSTTHAT CON VARIADICO (Corregido para C++17)
    // ============================================
    template<typename Predicate, typename... Args>
    ObjectInfo* FirstThat(Predicate pred, Args... args)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        ObjectInfo *pResult = nullptr;

        // Creamos una estructura temporal para pasar los datos a C++17
        struct Context {
            Predicate predicate;
            ObjectInfo** ppResult;
            std::tuple<Args...> argsTuple;
        };

        Context ctx = {pred, &pResult, std::make_tuple(args...)};

        m_Root.ForEach(
            [](ObjectInfo &info, int level, void *pExtra) {
                Context* pCtx = (Context*)pExtra;
                
                // Si aun no encontramos el objetivo, lo evaluamos
                if (*(pCtx->ppResult) == nullptr) {
                    // Usamos std::apply para desempaquetar la tupla de argumentos
                    bool match = std::apply([&](Args... unpackedArgs) {
                        return pCtx->predicate(info, unpackedArgs...);
                    }, pCtx->argsTuple);

                    // Si el predicado es verdadero, guardamos el puntero
                    if (match) {
                        *(pCtx->ppResult) = &info;
                    }
                }
            },
            0,
            &ctx
        );
        return pResult;
    }
    
    ObjectInfo* FirstThat(lpfnFirstThat2 lpfn, void *pExtra1)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_Root.FirstThat(lpfn, 0, pExtra1);
    }
    
    ObjectInfo* FirstThat(lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_Root.FirstThat(lpfn, 0, pExtra1, pExtra2);
    }
    
    // ============================================
    // OPERATOR <
    // ============================================
    bool operator<(const BTree &other) const
    {
        return m_NumKeys < other.m_NumKeys;
    }
    
    // ============================================
    // OPERATOR >
    // ============================================
    bool operator>(const BTree &other) const
    {
        return m_NumKeys > other.m_NumKeys;
    }
    
    // ============================================
    // OPERATOR ==
    // ============================================
    bool operator==(const BTree &other) const
    {
        return m_NumKeys == other.m_NumKeys;
    }
    
    // ============================================
    // UTILIDADES
    // ============================================
    long size() const { return m_NumKeys; }
    long height() const { return m_Height; }
    long GetOrder() const { return m_Order; }
    
    void Print(std::ostream &os)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Root.Print(os);
    }
};

void DemoBTree();

#endif // BTREE_H