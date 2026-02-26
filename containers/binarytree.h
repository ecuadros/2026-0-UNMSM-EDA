#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <mutex>
#include <functional>
#include <stack>
#include "../general/types.h"
#include "../util.h"

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
public:
    using value_type = typename Traits::value_type;
    using Node       = NodeBinaryTree<Traits>;

    value_type m_data;
    ref_type   m_ref;
    Node      *m_pChild[2] = {nullptr, nullptr};

    NodeBinaryTree() {}
    NodeBinaryTree(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref) {}

    value_type  GetValue()    const { return m_data; }
    value_type &GetValueRef()       { return m_data; }
};

template <typename Traits>
class CBinaryTree {
public:
    using value_type  = typename Traits::value_type;
    using Node        = NodeBinaryTree<Traits>;
    using CompareFunc = typename Traits::CompareFunc;

    // Forward Iterator (inorden: izq -> raiz -> der)
    class iterator {
    private:
        std::stack<Node*> m_stack;

        void pushLeft(Node* node) {
            while (node) {
                m_stack.push(node);
                node = node->m_pChild[0];
            }
        }
    public:
        iterator() {}
        iterator(Node* root) { pushLeft(root); }

        value_type& operator*() {
            return m_stack.top()->GetValueRef();
        }

        iterator& operator++() {
            Node* node = m_stack.top();
            m_stack.pop();
            pushLeft(node->m_pChild[1]);
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return m_stack != other.m_stack;
        }

        bool operator==(const iterator& other) const {
            return m_stack == other.m_stack;
        }
    };

    // Backward Iterator (inorden inverso: der -> raiz -> izq)
    class reverse_iterator {
    private:
        std::stack<Node*> m_stack;

        void pushRight(Node* node) {
            while (node) {
                m_stack.push(node);
                node = node->m_pChild[1];
            }
        }
    public:
        reverse_iterator() {}
        reverse_iterator(Node* root) { pushRight(root); }

        value_type& operator*() {
            return m_stack.top()->GetValueRef();
        }

        reverse_iterator& operator++() {
            Node* node = m_stack.top();
            m_stack.pop();
            pushRight(node->m_pChild[0]);
            return *this;
        }

        bool operator!=(const reverse_iterator& other) const {
            return m_stack != other.m_stack;
        }
    };

private:
    Node       *m_pRoot = nullptr;
    CompareFunc comp;
    std::mutex  m_mutex;

    Node* CopyNode(Node* src) {
        if (!src) return nullptr;
        Node* newNode = new Node(src->m_data, src->m_ref);
        newNode->m_pChild[0] = CopyNode(src->m_pChild[0]);
        newNode->m_pChild[1] = CopyNode(src->m_pChild[1]);
        return newNode;
    }

    void DestroyNode(Node* node) {
        if (!node) return;
        DestroyNode(node->m_pChild[0]);
        DestroyNode(node->m_pChild[1]);
        delete node;
    }

    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref) {
        if (!rParent) {
            rParent = new Node(val, ref);
            return;
        }
        auto path = comp(val, rParent->GetValue());
        InternalInsert(rParent->m_pChild[path], val, ref);
    }

    // Inorden con funcion variadic
    template <typename Func, typename... Args>
    void InternalInorden(Node* node, Func func, Args&&... args) {
        if (!node) return;
        InternalInorden(node->m_pChild[0], func, args...);
        func(node->m_data, std::forward<Args>(args)...);
        InternalInorden(node->m_pChild[1], func, args...);
    }

    // Preorden con funcion variadic
    template <typename Func, typename... Args>
    void InternalPreorden(Node* node, Func func, Args&&... args) {
        if (!node) return;
        func(node->m_data, std::forward<Args>(args)...);
        InternalPreorden(node->m_pChild[0], func, args...);
        InternalPreorden(node->m_pChild[1], func, args...);
    }

    // Postorden con funcion variadic
    template <typename Func, typename... Args>
    void InternalPostorden(Node* node, Func func, Args&&... args) {
        if (!node) return;
        InternalPostorden(node->m_pChild[0], func, args...);
        InternalPostorden(node->m_pChild[1], func, args...);
        func(node->m_data, std::forward<Args>(args)...);
    }

    // FirstThat interno
    template <typename Func, typename... Args>
    Node* InternalFirstThat(Node* node, Func func, Args&&... args) {
        if (!node) return nullptr;
        if (func(node->m_data, std::forward<Args>(args)...)) return node;
        Node* found = InternalFirstThat(node->m_pChild[0], func, args...);
        if (found) return found;
        return InternalFirstThat(node->m_pChild[1], func, args...);
    }

public:
    CBinaryTree() {}

    CBinaryTree(const CBinaryTree &another) {
        m_pRoot = CopyNode(another.m_pRoot);
    }

    CBinaryTree(CBinaryTree &&another) noexcept {
        m_pRoot = another.m_pRoot;
        another.m_pRoot = nullptr;
    }

    virtual ~CBinaryTree() {
        DestroyNode(m_pRoot);
        m_pRoot = nullptr;
    }

    // Iteradores
    iterator begin() { return iterator(m_pRoot); }
    iterator end()   { return iterator(); }

    reverse_iterator rbegin() { return reverse_iterator(m_pRoot); }
    reverse_iterator rend()   { return reverse_iterator(); }

    void Insert(const value_type &val, ref_type ref) {
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalInsert(m_pRoot, val, ref);
    }

    // Recorridos variadic
    template <typename Func, typename... Args>
    void Inorden(Func func, Args&&... args) {
        InternalInorden(m_pRoot, func, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void Preorden(Func func, Args&&... args) {
        InternalPreorden(m_pRoot, func, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void Postorden(Func func, Args&&... args) {
        InternalPostorden(m_pRoot, func, std::forward<Args>(args)...);
    }

    // ForEach (alias de Inorden)
    template <typename Func, typename... Args>
    void ForEach(Func func, Args&&... args) {
        InternalInorden(m_pRoot, func, std::forward<Args>(args)...);
    }

    // FirstThat
    template <typename Func, typename... Args>
    value_type* FirstThat(Func func, Args&&... args) {
        Node* found = InternalFirstThat(m_pRoot, func, std::forward<Args>(args)...);
        if (found) return &found->m_data;
        return nullptr;
    }

    // operator<< inserta un elemento con ref -1
    CBinaryTree& operator<<(const value_type& val) {
        Insert(val, -1);
        return *this;
    }

    // operator>> extrae el minimo (leftmost)
    CBinaryTree& operator>>(value_type& val) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_pRoot) return *this;
        Node* parent = nullptr;
        Node* curr   = m_pRoot;
        while (curr->m_pChild[0]) {
            parent = curr;
            curr   = curr->m_pChild[0];
        }
        val = curr->m_data;
        if (parent) parent->m_pChild[0] = curr->m_pChild[1];
        else        m_pRoot             = curr->m_pChild[1];
        delete curr;
        return *this;
    }

    // operator<< para imprimir
    friend std::ostream& operator<<(std::ostream& os, CBinaryTree& tree) {
        for (auto it = tree.begin(); it != tree.end(); ++it)
            os << *it << " ";
        return os;
    }
};

#endif // __BINARYTREE_H__