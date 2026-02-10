#ifndef __CIRCULARDOUBLELINKEDLIST_H__
#define __CIRCULARDOUBLELINKEDLIST_H__

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
// Nodo: LDE Circular
// ==========================
template <typename Traits>
class NodeCircularDoubleLinkedList{
    using value_type = typename Traits::value_type;
    using Node       = NodeCircularDoubleLinkedList<Traits>;

private:
    value_type m_data{};
    ref_type   m_ref{-1};
    Node      *m_pNext = nullptr;
    Node      *m_pPrev = nullptr;

public:
    NodeCircularDoubleLinkedList(){}
    NodeCircularDoubleLinkedList(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){}

    value_type GetValue() const { return m_data; }

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
};

// ========================================
// Iterators: Circular Doubly
// - end()/rend() se representa con nullptr
// - Forward: ++ va Next y corta al volver a start
// - Backward: ++ va Prev y corta al volver a start
// ========================================
template <typename Container>
class CircularDoubleLinkedListIterator {
public:
    using raw_container = std::remove_const_t<Container>;
    using value_type    = typename raw_container::value_type;
    using Node          = typename raw_container::Node;

    using node_ptr   = std::conditional_t<std::is_const_v<Container>, const Node*, Node*>;
    using reference  = std::conditional_t<std::is_const_v<Container>, const value_type&, value_type&>;

protected:
    Container *m_pContainer = nullptr;
    node_ptr   m_pStart     = nullptr;   // marca inicio del recorrido
    node_ptr   m_pNode      = nullptr;   // nullptr = end/rend

public:
    CircularDoubleLinkedListIterator(Container *pContainer=nullptr, node_ptr pStart=nullptr, node_ptr pNode=nullptr)
        : m_pContainer(pContainer), m_pStart(pStart), m_pNode(pNode) {}

    reference operator*() const { return m_pNode->GetValueRef(); }

    bool operator!=(const CircularDoubleLinkedListIterator &other) const { return m_pNode != other.m_pNode; }
    bool operator==(const CircularDoubleLinkedListIterator &other) const { return m_pNode == other.m_pNode; }
};

template <typename Container>
class CircularDoubleLinkedListForwardIterator : public CircularDoubleLinkedListIterator<Container>{
    using Parent = CircularDoubleLinkedListIterator<Container>;
public:
    using node_ptr = typename Parent::node_ptr;

    CircularDoubleLinkedListForwardIterator(Container *pContainer=nullptr, node_ptr pStart=nullptr, node_ptr pNode=nullptr)
        : Parent(pContainer, pStart, pNode) {}

    CircularDoubleLinkedListForwardIterator &operator++(){
        if(!Parent::m_pNode) return *this;

        node_ptr pNext = Parent::m_pNode->GetNext();
        if(pNext == Parent::m_pStart){
            Parent::m_pNode = nullptr;
        }else{
            Parent::m_pNode = pNext;
        }
        return *this;
    }
};

template <typename Container>
class CircularDoubleLinkedListBackwardIterator : public CircularDoubleLinkedListIterator<Container>{
    using Parent = CircularDoubleLinkedListIterator<Container>;
public:
    using node_ptr = typename Parent::node_ptr;

    CircularDoubleLinkedListBackwardIterator(Container *pContainer=nullptr, node_ptr pStart=nullptr, node_ptr pNode=nullptr)
        : Parent(pContainer, pStart, pNode) {}

    // Nota: ++ va hacia atrás (Prev) para rbegin/rend
    CircularDoubleLinkedListBackwardIterator &operator++(){
        if(!Parent::m_pNode) return *this;

        node_ptr pPrev = Parent::m_pNode->GetPrev();
        if(pPrev == Parent::m_pStart){
            Parent::m_pNode = nullptr;
        }else{
            Parent::m_pNode = pPrev;
        }
        return *this;
    }
};

// ==========================
// Lista Doble Circular
// ==========================
template <typename Traits>
class CCircularDoubleLinkedList{
public:
    using value_type = typename Traits::value_type;
    using Node       = NodeCircularDoubleLinkedList<Traits>;

    using forward_iterator        = CircularDoubleLinkedListForwardIterator< CCircularDoubleLinkedList<Traits> >;
    using const_forward_iterator  = CircularDoubleLinkedListForwardIterator< const CCircularDoubleLinkedList<Traits> >;

    using backward_iterator       = CircularDoubleLinkedListBackwardIterator< CCircularDoubleLinkedList<Traits> >;
    using const_backward_iterator = CircularDoubleLinkedListBackwardIterator< const CCircularDoubleLinkedList<Traits> >;

private:
    Node   *m_pRoot = nullptr;
    Node   *m_pLast = nullptr;
    size_t  m_nElements = 0;

public:
    CCircularDoubleLinkedList(){}
    CCircularDoubleLinkedList(const CCircularDoubleLinkedList&);
    CCircularDoubleLinkedList(CCircularDoubleLinkedList&&) noexcept;
    virtual ~CCircularDoubleLinkedList();

    void clear();

    CCircularDoubleLinkedList& operator=(const CCircularDoubleLinkedList&);
    CCircularDoubleLinkedList& operator=(CCircularDoubleLinkedList&& other) noexcept;

    value_type& operator[](size_t index);
    const value_type& operator[](size_t index) const;

    void push_back(const value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);

    size_t getSize() const { return m_nElements; }

    // Forward (begin, end)
    forward_iterator begin(){
        if(!m_pRoot) return forward_iterator(this, nullptr, nullptr);
        return forward_iterator(this, m_pRoot, m_pRoot);
    }
    forward_iterator end(){
        return forward_iterator(this, m_pRoot, nullptr);
    }

    const_forward_iterator begin() const{
        if(!m_pRoot) return const_forward_iterator(this, nullptr, nullptr);
        return const_forward_iterator(this, m_pRoot, m_pRoot);
    }
    const_forward_iterator end() const{
        return const_forward_iterator(this, m_pRoot, nullptr);
    }

    // Backward (rbegin, rend)
    backward_iterator rbegin(){
        if(!m_pLast) return backward_iterator(this, nullptr, nullptr);
        return backward_iterator(this, m_pLast, m_pLast);
    }
    backward_iterator rend(){
        return backward_iterator(this, m_pLast, nullptr);
    }

    const_backward_iterator rbegin() const{
        if(!m_pLast) return const_backward_iterator(this, nullptr, nullptr);
        return const_backward_iterator(this, m_pLast, m_pLast);
    }
    const_backward_iterator rend() const{
        return const_backward_iterator(this, m_pLast, nullptr);
    }

    // AZUL: ForEach
    template <typename ObjFunc, typename... Args>
    void Foreach(ObjFunc of, Args... args);

    // AZUL: FirstThat (member, variadic)
    template <typename ObjFunc, typename... Args>
    auto FirstThat(ObjFunc of, Args... args){
        for(auto it = begin(); it != end(); ++it){
            if(of(*it, args...)) return it;
        }
        return end();
    }

private:
    void InsertBefore(Node *pos, Node *pNew);
    void InsertAfter (Node *pos, Node *pNew);

    friend ostream &operator<<(ostream &os, const CCircularDoubleLinkedList<Traits> &container){
        os << "[";
        Node *p = container.m_pRoot;
        for(size_t i=0; i < container.m_nElements; ++i){
            os << "(" << p->GetValue() << ":" << p->GetRef() << ")";
            p = p->GetNext();
        }
        os << "]" << endl;
        return os;
    }

    template <typename T>
    friend istream &operator>>(istream &is, CCircularDoubleLinkedList<T> &container);
};

// ==========================
// Implementación
// ==========================
template <typename Traits>
void CCircularDoubleLinkedList<Traits>::push_back(const value_type &val, ref_type ref){
    Node *pNew = new Node(val, ref);

    if(!m_pRoot){
        m_pRoot = m_pLast = pNew;
        pNew->GetNextRef() = pNew;
        pNew->GetPrevRef() = pNew;
    }else{
        // pNew entre last y root
        pNew->GetPrevRef() = m_pLast;
        pNew->GetNextRef() = m_pRoot;

        m_pLast->GetNextRef() = pNew;
        m_pRoot->GetPrevRef() = pNew;

        m_pLast = pNew;
    }
    ++m_nElements;
}

template <typename Traits>
void CCircularDoubleLinkedList<Traits>::InsertBefore(Node *pos, Node *pNew){
    // insertar pNew antes de pos
    Node *prev = pos->GetPrev();

    pNew->GetNextRef() = pos;
    pNew->GetPrevRef() = prev;

    prev->GetNextRef() = pNew;
    pos->GetPrevRef()  = pNew;

    if(pos == m_pRoot) m_pRoot = pNew;

    ++m_nElements;
}

template <typename Traits>
void CCircularDoubleLinkedList<Traits>::InsertAfter(Node *pos, Node *pNew){
    // insertar pNew después de pos
    Node *next = pos->GetNext();

    pNew->GetPrevRef() = pos;
    pNew->GetNextRef() = next;

    pos->GetNextRef() = pNew;
    next->GetPrevRef() = pNew;

    if(pos == m_pLast) m_pLast = pNew;

    ++m_nElements;
}

template <typename Traits>
void CCircularDoubleLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    typename Traits::Func cmp;

    if(!m_pRoot){
        push_back(val, ref);
        return;
    }

    // si va antes del root
    if(cmp(m_pRoot->GetValue(), val)){
        Node *pNew = new Node(val, ref);
        InsertBefore(m_pRoot, pNew);
        // cerrar circularidad root/last ya está mantenida por links
        return;
    }

    // buscar primer cur donde cmp(cur, val) sea true para insertar antes
    Node *cur = m_pRoot->GetNext();
    while(cur != m_pRoot && !cmp(cur->GetValue(), val)){
        cur = cur->GetNext();
    }

    if(cur == m_pRoot){
        // va al final
        push_back(val, ref);
        return;
    }

    Node *pNew = new Node(val, ref);
    InsertBefore(cur, pNew);
}

template <typename Traits>
void CCircularDoubleLinkedList<Traits>::clear(){
    if(!m_pRoot){
        m_pLast = nullptr;
        m_nElements = 0;
        return;
    }

    Node *p = m_pRoot;
    for(size_t i=0; i < m_nElements; ++i){
        Node *nxt = p->GetNext();
        delete p;
        p = nxt;
    }

    m_pRoot = nullptr;
    m_pLast = nullptr;
    m_nElements = 0;
}

template <typename Traits>
CCircularDoubleLinkedList<Traits>::~CCircularDoubleLinkedList(){
    clear();
}

template <typename Traits>
CCircularDoubleLinkedList<Traits>::CCircularDoubleLinkedList(const CCircularDoubleLinkedList &other)
    : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0)
{
    Node *p = other.m_pRoot;
    for(size_t i=0; i < other.m_nElements; ++i){
        push_back(p->GetValue(), p->GetRef());
        p = p->GetNext();
    }
}

template <typename Traits>
CCircularDoubleLinkedList<Traits>&
CCircularDoubleLinkedList<Traits>::operator=(const CCircularDoubleLinkedList &other){
    if(this == &other) return *this;
    clear();

    Node *p = other.m_pRoot;
    for(size_t i=0; i < other.m_nElements; ++i){
        push_back(p->GetValue(), p->GetRef());
        p = p->GetNext();
    }
    return *this;
}

template <typename Traits>
CCircularDoubleLinkedList<Traits>::CCircularDoubleLinkedList(CCircularDoubleLinkedList &&other) noexcept
    : m_pRoot(other.m_pRoot), m_pLast(other.m_pLast), m_nElements(other.m_nElements)
{
    other.m_pRoot = nullptr;
    other.m_pLast = nullptr;
    other.m_nElements = 0;
}

template <typename Traits>
CCircularDoubleLinkedList<Traits>&
CCircularDoubleLinkedList<Traits>::operator=(CCircularDoubleLinkedList&& other) noexcept{
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
typename CCircularDoubleLinkedList<Traits>::value_type&
CCircularDoubleLinkedList<Traits>::operator[](size_t index){
    if(index >= m_nElements)
        throw std::out_of_range("Index out of range");

    Node *p = m_pRoot;
    for(size_t i=0; i < index; ++i) p = p->GetNext();
    return p->GetValueRef();
}

template <typename Traits>
const typename CCircularDoubleLinkedList<Traits>::value_type&
CCircularDoubleLinkedList<Traits>::operator[](size_t index) const{
    if(index >= m_nElements)
        throw std::out_of_range("Index out of range");

    Node *p = m_pRoot;
    for(size_t i=0; i < index; ++i) p = p->GetNext();
    return p->GetValueRef();
}

template <typename Traits>
template <typename ObjFunc, typename... Args>
void CCircularDoubleLinkedList<Traits>::Foreach(ObjFunc of, Args... args){
    Node *p = m_pRoot;
    for(size_t i=0; i < m_nElements; ++i){
        of(p->GetValueRef(), args...);
        p = p->GetNext();
    }
}

template <typename Traits>
istream &operator>>(istream &is, CCircularDoubleLinkedList<Traits> &container){
    using value_type = typename CCircularDoubleLinkedList<Traits>::value_type;

    if(!is) return is;

    CCircularDoubleLinkedList<Traits> tmp;

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

#endif // __CIRCULARDOUBLELINKEDLIST_H__
