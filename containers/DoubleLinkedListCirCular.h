#ifndef __DOUBLELINKEDLISTCIRCULAR_H__
#define __DOUBLELINKEDLISTCIRCULAR_H__

#include <iostream>
#include <mutex>
#include <utility> // std::exchange
#include "linkedlist.h" // Para Traits
#include "doubleLinkedList.h" // Reutilizamos el Nodo Doble

using namespace std;

// --- Iterador FORWARD (Hacia adelante) ---
// Se mueve usando GetNext() y se detiene al volver al inicio.
template <typename Container>
class CircularDoubleForwardIterator {
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

private:
    Node *m_pCurrent;
    Node *m_pStart;   // Punto de partida para saber cuándo detenerse

public:
    CircularDoubleForwardIterator(Node *pNode, Node *pStart) 
        : m_pCurrent(pNode), m_pStart(pStart) {}

    bool operator!=(const CircularDoubleForwardIterator &other) const {
        return m_pCurrent != other.m_pCurrent;
    }

    bool operator==(const CircularDoubleForwardIterator &other) const {
        return m_pCurrent == other.m_pCurrent;
    }

    CircularDoubleForwardIterator &operator++() {
        if (m_pCurrent) {
            m_pCurrent = m_pCurrent->GetNext();
            // MAGIA: Si volvemos al nodo de inicio, nos convertimos en nullptr (Fin)
            if (m_pCurrent == m_pStart) {
                m_pCurrent = nullptr;
            }
        }
        return *this;
    }

    value_type &operator*() {
        return m_pCurrent->GetValueRef();
    }
};

// --- Iterador BACKWARD (Hacia atrás) ---
// Se mueve usando GetPrev() y se detiene al volver al final.
template <typename Container>
class CircularDoubleBackwardIterator {
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

private:
    Node *m_pCurrent;
    Node *m_pStart; // En este caso, el "Inicio" suele ser m_pLast

public:
    CircularDoubleBackwardIterator(Node *pNode, Node *pStart) 
        : m_pCurrent(pNode), m_pStart(pStart) {}

    bool operator!=(const CircularDoubleBackwardIterator &other) const {
        return m_pCurrent != other.m_pCurrent;
    }

    // OJO: Usamos operator++ para "avanzar" el iterador, 
    // aunque físicamente nos movemos hacia ATRÁS en la lista.
    CircularDoubleBackwardIterator &operator++() {
        if (m_pCurrent) {
            m_pCurrent = m_pCurrent->GetPrev(); // <--- La diferencia clave
            
            // Si damos toda la vuelta y llegamos al que empezamos...
            if (m_pCurrent == m_pStart) {
                m_pCurrent = nullptr;
            }
        }
        return *this;
    }

    value_type &operator*() {
        return m_pCurrent->GetValueRef();
    }
};

// --- CLASE LISTA DOBLE CIRCULAR ---
template <typename Traits>
class CDoubleLinkedListCircular {
public:
    using value_type = typename Traits::value_type;
    using Node       = NodeDoubleLinkedList<Traits>; // Reutilizamos nodo de doubleLinkedList.h
    
    // Definimos los dos tipos de iteradores
    using forward_iterator  = CircularDoubleForwardIterator< CDoubleLinkedListCircular<Traits> >;
    using backward_iterator = CircularDoubleBackwardIterator< CDoubleLinkedListCircular<Traits> >;
    
    friend forward_iterator;
    friend backward_iterator;

private:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable std::mutex m_Block;

public:
    CDoubleLinkedListCircular() {}

    virtual ~CDoubleLinkedListCircular() {
        if (m_pRoot) {
            Node *pTemp = m_pRoot;
            do {
                Node *pNext = pTemp->GetNext();
                delete pTemp;
                pTemp = pNext;
            } while (pTemp != m_pRoot);
        }
        m_pRoot = nullptr;
        m_pLast = nullptr;
        m_nElements = 0;
    }

    // Constructor Copia
    CDoubleLinkedListCircular(const CDoubleLinkedListCircular &another) {
        std::lock_guard<std::mutex> lock(another.m_Block);
        if (another.m_pRoot) {
            Node *pTemp = another.m_pRoot;
            do {
                this->push_back(pTemp->GetValue(), pTemp->GetRef());
                pTemp = pTemp->GetNext();
            } while (pTemp != another.m_pRoot);
        }
    }

    // Move Constructor
    CDoubleLinkedListCircular(CDoubleLinkedListCircular &&another) noexcept {
        std::lock_guard<std::mutex> lock(another.m_Block);
        m_pRoot     = std::exchange(another.m_pRoot, nullptr);
        m_pLast     = std::exchange(another.m_pLast, nullptr);
        m_nElements = std::exchange(another.m_nElements, 0);
    }

    void push_back(const value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    
    size_t getSize() const { return m_nElements; }

    // --- ITERADORES ---
    
    // Forward Begin/End (Recorrido Normal: Root -> Last)
    forward_iterator begin() { 
        return forward_iterator(m_pRoot, m_pRoot); 
    }
    forward_iterator end() { 
        return forward_iterator(nullptr, m_pRoot); 
    }

    // Reverse Begin/End (Recorrido Inverso: Last -> Root)
    // Para usar Foreach al revés, pasas rbegin() y rend()
    backward_iterator rbegin() {
        return backward_iterator(m_pLast, m_pLast);
    }
    backward_iterator rend() {
        return backward_iterator(nullptr, m_pLast);
    }

    // --- FUNCIONES INTERNAS ---

    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        std::lock_guard<std::mutex> lock(m_Block);
        ::Foreach(*this, of, args...);
    }

    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args){
        std::lock_guard<std::mutex> lock(m_Block);
        return ::FirstThat(*this, of, args...);
    }

    // Operator << (Imprimir)
    friend ostream &operator<<(ostream &os, CDoubleLinkedListCircular<Traits> &container){
        std::lock_guard<std::mutex> lock(container.m_Block);
        os << "DoubleCircularList: size = " << container.m_nElements << endl;
        os << "[";
        if (container.m_pRoot) {
            Node *pTemp = container.m_pRoot;
            do {
                os << "(" << pTemp->GetValue() << "|" << pTemp->GetRef() << ")";
                pTemp = pTemp->GetNext();
                if (pTemp != container.m_pRoot) {
                    os << " <-> "; // Indicador de doble enlace
                }
            } while (pTemp != container.m_pRoot);
        }
        os << "]" << endl;
        return os;
    }

    // Operator []
    value_type &operator[](size_t index) {
        std::lock_guard<std::mutex> lock(m_Block);
        Node *pTemp = m_pRoot;
        for(size_t i = 0; i < index && pTemp; ++i) {
            pTemp = pTemp->GetNext();
        }
        return pTemp->GetValueRef();
    }
};

// --- IMPLEMENTACIONES ---

template <typename Traits>
void CDoubleLinkedListCircular<Traits>::push_back(const value_type &val, ref_type ref) {
    std::lock_guard<std::mutex> lock(m_Block);
    Node *pNew = new Node(val, ref);

    if (!m_pRoot) {
        m_pRoot = pNew;
        m_pLast = pNew;
        // Circularidad Doble: se apunta a sí mismo por ambos lados
        pNew->GetNextRef() = m_pRoot;
        pNew->GetPrevRef() = m_pRoot;
    } else {
        // Enlazar al final
        pNew->GetPrevRef()    = m_pLast; // Nuevo apunta atrás al viejo último
        pNew->GetNextRef()    = m_pRoot; // Nuevo apunta adelante al Root (cierra círculo)
        
        m_pLast->GetNextRef() = pNew;    // Viejo último apunta al nuevo
        m_pRoot->GetPrevRef() = pNew;    // Root apunta atrás al nuevo (cierra círculo inverso)
        
        m_pLast = pNew; // Actualizamos Last
    }
    ++m_nElements;
}

template <typename Traits>
void CDoubleLinkedListCircular<Traits>::Insert(const value_type &val, ref_type ref) {
    std::lock_guard<std::mutex> lock(m_Block);
    Node *pNew = new Node(val, ref);

    // Caso 1: Vacía
    if (!m_pRoot) {
        m_pRoot = pNew;
        m_pLast = pNew;
        pNew->GetNextRef() = m_pRoot;
        pNew->GetPrevRef() = m_pRoot;
        ++m_nElements;
        return;
    }

    // Caso 2: Insertar al Inicio (val < Root)
    if (m_pRoot->GetValue() > val) {
        pNew->GetNextRef() = m_pRoot;
        pNew->GetPrevRef() = m_pLast; // Se engancha al último
        
        m_pRoot->GetPrevRef() = pNew; // El viejo root se engancha al nuevo
        m_pLast->GetNextRef() = pNew; // El último se engancha al nuevo (Cierre circular)
        
        m_pRoot = pNew; // Cambiamos el líder
        ++m_nElements;
        return;
    }

    // Caso 3: Insertar Medio/Final
    Node *pTemp = m_pRoot;
    // Buscamos sin dar vuelta completa (paramos en m_pLast)
    while (pTemp != m_pLast && pTemp->GetNext()->GetValue() < val) {
        pTemp = pTemp->GetNext();
    }

    // pTemp es el nodo PREVIO a donde queremos insertar
    // Insertamos entre pTemp y pTemp->Next
    
    Node *pNextNode = pTemp->GetNext(); // Puede ser Root (si pTemp es Last) o un nodo medio

    // Conexiones del Nuevo
    pNew->GetPrevRef() = pTemp;
    pNew->GetNextRef() = pNextNode;

    // Conexiones de los vecinos
    pTemp->GetNextRef()     = pNew;
    pNextNode->GetPrevRef() = pNew;

    // Si insertamos justo al final (después de Last), actualizamos Last
    if (pTemp == m_pLast) {
        m_pLast = pNew;
    }
    
    ++m_nElements;
}

#endif // __DOUBLELINKEDLISTCIRCULAR_H__