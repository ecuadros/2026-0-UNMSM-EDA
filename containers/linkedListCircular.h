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
    public:
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
        return Forward_iterator(m_pRoot, m_pRoot); 
    }

    // end(): 
    Forward_iterator end() { 
        return Forward_iterator(nullptr, m_pRoot); 
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

    // Operator << 
    friend ostream &operator<<(ostream &os, CLinkedListCircular<Traits> &container){
        std::lock_guard<std::mutex> lock(container.m_Block);
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

    // Operator >> 
    friend istream &operator>>(istream &is, CLinkedListCircular<Traits> &container) {
        value_type val;
        ref_type   ref;
        is >> val >> ref; 
        container.Insert(val, ref); 
        return is;
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
template <typename Traits>
void CLinkedListCircular<Traits>::push_back(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_Block);
    Node *pNew = new Node(val, ref);
    
    if (!m_pRoot) {
        m_pRoot = pNew;
        pNew->GetNextRef() = m_pRoot; 
    } else {
        m_pLast->GetNextRef() = pNew; 
        pNew->GetNextRef() = m_pRoot; 
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
    if (compare(m_pRoot->GetValue(), val)) {
        pNew->GetNextRef() = m_pRoot;
        m_pRoot = pNew;
        m_pLast->GetNextRef() = m_pRoot; 
        ++m_nElements;
        return;
    }

    Node *pTemp = m_pRoot;
    while (pTemp != m_pLast && !compare(pTemp->GetNext()->GetValue(), val)) {
        pTemp = pTemp->GetNext();
    }

    pNew->GetNextRef() = pTemp->GetNext();
    pTemp->GetNextRef() = pNew;
    if (pTemp == m_pLast) {
        m_pLast = pNew;
    }
    ++m_nElements;
}



#endif