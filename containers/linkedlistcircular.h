#ifndef __LINKEDLIST_CIRCULAR_H__
#define __LINKEDLIST_CIRCULAR_H__
#include <iostream>
#include <mutex>
#include <functional>  // ← AGREGADO para greater<T> y less<T>
#include "../general/types.h"
#include "../util.h"
using namespace std;

// Traits para listas enlazadas circulares
template <typename T, typename _Func>
struct ListTraitCircular{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingTraitCircular : 
    public ListTraitCircular<T, greater<T> >{
};

template <typename T>
struct DescendingTraitCircular : 
    public ListTraitCircular<T, less<T> >{
};

// Forward declarations
template <typename Traits> class NodeLinkedListCircular;
template <typename Traits> class CLinkedListCircular;
template <typename Traits> class LinkedListCircularForwardIterator;

// ============================================
// NODO PARA LISTA CIRCULAR
// ============================================
template <typename Traits>
class NodeLinkedListCircular{
    template <typename T> friend class CLinkedListCircular;
    template <typename T> friend class LinkedListCircularForwardIterator;
    
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeLinkedListCircular<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;

public:
    NodeLinkedListCircular(){}
    NodeLinkedListCircular( value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){   }
    NodeLinkedListCircular( value_type _value, ref_type _ref, Node *pNext)
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

// ============================================
// ITERATOR CIRCULAR (da solo una vuelta completa)
// ============================================
template <typename Traits>
class LinkedListCircularForwardIterator {
    using Node = NodeLinkedListCircular<Traits>;
    using value_type = typename Traits::value_type;
    Node *m_pCurrent;
    Node *m_pStart;
    bool m_bFirstIteration;

public:
    LinkedListCircularForwardIterator(Node *pNode) 
        : m_pCurrent(pNode), m_pStart(pNode), m_bFirstIteration(true) {}
    
    value_type &operator*() { return m_pCurrent->GetValueRef(); }
    
    LinkedListCircularForwardIterator &operator++() {
        m_pCurrent = m_pCurrent->GetNext();
        if(m_pCurrent == m_pStart)
            m_bFirstIteration = false;
        return *this;
    }
    
    bool operator!=(const LinkedListCircularForwardIterator &other) const {
        if(!m_pCurrent) return false;
        if(m_bFirstIteration) return true;
        return m_pCurrent != m_pStart;
    }
    
    bool operator==(const LinkedListCircularForwardIterator &other) const {
        return !(*this != other);
    }
};

// ============================================
// LISTA ENLAZADA CIRCULAR (LEC)
// ============================================
template <typename Traits>
class CLinkedListCircular {
    using  value_type  = typename Traits::value_type;
    using  forward_iterator  = LinkedListCircularForwardIterator<Traits>;
    using  Node = NodeLinkedListCircular<Traits>;

    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable mutex m_mutex;

public:
    // Constructor por defecto
    CLinkedListCircular(){}
    
    // Constructor Copia
    CLinkedListCircular(const CLinkedListCircular &other){
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
    CLinkedListCircular(CLinkedListCircular &&other) noexcept {
        lock_guard<mutex> lock(other.m_mutex);
        m_pRoot = other.m_pRoot;
        m_pLast = other.m_pLast;
        m_nElements = other.m_nElements;
        
        other.m_pRoot = nullptr;
        other.m_pLast = nullptr;
        other.m_nElements = 0;
    }
    
    // Destructor seguro
    virtual ~CLinkedListCircular(){
        Clear();
    }
    
    // Iterators
    forward_iterator begin() { return forward_iterator(m_pRoot); }
    forward_iterator end()   { return forward_iterator(nullptr); }
    
    // operator[]
    value_type &operator[](size_t index){
        lock_guard<mutex> lock(m_mutex);
        if(index >= m_nElements || !m_pRoot)
            throw out_of_range("Index out of range");
        
        Node *pCurrent = m_pRoot;
        for(size_t i = 0; i < index; ++i)
            pCurrent = pCurrent->GetNext();
        
        return pCurrent->GetValueRef();
    }
    
    const value_type &operator[](size_t index) const {
        lock_guard<mutex> lock(m_mutex);
        if(index >= m_nElements || !m_pRoot)
            throw out_of_range("Index out of range");
        
        Node *pCurrent = m_pRoot;
        for(size_t i = 0; i < index; ++i)
            pCurrent = pCurrent->GetNext();
        
        return pCurrent->GetValue();
    }

    // Foreach
    template<typename Func, typename... Args>
    void Foreach(Func fn, Args... args){
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
    value_type *FirstThat(Predicate pred){
        lock_guard<mutex> lock(m_mutex);
        if(!m_pRoot) return nullptr;
        
        Node *pCurrent = m_pRoot;
        size_t count = 0;
        
        do {
            if(pred(pCurrent->GetValue()))
                return &(pCurrent->GetValueRef());
            pCurrent = pCurrent->GetNext();
            ++count;
        } while(pCurrent != m_pRoot && count < m_nElements);
        
        return nullptr;
    }
    
    // Clear
    void Clear(){
        if(!m_pRoot) return;
        
        // Romper circularidad primero
        if(m_pLast)
            m_pLast->GetNextRef() = nullptr;
        
        Node *pCurrent = m_pRoot;
        while(pCurrent){
            Node *pNext = pCurrent->GetNext();
            delete pCurrent;
            pCurrent = pNext;
        }
        
        m_pRoot = nullptr;
        m_pLast = nullptr;
        m_nElements = 0;
    }

    // Métodos públicos
    void push_back(const value_type &val, ref_type ref = -1);
    void Insert(const value_type &val, ref_type ref = -1);
    size_t getSize() const { return m_nElements;  }
    
    // Verificar si es circular
    bool IsCircular() const { 
        return m_pRoot && m_pLast && m_pLast->GetNext() == m_pRoot; 
    }
    
private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);

    // operator<<
    friend ostream &operator<<(ostream &os, CLinkedListCircular<Traits> &container){
        lock_guard<mutex> lock(container.m_mutex);
        os << "CLinkedListCircular: size = " << container.getSize();
        
        if(container.IsCircular())
            os << " [CIRCULAR]";
        
        os << endl << "[";
        
        if(container.m_pRoot){
            Node *pCurrent = container.m_pRoot;
            size_t count = 0;
            
            do {
                os << "(" << pCurrent->GetValue() << ":" << pCurrent->GetRef() << ")";
                pCurrent = pCurrent->GetNext();
                ++count;
                if(pCurrent != container.m_pRoot && count < container.m_nElements)
                    os << ",";
            } while(pCurrent != container.m_pRoot && count < container.m_nElements);
        }
        
        os << "]" << endl;
        return os;
    }
    
    // ============================================
    // operator>> CORREGIDO (SIN DEADLOCK)
    // ============================================
    friend istream &operator>>(istream &is, CLinkedListCircular<Traits> &container){
        // NO poner lock_guard aquí - push_back ya es thread-safe
        value_type val;
        ref_type ref;
        is >> val >> ref;
        container.push_back(val, ref);
        return is;
    }
};

// ============================================
// IMPLEMENTACIONES
// ============================================

template <typename Traits>
void CLinkedListCircular<Traits>::push_back(const value_type &val, ref_type ref){
    lock_guard<mutex> lock(m_mutex);
    
    Node *pNewNode = new Node(val, ref);
    
    if( !m_pRoot ){
        // Primera inserción
        m_pRoot = pNewNode;
        m_pLast = pNewNode;
        // CONECTAR PRIMER Y ULTIMO (CIRCULAR)
        m_pLast->GetNextRef() = m_pRoot;
    } else {
        // Insertar al final
        m_pLast->GetNextRef() = pNewNode;
        m_pLast = pNewNode;
        // RECONECTAR ULTIMO CON PRIMERO (CIRCULAR)
        m_pLast->GetNextRef() = m_pRoot;
    }
    
    ++m_nElements;
}

// CORREGIDO: Usar Traits y prevenir loop infinito
template <typename Traits>
void CLinkedListCircular<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    // 1. Usar Traits::Func() en vez de > directo
    if( !rParent || typename Traits::Func()(rParent->GetValue(), val) ){
        Node *pNew = new Node(val, ref, rParent);
        rParent = pNew;
        
        // Si insertamos en lista vacía o al final "lógico"
        if(!m_pLast || !pNew->GetNext() || pNew->GetNext() == m_pRoot){
            if (pNew->GetNext() == nullptr) {
                m_pLast = pNew;
            }
        }
        // Reconectar circularidad
        if(m_pLast) m_pLast->GetNextRef() = m_pRoot;

        ++m_nElements;
        return;
    }
    
    // 2. DETECTAR EL FINAL DE LA VUELTA para no loopear infinitamente
    if(rParent->GetNext() == m_pRoot){
        // Si llegamos al final y no se insertó antes, va al final.
        Node *pNew = new Node(val, ref, m_pRoot);
        rParent->GetNextRef() = pNew;
        m_pLast = pNew;
        ++m_nElements;
        return;
    }
    
    InternalInsert(rParent->GetNextRef(), val, ref);
}

template <typename Traits>
void CLinkedListCircular<Traits>::Insert(const value_type &val, ref_type ref){
    lock_guard<mutex> lock(m_mutex);
    
    // Check rápido para cabeza para eficiencia
    if (!m_pRoot || typename Traits::Func()(m_pRoot->GetValue(), val)) {
        Node *pNew = new Node(val, ref, m_pRoot);
        m_pRoot = pNew;
        if (!m_pLast) m_pLast = pNew;
        m_pLast->GetNextRef() = m_pRoot; 
        ++m_nElements;
        return;
    }

    InternalInsert(m_pRoot, val, ref);
    
    // Asegurar circularidad
    if(m_pLast && m_pRoot)
        m_pLast->GetNextRef() = m_pRoot;
}

#endif // __LINKEDLIST_CIRCULAR_H__
