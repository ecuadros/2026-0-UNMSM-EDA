#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <mutex>
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
class NodeBinaryTree{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;
    friend class CBinaryTree<Traits>;
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
};

template <typename Traits>
class CBinaryTree{
public:
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;
protected:
    mutable mutex mtx;
private:
    Node *m_pRoot = nullptr;
    CompareFunc comp;

public:
    CBinaryTree(){}
    // TODO: Copy constructor
    CBinaryTree(CBinaryTree &another){

    }
    // TODO: Move constructor
    CBinaryTree(CBinaryTree &&another){

    }
    virtual ~CBinaryTree(){

    }
private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
        if( !rParent ){
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
public:
    void Insert(const value_type &val, ref_type ref){
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

};


#endif // __BINARYTREE_H__
