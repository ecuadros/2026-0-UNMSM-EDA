#ifndef __LINKEDLISTCIRCULAR_H__
#define __LINKEDLISTCIRCULAR_H__
#include <iostream>
#include "../general/types.h"
#include "../util.h"
#include <mutex>
#include <utility>
#include "linkedlist.h"
using namespace std;
template <typename Container>
class CircularForwardIterator{
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

private:
    Node *m_pCurrent; 
    Node *m_pRoot;    
    public:
    // Guardamos el inicio (Root) para detectar la vuelta completa
    CircularForwardIterator(Node *pNode, Node *pRoot) 
        : m_pCurrent(pNode), m_pRoot(pRoot) {}

    bool operator!=(const CircularForwardIterator &other) const {
        return m_pCurrent != other.m_pCurrent;
    }

    CircularForwardIterator &operator++() {
        if (m_pCurrent) {
            m_pCurrent = m_pCurrent->GetNext(); 
            
            if (m_pCurrent == m_pRoot) {
                m_pCurrent = nullptr; 
            }
        }
        return *this;
    }

    value_type &operator*() {
        return m_pCurrent->GetValueRef();
    }
};
template <typename Traits>
class CLinkedListCircular{
    using value_type = typename Traits::value_type;
    using  Forward_iterator=CircularForwardIterator< CLinkedListCircular<Traits> >;
    friend Forward_iterator;
    using  Node = NodeLinkedList<Traits>;
    private:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable std::mutex m_Block;
    public:
    CLinkedListCircular() {}
    virtual ~CLinkedListCircular() {
        if (m_pRoot) {
            Node *pTemp = m_pRoot;
            do {
                Node *pNext = pTemp->GetNext();
                delete pTemp;
                pTemp = pNext;
            } while (pTemp!= m_pRoot); 
        }
        m_pRoot = nullptr;
    }
    CLinkedListCircular(const CLinkedListCircular &another) {
        std::lock_guard<std::mutex> lock(another.m_Block);
        if (another.m_pRoot) {
            Node *pTemp = another.m_pRoot;
            do {
                this->push_back(pTemp->GetValue(), pTemp->GetRef());
                pTemp = pTemp->GetNext();
            } while (pTemp != another.m_pRoot);
        }
    }
    CLinkedListCircular(CLinkedListCircular &&another) noexcept {
        std::lock_guard<std::mutex> lock(another.m_Block);
        m_pRoot     = std::exchange(another.m_pRoot, nullptr);
        m_pLast     = std::exchange(another.m_pLast, nullptr);
        m_nElements = std::exchange(another.m_nElements, 0);
    }
    void push_back(const value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    
    size_t getSize() const { return m_nElements; }

    Forward_iterator begin() { 
        // (Inicio, Marca_De_Tope)
        return Forward_iterator(m_pRoot, m_pRoot); 
    }

    // end(): El final virtual es nullptr
    Forward_iterator end() { 
        return Forward_iterator(nullptr, m_pRoot); 
    }
    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        std::lock_guard<std::mutex> lock(m_Block);
        // Usamos el iterador inteligente, así que ::Foreach funciona sin colgarse
        ::Foreach(*this, of, args...);
    }

    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args){
        std::lock_guard<std::mutex> lock(m_Block);
        return ::FirstThat(*this, of, args...);
    }

    // Operator << (Imprimir)
    friend ostream &operator<<(ostream &os, CLinkedListCircular<Traits> &container){
        std::lock_guard<std::mutex> lock(container.m_Block);
        os << "CircularList: size = " << container.m_nElements << endl;
        os << "[";
        if (container.m_pRoot) {
            Node *pTemp = container.m_pRoot;
            do {
                os << "(" << pTemp->GetValue() << "|" << pTemp->GetRef() << ")";
                pTemp = pTemp->GetNext();
                if (pTemp != container.m_pRoot) {
                    os << ", ";
                }
            } while (pTemp != container.m_pRoot);
        }
        os << "]" << endl;
        return os;
    }

    // Operator >> (Leer)
    friend istream &operator>>(istream &is, CLinkedListCircular<Traits> &container) {
        value_type val;
        ref_type   ref;
        is >> val >> ref; 
        container.Insert(val, ref); // Usamos Insert para mantener orden
        return is;
    } 

    // Operator [] (Acceso aleatorio)
    value_type &operator[](size_t index) {
        std::lock_guard<std::mutex> lock(m_Block);
        Node *pTemp = m_pRoot;
        // Nota: En circular esto podría dar vueltas infinitas si el index es enorme.
        // Limitamos al size o dejamos que de vueltas según lógica (aquí limitamos para seguridad básica)
        for(size_t i = 0; i < index && pTemp; ++i) {
            pTemp = pTemp->GetNext();
        }
        // Si pTemp es null o algo falló, comportamiento indefinido. 
        // Asumimos uso correcto o añadimos checks.
        return pTemp->GetValueRef();
    } 

   
};
template <typename Traits>
void CLinkedListCircular<Traits>::push_back(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_Block);
    Node *pNew = new Node(val, ref);
    
    if (!m_pRoot) {
        m_pRoot = pNew;
        pNew->GetNextRef() = m_pRoot; // Se apunta a sí mismo
    } else {
        m_pLast->GetNextRef() = pNew; // El anterior apunta al nuevo
        pNew->GetNextRef() = m_pRoot; // El nuevo cierra el círculo
    }
    m_pLast = pNew;
    ++m_nElements;
}


 template <typename Traits>
void CLinkedListCircular<Traits>::Insert(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_Block);
    typename Traits::Func compare;
    Node *pNew = new Node(val, ref);

    // Caso 1: Lista vacía
    if (!m_pRoot) {
        m_pRoot = pNew;
        m_pLast = pNew;
        pNew->GetNextRef() = m_pRoot;
        ++m_nElements;
        return;
    }

    // Caso 2: Insertar al inicio (Root cambia)
    if (compare(m_pRoot->GetValue(), val)) {
        pNew->GetNextRef() = m_pRoot;
        m_pRoot = pNew;
        m_pLast->GetNextRef() = m_pRoot; // ¡Crucial! Last apunta al nuevo Root
        ++m_nElements;
        return;
    }

    // Caso 3: Insertar en medio o final
    Node *pTemp = m_pRoot;
    // Buscamos posición, pero paramos si llegamos al Last para no dar vuelta
    while (pTemp != m_pLast && !compare(pTemp->GetNext()->GetValue(), val)) {
        pTemp = pTemp->GetNext();
    }

    // Insertamos
    pNew->GetNextRef() = pTemp->GetNext();
    pTemp->GetNextRef() = pNew;

    // Si insertamos después del Last actual, actualizamos m_pLast
    if (pTemp == m_pLast) {
        m_pLast = pNew;
    }
    ++m_nElements;
}



#endif