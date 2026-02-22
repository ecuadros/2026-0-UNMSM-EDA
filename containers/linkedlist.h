#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include <fstream>
#include <utility>
#include <mutex>
#include "../general/types.h"
#include "../util.h"
using namespace std;

// Traits
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

// Node
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
    NodeLinkedList( value_type _value, ref_type _ref = -1, Node *_pNext = nullptr)
        : m_data(_value), m_ref(_ref), m_pNext(_pNext){   }
        
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

// Iterator (patrón de GeneralIterator adaptado)
template <typename Traits>
class CLinkedList;

template <typename Traits>
struct ListIterator {
    using value_type  = typename Traits::value_type;
    using Node        = NodeLinkedList<Traits>;

    Node *m_pNode = nullptr;

    ListIterator(Node *pNode = nullptr) : m_pNode(pNode) {}
    
    ListIterator(ListIterator<Traits> &another) : m_pNode(another.m_pNode) {}

    virtual ~ListIterator(){}

    ListIterator<Traits> &operator++(){
        if( m_pNode )
            m_pNode = m_pNode->GetNext();
        return *this;
    }

    bool operator!=(const ListIterator<Traits> &another){
        return m_pNode != another.m_pNode;
    }

    value_type &operator*(){
        return m_pNode->GetValueRef();
    }
};

// LinkedList
template <typename Traits>
class CLinkedList {
public:
    using  value_type  = typename Traits::value_type;
    using  Node = NodeLinkedList<Traits>;
    using  iterator = ListIterator<Traits>;
    friend iterator;

private:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable mutex m_mtx;

public:
    CLinkedList(){}
    
    // Constructor copia (patrón intercambiar de util.h)
    CLinkedList(const CLinkedList &otro){
        lock_guard<mutex> lock(otro.m_mtx);
        Node *pCurr = otro.m_pRoot;
        while( pCurr ){
            push_back(pCurr->GetValue(), pCurr->GetRef());
            pCurr = pCurr->GetNext();
        }
    }

    // Move constructor
    CLinkedList(CLinkedList &&otro) noexcept {
        lock_guard<mutex> lock(otro.m_mtx);
        m_pRoot     = otro.m_pRoot;
        m_pLast     = otro.m_pLast;
        m_nElements = otro.m_nElements;
        otro.m_pRoot     = nullptr;
        otro.m_pLast     = nullptr;
        otro.m_nElements = 0;
    }

    // Destructor
    virtual ~CLinkedList(){
        lock_guard<mutex> lock(m_mtx);
        Node *pCurr = m_pRoot;
        while( pCurr ){
            Node *pNxt = pCurr->GetNext();
            delete pCurr;
            pCurr = pNxt;
        }
    }

    // Operator= copy
    CLinkedList& operator=(const CLinkedList &otro){
        if( this != &otro ){
            lock_guard<mutex> lock(otro.m_mtx);
            
            Node *pCurr = m_pRoot;
            while( pCurr ){
                Node *pNxt = pCurr->GetNext();
                delete pCurr;
                pCurr = pNxt;
            }
            m_pRoot = nullptr;
            m_pLast = nullptr;
            m_nElements = 0;

            pCurr = otro.m_pRoot;
            while( pCurr ){
                push_back(pCurr->GetValue(), pCurr->GetRef());
                pCurr = pCurr->GetNext();
            }
        }
        return *this;
    }

    // Operator= move
    CLinkedList& operator=(CLinkedList &&otro) noexcept {
        if( this != &otro ){
            lock_guard<mutex> lock(otro.m_mtx);

            Node *pCurr = m_pRoot;
            while( pCurr ){
                Node *pNxt = pCurr->GetNext();
                delete pCurr;
                pCurr = pNxt;
            }

            m_pRoot     = otro.m_pRoot;
            m_pLast     = otro.m_pLast;
            m_nElements = otro.m_nElements;
            otro.m_pRoot     = nullptr;
            otro.m_pLast     = nullptr;
            otro.m_nElements = 0;
        }
        return *this;
    }

    // Operator==
    bool operator==(const CLinkedList &otro) const {
        if( m_nElements != otro.m_nElements ) return false;
        Node *p1 = m_pRoot;
        Node *p2 = otro.m_pRoot;
        while( p1 && p2 ){
            if( p1->GetValue() != p2->GetValue() ) return false;
            p1 = p1->GetNext();
            p2 = p2->GetNext();
        }
        return true;
    }

    // Operator<
    bool operator<(const CLinkedList &otro) const {
        Node *p1 = m_pRoot;
        Node *p2 = otro.m_pRoot;
        while( p1 && p2 ){
            if( p1->GetValue() < p2->GetValue() ) return true;
            if( p1->GetValue() > p2->GetValue() ) return false;
            p1 = p1->GetNext();
            p2 = p2->GetNext();
        }
        return m_nElements < otro.m_nElements;
    }

    void push_back(value_type val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    
    size_t getSize() const { return m_nElements;  }

    // Operator[]
    value_type &operator[](size_t index);

    // Iterators
    iterator begin() { return iterator(m_pRoot);  }
    iterator end()   { return iterator(nullptr);  }

    // Foreach
    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        ::Foreach(*this, of, args...);
    }

    // FirstThat
    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args){
        return ::FirstThat(*this, of, args...);
    }

    // Operator<< para salida
    friend ostream &operator<<(ostream &os, CLinkedList<Traits> &container){
        os << "CLinkedList: size = " << container.getSize() << endl;
        os << "[";
        Node *pCurr = container.m_pRoot;
        while( pCurr ){
            os << "(" << pCurr->GetValue() << ":" << pCurr->GetRef() << ")";
            pCurr = pCurr->GetNext();
            if( pCurr ) os << ",";
        }
        os << "]" << endl;
        return os;
    }

    // Operator>> para entrada
    friend istream &operator>>(istream &is, CLinkedList<Traits> &container){
        size_t size;
        if( is >> size ){
            value_type value;
            ref_type ref;
            for( size_t i = 0; i < size; ++i ){
                is >> value >> ref;
                container.Insert(value, ref);
            }
        }
        return is;
    }

    // Guardar en archivo
    void Save(const string &filename){
        lock_guard<mutex> lock(m_mtx);
        ofstream ofs(filename);
        ofs << m_nElements << endl;
        Node *pCurr = m_pRoot;
        while( pCurr ){
            ofs << pCurr->GetValue() << " " << pCurr->GetRef() << endl;
            pCurr = pCurr->GetNext();
        }
        ofs.close();
    }

    // Leer desde archivo
    void Load(const string &filename){
        lock_guard<mutex> lock(m_mtx);
        ifstream ifs(filename);
        size_t size;
        ifs >> size;
        value_type value;
        ref_type ref;
        for( size_t i = 0; i < size; ++i ){
            ifs >> value >> ref;
            InternalInsert(m_pRoot, value, ref);
        }
        ifs.close();
    }

private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);
};

// push_back
template <typename Traits>
void CLinkedList<Traits>::push_back(value_type val, ref_type ref){
    lock_guard<mutex> lock(m_mtx);
    Node *pNew = new Node(val, ref);
    if( !m_pRoot )
        m_pRoot = pNew;
    else
        m_pLast->GetNextRef() = pNew;
    m_pLast = pNew;
    ++m_nElements;
}

// InternalInsert
template <typename Traits>
void CLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    if( !rParent || rParent->GetValue() > val ){
        Node *pNew = new Node(val, ref, rParent);
        rParent = pNew;
        ++m_nElements;
        if( !pNew->GetNext() )
            m_pLast = pNew;
        return;
    }
    InternalInsert(rParent->GetNextRef(), val, ref);
}

// Insert
template <typename Traits>
void CLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    lock_guard<mutex> lock(m_mtx);
    InternalInsert(m_pRoot, val, ref);
}

// Operator[]
template <typename Traits>
typename CLinkedList<Traits>::value_type &CLinkedList<Traits>::operator[](size_t index){
    Node *pCurr = m_pRoot;
    for( size_t i = 0; i < index && pCurr; ++i )
        pCurr = pCurr->GetNext();
    return pCurr->GetValueRef();
}

#endif // 
