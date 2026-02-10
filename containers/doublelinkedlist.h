#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__

#include <iostream>
#include <mutex>
#include <utility>
#include "../general/types.h"
#include "../util.h"
#include "GeneralIterator.h"

using namespace std;

template <typename Traits>
class NodeDoubleLinkedList {
    using value_type = typename Traits::value_type;
    using Node = NodeDoubleLinkedList<Traits>;
private:
    value_type m_data;
    ref_type m_ref;
    Node* m_pNext = nullptr;
    Node* m_pPrev = nullptr; // Puntero al nodo anterior

public:
    NodeDoubleLinkedList(value_type _v, ref_type _r = -1, Node* _n = nullptr, Node* _p = nullptr)
        : m_data(_v), m_ref(_r), m_pNext(_n), m_pPrev(_p) {}

    value_type& GetValueRef() { return m_data; }
    value_type  GetValue() const { return m_data; }
    ref_type    GetRef() const { return m_ref; }
    
    Node*& GetNextRef() { return m_pNext; }
    Node* GetNext() const { return m_pNext; }
    Node*& GetPrevRef() { return m_pPrev; }
    Node* GetPrev() const { return m_pPrev; }
};

// TODO: ForwardIterator para listas doblemente enlazadas
// TODO: BackwardIterator para listas doblemente enlazadas

template <typename Container>
class DoubleLinkedListIterator : public GeneralIterator<Container> {
public:
    using Parent = GeneralIterator<Container>;
    using Node = typename Container::Node;

    DoubleLinkedListIterator(Container* pCont, Node* pNode) : Parent(pCont, pNode) {}

    // Movimiento hacia adelante
    DoubleLinkedListIterator& operator++() {
        if (this->m_pCurrent) this->m_pCurrent = this->m_pCurrent->GetNext();
        return *this;
    }

    // Movimiento hacia atrás (Backward)
    DoubleLinkedListIterator& operator--() {
        if (this->m_pCurrent) this->m_pCurrent = this->m_pCurrent->GetPrev();
        return *this;
    }

    bool operator!=(const DoubleLinkedListIterator& other) const {
        return this->m_pCurrent != other.m_pCurrent;
    }

    typename Container::value_type& operator*() {
        return this->m_pCurrent->GetValueRef();
    }
};
template <typename Traits>
class CDoubleLinkedList {
public:
    using value_type = typename Traits::value_type;
    using Node = NodeDoubleLinkedList<Traits>;
    using iterator = DoubleLinkedListIterator<CDoubleLinkedList<Traits>>;

protected:
    Node* m_pRoot = nullptr;
    Node* m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable recursive_mutex m_mutex; // Concurrencia

public:
    template <typename ObjFunc, typename... Args>
    auto FirstThat(ObjFunc of, Args... args) {
        lock_guard<recursive_mutex> lock(m_mutex);
        for (auto it = this->begin(); it != this->end(); ++it) {
            if (of(*it, args...)) return it;
        }
        return this->end();
    }
    CDoubleLinkedList() = default;

    virtual ~CDoubleLinkedList() {
        lock_guard<recursive_mutex> lock(m_mutex);
        Node* pTemp;
        while (m_pRoot && m_nElements > 0) {
            pTemp = m_pRoot;
            m_pRoot = m_pRoot->GetNext();
            delete pTemp;
            m_nElements--;
        }
    }

    value_type& operator[](size_t index) {
        lock_guard<recursive_mutex> lock(m_mutex);
        Node* pTemp = m_pRoot;
        for (size_t i = 0; i < index && pTemp; ++i) pTemp = pTemp->GetNextRef();
        return pTemp->GetValueRef();
    }

    template <typename ObjFunc, typename... Args>
    void Foreach(ObjFunc of, Args... args) {
        lock_guard<recursive_mutex> lock(m_mutex);
        for (auto it = this->begin(); it != this->end(); ++it) {
            of(*it, args...);
        }
    }

    iterator begin() { return iterator(this, m_pRoot); }
    iterator end()   { return iterator(this, nullptr); }
    iterator rbegin() { return iterator(this, m_pLast); }

    void Insert(const value_type& val, ref_type ref) {
        lock_guard<recursive_mutex> lock(m_mutex);
        if (!m_pRoot || m_pRoot->GetValue() > val) {
            Node* pNew = new Node(val, ref, m_pRoot, nullptr);
            if (m_pRoot) m_pRoot->GetPrevRef() = pNew;
            m_pRoot = pNew;
            if (!m_pLast) m_pLast = pNew;
        } else {
            Node* pTemp = m_pRoot;
            while (pTemp->GetNext() && pTemp->GetNext()->GetValue() < val) {
                pTemp = pTemp->GetNext();
            }
            Node* pNew = new Node(val, ref, pTemp->GetNext(), pTemp);
            if (pTemp->GetNext()) pTemp->GetNext()->GetPrevRef() = pNew;
            else m_pLast = pNew;
            pTemp->GetNextRef() = pNew;
        }
        m_nElements++;
    } 

    friend ostream& operator<<(ostream& os, CDoubleLinkedList& container) {
        lock_guard<recursive_mutex> lock(container.m_mutex);
        os << "DoubleLinkedList: " << container.m_nElements << " nodes" << endl;
        
        typename CDoubleLinkedList::Node* pTemp = container.m_pRoot;

        for (size_t i = 0; i < container.m_nElements; ++i) {
            if (pTemp == nullptr) break; 
            os << "[" << pTemp->GetValue() << "] <-> ";
            pTemp = pTemp->GetNext();
        }
    
        os << (container.m_nElements > 0 ? "(Circular)" : "NULL") << endl;
        return os;
        }
};

template <typename Traits>
class CDoubleCircularLinkedList : public CDoubleLinkedList<Traits> {
public:
    // Primer y último conectados en ambos sentidos
    void MakeCircular() {
        lock_guard<recursive_mutex> lock(this->m_mutex);
        if (this->m_pLast && this->m_pRoot) {
            this->m_pLast->GetNextRef() = this->m_pRoot;
            this->m_pRoot->GetPrevRef() = this->m_pLast;
        }
    }

    // Sobrecarga de destructor para evitar ciclos infinitos
    ~CDoubleCircularLinkedList() {
        if (this->m_pLast) {
            this->m_pLast->GetNextRef() = nullptr;
            this->m_pRoot->GetPrevRef() = nullptr;
        }
    }
};

#endif // __DOUBLE_LINKED_LIST_H__