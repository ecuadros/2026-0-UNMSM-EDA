#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include "../general/types.h"
#include <mutex>
#include <stdexcept>
#include "../util.h"
using namespace std;

// TODO: Traits para listas enlazadas
//traits de la lista

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
//Creacion del nodo de la lista
template <typename Traits>
class NodeLinkedList{
public:
    using  Node        = NodeLinkedList<Traits>;

    using  value_type  = typename Traits::value_type;
private:
    value_type m_data;
    ref_type   m_ref;
    Node       *m_pNext = nullptr;
    //Constructor
public:
    NodeLinkedList() : m_pNext(nullptr) {}

    NodeLinkedList( value_type _value, ref_type _ref = -1, Node* _next = nullptr) // la referencia esta apuntando a null)
        : m_data(_value), m_ref(_ref), m_pNext(_next){   }

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

    //Operadores de ordenamiento
    bool operator==(const Node &another) const
    { return m_data == another.GetValue();   }
    bool operator<(const Node &another) const
    { return m_data < another.GetValue();   }
};
//Forward Iterator
template<typename Node>
class forward_iterator{
public:
    using value_type =  typename Node::value_type;
    using pointer    =  value_type*;
    using reference  =  value_type&;
private:
    Node* m_pNode;
public:
    forward_iterator(Node* node)    :   m_pNode(node) {}
    //sobrecargamos los operadores
    bool operator!=(const forward_iterator& other)  const{  return m_pNode != other.m_pNode; }
    bool operator==(const forward_iterator& other)  const{  return m_pNode != other.m_pNode; }
    // (*it)
    reference operator*() { return m_pNode -> GetValueRef(); }
    //(++it)
    forward_iterator& operator++(){
        if(m_pNode) m_pNode = m_pNode->GetNext();
        return *this;
    }

};

//Lista Enlazada
template <typename Traits>
class CLinkedList {
public:
    using  value_type  = typename Traits::value_type;
    // using  forward_iterator  = LinkedListForwardIterator < CLinkedList<Traits> >;
    // friend forward_iterator;
    // friend GeneralIterator< CLinkedList<Traits> >;
    using  Node = NodeLinkedList<Traits>;
    using iterator = forward_iterator<Node>;

protected:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;    //push back rapido
    size_t m_nElements = 0;

    mutable std::mutex m_mutex;
public:
    //constructor
    CLinkedList() = default;
    //Constructor Copia
    CLinkedList(const CLinkedList& other){
        std::lock_guard<std::mutex> lock(other.m_mutex); //bloqueamos la lista de origen
        Node* curr = other.m_pRoot;
        while(curr){
            push_back(curr->GetValueRef(), curr->GetRef());
            curr = curr->GetNext();
        }
    }
    //Move Constructor
    //roba los punteros de la lista de origen
    CLinkedList(CLinkedList&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_pRoot = other.m_pRoot;
        m_pLast = other.m_pLast;
        m_nElements = other.m_nElements;

        //lista vacia pero funcional
        other.m_pRoot = nullptr;
        other.m_pLast = nullptr;
        other.m_nElements = 0;
    }
    //destructor
    //CLinkedList(){
    //    Node *pNode = m_pRoot;
    //    while(pNode != nullptr) {
    //        Node *temp =pNode;
    //        pNode = pNode->GetNext();
    //        delete temp;
    //    }
    //Destructor Seguro
    virtual ~CLinkedList(){
        clear();
    }

    //funcion auxiliar para limpiar memoria
    void clear(){
        std::lock_guard<std::mutex> lock(m_mutex);
        Node *pNode = m_pRoot;
        while(pNode != nullptr) {
            Node *temp =pNode;
            pNode = pNode->GetNext();
            delete temp;
    }
        m_pRoot = nullptr;
        m_pLast = nullptr;
        m_nElements = 0;
    }
    //iteradores begin() y end()
    //uso de forech y firstTaht
    iterator begin() {  return iterator(m_pRoot);  }
    iterator end()   {  return iterator(nullptr);  }

    //operadoes de acceso []
    value_type& operator[](size_t index){
        std::lock_guard<std::mutex> lock(m_mutex);
        if(index >= m_nElements)
            throw std::out_of_range("Indice fuera del rango");
        Node* curr = m_pRoot;
        for(size_t i = 0; i < index; ++i){
            curr = curr->GetNext();  }
        return curr -> GetValueRef();
    }

    // TODO: Constructor copia                          check
    // TODO: Move Constructor                           check
    // TODO: Destructor seguro y virtual                check
    // TODO: Concurrencia (mutex)                       check
    // TODO: Iterators begin() end()                    check
    // TODO: Operadores de acceso []

    void push_back(const value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);

    size_t getSize() const{
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_nElements;
    }
//funcion para invertir el orden sea ascendente o descendente -- mayor flexibilidad
    void PrintReverse(){
        std::lock_guard<std::mutex> lock(m_mutex);
        cout<<"[";
        InternalPrintReverse(m_pRoot);
        cout<<"]"<<endl; }
protected:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);

    // TODO: Persistencia (write)
    //operador ("<<")
    friend ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
        os << "CLinkedList: size = " << container.getSize() << endl;
        os << "[";
        Node* pNode =container.m_pRoot; //declaracion pNode
        while(pNode != nullptr){
            os<<pNode-> GetValue()<<" ";
            pNode = pNode->GetNext();
        //for (auto i = 0; i < container.getSize(); ++i){
            // os << "(" << arr.m_data[i].GetValue() << ":" << arr.m_data[i].GetRef() << "),";
        }
        os << "]" << endl;
        return os;
    }
    // TODO: Persistencia (read)
    //operador (">>")
    friend istream &operator>>(istream &is, CLinkedList<Traits> &container){
        value_type val;
        cout<<"ingrese valor para agregar a la lista ";
        is>>val;
        container.Insert(val,0); //Insertar al final de la lista; ordenadamente
        return is;
    }

    //recursividad para imprimir en orden inverso
    void InternalPrintReverse(Node* pNode){
        if(pNode == nullptr) return; //caso base -- se llega hasta el final de la lista
        InternalPrintReverse(pNode->GetNext()); //recursividad
        cout<<pNode->GetValue()<<" "; //imprime de atras para delante
    }
};
//Implementacion de los traits
template <typename Traits>
void CLinkedList<Traits>::push_back(const value_type &val, ref_type ref){
    Node *pNewNode = new Node(val, ref);
    if( !m_pRoot )
        m_pRoot = pNewNode;
    else
        m_pLast->GetNextRef() = pNewNode;
    m_pLast = pNewNode;
    ++m_nElements;
}

template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    // TODO: Agregar algo para el caso de circular              check
    if( !rParent || typename Traits::Func()(val, rParent->GetValue()) ){
        Node *pNew = new Node(val, ref, rParent);
        rParent = pNew;
        //para actualizar el m_pLast si es que insertamos al final o si la lista esta vacia
        if(pNew->GetNext() == nullptr)
            m_pLast = pNew;
        if(rParent)
        ++m_nElements;
        return;
    }
    InternalInsert(rParent->GetNextRef(), val, ref);

}

template <typename Traits>
void CLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mutex); //bloqueamos al incio de la operacion publica

    InternalInsert(m_pRoot, val, ref);
}

#endif // __LINKEDLIST_H__
