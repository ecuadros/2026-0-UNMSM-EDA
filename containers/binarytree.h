#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <mutex>
#include <stack>
#include <functional>
#include "../general/types.h"
#include "../util.h"

template <typename Traits>
class CBinaryTree;

template <typename T>
struct TreeTraitAscending {
    using value_type  = T;
    using CompareFunc = std::greater<T>;
};

template <typename T>
struct TreeTraitDescending {
    using value_type  = T;
    using CompareFunc = std::less<T>;
};

template <typename Traits>
class NodeBinaryTree {
    using value_type  = typename Traits::value_type;
    using Node        = NodeBinaryTree<Traits>;
protected:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pChild[2]  = {nullptr, nullptr};
public:
    NodeBinaryTree() : m_ref(-1) {}
    NodeBinaryTree(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref) { }

    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }
    friend class CBinaryTree<Traits>;
};

template <typename Traits>
class CBinaryTree {
public:
    using value_type  = typename Traits::value_type;
    using Node        = NodeBinaryTree<Traits>;
    using CompareFunc = typename Traits::CompareFunc;

private:
    Node *m_pRoot = nullptr;
    CompareFunc comp;
    std::mutex m_mtx;

public:
    CBinaryTree() {}

    // Deep Copy Constructor
    CBinaryTree(const CBinaryTree &other) {
        std::lock_guard<std::mutex> lock(const_cast<CBinaryTree&>(other).m_mtx);
        m_pRoot = CopyNodes(other.m_pRoot);
    }

    // Move Constructor
    CBinaryTree(CBinaryTree &&other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mtx);
        m_pRoot = other.m_pRoot;
        other.m_pRoot = nullptr;
    }

    virtual ~CBinaryTree() { Clear(m_pRoot); }

    // Métodos Públicos Seguros
    void InsertSafe(const value_type &val, ref_type ref) {
        std::lock_guard<std::mutex> lock(m_mtx);
        InternalInsert(m_pRoot, val, ref);
    }

    void Remove(const value_type& val) {
        std::lock_guard<std::mutex> lock(m_mtx);
        InternalRemove(m_pRoot, val);
    }

    // Recorridos Variádicos
    template <typename Func, typename... Args>
    void inorder(Func fn, Args... args) { internalinorden(m_pRoot, fn, args...); }

    template <typename Func, typename... Args>
    void preorder(Func fn, Args... args) { internalpreorder(m_pRoot, fn, args...); }

    template <typename Func, typename... Args>
    void postorder(Func fn, Args... args) { internalpostorder(m_pRoot, fn, args...); }

    template <typename Condition, typename... Args>
    value_type* FirstThat(Condition cond, Args... args) {
        return InternalFirstThat(m_pRoot, cond, args...);
    }

private:
    // Lógica Interna
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref) {
        if (!rParent) {
            rParent = new Node(val, ref);
            return;
        }
        auto path = comp(val, rParent->m_data);
        InternalInsert(rParent->m_pChild[path], val, ref);
    }

    void InternalRemove(Node*& pNode, const value_type& val) {
        if (!pNode) return;
        if (val == pNode->m_data) {
            if (!pNode->m_pChild[0] || !pNode->m_pChild[1]) {
                Node* temp = pNode->m_pChild[pNode->m_pChild[0] == nullptr];
                delete pNode;
                pNode = temp;
            } else {
                Node** successor = &(pNode->m_pChild[1]);
                while ((*successor)->m_pChild[0]) successor = &((*successor)->m_pChild[0]);
                pNode->m_data = (*successor)->m_data;
                pNode->m_ref = (*successor)->m_ref;
                InternalRemove(*successor, (*successor)->m_data);
            }
        } else {
            auto path = comp(val, pNode->m_data);
            InternalRemove(pNode->m_pChild[path], val);
        }
    }

    template <typename Func, typename... Args>
    void internalinorden(Node *pNode, Func fn, Args... args) {
        if (pNode) {
            internalinorden(pNode->m_pChild[0], fn, args...);
            fn(pNode->m_data, args...);
            internalinorden(pNode->m_pChild[1], fn, args...);
        }
    }

    template <typename Func, typename... Args>
    void internalpreorder(Node *pNode, Func fn, Args... args) {
        if (pNode) {
            fn(pNode->m_data, args...);
            internalpreorder(pNode->m_pChild[0], fn, args...);
            internalpreorder(pNode->m_pChild[1], fn, args...);
        }
    }

    template <typename Func, typename... Args>
    void internalpostorder(Node *pNode, Func fn, Args... args) {
        if (pNode) {
            internalpostorder(pNode->m_pChild[0], fn, args...);
            internalpostorder(pNode->m_pChild[1], fn, args...);
            fn(pNode->m_data, args...);
        }
    }

    template <typename Condition, typename... Args>
    value_type* InternalFirstThat(Node* pNode, Condition cond, Args... args) {
        if (!pNode) return nullptr;
        if (cond(pNode->m_data, args...)) return &pNode->m_data;
        value_type* res = InternalFirstThat(pNode->m_pChild[0], cond, args...);
        return res ? res : InternalFirstThat(pNode->m_pChild[1], cond, args...);
    }

public:
    void Clear(Node* node) {
        if (!node) return;
        Clear(node->m_pChild[0]);
        Clear(node->m_pChild[1]);
        delete node;
    }

    Node* CopyNodes(Node* otherNode) {
        if (!otherNode) return nullptr;
        Node* newNode = new Node(otherNode->m_data, otherNode->m_ref);
        newNode->m_pChild[0] = CopyNodes(otherNode->m_pChild[0]);
        newNode->m_pChild[1] = CopyNodes(otherNode->m_pChild[1]);
        return newNode;
    }

    // Operadores
    friend std::ostream& operator<<(std::ostream& os, CBinaryTree& tree) {
        tree.inorder([](value_type& val, std::ostream& _os) { _os << val << " "; }, std::ref(os));
        return os;
    }

    CBinaryTree& operator>>(const value_type& val) {
        InsertSafe(val, -1);
        return *this;
    }

    // Iteradores
    class Iterator {
        std::stack<Node*> s;
        void pushLeft(Node* n) { while(n) { s.push(n); n = n->m_pChild[0]; } }
    public:
        Iterator(Node* root) { pushLeft(root); }
        value_type& operator*() { return s.top()->m_data; }
        bool operator!=(const Iterator& o) const { return !(*this == o); }
        bool operator==(const Iterator& o) const { 
            return (s.empty() && o.s.empty()) || (!s.empty() && !o.s.empty() && s.top() == o.s.top()); 
        }
        Iterator& operator++() {
            Node* curr = s.top(); s.pop();
            pushLeft(curr->m_pChild[1]);
            return *this;
        }
    };

    Iterator begin() { return Iterator(m_pRoot); }
    Iterator end()   { return Iterator(nullptr); }

    class BackwardIterator {
        std::stack<Node*> s;
        void pushRight(Node* n) { while(n) { s.push(n); n = n->m_pChild[1]; } }
    public:
        BackwardIterator(Node* root) { pushRight(root); }
        value_type& operator*() { return s.top()->m_data; }
        bool operator!=(const BackwardIterator& o) const { return !(*this == o); }
        bool operator==(const BackwardIterator& o) const { 
            return (s.empty() && o.s.empty()) || (!s.empty() && !o.s.empty() && s.top() == o.s.top()); 
        }
        BackwardIterator& operator++() {
            Node* curr = s.top(); s.pop();
            pushRight(curr->m_pChild[0]);
            return *this;
        }
    };

    BackwardIterator rbegin() { return BackwardIterator(m_pRoot); }
    BackwardIterator rend()   { return BackwardIterator(nullptr); }
};

#endif