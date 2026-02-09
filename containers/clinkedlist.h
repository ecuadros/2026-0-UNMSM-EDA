#ifndef __CLINKEDLIST_H__
#define __CLINKEDLIST_H__
#include <iostream>
#include <mutex>
#include <fstream>
#include <utility>
#include "../general/types.h"
#include "../util.h"
using namespace std;

// Traits para listas enlazadas circulares
template <typename T, typename _Func>
struct CListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct CAscendingTrait : 
    public CListTrait<T, std::greater<T> >{
};

template <typename T>
struct CDescendingTrait : 
    public CListTrait<T, std::less<T> >{
};

// Nodo para listas enlazadas circulares
template <typename Traits>
class NodeCircularLinkedList{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeCircularLinkedList<Traits>;
    
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;

public:
    NodeCircularLinkedList(){}
    NodeCircularLinkedList( value_type _value, ref_type _ref = -1)
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

// Forward Iterator para listas enlazadas circulares
template <typename Container>
class CircularLinkedListForwardIterator{
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;
    
private:
    Node *m_pCurrent = nullptr;
    Node *m_pRoot = nullptr;
    bool m_firstIteration = true;
    
public:
    CircularLinkedListForwardIterator(Node *pNode, Node *pRoot = nullptr) 
        : m_pCurrent(pNode), m_pRoot(pRoot) {}
    
    bool operator!=(const CircularLinkedListForwardIterator &other) const {
        // Para listas circulares  verificar si se dio la vuelta
        if (m_pCurrent == nullptr && other.m_pCurrent == nullptr) {
            return false;
        }
        if (m_pCurrent == nullptr || other.m_pCurrent == nullptr) {
            return true;
        }
        
        return m_pCurrent != other.m_pCurrent || m_firstIteration != other.m_firstIteration;
    }
    
    CircularLinkedListForwardIterator &operator++() {
        if (m_pCurrent) {
            m_firstIteration = false;
            m_pCurrent = m_pCurrent->GetNext();
            
            // Si se dio la vuelta completa, marcar como fin
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

// Clase Circular LinkedList
template <typename Traits>
class CCircularLinkedList {
public:
    using  value_type        = typename Traits::value_type;
    using  forward_iterator  = CircularLinkedListForwardIterator < CCircularLinkedList<Traits> >;
    friend forward_iterator;
    using  Node = NodeCircularLinkedList<Traits>;

private:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable std::mutex mtx;  // Para concurrencia

public:
    // Constructor por defecto
    CCircularLinkedList(){}
    
    // Constructor copia
    CCircularLinkedList(const CCircularLinkedList<Traits> &other) {
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
    CCircularLinkedList(CCircularLinkedList<Traits> &&other) noexcept 
        : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0) {
        std::lock_guard<std::mutex> lock(other.mtx);
        
        m_pRoot = std::exchange(other.m_pRoot, nullptr);
        m_pLast = std::exchange(other.m_pLast, nullptr);
        m_nElements = std::exchange(other.m_nElements, 0);
    }
    
    // Destructor seguro y virtual
    virtual ~CCircularLinkedList() {
        std::lock_guard<std::mutex> lock(mtx);
        
        if (m_pRoot == nullptr) return;
        
        // Romper el círculo primero
        if (m_pLast) {
            m_pLast->GetNextRef() = nullptr;
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

    // Iteradores
    forward_iterator begin() {
        return forward_iterator(m_pRoot, m_pRoot); 
    }

    forward_iterator end() {
        return forward_iterator(nullptr, m_pRoot); 
    }

    // Push back
    void push_back(const value_type &val, ref_type ref){
        std::lock_guard<std::mutex> lock(mtx);
        
        Node *pNewNode = new Node(val, ref);
        
        if( !m_pRoot ) {
            m_pRoot = pNewNode;
            m_pLast = pNewNode;
            // CIRCULAR: El último apunta al primero
            pNewNode->GetNextRef() = m_pRoot;
        } else {
            m_pLast->GetNextRef() = pNewNode;
            m_pLast = pNewNode;
            // CIRCULAR: El último apunta al primero
            m_pLast->GetNextRef() = m_pRoot;
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
    friend ostream &operator<<(ostream &os, CCircularLinkedList<Traits> &container){
        std::lock_guard<std::mutex> lock(container.mtx);
        
        os << "CCircularLinkedList: size = " << container.m_nElements << endl;
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
        
        os << "] (circular)" << endl;
        return os;
    }
    
    // Operator>> (entrada/rellenar)
    friend istream &operator>>(istream &is, CCircularLinkedList<Traits> &container){
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
                pNewNode->GetNextRef() = m_pRoot; // CIRCULAR
            } else {
                m_pLast->GetNextRef() = pNewNode;
                m_pLast = pNewNode;
                m_pLast->GetNextRef() = m_pRoot; // CIRCULAR
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
            pNew->GetNextRef() = m_pRoot; // CIRCULAR
            rParent = pNew;
            ++m_nElements;
            return;
        }
        
        // Caso: insertar al inicio
        if (typename Traits::Func()(rParent->GetValue(), val)) {
            Node *pNew = new Node(val, ref);
            pNew->GetNextRef() = rParent;
            
            // Si rParent es la raíz, actualizar last para que apunte al nuevo root
            if (rParent == m_pRoot) {
                m_pLast->GetNextRef() = pNew; // CIRCULAR
                m_pRoot = pNew;
            }
            
            rParent = pNew;
            ++m_nElements;
            return;
        }
        
        // Recorrer hasta encontrar la posición o llegar al último
        Node *pCurrent = rParent;
        
        do {
            if (pCurrent->GetNext() == m_pRoot || typename Traits::Func()(pCurrent->GetNext()->GetValue(), val)) {
                Node *pNew = new Node(val, ref);
                pNew->GetNextRef() = pCurrent->GetNext();
                pCurrent->GetNextRef() = pNew;
                
                // Si se inserta después del último, actualizar m_pLast
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

#endif // __CLINKEDLIST_H__
