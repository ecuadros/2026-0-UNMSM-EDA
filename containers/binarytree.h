#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <cctype>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>
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

// declaracion previa para hacerla friend en NodeBinaryTree
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
class NodeBinaryTree{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;
    friend class CBinaryTree<Traits>;
    friend class BinaryTreeForwardIterator<Traits>;
    friend class BinaryTreeBackwardIterator<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pChild[2]  = {nullptr, nullptr};
public:
    NodeBinaryTree(){}
    NodeBinaryTree( value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){   }
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }
    ref_type    GetRef() const { return m_ref; }
    ref_type   &GetRefRef() { return m_ref; }
};

template <typename Traits>
class CBinaryTree{
public:
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;
    using  forward_iterator  = BinaryTreeForwardIterator<Traits>;
    using  backward_iterator = BinaryTreeBackwardIterator<Traits>;
    friend class BinaryTreeForwardIterator<Traits>;
    friend class BinaryTreeBackwardIterator<Traits>;
protected:
    mutable mutex mtx;
private:
    Node *m_pRoot = nullptr;
    CompareFunc comp;

public:
    CBinaryTree(){}
    // TODO: Copy constructor
    CBinaryTree(const CBinaryTree &another) {
        lock_guard lock(another.mtx);
        m_pRoot = _clone(another.m_pRoot);
    }
    // TODO: Move constructor
    CBinaryTree(CBinaryTree &&another) {
        lock_guard lock(another.mtx);
        m_pRoot = std::exchange(another.m_pRoot, nullptr);
    }
    virtual ~CBinaryTree() {
        lock_guard lock(mtx);
        _clear_unlocked();
    }

private:
    static Node *child(Node *node, size_t index) {
        return node ? node->m_pChild[index] : nullptr;
    }

    static void _serialize_node(ostream &os, Node *node) {
        if (!node) {
            os << "# ";
            return;
        }
        if constexpr (std::is_same_v<value_type, std::string>) {
            os << "(" << std::quoted(node->GetValue()) << ":" << node->m_ref << ") ";
        } else {
            os << "(" << node->GetValue() << ":" << node->m_ref << ") ";
        }
        _serialize_node(os, node->m_pChild[0]);
        _serialize_node(os, node->m_pChild[1]);
    }

    static Node *_deserialize_node(istream &is) {
        char ch;
        do {
            if (!is.get(ch)) return nullptr;
        } while (isspace(static_cast<unsigned char>(ch)));

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
        node->m_pChild[0] = _deserialize_node(is);
        node->m_pChild[1] = _deserialize_node(is);
        return node;
    }

    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
        if ( !rParent ) {
            rParent = new Node(val, ref);
            return;
        }
        auto path = comp(val, rParent->GetValue());
        InternalInsert(rParent->m_pChild[path], val, ref);
    }

    value_type _replaceRemovalTarget(Node *&rCurrent, size_t from) {
        // "from" means
        // 0: left
        // 1: right

        Node *parent = rCurrent;
        Node *trav = rCurrent->m_pChild[from];
        if (!trav) return rCurrent->GetValue();

        // buscar el maximo del subtree izquierdo (from=0) o
        // el minimo del subtree derecho (from=1)
        while (trav->m_pChild[!from]) {
            parent = trav;
            trav = trav->m_pChild[!from];
        }

        rCurrent->GetValueRef() = trav->GetValue();

        Node *child = trav->m_pChild[from];
        if (parent == rCurrent) {
            parent->m_pChild[from] = child;
        } else {
            parent->m_pChild[!from] = child;
        }
        delete trav;
        return rCurrent->GetValueRef();
    }

    void _remove(Node *&rTarget) {
        if (!rTarget) return;

        // caso 1: hoja
        if (!rTarget->m_pChild[0] && !rTarget->m_pChild[1]) {
            delete rTarget;
            rTarget = nullptr;
            return;
        }
        // caso 2: un solo hijo
        if (!rTarget->m_pChild[0]) {
            Node *tmp = rTarget;
            rTarget = rTarget->m_pChild[1];
            delete tmp;
            return;
        }
        if (!rTarget->m_pChild[1]) {
            Node *tmp = rTarget;
            rTarget = rTarget->m_pChild[0];
            delete tmp;
            return;
        }
        // caso 3: dos hijos
        _replaceRemovalTarget(rTarget, 0);
    }
    template <typename Func, typename...Args>
    void _preorderTraversal(Node *current, Func foo, Args ...args) {
        if ( !current ) return;
        foo(current->GetValueRef(), args...);
        _preorderTraversal(current->m_pChild[0], foo, args...);
        _preorderTraversal(current->m_pChild[1], foo, args...);
    }

    template <typename Func, typename...Args>
    void _inorderTraversal(Node *current, Func foo, Args ...args) {
        if ( !current ) return;
        _inorderTraversal(current->m_pChild[0], foo, args...);
        foo(current->GetValueRef(), args...);
        _inorderTraversal(current->m_pChild[1], foo, args...);
    }

    template <typename Func, typename...Args>
    void _postorderTraversal(Node *current, Func foo, Args ...args) {
        if ( !current ) return;
        _postorderTraversal(current->m_pChild[0], foo, args...);
        _postorderTraversal(current->m_pChild[1], foo, args...);
        foo(current->GetValueRef(), args...);
    }

    // versiones estaticas de los traversals para copiar los nodos
    // y para el destructor
    // el usuario no deberia tener acceso a los nodos, entonces estas
    // helpers ayudan con eso

    // usa preorden
    static Node *_clone(Node *src) {
        if (!src) return nullptr;
        Node *node = new Node(src->m_data, src->m_ref);
        node->m_pChild[0] = _clone(src->m_pChild[0]);
        node->m_pChild[1] = _clone(src->m_pChild[1]);
        return node;
    }

    // usa postorden
    static void _clear_nodes(Node *node) {
        if (!node) return;
        _clear_nodes(node->m_pChild[0]);
        _clear_nodes(node->m_pChild[1]);
        delete node;
    }

    void _clear_unlocked() {
        _clear_nodes(m_pRoot);
        m_pRoot = nullptr;
    }

public:
    void Insert(const value_type &val, ref_type ref) {
        lock_guard lock(mtx);
        InternalInsert(m_pRoot, val, ref);
    }

    template <typename Func, typename... Args>
    void preorderTraversal(Func foo, Args ...args) {
        lock_guard lock(mtx);
        _preorderTraversal(m_pRoot, foo, args...);
    }

    template <typename Func, typename... Args>
    void inorderTraversal(Func foo, Args ...args) {
        lock_guard lock(mtx);
        _inorderTraversal(m_pRoot, foo, args...);
    }

    template <typename Func, typename... Args>
    void postorderTraversal(Func foo, Args ...args) {
        lock_guard lock(mtx);
        _postorderTraversal(m_pRoot, foo, args...);
    }

    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args) {
        ::Foreach(*this, of, args...);
    }

    template <typename ObjFunc, typename ...Args>
    value_type FirstThat(ObjFunc of, Args... args) {
        return *::FirstThat(*this, of, args...);
    }

    forward_iterator begin() { return forward_iterator(this, false); }
    forward_iterator end() { return forward_iterator(this, true); }
    backward_iterator rbegin() { return backward_iterator(this, false); }
    backward_iterator rend() { return backward_iterator(this, true); }

    value_type remove(value_type &val) {
        lock_guard lock(mtx);
        Node **link = &m_pRoot;

        // llegar hasta la referencia donde deberia estar el objetivo
        while (*link != nullptr) {
            // si hemos encontrado el valor, eliminar el nodo
            if ((*link)->GetValue() == val) {
                value_type removed = (*link)->GetValue();
                _remove(*link);
                return removed;
            }
            // sino continuar iterando
            if (comp(val, (*link)->GetValue())) link = &((*link)->m_pChild[0]);
            else link = &((*link)->m_pChild[1]);
        }

        // si llegamos hasta aca es porque no hay nodo
        return value_type {};
    }

    friend ostream& operator<< <>(ostream &os, CBinaryTree<Traits> &tree);
    friend istream& operator>> <>(istream &is, CBinaryTree<Traits> &tree);
};

template <typename Traits>
ostream &operator<<(ostream &os, CBinaryTree<Traits> &tree) {
    lock_guard<mutex> lock(tree.mtx);
    os << "CBinaryTree [ ";
    tree._serialize_node(os, tree.m_pRoot);
    os << "]";
    return os;
}

template <typename Traits>
istream &operator>>(istream &is, CBinaryTree<Traits> &tree) {
    if (!is) return is;

    string bar;
    getline(is, bar, '[');
    lock_guard<mutex> lock(tree.mtx);
    tree._clear_unlocked();

    tree.m_pRoot = tree._deserialize_node(is);
    if (!is) {
        tree._clear_unlocked();
    } else {
        char ch;
        while (is.get(ch)) {
            if (ch == ']') break;
            if (!isspace(static_cast<unsigned char>(ch))) {
                is.setstate(ios::failbit);
                tree._clear_unlocked();
                break;
            }
        }
    }
    return is;
}

template <typename Traits>
class BinaryTreeForwardIterator {
    using Tree = CBinaryTree<Traits>;
    using Node = typename Tree::Node;
    using value_type = typename Tree::value_type;

    // tendra un puntero al arbol original y al nodo actual
    Tree *m_tree = nullptr;
    std::vector<Node*> m_stack;
    Node *m_current = nullptr;

    // en un iterador se reciben los valores de manera ascendente, esta funcion
    // se llama cada que se avanza a la derecha, para acumular los valores
    // en orden y no saltarse ninguno
    void push_left(Node *node) {
        while (node) {
            m_stack.push_back(node);
            node = Tree::child(node, 0);
        }
    }

    // helper para avanzar en el iterador
    void advance() {
        if (m_stack.empty()) {
            m_current = nullptr;
            return;
        }
        Node *node = m_stack.back();
        m_stack.pop_back();
        if (Tree::child(node, 1)) {
            push_left(Tree::child(node, 1));
        }
        m_current = m_stack.empty() ? nullptr : m_stack.back();
    }

public:
    BinaryTreeForwardIterator(Tree *tree, bool is_end=false) : m_tree(tree) {
        if (!is_end && m_tree && m_tree->m_pRoot) {
            push_left(m_tree->m_pRoot);
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

    // al igual que en ForwardIterator, esta funcion sirve para acumular
    // los nodos en orden y no pasarse ninguno
    void push_right(Node *node) {
        while (node) {
            m_stack.push_back(node);
            node = Tree::child(node, 1);
        }
    }

    void advance() {
        if (m_stack.empty()) {
            m_current = nullptr;
            return;
        }
        Node *node = m_stack.back();
        m_stack.pop_back();
        if (Tree::child(node, 0)) {
            push_right(Tree::child(node, 0));
        }
        m_current = m_stack.empty() ? nullptr : m_stack.back();
    }

public:
    BinaryTreeBackwardIterator(Tree *tree, bool is_end=false) : m_tree(tree) {
        if (!is_end && m_tree && m_tree->m_pRoot) {
            push_right(m_tree->m_pRoot);
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
