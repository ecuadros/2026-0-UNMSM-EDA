#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <mutex>
#include <assert.h>
#include "stack.h"
#include "../general/types.h"
#include "../util.h"
#include "../variadic-util.h"
#include "GeneralIterator.h"

using namespace std;

template <typename T> class NodeBinaryTree;

template <typename T>
struct TreeTraitAscending {
    using value_type  = T;
    using CompareFunc = std::greater<T>;
    using Node        = NodeBinaryTree<TreeTraitAscending<T>>;
};

template <typename T>
struct TreeTraitDescending {
    using value_type  = T;
    using CompareFunc = std::less<T>;
    using Node        = NodeBinaryTree<TreeTraitDescending<T>>;
};

template <typename Container>
class BTreeForwardIterator : public GeneralIterator<Container> { 
    using Parent = GeneralIterator<Container>;
    using Node = typename Container::Node;
    using value_type = typename Container::value_type;
    CStack<Node*> stack;

    void addLeft(Node *node) {
        while (node) {
            stack.push(node, -1);
            node = node->GetLeft();
        }
    }
public:
    BTreeForwardIterator(Container *pContainer, Node *m_pRoot, Size pos=0) : Parent(pContainer, pos, m_pRoot) {
        addLeft(m_pRoot);
        if (stack.empty()) Parent::m_data = nullptr;
        else Parent::m_data = stack.top();
    }
    BTreeForwardIterator(BTreeForwardIterator<Container> &another) : Parent(another), stack(another.stack) {}

    BTreeForwardIterator<Container> &operator++(){
        if (stack.empty()) {
            Parent::m_data = nullptr;
            return *this;
        }
        Node *temp = stack.pop();
        if (temp->GetRight()) addLeft(temp->GetRight());

        if (!stack.empty()) Parent::m_data = stack.top();
        else Parent::m_data = nullptr;
        
        ++Parent::m_pos;
        return *this;
    }
    value_type &operator*(){
        return Parent::m_data->GetValueRef();
    }
};

template <typename Container>
class BTreeBackwardIterator : public GeneralIterator<Container> { 
    using Parent     = GeneralIterator<Container>;
    using Node       = typename Container::Node;
    using value_type = typename Container::value_type;
    CStack<Node*> stack;

    void addRight(Node *node) {
        while (node) {
            stack.push(node, -1);
            node = node->GetRight();
        }
    }
public:
    BTreeBackwardIterator(Container *pContainer, Node *m_pRoot, Size pos = 0) : Parent(pContainer, pos, m_pRoot) {
        addRight(m_pRoot);
        if (stack.empty()) Parent::m_data = nullptr;
        else Parent::m_data = stack.top();
    }
    BTreeBackwardIterator(BTreeBackwardIterator<Container> &another)  :  Parent(another), stack(another.stack) {}

    BTreeBackwardIterator<Container> &operator++(){
        if (stack.empty()) {
            Parent::m_data = nullptr;
            return *this;
        }
        Node *temp = stack.pop();
        if (temp->GetLeft()) addRight(temp->GetLeft());

        if (!stack.empty()) Parent::m_data = stack.top();
        else Parent::m_data = nullptr;
        
        --Parent::m_pos;
        return *this;
    }
    value_type &operator*(){
        return Parent::m_data->GetValueRef();
    }
};

template <typename Traits>
class NodeBinaryTree{
    using  value_type  = typename Traits::value_type;
    using  Node        = typename Traits::Node;
    using  CompareFunc = typename Traits::CompareFunc;
protected:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pChild[2]  = {nullptr, nullptr};
public:
    NodeBinaryTree(){}
    NodeBinaryTree( value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){   }
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef()       { return m_data; }
    ref_type    GetRef     () const { return m_ref; }
    ref_type   &GetRefRef  ()       { return m_ref; }
    Node       *GetLeft    () const { return m_pChild[0]; }
    Node      *&GetLeftRef ()       { return m_pChild[0]; }
    Node       *GetRight   () const { return m_pChild[1]; }
    Node      *&GetRightRef()       { return m_pChild[1]; }
};

template <typename Traits>
class CBinaryTree{
public:
    using  value_type  = typename Traits::value_type;
    using  CompareFunc = typename Traits::CompareFunc;
    using  Node        = typename Traits::Node;
    using  forwarditerator = BTreeForwardIterator<CBinaryTree<Traits>>;
    using  backwardditerator = BTreeBackwardIterator<CBinaryTree<Traits>>;
protected:
    Node *m_pRoot = nullptr;
    CompareFunc comp;
    size_t n_Elements = 0;
    mutable mutex mtx;

public:
    // Normal constructor
    CBinaryTree(){}

    // Copy constructor
    CBinaryTree(const CBinaryTree &another){
        lock_guard<mutex> lock(another.mtx);
        // Necistamos insertar los m_data y m_ref de cada nodo del arbol de origen, la forma correcta es hacerlo en preorden
        CopyTree(another.m_pRoot);
    }
    // Move constructor
    CBinaryTree(CBinaryTree &&another) {
        lock_guard<mutex> lock(another.mtx);

        m_pRoot = another.m_pRoot;
        n_Elements = another.n_Elements;
        another.m_pRoot = nullptr;
        another.n_Elements = 0;

    }
    virtual ~CBinaryTree(){
        lock_guard<mutex> lock(mtx);
        Clear(m_pRoot);
    }

    size_t getSize() const { return n_Elements; }
protected:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);
    void InternalRemove(Node *&root, const value_type &value);
    void CopyTree(Node *root);
    void Clear(Node *Node);
    template <typename Func, typename... Args>
    void internalpreorden (Node *pNode, Func fn, Args... args);
    template <typename Func, typename... Args>
    void internalinorden  (Node *pNode, Func fn, Args... args);
    template <typename Func, typename... Args>
    void internalpostorden(Node *pNode, Func fn, Args... args);

public:
    void Insert(const value_type &val, ref_type ref) {
        lock_guard<mutex> lock(mtx);
        InternalInsert(m_pRoot, val, ref);
    }
    void Remove(const value_type &val) {
        lock_guard<mutex> lock(mtx);
        InternalRemove(m_pRoot, val);
    }

    template <typename Func, typename... Args>
    void preorden  (Func fn, Args... args) { 
        lock_guard<mutex> lock(mtx); 
        internalpreorden  (m_pRoot, fn, args...); 
    };
    template <typename Func, typename... Args>
    void inorden   (Func fn, Args... args) { 
        lock_guard<mutex> lock(mtx);
        internalinorden   (m_pRoot, fn, args...); 
    };
    template <typename Func, typename... Args>
    void postorden (Func fn, Args... args) { 
        lock_guard<mutex> lock(mtx);
        internalpostorden (m_pRoot, fn, args...); 
    };
    
    forwarditerator begin() 
    { return forwarditerator( this, m_pRoot, 0 ); }
    forwarditerator end() 
    { return forwarditerator( this, nullptr, getSize()); }
    backwardditerator rbegin() 
    { return backwardditerator( this, m_pRoot, getSize()-1 ); }
    backwardditerator rend() 
    { return backwardditerator( this, nullptr, -1 ); }

    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args);
    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args);

    // Usamos un lambda para una función sencilla que imprima en inorden
    friend ostream &operator<<(ostream &os, CBinaryTree &CTree) {
        // Estas funciones no capturan automáticamente el scope que las encierra, así que le pasamos 'os' dentro de []
        os << "Tamaño del CBinaryTree: " << CTree.getSize() << endl << endl;
        CTree.inorden( [&os]( auto &val ) { os << val << " "; } );
        return os;
    }
    friend istream &operator>>(istream &is, CBinaryTree &CTree) {
        value_type val;
        ref_type ref;

        is >> val >> ref;
        CTree.Insert( val, ref );
        return is;
    }
};

template <typename Traits>
template <typename Func, typename... Args>
void CBinaryTree<Traits>::internalpreorden(Node *pNode, Func fn, Args... args) {
    if (pNode) {
        fn(pNode->GetValueRef(), args...);
        internalpreorden(pNode->GetLeft(), fn, args...);
        internalpreorden(pNode->GetRight(), fn, args...);
    }
}

template <typename Traits>
template <typename Func, typename... Args>
void CBinaryTree<Traits>::internalinorden(Node *pNode, Func fn, Args... args) {
    if (pNode) {
        internalinorden(pNode->GetLeft(), fn, args...);
        fn(pNode->GetValueRef(), args...);
        internalinorden(pNode->GetRight(), fn, args...);
    }
}

template <typename Traits>
template <typename Func, typename... Args>
void CBinaryTree<Traits>::internalpostorden(Node *pNode, Func fn, Args... args) {
    if (pNode) {
        internalpostorden(pNode->GetLeft(), fn, args...);
        internalpostorden(pNode->GetRight(), fn, args...);
        fn(pNode->GetValueRef(), args...);
    }
}

template <typename Traits>
void CBinaryTree<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    if( !rParent ){
        rParent = new Node(val, ref);
        ++n_Elements;
        return;
    }
    auto path = comp(val, rParent->GetValue());
    InternalInsert(path? rParent->GetRightRef():rParent->GetLeftRef(), val, ref);
}

template <typename Traits>
void CBinaryTree<Traits>::InternalRemove(Node *&rParent, const value_type &value) {
    assert(n_Elements > 0);

    if (!rParent) return;

    if ( value != rParent->GetValue() ) {
        auto path = comp(value, rParent->GetValue());
        InternalRemove(path ? rParent->GetRightRef() : rParent->GetLeftRef(), value);
        return;
    }

    Node* temp = rParent;
    if (!rParent->GetLeft() && !rParent->GetRight()) rParent = nullptr;
    else if (!rParent->GetLeft())  rParent = rParent->GetRight();
    else if (!rParent->GetRight()) rParent = rParent->GetLeft();
    else {  // En este caso optamos por el reemplazar por el predecesor del dato a eliminar
        Node* parent = rParent;
        temp = rParent->GetLeft();
        while ( temp->GetRight() ) {
            parent = temp;
            temp = temp->GetRight();
        }
        rParent->GetValueRef() = temp->GetValue();
        rParent->GetRefRef() = temp->GetRef();
        
        // Actualizamos el enlace antes del nodo anterior al predecesor del nodo a eliminar
        (parent == rParent ? parent->GetLeftRef() : parent->GetRightRef()) = temp->GetLeft();
    }
    delete temp;
    --n_Elements;
}

// Una función que cumple el rol de recorrido preorden, utilizada solo para copiar también los ref de otro arbol.
// No se usa el "internalpreorden" pues se reserva para casos más genéricos donde solo se usa el dato del nodo.
template <typename Traits>
void CBinaryTree<Traits>::CopyTree(Node* root) {
    if (!root) return;

    InternalInsert(m_pRoot, root->GetValue(), root->GetRef());
    CopyTree(root->GetLeft());
    CopyTree(root->GetRight());
}

template <typename Traits>
void CBinaryTree<Traits>::Clear(Node *root) {
    if (!root) return;

    Clear( root->GetLeft() );
    Clear( root->GetRight() );
    delete root;
}

template <typename Traits>
template <typename ObjFunc, typename ...Args>
void CBinaryTree<Traits>::Foreach(ObjFunc of, Args... args) {
    ::Foreach(*this, of, args...);
}

template <typename Traits>
template <typename ObjFunc, typename ...Args>
auto CBinaryTree<Traits>::FirstThat(ObjFunc of, Args... args) {
    return ::FirstThat(*this, of, args...);
}

void DemoBinaryTree();

#endif // __BINARYTREE_H__