#ifndef __CDOUBLE_LINKED_LIST_H__
#define __CDOUBLE_LINKED_LIST_H__
#include <iostream>
#include <mutex>
#include <fstream>
#include <utility>
#include "../general/types.h"
#include "../util.h"
using namespace std;

// Traits para listas doblemente enlazadas circulares
template <typename T, typename _Func>
struct CDListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct CDAscendingTrait : 
    public CDListTrait<T, std::greater<T> >{
};

template <typename T>
struct CDDescendingTrait : 
    public CDListTrait<T, std::less<T> >{
};

// Nodo para listas doblemente enlazadas circulares
template <typename Traits>
class NodeCircularDoubleLinkedList{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeCircularDoubleLinkedList<Traits>;
    
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;
    Node *m_pPrev = nullptr;  // Puntero al nodo anterior

public:
    NodeCircularDoubleLinkedList(){}
    NodeCircularDoubleLinkedList( value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){   }
    
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }

    ref_type    GetRef     () const { return m_ref;   }
    ref_type   &GetRefRef  () { return m_ref;   }

    Node      * GetNext     () const { return m_pNext;   }
    Node      *&GetNextRef  () { return m_pNext;   }
    
    Node      * GetPrev     () const { return m_pPrev;   }
    Node      *&GetPrevRef  () { return m_pPrev;   }

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

// Forward Iterator para listas doblemente enlazadas circulares
template <typename Container>
class CircularDoubleLinkedListForwardIterator{
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;
    
private:
    Node *m_pCurrent = nullptr;
    Node *m_pRoot = nullptr;
    bool m_firstIteration = true;
    
public:
    CircularDoubleLinkedListForwardIterator(Node *pNode, Node *pRoot = nullptr) 
        : m_pCurrent(pNode), m_pRoot(pRoot) {}
    
    bool operator!=(const CircularDoubleLinkedListForwardIterator &other) const {
        if (m_pCurrent == nullptr && other.m_pCurrent == nullptr) {
            return false;
        }
        if (m_pCurrent == nullptr || other.m_pCurrent == nullptr) {
            return true;
        }
        
        return m_pCurrent != other.m_pCurrent || m_firstIteration != other.m_firstIteration;
    }
    
    CircularDoubleLinkedListForwardIterator &operator++() {
        if (m_pCurrent) {
            m_firstIteration = false;
            m_pCurrent = m_pCurrent->GetNext();
            
            if (m_pCurrent == m_pRoot && !m_firstIteration) {
                m_pCurrent = nullptr;
            }
        }
        return *this;
    }
    
    value_type &operator*() {
        return m_pCurrent->GetValueRef();
    }  
};

// Backward Iterator para listas doblemente enlazadas circulares 
template <typename Container>
class CircularDoubleLinkedListBackwardIterator{
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;
    
private:
    Node *m_pCurrent = nullptr;
    Node *m_pLast = nullptr;
    bool m_firstIteration = true;
    
public:
    CircularDoubleLinkedListBackwardIterator(Node *pNode, Node *pLast = nullptr) 
        : m_pCurrent(pNode), m_pLast(pLast) {}
    
    bool operator!=(const CircularDoubleLinkedListBackwardIterator &other) const {
        if (m_pCurrent == nullptr && other.m_pCurrent == nullptr) {
            return false;
        }
        if (m_pCurrent == nullptr || other.m_pCurrent == nullptr) {
            return true;
        }
        
        return m_pCurrent != other.m_pCurrent || m_firstIteration != other.m_firstIteration;
    }
    
    CircularDoubleLinkedListBackwardIterator &operator++() {
        if (m_pCurrent) {
            m_firstIteration = false;
            m_pCurrent = m_pCurrent->GetPrev();  // Va hacia ATRÁS
            
            if (m_pCurrent == m_pLast && !m_firstIteration) {
                m_pCurrent = nullptr;
            }
        }
        return *this;
    }
    
    value_type &operator*() {
        return m_pCurrent->GetValueRef();
    }  
};

// Clase CircularDoubleLinkedList
template <typename Traits>
class CCircularDoubleLinkedList {
public:
    using  value_type         = typename Traits::value_type;
    using  forward_iterator   = CircularDoubleLinkedListForwardIterator < CCircularDoubleLinkedList<Traits> >;
    using  backward_iterator  = CircularDoubleLinkedListBackwardIterator< CCircularDoubleLinkedList<Traits> >;  
    friend forward_iterator;
    friend backward_iterator;
    using  Node = NodeCircularDoubleLinkedList<Traits>;

private:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable std::mutex mtx;

public:
    // Constructor por defecto
    CCircularDoubleLinkedList(){}
    
    // Constructor copia
    CCircularDoubleLinkedList(const CCircularDoubleLinkedList<Traits> &other) {
        std::lock_guard<std::mutex> lock(other.mtx);
        
        m_pRoot = nullptr;
        m_pLast = nullptr;
        m_nElements = 0;
        
        if (other.m_pRoot == nullptr) return;
        
        Node *pCurrent = other.m_pRoot;
        do {
            push_back(pCurrent->GetValue(), pCurrent->GetRef());
            pCurrent = pCurrent->GetNext();
        } while (pCurrent != other.m_pRoot);
    }
    
    // Move Constructor
    CCircularDoubleLinkedList(CCircularDoubleLinkedList<Traits> &&other) noexcept 
        : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0) {
        std::lock_guard<std::mutex> lock(other.mtx);
        
        m_pRoot = std::exchange(other.m_pRoot, nullptr);
        m_pLast = std::exchange(other.m_pLast, nullptr);
        m_nElements = std::exchange(other.m_nElements, 0);
    }
    
    // Destructor seguro y virtual
    virtual ~CCircularDoubleLinkedList() {
        std::lock_guard<std::mutex> lock(mtx);
        
        if (m_pRoot == nullptr) return;
        
        // Romper el círculo primero
        if (m_pLast) {
            m_pLast->GetNextRef() = nullptr;
            m_pRoot->GetPrevRef() = nullptr;  // También romper conexión anterior
        }
        
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

    // Iteradores Forward
    forward_iterator begin() {
        return forward_iterator(m_pRoot, m_pRoot); 
    }

    forward_iterator end() {
        return forward_iterator(nullptr, m_pRoot); 
    }

    // Iteradores Backward 
    backward_iterator rbegin() {
        return backward_iterator(m_pLast, m_pLast);
    }

    backward_iterator rend() {
        return backward_iterator(nullptr, m_pLast);
    }

    // Push back
    void push_back(const value_type &val, ref_type ref){
        std::lock_guard<std::mutex> lock(mtx);
        
        Node *pNewNode = new Node(val, ref);
        
        if( !m_pRoot ) {
            m_pRoot = pNewNode;
            m_pLast = pNewNode;
            // CIRCULAR DOBLE: Conexiones bidireccionales
            pNewNode->GetNextRef() = m_pRoot;
            pNewNode->GetPrevRef() = m_pRoot;
        } else {
            m_pLast->GetNextRef() = pNewNode;
            pNewNode->GetPrevRef() = m_pLast;
            m_pLast = pNewNode;
            // CIRCULAR DOBLE: Cerrar el círculo
            m_pLast->GetNextRef() = m_pRoot;
            m_pRoot->GetPrevRef() = m_pLast;
        }
        ++m_nElements;
    }
    
    // Push front 
    void push_front(const value_type &val, ref_type ref){
        std::lock_guard<std::mutex> lock(mtx);
        
        Node *pNewNode = new Node(val, ref);
        
        if( !m_pRoot ) {
            m_pRoot = pNewNode;
            m_pLast = pNewNode;
            //CIRCULAR DOBLE
            pNewNode->GetNextRef() = m_pRoot;
            pNewNode->GetPrevRef() = m_pRoot;
        } else {
            pNewNode->GetNextRef() = m_pRoot;
            pNewNode->GetPrevRef() = m_pLast;
            m_pRoot->GetPrevRef() = pNewNode;
            m_pLast->GetNextRef() = pNewNode;
            m_pRoot = pNewNode;
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
        
        if (m_pRoot == nullptr) return;
        
        Node *pCurrent = m_pRoot;
        do {
            of(pCurrent->GetValueRef(), args...);
            pCurrent = pCurrent->GetNext();
        } while (pCurrent != m_pRoot);
    }
    
    // FirstThat
    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args){
        std::lock_guard<std::mutex> lock(mtx);
        
        if (m_pRoot == nullptr) return end();
        
        Node *pCurrent = m_pRoot;
        do {
            if (of(pCurrent->GetValueRef(), args...)) {
                return forward_iterator(pCurrent, m_pRoot);
            }
            pCurrent = pCurrent->GetNext();
        } while (pCurrent != m_pRoot);
        
        return end();
    }

    // Operator<< (salida/imprimir)
    friend ostream &operator<<(ostream &os, CCircularDoubleLinkedList<Traits> &container){
        std::lock_guard<std::mutex> lock(container.mtx);
        
        os << "CCircularDoubleLinkedList: size = " << container.m_nElements << endl;
        os << "[";
        
        if (container.m_pRoot != nullptr) {
            Node *pCurrent = container.m_pRoot;
            do {
                os << "(" << pCurrent->GetValue() << ":" << pCurrent->GetRef() << ")";
                pCurrent = pCurrent->GetNext();
                if (pCurrent != container.m_pRoot) {
                    os << ", ";
                }
            } while (pCurrent != container.m_pRoot);
        }
        
        os << "] (circular-double)" << endl;
        return os;
    }
    
    // Operator>> (entrada/rellenar)
    friend istream &operator>>(istream &is, CCircularDoubleLinkedList<Traits> &container){
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
        
        if (m_pRoot != nullptr) {
            Node *pCurrent = m_pRoot;
            do {
                outFile << pCurrent->GetValue() << " " << pCurrent->GetRef() << endl;
                pCurrent = pCurrent->GetNext();
            } while (pCurrent != m_pRoot);
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
        if (m_pRoot != nullptr) {
            if (m_pLast) {
                m_pLast->GetNextRef() = nullptr;
                m_pRoot->GetPrevRef() = nullptr;  //  Romper ambas conexiones
            }
            
            Node *pCurrent = m_pRoot;
            while (pCurrent) {
                Node *pNext = pCurrent->GetNext();
                delete pCurrent;
                pCurrent = pNext;
            }
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
                pNewNode->GetNextRef() = m_pRoot;
                pNewNode->GetPrevRef() = m_pRoot;  // CIRCULAR DOBLE
            } else {
                m_pLast->GetNextRef() = pNewNode;
                pNewNode->GetPrevRef() = m_pLast;
                m_pLast = pNewNode;
                m_pLast->GetNextRef() = m_pRoot;
                m_pRoot->GetPrevRef() = m_pLast;  // CIRCULAR DOBLE
            }
            ++m_nElements;
        }
        
        inFile.close();
    }

private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
        // Caso especial: lista vacía
        if (m_nElements == 0) {
            Node *pNew = new Node(val, ref);
            m_pRoot = pNew;
            m_pLast = pNew;
            pNew->GetNextRef() = m_pRoot;
            pNew->GetPrevRef() = m_pRoot;  // CIRCULAR DOBLE
            rParent = pNew;
            ++m_nElements;
            return;
        }
        
        // Caso: insertar al inicio
        if (typename Traits::Func()(rParent->GetValue(), val)) {
            Node *pNew = new Node(val, ref);
            pNew->GetNextRef() = rParent;
            pNew->GetPrevRef() = rParent->GetPrev();  // Conexión bidireccional
            
            rParent->GetPrev()->GetNextRef() = pNew;
            rParent->GetPrevRef() = pNew;
            
            if (rParent == m_pRoot) {
                m_pRoot = pNew;
            }
            
            rParent = pNew;
            ++m_nElements;
            return;
        }
        
        // Recorrer hasta encontrar la posición
        Node *pCurrent = rParent;
        
        do {
            if (pCurrent->GetNext() == m_pRoot || typename Traits::Func()(pCurrent->GetNext()->GetValue(), val)) {
                Node *pNew = new Node(val, ref);
                pNew->GetNextRef() = pCurrent->GetNext();
                pNew->GetPrevRef() = pCurrent;  //  Conexión bidireccional
                
                pCurrent->GetNext()->GetPrevRef() = pNew;
                pCurrent->GetNextRef() = pNew;
                
                if (pCurrent == m_pLast) {
                    m_pLast = pNew;
                }
                
                ++m_nElements;
                return;
            }
            
            pCurrent = pCurrent->GetNext();
        } while (pCurrent != m_pRoot);
    }
};

#endif // __CDOUBLE_LINKED_LIST_H__
