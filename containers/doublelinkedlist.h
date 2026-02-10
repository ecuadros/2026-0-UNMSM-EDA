#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__

#include <iostream>
#include <mutex>
#include <stdexcept> 
#include "../general/types.h"
#include "../util.h"

using namespace std;


// TRAITS 
template <typename T, typename _Func>
struct DoubleListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct DoubleAscendingTrait : public DoubleListTrait<T, greater<T>> {};

template <typename T>
struct DoubleDescendingTrait : public DoubleListTrait<T, less<T>> {};

// Forward declarations
template <typename Traits> class NodeDoubleLinkedList;
template <typename Traits> class CDoubleLinkedList;
template <typename Traits> class DoubleLinkedListForwardIterator;
template <typename Traits> class DoubleLinkedListBackwardIterator;

// NODO DOBLE ENLAZADO
template <typename Traits>
class NodeDoubleLinkedList{
    template <typename T> friend class CDoubleLinkedList;
    template <typename T> friend class DoubleLinkedListForwardIterator;
    template <typename T> friend class DoubleLinkedListBackwardIterator;
    
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeDoubleLinkedList<Traits>;

private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;  
    Node *m_pPrev = nullptr;  

public:
    NodeDoubleLinkedList() {}
    NodeDoubleLinkedList(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref) {}
    
    // Constructor completo (muy util para enlazar rapido)
    NodeDoubleLinkedList(value_type _value, ref_type _ref, Node *pNext, Node *pPrev = nullptr)
        : m_data(_value), m_ref(_ref), m_pNext(pNext), m_pPrev(pPrev) {}
        
    value_type  GetValue() const { return m_data; }
    value_type &GetValueRef()    { return m_data; }
    ref_type    GetRef() const   { return m_ref; }
    
    Node *GetNext() const { return m_pNext; }
    Node *GetPrev() const { return m_pPrev; }
    
    // Setters para facilitar manipulacion
    void SetNext(Node *pNext) { m_pNext = pNext; }
    void SetPrev(Node *pPrev) { m_pPrev = pPrev; }
};

// FORWARD ITERATOR (Begin -> End)

template <typename Traits>
class DoubleLinkedListForwardIterator {
    using Node = NodeDoubleLinkedList<Traits>;
    using value_type = typename Traits::value_type;
    Node *m_pCurrent;

public:
    DoubleLinkedListForwardIterator(Node *pNode) : m_pCurrent(pNode) {}
    
    value_type &operator*() { return m_pCurrent->GetValueRef(); }
    
    DoubleLinkedListForwardIterator &operator++() {
        if(m_pCurrent) m_pCurrent = m_pCurrent->GetNext();
        return *this;
    }
    
    bool operator!=(const DoubleLinkedListForwardIterator &other) const {
        return m_pCurrent != other.m_pCurrent;
    }
    bool operator==(const DoubleLinkedListForwardIterator &other) const {
        return m_pCurrent == other.m_pCurrent;
    }
};

// BACKWARD ITERATOR (End -> Begin)

template <typename Traits>
class DoubleLinkedListBackwardIterator {
    using Node = NodeDoubleLinkedList<Traits>;
    using value_type = typename Traits::value_type;
    Node *m_pCurrent;

public:
    DoubleLinkedListBackwardIterator(Node *pNode) : m_pCurrent(pNode) {}
    
    value_type &operator*() { return m_pCurrent->GetValueRef(); }
    
    
    DoubleLinkedListBackwardIterator &operator++() {
        if(m_pCurrent) m_pCurrent = m_pCurrent->GetPrev();
        return *this;
    }
    
    bool operator!=(const DoubleLinkedListBackwardIterator &other) const {
        return m_pCurrent != other.m_pCurrent;
    }
    bool operator==(const DoubleLinkedListBackwardIterator &other) const {
        return m_pCurrent == other.m_pCurrent;
    }
};

// CLASE LISTA DOBLE ENLAZADA

template <typename Traits>
class CDoubleLinkedList {
    using  value_type  = typename Traits::value_type;
    using  forward_iterator  = DoubleLinkedListForwardIterator<Traits>;
    using  backward_iterator = DoubleLinkedListBackwardIterator<Traits>;
    using  Node = NodeDoubleLinkedList<Traits>;

    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;    
    
    mutable mutex m_mutex;

public:
    CDoubleLinkedList() {}
    
    // Constructor Copia
    CDoubleLinkedList(const CDoubleLinkedList &other) {
       
        lock_guard<mutex> lock(other.m_mutex);
        Node *pCurrent = other.m_pRoot;
        while(pCurrent) {
            push_back(pCurrent->GetValue(), pCurrent->GetRef());
            pCurrent = pCurrent->GetNext();
        }
    }
    
    // Move Constructor
    CDoubleLinkedList(CDoubleLinkedList &&other) noexcept {
        lock_guard<mutex> lock(other.m_mutex);
        m_pRoot = other.m_pRoot;
        m_pLast = other.m_pLast;
        m_nElements = other.m_nElements;
        
        other.m_pRoot = nullptr;
        other.m_pLast = nullptr;
        other.m_nElements = 0;
    }
    
    // Destructor
    virtual ~CDoubleLinkedList() { Clear(); }
    
    // Iteradores
    forward_iterator begin() { return forward_iterator(m_pRoot); }
    forward_iterator end()   { return forward_iterator(nullptr); }
    
    backward_iterator rbegin() { return backward_iterator(m_pLast); }
    backward_iterator rend()   { return backward_iterator(nullptr); }
    
    // Acceso por indice
    value_type &operator[](size_t index) {
        lock_guard<mutex> lock(m_mutex);
        
        if(index >= m_nElements) throw out_of_range("Index out of range");
        Node *pCurrent = m_pRoot;
        for(size_t i = 0; i < index; ++i) pCurrent = pCurrent->GetNext();
        return pCurrent->GetValueRef();
    }
    
    const value_type &operator[](size_t index) const {
        lock_guard<mutex> lock(m_mutex);
        if(index >= m_nElements) throw out_of_range("Index out of range");
        Node *pCurrent = m_pRoot;
        for(size_t i = 0; i < index; ++i) pCurrent = pCurrent->GetNext();
        return pCurrent->GetValue();
    }

    // MÉTODOS DE INSERCIÓN
  

    void push_back(const value_type &val, ref_type ref = -1) {
        lock_guard<mutex> lock(m_mutex);
        Node *pNew = new Node(val, ref, nullptr, m_pLast); 
        if (!m_pRoot) {
            m_pRoot = pNew;
            m_pLast = pNew;
        } else {
            m_pLast->SetNext(pNew);
            m_pLast = pNew;
        }
        ++m_nElements;
    }

    void push_front(const value_type &val, ref_type ref = -1) {
        lock_guard<mutex> lock(m_mutex);
        Node *pNew = new Node(val, ref, m_pRoot, nullptr); 
        
        if (!m_pRoot) {
            m_pRoot = pNew;
            m_pLast = pNew;
        } else {
            m_pRoot->SetPrev(pNew);
            m_pRoot = pNew;
        }
        ++m_nElements;
    }

    // INSERT: Usa Traits
    void Insert(const value_type &val, ref_type ref = -1) {
        lock_guard<mutex> lock(m_mutex);
        Node *pNew = new Node(val, ref);
        
        // 1. Lista vacia
        if (!m_pRoot) {
            m_pRoot = m_pLast = pNew;
            m_nElements++;
            return;
        }

        // 2. Buscar posicion usando Traits::Func
        Node *pCurrent = m_pRoot;
        typename Traits::Func compare; 
        
        // Buscamos donde insertar
        while (pCurrent) {
            if (compare(pCurrent->GetValue(), val)) break; 
            pCurrent = pCurrent->GetNext();
        }

        // 3. Realizar insercion
        if (pCurrent == m_pRoot) { 
            // Insertar al inicio
            pNew->SetNext(m_pRoot);
            m_pRoot->SetPrev(pNew);
            m_pRoot = pNew;
        } 
        else if (pCurrent == nullptr) { 
            // Insertar al final
            m_pLast->SetNext(pNew);
            pNew->SetPrev(m_pLast);
            m_pLast = pNew;
        } 
        else { 
            // Insertar en medio
            Node *pPrevNode = pCurrent->GetPrev();
            pPrevNode->SetNext(pNew);
            pNew->SetPrev(pPrevNode);
            pNew->SetNext(pCurrent);
            pCurrent->SetPrev(pNew);
        }
        m_nElements++;
    }

    // Foreach
    template<typename Func, typename... Args>
    void Foreach(Func fn, Args... args) {
        lock_guard<mutex> lock(m_mutex);
        Node *pCurrent = m_pRoot;
        while(pCurrent) {
            fn(pCurrent->GetValueRef(), args...);
            pCurrent = pCurrent->GetNext();
        }
    }
    
    // FirstThat
    template<typename Predicate>
    value_type *FirstThat(Predicate pred) {
        lock_guard<mutex> lock(m_mutex);
        Node *pCurrent = m_pRoot;
        while(pCurrent) {
            if(pred(pCurrent->GetValue())) return &(pCurrent->GetValueRef());
            pCurrent = pCurrent->GetNext();
        }
        return nullptr;
    }
    
    // Clear
    void Clear() {
        lock_guard<mutex> lock(m_mutex); 
        Node *pCurrent = m_pRoot;
        while(pCurrent) {
            Node *pNext = pCurrent->GetNext();
            delete pCurrent;
            pCurrent = pNext;
        }
        m_pRoot = nullptr;
        m_pLast = nullptr;
        m_nElements = 0;
    }

    size_t getSize() const { return m_nElements; }

    // Output Operator <<
    friend ostream &operator<<(ostream &os, CDoubleLinkedList<Traits> &container) {
        lock_guard<mutex> lock(container.m_mutex);
        os << "CDoubleLinkedList: size = " << container.getSize() << endl;
        os << "[ ";
        Node *pCurrent = container.m_pRoot;
        while(pCurrent) {
            os << "(" << pCurrent->GetValue() << ":" << pCurrent->GetRef() << ")";
            pCurrent = pCurrent->GetNext();
            if(pCurrent) os << " <-> "; 
        }
        os << " ]" << endl;
        return os;
    }
    
    // Input Operator >>
    friend istream &operator>>(istream &is, CDoubleLinkedList<Traits> &container) {
        value_type val;
        ref_type ref;
        if(is >> val >> ref) {
            container.push_back(val, ref);
        }
        return is;
    }
};

#endif // __DOUBLE_LINKED_LIST_H__