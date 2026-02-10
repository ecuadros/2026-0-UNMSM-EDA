#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include <mutex>
#include <fstream>
#include <utility>
#include "../general/types.h"
#include "../util.h"
#include "GeneralIterator.h"
using namespace std;

// TODO: Traits para listas enlazadas
template <typename T, typename _Func>
struct ListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingTrait : 
    public ListTrait<T, std::greater<T> >{
};

template <typename T>
struct DescendingTrait : 
    public ListTrait<T, std::less<T> >{
};

// Iterators para listas enlazadas

template <typename Traits>
class NodeLinkedList{

    using  value_type  = typename Traits::value_type;
    using  Node        = NodeLinkedList<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;

public:
    NodeLinkedList(){}
    NodeLinkedList(value_type _value, ref_type _ref = -1, Node* _next = nullptr)
    : m_data(_value), m_ref(_ref), m_pNext(_next) { }
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

template <typename Traits>
class CLinkedList {
public:
    using  value_type  = typename Traits::value_type;
    using forward_iterator = LinkedListForwardIterator<CLinkedList<Traits>>;
    using  Node = NodeLinkedList<Traits>;

    friend forward_iterator;
    friend class GeneralIterator<CLinkedList<Traits>>;

protected:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable recursive_mutex m_mutex;

public:
    CLinkedList() : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0) {}
    // TODO: Constructor copia
    CLinkedList(const CLinkedList<Traits>& another){
        lock_guard<recursive_mutex> lock(another.m_mutex);
        Node* pTemp = another.m_pRoot;
        while (pTemp) {
            this->Insert(pTemp->GetValue(), pTemp->GetRef());
            pTemp = pTemp->GetNext();
        }
    }

    template <typename ObjFunc, typename... Args>
    void Foreach(ObjFunc of, Args... args) {
        lock_guard<recursive_mutex> lock(m_mutex); // Requisito de Concurrencia
        Node* pTemp = m_pRoot;
        while (pTemp) {
            of(pTemp->GetValueRef(), args...);
            pTemp = pTemp->GetNext();
        }
    }

    // TODO: Move Constructor
    CLinkedList(CLinkedList<Traits>&& another) noexcept {
        lock_guard<recursive_mutex> lock(another.m_mutex);
        m_pRoot = exchange(another.m_pRoot, nullptr);
        m_pLast = exchange(another.m_pLast, nullptr);
        m_nElements = exchange(another.m_nElements, 0);
    }
    // TODO: Destructor seguro y virtual
    virtual ~CLinkedList() {
        Node* pTemp;
        while (m_pRoot && (m_pRoot != m_pLast->GetNext())) { // Safe para LE y LEC
            pTemp = m_pRoot;
            m_pRoot = m_pRoot->GetNext();
            delete pTemp;
            if (m_pRoot == nullptr) break;
        }
    }
    // TODO: Iterators begin() end()
    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }
    // TODO: Operadores de acceso []
    value_type& operator[](size_t index) {
        lock_guard<recursive_mutex> lock(m_mutex);
        Node* pTemp = m_pRoot;
        for (size_t i = 0; i < index && pTemp; ++i) pTemp = pTemp->GetNext();
        return pTemp->GetValueRef();
    }
    void push_back(value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    size_t getSize(){
        lock_guard<recursive_mutex> lock(m_mutex); 
        return m_nElements;  }

private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);

    // TODO: Persistencia (write)
    friend ostream& operator<<(ostream& os, CLinkedList<Traits>& container) {
        lock_guard<recursive_mutex> lock(container.m_mutex);
        os << "CLinkedList: size = " << container.m_nElements << endl;
        os << "[";
        Node* pTemp = container.m_pRoot;
        for (size_t i = 0; i < container.m_nElements; ++i) {
            os << "(" << pTemp->GetValue() << ":" << pTemp->GetRef() << ")";
            if (i < container.m_nElements - 1) os << ", ";
            pTemp = pTemp->GetNext();
        }
        os << "]" << endl;
        return os;
    }
    // TODO: Persistencia (read)
    friend istream& operator>>(istream& is, CLinkedList<Traits>& container) {
        value_type val;
        ref_type ref;
        char ignore;
        // Formato esperado: (valor:ref)
        while (is >> ignore >> val >> ignore >> ref >> ignore) {
            container.Insert(val, ref);
        }
        return is;
    }
};

template <typename Traits>
void CLinkedList<Traits>::push_back(value_type& val, ref_type ref) {
    lock_guard<recursive_mutex> lock(m_mutex);
    Node* pNewNode = new Node(val, ref);
    if (!m_pRoot) {
        m_pRoot = pNewNode;
    } else {
        m_pLast->GetNextRef() = pNewNode;
    }
    m_pLast = pNewNode;
    ++m_nElements;
}

template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node*& rParent, const value_type& val, ref_type ref) {
    // Caso base o inserción ordenada por valor
    if (!rParent || rParent->GetValue() > val) {
        Node* pNew = new Node(val, ref, rParent);
        rParent = pNew;
        if (pNew->GetNext() == nullptr) m_pLast = pNew;
        ++m_nElements;
        return;
    }
    InternalInsert(rParent->GetNextRef(), val, ref);
}

template <typename Traits>
void CLinkedList<Traits>::Insert(const value_type& val, ref_type ref) {
    lock_guard<recursive_mutex> lock(m_mutex);
    InternalInsert(m_pRoot, val, ref);
}

template <typename Traits>
class CCircularLinkedList : public CLinkedList<Traits> {
public:
    void MakeCircular() {
        lock_guard<recursive_mutex> lock(this->m_mutex);
        if (this->m_pLast && this->m_pRoot) {
            this->m_pLast->GetNextRef() = this->m_pRoot;
        }
    }
};

#endif // __LINKEDLIST_H__
