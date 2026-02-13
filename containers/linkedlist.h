#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include <mutex>
#include <fstream>
#include <utility>
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
};

template <typename T>
struct DescendingTrait : 
    public ListTrait<T, std::less<T> >{
};

// Nodo para listas enlazadas
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

// Forward Iterator para listas enlazadas
template <typename Container>
class LinkedListForwardIterator{
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;
    
private:
    Node *m_pCurrent = nullptr;
    
public:
    LinkedListForwardIterator(Node *pNode) : m_pCurrent(pNode) {}
    
    bool operator!=(const LinkedListForwardIterator &other) const {
        return m_pCurrent != other.m_pCurrent;
    }
    
    LinkedListForwardIterator &operator++() {
        if (m_pCurrent) {
            m_pCurrent = m_pCurrent->GetNext(); 
        }
        return *this;
    }
    
    value_type &operator*() {
        return m_pCurrent->GetValueRef();
    }  
};

// Clase LinkedList
template <typename Traits>
class CLinkedList {
public:
    using  value_type        = typename Traits::value_type;
    using  forward_iterator  = LinkedListForwardIterator < CLinkedList<Traits> >;
    friend forward_iterator;
    using  Node = NodeLinkedList<Traits>;

private:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable std::mutex mtx;  // Para concurrencia

public:
    // Constructor por defecto
    CLinkedList(){}
    
    // Constructor copia
    CLinkedList(const CLinkedList<Traits> &other) {
        std::lock_guard<std::mutex> lock(other.mtx);
        
        m_pRoot = nullptr;
        m_pLast = nullptr;
        m_nElements = 0;
        
        Node *pCurrent = other.m_pRoot;
        while (pCurrent != nullptr) {
            push_back(pCurrent->GetValue(), pCurrent->GetRef());
            pCurrent = pCurrent->GetNext();
        }
    }
    
    // Move Constructor
    CLinkedList(CLinkedList<Traits> &&other) noexcept 
        : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0) {
        std::lock_guard<std::mutex> lock(other.mtx);
        
        m_pRoot = std::exchange(other.m_pRoot, nullptr);
        m_pLast = std::exchange(other.m_pLast, nullptr);
        m_nElements = std::exchange(other.m_nElements, 0);
    }
    
    // Destructor seguro y virtual
    virtual ~CLinkedList() {
        std::lock_guard<std::mutex> lock(mtx);
        
        Node *pCurrent = m_pRoot;
        while (pCurrent) {
            Node *pNext = pCurrent->GetNext(); 
            delete pCurrent;                   
            pCurrent = pNext;                  
        }
        m_pRoot = nullptr;
        m_pLast = nullptr;
        m_nElements = 0;
    }

    // Operator[] (acceso por índice)
    value_type &operator[](Size index) {
        std::lock_guard<std::mutex> lock(mtx);
        
        if (index < 0 || index >= static_cast<Size>(m_nElements)) {
            throw std::out_of_range("Index out of range");
        }
        
        Node *pCurrent = m_pRoot;
        for (Size i = 0; i < index; ++i) {
            pCurrent = pCurrent->GetNext();
        }
        
        return pCurrent->GetValueRef();
    }

    // Iteradores
    forward_iterator begin() {
        return forward_iterator(m_pRoot); 
    }

    forward_iterator end() {
        return forward_iterator(nullptr); 
    }

    // Push back
    void push_back(const value_type &val, ref_type ref){
        std::lock_guard<std::mutex> lock(mtx);
        
        Node *pNewNode = new Node(val, ref);
        
        if( !m_pRoot ) {
            m_pRoot = pNewNode;
            m_pLast = pNewNode;
        } else {
            m_pLast->GetNextRef() = pNewNode;
            m_pLast = pNewNode;
        }
        ++m_nElements;
    }
    
    // Insert ordenado
    void Insert(const value_type &val, ref_type ref){
        std::lock_guard<std::mutex> lock(mtx);
        InternalInsert(m_pRoot, val, ref);
    }
    
    size_t getSize() const { 
        std::lock_guard<std::mutex> lock(mtx);
        return m_nElements;  
    }

    // Foreach
    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        std::lock_guard<std::mutex> lock(mtx);
        ::Foreach(*this, of, args...);
    }
    
    // FirstThat
    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args){
        std::lock_guard<std::mutex> lock(mtx);
        return ::FirstThat(*this, of, args...);
    }

    // Operator<< (salida/imprimir)
    friend ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
        std::lock_guard<std::mutex> lock(container.mtx);
        
        os << "CLinkedList: size = " << container.m_nElements << endl;
        os << "[";
        Node *pCurrent = container.m_pRoot;
        while (pCurrent != nullptr) {
            os << "(" << pCurrent->GetValue() << ":" << pCurrent->GetRef() << ")";
            if (pCurrent->GetNext() != nullptr) {
                os << ", ";
            }
            pCurrent = pCurrent->GetNext();
        }
        os << "]" << endl;
        return os;
    }
    
    // Operator>> (entrada/rellenar)
    friend istream &operator>>(istream &is, CLinkedList<Traits> &container){
        std::lock_guard<std::mutex> lock(container.mtx);
        
        value_type val;
        ref_type ref;
        
        while (is >> val >> ref) {
            container.InternalInsert(container.m_pRoot, val, ref);
        }
        
        return is;
    }
    
    // Persistencia - Guardar en archivo
    void SaveToFile(const string &filename) {
        std::lock_guard<std::mutex> lock(mtx);
        
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            throw std::runtime_error("Cannot open file for writing");
        }
        
        outFile << m_nElements << endl;
        Node *pCurrent = m_pRoot;
        while (pCurrent != nullptr) {
            outFile << pCurrent->GetValue() << " " << pCurrent->GetRef() << endl;
            pCurrent = pCurrent->GetNext();
        }
        
        outFile.close();
    }
    
    // Persistencia - Leer desde archivo
    void LoadFromFile(const string &filename) {
        std::lock_guard<std::mutex> lock(mtx);
        
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            throw std::runtime_error("Cannot open file for reading");
        }
        
        // Limpiar la lista actual
        Node *pCurrent = m_pRoot;
        while (pCurrent) {
            Node *pNext = pCurrent->GetNext();
            delete pCurrent;
            pCurrent = pNext;
        }
        m_pRoot = nullptr;
        m_pLast = nullptr;
        m_nElements = 0;
        
        // Leer el tamaño
        size_t size;
        inFile >> size;
        
        // Leer elementos
        value_type val;
        ref_type ref;
        while (inFile >> val >> ref) {
            Node *pNewNode = new Node(val, ref);
            
            if (!m_pRoot) {
                m_pRoot = pNewNode;
                m_pLast = pNewNode;
            } else {
                m_pLast->GetNextRef() = pNewNode;
                m_pLast = pNewNode;
            }
            ++m_nElements;
        }
        
        inFile.close();
    }

private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
        // Usa el comparador del Trait (Func) en lugar de hardcodear '>'
        if( !rParent || typename Traits::Func()(rParent->GetValue(), val) ){
            Node *pNew = new Node(val, ref);
            pNew->GetNextRef() = rParent;
            rParent = pNew;
            
            if (!pNew->GetNext()) {
                m_pLast = pNew;
            }
            
            ++m_nElements;
            return;
        }
        InternalInsert(rParent->GetNextRef(), val, ref);
    }
};

#endif // __LINKEDLIST_H__
