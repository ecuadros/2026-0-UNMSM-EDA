#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <functional>          // <-- necesario para std::greater / std::less
#include "../general/types.h"
#include "../util.h"
using namespace std;

// Traits para listas enlazadas
template <typename T, typename _Func>
struct ListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingTrait :
    public ListTrait<T, std::greater<T> >{
    // std::greater: insertamos antes si (actual > nuevo)  => orden ascendente
};

template <typename T>
struct DescendingTrait :
    public ListTrait<T, std::less<T> >{
    // std::less: insertamos antes si (actual < nuevo) => orden descendente
};

// Nodo para lista enlazada simple
template <typename Traits>
class NodeLinkedList{
public:
    using value_type = typename Traits::value_type;
    using Node       = NodeLinkedList<Traits>;

private:
    value_type m_data;
    ref_type   m_ref;
    Node      *m_pNext = nullptr;

public:
    NodeLinkedList(){}
    NodeLinkedList(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){}

    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef()       { return m_data; }

    ref_type    GetRef     () const { return m_ref; }
    ref_type   &GetRefRef  ()       { return m_ref; }

    Node       *GetNext    () const { return m_pNext; }
    Node      *&GetNextRef ()       { return m_pNext; }

    Node &operator=(const Node &another){
        m_data = another.GetValue();
        m_ref  = another.GetRef();
        return *this;
    }
    bool operator==(const Node &another) const
    { return m_data == another.GetValue(); }
    bool operator<(const Node &another) const
    { return m_data < another.GetValue(); }
};

template <typename Traits>
class CLinkedList {
public:
    using value_type = typename Traits::value_type;
    using Func       = typename Traits::Func;
    using Node       = NodeLinkedList<Traits>;

private:
    Node   *m_pRoot = nullptr;
    Node   *m_pLast = nullptr;
    size_t  m_nElements = 0;

public:
    CLinkedList(){}

    // (por ahora) básico para compilar y usar
    void push_back(const value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);

    size_t getSize() const { return m_nElements; }

private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);

    // Persistencia (write) - placeholder
    friend ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
        os << "CLinkedList: size = " << container.getSize() << endl;
        os << "[";
        // Aquí luego lo recorres con iterador o puntero.
        os << "]" << endl;
        return os;
    }
    // Persistencia (read) - TODO
};

template <typename Traits>
void CLinkedList<Traits>::push_back(const value_type &val, ref_type ref){
    Node *pNewNode = new Node(val, ref);

    if(!m_pRoot){
        m_pRoot = pNewNode;
        m_pLast = pNewNode;
        ++m_nElements;
        return;
    }

    // enlazar al final
    m_pLast->GetNextRef() = pNewNode;
    m_pLast = pNewNode;
    ++m_nElements;
}

template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    // Orden controlado por Traits::Func
    // AscendingTrait: std::greater -> si (actual > nuevo) insertamos antes
    Func comp;

    if(!rParent || comp(rParent->GetValue(), val)){
        Node *pNew = new Node(val, ref);
        pNew->GetNextRef() = rParent;
        rParent = pNew;

        // si insertamos en lista vacía, también actualizamos last
        if(!m_pLast) m_pLast = pNew;

        ++m_nElements;
        return;
    }

    InternalInsert(rParent->GetNextRef(), val, ref);

    // mantener m_pLast correcto (por si insertas al final)
    if(rParent && !rParent->GetNext()) m_pLast = rParent;
}

template <typename Traits>
void CLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    InternalInsert(m_pRoot, val, ref);

    // asegurar last si estaba vacío antes
    if(m_pRoot && !m_pLast){
        Node *p = m_pRoot;
        while(p->GetNext()) p = p->GetNext();
        m_pLast = p;
    }
}

#endif // __LINKEDLIST_H__
