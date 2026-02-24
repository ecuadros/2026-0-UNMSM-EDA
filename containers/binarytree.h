#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <functional>
#include <vector>
#include <utility>
#include <string>
#include <sstream>
#include <mutex>
#include "../general/types.h"
#include "../util.h"
#include "../foreach.h"
using namespace std;

template <typename T>
struct TreeTraitAscending{
    using value_type  = T;
    using CompareFunc = std::greater<T>; 
};
template <typename T>
struct TreeTraitDescending {
    using value_type  = T;
    using CompareFunc = std::less<T>;   
};

template <typename Traits>
class CBinaryTreeAVL;
template <typename Traits>
class NodeBinaryTree{
    friend CBinaryTreeAVL<Traits>;
public:
    using value_type = typename Traits::value_type;
    using Node       = NodeBinaryTree<Traits>;

private:
    value_type m_data;
    ref_type   m_ref;
    Node      *m_pParent = nullptr;
    Node      *m_pChild[2] = {nullptr, nullptr};
    Size       m_height = 1; // para AVL

public:
    NodeBinaryTree(){}
    NodeBinaryTree(value_type _value, ref_type _ref = -1, Node *_parent = nullptr)
        : m_data(_value), m_ref(_ref), m_pParent(_parent){}

    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef()       { return m_data; }

    ref_type    GetRef     () const { return m_ref; }
    ref_type   &GetRefRef  ()       { return m_ref; }

    Node       *GetParent  () const { return m_pParent; }
    Node      *&GetParentRef()      { return m_pParent; }

    Node       *GetChild   (size_t branch) const { return m_pChild[branch]; }
    Node      *&GetChildRef(size_t branch)       { return m_pChild[branch]; }

    Size        GetHeight() const { return m_height; }
    Size       &GetHeightRef()    { return m_height; }
};
template <typename Container>
class BinaryTreeForwardIterator{
public:
    using value_type  = typename Container::value_type;
    using Node        = typename Container::Node;

private:
    Container  *m_pContainer = nullptr;
    Node       *m_pCurrent   = nullptr;
    std::vector<Node*> m_stack; // para simular la recursión

private:
    void pushLeft(Node *p){
        while(p){
            m_stack.push_back(p);
            p = p->GetChild(0);
        }
    }

public:
    BinaryTreeForwardIterator(Container *pContainer=nullptr , bool end = false) 
         : m_pContainer(pContainer){
         if(!m_pContainer || end){
            m_pCurrent = nullptr;
            return;
         }
         pushLeft(m_pContainer->GetRoot());
         if(!m_stack.empty()){
            m_pCurrent = m_stack.back();
            m_stack.pop_back();
         }
    }
    BinaryTreeForwardIterator(const BinaryTreeForwardIterator<Container> &another) = default;
    virtual ~BinaryTreeForwardIterator(){};

    bool operator!=(const BinaryTreeForwardIterator<Container> &another) const{
        return m_pContainer != another.m_pContainer ||
               m_pCurrent   != another.m_pCurrent;         
    }
    value_type &operator*(){
      return m_pCurrent->GetValueRef();
    }
    BinaryTreeForwardIterator &operator++(){
        if(!m_pCurrent) return *this; // ya estamos al final

        // Si hay hijo derecho, bajamos por su rama izquierda
        Node *pRight = m_pCurrent->GetChild(1);
        if(pRight) pushLeft(pRight);

        if(!m_stack.empty()){
            m_pCurrent = m_stack.back();
            m_stack.pop_back();
        } else {
            m_pCurrent = nullptr; // llegamos al final
        }
        return *this; // para permitir ++it en bucles
    } 
};    
template <typename Container>
class BinaryTreeBackwardIterator{
public:
    using value_type  = typename Container::value_type;
    using Node        = typename Container::Node;
private:
    Container  *m_pContainer = nullptr;
    Node       *m_pCurrent   = nullptr;
    std::vector<Node*> m_stack; // para simular la recursión
private:
    void pushRight(Node *p){
        while(p){
            m_stack.push_back(p);
            p = p->GetChild(1);
        }
    }
public:
    BinaryTreeBackwardIterator(Container *pContainer=nullptr , bool end = false) 
         : m_pContainer(pContainer){
         if(!m_pContainer || end){
            m_pCurrent = nullptr;
            return;
         }
         pushRight(m_pContainer->GetRoot());
         if(!m_stack.empty()){
            m_pCurrent = m_stack.back();
            m_stack.pop_back();
         }
    }
    BinaryTreeBackwardIterator(const BinaryTreeBackwardIterator<Container> &another) = default;
    virtual ~BinaryTreeBackwardIterator(){};

    bool operator!=(const BinaryTreeBackwardIterator<Container> &another){
        return m_pContainer != another.m_pContainer ||
               m_pCurrent   != another.m_pCurrent;         
    }
    value_type &operator*(){
      return m_pCurrent->GetValueRef();
    }

    BinaryTreeBackwardIterator &operator++(){
        if(!m_pCurrent) return *this; 

        // Si hay hijo izquierdo, bajamos por su rama derecha
        Node *pLeft = m_pCurrent->GetChild(0);
        if(pLeft) pushRight(pLeft);

        if(!m_stack.empty()){
            m_pCurrent = m_stack.back();
            m_stack.pop_back();
        } else {
            m_pCurrent = nullptr; // llegamos al final
        }
        return *this; 
    }
};  
template <typename Traits>
class CBinaryTree{
public:
    using value_type  = typename Traits::value_type;
    using Node        = NodeBinaryTree<Traits>;     
    using CompareFunc = typename Traits::CompareFunc;

protected:
    mutable std::mutex m_mutex;
    Node       *m_pRoot = nullptr;
    size_t      m_size  = 0;
    CompareFunc comp;

private:
    Node *CreateNode(const value_type &val, ref_type ref, Node *pParent){
        return new Node(val, ref, pParent);
    }

    void InternalClear(Node *pNode){
        if(!pNode) return;
        InternalClear(pNode->GetChild(0));
        InternalClear(pNode->GetChild(1));
        delete pNode;
    }

    Node *InternalClone(const Node *pNode, Node *pParent){
        if(!pNode) return nullptr;

        Node *pCopy = CreateNode(pNode->GetValue(), pNode->GetRef(), pParent);
        pCopy->GetChildRef(0) = InternalClone(pNode->GetChild(0), pCopy);
        pCopy->GetChildRef(1) = InternalClone(pNode->GetChild(1), pCopy);
        return pCopy;
    }

    void InternalInsert(Node *&rParent, Node *pParent, const value_type &val, ref_type ref){
        if(!rParent){
            rParent = CreateNode(val, ref, pParent);
            ++m_size;
            return;
        }
        size_t branch = (size_t) comp(val, rParent->GetValue());
        InternalInsert(rParent->GetChildRef(branch), rParent, val, ref);
    }
    Node *FinNode(const value_type &val) const {
        Node *p = m_pRoot;
        while(p){
            if(p->GetValue() == val) return p;
            size_t branch = (size_t) comp(val, p->GetValue());
            p = p->GetChild(branch);
        }
        return nullptr;
    }

    //bajao por child[0] hasta el final
    Node *FirstInSubtree(Node *p) const {
        if(!p) return nullptr;
        while(p->GetChild(0)) p = p->GetChild(0);
        return p;
    }
    
    //remplaza el subarbol u por v
    void Transplant(Node *u, Node *v){
        Node *p = u->GetParent();
        if(!p) m_pRoot = v;
        else if(p->GetChild(0) == u) p->GetChildRef(0) = v;
        else p->GetChildRef(1) = v;
        if(v) v->GetParentRef() = p;
    }


    void Swap(CBinaryTree &other){
        std::swap(m_pRoot, other.m_pRoot);
        std::swap(m_size,  other.m_size);
        std::swap(comp,    other.comp);
    }

    template<typename Func,typename... Args>
    void internal_inorden(Node *pNode, Func fn, Args... args){
        if(!pNode) return;
        internal_inorden(pNode->GetChild(0), fn, args...);
        fn(pNode->GetValueRef(), args...);
        internal_inorden(pNode->GetChild(1), fn, args...);
    }
    template<typename Func,typename... Args>
    void internal_preorden(Node *pNode, Func fn, Args... args){
        if(!pNode) return;
        fn(pNode->GetValueRef(), args...);
        internal_preorden(pNode->GetChild(0), fn, args...);
        internal_preorden(pNode->GetChild(1), fn, args...);
    }
    
    template<typename Func,typename... Args>
    void internal_postorden(Node *pNode, Func fn, Args... args){
        if(!pNode) return;
        internal_postorden(pNode->GetChild(0), fn, args...);
        internal_postorden(pNode->GetChild(1), fn, args...);
        fn(pNode->GetValueRef(), args...);
    }

    void internal_write(std::ostream &os, const Node *p) const {
        if(!p){
            os << "# "; 
            return;
        }
        os << p->GetValue() << " "<<p->GetRef() << " ";
        internal_write(os, p->GetChild(0));
        internal_write(os, p->GetChild(1));
    }
    
    Node *internal_read(std::istream &is, Node *pParent){
        std::string tok;
        if(!(is>>tok)) return nullptr; 

        if(tok == "#") return nullptr; // nodo nulo

        value_type val;
        {
            std::istringstream ss(tok);
            ss >> val;
        }
        ref_type ref;
        is>>ref;

        Node *pNew = CreateNode(val, ref, pParent);
        ++m_size;

        pNew->GetChildRef(0) = internal_read(is, pNew);
        pNew->GetChildRef(1) = internal_read(is, pNew);
        return pNew;
    }
    void ClearUnsafe(){
        InternalClear(m_pRoot);
        m_pRoot = nullptr;
        m_size  = 0;
    }
    

public:
    CBinaryTree(){}

    // COPY constructor
    CBinaryTree(const CBinaryTree &another){
        std::lock_guard<std::mutex> lock(another.m_mutex);
        m_pRoot = InternalClone(another.m_pRoot, nullptr);
        m_size = another.m_size;
        comp = another.comp;
    }

    // COPY assignment (opcional pero "pro")
    CBinaryTree &operator=(const CBinaryTree &another){
        if(this == &another) return *this;

        CBinaryTree tmp(another); // copia segura
        std::lock_guard<std::mutex> lock1(m_mutex);
        Swap(tmp);                // swap para no fugar memoria
        return *this;
    }

    // MOVE constructor
    CBinaryTree(CBinaryTree &&another) noexcept{
    std::lock_guard<std::mutex> lock(another.m_mutex);
    m_pRoot = std::exchange(another.m_pRoot, nullptr);
    m_size  = std::exchange(another.m_size, 0);
    comp    = std::move(another.comp);
}

    // MOVE assignment
   CBinaryTree &operator=(CBinaryTree &&another) noexcept{
    if(this == &another) return *this;

    std::scoped_lock lock(m_mutex, another.m_mutex);

    Node *oldRoot = std::exchange(m_pRoot, nullptr);
    m_size = 0;
    InternalClear(oldRoot);
    m_pRoot = std::exchange(another.m_pRoot, nullptr);
    m_size  = std::exchange(another.m_size, 0);
    comp    = std::move(another.comp);

    return *this;
}

    virtual ~CBinaryTree(){
        clear();
    }

    void Insert(const value_type &val, ref_type ref){
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalInsert(m_pRoot,nullptr, val, ref);
    }
    bool Remove(const value_type &val){
        std::lock_guard<std::mutex> lock(m_mutex);
        Node *z = FinNode(val);
        if(!z) return false;

        Node *zLeft = z->GetChild(0);
        Node *zRight = z->GetChild(1);

        if(!zLeft) Transplant(z, zRight);
        else if(!zRight) Transplant(z, zLeft);
        else{
            Node *y = FirstInSubtree(zRight);
            if(y->GetParent() != z){
                Transplant(y, y->GetChild(1));

                y->GetChildRef(1) = zRight;
                if(y->GetChild(1)) y->GetChild(1)->GetParentRef() = y;
            }
            Transplant(z, y); // y ocupa z
            y->GetChildRef(0) = zLeft; // y toma el left de z
            if(y->GetChild(0)) y->GetChild(0)->GetParentRef() = y;
        }
        z->GetChildRef(0) = nullptr;
        z->GetChildRef(1) = nullptr;    
        z->GetParentRef() = nullptr;
        delete z;
        --m_size;
        return true;
    }

    size_t size() const { return m_size; }
    bool   empty() const { return m_size == 0; }

    void clear(){
    Node *oldRoot = nullptr;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        oldRoot = std::exchange(m_pRoot, nullptr); 
        m_size = 0;
    }
    InternalClear(oldRoot);
}

    Node *GetRoot() const { return m_pRoot; }

    void Write(std::ostream &os) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        internal_write(os, m_pRoot);
    }
    void Read(std::istream &is){
        std::lock_guard<std::mutex> lock(m_mutex);
        ClearUnsafe();
        m_size = 0;
        m_pRoot = internal_read(is, nullptr);
    }

    friend std::ostream &operator<<(std::ostream &os, const CBinaryTree &t){
        t.Write(os);
        return os;
    }

    friend std::istream &operator>>(std::istream &is, CBinaryTree &t){
        t.Read(is);
        return is;
    }



    using forward_iterator = BinaryTreeForwardIterator<CBinaryTree<Traits>>;
    friend forward_iterator;

    forward_iterator begin() { return forward_iterator(this,false); }
    forward_iterator end()   { return forward_iterator(this,true); }

   // Foreach y FirstThat (variadic) 
    template <typename ObjFunc, typename... Args>
void Foreach(ObjFunc&& foo, Args&&... args){
    std::lock_guard<std::mutex> lock(m_mutex);
    ::Foreach(begin(), end(),
              std::forward<ObjFunc>(foo),
              std::forward<Args>(args)...);
}

template <typename Pred, typename... Args>
auto FirstThat(Pred&& pred, Args&&... args){
    std::lock_guard<std::mutex> lock(m_mutex);
    return ::FirstThat(begin(), end(),
                       std::forward<Pred>(pred),
                       std::forward<Args>(args)...);
}


    using backward_iterator = BinaryTreeBackwardIterator<CBinaryTree<Traits>>;
    friend backward_iterator;

    backward_iterator rbegin() { return backward_iterator(this,false); }
    backward_iterator rend()   { return backward_iterator(this,true); }

    template<typename Func,typename... Args>
    void inorden(Func fn, Args... args){
        std::lock_guard<std::mutex> lock(m_mutex);
        internal_inorden(m_pRoot, fn, args...);
    }
    template<typename Func,typename... Args>
    void preorden(Func fn, Args... args){
        std::lock_guard<std::mutex> lock(m_mutex);
        internal_preorden(m_pRoot, fn, args...);
    }
    template<typename Func,typename... Args>
    void postorden(Func fn, Args... args){
        std::lock_guard<std::mutex> lock(m_mutex);
        internal_postorden(m_pRoot, fn, args...);
    }
};

#endif // __BINARYTREE_H__
