#ifndef __DOUBLE_LINKED_LIST_CIRCULAR_H__
#define __DOUBLE_LINKED_LIST_CIRCULAR_H__

#include <iostream>
#include <mutex>
#include <stdexcept>
#include "../general/types.h"
#include "../util.h"

using namespace std;

// TRAITS para listas circulares
template <typename T, typename _Func>
struct DoubleCircularListTrait{
    using value_type = T;
    using Func       = _Func;
};

// Quitamos std:: de greater
template <typename T>
struct DoubleCircularAscendingTrait : public DoubleCircularListTrait<T, greater<T>> {};

// Quitamos std:: de less
template <typename T>
struct DoubleCircularDescendingTrait : public DoubleCircularListTrait<T, less<T>> {};

// Forward declarations
template <typename Traits> class NodeDoubleLinkedListCircular;
template <typename Traits> class CDoubleLinkedListCircular;
template <typename Traits> class DoubleCircularForwardIterator;
template <typename Traits> class DoubleCircularBackwardIterator;

// NODO DOBLE ENLAZADO CIRCULAR
template <typename Traits>
class NodeDoubleLinkedListCircular{
    template <typename T> friend class CDoubleLinkedListCircular;
    template <typename T> friend class DoubleCircularForwardIterator;
    template <typename T> friend class DoubleCircularBackwardIterator;
    
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeDoubleLinkedListCircular<Traits>;

private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;
    Node *m_pPrev = nullptr;

public:
    NodeDoubleLinkedListCircular() {}
    NodeDoubleLinkedListCircular(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref) {}
    NodeDoubleLinkedListCircular(value_type _value, ref_type _ref, Node *pNext, Node *pPrev = nullptr)
        : m_data(_value), m_ref(_ref), m_pNext(pNext), m_pPrev(pPrev) {}
        
    value_type  GetValue() const { return m_data; }
    value_type &GetValueRef()    { return m_data; }
    ref_type    GetRef() const   { return m_ref; }
    
    Node *GetNext() const { return m_pNext; }
    Node *GetPrev() const { return m_pPrev; }
    
    void SetNext(Node *pNext) { m_pNext = pNext; }
    void SetPrev(Node *pPrev) { m_pPrev = pPrev; }
};

// FORWARD ITERATOR CIRCULAR
template <typename Traits>
class DoubleCircularForwardIterator {
    using Node = NodeDoubleLinkedListCircular<Traits>;
    using value_type = typename Traits::value_type;
    Node *m_pCurrent;
    Node *m_pStart;
    bool m_bFirstIteration;

public:
    DoubleCircularForwardIterator(Node *pNode) 
        : m_pCurrent(pNode), m_pStart(pNode), m_bFirstIteration(true) {}
    
    value_type &operator*() { return m_pCurrent->GetValueRef(); }
    
    DoubleCircularForwardIterator &operator++() {
        if(m_pCurrent) {
            m_pCurrent = m_pCurrent->GetNext();
            if(m_pCurrent == m_pStart) m_bFirstIteration = false;
        }
        return *this;
    }
    
    bool operator!=(const DoubleCircularForwardIterator &other) const {
        if(!m_pCurrent) return false;
        if(m_bFirstIteration) return true;
        return m_pCurrent != m_pStart;
    }
    
    bool operator==(const DoubleCircularForwardIterator &other) const {
        return !(*this != other);
    }
};


// BACKWARD ITERATOR CIRCULAR

template <typename Traits>
class DoubleCircularBackwardIterator {
    using Node = NodeDoubleLinkedListCircular<Traits>;
    using value_type = typename Traits::value_type;
    Node *m_pCurrent;
    Node *m_pStart;
    bool m_bFirstIteration;

public:
    DoubleCircularBackwardIterator(Node *pNode) 
        : m_pCurrent(pNode), m_pStart(pNode), m_bFirstIteration(true) {}
    
    value_type &operator*() { return m_pCurrent->GetValueRef(); }
    
    DoubleCircularBackwardIterator &operator++() {
        if(m_pCurrent) {
            m_pCurrent = m_pCurrent->GetPrev();
            if(m_pCurrent == m_pStart) m_bFirstIteration = false;
        }
        return *this;
    }
    
    bool operator!=(const DoubleCircularBackwardIterator &other) const {
        if(!m_pCurrent) return false;
        if(m_bFirstIteration) return true;
        return m_pCurrent != m_pStart;
    }
    
    bool operator==(const DoubleCircularBackwardIterator &other) const {
        return !(*this != other);
    }
};

// LISTA DOBLE ENLAZADA CIRCULAR 

template <typename Traits>
class CDoubleLinkedListCircular {
    using  value_type  = typename Traits::value_type;
    using  forward_iterator  = DoubleCircularForwardIterator<Traits>;
    using  backward_iterator = DoubleCircularBackwardIterator<Traits>;
    using  Node = NodeDoubleLinkedListCircular<Traits>;

    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    
    mutable mutex m_mutex;

public:
    CDoubleLinkedListCircular() {}
    
    // Constructor Copia
    CDoubleLinkedListCircular(const CDoubleLinkedListCircular &other) {
        lock_guard<mutex> lock(other.m_mutex);
        if(!other.m_pRoot) return;
        
        Node *pCurrent = other.m_pRoot;
        size_t count = 0;
        do {
            push_back(pCurrent->GetValue(), pCurrent->GetRef());
            pCurrent = pCurrent->GetNext();
            ++count;
        } while(pCurrent != other.m_pRoot && count < other.m_nElements);
    }
    
    // Move Constructor
    CDoubleLinkedListCircular(CDoubleLinkedListCircular &&other) noexcept {
        lock_guard<mutex> lock(other.m_mutex);
        m_pRoot = other.m_pRoot;
        m_pLast = other.m_pLast;
        m_nElements = other.m_nElements;
        
        other.m_pRoot = nullptr;
        other.m_pLast = nullptr;
        other.m_nElements = 0;
    }
    
    // Destructor
    virtual ~CDoubleLinkedListCircular() { Clear(); }
    
    // Iterators
    forward_iterator begin() { return forward_iterator(m_pRoot); }
    forward_iterator end()   { return forward_iterator(nullptr); }
    
    backward_iterator rbegin() { return backward_iterator(m_pLast); }
    backward_iterator rend()   { return backward_iterator(nullptr); }
    
    // operator[]
    value_type &operator[](size_t index) {
        lock_guard<mutex> lock(m_mutex);
    
        if(index >= m_nElements || !m_pRoot) throw out_of_range("Index out of range");
        Node *pCurrent = m_pRoot;
        for(size_t i = 0; i < index; ++i) pCurrent = pCurrent->GetNext();
        return pCurrent->GetValueRef();
    }

    // MÉTODOS DE INSERCIÓN CIRCULARES
    
    void push_back(const value_type &val, ref_type ref = -1) {
        lock_guard<mutex> lock(m_mutex);
        Node *pNew = new Node(val, ref);
        
        if (!m_pRoot) {
            // Primera inserción - conectar consigo mismo
            m_pRoot = m_pLast = pNew;
            pNew->SetNext(pNew);
            pNew->SetPrev(pNew);
        } else {
            // Insertar al final y reconectar circularmente
            pNew->SetNext(m_pRoot);
            pNew->SetPrev(m_pLast);
            m_pLast->SetNext(pNew);
            m_pRoot->SetPrev(pNew);
            m_pLast = pNew;
        }
        ++m_nElements;
    }

    void push_front(const value_type &val, ref_type ref = -1) {
        lock_guard<mutex> lock(m_mutex);
        Node *pNew = new Node(val, ref);
        
        if (!m_pRoot) {
            m_pRoot = m_pLast = pNew;
            pNew->SetNext(pNew);
            pNew->SetPrev(pNew);
        } else {
            pNew->SetNext(m_pRoot);
            pNew->SetPrev(m_pLast);
            m_pRoot->SetPrev(pNew);
            m_pLast->SetNext(pNew);
            m_pRoot = pNew;
        }
        ++m_nElements;
    }

    // INSERT usando Traits
    void Insert(const value_type &val, ref_type ref = -1) {
        lock_guard<mutex> lock(m_mutex);
        Node *pNew = new Node(val, ref);
        
        if (!m_pRoot) {
            m_pRoot = m_pLast = pNew;
            pNew->SetNext(pNew);
            pNew->SetPrev(pNew);
            m_nElements++;
            return;
        }

        Node *pCurrent = m_pRoot;
        typename Traits::Func compare;
        size_t count = 0;
        
        // Buscar posición correcta
        do {
            if (compare(pCurrent->GetValue(), val)) break;
            pCurrent = pCurrent->GetNext();
            ++count;
        } while (pCurrent != m_pRoot && count < m_nElements);

        if (pCurrent == m_pRoot && compare(m_pRoot->GetValue(), val)) {
            // Insertar al inicio
            pNew->SetNext(m_pRoot);
            pNew->SetPrev(m_pLast);
            m_pRoot->SetPrev(pNew);
            m_pLast->SetNext(pNew);
            m_pRoot = pNew;
        } else if (count == m_nElements || pCurrent == m_pRoot) {
            // Insertar al final
            pNew->SetNext(m_pRoot);
            pNew->SetPrev(m_pLast);
            m_pLast->SetNext(pNew);
            m_pRoot->SetPrev(pNew);
            m_pLast = pNew;
        } else {
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
        if(!m_pRoot) return;
        
        Node *pCurrent = m_pRoot;
        size_t count = 0;
        do {
            fn(pCurrent->GetValueRef(), args...);
            pCurrent = pCurrent->GetNext();
            ++count;
        } while(pCurrent != m_pRoot && count < m_nElements);
    }
    
    // FirstThat
    template<typename Predicate>
    value_type *FirstThat(Predicate pred) {
        lock_guard<mutex> lock(m_mutex);
        if(!m_pRoot) return nullptr;
        
        Node *pCurrent = m_pRoot;
        size_t count = 0;
        do {
            if(pred(pCurrent->GetValue())) return &(pCurrent->GetValueRef());
            pCurrent = pCurrent->GetNext();
            ++count;
        } while(pCurrent != m_pRoot && count < m_nElements);
        return nullptr;
    }
    
    // Clear CON PROTECCIÓN
    void Clear() {
        lock_guard<mutex> lock(m_mutex);
        if(!m_pRoot) return;
        
        // Romper circularidad
        m_pLast->SetNext(nullptr);
        m_pRoot->SetPrev(nullptr);
        
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
    
    bool IsCircular() const { 
        return m_pRoot && m_pLast && 
               m_pLast->GetNext() == m_pRoot && 
               m_pRoot->GetPrev() == m_pLast; 
    }

    // operator<<
    friend ostream &operator<<(ostream &os, CDoubleLinkedListCircular<Traits> &container) {
        lock_guard<mutex> lock(container.m_mutex);
        os << "CDoubleLinkedListCircular: size = " << container.getSize();
        if(container.IsCircular()) os << " [CIRCULAR]";
        os << endl << "[ ";
        
        if(container.m_pRoot) {
            Node *pCurrent = container.m_pRoot;
            size_t count = 0;
            do {
                os << "(" << pCurrent->GetValue() << ":" << pCurrent->GetRef() << ")";
                pCurrent = pCurrent->GetNext();
                ++count;
                if(pCurrent != container.m_pRoot && count < container.m_nElements)
                    os << " <-> ";
            } while(pCurrent != container.m_pRoot && count < container.m_nElements);
        }
        os << " ]" << endl;
        return os;
    }
    
    // operator>>
    friend istream &operator>>(istream &is, CDoubleLinkedListCircular<Traits> &container) {
        value_type val;
        ref_type ref;
        if(is >> val >> ref) {
            container.push_back(val, ref);
        }
        return is;
    }
};

#endif // __DOUBLE_LINKED_LIST_CIRCULAR_H__