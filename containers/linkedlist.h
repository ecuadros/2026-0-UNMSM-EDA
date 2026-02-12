#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include "../general/types.h"
#include "../util.h"
#include <mutex> //Para poder usar mutex en concurrencia
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

    //Node *m_pRoot = nullptr;
    Node *m_pHat; //para poder hacer la lista circular y evitar casos especiales en insercion/eliminacion
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    std::mutex m_mutex; // Mutex para control de concurrencia
public:
    CLinkedList(){
        m_pHat= new Node();
        m_pHat->GetNextRef() = m_pHat; // Lista circular
        m_pLast = m_pHat;
        m_nElements = 0;
    };

    // TODO: Constructor copia
    CLinkedList(const CLinkedList &other);

    // TODO: Move Constructor
    CLinkedList(CLinkedList &&other);

    // TODO: Destructor seguro y virtual
    virtual ~CLinkedList();

    // TODO: Concurrencia (mutex)

    // TODO: Iterators begin() end()
    Node* begin(){
        return m_pHat->GetNext();
    };
    Node* end(){
        return m_pHat;
    };

    // TODO: Operadores de acceso []
    value_type &operator[](size_t index);
    const value_type &operator[](size_t index) const;

    //Foreach
    template <typename Func>
    void Foreach(Func f){
        Node* curr=m_pHat->GetNext();  
        while (curr!=m_pHat){  
            f(curr->GetValueRef());  
            curr=curr->GetNext();  
        }
    }

    void push_back(value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    size_t getSize() const { return m_nElements;  }

    // TODO: Eliminar por valor, por referencia, por posición
    void pop_back();

    // TODO: Métodos específicos para stack
    void push_front(value_type &val);
    void pop_front();
    value_type get_front() const;

private:
    //void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);

    // TODO: Persistencia (write)
    friend ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
        os << "CLinkedList: size = " << container.getSize() << endl;
        os << "[";
        Node *pCurrent = container.m_pHat->GetNext();
        while (pCurrent != container.m_pHat){
            os << "(" << pCurrent->GetValue() << ":" << pCurrent->GetRef() << ")";
            pCurrent = pCurrent->GetNext();
            if(pCurrent != container.m_pHat) os << " , ";

        }
        os << "]" << endl;
        return os;
    }

    // TODO: Persistencia (read)
    friend istream &operator>>(istream &is, CLinkedList<Traits> &container){
        value_type val;
        ref_type   ref;
        char ch;
        while (is >> ch){
            if (ch == '('){
                is >> val;
                is >> ch; // :
                is >> ref;
                is >> ch; // )
                container.push_back(val, ref);
            }
        }
        return is;
    }

};

//Constructor copia
template <typename Traits>
CLinkedList<Traits>::CLinkedList(const CLinkedList &other){
    m_pHat = new Node();
    m_pHat->GetNextRef() = m_pHat; // Lista circular
    m_pLast = m_pHat;
    m_nElements = 0;

    Node *pCurrent = other.m_pHat->GetNext();
    while (pCurrent != other.m_pHat){
        value_type val = pCurrent->GetValue();
        ref_type   ref = pCurrent->GetRef();
        push_back(val, ref);
        pCurrent = pCurrent->GetNext();
    }
}

//Move Constructor
template <typename Traits>
CLinkedList<Traits>::CLinkedList(CLinkedList &&other){
    m_pHat = other.m_pHat;
    m_pLast = other.m_pLast;
    m_nElements = other.m_nElements;

    other.m_pHat = nullptr;
    other.m_pLast = nullptr;
    other.m_nElements = 0;
}

//Destructor virtual
template <typename Traits>
CLinkedList<Traits>::~CLinkedList(){
    std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la ejecucion de esta funcion
    Node* curr = m_pHat->GetNext();
    while (curr != m_pHat){
        Node* tmp = curr;
        curr = curr->GetNext();
        delete tmp;
    }
    delete m_pHat;
}

//Operador de acceso [] sin const
template <typename Traits>
typename CLinkedList<Traits>::value_type &
CLinkedList<Traits>::operator[](size_t index){
    assert(index < m_nElements);

    Node *pCurrent = m_pHat->GetNext();
    for (size_t i = 0; i < index; ++i){
        pCurrent = pCurrent->GetNext();
    }
    return pCurrent->GetValueRef();
}



//Operador de acceso con const []
template <typename Traits>
const typename CLinkedList<Traits>::value_type &CLinkedList<Traits>::operator[](size_t index) const {
    assert (index < m_nElements);

    Node *pCurrent = m_pHat->GetNext();
    for (size_t i = 0; i < index; ++i){
        pCurrent = pCurrent->GetNext();
    }
    return pCurrent->GetValueRef();
}



template <typename Traits>
void CLinkedList<Traits>::push_back(value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mutex); 
    Node *pNew = new Node(val, ref);
    pNew->GetNextRef() = m_pHat; // Apunta al hat
    m_pLast->GetNextRef() = pNew; // El ultimo apunta al nuevo
    m_pLast = pNew; // Actualiza el ultimo
    ++m_nElements;
}

template <typename Traits>
void CLinkedList<Traits>::pop_back(){
    std::lock_guard<std::mutex> lock(m_mutex); 
    assert(m_nElements > 0);

    if(m_nElements == 1){
        delete m_pLast;
        m_pHat->GetNextRef() = m_pHat; // Lista vacía
        m_pLast = m_pHat;
    } else {
        Node *prev = m_pHat;
        while (prev->GetNext() != m_pLast){
            prev = prev->GetNext();
        }

        prev->GetNextRef() = m_pHat; // El nuevo ultimo apunta al hat
        delete m_pLast; // Elimina el antiguo ultimo
        m_pLast = prev; // Actualiza el ultimo
    }

    --m_nElements;

}

// Métodos específicos para stack
template <typename Traits>
void CLinkedList<Traits>::push_front(value_type &val){
    std::lock_guard<std::mutex> lock(m_mutex); 

    Node *pNew = new Node(val);
    pNew->GetNextRef() = m_pHat->GetNext(); // El nuevo apunta al antiguo primero
    m_pHat->GetNextRef() = pNew; // El hat apunta al nuevo

    if(m_nElements == 0) // Si la lista estaba vacía, el nuevo es también el ultimo
        m_pLast = pNew;

    ++m_nElements;
}

template <typename Traits>
void CLinkedList<Traits>::pop_front(){
    std::lock_guard<std::mutex> lock(m_mutex); 
    assert(m_nElements > 0);

    Node *pDelete = m_pHat->GetNext();

    if(m_nElements == 1){
        m_pHat->GetNextRef() = m_pHat; // Lista vacía
        m_pLast = m_pHat;
    } else {
        m_pHat->GetNextRef() = pDelete->GetNext(); // El hat apunta al nuevo primero
    }

    delete pDelete; // Elimina el antiguo primero
    --m_nElements;
}

template <typename Traits>
typename CLinkedList<Traits>::value_type CLinkedList<Traits>::get_front() const {
    assert(m_nElements > 0);
    return m_pHat->GetNext()->GetValue(); // Retorna el valor del primer nodo (tope del stack)
}



//Como es lista circular, se usa Insert iterativo enves de un InternalInsert recursivo
/*template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    // TODO: Agregar algo para el caso de circular
    if( !rParent || rParent->m_data > val ){
        Node *pNew = new Node(val, ref, rParent);
        rParent = pNew;
        ++m_nElements;
        return;
    }
    InternalInsert(rParent->GetNextRef(), val, ref);
}*/

template <typename Traits>
void CLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mutex); 
    //InternalInsert(m_pHat, val, ref);
    Node *pPrev = m_pHat;
    Node *pCurrent = m_pHat->GetNext();
    
    while(pCurrent != m_pHat && pCurrent->GetValue() < val){
        pPrev = pCurrent;
        pCurrent = pCurrent->GetNext();
    }

    Node *pNew = new Node(val, ref);
    pNew->GetNextRef() = pCurrent;
    pPrev->GetNextRef() = pNew;

    if( pCurrent == m_pHat) // Si se inserta al final, actualizar el ultimo
        m_pLast = pNew;

    ++m_nElements;
} 

#endif // __LINKEDLIST_H__
