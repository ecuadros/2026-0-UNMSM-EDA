#ifndef __AVLTREE_H__
#define __AVLTREE_H__

#include <iostream>
#include <mutex>
#include <algorithm>
#include <utility>
#include "binarytree.h" 

using namespace std;

template <typename Traits>
class CAVLTree {
public:
    using value_type  = typename Traits::value_type;
    using Node        =  NodeBinaryTree<Traits>;
    using CompareFunc = typename Traits::CompareFunc;
    using forward_iterator  = TreeForwardIterator< CAVLTree<Traits> >;
    using backward_iterator = TreeBackwardIterator< CAVLTree<Traits> >;
    friend forward_iterator;
    friend backward_iterator;

private:
    Node *m_pRoot = nullptr;
    CompareFunc comp;
    mutable mutex m_Block; 

    size_t _height(Node* pNode) const;
    void _updateHeight(Node* pNode);
    long _balanceFactor(Node* pNode) const;
    void _rotateLeft(Node *&rParent);
    void _rotateRight(Node *&rParent);
    void _balance(Node *&rParent);

    Node* _copyTree(Node* pNode);
    Node* _getMin(Node* pNode) const;

    void InternalInsert(Node *&rParent, const value_type &val);
    void InternalRemove(Node *&rParent, const value_type &val);

    template <typename Func, typename ...Args>
    void _internalinorden(Node *pNode, Func fn, Args... args) const;
    
    template <typename Func, typename ...Args>
    void _internalpreorden(Node *pNode, Func fn, Args... args) const;
    
    template <typename Func, typename ...Args>
    void _internalpostorden(Node *pNode, Func fn, Args... args) const;

public:
    CAVLTree() {}

    CAVLTree(const CAVLTree &another) {
        lock_guard<mutex> lock(another.m_Block);
        m_pRoot = _copyTree(another.m_pRoot);
        if (m_pRoot) m_pRoot->SetFather(nullptr);
    }

     CAVLTree(CAVLTree &&another) noexcept {
        lock_guard<mutex> lock(another.m_Block);
        m_pRoot = exchange(another.m_pRoot, nullptr);
    }

    ~CAVLTree();

    void Insert(const value_type &val) {
        lock_guard<mutex> lock(m_Block);
        InternalInsert(m_pRoot, val);
        if (m_pRoot) m_pRoot->SetFather(nullptr); 
    }

    void Remove(const value_type &val) {
        lock_guard<mutex> lock(m_Block);
        InternalRemove(m_pRoot, val);
        if (m_pRoot) m_pRoot->SetFather(nullptr); 
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

    friend ostream &operator<<(ostream &os, CAVLTree<Traits> &container) {
        lock_guard<mutex> lock(container.m_Block);
        os << "[ ";
        for (auto iter = container.begin(); iter != container.end(); ++iter) {
            os << *iter << " ";
        }
        os << "]";
        return os;
    }

    friend istream &operator>>(istream &is, CAVLTree<Traits> &container) {
        value_type val;
        is >> val;
        container.Insert(val);
        return is;
    }
};

// --- IMPLEMENTACIONES FUERA DE LA CLASE ---

template <typename Traits>
CAVLTree<Traits>::~CAVLTree() {
    lock_guard<mutex> lock(m_Block);
    Node* curr = m_pRoot;
    while (curr) {
        if (curr->GetChild(0)) {
            curr = curr->GetChild(0);
        } else if (curr->GetChild(1)) {
            curr = curr->GetChild(1);
        } else {
            Node* parent = curr->GetFather();
            if (parent) {
                if (parent->GetChild(0) == curr) {
                    parent->GetChildRef(0) = nullptr;
                } else {
                    parent->GetChildRef(1) = nullptr;
                }
            }
            delete curr;
            curr = parent; 
        }
    }
    m_pRoot = nullptr;
}

template <typename Traits>
size_t CAVLTree<Traits>::_height(Node* pNode) const {
    if (pNode != nullptr) {
        return pNode->GetHeight();
    } else {
        return 0;
    }
}

template <typename Traits>
void CAVLTree<Traits>::_updateHeight(Node* pNode) {
    if (pNode) {
        size_t h0 = _height(pNode->GetChild(0));
        size_t h1 = _height(pNode->GetChild(1));
        pNode->SetHeight(1 + max(h0, h1));
    }
}

template <typename Traits>
long CAVLTree<Traits>::_balanceFactor(Node* pNode) const {
    if (pNode) {
        long h0 = _height(pNode->GetChild(0));
        long h1 = _height(pNode->GetChild(1));
        return h1 - h0;
    } else {
        return 0;
    }
}

template <typename Traits>
void CAVLTree<Traits>::_rotateLeft(Node *&rParent) {
    Node* pRight = rParent->GetChild(1);
    
    rParent->GetChildRef(1) = pRight->GetChild(0);
    if (pRight->GetChild(0)) {
        pRight->GetChild(0)->SetFather(rParent); 
    }

    pRight->GetChildRef(0) = rParent;
    
    pRight->SetFather(rParent->GetFather()); 
    rParent->SetFather(pRight);              
    
    _updateHeight(rParent);
    _updateHeight(pRight);
    rParent = pRight;
}

template <typename Traits>
void CAVLTree<Traits>::_rotateRight(Node *&rParent) {
    Node* pLeft = rParent->GetChild(0);
    
    rParent->GetChildRef(0) = pLeft->GetChild(1);
    if (pLeft->GetChild(1)) {
        pLeft->GetChild(1)->SetFather(rParent); 
    }

    pLeft->GetChildRef(1) = rParent;
    
    pLeft->SetFather(rParent->GetFather()); 
    rParent->SetFather(pLeft);              
    
    _updateHeight(rParent);
    _updateHeight(pLeft);
    rParent = pLeft;
}

template <typename Traits>
void CAVLTree<Traits>::_balance(Node *&rParent) {
    long bf = _balanceFactor(rParent);
    
    if (bf > 1) {
        if (_balanceFactor(rParent->GetChild(1)) < 0) {
            _rotateRight(rParent->GetChildRef(1));
        }
        _rotateLeft(rParent);
    } 
    else if (bf < -1) {
        if (_balanceFactor(rParent->GetChild(0)) > 0) {
            _rotateLeft(rParent->GetChildRef(0));
        }
        _rotateRight(rParent);
    }
}

template <typename Traits>
typename CAVLTree<Traits>::Node* CAVLTree<Traits>::_copyTree(Node* pNode) {
    if (!pNode) {
        return nullptr;
    }
    Node* newNode = new Node(pNode->GetValue());
    newNode->SetHeight(pNode->GetHeight());
    
    newNode->GetChildRef(0) = _copyTree(pNode->GetChild(0));
    if (newNode->GetChild(0)) newNode->GetChild(0)->SetFather(newNode);

    newNode->GetChildRef(1) = _copyTree(pNode->GetChild(1));
    if (newNode->GetChild(1)) newNode->GetChild(1)->SetFather(newNode);

    return newNode;
}

template <typename Traits>
typename CAVLTree<Traits>::Node* CAVLTree<Traits>::_getMin(Node* pNode) const {
    while (pNode && pNode->GetChild(0) != nullptr) {
        pNode = pNode->GetChild(0);
    }
    return pNode;
}

template <typename Traits>
void CAVLTree<Traits>::InternalInsert(Node *&rParent, const value_type &val) {
    if (!rParent) {
        rParent = new Node(val);
        return;
    }
    
    if (val == rParent->GetValue()) {
        return; 
    }

    auto path = comp(val, rParent->GetValue()); 
    InternalInsert(rParent->GetChildRef(path), val);
    
    // Actualizar padre del hijo evaluado
    if (rParent->GetChild(path)) {
        rParent->GetChild(path)->SetFather(rParent);
    }

    _updateHeight(rParent);
    _balance(rParent);
}

template <typename Traits>
void CAVLTree<Traits>::InternalRemove(Node *&rParent, const value_type &val) {
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
            if (rParent->GetChild(1)) {
                rParent->GetChild(1)->SetFather(rParent);
            }
        }
    } else {
        auto path = comp(val, rParent->GetValue());
        InternalRemove(rParent->GetChildRef(path), val);
        if (rParent->GetChild(path)) {
            rParent->GetChild(path)->SetFather(rParent);
        }
    }

    if (rParent) {
        _updateHeight(rParent);
        _balance(rParent);
    }
}

template <typename Traits>
template <typename Func, typename ...Args>
void CAVLTree<Traits>::_internalinorden(Node *pNode, Func fn, Args... args) const {
    if (pNode) {
        _internalinorden(pNode->GetChild(0), fn, args...); 
        fn(pNode->GetValueRef(), args...);                 
        _internalinorden(pNode->GetChild(1), fn, args...); 
    }
}

template <typename Traits>
template <typename Func, typename ...Args>
void CAVLTree<Traits>::_internalpreorden(Node *pNode, Func fn, Args... args) const {
    if (pNode) {
        fn(pNode->GetValueRef(), args...);                 
        _internalpreorden(pNode->GetChild(0), fn, args...); 
        _internalpreorden(pNode->GetChild(1), fn, args...); 
    }
}

template <typename Traits>
template <typename Func, typename ...Args>
void CAVLTree<Traits>::_internalpostorden(Node *pNode, Func fn, Args... args) const {
    if (pNode) {
        _internalpostorden(pNode->GetChild(0), fn, args...); 
        _internalpostorden(pNode->GetChild(1), fn, args...); 
        fn(pNode->GetValueRef(), args...);                   
    }
}

#endif // __AVLTREE_H__