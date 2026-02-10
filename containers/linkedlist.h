#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
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

template <typename Container>
class LinkedListForwardIterator : public GeneralIterator<Container> {
    using Parent = GeneralIterator<Container>;
    using Node = typename Container::Node;
public:
    LinkedListForwardIterator(Container *pContainer, Size pos=0) 
    : Parent(pContainer, pos){}
    LinkedListForwardIterator(LinkedListForwardIterator<Container> &another) 
    : Parent(another){}

    LinkedListForwardIterator<Container> &operator++(){
        if( Parent::m_data && Parent::m_pos < Parent::m_pContainer->getSize() )
            Parent::m_data = Parent::m_data->GetNext();
        ++Parent::m_pos;
        return *this;
    }
    bool operator==(const LinkedListForwardIterator<Container> &another) const{
        return Parent::m_pContainer == another.m_pContainer &&
               Parent::m_pos        == another.m_pos;         
    }
    bool operator!=(const LinkedListForwardIterator<Container> &another) const{
        return !(*this == another);
    }
    LinkedListForwardIterator<Container> operator=(LinkedListForwardIterator<Container> &another){
        Parent::m_pContainer = another.m_pContainer;
        Parent::m_data       = another.m_data;
        Parent::m_pos        = another.m_pos;
        return *this;
    }
    auto &operator*(){
        return Parent::m_data->GetValueRef();
    }

    template <typename T>
    friend std::ostream& operator<<(std::ostream& os, const LinkedListForwardIterator<T>& it){
        os << "Iterator(pos=" << it.m_pos << ")";
        if (it.m_data) os << " value=" << it.m_data->GetValue();
        return os;
    }
};

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
    NodeLinkedList( value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){   }
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
    using forward_iterator = LinkedListForwardIterator< CLinkedList<Traits> >;
    friend forward_iterator;

    using  Node = NodeLinkedList<Traits>;

private:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable std::mutex m_mutex;    

public:
    CLinkedList(){}
    // TODO: Concurrencia (mutex) - ( pushback,operator[], etc )

    // Constructor copia 
    CLinkedList(const CLinkedList<Traits> &another){
        std::lock_guard<std::mutex> lock(another.m_mutex);

        Node *pOriginal = another.m_pRoot;

        while (pOriginal) {
            Node *pNewNode = new Node(pOriginal->GetValue(), pOriginal->GetRef());
            if (!m_pRoot) {
                m_pRoot = pNewNode;
                m_pLast = pNewNode;
            } else {
                m_pLast->GetNextRef() = pNewNode;
                m_pLast = pNewNode;
            }
            ++m_nElements;
            pOriginal = pOriginal->GetNext();
        }
    }


    // Move Constructor
    CLinkedList(CLinkedList<Traits> &&another) noexcept {
        std::lock_guard<std::mutex> lock(another.m_mutex);
        m_pRoot = std::exchange(another.m_pRoot, nullptr);
        m_pLast = std::exchange(another.m_pLast, nullptr);
        m_nElements = std::exchange(another.m_nElements, 0);
    }

    // Destructor seguro y virtual
    virtual ~CLinkedList(){ 
    std::lock_guard<std::mutex> lock(m_mutex);
    Node *pNode = m_pRoot;
    while(pNode){
        Node *pNext = pNode->GetNext();
        delete pNode;
        pNode = pNext;
    }
    m_pRoot = nullptr;
    m_pLast = nullptr;
    m_nElements = 0;
    }

    // TODO: Operadores de acceso []
    value_type &operator[](Size index){
        std::lock_guard<std::mutex> lock(m_mutex);
        Node* pNode = m_pRoot;
        for (size_t i=0; i < index; ++i)
        pNode = pNode->GetNext();
        return pNode->GetValueRef();
    }

    const value_type &operator[](Size index) const{
        std::lock_guard<std::mutex> lock(m_mutex);
        Node* pNode = m_pRoot;
        for (size_t i=0; i < index; ++i)
        pNode = pNode->GetNext();
        return pNode->GetValue();
    }

    // TODO: Iterators begin() end()
    forward_iterator begin()
    { return forward_iterator(this);  } 
    forward_iterator end()
    { return forward_iterator(this, getSize());  }

    Node* getRoot() const { return m_pRoot; }

    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        ::Foreach(*this, of, args...);
    }

    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args){
        return ::FirstThat(*this, of, args...);
    }

    void push_back(const value_type &val, ref_type ref);
    void clear() noexcept;
    void Insert(const value_type &val, ref_type ref);
    Size getSize(){ return m_nElements;  }
    
private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);

    friend std::ostream &operator<<(std::ostream &os, const CLinkedList<Traits> &container){
        std::lock_guard<std::mutex> lock(container.m_mutex);
        os << "CLinkedList: size = " << container.m_nElements << endl;
        os << "[";
        for (const Node *p = container.m_pRoot; p; p = p->GetNext()){
            os << "(" << p->GetValue() << ":" << p->GetRef() << ")";
            if (p->GetNext()) 
            os << ",";
        }
        os << "]";
        return os;
    }

    friend std::istream &operator>>(std::istream &is, CLinkedList<Traits> &container){
        std::lock_guard<std::mutex> lock(container.m_mutex);

        Node *pNode = container.m_pRoot;
        while (pNode){
            Node *pNext = pNode->GetNext();
            delete pNode;
            pNode = pNext;
        }
        container.m_pRoot = nullptr;
        container.m_pLast = nullptr;
        container.m_nElements = 0;

        std::cout << "¿Cuántos elementos desea añadir? ";
        size_t count;
        std::cin >> count;

        for (size_t i = 0; i < count; ++i){
            value_type val;
            ref_type ref;
            std::cout << "Elemento " << (i+1) << " - value ref: ";
            std::cin >> val >> ref;
            
            Node *pNew = new Node(val, ref);
            if (!container.m_pRoot) container.m_pRoot = pNew;
            else container.m_pLast->GetNextRef() = pNew;
            container.m_pLast = pNew;
            ++container.m_nElements;
        }
        return is;
    }
};

template <typename Traits>
void CLinkedList<Traits>::push_back(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mutex);
    Node *pNewNode = new Node(val, ref);
    if (!m_pRoot) {
        m_pRoot = pNewNode;
        m_pLast = pNewNode;
    } else {
        m_pLast->GetNextRef() = pNewNode;
        m_pLast = pNewNode;
    }
    ++m_nElements;
}



template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    if (!rParent || rParent->GetValue() > val) {
        Node *pNew = new Node(val, ref, rParent);
        rParent = pNew;
        ++m_nElements;
        return;
    }
    InternalInsert(rParent->GetNextRef(), val, ref);
}

template <typename Traits>
void CLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    std::lock_guard<std::mutex> lock(m_mutex);
    InternalInsert(m_pRoot, val, ref);
}

#endif // __LINKEDLIST_H__
