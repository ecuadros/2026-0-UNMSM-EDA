#ifndef __CIRCULARLINKEDLIST_H__
#define __CIRCULARLINKEDLIST_H__

#include <iostream>
#include <fstream>
#include <utility>
#include <mutex>
#include "../general/types.h"
#include "../util.h"

using namespace std;

// Traits
template <typename T, typename _Func>
struct CListTrait{
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct CAscendingTrait : 
    public CListTrait<T, std::greater<T> >{
};

template <typename T>
struct CDescendingTrait : 
    public CListTrait<T, std::less<T> >{
};

// Node
template <typename Traits>
class NodeCircularLinkedList{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeCircularLinkedList<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;

public:
    NodeCircularLinkedList(){}
    NodeCircularLinkedList( value_type _value, ref_type _ref = -1, Node *_pNext = nullptr)
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

// Iterator
template <typename Traits>
class CCircularLinkedList;

template <typename Traits>
struct CListIterator {
    using value_type  = typename Traits::value_type;
    using Node        = NodeCircularLinkedList<Traits>;

    Node *m_pNode = nullptr;
    Node *m_pRoot = nullptr; // Referencia de donde inicio para saber cuando parar

    CListIterator(Node *pNode = nullptr, Node *pRoot = nullptr) 
        : m_pNode(pNode), m_pRoot(pRoot) {}
    
    CListIterator(CListIterator<Traits> &another) 
        : m_pNode(another.m_pNode), m_pRoot(another.m_pRoot) {}

    virtual ~CListIterator(){}

    CListIterator<Traits> &operator++(){
        if( m_pNode ){
            m_pNode = m_pNode->GetNext();
            if( m_pNode == m_pRoot ) // Vuelta completa
                m_pNode = nullptr;
        }
        return *this;
    }

    bool operator!=(const CListIterator<Traits> &another){
        return m_pNode != another.m_pNode;
    }

    value_type &operator*(){
        return m_pNode->GetValueRef();
    }
};

// CircularLinkedList
template <typename Traits>
class CCircularLinkedList {
public:
    using  value_type  = typename Traits::value_type;
    using  Node = NodeCircularLinkedList<Traits>;
    using  iterator = CListIterator<Traits>;
    friend iterator;

private:
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    mutable mutex m_mtx;

public:
    CCircularLinkedList(){}
    
    // Constructor copia
    CCircularLinkedList(const CCircularLinkedList &otro){
        lock_guard<mutex> lock(otro.m_mtx);
        if( otro.m_pRoot ){
            Node *pCurr = otro.m_pRoot;
            do {
                push_back(pCurr->GetValue(), pCurr->GetRef());
                pCurr = pCurr->GetNext();
            } while( pCurr != otro.m_pRoot );
        }
    }

    // Move constructor
    CCircularLinkedList(CCircularLinkedList &&otro) noexcept {
        lock_guard<mutex> lock(otro.m_mtx);
        m_pRoot     = otro.m_pRoot;
        m_pLast     = otro.m_pLast;
        m_nElements = otro.m_nElements;
        otro.m_pRoot     = nullptr;
        otro.m_pLast     = nullptr;
        otro.m_nElements = 0;
    }

    // Destructor
    virtual ~CCircularLinkedList(){
        lock_guard<mutex> lock(m_mtx);
        if( m_pRoot ){
            m_pLast->GetNextRef() = nullptr; // Romper circulo
            Node *pCurr = m_pRoot;
            while( pCurr ){
                Node *pNxt = pCurr->GetNext();
                delete pCurr;
                pCurr = pNxt;
            }
        }
    }

    // Operator= copy
    CCircularLinkedList& operator=(const CCircularLinkedList &otro){
        if( this != &otro ){
            lock_guard<mutex> lock(otro.m_mtx);
            
            // Clear current
            if( m_pRoot ){
                m_pLast->GetNextRef() = nullptr;
                Node *pCurr = m_pRoot;
                while( pCurr ){
                    Node *pNxt = pCurr->GetNext();
                    delete pCurr;
                    pCurr = pNxt;
                }
            }
            m_pRoot = nullptr;
            m_pLast = nullptr;
            m_nElements = 0;

            if( otro.m_pRoot ){
                Node *pCurr = otro.m_pRoot;
                do {
                    push_back(pCurr->GetValue(), pCurr->GetRef());
                    pCurr = pCurr->GetNext();
                } while( pCurr != otro.m_pRoot );
            }
        }
        return *this;
    }

    // Operator= move
    CCircularLinkedList& operator=(CCircularLinkedList &&otro) noexcept {
        if( this != &otro ){
            lock_guard<mutex> lock(otro.m_mtx);

            if( m_pRoot ){
                m_pLast->GetNextRef() = nullptr;
                Node *pCurr = m_pRoot;
                while( pCurr ){
                    Node *pNxt = pCurr->GetNext();
                    delete pCurr;
                    pCurr = pNxt;
                }
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
    bool operator==(const CCircularLinkedList &otro) const {
        if( m_nElements != otro.m_nElements ) return false;
        if( m_nElements == 0 ) return true;
        
        Node *p1 = m_pRoot;
        Node *p2 = otro.m_pRoot;
        do {
            if( p1->GetValue() != p2->GetValue() ) return false;
            p1 = p1->GetNext();
            p2 = p2->GetNext();
        } while( p1 != m_pRoot );
        return true;
    }

    // Operator<
    bool operator<(const CCircularLinkedList &otro) const {
        if( m_nElements == 0 && otro.m_nElements > 0 ) return true;
        if( m_nElements > 0 && otro.m_nElements == 0 ) return false;
        if( m_nElements == 0 && otro.m_nElements == 0 ) return false;

        Node *p1 = m_pRoot;
        Node *p2 = otro.m_pRoot;
        do {
            if( p1->GetValue() < p2->GetValue() ) return true;
            if( p1->GetValue() > p2->GetValue() ) return false;
            p1 = p1->GetNext();
            p2 = p2->GetNext();
        } while( p1 != m_pRoot && p2 != otro.m_pRoot );
        
        return m_nElements < otro.m_nElements;
    }

    void push_back(value_type val, ref_type ref);
    void Insert(const value_type &val, ref_type ref);
    
    size_t getSize() const { return m_nElements;  }

    // Operator[]
    value_type &operator[](size_t index);

    // Iterators
    iterator begin() { return iterator(m_pRoot, m_pRoot);  }
    iterator end()   { return iterator(nullptr, m_pRoot);  }

    // Foreach
    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        if( !m_pRoot ) return;
        Node *pCurr = m_pRoot;
        do {
            of(pCurr->GetValueRef(), args...);
            pCurr = pCurr->GetNext();
        } while( pCurr != m_pRoot );
    }

    // FirstThat
    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args){
        if( !m_pRoot ) return end();
        Node *pCurr = m_pRoot;
        do {
            if( of(pCurr->GetValueRef(), args...) )
                return iterator(pCurr, m_pRoot);
            pCurr = pCurr->GetNext();
        } while( pCurr != m_pRoot );
        return end();
    }

    // Operator<<
    friend ostream &operator<<(ostream &os, CCircularLinkedList<Traits> &container){
        os << "CCircularLinkedList: size = " << container.getSize() << endl;
        os << "[";
        if( container.m_pRoot ){
            Node *pCurr = container.m_pRoot;
            do {
                os << "(" << pCurr->GetValue() << ":" << pCurr->GetRef() << ")";
                pCurr = pCurr->GetNext();
                if( pCurr != container.m_pRoot ) os << ",";
            } while( pCurr != container.m_pRoot );
        }
        os << "]" << endl;
        return os;
    }

    // Operator>>
    friend istream &operator>>(istream &is, CCircularLinkedList<Traits> &container){
        size_t size;
        if( is >> size ){
            value_type val;
            ref_type ref;
            for( size_t i = 0; i < size; ++i ){
                is >> val >> ref;
                container.Insert(val, ref);
            }
        }
        return is;
    }

    // Save
    void Save(const string &filename){
        lock_guard<mutex> lock(m_mtx);
        ofstream ofs(filename);
        ofs << m_nElements << endl;
        if( m_pRoot ){
            Node *pCurr = m_pRoot;
            do {
                ofs << pCurr->GetValue() << " " << pCurr->GetRef() << endl;
                pCurr = pCurr->GetNext();
            } while( pCurr != m_pRoot );
        }
        ofs.close();
    }

    // Load
    void Load(const string &filename){
        lock_guard<mutex> lock(m_mtx);
        ifstream ifs(filename);
        size_t size;
        ifs >> size;
        value_type val;
        ref_type ref;
        for( size_t i = 0; i < size; ++i ){
            ifs >> val >> ref;
            Node *pNew = new Node(val, ref);
            if( !m_pRoot ){
                m_pRoot = pNew;
                m_pLast = pNew;
                pNew->GetNextRef() = m_pRoot;
            } else {
                m_pLast->GetNextRef() = pNew;
                m_pLast = pNew;
                m_pLast->GetNextRef() = m_pRoot;
            }
            ++m_nElements;
        }
        ifs.close();
    }
    
private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);
};

// push_back
template <typename Traits>
void CCircularLinkedList<Traits>::push_back(value_type val, ref_type ref){
    lock_guard<mutex> lock(m_mtx);
    Node *pNew = new Node(val, ref);
    if( !m_pRoot ){
        m_pRoot = pNew;
        pNew->GetNextRef() = m_pRoot;
    }
    else{
        m_pLast->GetNextRef() = pNew;
        pNew->GetNextRef() = m_pRoot;
    }
    m_pLast = pNew;
    ++m_nElements;
}

// InternalInsert
template <typename Traits>
void CCircularLinkedList<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
    // Caso especial: lista vacia
    if( !m_pRoot ){
        Node *pNew = new Node(val, ref);
        m_pRoot = pNew;
        m_pLast = pNew;
        pNew->GetNextRef() = m_pRoot;
        ++m_nElements;
        return;
    }

    // Caso: insertar al inicio (menor que root)
    if( rParent == m_pRoot && rParent->GetValue() > val ){
        Node *pNew = new Node(val, ref);
        pNew->GetNextRef() = m_pRoot;
        m_pRoot = pNew;
        m_pLast->GetNextRef() = m_pRoot; // Actualizar ultimo
        ++m_nElements;
        return;
    }

    // Caso recursivo/iterativo buscar posicion
    // Si el siguiente es root (llegamos al final) o el siguiente es mayor
    if( rParent->GetNext() == m_pRoot || rParent->GetNext()->GetValue() > val ){
        Node *pNew = new Node(val, ref);
        pNew->GetNextRef() = rParent->GetNext();
        rParent->GetNextRef() = pNew;
        if( rParent == m_pLast ) m_pLast = pNew;
        ++m_nElements;
        return;
    }
    
    InternalInsert(rParent->GetNextRef(), val, ref);
}

// Insert
template <typename Traits>
void CCircularLinkedList<Traits>::Insert(const value_type &val, ref_type ref){
    lock_guard<mutex> lock(m_mtx);
    InternalInsert(m_pRoot, val, ref);
}

// Operator[]
template <typename Traits>
typename CCircularLinkedList<Traits>::value_type &CCircularLinkedList<Traits>::operator[](size_t index){
    Node *pCurr = m_pRoot;
    for( size_t i = 0; i < index; ++i )
        pCurr = pCurr->GetNext();
    return pCurr->GetValueRef();
}

#endif // __CIRCULARLINKEDLIST_H__
