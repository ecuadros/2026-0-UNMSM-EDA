#ifndef __AVL_H__
#define __AVL_H__

#include <iostream>
#include <algorithm>
#include <mutex>
#include <stack>
#include <functional>
#include "../general/types.h"

template <typename Traits>
class CBinaryTreeAVL {
public:
    using value_type  = typename Traits::value_type;
    using CompareFunc = typename Traits::CompareFunc;

    struct Node {
        value_type m_data;
        ref_type   m_ref;
        T1        m_height;
        Node* m_pChild[2];

        Node(value_type v, ref_type r) 
            : m_data(v), m_ref(r), m_height(1) {
            m_pChild[0] = m_pChild[1] = nullptr;
        }
    };

private:
    Node* m_pRoot = nullptr;
    CompareFunc m_comp;
    mutable std::mutex m_mtx; // mutable para permitir locks en metodos const

    // --- Auxiliares de Balance ---
    T1 GetHeight(Node* n) { return n ? n->m_height : 0; }
    T1 GetBalance(Node* n) { return n ? GetHeight(n->m_pChild[1]) - GetHeight(n->m_pChild[0]) : 0; }
    
    void UpdateHeight(Node* n) {
        if (n) n->m_height = 1 + std::max(GetHeight(n->m_pChild[0]), GetHeight(n->m_pChild[1]));
    }

    void Rotate(Node*& p, T1 side) {
        Node* q = p->m_pChild[side ^ 1];
        p->m_pChild[side ^ 1] = q->m_pChild[side];
        q->m_pChild[side] = p;
        UpdateHeight(p);
        UpdateHeight(q);
        p = q;
    }

    void Balance(Node*& p) {
        if (!p) return;
        UpdateHeight(p);
        T1 b = GetBalance(p);
        if (b < -1) {
            if (GetBalance(p->m_pChild[0]) > 0) Rotate(p->m_pChild[0], 0);
            Rotate(p, 1);
        } else if (b > 1) {
            if (GetBalance(p->m_pChild[1]) < 0) Rotate(p->m_pChild[1], 1);
            Rotate(p, 0);
        }
    }

public:
    CBinaryTreeAVL() = default;

    // --- 1. Constructor Copia (Deep Copy) ---
    CBinaryTreeAVL(const CBinaryTreeAVL& other) {
        std::lock_guard<std::mutex> lock(other.m_mtx);
        m_pRoot = CopyNodes(other.m_pRoot);
    }

    // --- 2. Move Constructor ---
    CBinaryTreeAVL(CBinaryTreeAVL&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mtx);
        m_pRoot = other.m_pRoot;
        other.m_pRoot = nullptr;
    }

    // --- 3. Destructor Seguro ---
    ~CBinaryTreeAVL() { Clear(m_pRoot); }

    // --- 4. Insert Safe ---
    void Insert(const value_type& val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(m_mtx);
        InternalInsert(m_pRoot, val, ref);
    }

    // --- 5. Remove ---
    void Remove(const value_type& val) {
        std::lock_guard<std::mutex> lock(m_mtx);
        InternalRemove(m_pRoot, val);
    }

    // --- 6 y 7. Iteradores (Forward y Backward) ---
    class Iterator {
        std::stack<Node*> s;
        T1 side; // 0 para Forward, 1 para Backward
        void push(Node* n) { while(n) { s.push(n); n = n->m_pChild[side]; } }
    public:
        Iterator(Node* root, T1 _side) : side(_side) { push(root); }
        value_type& operator*() { return s.top()->m_data; }
        bool operator!=(const Iterator& o) const { return s.empty() != o.s.empty() || (!s.empty() && s.top() != o.s.top()); }
        Iterator& operator++() {
            Node* curr = s.top(); s.pop();
            push(curr->m_pChild[side ^ 1]);
            return *this;
        }
    };

    Iterator begin()  { return Iterator(m_pRoot, 0); }
    Iterator end()    { return Iterator(nullptr, 0); }
    Iterator rbegin() { return Iterator(m_pRoot, 1); }
    Iterator rend()   { return Iterator(nullptr, 1); }

    // --- 8, 9, 10. Recorridos Variádicos ---
    template <typename Func, typename... Args>
    void inorder(Func fn, Args... args) { internalTraverse(m_pRoot, 0, fn, args...); }

    template <typename Func, typename... Args>
    void preorder(Func fn, Args... args) { internalTraverse(m_pRoot, 1, fn, args...); }

    template <typename Func, typename... Args>
    void postorder(Func fn, Args... args) { internalTraverse(m_pRoot, 2, fn, args...); }

    // --- 11. FirstThat (Variádico) ---
    template <typename Condition, typename... Args>
    value_type* FirstThat(Condition cond, Args... args) {
        std::lock_guard<std::mutex> lock(m_mtx);
        return InternalFirstThat(m_pRoot, cond, args...);
    }

    // --- 12 y 13. Operadores ---
    friend std::ostream& operator<<(std::ostream& os, CBinaryTreeAVL& tree) {
        tree.inorder([&os](value_type& val) { os << val << " "; });
        return os;
    }

    CBinaryTreeAVL& operator>>(const value_type& val) {
        Insert(val);
        return *this;
    }

private:
    // Lógica interna (sin locks para evitar deadlocks recursivos)
    void InternalInsert(Node*& r, const value_type& v, ref_type ref) {
        if (!r) { r = new Node(v, ref); return; }
        InternalInsert(r->m_pChild[m_comp(v, r->m_data)], v, ref);
        Balance(r);
    }

    void InternalRemove(Node*& p, const value_type& v) {
        if (!p) return;
        if (v == p->m_data) {
            if (!p->m_pChild[0] || !p->m_pChild[1]) {
                Node* t = p->m_pChild[p->m_pChild[0] == nullptr];
                delete p; p = t;
            } else {
                Node** s = &(p->m_pChild[1]);
                while ((*s)->m_pChild[0]) s = &((*s)->m_pChild[0]);
                p->m_data = (*s)->m_data;
                InternalRemove(p->m_pChild[1], (*s)->m_data);
            }
        } else {
            InternalRemove(p->m_pChild[m_comp(v, p->m_data)], v);
        }
        Balance(p);
    }

    template <typename Func, typename... Args>
    void internalTraverse(Node* n, T1 type, Func fn, Args... args) {
        if (!n) return;
        if (type == 1) fn(n->m_data, args...); // Pre
        internalTraverse(n->m_pChild[0], type, fn, args...);
        if (type == 0) fn(n->m_data, args...); // In
        internalTraverse(n->m_pChild[1], type, fn, args...);
        if (type == 2) fn(n->m_data, args...); // Post
    }

    template <typename Condition, typename... Args>
    value_type* InternalFirstThat(Node* n, Condition cond, Args... args) {
        if (!n) return nullptr;
        if (cond(n->m_data, args...)) return &n->m_data;
        value_type* res = InternalFirstThat(n->m_pChild[0], cond, args...);
        return res ? res : InternalFirstThat(n->m_pChild[1], cond, args...);
    }

    Node* CopyNodes(Node* n) {
        if (!n) return nullptr;
        Node* newNode = new Node(n->m_data, n->m_ref);
        newNode->m_height = n->m_height;
        newNode->m_pChild[0] = CopyNodes(n->m_pChild[0]);
        newNode->m_pChild[1] = CopyNodes(n->m_pChild[1]);
        return newNode;
    }

    void Clear(Node* n) {
        if (!n) return;
        Clear(n->m_pChild[0]);
        Clear(n->m_pChild[1]);
        delete n;
    }
};

#endif