#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include <mutex>
#include <stdexcept>
#include "../general/types.h"
#include "../util.h"
using namespace std;

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

template <typename NodePtr>
class LinkedListIterator {
    NodePtr m_pCurrent;
public:
    LinkedListIterator(NodePtr pNode) : m_pCurrent(pNode) {}

    auto& operator*() { return m_pCurrent->GetValueRef(); }

    LinkedListIterator& operator++() {
        if (m_pCurrent) m_pCurrent = m_pCurrent->GetNext();
        return *this;
    }

    bool operator!=(const LinkedListIterator& other) const {
        return m_pCurrent != other.m_pCurrent;
    }
};


template <typename Traits>
class NodeLinkedList{

    using  value_type  = typename Traits::value_type;
    using  Node        = NodeLinkedList<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node* m_pNext = nullptr;

public:
    NodeLinkedList(){}
    NodeLinkedList( value_type _value, ref_type _ref = -1, Node * _next = nullptr)
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
    using  value_type  = typename Traits::value_type;
    // using  forward_iterator  = LinkedListForwardIterator < CLinkedList<Traits> >;
    // friend forward_iterator;
    // friend GeneralIterator< CLinkedList<Traits> >;
protected:
    using  Node = NodeLinkedList<Traits>;
    mutable std::mutex m_mtx;
    
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;

public:
    CLinkedList() = default;
    
template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args) {
        return ::FirstThat(*this, of, args...);
    }

template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args) {
        ::Foreach(*this, of, args...);
    }

value_type& operator[](ref_type ref) {
    Node* pTmp = m_pRoot;
    while (pTmp != nullptr) {
        if (pTmp->GetRef() == ref) {
            return const_cast<value_type&>(pTmp->GetValueRef());
        }
        pTmp = pTmp->GetNext();
    }
    throw std::out_of_range("Referencia no encontrada");
}

    // DESTRUCTOR 
    ~CLinkedList() {
        std::lock_guard<std::mutex> lock(m_mtx); // BLOQUEAMO
        Node* pTmp = m_pRoot;
        while (pTmp != nullptr) {
            Node* pNext = pTmp->GetNext();
            delete pTmp;
            pTmp = pNext;
        }
        m_pRoot = m_pLast = nullptr;
        m_nElements = 0;
    }

    // CONSTRUCTOR DE COPIA 
    CLinkedList(const CLinkedList& other) {
        std::lock_guard<std::mutex> lock(other.m_mtx); // BLOQUEAMOS
        m_pLast = nullptr;
        m_nElements = 0;

        Node* pTmp = other.m_pRoot;
        while(pTmp) {
            this->Insert(pTmp->GetValue(), pTmp->GetRef());
            pTmp = pTmp->GetNext();
        }
    }
    // CONSTRUCTOR DE MOVE
    CLinkedList(CLinkedList&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.m_mtx); // Bloqueamos para "robar"
        this->m_pRoot = other.m_pRoot;
        this->m_pLast = other.m_pLast;
        this->m_nElements = other.m_nElements;

        // Dejamos al otro en un estado limpio
        other.m_pRoot = nullptr;
        other.m_pLast = nullptr;
        other.m_nElements = 0;
    }
        // FOREACH
    using iterator = LinkedListIterator<Node*>;
    iterator begin() { 
        return iterator(m_pRoot); 
    }
    iterator end() { 
        return iterator(nullptr); 
    }

    void push_back(value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    
    size_t getSize(){ 
        std::lock_guard<std::mutex> lock(m_mtx);
        return m_nElements;  }

protected:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);

    // SALIDA
    friend ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
      os << "CLinkedList: size = " << container.m_nElements << endl;
      os << "[";
    Node* pTmp = container.m_pRoot;
    while(pTmp != nullptr){
        os << "(" << pTmp->GetValue() << ":" << pTmp->GetRef() << ") -> ";
        pTmp = pTmp->GetNext(); 
    }
    os << "NULL]" << endl;
    return os;
}
};

    // PEDIR ELEMENTOS
    template <typename Traits>
    istream &operator>>(istream &is, CLinkedList<Traits> &container) {
    typename Traits :: value_type val;
    ref_type ref;
    char Tc;

    if (is >> Tc && Tc == '(') {
        if (is >> val >> Tc && Tc == ':') {
            if (is >> ref >> Tc && Tc == ')') {
                container.Insert(val, ref);
            }
        }
    }
    return is;
}

template <typename Traits>
void CLinkedList<Traits>::push_back(value_type &val, ref_type ref){
    Node *pNewNode = new Node(val, ref);
    if( !m_pRoot )
        m_pRoot = pNewNode;
    else {m_pLast->GetNextRef() = pNewNode;}
    m_pLast = pNewNode;
    ++m_nElements;
}

template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    if( !rParent || rParent->GetValue() > val ){
        //cout << "Insertando " << val << " antes de " << (rParent ? to_string(rParent->GetValue()) : "NULL") << endl;
        Node *pNew = new Node(val, ref, rParent);
        rParent = pNew;
        ++m_nElements;
        return;
    }
    InternalInsert(rParent->GetNextRef(), val, ref);
}

template <typename Traits>
void CLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mtx);
    InternalInsert(m_pRoot, val, ref);
}

#endif // __LINKEDLIST_H__
