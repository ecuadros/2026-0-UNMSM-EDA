#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <functional>
#include <stack>
#include "../general/types.h"
#include "../util.h"


// TRAITS

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


// FORWARD DECLARATIONS

template <typename Traits> class NodeBinaryTree;
template <typename Traits> class CBinaryTree;
template <typename Traits> class BinaryTreeForwardIterator;
template <typename Traits> class BinaryTreeBackwardIterator;
template <typename Traits> class CAVL; 

template <typename Traits>
std::ostream& operator<<(std::ostream& os, CBinaryTree<Traits>& tree);

template <typename Traits>
std::istream& operator>>(std::istream& is, CBinaryTree<Traits>& tree);


// NODO BINARY TREE

template <typename Traits>
class NodeBinaryTree{
    template <typename T> friend class CBinaryTree;
    template <typename T> friend class BinaryTreeForwardIterator;
    template <typename T> friend class BinaryTreeBackwardIterator;
    template <typename T> friend class CAVL; 
    
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;
    
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pChild[2] = {nullptr, nullptr};  
    int m_height = 1; 
    
public:
    NodeBinaryTree() : m_ref(-1) {}
    NodeBinaryTree(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref) {}
        
    value_type  GetValue() const { return m_data; }
    value_type &GetValueRef() { return m_data; }
    ref_type    GetRef() const { return m_ref; }
    
    Node *GetLeft() const { return m_pChild[0]; }
    Node *GetRight() const { return m_pChild[1]; }
};


// FORWARD ITERATOR 

template <typename Traits>
class BinaryTreeForwardIterator {
    using Node = NodeBinaryTree<Traits>;
    using value_type = typename Traits::value_type;
    
    std::stack<Node*> m_stack;
    Node *m_pCurrent;
    
    void PushLeft(Node *node) {
        while (node) {
            m_stack.push(node);
            node = node->m_pChild[0];
        }
    }
    
public:
    BinaryTreeForwardIterator(Node *root) : m_pCurrent(nullptr) {
        PushLeft(root);
        if (!m_stack.empty()) {
            m_pCurrent = m_stack.top();
            m_stack.pop();
        }
    }
    
    value_type &operator*() { return m_pCurrent->GetValueRef(); }
    
    BinaryTreeForwardIterator &operator++() {
        if (m_pCurrent->m_pChild[1]) {
            PushLeft(m_pCurrent->m_pChild[1]);
        }
        
        if (!m_stack.empty()) {
            m_pCurrent = m_stack.top();
            m_stack.pop();
        } else {
            m_pCurrent = nullptr;
        }
        return *this;
    }
    
    bool operator!=(const BinaryTreeForwardIterator &other) const {
        return m_pCurrent != other.m_pCurrent;
    }
};

// BACKWARD ITERATOR 

template <typename Traits>
class BinaryTreeBackwardIterator {
    using Node = NodeBinaryTree<Traits>;
    using value_type = typename Traits::value_type;
    
    std::stack<Node*> m_stack;
    Node *m_pCurrent;
    
    void PushRight(Node *node) {
        while (node) {
            m_stack.push(node);
            node = node->m_pChild[1];
        }
    }
    
public:
    BinaryTreeBackwardIterator(Node *root) : m_pCurrent(nullptr) {
        PushRight(root);
        if (!m_stack.empty()) {
            m_pCurrent = m_stack.top();
            m_stack.pop();
        }
    }
    
    value_type &operator*() { return m_pCurrent->GetValueRef(); }
    
    BinaryTreeBackwardIterator &operator++() {
        if (m_pCurrent->m_pChild[0]) {
            PushRight(m_pCurrent->m_pChild[0]);
        }
        
        if (!m_stack.empty()) {
            m_pCurrent = m_stack.top();
            m_stack.pop();
        } else {
            m_pCurrent = nullptr;
        }
        return *this;
    }
    
    bool operator!=(const BinaryTreeBackwardIterator &other) const {
        return m_pCurrent != other.m_pCurrent;
    }
};

// CLASE BINARY TREE

template <typename Traits>
class CBinaryTree{
public:
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;
    using  forward_iterator = BinaryTreeForwardIterator<Traits>;
    using  backward_iterator = BinaryTreeBackwardIterator<Traits>;
    
protected: 
    Node *m_pRoot = nullptr;
    size_t m_nElements = 0;
    CompareFunc comp;
    mutable std::mutex m_mutex;

private:
    // Métodos auxiliares recursivos
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
        if( !rParent ){
            rParent = new Node(val, ref);
            ++m_nElements;
            return;
        }
        auto path = comp(val, rParent->GetValue());
        InternalInsert(rParent->m_pChild[path], val, ref);
    }
    
    Node* InternalCopy(Node *node) {
        if (!node) return nullptr;
        Node *newNode = new Node(node->m_data, node->m_ref);
        newNode->m_pChild[0] = InternalCopy(node->m_pChild[0]);
        newNode->m_pChild[1] = InternalCopy(node->m_pChild[1]);
        return newNode;
    }
    
    void InternalClear(Node *node) {
        if (!node) return;
        InternalClear(node->m_pChild[0]);
        InternalClear(node->m_pChild[1]);
        delete node;
    }
    
    Node* InternalFindMin(Node *node) {
        while (node && node->m_pChild[0]) {
            node = node->m_pChild[0];
        }
        return node;
    }
    
    Node* InternalRemove(Node *node, const value_type &val) {
        if (!node) return nullptr;
        
        if (val == node->m_data) {
            // Nodo encontrado
            if (!node->m_pChild[0]) {
                Node *temp = node->m_pChild[1];
                delete node;
                --m_nElements;
                return temp;
            } else if (!node->m_pChild[1]) {
                Node *temp = node->m_pChild[0];
                delete node;
                --m_nElements;
                return temp;
            }
            
            // Nodo con dos hijos
            Node *temp = InternalFindMin(node->m_pChild[1]);
            node->m_data = temp->m_data;
            node->m_ref = temp->m_ref;
            node->m_pChild[1] = InternalRemove(node->m_pChild[1], temp->m_data);
        } else {
            // Usamos "comp" para saber si ir a la izquierda o derecha
            size_t path = comp(val, node->m_data);
            node->m_pChild[path] = InternalRemove(node->m_pChild[path], val);
        }
        return node;
    }
    
    // Recorridos con variadico
    template<typename Func, typename... Args>
    void InternalInorden(Node *node, Func fn, Args... args) {
        if (!node) return;
        InternalInorden(node->m_pChild[0], fn, args...);
        fn(node->GetValueRef(), args...);
        InternalInorden(node->m_pChild[1], fn, args...);
    }
    
    template<typename Func, typename... Args>
    void InternalPreorden(Node *node, Func fn, Args... args) {
        if (!node) return;
        fn(node->GetValueRef(), args...);
        InternalPreorden(node->m_pChild[0], fn, args...);
        InternalPreorden(node->m_pChild[1], fn, args...);
    }
    
    template<typename Func, typename... Args>
    void InternalPosorden(Node *node, Func fn, Args... args) {
        if (!node) return;
        InternalPosorden(node->m_pChild[0], fn, args...);
        InternalPosorden(node->m_pChild[1], fn, args...);
        fn(node->GetValueRef(), args...);
    }
    
    
    // FirstThat con VARIADICO 
    
    template<typename Predicate, typename... Args>
    value_type* InternalFirstThat(Node *node, Predicate pred, Args... args) {
        if (!node) return nullptr;
        
        // Inorden
        value_type *result = InternalFirstThat(node->m_pChild[0], pred, args...);
        if (result) return result;
        
        if (pred(node->GetValueRef(), args...)) return &(node->GetValueRef());
        
        return InternalFirstThat(node->m_pChild[1], pred, args...);
    }

public:
    CBinaryTree() = default;
    
    
    // Constructor Copia
    
    CBinaryTree(const CBinaryTree &another){
        std::lock_guard<std::mutex> lock(another.m_mutex);
        m_pRoot = InternalCopy(another.m_pRoot);
        m_nElements = another.m_nElements;
    }
    
    
    // Move Constructor
    
    CBinaryTree(CBinaryTree &&another) noexcept {
        std::lock_guard<std::mutex> lock(another.m_mutex);
        m_pRoot = another.m_pRoot;
        m_nElements = another.m_nElements;
        
        another.m_pRoot = nullptr;
        another.m_nElements = 0;
    }
    
    
    // Destructor
    
    virtual ~CBinaryTree(){
        Clear();
    }
    
    
    // Iterators

    forward_iterator begin() { return forward_iterator(m_pRoot); }
    forward_iterator end() { return forward_iterator(nullptr); }
    
    backward_iterator rbegin() { return backward_iterator(m_pRoot); }
    backward_iterator rend() { return backward_iterator(nullptr); }
    
    
    // Insert
    
    virtual void Insert(const value_type &val, ref_type ref = -1){ 
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalInsert(m_pRoot, val, ref);
    }
    
    
    // Remove
    
    virtual void Remove(const value_type &val) { 
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pRoot = InternalRemove(m_pRoot, val);
    }
    
    
    // Recorridos con variadico
    
    template<typename Func, typename... Args>
    void Inorden(Func fn, Args... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalInorden(m_pRoot, fn, args...);
    }
    
    template<typename Func, typename... Args>
    void Preorden(Func fn, Args... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalPreorden(m_pRoot, fn, args...);
    }
    
    template<typename Func, typename... Args>
    void Posorden(Func fn, Args... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalPosorden(m_pRoot, fn, args...);
    }
    
    
    // Foreach 
    
    template<typename Func, typename... Args>
    void Foreach(Func fn, Args... args) {
        Inorden(fn, args...);
    }
    
    
    // FirstThat con VARIADICO
    
    template<typename Predicate, typename... Args>
    value_type* FirstThat(Predicate pred, Args... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return InternalFirstThat(m_pRoot, pred, args...);
    }
    
    
    // Clear
    
    void Clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalClear(m_pRoot);
        m_pRoot = nullptr;
        m_nElements = 0;
    }
    
    
    // getSize
    
    size_t getSize() const { return m_nElements; }
    
    bool isEmpty() const { return m_pRoot == nullptr; }
    
    
    // Operadores
    
    bool operator<(const CBinaryTree &other) const {
        return m_nElements < other.m_nElements;
    }
    
    bool operator>(const CBinaryTree &other) const {
        return m_nElements > other.m_nElements;
    }
    
    bool operator==(const CBinaryTree &other) const {
        return m_nElements == other.m_nElements;
    }
    
    // Friend
    friend std::ostream& operator<< <Traits>(std::ostream& os, CBinaryTree<Traits>& tree);
    friend std::istream& operator>> <Traits>(std::istream& is, CBinaryTree<Traits>& tree);
};

// Operator <<

template <typename Traits>
std::ostream& operator<<(std::ostream& os, CBinaryTree<Traits>& tree) {
    std::lock_guard<std::mutex> lock(tree.m_mutex);
    os << "CBinaryTree: size = " << tree.m_nElements << std::endl;
    os << "[ ";
    
    // Mostrar en inorden
    tree.InternalInorden(tree.m_pRoot, [&os](auto &val) {
        os << val << " ";
    });
    
    os << "]" << std::endl;
    return os;
}


// Operator >>

template <typename Traits>
std::istream& operator>>(std::istream& is, CBinaryTree<Traits>& tree) {
    typename Traits::value_type val;
    if (is >> val) {
        tree.Insert(val);
    }
    return is;
}

#endif // __BINARYTREE_H__