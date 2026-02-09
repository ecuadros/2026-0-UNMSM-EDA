#ifndef __DOUBLELINKEDLISTCIRCULAR_H__
#define __DOUBLELINKEDLISTCIRCULAR_H__

#include <iostream>
#include <mutex>
#include <utility> 
#include "linkedlist.h" 
#include "doubleLinkedList.h" 

using namespace std;

template <typename Container>
class CircularDoubleForwardIterator {
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

private:
    Node *m_pCurrent;
    Node *m_pStart;  

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
template <typename Container>
class CircularDoubleBackwardIterator {
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

private:
    Node *m_pCurrent;
    Node *m_pStart;

public:
    CircularDoubleBackwardIterator(Node *pNode, Node *pStart) 
        : m_pCurrent(pNode), m_pStart(pStart) {}

    bool operator!=(const CircularDoubleBackwardIterator &other) const {
        return m_pCurrent != other.m_pCurrent;
    }
    CircularDoubleBackwardIterator &operator++() {
        if (m_pCurrent) {
            m_pCurrent = m_pCurrent->GetPrev(); 
            
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


template <typename Traits>
class CDoubleLinkedListCircular {
public:
    using value_type = typename Traits::value_type;
    using Node       = NodeDoubleLinkedList<Traits>; 
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

    forward_iterator begin() { 
        return forward_iterator(m_pRoot, m_pRoot); 
    }
    forward_iterator end() { 
        return forward_iterator(nullptr, m_pRoot); 
    }

    backward_iterator rbegin() {
        return backward_iterator(m_pLast, m_pLast);
    }
    backward_iterator rend() {
        return backward_iterator(nullptr, m_pLast);
    }

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

    friend ostream &operator<<(ostream &os, CDoubleLinkedListCircular<Traits> &container){
        std::lock_guard<std::mutex> lock(container.m_Block);
        os << "[";
        if (container.m_pRoot) {
            Node *pTemp = container.m_pRoot;
            do {
                os << "(" << pTemp->GetValue() << "|" << pTemp->GetRef() << ")";
                pTemp = pTemp->GetNext();
                if (pTemp != container.m_pRoot) {
                    os << " <-> "; 
                }
            } while (pTemp != container.m_pRoot);
        }
        os << "]" << endl;
        return os;
    }

    value_type &operator[](size_t index) {
        std::lock_guard<std::mutex> lock(m_Block);
        Node *pTemp = m_pRoot;
        for(size_t i = 0; i < index && pTemp; ++i) {
            pTemp = pTemp->GetNext();
        }
        return pTemp->GetValueRef();
    }
};

template <typename Traits>
void CDoubleLinkedListCircular<Traits>::push_back(const value_type &val, ref_type ref) {
    std::lock_guard<std::mutex> lock(m_Block);
    Node *pNew = new Node(val, ref);

    if (!m_pRoot) {
        m_pRoot = pNew;
        m_pLast = pNew;
        pNew->GetNextRef() = m_pRoot;
        pNew->GetPrevRef() = m_pRoot;
    } else {
        // Enlazar al final
        pNew->GetPrevRef()    = m_pLast; 
        pNew->GetNextRef()    = m_pRoot; 
        
        m_pLast->GetNextRef() = pNew;    
        m_pRoot->GetPrevRef() = pNew;    
        
        m_pLast = pNew;
    }
    ++m_nElements;
}

template <typename Traits>
void CDoubleLinkedListCircular<Traits>::Insert(const value_type &val, ref_type ref) {
    std::lock_guard<std::mutex> lock(m_Block);
    typename Traits::Func compare;
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

    if (compare(m_pRoot->GetValue(), val)) {
        pNew->GetNextRef() = m_pRoot;
        pNew->GetPrevRef() = m_pLast; 
        
        m_pRoot->GetPrevRef() = pNew; 
        m_pLast->GetNextRef() = pNew; 
        
        m_pRoot = pNew; 
        ++m_nElements;
        return;
    }

   
    Node *pTemp = m_pRoot;
    while (pTemp !=m_pLast && !compare(pTemp->GetNext()->GetValue(), val)) {
        pTemp = pTemp->GetNext();
    }
    
    Node *pNextNode = pTemp->GetNext(); 

    pNew->GetPrevRef() = pTemp;
    pNew->GetNextRef() = pNextNode;

    pTemp->GetNextRef()     = pNew;
    pNextNode->GetPrevRef() = pNew;


    if (pTemp == m_pLast) {
        m_pLast = pNew;
    }
    
    ++m_nElements;
}

#endif // __DOUBLELINKEDLISTCIRCULAR_H__