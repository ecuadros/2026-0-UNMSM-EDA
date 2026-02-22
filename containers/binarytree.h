#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <mutex>
#include <utility>
#include <vector>
#include <type_traits>
#include <iomanip>
#include "../general/types.h"
#include "../util.h"

using namespace std;

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
class CBinaryTree;

template <typename Traits>
ostream& operator<<(ostream &os, CBinaryTree<Traits> &tree);

template <typename Traits>
istream& operator>>(istream &is, CBinaryTree<Traits> &tree);

template <typename Traits>
class BinaryTreeForwardIterator;

template <typename Traits>
class BinaryTreeBackwardIterator;

template <typename Traits>
class NodeBinaryTree {
    using value_type  = typename Traits::value_type;
    using Node        = NodeBinaryTree<Traits>;
    using CompareFunc = typename Traits::CompareFunc;

private:
    friend class CBinaryTree<Traits>;
    friend class BinaryTreeForwardIterator<Traits>;
    friend class BinaryTreeBackwardIterator<Traits>;

    value_type m_data;
    ref_type   m_ref;
    Node *m_pChild[2] = {nullptr, nullptr};

public:
    size_t m_height = 1;  // for AVL

    NodeBinaryTree() {}
    NodeBinaryTree(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref) {}

    value_type  GetValue() const { return m_data; }
    value_type &GetValueRef() { return m_data; }
    ref_type    GetRef() const { return m_ref; }
    ref_type   &GetRefRef() { return m_ref; }
};

template <typename Traits>
class CBinaryTree {
public:
    using value_type  = typename Traits::value_type;
    using Node        = NodeBinaryTree<Traits>;
    using CompareFunc = typename Traits::CompareFunc;

private:
    using forward_iterator  = BinaryTreeForwardIterator<Traits>;
    using backward_iterator = BinaryTreeBackwardIterator<Traits>;

    friend class BinaryTreeForwardIterator<Traits>;
    friend class BinaryTreeBackwardIterator<Traits>;

protected:
    mutable mutex mtx;
    Node *m_pRoot = nullptr;
    CompareFunc comp;

public:
    
    static Node *getChild(Node *node, size_t idx) {
        return node ? node->m_pChild[idx] : nullptr;
    }

    static Node *&getChildRef(Node *node, size_t idx) {
        return node->m_pChild[idx];
    }

    CBinaryTree() {}

    
    CBinaryTree(const CBinaryTree &another) {
        lock_guard lock(another.mtx);
        m_pRoot = _cloneTree(another.m_pRoot);
    }

    
    CBinaryTree(CBinaryTree &&another) noexcept {
        lock_guard lock(another.mtx);
        m_pRoot = std::exchange(another.m_pRoot, nullptr);
    }

    
    CBinaryTree &operator=(CBinaryTree &&another) noexcept {
        if (this == &another) return *this;
        scoped_lock lock(mtx, another.mtx);
        _clearUnlocked();
        m_pRoot = std::exchange(another.m_pRoot, nullptr);
        return *this;
    }

    virtual ~CBinaryTree() {
        lock_guard lock(mtx);
        _clearUnlocked();
    }

private:
    
    void _serializeNode(ostream &os, Node *node) const {
        if (!node) {
            os << "#,";
            return;
        }
        if constexpr (std::is_same_v<value_type, std::string>) {
            os << "(" << std::quoted(node->GetValue()) << ":" << node->m_ref << "),";
        } else {
            os << "(" << node->GetValue() << ":" << node->m_ref << "),";
        }
        _serializeNode(os, node->m_pChild[0]);
        _serializeNode(os, node->m_pChild[1]);
    }

    Node *_deserializeNode(istream &is) {
        char ch;
        if (!is.get(ch)) return nullptr;
        while (ch == ',') {
            if (!is.get(ch)) return nullptr;
        }

        if (ch == '#') return nullptr;
        if (ch != '(') {
            is.setstate(ios::failbit);
            return nullptr;
        }

        value_type val;
        if constexpr (std::is_same_v<value_type, std::string>) {
            is >> std::quoted(val);
        } else {
            is >> val;
        }

        is >> ch;
        if (!is || ch != ':') {
            is.setstate(ios::failbit);
            return nullptr;
        }

        ref_type ref;
        is >> ref;
        is >> ch;
        if (!is || ch != ')') {
            is.setstate(ios::failbit);
            return nullptr;
        }

        Node *node = new Node(val, ref);
        node->m_pChild[0] = _deserializeNode(is);
        node->m_pChild[1] = _deserializeNode(is);
        return node;
    }

    
    void _insertNode(Node *&parent, const value_type &val, ref_type ref) {
        if (!parent) {
            parent = new Node(val, ref);
            return;
        }
        size_t dir = comp(val, parent->GetValue());
        _insertNode(parent->m_pChild[dir], val, ref);
    }

    value_type _findAndReplace(Node *&current, size_t fromDir) {
        Node *parent = current;
        Node *trav = current->m_pChild[fromDir];
        if (!trav) return current->GetValue();

        while (trav->m_pChild[!fromDir]) {
            parent = trav;
            trav = trav->m_pChild[!fromDir];
        }

        current->GetValueRef() = trav->GetValue();

        Node *child = trav->m_pChild[fromDir];
        if (parent == current) {
            parent->m_pChild[fromDir] = child;
        } else {
            parent->m_pChild[!fromDir] = child;
        }
        delete trav;
        return current->GetValueRef();
    }

    void _deleteNode(Node *&target) {
        if (!target) return;

        if (!target->m_pChild[0] && !target->m_pChild[1]) {
            delete target;
            target = nullptr;
            return;
        }

        if (!target->m_pChild[0]) {
            Node *tmp = target;
            target = target->m_pChild[1];
            delete tmp;
            return;
        }

        if (!target->m_pChild[1]) {
            Node *tmp = target;
            target = target->m_pChild[0];
            delete tmp;
            return;
        }

        _findAndReplace(target, 0);
    }

    
    template <typename Func, typename... Args>
    void _preorderWalk(Node *current, Func f, Args&&... args) {
        if (!current) return;
        f(current->GetValueRef(), std::forward<Args>(args)...);
        _preorderWalk(current->m_pChild[0], f, std::forward<Args>(args)...);
        _preorderWalk(current->m_pChild[1], f, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void _inorderWalk(Node *current, Func f, Args&&... args) {
        if (!current) return;
        _inorderWalk(current->m_pChild[0], f, std::forward<Args>(args)...);
        f(current->GetValueRef(), std::forward<Args>(args)...);
        _inorderWalk(current->m_pChild[1], f, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void _postorderWalk(Node *current, Func f, Args&&... args) {
        if (!current) return;
        _postorderWalk(current->m_pChild[0], f, std::forward<Args>(args)...);
        _postorderWalk(current->m_pChild[1], f, std::forward<Args>(args)...);
        f(current->GetValueRef(), std::forward<Args>(args)...);
    }

    
    static Node *_cloneTree(Node *src) {
        if (!src) return nullptr;
        Node *node = new Node(src->m_data, src->m_ref);
        node->m_pChild[0] = _cloneTree(src->m_pChild[0]);
        node->m_pChild[1] = _cloneTree(src->m_pChild[1]);
        return node;
    }

    
    static void _deleteAllNodes(Node *node) {
        if (!node) return;
        _deleteAllNodes(node->m_pChild[0]);
        _deleteAllNodes(node->m_pChild[1]);
        delete node;
    }

    void _clearUnlocked() {
        _deleteAllNodes(m_pRoot);
        m_pRoot = nullptr;
    }

public:
    virtual void Insert(const value_type &val, ref_type ref) {
        lock_guard lock(mtx);
        _insertNode(m_pRoot, val, ref);
    }

    virtual value_type remove(value_type &val) {
        lock_guard lock(mtx);
        Node *parent = nullptr;
        Node *current = m_pRoot;
        size_t dir = 0;

        while (current != nullptr) {
            if (current->GetValue() == val) {
                value_type removed = current->GetValue();
                if (!parent) _deleteNode(m_pRoot);
                else _deleteNode(parent->m_pChild[dir]);
                return removed;
            }
            dir = comp(val, current->GetValue());
            parent = current;
            current = current->m_pChild[dir];
        }

        return value_type{};
    }

    
    template <typename Func, typename... Args>
    void preorderTraversal(Func f, Args&&... args) {
        lock_guard lock(mtx);
        _preorderWalk(m_pRoot, f, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void inorderTraversal(Func f, Args&&... args) {
        lock_guard lock(mtx);
        _inorderWalk(m_pRoot, f, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void postorderTraversal(Func f, Args&&... args) {
        lock_guard lock(mtx);
        _postorderWalk(m_pRoot, f, std::forward<Args>(args)...);
    }

    template <typename ObjFunc, typename... Args>
    void Foreach(ObjFunc f, Args&&... args) {
        lock_guard lock(mtx);
        ::Foreach(*this, f, std::forward<Args>(args)...);
    }

    template <typename ObjFunc, typename... Args>
    value_type FirstThat(ObjFunc f, Args&&... args) {
        lock_guard lock(mtx);
        return *::FirstThat(*this, f, std::forward<Args>(args)...);
    }

    
    forward_iterator begin() { return forward_iterator(this, false); }
    forward_iterator end() { return forward_iterator(this, true); }
    backward_iterator rbegin() { return backward_iterator(this, false); }
    backward_iterator rend() { return backward_iterator(this, true); }

    friend ostream& operator<< <>(ostream &os, CBinaryTree<Traits> &tree);
    friend istream& operator>> <>(istream &is, CBinaryTree<Traits> &tree);
};

template <typename Traits>
ostream &operator<<(ostream &os, CBinaryTree<Traits> &tree) {
    lock_guard<mutex> lock(tree.mtx);
    os << "CBinaryTree [";
    tree._serializeNode(os, tree.m_pRoot);
    os << "]";
    return os;
}

template <typename Traits>
istream &operator>>(istream &is, CBinaryTree<Traits> &tree) {
    if (!is) return is;

    string bar;
    getline(is, bar, '[');
    lock_guard<mutex> lock(tree.mtx);
    tree._clearUnlocked();

    tree.m_pRoot = tree._deserializeNode(is);
    if (!is) tree._clearUnlocked();
    else {
        char ch;
        while (is.get(ch)) {
            if (ch == ']') break;
            if (ch == ',') continue;
            is.setstate(ios::failbit);
            tree._clearUnlocked();
            break;
        }
    }
    return is;
}

template <typename Traits>
class BinaryTreeForwardIterator {
    using Tree = CBinaryTree<Traits>;
    using Node = typename Tree::Node;
    using value_type = typename Tree::value_type;

    Tree *m_tree = nullptr;
    std::vector<Node*> m_stack;
    Node *m_current = nullptr;

    void pushLeft(Node *node) {
        while (node) {
            m_stack.push_back(node);
            node = Tree::getChild(node, 0);
        }
    }

    void advance() {
        if (m_stack.empty()) {
            m_current = nullptr;
            return;
        }
        Node *node = m_stack.back();
        m_stack.pop_back();
        if (Tree::getChild(node, 1)) {
            pushLeft(Tree::getChild(node, 1));
        }
        m_current = m_stack.empty() ? nullptr : m_stack.back();
    }

public:
    BinaryTreeForwardIterator(Tree *tree, bool is_end = false) : m_tree(tree) {
        if (!is_end && m_tree && m_tree->m_pRoot) {
            pushLeft(m_tree->m_pRoot);
            m_current = m_stack.empty() ? nullptr : m_stack.back();
        }
    }

    value_type &operator*() { return m_current->GetValueRef(); }

    BinaryTreeForwardIterator &operator++() {
        advance();
        return *this;
    }

    bool operator!=(const BinaryTreeForwardIterator &other) const {
        return m_current != other.m_current || m_tree != other.m_tree;
    }
};

template <typename Traits>
class BinaryTreeBackwardIterator {
    using Tree = CBinaryTree<Traits>;
    using Node = typename Tree::Node;
    using value_type = typename Tree::value_type;

    Tree *m_tree = nullptr;
    std::vector<Node*> m_stack;
    Node *m_current = nullptr;

    void pushRight(Node *node) {
        while (node) {
            m_stack.push_back(node);
            node = Tree::getChild(node, 1);
        }
    }

    void advance() {
        if (m_stack.empty()) {
            m_current = nullptr;
            return;
        }
        Node *node = m_stack.back();
        m_stack.pop_back();
        if (Tree::getChild(node, 0)) {
            pushRight(Tree::getChild(node, 0));
        }
        m_current = m_stack.empty() ? nullptr : m_stack.back();
    }

public:
    BinaryTreeBackwardIterator(Tree *tree, bool is_end = false) : m_tree(tree) {
        if (!is_end && m_tree && m_tree->m_pRoot) {
            pushRight(m_tree->m_pRoot);
            m_current = m_stack.empty() ? nullptr : m_stack.back();
        }
    }

    value_type &operator*() { return m_current->GetValueRef(); }

    BinaryTreeBackwardIterator &operator++() {
        advance();
        return *this;
    }

    bool operator!=(const BinaryTreeBackwardIterator &other) const {
        return m_current != other.m_current || m_tree != other.m_tree;
    }
};

void DemoBinaryTree();

#endif // __BINARYTREE_H__