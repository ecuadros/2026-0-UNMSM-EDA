#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include "../general/types.h"
#include "GeneralIterator.h"
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


template <typename Container>
class LinkedListForwardIterator : public GeneralIterator<Container> {
public:
    using Parent     = GeneralIterator<Container>;
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

    // como el operador * originalmente regresa un nodo con el operador []
    // itera sobre todos los nodos cada vez que se llama
    // para optimizar este proceso, se crea el puntero a nodo pCurrent
    Node *pCurrent = nullptr;

    LinkedListForwardIterator(Container *pContainer, Size pos=0)
        : GeneralIterator<Container>(pContainer, pos), pCurrent(pContainer->m_pRoot) {
        // desplazarse a la posicion
        for (Size i = 0; i < pos; ++i) pCurrent = pCurrent->GetNext();
    }
    LinkedListForwardIterator(LinkedListForwardIterator<Container> &another)
        : GeneralIterator<Container>(another), pCurrent(another.pCurrent) {}

    // se sobrecarga el operador * y ++
    value_type &operator*() override { return pCurrent->GetValueRef(); }
    LinkedListForwardIterator<Container> &operator++() {
        if (pCurrent) {
            pCurrent = pCurrent->GetNext();
            ++this->m_pos;
        }
        return *this;
    }

    LinkedListForwardIterator<Container> operator++(int) {
        LinkedListForwardIterator<Container> tmp(*this);
        ++(*this);
        return tmp;
    }
};


template <typename Traits>
class CLinkedList {
public:
    using value_type       = typename Traits::value_type;
    using forward_iterator = LinkedListForwardIterator<CLinkedList<Traits>>;

    friend forward_iterator;
    friend GeneralIterator<CLinkedList<Traits>>;
    using Node = NodeLinkedList<Traits>;

    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    CLinkedList(){}
    // TODO: Constructor copia
    CLinkedList(const CLinkedList &to_copy): m_pRoot(nullptr), m_pLast(nullptr) {
        // usa la helper function implementada
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
    forward_iterator begin() { return forward_iterator(this); }
    forward_iterator end()   { return forward_iterator(this, m_nElements); }

    // TODO: Operadores de acceso [] done
    value_type &operator[](size_t index);

    void push_back(const value_type &val, ref_type ref);

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
    // lee el mismo formato en que se escribe
    friend istream &operator>>(istream &is, CLinkedList<Traits> &container) {
        // verificar el buen estado del stream
        if (!is) return is;

        try {
            // crea un contenedor temporal
            CLinkedList<Traits> tmp;

            // ignorar texto hasta el primer '['
            string bar;
            getline(is, bar, '[');

            // leer elementos continuamente
            char ch;
            while (is.get(ch) && ch != ']') {
                if (ch != '(') {
                    is.putback(ch);
                    continue;
                }
                value_type val;
                ref_type ref;
                string separator;

                // leer el valor
                is >> val;
                getline(is, separator, ':');
                // leer la ref
                is >> ref;
                getline(is, separator, ')');

                tmp.push_back(val, ref);
            }
            // si la lectura salio bien, intercambia los contenidos
            container = std::move(tmp);
        } catch (const exception& e) {
            // si algo paso, setea el estado del stream en failbit
            // (hubo fallo al leer el contenido)
            is.setstate(ios::failbit);
        }

        return is;
    }

    CLinkedList &operator=(const CLinkedList &to_copy) {
        _copyNodesFrom(to_copy);
        return *this;
    }

    /*
     * Helper function
     * copia los nodos de otra linked list
     */
    void _copyNodesFrom(const CLinkedList &to_copy) {
        // si se asigna a si mismo se regresa a si mismo
        if (this == &to_copy) return;

        // llama al destructor para eliminar los nodos
        this->~CLinkedList();

        Node* trav = to_copy.m_pRoot;
        while (trav) {
            value_type val = trav->GetValue();
            ref_type   ref = trav->GetRef();
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
void CLinkedList<Traits>::push_back(const value_type &val, ref_type ref) {
    typename Traits::Func compareFunc;
    // si el valor a añadir no sigue el orden (ascendente/descendente)
    // si es ascendente:  val > m_pLast->GetValue()
    // si es descendente: val < m_pLast->GetValue()
    if ( m_pLast && compareFunc(m_pLast->GetValueRef(), val) ) {
        InternalInsert(m_pRoot, val, ref);
        return;
    }

    Node *pNewNode = new Node(val, ref);
    if ( !m_pRoot ) m_pRoot = m_pLast = pNewNode;
    else {
        m_pLast->GetNextRef() = pNewNode;
        m_pLast = pNewNode;
    }
    ++m_nElements;
}


template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rCurrentNode, const value_type &val, ref_type ref) {
    typename Traits::Func compareFunc;
    // TODO: Agregar algo para el caso de circular
    // crea un nuevo nodo
    Node *pNew = new Node(val, ref);

    // caso ultimo nodo
    if ( !rCurrentNode ) {
        rCurrentNode = pNew;
        m_pLast = pNew;
        ++m_nElements;
        return;
    }
    // caso base, el valor debe insertarse antes del nodo actual
    if ( compareFunc(rCurrentNode->GetValueRef(), val ) ) {
        pNew->GetNextRef() = rCurrentNode;
        rCurrentNode = pNew;
        ++m_nElements;
        return;
    }
    /*
    if ( !rCurrentNode || compareFunc(val, rCurrentNode->GetValue()) ) {
        // reemplaza rCurrent node por pNew
        rCurrentNode = pNew;
        ++m_nElements;
        return;
    }
    */
    InternalInsert(rCurrentNode->GetNextRef(), val, ref);
}

template <typename Traits>
void CLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    if (!m_nElements) push_back(val, ref);
    else InternalInsert(m_pRoot, val, ref);
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
