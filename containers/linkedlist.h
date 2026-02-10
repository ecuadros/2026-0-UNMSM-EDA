#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include <fstream>
#include <mutex>
#include "../general/types.h"
#include "../util.h"
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

template<typename Container>
class LinkedListForwardIterator : public GeneralIterator<Container>{
    using parent = GeneralIterator<Container>;
    using value_type = typename Container::value_type;
    using Node = typename Container::Node;
    Node *pCurrent = nullptr; //Iterar sobre cada nodo cuando se le llame
public:
    LinkedListForwardIterator(Container *pContainer, Size position = 0) : GeneralIterator<Container>(pContainer, position), pCurrent(pContainer->m_pRoot){
        for(Size i=0;i<position;i++) 
            pCurrent = pCurrent->GetNext(); //Desplazamiento
    }

    LinkedListForwardIterator(LinkedListForwardIterator<Container> &another) : GeneralIterator<Container>(another), pCurrent(another.pCurrent){}

    //Sobrecarga de operadores
    value_type &operator*() override {return pCurrent->GetValueRef()}
    LinkedListForwardIterator<Container> &operator++(){
        if(pCurrent){
            pCurrent = pCurrent->GetNext();
            ++this->m_pos;
        }
        return *this;
    }
};

template<typename Traits>
ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
    std::lock_guard<std::mutex> lock(container.m_mutex);
        
        os << "CLinkedList: size = " << container.m_nElements 
           << (container.m_isCircular ? " (Circular)" : "") << endl;
        os << "[";
        
        if (container.m_pRoot) {
            NodeLinkedList* pCurrent = container.m_pRoot;
            size_t count = 0;
            
            do {
                os << "(" << pCurrent->GetValue() << ":" << pCurrent->GetRef() << ")";
                pCurrent = pCurrent->GetNext();
                count++;
                
                if (pCurrent && ((!container.m_isCircular) || count < container.m_nElements)) {
                    os << ",";
                }
                
                if (container.m_isCircular && count >= container.m_nElements) break;
                
            } while (pCurrent && pCurrent != container.m_pRoot);
        }
        
        os << "]" << endl;
        return os;
}

template<typename Traits>
istream &operator>>(istream &is, CLinkedList<Traits> &container){
    std::lock_guard<std::mutex> lock(container._mutex);
        
        container.clear();
        
        size_t n;
        is >> n;

        for (size_t i = 0; i < n; ++i) {
            value_type val;
            ref_type ref;
            is >> val >> ref;
            container.push_back(val, ref);
        }

        return is;
}

template<typename Traits>
class BaseList{
public:
    using value_type = typename Traits::value_type;
    using Node = NodeLinkedList<Traits>;

protected:
    bool compare(const value_type &a, const value_type &b) const{
        if constexpr (Traits::ordered){
            typename Traits::Func compareFunc;
            return compareFunc(a,b);
        }
        return false;
    }
};

template <typename Traits>
class CLinkedList : public BaseList<Traits>{
    mutable std::mutex _mutex;
public:
    using  value_type  = typename Traits::value_type;
    using  forward_iterator  = LinkedListForwardIterator < CLinkedList<Traits> >;
    friend forward_iterator;
    friend GeneralIterator<CLinkedList<Traits>>;
    using  Node = NodeLinkedList<Traits>;

    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    bool m_isCircular;

    CLinkedList(){}
    // TODO: Constructor copia
    CLinkedList(const CLinkedList &_copy);
    // TODO: Move Constructor
    CLinkedList(CLinkedList &&_move);
    // TODO: Destructor seguro y virtual
    virtual ~CLinkedList(){
        clear();
    }

    // TODO: Concurrencia (mutex)
    // TODO: Iterators begin() end()
    void clear();
    forward_iterator begin();
    forward_iterator end();
    // TODO: Operadores de acceso []
    value_type& operator[](size_t index) {
        std::lock_guard<std::mutex> lock(_mutex);
        
        if (index >= m_nElements) {
            throw std::out_of_range("Index out of range");
        }

        Node* pCurrent = m_pRoot;
        for (size_t i = 0; i < index; ++i) {
            pCurrent = pCurrent->GetNext();
        }
        
        return pCurrent->GetValueRef();
    }

    const value_type& operator[](size_t index) const {
        std::lock_guard<std::mutex> lock(_mutex);
        
        if (index >= m_nElements) {
            throw std::out_of_range("Index out of range");
        }

        Node* pCurrent = m_pRoot;
        for (size_t i = 0; i < index; ++i) {
            pCurrent = pCurrent->GetNext();
        }
        
        return pCurrent->GetValue();
    }

    forward_iterator begin() {
        return forward_iterator(m_pRoot, m_pRoot, m_isCircular);
    }

    forward_iterator end() {
        if (m_isCircular) {
            return forward_iterator(m_pRoot, m_pRoot, m_isCircular);
        }
        return forward_iterator(nullptr, m_pRoot, m_isCircular);
    }

    CLinkedList &operator=(const CLinkedList &to_copy);

    void push_back(value_type &val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    size_t getSize();

    //TODO: ForEach y firstThat
    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        std::lock_guard<std::mutex> lock(_mutex);
        if (!m_pRoot) return;

        Node* pCurrent = m_pRoot;
        size_t count = 0;
        
        do {
            of(pCurrent->GetValueRef(), args...);
            pCurrent = pCurrent->GetNext();
            count++;
            
            if (m_isCircular && count >= m_nElements) break;
            
        } while (pCurrent && pCurrent != m_pRoot);
    }

    template <typename ObjFunc, typename ...Args>
    value_type FirstThat(ObjFunc of, Args... args){
        std::lock_guard<std::mutex> lock(_mutex);
        if (!m_pRoot) return end();

        Node* pCurrent = m_pRoot;
        size_t count = 0;
        
        do {
            if (of(pCurrent->GetValueRef(), args...)) {
                return forward_iterator(pCurrent, m_pRoot, m_isCircular);
            }
            pCurrent = pCurrent->GetNext();
            count++;
            
            if (m_isCircular && count >= m_nElements) break;
            
        } while (pCurrent && pCurrent != m_pRoot);

        return end();
    }

private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);

    // TODO: Persistencia (write)
    friend ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
        os << "CLinkedList: size = " << container.getSize() << endl;
        os << "[";
        for (auto i = 0; i < container.getSize(); ++i){
            // os << "(" << arr.m_data[i].GetValue() << ":" << arr.m_data[i].GetRef() << "),";
        }
        os << "]" << endl;
        return os;
    }
    // TODO: Persistencia (read)
};

template <typename Traits>
void CLinkedList<Traits>::push_back(value_type &val, ref_type ref=-1){
    std::lock_guard<std::mutex> lock(_mutex);
        
        Node* pNewNode = new Node(val, ref);
        
        if (!m_pRoot) {
            m_pRoot = pNewNode;
            m_pLast = pNewNode;
        } else {
            m_pLast->GetNextRef() = pNewNode;
            m_pLast = pNewNode;
        }
        
        if (m_isCircular) {
            m_pLast->GetNextRef() = m_pRoot;
        }
        
        ++m_nElements;
}

template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    // TODO: Agregar algo para el caso de circular
        if (!rParent || rParent->GetValue() > val) {
            Node* pNew = new Node(val, ref, rParent);
            rParent = pNew;
            
            if (!m_pLast || pNew->GetNext() == nullptr) {
                m_pLast = pNew;
            }
            
            if (m_isCircular && m_pLast) {
                m_pLast->GetNextRef() = m_pRoot;
            }
            
            ++m_nElements;
            return;
        }
        
        InternalInsert(rParent->GetNextRef(), val, ref);
}

template <typename Traits>
void CLinkedList<Traits>::Insert(const value_type &val, ref_type ref=-1){
    std::lock_guard<std::mutex> lock(_mutex);
    InternalInsert(m_pRoot, val, ref);
}

template <typename Traits>
void CLinkedList<Traits>::clear(){
    std::lock_guard<std::mutex> lock(_mutex);
        
        if (!m_pRoot) return;

        if (m_isCircular && m_pLast) {
            m_pLast->GetNextRef() = nullptr;
        }

        Node* pCurrent = m_pRoot;
        while (pCurrent) {
            Node* pNext = pCurrent->GetNext();
            delete pCurrent;
            pCurrent = pNext;
        }

        m_pRoot = nullptr;
        m_pLast = nullptr;
        m_nElements = 0;
}

template <typename Traits>
size_t CLinkedList<Traits>::getSize(){
    std::lock_guard<std::mutex> lock(_mutex);
    return m_nElements;
}

#endif // __LINKEDLIST_H__
