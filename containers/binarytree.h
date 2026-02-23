#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <mutex>
#include <stack>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include "../general/types.h"
#include "GeneralIterator.h"

using namespace std;

// traits
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

//forward iterator con stack
template <typename Container>
class BinaryTreeForwardIterator : public GeneralIterator<Container>{
    using Parent = GeneralIterator<Container>;
    using Node   = typename Container::Node;
private:
    stack<Node*> m_stack;
    void pushLeft(Node* n) {
        while(n) { m_stack.push(n); n = n->m_pChild[0]; }
    }
public:
    BinaryTreeForwardIterator(Container *pContainer, Node *pNode)
                      : Parent(pContainer, nullptr){
        if (pNode && pContainer && pNode == pContainer->m_pRoot) {
            pushLeft(pNode);
            if (!m_stack.empty()) Parent::m_pNode = m_stack.top();
        } else {
            Parent::m_pNode = pNode; // para el end()
        }
    }
    BinaryTreeForwardIterator(const BinaryTreeForwardIterator<Container> &another)
                      : Parent(another), m_stack(another.m_stack){}

    BinaryTreeForwardIterator<Container> &operator++(){
        if(m_stack.empty()) { Parent::m_pNode = nullptr; return *this; }
        Node* curr = m_stack.top();
        m_stack.pop();
        pushLeft(curr->m_pChild[1]);
        if (!m_stack.empty()) Parent::m_pNode = m_stack.top();
            else Parent::m_pNode = nullptr;
        return *this;
    }
};

// backward iterator con stack
template <typename Container>
class BinaryTreeBackwardIterator : public GeneralIterator<Container>{
    using Parent = GeneralIterator<Container>;
    using Node   = typename Container::Node;
private:
    stack<Node*> m_stack;
    void pushRight(Node* n) {
        while(n) { m_stack.push(n); n = n->m_pChild[1]; }
    }
public:
    BinaryTreeBackwardIterator(Container *pContainer, Node *pNode)
                      : Parent(pContainer, nullptr){
        if (pNode && pContainer && pNode == pContainer->m_pRoot) {
            pushRight(pNode);
            if (!m_stack.empty()) Parent::m_pNode = m_stack.top();
        } else {
            Parent::m_pNode = pNode; // para el rend()
        }
    }
    BinaryTreeBackwardIterator(const BinaryTreeBackwardIterator<Container> &another)
                      : Parent(another), m_stack(another.m_stack){}

    BinaryTreeBackwardIterator<Container> &operator++(){
        if(m_stack.empty()) { Parent::m_pNode = nullptr; return *this; }
        Node* curr = m_stack.top();
        m_stack.pop();
        pushRight(curr->m_pChild[0]);
        if (!m_stack.empty()) Parent::m_pNode = m_stack.top();
        else Parent::m_pNode = nullptr;
        return *this;
    }
};

// nodo
template <typename Traits>
class NodeBinaryTree{
public:
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;

    value_type m_data;
    ref_type   m_ref;
    Node *m_pChild[2]  = {nullptr, nullptr};
    int m_height       = 1; //altura del avl

    NodeBinaryTree(){}
    NodeBinaryTree( value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){}
        
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }
    
    ref_type    GetRef     () const { return m_ref; }
    ref_type   &GetRefRef  () { return m_ref; }
};

//arbol avl
template <typename Traits>
class CBinaryTree{
public:
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;
    
    using forward_iterator  = BinaryTreeForwardIterator<CBinaryTree<Traits>>;
    using backward_iterator = BinaryTreeBackwardIterator<CBinaryTree<Traits>>;

    friend forward_iterator;
    friend backward_iterator;
    friend struct GeneralIterator<CBinaryTree<Traits>>;

private:
    Node *m_pRoot       = nullptr;
    size_t m_nElements  = 0;
    CompareFunc comp;
    
    mutable mutex m_mtx;
    using LOCK = lock_guard<mutex>;

    //helper logica AVL
    auto GetHeight(Node *pNode) { return pNode ? pNode->m_height : 0; }

    auto GetBalanceFactor(Node *pNode) {
        if (!pNode) return 0;
        return GetHeight(pNode->m_pChild[0]) - GetHeight(pNode->m_pChild[1]);
    }

    //rotacion (0 = left, 1 = right)
    Node* Rotate(Node* root, int dir) {
        int otherDir    = 1 - dir;
        Node* newRoot   = root->m_pChild[otherDir];
        
        root->m_pChild[otherDir]    = newRoot->m_pChild[dir];
        newRoot->m_pChild[dir]      = root;
        
        root->m_height      = 1 + max(GetHeight(root->m_pChild[0]),    GetHeight(root->m_pChild[1]));
        newRoot->m_height   = 1 + max(GetHeight(newRoot->m_pChild[0]), GetHeight(newRoot->m_pChild[1]));
        
        return newRoot;
    }

    void Balance(Node *&rParent) {
        if (!rParent) return;
        
        rParent->m_height   = 1 + max(GetHeight(rParent->m_pChild[0]), GetHeight(rParent->m_pChild[1]));
        int balance         = GetBalanceFactor(rParent);

        //left heavy
        if (balance > 1) {
            if (GetBalanceFactor(rParent->m_pChild[0]) < 0)
                rParent->m_pChild[0] = Rotate(rParent->m_pChild[0], 0); //rotacion izquierda en el hijo
            rParent = Rotate(rParent, 1); //rotacion derecha en la raiz
        }
        //right heavy
        else if (balance < -1) {
            if (GetBalanceFactor(rParent->m_pChild[1]) > 0)
                rParent->m_pChild[1] = Rotate(rParent->m_pChild[1], 1); //rotacion derecha en el hijo
            rParent = Rotate(rParent, 0); //rotacion izquierda en la raiz
        }
    }

    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
        if( !rParent ){
            rParent = new Node(val, ref);
            ++m_nElements;
            return;
        }
        if (val == rParent->GetValue()) return; //no duplicados

        auto path = comp(val, rParent->GetValue()); 
        InternalInsert(rParent->m_pChild[path], val, ref);
        
        Balance(rParent); //autobalance tras insertar
    }

    Node* GetMinNode(Node* pNode) {
        while (pNode && pNode->m_pChild[0]) pNode = pNode->m_pChild[0];
        return pNode;
    }

    void InternalRemove(Node *&rParent, const value_type &val){
        if (!rParent) return;

        if (val == rParent->GetValue()) {
            if (!rParent->m_pChild[0] || !rParent->m_pChild[1]) {
                Node *temp      = rParent->m_pChild[0] ? rParent->m_pChild[0] : rParent->m_pChild[1];
                Node* pDelete   = rParent;
                rParent         = temp; //re-enlazamos
                delete pDelete;
                --m_nElements;
            } else {
                Node *temp              = GetMinNode(rParent->m_pChild[1]);
                rParent->GetValueRef()  = temp->GetValue();
                rParent->GetRefRef()    = temp->GetRef();
                InternalRemove(rParent->m_pChild[1], temp->GetValue());
            }
        } else {
            auto path = comp(val, rParent->GetValue());
            InternalRemove(rParent->m_pChild[path], val);
        }

        if (rParent) Balance(rParent); //autobalance tras remover
    }

    //constructor
    Node* Clone(Node* root) {
        if (!root) return nullptr;
        Node* newNode       = new Node(root->GetValue(), root->GetRef());
        newNode->m_height   = root->m_height;
        newNode->m_pChild[0] = Clone(root->m_pChild[0]);
        newNode->m_pChild[1] = Clone(root->m_pChild[1]);
        return newNode;
    }
    //destructor
    void Destroy(Node* root) {
        if (!root) return;
        Destroy(root->m_pChild[0]);
        Destroy(root->m_pChild[1]);
        delete root;
    }
    //recorridos
    template<typename Func, typename... Args>
    void InternalInOrder(Node* pNode, Func fn, Args... args) {
        if (!pNode) return;
        InternalInOrder(pNode->m_pChild[0], fn, args...);
        fn(pNode->GetValue(), args...);
        InternalInOrder(pNode->m_pChild[1], fn, args...);
    }

    template<typename Func, typename... Args>
    void InternalPreOrder(Node* pNode, Func fn, Args... args) {
        if (!pNode) return;
        fn(pNode->GetValue(), args...);
        InternalPreOrder(pNode->m_pChild[0], fn, args...);
        InternalPreOrder(pNode->m_pChild[1], fn, args...);
    }

    template<typename Func, typename... Args>
    void InternalPostOrder(Node* pNode, Func fn, Args... args) {
        if (!pNode) return;
        InternalPostOrder(pNode->m_pChild[0], fn, args...);
        InternalPostOrder(pNode->m_pChild[1], fn, args...);
        fn(pNode->GetValue(), args...);
    }

public:
    CBinaryTree(){}
    // contructor copia
    CBinaryTree(const CBinaryTree &another){
        LOCK lock(another.m_mtx);
        m_pRoot     = Clone(another.m_pRoot);
        m_nElements = another.m_nElements;
    }
    // move constructor
    CBinaryTree(CBinaryTree &&another) noexcept{
        LOCK lock(another.m_mtx);
        m_pRoot     = another.m_pRoot;
        m_nElements = another.m_nElements;
        another.m_pRoot     = nullptr;
        another.m_nElements = 0;
    }
    // destructor seguro
    virtual ~CBinaryTree(){
        Destroy(m_pRoot);
        m_pRoot = nullptr;
    }

    void Insert(const value_type &val, ref_type ref = -1){
        LOCK lock(m_mtx);
        InternalInsert(m_pRoot, val, ref);
    }

    void remove(const value_type &val){
        LOCK lock(m_mtx);
        InternalRemove(m_pRoot, val);
    }

    size_t size() const {
        LOCK lock(m_mtx);
        return m_nElements;
    }

    //iteradores
    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }
    
    backward_iterator rbegin() { return backward_iterator(this, m_pRoot); }
    backward_iterator rend()   { return backward_iterator(this, nullptr); }

    //recorridos con variadic
    template <typename Func, typename... Args>
    void InOrder(Func fn, Args... args) {
        LOCK lock(m_mtx);
        InternalInOrder(m_pRoot, fn, args...);
    }

    template <typename Func, typename... Args>
    void PreOrder(Func fn, Args... args) {
        LOCK lock(m_mtx);
        InternalPreOrder(m_pRoot, fn, args...);
    }

    template <typename Func, typename... Args>
    void PostOrder(Func fn, Args... args) {
        LOCK lock(m_mtx);
        InternalPostOrder(m_pRoot, fn, args...);
    }

    template <typename Func, typename... Args>
    void Foreach(Func fn, Args... args) {
        InOrder(fn, args...); //foreach suele ser in-order
    }

private:
    //persistencia write (in-order)
    friend ostream &operator<<(ostream &os, CBinaryTree<Traits> &container){
        LOCK lock(container.m_mtx);
        os << "CBinaryTree AVL: size = " << container.m_nElements << "\n[";
        size_t count    = 0;
        auto printNode  = [&](auto& self, Node* node) -> void {
            if (!node) return;
            self(self, node->m_pChild[0]);
            os << "(" << node->GetValue() << ":" << node->GetRef() << ")";
            if (++count < container.m_nElements) os << ", ";
            self(self, node->m_pChild[1]);
        };
        printNode(printNode, container.m_pRoot);
        os << "]\n";
        return os;
    }

    // persistencia read
    friend istream &operator>>(istream &is, CBinaryTree<Traits> &container) {
        T2 line;
        for (; getline(is, line); ) {
            if (!line.empty() && line[0] == '[') {
                for (size_t pos = 1; pos < line.length() && line[pos] != ']'; ++pos) {
                    if (line[pos] == '(') {
                        size_t _value = line.find(':', pos);
                        size_t _refer = line.find(')', pos);
                
                        typename Traits::value_type val;
                        ref_type ref;
                        
                        stringstream(line.substr(pos + 1, _value - pos - 1))       >> val;
                        stringstream(line.substr(_value + 1, _refer - _value - 1)) >> ref;
                        
                        container.Insert(val, ref);
                        pos = _refer;
                    }
                } break;
            }
        }
        return is;
    }
};

#endif // __BINARYTREE_H__