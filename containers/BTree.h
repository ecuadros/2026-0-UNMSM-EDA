#ifndef __BTREE_H__
#define __BTREE_H__

#include <iostream>
#include <mutex>
#include <utility>
#include <vector>
#include "BTreePage.h"
#include "../containers/GeneralIterator.h"
#include "../general/types.h"

using namespace std;

#define DEFAULT_BTREE_ORDER 3

template <typename Traits>
class BTree {
    using BTNode     = CBTreePage<Traits>;
public:
    using value_type = typename Traits::value_type;
    using ObjectInfo = typename BTNode::ObjectInfo;
    using Node       = typename BTNode::ObjectInfo;

protected:
    BTNode  m_Root;
    Size    m_NumKeys;
    bool    m_Unique;
    Size    m_Order;
    Size    m_Height;

    mutable mutex m_mtx;
    using LOCK = lock_guard<mutex>;

public:
    //forward iterator
    class forward_iterator : public GeneralIterator< BTree<Traits> > {
        using BTPage = CBTreePage<Traits>;
        using base   = GeneralIterator< BTree<Traits> >;
              
        vector< pair<BTPage*, Size> > m_path;

        void push_leftmost(BTPage* pPage) {
            while (pPage && pPage->GetNumberOfKeys() > 0) {
                m_path.push_back({pPage, 0});
                pPage = pPage->m_SubPages[0];
            }
            update_base();
        }

        void update_base() {
            if (m_path.empty()) {
                this->m_pNode = nullptr; //conectado a GeneralIterator
            } else {
                //obtenemos el puntero exacto al ObjectInfo usando el índice
                this->m_pNode = &m_path.back().first->m_Keys[m_path.back().second];
            }
        }

    public:
        //llamamos al constructor base pasando el contenedor y nullptr al inicio
        forward_iterator(BTree<Traits>* pTree, bool is_end = false) : base(pTree, nullptr) {
            if (!is_end && pTree->m_NumKeys > 0) push_leftmost(&pTree->m_Root);
        }

        forward_iterator& operator++() {
            if (m_path.empty()) return *this;

            auto& top = m_path.back();
            BTPage* pPage = top.first;
            Size next_child = top.second + 1;
                   
            top.second++;

            if (pPage->m_SubPages[next_child]) {
                push_leftmost(pPage->m_SubPages[next_child]);
            } else {
                while (!m_path.empty()) {
                    if (m_path.back().second < m_path.back().first->GetNumberOfKeys()) break;
                    m_path.pop_back();
                }
                update_base();
            }
            return *this;
        }
        //eliminamos operator!= porque GeneralIterator ya lo tiene programado perfecto
    };

    //backward iterator
    class backward_iterator : public GeneralIterator< BTree<Traits> > {
        using BTPage = CBTreePage<Traits>;
        using base   = GeneralIterator< BTree<Traits> >;
        
        vector< pair<BTPage*, Size> > m_path;

        void push_rightmost(BTPage* pPage) {
            while (pPage && pPage->GetNumberOfKeys() > 0) {
                m_path.push_back({pPage, pPage->GetNumberOfKeys() - 1});
                pPage = pPage->m_SubPages[pPage->GetNumberOfKeys()];
            }
            update_base();
        }

        void update_base() {
            if (m_path.empty()) {
                this->m_pNode = nullptr; //conectado a GeneralIterator
            } else {
                this->m_pNode = &m_path.back().first->m_Keys[m_path.back().second];
            }
        }

    public:
        backward_iterator(BTree<Traits>* pTree, bool is_end = false) : base(pTree, nullptr) {
            if (!is_end && pTree->m_NumKeys > 0) push_rightmost(&pTree->m_Root);
        }

        backward_iterator& operator++() {
            if (m_path.empty()) return *this;

            auto& top = m_path.back();
            BTPage* pPage = top.first;
            Size left_child = top.second;
            
            top.second--;

            if (pPage->m_SubPages[left_child]) {
                push_rightmost(pPage->m_SubPages[left_child]);
            } else {
                while (!m_path.empty() && m_path.back().second < 0) {
                    m_path.pop_back();
                }
                update_base();
            }
            return *this;
        }
    };

    BTree(Size order = DEFAULT_BTREE_ORDER, bool unique = true);

    //constructor de copia 
    BTree(const BTree& another);

    //move constructor
    BTree(BTree&& another) noexcept;

    //destructor
    virtual ~BTree();

    //iteradores
    forward_iterator  begin() {
        LOCK lock(m_mtx);
        return forward_iterator(this, false); 
    }

    forward_iterator  end() {
        LOCK lock(m_mtx);
        return forward_iterator(this, true);
    }

    backward_iterator rbegin() {
        LOCK lock(m_mtx);
        return backward_iterator(this, false);
    }

    backward_iterator rend() {
        LOCK lock(m_mtx);
        return backward_iterator(this, true);
    }

    bool        Insert (const value_type key, const ref_type ObjID);
    bool        Remove (const value_type key, const ref_type ObjID);
    ref_type    Search (const value_type key);

    Size size() {
        LOCK lock(m_mtx);
        return m_NumKeys; 
    }

    Size height() {
        LOCK lock(m_mtx);
        return m_Height;
    }

    Size GetOrder() {
        LOCK lock(m_mtx);
        return m_Order;
    }

    void Clear();
    void Print (ostream &os);

    //recorridos in-order, pre-order, post-order (variadic)
    template <typename Func, typename... Args>
    void InOrder(Func fn, Args&&... args) {
        m_Root.InOrder(fn, 0, forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void PreOrder(Func fn, Args&&... args) {
        m_Root.PreOrder(fn, 0, forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void PostOrder(Func fn, Args&&... args) {
        m_Root.PostOrder(fn, 0, forward<Args>(args)...);
    }

    //foreach
    template <typename Func, typename... Args>
    void ForEach(Size level, Func fn, Args&&... args) {
        //asumiendo que esta lógica corresponde a delegarla a m_Root como en los recorridos.
        //se corrige la referencia directa a m_KeyCount para usar m_Root
        for (Size i = 0; i < m_Root.GetNumberOfKeys(); i++) {
            if (m_Root.m_SubPages[i]) m_Root.m_SubPages[i]->ForEach(level + 1, fn, std::forward<Args>(args)...);
            fn(m_Root.m_Keys[i], level, std::forward<Args>(args)...);
        }
        if (m_Root.m_SubPages[m_Root.GetNumberOfKeys()]) 
            m_Root.m_SubPages[m_Root.GetNumberOfKeys()]->ForEach(level + 1, fn, std::forward<Args>(args)...);
    }

    //first that
    template <typename Func, typename... Args>
    ObjectInfo* FirstThat(Func fn, Args&&... args) {
        return m_Root.FirstThat(fn, 0, forward<Args>(args)...);
    }

    //persistencia write
    friend ostream& operator<<(ostream &os, BTree<Traits> &tree) {
        os << "[ ";
        auto printFlat = [](ObjectInfo& info, Size level, ostream& out) {
            out << "(" << info.key << ":" << info.ObjID << ") ";
        };
        tree.InOrder(printFlat, os);
        os << "]";
        return os;
    }

    //persistencia read
    friend istream& operator>>(istream &is, BTree<Traits> &tree) {
        tree.Clear();

        char c;
        typename Traits::value_type val;
        ref_type ref;

        while (is >> c && c != '[') {}
        while (is >> c && c != ']') {
            if (c == '(') {
                is >> val;
                is >> c;
                if(c == ':') { 
                    is >> ref;
                    tree.Insert(val, ref);
                }
                while(is >> c && c != ',' && c != ']' && c != ')');
                if (c == ']') break;
            }
        }
        return is;
    }

private:
    //helpers privados para el control de mutex en constructores
    BTree(const BTree& another, const lock_guard<mutex>& lock);
    BTree(BTree&& another, const lock_guard<mutex>& lock) noexcept;
};

const Size MaxHeight = 5;

template <typename Traits>
BTree<Traits>::BTree(Size order, bool unique) 
    : m_Root(2 * order + 1, unique), m_NumKeys(0), m_Unique(unique), m_Order(order) 
{
    m_Root.SetMaxKeysForChilds(order);
    m_Height = 1;
}

template <typename Traits>
BTree<Traits>::BTree(const BTree<Traits>& another) 
    : BTree(another, lock_guard<mutex>(another.m_mtx)) {}

template <typename Traits>
BTree<Traits>::BTree(const BTree<Traits>& another, const lock_guard<mutex>& lock) 
    : m_Root(another.m_Root),
      m_NumKeys(another.m_NumKeys),
      m_Unique(another.m_Unique),
      m_Order(another.m_Order),
      m_Height(another.m_Height){}

template <typename Traits>
BTree<Traits>::BTree(BTree<Traits>&& another) noexcept 
    : BTree(move(another), lock_guard<mutex>(another.m_mtx)) {}

template <typename Traits>
BTree<Traits>::BTree(BTree<Traits>&& another, const lock_guard<mutex>& lock) noexcept 
    : m_Root(move(another.m_Root)),
      m_NumKeys(exchange(another.m_NumKeys, 0)),
      m_Unique(another.m_Unique),
      m_Order(another.m_Order),
      m_Height(exchange(another.m_Height, 0)){}

template <typename Traits>
BTree<Traits>::~BTree(){
    lock_guard<mutex> lock(m_mtx);
}

template <typename Traits>
bool BTree<Traits>::Insert(const value_type key, const ref_type ObjID) {
    lock_guard<mutex> lock(m_mtx); 
    bt_ErrorCode error = m_Root.Insert(key, ObjID);
    if( error == bt_duplicate ) return false;
    
    m_NumKeys++;
    if( error == bt_overflow ){
        m_Root.SplitRoot();
        m_Height++;
    }
    return true;
}

template <typename Traits>
bool BTree<Traits>::Remove (const value_type key, const ref_type ObjID)
{
    lock_guard<mutex> lock(m_mtx);
    bt_ErrorCode error = m_Root.Remove(key, ObjID);
    if( error == bt_duplicate || error == bt_nofound ) return false;
    
    m_NumKeys--;
    if( error == bt_rootmerged ) m_Height--;
    
    return true;
}

template <typename Traits>
ref_type BTree<Traits>::Search(const typename Traits::value_type key)
{
    lock_guard<mutex> lock(m_mtx);
    ref_type ObjID = -1;
    m_Root.Search(key, ObjID);
    return ObjID;
}

template <typename Traits>
void BTree<Traits>::Clear() {
    lock_guard<mutex> lock(m_mtx);
    m_Root.Reset();
    m_NumKeys = 0;
    m_Height = 1;
}

template <typename Traits>
void BTree<Traits>::Print(ostream &os) {
    lock_guard<mutex> lock(m_mtx);
    m_Root.Print(os);
}

void DemoBTree();

#endif // __BTREE_H__