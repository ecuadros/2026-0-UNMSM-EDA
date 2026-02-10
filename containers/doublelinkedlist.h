#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__


#include <iostream>
#include <mutex>
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
    public ListTrait<T, std::greater<T>>{};

template <typename T>
struct DescendingTrait :
    public ListTrait<T, std::less<T>>{};


template <typename Traits>
class NodeDoubleLinkedList{

    using value_type = typename Traits::value_type;
    using Node       = NodeDoubleLinkedList<Traits>;

private:
    value_type m_data;
    ref_type   m_ref;
    Node      *m_pNext = nullptr;
    Node      *m_pPrev = nullptr;

public:
    NodeDoubleLinkedList(){}
    NodeDoubleLinkedList(value_type val, ref_type ref = -1)
        : m_data(val), m_ref(ref){}

    value_type  GetValue() const { return m_data; }
    value_type &GetValueRef()    { return m_data; }

    ref_type  GetRef() const { return m_ref; }
    ref_type &GetRefRef()    { return m_ref; }

    Node*  GetNext() const { return m_pNext; }
    Node*& GetNextRef()    { return m_pNext; }

    Node*  GetPrev() const { return m_pPrev; }
    Node*& GetPrevRef()    { return m_pPrev; }

    bool operator<(const Node &another) const
    { return m_data < another.GetValue(); }
};


template <typename Container>
class DoubleLinkedListForwardIterator : public GeneralIterator<Container>{
    using Parent = GeneralIterator<Container>;
    using Node   = typename Container::Node;

public:
    DoubleLinkedListForwardIterator(Container *container, Size pos = 0)
        : Parent(container, pos){}

    DoubleLinkedListForwardIterator &operator++(){
        if (Parent::m_data && Parent::m_pos < Parent::m_pContainer->getSize())
            Parent::m_data = Parent::m_data->GetNext();
        ++Parent::m_pos;
        return *this;
    }

    bool operator==(const DoubleLinkedListForwardIterator &another) const{
        return Parent::m_pContainer == another.m_pContainer &&
               Parent::m_pos        == another.m_pos;
    }

    bool operator!=(const DoubleLinkedListForwardIterator &another) const{
        return !(*this == another);
    }

    auto &operator*(){
        return Parent::m_data->GetValueRef();
    }
};

template <typename Container>
class DoubleLinkedListBackwardIterator : public GeneralIterator<Container> {
    using Parent = GeneralIterator<Container>;
    using Node   = typename Container::Node;

public:
    DoubleLinkedListBackwardIterator(Container *pContainer, Size pos = 0)
        : Parent(pContainer, pos) {}

    DoubleLinkedListBackwardIterator(DoubleLinkedListBackwardIterator &another)
        : Parent(another) {}

    DoubleLinkedListBackwardIterator &operator++(){
        if (Parent::m_data && Parent::m_pos > 0)
            Parent::m_data = Parent::m_data->GetPrev();
        --Parent::m_pos;
        return *this;
    }

    bool operator==(const DoubleLinkedListBackwardIterator &another) const{
        return Parent::m_pContainer == another.m_pContainer &&
               Parent::m_pos        == another.m_pos;
    }

    bool operator!=(const DoubleLinkedListBackwardIterator &another) const{
        return !(*this == another);
    }

    DoubleLinkedListBackwardIterator operator=(DoubleLinkedListBackwardIterator &another){
        Parent::m_pContainer = another.m_pContainer;
        Parent::m_data       = another.m_data;
        Parent::m_pos        = another.m_pos;
        return *this;
    }

    auto &operator*(){
        return Parent::m_data->GetValueRef();
    }

    template <typename T>
    friend std::ostream& operator<<(std::ostream& os,
        const DoubleLinkedListBackwardIterator<T>& it){
        os << "BackwardIterator(pos=" << it.m_pos << ")";
        if (it.m_data)
            os << " value=" << it.m_data->GetValue();
        return os;
    }
};


template <typename Traits>
class CDoubleLinkedList{
public:
    using value_type = typename Traits::value_type;
    using Node       = NodeDoubleLinkedList<Traits>;
    using forward_iterator = DoubleLinkedListForwardIterator<CDoubleLinkedList<Traits>>;
    friend forward_iterator;
    using backward_iterator = DoubleLinkedListBackwardIterator<CDoubleLinkedList<Traits>>;
    friend backward_iterator;

private:
    Node   *m_pRoot = nullptr;
    Node   *m_pLast = nullptr;
    size_t  m_nElements = 0;
    mutable std::mutex m_mutex;

public:
    CDoubleLinkedList(){}

    CDoubleLinkedList(const CDoubleLinkedList &another){
        std::lock_guard<std::mutex> lock(another.m_mutex);

        Node *p = another.m_pRoot;
        while (p){
            push_back(p->GetValue(), p->GetRef());
            p = p->GetNext();
        }
    }

    CDoubleLinkedList(CDoubleLinkedList &&another) noexcept{
        std::lock_guard<std::mutex> lock(another.m_mutex);
        m_pRoot      = std::exchange(another.m_pRoot, nullptr);
        m_pLast      = std::exchange(another.m_pLast, nullptr);
        m_nElements  = std::exchange(another.m_nElements, 0);
    }

    virtual ~CDoubleLinkedList() noexcept {
        std::lock_guard<std::mutex> lock(m_mutex);
        Node *p = m_pRoot;
        while (p){
            Node *n = p->GetNext();
            delete p;
            p = n;
        }
        m_pRoot = m_pLast = nullptr;
        m_nElements = 0;
    }



    value_type &operator[](Size index){
        std::lock_guard<std::mutex> lock(m_mutex);
        Node *p = m_pRoot;
        for (Size i = 0; i < index; ++i)
            p = p->GetNext();
        return p->GetValueRef();
    }

    const value_type &operator[](Size index) const{
        std::lock_guard<std::mutex> lock(m_mutex);
        Node *p = m_pRoot;
        for (Size i = 0; i < index; ++i)
            p = p->GetNext();
        return p->GetValue();
    }

    backward_iterator rbegin(){
        backward_iterator it(this);
        it.m_data = m_pLast;
        it.m_pos = m_nElements - 1;
        return it;
    }
    backward_iterator rend(){
        return backward_iterator(this, -1);
    }

    forward_iterator begin(){
        forward_iterator it(this);
        it.m_data = m_pRoot;
        return it;
    }

    forward_iterator end(){
        return forward_iterator(this, m_nElements);
    }


    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        ::Foreach(*this, of, args...); }

    template <typename ObjFunc, typename ...Args> 
    auto FirstThat(ObjFunc of, Args... args){ 
        return ::FirstThat(*this, of, args...);}


    void push_back(const value_type &val, ref_type ref){
        std::lock_guard<std::mutex> lock(m_mutex);
        Node *pNew = new Node(val, ref);

        if (!m_pRoot){
            m_pRoot = m_pLast = pNew;
        } else {
            pNew->GetPrevRef() = m_pLast;
            m_pLast->GetNextRef() = pNew;
            m_pLast = pNew;
        }
        ++m_nElements;
    }


    void Insert(const value_type &val, ref_type ref){
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalInsert(m_pRoot, val, ref);
    }

    Size getSize() const { return m_nElements; }

private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
        if (!rParent || rParent->GetValue() > val){
            Node *pNew = new Node(val, ref);
            pNew->GetNextRef() = rParent;
            if (rParent)
                rParent->GetPrevRef() = pNew;
            rParent = pNew;
            if (!pNew->GetNext())
                m_pLast = pNew;
            ++m_nElements;
            return;
        }
        InternalInsert(rParent->GetNextRef(), val, ref);
    }

    friend std::ostream &operator<<(std::ostream &os, const CDoubleLinkedList &container){
        std::lock_guard<std::mutex> lock(container.m_mutex);
        os << "CDoubleLinkedList: size = " << container.m_nElements << endl;
        os << "[";
        for (Node *p = container.m_pRoot; p; p = p->GetNext()){
            os << "(" << p->GetValue() << ":" << p->GetRef() << ")";
            if (p->GetNext()) os << ",";
        }
        os << "]";
        return os;
    }

    friend std::istream &operator>>(std::istream &is, CDoubleLinkedList<Traits> &container){
    std::lock_guard<std::mutex> lock(container.m_mutex);

    using Node = typename CDoubleLinkedList<Traits>::Node;
    using value_type = typename Traits::value_type;

    Node *pNode = container.m_pRoot;
    while (pNode){
        Node *pNext = pNode->GetNext();
        delete pNode;
        pNode = pNext;
    }

    container.m_pRoot = nullptr;
    container.m_pLast = nullptr;
    container.m_nElements = 0;

    cout << "¿Cuántos elementos desea añadir? ";
    size_t count;
    is >> count;

    for (size_t i = 0; i < count; ++i){
        value_type val;
        ref_type   ref;

        cout << "Elemento " << (i + 1) << " - value ref: ";
        is >> val >> ref;

        Node *pNew = new Node(val, ref);

        if (!container.m_pRoot){
            container.m_pRoot = container.m_pLast = pNew;
        } else {
            pNew->GetPrevRef() = container.m_pLast;
            container.m_pLast->GetNextRef() = pNew;
            container.m_pLast = pNew;
        }

        ++container.m_nElements;
    }

    return is;
}

};

#endif // __DOUBLE_LINKEDLIST_H__
