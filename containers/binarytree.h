#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__
#include <iostream>
#include <mutex>
#include <stack>
#include <algorithm>
#include <utility>
#include "../general/types.h"
#include "../util.h"
#include "../foreach.h"
template <typename T>
struct TreeTraitAscending {
    using value_type  = T;
    using CompareFunc = std::greater<T>;
};

template <typename T>
struct TreeTraitDescending {
    using value_type  = T;
    using CompareFunc = std::less<T>;
};

template <typename Traits>
class NodeBinaryTree{
    using  value_type  = typename Traits::value_type;
    using  Node        =  NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;
private:
    value_type m_data;
    Node *m_pChild[2] = {nullptr, nullptr};
    size_t m_height = 1;
public:
    NodeBinaryTree(){}
    NodeBinaryTree(value_type _value) : m_data(_value){}
    
    value_type  GetValue() const { return m_data; }
    value_type &GetValueRef() { return m_data; }

    Node* GetChild(size_t index) const { return m_pChild[index]; }
    Node*& GetChildRef(size_t index) { return m_pChild[index]; }

    size_t  GetHeight() const { return m_height; }
    void SetHeight(size_t h) { m_height = h; }
};
template <typename Container>
class TreeForwardIterator {
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

private:
    Node* m_pCurrent = nullptr;
    std::stack<Node*> m_Stack;

    void pushLeft(Node* pNode) {
        while (pNode) {
            m_Stack.push(pNode);
            pNode = pNode->GetChild(0); 
        }
    }

public:
    TreeForwardIterator(Node* pRoot) {
        if (pRoot) {
            pushLeft(pRoot); 
            if (m_Stack.empty()) {m_pCurrent = nullptr; } 
            else  {m_pCurrent = m_Stack.top(); }
        } 
        else {m_pCurrent = nullptr;}
    }
    bool operator!=(const TreeForwardIterator& another) const { 
        return m_pCurrent != another.m_pCurrent; 
    }   
    value_type& operator*() { 
        return m_pCurrent->GetValueRef(); 
    }
    TreeForwardIterator& operator++() {
        if (m_Stack.empty()) { 
            m_pCurrent = nullptr; 
            return *this; 
        }
        Node* pNode = m_Stack.top();
        m_Stack.pop();
        
        if (pNode->GetChild(1)) { pushLeft(pNode->GetChild(1));  }

        if (m_Stack.empty())    {   m_pCurrent = nullptr;  } 
        else                    { m_pCurrent = m_Stack.top(); }
        return *this;
    }
};

template <typename Container>
class TreeBackwardIterator {
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

private:
    Node* m_pCurrent = nullptr;
    std::stack<Node*> m_Stack;

    void pushRight(Node* pNode) {
        while (pNode) {
            m_Stack.push(pNode);
            pNode = pNode->GetChild(1); 
        }
    }

public:
    TreeBackwardIterator(Node* pRoot) {
        if (pRoot) {
            pushRight(pRoot); 
            if (m_Stack.empty()) {m_pCurrent = nullptr; } 
            else  { m_pCurrent = m_Stack.top(); }
        } 
        else {m_pCurrent = nullptr; }
    }

    bool operator!=(const TreeBackwardIterator& another) const { 
        return m_pCurrent != another.m_pCurrent; 
    }
    value_type& operator*() {return m_pCurrent->GetValueRef(); }
    
    TreeBackwardIterator& operator++() {
        if (m_Stack.empty()) { 
            m_pCurrent = nullptr; 
            return *this; 
        }
        Node* pNode = m_Stack.top();
        m_Stack.pop();
        if (pNode->GetChild(0)) {pushRight(pNode->GetChild(0));  }

        if (m_Stack.empty()) {m_pCurrent = nullptr; } 
        else { m_pCurrent = m_Stack.top();  }
        return *this;
    }
};
template <typename Traits>
class CBinaryTree{
public:
    using  value_type  = typename Traits::value_type;
    using  Node        =  NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;
    using  forward_iterator  = TreeForwardIterator< CBinaryTree<Traits> >;
    using  backward_iterator = TreeBackwardIterator< CBinaryTree<Traits> >;
    friend forward_iterator;
    friend backward_iterator;
protected:
    Node *m_pRoot = nullptr;
    CompareFunc comp;
    mutable mutex m_Block; 
    size_t _getHeight(Node* pNode) const;
    void _updateHeight(Node* pNode);
    Node* _copyTree(Node* pNode);
    Node* _getMin(Node* pNode) const;
    virtual void InternalInsert(Node *&rParent, const value_type &val);
    virtual void InternalRemove(Node *&rParent, const value_type &val);
    template <typename Func, typename ...Args>
    void _internalinorden(Node *pNode, Func fn, Args... args) const;
    template <typename Func, typename ...Args>
    void _internalpreorden(Node *pNode, Func fn, Args... args) const;
    template <typename Func, typename ...Args>
    void _internalpostorden(Node *pNode, Func fn, Args... args) const;
public:
    CBinaryTree() {}
    //  Copy constructor
    CBinaryTree(const CBinaryTree &another) {
        lock_guard<mutex> lock(another.m_Block);
        m_pRoot = _copyTree(another.m_pRoot);
    }
    //  Move constructor
    CBinaryTree(CBinaryTree &&another) noexcept {
        lock_guard<mutex> lock(another.m_Block);
        m_pRoot = exchange(another.m_pRoot, nullptr);
    }
    virtual ~CBinaryTree();
    virtual void Insert(const value_type &val) {
        lock_guard<mutex> lock(m_Block);
        InternalInsert(m_pRoot, val);
    }

    virtual void Remove(const value_type &val) {
        lock_guard<mutex> lock(m_Block);
        InternalRemove(m_pRoot, val);
    }
    forward_iterator begin() { 
        return forward_iterator(m_pRoot); 
    }
    
    forward_iterator end() { 
        return forward_iterator(nullptr); 
    }

    backward_iterator rbegin() { 
        return backward_iterator(m_pRoot); 
    }
    
    backward_iterator rend() { 
        return backward_iterator(nullptr); 
    }
    template <typename ObjFunc, typename ...Args>
    void Foreach(ObjFunc of, Args... args) {
        lock_guard<mutex> lock(m_Block);
        ::Foreach(*this, of, args...); 
    }

    template <typename ObjFunc, typename ...Args>
    auto FirstThat(ObjFunc of, Args... args) {
        lock_guard<mutex> lock(m_Block);
        return ::FirstThat(*this, of, args...); 
    }
    template <typename Func, typename ...Args>
    void inorden(Func fn, Args... args) const {
        lock_guard<mutex> lock(m_Block);
        _internalinorden(m_pRoot, fn, args...);
    }
    template <typename Func, typename ...Args>
    void preorden(Func fn, Args... args) const {
        lock_guard<mutex> lock(m_Block);
        _internalpreorden(m_pRoot, fn, args...);
    }
    template <typename Func, typename ...Args>
    void postorden(Func fn, Args... args) const {
        lock_guard<mutex> lock(m_Block);
        _internalpostorden(m_pRoot, fn, args...);
    }
    friend ostream &operator<<(ostream &os, CBinaryTree<Traits> &container) {
        lock_guard<mutex> lock(container.m_Block);
        os << "[ ";
        for (auto iter = container.begin(); iter != container.end(); ++iter) {
            os << *iter << " ";
        }
        os << "]";
        return os;
    }

    friend istream &operator>>(istream &is, CBinaryTree<Traits> &container) {
        value_type val;
        is >> val;
        container.Insert(val);
        return is;
    }
};
template <typename Traits>
size_t CBinaryTree<Traits>::_getHeight(Node* pNode) const { 
    if (pNode != nullptr) {  return pNode->GetHeight(); } 
    else { return 0; }
}
template <typename Traits>
void CBinaryTree<Traits>::_updateHeight(Node* pNode) {
    if (pNode) {
        size_t h0 = _getHeight(pNode->GetChild(0)); 
        size_t h1 = _getHeight(pNode->GetChild(1)); 
        pNode->SetHeight(1 + max(h0, h1));
    }
}
template <typename Traits>
typename CBinaryTree<Traits>::Node* CBinaryTree<Traits>::_copyTree(Node* pNode) {
    if (!pNode) { return nullptr; }
    Node* newNode = new Node(pNode->GetValue());
    newNode->SetHeight(pNode->GetHeight());
    newNode->GetChildRef(0) = _copyTree(pNode->GetChild(0));
    newNode->GetChildRef(1) = _copyTree(pNode->GetChild(1));
    return newNode;
}
template <typename Traits>
typename CBinaryTree<Traits>::Node* CBinaryTree<Traits>::_getMin(Node* pNode) const {
    while (pNode && pNode->GetChild(0) != nullptr) {
        pNode = pNode->GetChild(0);
    }
    return pNode;
}
template <typename Traits>
void CBinaryTree<Traits>::InternalInsert(Node *&rParent, const value_type &val){
    if (!rParent) {
        rParent = new Node(val);
        return;
    }
    
    if (val == rParent->GetValue()) { return ; }

    auto path = comp(val, rParent->GetValue()); 
    InternalInsert(rParent->GetChildRef(path), val);
    _updateHeight(rParent);
}
template <typename Traits>
void CBinaryTree<Traits>::InternalRemove(Node *&rParent, const value_type &val) {
    if (!rParent) {
        return;
    }

    if (val == rParent->GetValue()) {
        if (!rParent->GetChild(0)) {
            Node* temp = rParent->GetChild(1);
            delete rParent;
            rParent = temp; 
        } 
        else if (!rParent->GetChild(1)) {
            Node* temp = rParent->GetChild(0);
            delete rParent;
            rParent = temp; 
        } 
        else {
            Node* temp = _getMin(rParent->GetChild(1));
            rParent->GetValueRef() = temp->GetValue();
            InternalRemove(rParent->GetChildRef(1), temp->GetValue());
        }
    } else {
        auto path = comp(val, rParent->GetValue());
        InternalRemove(rParent->GetChildRef(path), val);
    }
    _updateHeight(rParent);
}
template <typename Traits>
template <typename Func, typename ...Args>
void CBinaryTree<Traits>::_internalinorden(Node *pNode, Func fn, Args... args) const {
    if (pNode) {
        _internalinorden(pNode->GetChild(0), fn, args...); 
        fn(pNode->GetValueRef(), args...);                 
        _internalinorden(pNode->GetChild(1), fn, args...); 
    }
}
template <typename Traits>
template <typename Func, typename ...Args>
void CBinaryTree<Traits>::_internalpreorden(Node *pNode, Func fn, Args... args) const {
    if (pNode) {
        fn(pNode->GetValueRef(), args...);                 
        _internalpreorden(pNode->GetChild(0), fn, args...); 
        _internalpreorden(pNode->GetChild(1), fn, args...); 
    }
}
template <typename Traits>
template <typename Func, typename ...Args>
void CBinaryTree<Traits>::_internalpostorden(Node *pNode, Func fn, Args... args) const {
    if (pNode) {
        _internalpostorden(pNode->GetChild(0), fn, args...); 
        _internalpostorden(pNode->GetChild(1), fn, args...); 
        fn(pNode->GetValueRef(), args...);                   
    }
}

template <typename Traits>
CBinaryTree<Traits>::~CBinaryTree() {
    lock_guard<mutex> lock(m_Block);
    if (m_pRoot) {
        stack<Node*> s;
        s.push(m_pRoot);
        while (!s.empty()) {
            Node* pTemp = s.top();
            s.pop();
            if (pTemp->GetChild(0)) {s.push(pTemp->GetChild(0));  }
            if (pTemp->GetChild(1)) { s.push(pTemp->GetChild(1)); }
            delete pTemp;
        }
        m_pRoot = nullptr;
    }
}
#endif // __BINARYTREE_H__