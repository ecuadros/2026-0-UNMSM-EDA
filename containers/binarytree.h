#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <mutex>
#include <vector>
#include <functional>
#include <algorithm>
#include "../general/types.h"
#include "../util.h"

using namespace std;

template <typename T>
struct TreeTraitAscending {
    using value_type  = T;
    using CompareFunc = std::less<T>;
};

template <typename T>
struct TreeTraitDescending {
    using value_type  = T;
    using CompareFunc = std::greater<T>;
};

template <typename Traits> class CBinaryTree;
template <typename Traits> class BinaryTreeForwardIterator;
template <typename Traits> class BinaryTreeBackwardIterator;

template <typename Traits>
class NodeBinaryTree {
    using value_type = typename Traits::value_type;
    friend class CBinaryTree<Traits>;
    friend class BinaryTreeForwardIterator<Traits>;
    friend class BinaryTreeBackwardIterator<Traits>;
    friend class CBinaryTree<Traits>;
    template <typename T> friend class CAVL;

    value_type m_data;
    ref_type m_ref;
    NodeBinaryTree* m_pChild[2] = {nullptr, nullptr};
public:
    size_t m_height = 1; 
    NodeBinaryTree(value_type v, ref_type r = -1) : m_data(v), m_ref(r) {}
    value_type& GetValueRef() { return m_data; }
    ref_type GetRef() const { return m_ref; }
};

template <typename Traits>
class CBinaryTree {
public:
    using value_type = typename Traits::value_type;
    using Node = NodeBinaryTree<Traits>;
    using CompareFunc = typename Traits::CompareFunc;
    using forward_iterator = BinaryTreeForwardIterator<Traits>;
    using backward_iterator = BinaryTreeBackwardIterator<Traits>;

    friend class BinaryTreeForwardIterator<Traits>;
    friend class BinaryTreeBackwardIterator<Traits>;

protected:
    mutable recursive_mutex m_mutex; 
    Node* m_pRoot = nullptr;
    CompareFunc comp;

    void _internalInsert(Node*& node, const value_type& val, ref_type ref) {
        if (!node) {
            node = new Node(val, ref);
            return;
        }
        bool side = comp(node->m_data, val); 
        _internalInsert(node->m_pChild[side], val, ref);
    }

    void _internalRemove(Node*& node, const value_type& val) {
        if (!node) return;

        if (node->m_data == val) {
            Node* old = node;
            if (!node->m_pChild[0]) {
                node = node->m_pChild[1];
                delete old;
            } else if (!node->m_pChild[1]) {
                node = node->m_pChild[0];
                delete old;
            } else {
                _replaceTarget(old, node->m_pChild[0]);
            }
        } else {
            bool side = comp(node->m_data, val);
            _internalRemove(node->m_pChild[side], val);
        }
    }

    void _replaceTarget(Node* target, Node*& replacement) {
        if (replacement->m_pChild[1]) {
            _replaceTarget(target, replacement->m_pChild[1]);
        } else {
            target->m_data = replacement->m_data;
            target->m_ref = replacement->m_ref;
            Node* old = replacement;
            replacement = replacement->m_pChild[0];
            delete old;
        }
    }

    template <typename Func, typename... Args>
    void _internalInorder(Node* p, Func fn, Args&&... args) {
        if (!p) return;
        _internalInorder(p->m_pChild[0], fn, std::forward<Args>(args)...);
        fn(p->m_data, p->m_ref, std::forward<Args>(args)...);
        _internalInorder(p->m_pChild[1], fn, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void _internalPreorder(Node* p, Func fn, Args&&... args) {
        if (!p) return;
        fn(p->m_data, p->m_ref, std::forward<Args>(args)...); // Raíz
        _internalPreorder(p->m_pChild[0], fn, std::forward<Args>(args)...); // Izquierda
        _internalPreorder(p->m_pChild[1], fn, std::forward<Args>(args)...); // Derecha
    }

    template <typename Func, typename... Args>
    void _internalPostorder(Node* p, Func fn, Args&&... args) {
        if (!p) return;
        _internalPostorder(p->m_pChild[0], fn, std::forward<Args>(args)...); // Izquierda
        _internalPostorder(p->m_pChild[1], fn, std::forward<Args>(args)...); // Derecha
        fn(p->m_data, p->m_ref, std::forward<Args>(args)...); // Raíz
    }

    static Node* _clone(Node* src) {
        if (!src) return nullptr;
        Node* n = new Node(src->m_data, src->m_ref);
        n->m_pChild[0] = _clone(src->m_pChild[0]);
        n->m_pChild[1] = _clone(src->m_pChild[1]);
        return n;
    }

    static void _clear_nodes(Node* node) {
        if (!node) return;
        _clear_nodes(node->m_pChild[0]);
        _clear_nodes(node->m_pChild[1]);
        delete node;
    }

public:
    CBinaryTree() = default;

    // Constructor Copia
    CBinaryTree(const CBinaryTree& other) {
        lock_guard<recursive_mutex> lock(other.m_mutex);
        m_pRoot = _clone(other.m_pRoot);
    }

    // Move Constructor
    CBinaryTree(CBinaryTree&& other) noexcept {
        lock_guard<recursive_mutex> lock(other.m_mutex);
        m_pRoot = std::exchange(other.m_pRoot, nullptr);
    }

    // Destructor Seguro
    virtual ~CBinaryTree() {
        lock_guard<recursive_mutex> lock(m_mutex);
        _clear_nodes(m_pRoot);
    }

    // INSERT
    virtual void Insert(const value_type& val, ref_type ref = -1) {
        lock_guard<recursive_mutex> lock(m_mutex);
        _internalInsert(m_pRoot, val, ref);
    }

    // REMOVE
    virtual void Remove(const value_type& val) {
        lock_guard<recursive_mutex> lock(m_mutex);
        _internalRemove(m_pRoot, val);
    }

    // FOREACH 
    template <typename Func, typename... Args>
    void Foreach(Func fn, Args&&... args) {
        lock_guard<recursive_mutex> lock(m_mutex);
        _internalInorder(m_pRoot, fn, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void Preorder(Func fn, Args&&... args) {
        lock_guard<recursive_mutex> lock(m_mutex);
        _internalPreorder(m_pRoot, fn, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void Postorder(Func fn, Args&&... args) {
        lock_guard<recursive_mutex> lock(m_mutex);
        _internalPostorder(m_pRoot, fn, std::forward<Args>(args)...);
    }

    // FIRSTTHAT
    template <typename Func, typename... Args>
    value_type* FirstThat(Func fn, Args&&... args) {
        lock_guard<recursive_mutex> lock(m_mutex);
        value_type* result = nullptr;
        _findFirst(m_pRoot, result, fn, std::forward<Args>(args)...);
        return result;
    }

    friend ostream& operator<<(ostream& os, CBinaryTree& tree) {
        lock_guard<recursive_mutex> lock(tree.m_mutex);
        tree.Foreach([&os](value_type& v, ref_type& r) {
            os << "(" << v << ":" << r << ") ";
        });
        return os;
    }

    template <typename Func, typename... Args>
    void _findFirst(Node* p, value_type*& res, Func fn, Args&&... args) {
        if (!p || res) return;
        _findFirst(p->m_pChild[0], res, fn, std::forward<Args>(args)...);
        if (!res && fn(p->m_data, p->m_ref, std::forward<Args>(args)...)) {
            res = &(p->m_data);
        }
        _findFirst(p->m_pChild[1], res, fn, std::forward<Args>(args)...);
    }

    // ITERADORES
    forward_iterator begin() { return forward_iterator(this, false); }
    forward_iterator end()   { return forward_iterator(this, true); }
    backward_iterator rbegin() { return backward_iterator(this, false); }
    backward_iterator rend()   { return backward_iterator(this, true); }
};

// FORWARD 
template <typename Traits>
class BinaryTreeForwardIterator {
    using Tree = CBinaryTree<Traits>;
    using Node = typename Tree::Node;
    Tree* m_tree;
    vector<Node*> m_stack;
    Node* m_current;
    void push_left(Node* n) { while(n){ m_stack.push_back(n); n = n->m_pChild[0]; } }
public:
    BinaryTreeForwardIterator(Tree* t, bool end) : m_tree(t), m_current(nullptr) {
        if(!end && m_tree && m_tree->m_pRoot){ push_left(m_tree->m_pRoot); m_current = m_stack.back(); }
    }
    auto& operator*() { return m_current->GetValueRef(); }
    BinaryTreeForwardIterator& operator++() {
        Node* n = m_stack.back(); m_stack.pop_back();
        push_left(n->m_pChild[1]);
        m_current = m_stack.empty() ? nullptr : m_stack.back();
        return *this;
    }
    bool operator!=(const BinaryTreeForwardIterator& o) const { return m_current != o.m_current; }
};

// BACKWARD
template <typename Traits>
class BinaryTreeBackwardIterator {
    using Tree = CBinaryTree<Traits>;
    using Node = typename Tree::Node;
    Tree* m_tree;
    vector<Node*> m_stack;
    Node* m_current;
    void push_right(Node* n) { while(n){ m_stack.push_back(n); n = n->m_pChild[1]; } }
public:
    BinaryTreeBackwardIterator(Tree* t, bool end) : m_tree(t), m_current(nullptr) {
        if(!end && m_tree && m_tree->m_pRoot){ push_right(m_tree->m_pRoot); m_current = m_stack.back(); }
    }
    auto& operator*() { return m_current->GetValueRef(); }
    BinaryTreeBackwardIterator& operator++() {
        Node* n = m_stack.back(); m_stack.pop_back();
        push_right(n->m_pChild[0]);
        m_current = m_stack.empty() ? nullptr : m_stack.back();
        return *this;
    }
    bool operator!=(const BinaryTreeBackwardIterator& o) const { return m_current != o.m_current; }
};

#endif