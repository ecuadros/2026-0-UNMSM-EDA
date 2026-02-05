#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include "../general/types.h"
#include "../util.h"
using namespace std;

// TODO: Traits para listas enlazadas
template <typename T, typename _Func>
struct ListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingTrait : 
    public ListTrait<T, std::greater<T> >{
};

template <typename T>
struct DescendingTrait : 
    public ListTrait<T, std::less<T> >{
};

// Iterators para listas enlazadas

template <typename Traits>
class NodeLinkedList{

    using  value_type  = typename Traits::value_type;
    using  Node        = NodeLinkedList<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;

public:
    NodeLinkedList(){}
    NodeLinkedList( value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){   }
    NodeLinkedList( value_type _value, ref_type _ref, Node *pNext)
        : m_data(_value), m_ref(_ref), m_pNext(pNext){   }
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }

    ref_type    GetRef     () const { return m_ref;   }
    ref_type   &GetRefRef  () { return m_ref;   }

    Node      * GetNext     () const { return m_pNext;   }
    Node      *&GetNextRef  () { return m_pNext;   }

    Node &operator=(const Node &another){
        m_data = another.GetValue();
        m_ref   = another.GetRef();
        return *this;
    }
    bool operator==(const Node &another) const
    { return m_data == another.GetValue();   }
    bool operator<(const Node &another) const
    { return m_data < another.GetValue();   }
};

template <typename Traits>
class CLinkedList {
    using  value_type  = typename Traits::value_type;
    // using  forward_iterator  = LinkedListForwardIterator < CLinkedList<Traits> >;
    // friend forward_iterator;
    // friend GeneralIterator< CLinkedList<Traits> >;
    using  Node = NodeLinkedList<Traits>;

    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
public:
    CLinkedList(){}
    // TODO: Constructor copia
    CLinkedList(const CLinkedList &to_copy): m_pRoot(nullptr), m_pLast(nullptr) {
        // si se asigna a si mismo se regresa a si mismo
        if (this == &to_copy) return *this;

        // llama al destructor para eliminar nodos
        // que la lista a asignarse podria tener
        this->~CLinkedList();
        // copia los nodos
        _copyNodesFrom(to_copy);
    }
    // TODO: Move Constructor
    CLinkedList(CLinkedList &&to_move)
    : m_pRoot(to_move.m_pRoot), m_pLast(to_move.m_pLast), m_nElements(to_move.m_nElements) {
        // solo mueve los punteros y el numero de elementos
        // reinicia los de la lista a mover
        to_move.m_pRoot = nullptr;
        to_move.m_pLast = nullptr;
        to_move.m_nElements = 0;
    }


    // TODO: Destructor seguro y virtual
    virtual ~CLinkedList();
    // TODO: Concurrencia (mutex)
    // TODO: Iterators begin() end()
    // TODO: Operadores de acceso [] done
    value_type &operator[](size_t index);

    void push_back(value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    size_t getSize(){ return m_nElements;  }
private:
    void InternalInsert(Node *&rCurrentNode, const value_type &val, ref_type ref);

    // TODO: Persistencia (write)
    friend ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
        os << "CLinkedList: size = " << container.getSize() << endl;
        os << "[";
        // cambio: variable del loop es un traveler del que se extrae el value y el Ref
        for (auto trav = container.m_pRoot; trav; trav = trav->GetNext()) {
            os << "(" << trav->GetValue() << ":" << trav->GetRef() << "),";
        }
        os << "]" << endl;
        return os;
    }
    // TODO: Persistencia (read)
    friend istream &operator>>(istream &is, CLinkedList<Traits> &container) {
        // hay que manejar el estilo en que se escribe
        return is;
    }

    CLinkedList &operator=(const CLinkedList &other) {
        if (this == &other) return *this;
        this->~CLinkedList();
        // usa el helper para ahorrar codigo
        _copyNodesFrom(other);
        return *this;
    }

    /*
     * Helper function
     * copia los nodos de otra linked list
     */
    void _copyNodesFrom(const CLinkedList &other) {
        // ahorrate el trabajo si es el mismo LL
        if (this == &other) return;
        // nodo raiz del otro linked list
        Node* trav = other.m_pRoot;
        while (trav) {
            value_type val = trav->GetValue();
            ref_type ref = trav->GetRef();
            push_back(val, ref);
            trav = trav->GetNext();
        }
    }

    /*
     * Helper function
     * limpia la lista
     */
    void clear() {
        this->~CLinkedList();
        m_pRoot = m_pLast = nullptr;
        m_nElements = 0;
    }
};

// destructor implementado
template <typename Traits>
CLinkedList<Traits>::~CLinkedList() {
    auto trav = m_pRoot;
    while (trav) {
        auto temp = trav->GetNext();
        delete trav;
        trav = temp;
    }
    m_pRoot = m_pLast = nullptr;
}

template <typename Traits>
void CLinkedList<Traits>::push_back(value_type &val, ref_type ref){
    Node *pNewNode = new Node(val, ref);
    if( !m_pRoot )
        m_pRoot = pNewNode;
    m_pLast = pNewNode;
    ++m_nElements;
}

template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rCurrentNode, const value_type &val, ref_type ref){
    // TODO: Agregar algo para el caso de circular
    if( !rCurrentNode || rCurrentNode->GetValue() > val ){
        Node *pNew = new Node(val, ref, rCurrentNode);
        rCurrentNode = pNew;
        ++m_nElements;
        return;
    }
    InternalInsert(rCurrentNode->GetNextRef(), val, ref);
}

template <typename Traits>
void CLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    InternalInsert(m_pRoot, val, ref);
}

// implementado operador []
template <typename Traits>
typename CLinkedList<Traits>::value_type &
CLinkedList<Traits>::operator[](const size_t index) {
    if (index >= m_nElements) {
        throw std::out_of_range("Index out of range");
    }
    Node *trav = m_pRoot;
    for (size_t i = 0; i < index; ++i)
        trav = trav->GetNext();
    return trav->GetValueRef();
}

#endif // __LINKEDLIST_H__
