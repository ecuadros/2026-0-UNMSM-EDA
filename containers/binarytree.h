#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <utility>
#include <mutex>
#include <stdexcept>
#include "../general/types.h"
#include "../util.h"

using namespace std;

// Traits
template <typename T, typename _Func, typename _Ref = long>
struct BTreeTrait{
    using value_type = T;
    using Func       = _Func;
    using ref_type   = _Ref;
};

template <typename T, typename _Ref = long>
struct AscendingBTreeTrait : 
    public BTreeTrait<T, std::less<T>, _Ref >{
};

template <typename T, typename _Ref = long>
struct DescendingBTreeTrait : 
    public BTreeTrait<T, std::greater<T>, _Ref >{
};

// Node
template <typename Traits>
class NodeBTree{
    using  value_type  = typename Traits::value_type;
    using  ref_type    = typename Traits::ref_type;
    using  Node        = NodeBTree<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pLeft   = nullptr;
    Node *m_pRight  = nullptr;
    Node *m_pParent = nullptr;

public:
    NodeBTree(){}
    NodeBTree( value_type _value, ref_type _ref = ref_type{})
        : m_data(_value), m_ref(_ref){}
        
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }

    ref_type    GetRef     () const { return m_ref;   }
    ref_type   &GetRefRef  () { return m_ref;   }

    Node      * GetLeft     () const { return m_pLeft;    }
    Node      *&GetLeftRef  () { return m_pLeft;    }

    Node      * GetRight    () const { return m_pRight;   }
    Node      *&GetRightRef () { return m_pRight;   }

    Node      * GetParent   () const { return m_pParent;  }
    Node      *&GetParentRef() { return m_pParent;  }

    Node &operator=(const Node &another){
        m_data = another.GetValue();
        m_ref  = another.GetRef();
        return *this;
    }
    bool operator==(const Node &another) const
    { return m_data == another.GetValue();   }
    bool operator<(const Node &another) const
    { return m_data < another.GetValue();   }
};

// ForwardIterator
template <typename Traits>
class CBTree;

template <typename Traits>
struct BTreeForwardIterator {
    using value_type  = typename Traits::value_type;
    using Node        = NodeBTree<Traits>;
    Node *m_pNode = nullptr;

    BTreeForwardIterator(Node *pNode = nullptr) : m_pNode(pNode) {}
    BTreeForwardIterator(BTreeForwardIterator<Traits> &another) : m_pNode(another.m_pNode) {}
    virtual ~BTreeForwardIterator(){}

    // ++iter
    BTreeForwardIterator<Traits> &operator++(){
        if( !m_pNode ) return *this;
        if( m_pNode->GetRight() ){
            m_pNode = m_pNode->GetRight();
            while( m_pNode->GetLeft() )
                m_pNode = m_pNode->GetLeft();
        } else {
            Node *pParent = m_pNode->GetParent();
            while( pParent && m_pNode == pParent->GetRight() ){
                m_pNode = pParent;
                pParent = pParent->GetParent();
            }
            m_pNode = pParent;
        }
        return *this;
    }

    bool operator!=(const BTreeForwardIterator<Traits> &another){
        return m_pNode != another.m_pNode;
    }
    value_type &operator*(){ return m_pNode->GetValueRef(); }
};

// BackwardIterator
template <typename Traits>
struct BTreeBackwardIterator {
    using value_type  = typename Traits::value_type;
    using Node        = NodeBTree<Traits>;
    Node *m_pNode = nullptr;

    BTreeBackwardIterator(Node *pNode = nullptr) : m_pNode(pNode) {}
    BTreeBackwardIterator(BTreeBackwardIterator<Traits> &another) : m_pNode(another.m_pNode) {}
    virtual ~BTreeBackwardIterator(){}

    // ++iter
    BTreeBackwardIterator<Traits> &operator++(){
        if( !m_pNode ) return *this;
        if( m_pNode->GetLeft() ){
            m_pNode = m_pNode->GetLeft();
            while( m_pNode->GetRight() )
                m_pNode = m_pNode->GetRight();
        } else {
            Node *pParent = m_pNode->GetParent();
            while( pParent && m_pNode == pParent->GetLeft() ){
                m_pNode = pParent;
                pParent = pParent->GetParent();
            }
            m_pNode = pParent;
        }
        return *this;
    }

    bool operator!=(const BTreeBackwardIterator<Traits> &another){
        return m_pNode != another.m_pNode;
    }
    value_type &operator*(){ return m_pNode->GetValueRef(); }
};

// Forward declarations
template <typename Traits> class CBTree;
template <typename Traits> ostream &operator<<(ostream &os, CBTree<Traits> &container);
template <typename Traits> istream &operator>>(istream &is, CBTree<Traits> &container);

// BinaryTree
template <typename Traits>
class CBTree {
public:
    using  value_type        = typename Traits::value_type;
    using  Func              = typename Traits::Func;
    using  ref_type          = typename Traits::ref_type;
    using  Node              = NodeBTree<Traits>;
    using  forward_iterator  = BTreeForwardIterator<Traits>;
    using  backward_iterator = BTreeBackwardIterator<Traits>;
    using  iterator          = forward_iterator;

protected:
    Node   *m_pRoot     = nullptr;
    size_t  m_nElements = 0;
    mutable mutex m_mtx;
    Func    m_comp;

public:
    CBTree(){}

    // constructor copia
    CBTree(const CBTree &otro);

    // move constructor
    CBTree(CBTree &&otro) noexcept;

    // destructor seguro
    virtual ~CBTree();

    // Insert
    void Insert(const value_type &val, ref_type ref = ref_type{});

    // Remove
    void Remove(const value_type &val);

    size_t getSize() const { return m_nElements; }
    bool   empty()   const { return m_nElements == 0; }

    // forward iterator begin
    forward_iterator begin(){
        Node *p = m_pRoot;
        while( p && p->GetLeft() ) p = p->GetLeft();
        return forward_iterator(p);
    }
    forward_iterator end(){ return forward_iterator(nullptr); }

    // backward iterator rbegin
    backward_iterator rbegin(){
        Node *p = m_pRoot;
        while( p && p->GetRight() ) p = p->GetRight();
        return backward_iterator(p);
    }
    backward_iterator rend(){ return backward_iterator(nullptr); }

    // inorder
    template <typename ObjFunc, typename ...Args>
    void Inorder(ObjFunc of, Args... args){
        InternalInorder(m_pRoot, of, args...);
    }

    // preorder
    template <typename ObjFunc, typename ...Args>
    void Preorder(ObjFunc of, Args... args){
        InternalPreorder(m_pRoot, of, args...);
    }

    // postorder
    template <typename ObjFunc, typename ...Args>
    void Postorder(ObjFunc of, Args... args){
        InternalPostorder(m_pRoot, of, args...);
    }

    // foreach
    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args){
        ::Foreach(*this, of, args...);
    }

    // firstthat
    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args){
        return ::FirstThat(*this, of, args...);
    }

    // operator<<
    friend ostream &operator<< <>(ostream &os, CBTree<Traits> &container);

    // operator>>
    friend istream &operator>> <>(istream &is, CBTree<Traits> &container);

protected:
    virtual void InternalInsert(Node *&rParent, const value_type &val, ref_type ref, Node *pParent);
    virtual void InternalRemove(Node *&rNode, const value_type &val);
    void InternalCopy(Node *&rDest, Node *pSrc, Node *pParent);
    void InternalDestroy(Node *pNode);
    void InternalPrint(ostream &os, Node *pNode, const string &prefix, bool isLeft) const;

    // inorder
    template <typename ObjFunc, typename ...Args>
    void InternalInorder(Node *pNode, ObjFunc of, Args... args){
        if( !pNode ) return;
        InternalInorder(pNode->GetLeft(), of, args...);
        of(pNode->GetValueRef(), args...);
        InternalInorder(pNode->GetRight(), of, args...);
    }

    template <typename ObjFunc, typename ...Args>
    void InternalPreorder(Node *pNode, ObjFunc of, Args... args){
        if( !pNode ) return;
        of(pNode->GetValueRef(), args...);
        InternalPreorder(pNode->GetLeft(), of, args...);
        InternalPreorder(pNode->GetRight(), of, args...);
    }

    template <typename ObjFunc, typename ...Args>
    void InternalPostorder(Node *pNode, ObjFunc of, Args... args){
        if( !pNode ) return;
        InternalPostorder(pNode->GetLeft(), of, args...);
        InternalPostorder(pNode->GetRight(), of, args...);
        of(pNode->GetValueRef(), args...);
    }
};

// constructor copia
template <typename Traits>
CBTree<Traits>::CBTree(const CBTree &otro){
    lock_guard<mutex> lock(otro.m_mtx);
    InternalCopy(m_pRoot, otro.m_pRoot, nullptr);
    m_nElements = otro.m_nElements;
}

// move constructor
template <typename Traits>
CBTree<Traits>::CBTree(CBTree &&otro) noexcept {
    lock_guard<mutex> lock(otro.m_mtx);
    m_pRoot     = otro.m_pRoot;
    m_nElements = otro.m_nElements;
    otro.m_pRoot     = nullptr;
    otro.m_nElements = 0;
}

// destructor seguro
template <typename Traits>
CBTree<Traits>::~CBTree(){
    lock_guard<mutex> lock(m_mtx);
    InternalDestroy(m_pRoot);
}

// InternalCopy
template <typename Traits>
void CBTree<Traits>::InternalCopy(Node *&rDest, Node *pSrc, Node *pParent){
    if( !pSrc ){ rDest = nullptr; return; }
    rDest = new Node(pSrc->GetValue(), pSrc->GetRef());
    rDest->GetParentRef() = pParent;
    InternalCopy(rDest->GetLeftRef(),  pSrc->GetLeft(),  rDest);
    InternalCopy(rDest->GetRightRef(), pSrc->GetRight(), rDest);
}

// InternalDestroy
template <typename Traits>
void CBTree<Traits>::InternalDestroy(Node *pNode){
    if( !pNode ) return;
    InternalDestroy(pNode->GetLeft());
    InternalDestroy(pNode->GetRight());
    delete pNode;
}

// InternalInsert
template <typename Traits>
void CBTree<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref, Node *pParent){
    if( !rParent ){
        rParent = new Node(val, ref);
        rParent->GetParentRef() = pParent;
        ++m_nElements;
        return;
    }
    if( m_comp(val, rParent->GetValue()) )
        InternalInsert(rParent->GetLeftRef(), val, ref, rParent);
    else
        InternalInsert(rParent->GetRightRef(), val, ref, rParent);
}

// Insert
template <typename Traits>
void CBTree<Traits>::Insert(const value_type &val, ref_type ref){
    lock_guard<mutex> lock(m_mtx);
    InternalInsert(m_pRoot, val, ref, nullptr);
}

// InternalRemove
template <typename Traits>
void CBTree<Traits>::InternalRemove(Node *&rNode, const value_type &val){
    if( !rNode ) return;
    if( m_comp(val, rNode->GetValue()) ){
        InternalRemove(rNode->GetLeftRef(), val);
    } else if( m_comp(rNode->GetValue(), val) ){
        InternalRemove(rNode->GetRightRef(), val);
    } else {
        // encontrado
        if( !rNode->GetLeft() && !rNode->GetRight() ){
            // hoja
            delete rNode;
            rNode = nullptr;
            --m_nElements;
        } else if( !rNode->GetLeft() ){
            // solo hijo derecho
            Node *pTemp = rNode;
            rNode = rNode->GetRight();
            rNode->GetParentRef() = pTemp->GetParent();
            delete pTemp;
            --m_nElements;
        } else if( !rNode->GetRight() ){
            // solo hijo izquierdo
            Node *pTemp = rNode;
            rNode = rNode->GetLeft();
            rNode->GetParentRef() = pTemp->GetParent();
            delete pTemp;
            --m_nElements;
        } else {
            // dos hijos
            Node *pSucc = rNode->GetRight();
            while( pSucc->GetLeft() )
                pSucc = pSucc->GetLeft();
            rNode->GetValueRef() = pSucc->GetValue();
            rNode->GetRefRef()   = pSucc->GetRef();
            InternalRemove(rNode->GetRightRef(), pSucc->GetValue());
        }
    }
}

// Remove
template <typename Traits>
void CBTree<Traits>::Remove(const value_type &val){
    lock_guard<mutex> lock(m_mtx);
    InternalRemove(m_pRoot, val);
}

// InternalPrint
template <typename Traits>
void CBTree<Traits>::InternalPrint(ostream &os, Node *pNode, const string &prefix, bool isLeft) const {
    if( !pNode ) return;
    os << prefix;
    os << (isLeft ? "├── " : "└── ");
    os << "(" << pNode->GetValue() << ":" << pNode->GetRef() << ")" << endl;
    InternalPrint(os, pNode->GetLeft(),  prefix + (isLeft ? "│   " : "    "), true);
    InternalPrint(os, pNode->GetRight(), prefix + (isLeft ? "│   " : "    "), false);
}

// operator<<
template <typename Traits>
ostream &operator<<(ostream &os, CBTree<Traits> &container){
    os << "CBTree: size = " << container.getSize() << endl;
    container.InternalPrint(os, container.m_pRoot, "", false);
    return os;
}

// operator>>
template <typename Traits>
istream &operator>>(istream &is, CBTree<Traits> &container){
    using value_type = typename Traits::value_type;
    using ref_type   = typename Traits::ref_type;
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

#endif // __BINARYTREE_H__