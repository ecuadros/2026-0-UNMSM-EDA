#ifndef __DOUBLELINKEDLIST_H__
#define __DOUBLELINKEDLIST_H__

#include <iostream>
#include <functional>
#include <stdexcept>
#include <iomanip>
#include <type_traits>
#include <utility>
#include <string>

#include "../general/types.h"
#include "../util.h"
#include "list_traits.h"
using namespace std;

// ==========================
// Nodo para LDE
// ==========================
template <typename Traits>
class NodeDoubleLinkedList{
    using value_type = typename Traits::value_type;
    using Node       = NodeDoubleLinkedList<Traits>;

private:
    value_type m_data{};
    ref_type   m_ref{-1};
    Node      *m_pNext = nullptr;
    Node      *m_pPrev = nullptr;

public:
    NodeDoubleLinkedList(){}
    NodeDoubleLinkedList(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){}

    value_type GetValue() const { return m_data; }

    // IMPORTANTE: 2 overloads
    value_type       &GetValueRef()       { return m_data; }
    const value_type &GetValueRef() const { return m_data; }

    ref_type  GetRef() const { return m_ref; }
    ref_type &GetRefRef()    { return m_ref; }

    Node       *GetNext()       { return m_pNext; }
    const Node *GetNext() const { return m_pNext; }
    Node      *&GetNextRef()    { return m_pNext; }

    Node       *GetPrev()       { return m_pPrev; }
    const Node *GetPrev() const { return m_pPrev; }
    Node      *&GetPrevRef()    { return m_pPrev; }

    Node &operator=(const Node &another){
        m_data = another.GetValue();
        m_ref  = another.GetRef();
        return *this;
    }

    bool operator==(const Node &another) const
    { return m_data == another.GetValue(); }

    bool operator<(const Node &another) const
    { return m_data < another.GetValue(); }
};

// ==========================
// Iterators para LDE
//  - Forward: ++ usa Next
//  - Backward: ++ usa Prev (para rbegin/rend)
// ==========================
template <typename Container>
class DoubleLinkedListIterator {
public:
    using raw_container = std::remove_const_t<Container>;
    using value_type    = typename raw_container::value_type;
    using Node          = typename raw_container::Node;

    using node_ptr   = std::conditional_t<std::is_const_v<Container>, const Node*, Node*>;
    using reference  = std::conditional_t<std::is_const_v<Container>, const value_type&, value_type&>;

protected:
    Container *m_pContainer = nullptr;
    node_ptr   m_pNode      = nullptr;

public:
    DoubleLinkedListIterator(Container *pContainer=nullptr, node_ptr pNode=nullptr)
        : m_pContainer(pContainer), m_pNode(pNode) {}

    reference operator*() const { return m_pNode->GetValueRef(); }

    bool operator!=(const DoubleLinkedListIterator &other) const { return m_pNode != other.m_pNode; }
    bool operator==(const DoubleLinkedListIterator &other) const { return m_pNode == other.m_pNode; }
};

template <typename Container>
class DoubleLinkedListForwardIterator : public DoubleLinkedListIterator<Container>{
    using Parent = DoubleLinkedListIterator<Container>;
public:
    using Node = typename Parent::Node;

    DoubleLinkedListForwardIterator(Container *pContainer=nullptr, Node *pNode=nullptr)
        : Parent(pContainer, pNode) {}

    DoubleLinkedListForwardIterator &operator++(){
        if(Parent::m_pNode){
            Parent::m_pNode = Parent::m_pNode->GetNext();
        }
        return *this;
    }
};

template <typename Container>
class DoubleLinkedListBackwardIterator : public DoubleLinkedListIterator<Container>{
    using Parent = DoubleLinkedListIterator<Container>;
public:
    using Node = typename Parent::Node;

    DoubleLinkedListBackwardIterator(Container *pContainer=nullptr, Node *pNode=nullptr)
        : Parent(pContainer, pNode) {}

    // Nota: ++ va hacia atras (Prev), porque se usa con rbegin()/rend()
    DoubleLinkedListBackwardIterator &operator++(){
        if(Parent::m_pNode){
            Parent::m_pNode = Parent::m_pNode->GetPrev();
        }
        return *this;
    }
};

// ==========================
// Clase LDE
// ==========================
template <typename Traits>
class CDoubleLinkedList{
public:
    using value_type = typename Traits::value_type;
    using Node       = NodeDoubleLinkedList<Traits>;

    using forward_iterator       = DoubleLinkedListForwardIterator< CDoubleLinkedList<Traits> >;
    using const_forward_iterator = DoubleLinkedListForwardIterator< const CDoubleLinkedList<Traits> >;

    using backward_iterator       = DoubleLinkedListBackwardIterator< CDoubleLinkedList<Traits> >;
    using const_backward_iterator = DoubleLinkedListBackwardIterator< const CDoubleLinkedList<Traits> >;

private:
    Node   *m_pRoot = nullptr;
    Node   *m_pLast = nullptr;
    size_t  m_nElements = 0;

public:
    CDoubleLinkedList(){}
    CDoubleLinkedList(const CDoubleLinkedList&);
    CDoubleLinkedList(CDoubleLinkedList&&) noexcept;
    virtual ~CDoubleLinkedList();

    void clear();

    CDoubleLinkedList& operator=(const CDoubleLinkedList&);
    CDoubleLinkedList& operator=(CDoubleLinkedList&& other) noexcept;

    value_type& operator[](size_t index);
    const value_type& operator[](size_t index) const;

    void push_back(const value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);

    size_t getSize() const { return m_nElements; }

    // Forward iterator (begin, end)
    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }

    const_forward_iterator begin() const { return const_forward_iterator(this, m_pRoot); }
    const_forward_iterator end()   const { return const_forward_iterator(this, nullptr); }

    // Backward iterator (rbegin, rend)
    backward_iterator rbegin() { return backward_iterator(this, m_pLast); }
    backward_iterator rend()   { return backward_iterator(this, nullptr); }

    const_backward_iterator rbegin() const { return const_backward_iterator(this, m_pLast); }
    const_backward_iterator rend()   const { return const_backward_iterator(this, nullptr); }

    // ForEach (azul en Branch 27)
    template <typename ObjFunc, typename... Args>
    void Foreach(ObjFunc of, Args... args);

    // FirstThat (azul en Branch 27)
    template <typename ObjFunc, typename... Args>
    auto FirstThat(ObjFunc of, Args... args){
        return ::FirstThat(*this, of, args...);
    }

private:
    void InsertBefore(Node *pos, Node *pNew);
    void InsertAfter(Node *pos, Node *pNew);

    // Persistencia (write)
    friend ostream &operator<<(ostream &os, const CDoubleLinkedList<Traits> &container){
        os << "[";
        Node *p = container.m_pRoot;
        while(p != nullptr){
            os << "(" << p->GetValue() << ":" << p->GetRef() << ")";
            p = p->GetNext();
        }
        os << "]" << endl;
        return os;
    }

    // Persistencia (read)
    template <typename T>
    friend istream &operator>>(istream &is, CDoubleLinkedList<T> &container);
};

// ==========================
// Implementación (inline)
// ==========================
template <typename Traits>
void CDoubleLinkedList<Traits>::push_back(const value_type &val, ref_type ref){
    Node *pNew = new Node(val, ref);

    if(!m_pRoot){
        m_pRoot = m_pLast = pNew;
    }else{
        pNew->GetPrevRef() = m_pLast;
        m_pLast->GetNextRef() = pNew;
        m_pLast = pNew;
    }
    ++m_nElements;
}

template <typename Traits>
void CDoubleLinkedList<Traits>::InsertBefore(Node *pos, Node *pNew){
    // insertar antes de pos (pos != nullptr)
    Node *prev = pos->GetPrev();

    pNew->GetNextRef() = pos;
    pNew->GetPrevRef() = prev;

    pos->GetPrevRef() = pNew;

    if(prev) prev->GetNextRef() = pNew;
    else     m_pRoot = pNew;

    ++m_nElements;
}

template <typename Traits>
void CDoubleLinkedList<Traits>::InsertAfter(Node *pos, Node *pNew){
    // insertar después de pos (pos != nullptr)
    Node *next = pos->GetNext();

    pNew->GetPrevRef() = pos;
    pNew->GetNextRef() = next;

    pos->GetNextRef() = pNew;

    if(next) next->GetPrevRef() = pNew;
    else     m_pLast = pNew;

    ++m_nElements;
}

template <typename Traits>
void CDoubleLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    typename Traits::Func cmp;

    // caso vacío
    if(!m_pRoot){
        push_back(val, ref);
        return;
    }

    // si va antes del root
    if(cmp(m_pRoot->GetValue(), val)){
        Node *pNew = new Node(val, ref);
        InsertBefore(m_pRoot, pNew);
        return;
    }

    // buscar posición
    Node *cur = m_pRoot->GetNext();
    while(cur != nullptr && !cmp(cur->GetValue(), val)){
        cur = cur->GetNext();
    }

    // si llegamos al final, insertar al final
    if(cur == nullptr){
        push_back(val, ref);
        return;
    }

    // insertar antes de cur
    Node *pNew = new Node(val, ref);
    InsertBefore(cur, pNew);
}

template <typename Traits>
void CDoubleLinkedList<Traits>::clear(){
    Node *p = m_pRoot;
    while(p != nullptr){
        Node *nxt = p->GetNext();
        delete p;
        p = nxt;
    }
    m_pRoot = nullptr;
    m_pLast = nullptr;
    m_nElements = 0;
}

template <typename Traits>
CDoubleLinkedList<Traits>::~CDoubleLinkedList(){
    clear();
}

template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList(const CDoubleLinkedList &other)
    : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0)
{
    Node *p = other.m_pRoot;
    while(p != nullptr){
        push_back(p->GetValue(), p->GetRef());
        p = p->GetNext();
    }
}

template <typename Traits>
CDoubleLinkedList<Traits>& CDoubleLinkedList<Traits>::operator=(const CDoubleLinkedList &other){
    if(this == &other) return *this;
    clear();

    Node *p = other.m_pRoot;
    while(p != nullptr){
        push_back(p->GetValue(), p->GetRef());
        p = p->GetNext();
    }
    return *this;
}

template <typename Traits>
CDoubleLinkedList<Traits>::CDoubleLinkedList(CDoubleLinkedList &&other) noexcept
    : m_pRoot(other.m_pRoot), m_pLast(other.m_pLast), m_nElements(other.m_nElements)
{
    other.m_pRoot = nullptr;
    other.m_pLast = nullptr;
    other.m_nElements = 0;
}

template <typename Traits>
CDoubleLinkedList<Traits>& CDoubleLinkedList<Traits>::operator=(CDoubleLinkedList&& other) noexcept{
    if(this == &other) return *this;

    clear();

    m_pRoot = other.m_pRoot;
    m_pLast = other.m_pLast;
    m_nElements = other.m_nElements;

    other.m_pRoot = nullptr;
    other.m_pLast = nullptr;
    other.m_nElements = 0;

    return *this;
}

template <typename Traits>
typename CDoubleLinkedList<Traits>::value_type&
CDoubleLinkedList<Traits>::operator[](size_t index){
    if(index >= m_nElements)
        throw std::out_of_range("Index out of range");

    Node *p = m_pRoot;
    for(size_t i=0; i<index; ++i) p = p->GetNext();
    return p->GetValueRef();
}

template <typename Traits>
const typename CDoubleLinkedList<Traits>::value_type&
CDoubleLinkedList<Traits>::operator[](size_t index) const{
    if(index >= m_nElements)
        throw std::out_of_range("Index out of range");

    Node *p = m_pRoot;
    for(size_t i=0; i<index; ++i) p = p->GetNext();
    return p->GetValueRef();
}

template <typename Traits>
template <typename ObjFunc, typename... Args>
void CDoubleLinkedList<Traits>::Foreach(ObjFunc of, Args... args){
    Node *p = m_pRoot;
    while(p != nullptr){
        of(p->GetValueRef(), args...);
        p = p->GetNext();
    }
}

template <typename Traits>
istream &operator>>(istream &is, CDoubleLinkedList<Traits> &container){
    using value_type = typename CDoubleLinkedList<Traits>::value_type;

    if(!is) return is;

    CDoubleLinkedList<Traits> tmp;

    char ch;
    do{
        if(!is.get(ch)) return is;
    }while(ch != '[');

    while(true){
        is >> ws;

        int pk = is.peek();
        if(pk == EOF){
            is.setstate(ios::failbit);
            return is;
        }

        if(static_cast<char>(pk) == ']'){
            is.get(ch);
            break;
        }

        is >> ch;
        if(!is || ch != '('){
            is.setstate(ios::failbit);
            return is;
        }

        value_type val{};
        ref_type ref{};

        if constexpr (std::is_same_v<value_type, std::string>){
            is >> std::quoted(val);
        }else{
            is >> val;
        }

        is >> ch;
        if(!is || ch != ':'){
            is.setstate(ios::failbit);
            return is;
        }

        is >> ref;

        is >> ch;
        if(!is || ch != ')'){
            is.setstate(ios::failbit);
            return is;
        }

        tmp.Insert(val, ref);
    }

    container = std::move(tmp);
    return is;
}

#endif // __DOUBLELINKEDLIST_H__
