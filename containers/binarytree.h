#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <mutex>
#include <utility>
#include <functional>
#include <stack>
#include "../general/types.h"
#include "../util.h"

template <typename Traits> class CBinaryTree;
template <typename Container> class TreeForwardIterator;
template <typename Container> class TreeBackwardIterator;
template <typename Traits> class CAVLTree;

template <typename Traits> std::ostream& operator<<(std::ostream& os, CBinaryTree<Traits>& tree);
template <typename Traits> std::istream& operator>>(std::istream& is, CBinaryTree<Traits>& tree);

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
    using  Node        = NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;

    friend class CBinaryTree<Traits>;
    friend class TreeForwardIterator<CBinaryTree<Traits>>;
    friend class TreeBackwardIterator<CBinaryTree<Traits>>;
    friend class CAVLTree<Traits>;
private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pChild[2]  = {nullptr, nullptr};
    Size m_height = 1;
public:
    NodeBinaryTree(){}
    NodeBinaryTree( value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){   }
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }
};

template <typename Container>
class TreeForwardIterator {
    using Node = typename Container::Node;
    using value_type = typename Container::value_type;
private:
    std::stack<Node*> m_stack;
    Node* m_current;

public:
    TreeForwardIterator(Node* root = nullptr) : m_current(nullptr) {
        Node* curr = root;
        while (curr) {
            m_stack.push(curr);
            curr = curr->m_pChild[0];
        }
        if (!m_stack.empty()) 
            m_current = m_stack.top();
    }

    bool operator!=(const TreeForwardIterator &other) const {
        return m_current != other.m_current;
    }

    value_type& operator*() {
        return m_current->GetValueRef();
    }

    TreeForwardIterator& operator++() {
        if (m_stack.empty()) { 
            m_current = nullptr; 
            return *this; 
        }
        
        Node* node = m_stack.top();
        m_stack.pop();
        if (node->m_pChild[1]) {
            node = node->m_pChild[1];
            while (node) {
                m_stack.push(node);
                node = node->m_pChild[0];
            }
        }
        
        if (!m_stack.empty()) 
            m_current = m_stack.top();
        else 
            m_current = nullptr;

        return *this;
    }
};

template <typename Container>
class TreeBackwardIterator {
    using Node = typename Container::Node;
    using value_type = typename Container::value_type;
private:
    std::stack<Node*> m_stack;
    Node* m_current;

public:
    TreeBackwardIterator(Node* root = nullptr) : m_current(nullptr) {
        Node* curr = root;
        while (curr) {
            m_stack.push(curr);
            curr = curr->m_pChild[1];
        }
        if (!m_stack.empty()) 
            m_current = m_stack.top();
    }

    bool operator!=(const TreeBackwardIterator &other) const {
        return m_current != other.m_current;
    }

    value_type& operator*() {
        return m_current->GetValueRef();
    }

    TreeBackwardIterator& operator++() {
        if (m_stack.empty()) { 
            m_current = nullptr; 
            return *this; 
        }
        
        Node* node = m_stack.top();
        m_stack.pop();

        if (node->m_pChild[0]) {
            node = node->m_pChild[0];
            while (node) {
                m_stack.push(node);
                node = node->m_pChild[1];
            }
        }
        
        if (!m_stack.empty()) 
            m_current = m_stack.top();
        else 
            m_current = nullptr;

        return *this;
    }
};

template <typename Traits>
class CBinaryTree{
public:
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;

    using forward_iterator  = TreeForwardIterator<CBinaryTree<Traits>>;
    using backward_iterator = TreeBackwardIterator<CBinaryTree<Traits>>;
protected:
    Node *m_pRoot = nullptr;
    CompareFunc comp;
    mutable std::mutex m_mtx;

public:
    CBinaryTree() {}
    CBinaryTree(const CBinaryTree &another);
    CBinaryTree(CBinaryTree &&another);
    virtual ~CBinaryTree();
    
    forward_iterator begin(){
        std::lock_guard<std::mutex> lock(m_mtx); 
        return forward_iterator(m_pRoot); 
    }
    
    forward_iterator end(){ return forward_iterator(nullptr);   }

    backward_iterator rbegin(){
        std::lock_guard<std::mutex> lock(m_mtx); 
        return backward_iterator(m_pRoot);
    }
    backward_iterator rend(){   return backward_iterator(nullptr);  }

    void Insert(const value_type &val, ref_type ref = -1);
    void Insert(value_type &&val, ref_type ref = -1);
    void Remove(const value_type &val);

    template <typename Func, typename ...Args>
    void InOrderVariadic(Func&& f, Args&&... args);

    template <typename Func, typename ...Args>
    void PreOrderVariadic(Func&& f, Args&&... args);

    template <typename Func, typename ...Args>
    void PostOrderVariadic(Func&& f, Args&&... args);

    template <typename Func, typename ...Args>
    void ForEach(Func&& f, Args&&... args);

    template <typename Func, typename ...Args>
    value_type* FirstThat(Func&& f, Args&&... args);

    friend std::ostream& operator<< <Traits>(std::ostream& os, CBinaryTree<Traits>& tree);
    friend std::istream& operator>> <Traits>(std::istream& is, CBinaryTree<Traits>& tree);
private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref);
    void InternalInsertMove(Node *&rParent, value_type &&val, ref_type ref);
    void InternalRemove(Node *&rParent, const value_type &val);
    Node* InternalClone(Node* node);
    void InternalDestroy(Node* node);

    template <typename Func, typename ...Args>
    void InternalInOrder(Node* p, Func&& f, Args&&... args);

    template <typename Func, typename ...Args>
    void InternalPreOrder(Node* p, Func&& f, Args&&... args);

    template <typename Func, typename ...Args>
    void InternalPostOrder(Node* p, Func&& f, Args&&... args);

    template <typename Func, typename ...Args>
    value_type* InternalFirstThat(Node* p, Func&& f, Args&&... args);
};

template <typename Traits>
CBinaryTree<Traits>::CBinaryTree(const CBinaryTree &another){
    std::lock_guard<std::mutex> lock(another.m_mtx);
        if(another.m_pRoot)
            m_pRoot = InternalClone(another.m_pRoot);
}

template <typename Traits>
CBinaryTree<Traits>::CBinaryTree(CBinaryTree &&another){
    std::lock_guard<std::mutex> lock(another.m_mtx);
        m_pRoot = another.m_pRoot;
        another.m_pRoot = nullptr;
}

template <typename Traits>
CBinaryTree<Traits>::~CBinaryTree() {
    InternalDestroy(m_pRoot);
}

template <typename Traits>
void CBinaryTree<Traits>::Insert(const value_type &val, ref_type ref) {
    std::lock_guard<std::mutex> lock(m_mtx);
    InternalInsert(m_pRoot, val, ref);
}

template <typename Traits>
void CBinaryTree<Traits>::Insert(value_type &&val, ref_type ref) {
    std::lock_guard<std::mutex> lock(m_mtx);
    InternalInsertMove(m_pRoot, std::move(val), ref);
}

template <typename Traits>
void CBinaryTree<Traits>::Remove(const value_type &val) {
    std::lock_guard<std::mutex> lock(m_mtx);
    InternalRemove(m_pRoot, val);
}

template <typename Traits>
void CBinaryTree<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref) {
    if( !rParent ){
        rParent = new Node(val, ref);
        return;
    }
    auto path = comp(val, rParent->GetValue());
    InternalInsert(rParent->m_pChild[path], val, ref);
}

template <typename Traits>
void CBinaryTree<Traits>::InternalInsertMove(Node *&rParent, value_type &&val, ref_type ref) {
    if( !rParent ){
        rParent = new Node(std::move(val), ref);
        return;
    }
    auto path = comp(val, rParent->GetValue());
    InternalInsertMove(rParent->m_pChild[path], std::move(val), ref);
}

template <typename Traits>
void CBinaryTree<Traits>::InternalRemove(Node *&rParent, const value_type &val) {
    if (!rParent) 
        return;

    if (val == rParent->GetValue()) {
        if (!rParent->m_pChild[0] || !rParent->m_pChild[1]) {
            Node *temp;
            if (rParent->m_pChild[0]) 
                temp = rParent->m_pChild[0]; 
            else 
                temp = rParent->m_pChild[1]; 
        
            delete rParent;
            rParent = temp;
        } 
        else {
            Node *succ = rParent->m_pChild[1];
            while (succ->m_pChild[0]) 
                succ = succ->m_pChild[0];

            rParent->GetValueRef() = std::move(succ->GetValueRef());
            InternalRemove(rParent->m_pChild[1], rParent->GetValue());
        }
    } else {
        auto path = comp(val, rParent->GetValue());
        InternalRemove(rParent->m_pChild[path], val);
    }
}

template <typename Traits>
typename CBinaryTree<Traits>::Node* CBinaryTree<Traits>::InternalClone(Node* node) {
    if (!node) 
        return nullptr;
    Node* newNode = new Node(node->GetValue(), node->m_ref); 
    newNode->m_pChild[0] = InternalClone(node->m_pChild[0]);
    newNode->m_pChild[1] = InternalClone(node->m_pChild[1]);
    return newNode;
}

template <typename Traits>
void CBinaryTree<Traits>::InternalDestroy(Node* node) {
    if (node) {
        InternalDestroy(node->m_pChild[0]);
        InternalDestroy(node->m_pChild[1]);
        delete node;
    }
}

template <typename Traits>
template <typename Func, typename ...Args>
void CBinaryTree<Traits>::InOrderVariadic(Func&& action, Args&&... args) {
    std::lock_guard<std::mutex> lock(m_mtx);
    InternalInOrder(m_pRoot, std::forward<Func>(action), std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename ...Args>
void CBinaryTree<Traits>::InternalInOrder(Node* pNode, Func&& action, Args&&... args) {
    if (!pNode) 
        return;
    InternalInOrder(pNode->m_pChild[0], std::forward<Func>(action), std::forward<Args>(args)...);
    action(pNode->GetValueRef(), std::forward<Args>(args)...);
    InternalInOrder(pNode->m_pChild[1], std::forward<Func>(action), std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename ...Args>
void CBinaryTree<Traits>::PreOrderVariadic(Func&& action, Args&&... args) {
    std::lock_guard<std::mutex> lock(m_mtx);
    InternalPreOrder(m_pRoot, std::forward<Func>(action), std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename ...Args>
void CBinaryTree<Traits>::InternalPreOrder(Node* pNode, Func&& action, Args&&... args) {
    if (!pNode) 
        return;
    action(pNode->GetValueRef(), std::forward<Args>(args)...);
    InternalPreOrder(pNode->m_pChild[0], std::forward<Func>(action), std::forward<Args>(args)...);
    InternalPreOrder(pNode->m_pChild[1], std::forward<Func>(action), std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename ...Args>
void CBinaryTree<Traits>::PostOrderVariadic(Func&& action, Args&&... args) {
    std::lock_guard<std::mutex> lock(m_mtx);
    InternalPostOrder(m_pRoot, std::forward<Func>(action), std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename ...Args>
void CBinaryTree<Traits>::InternalPostOrder(Node* pNode, Func&& action, Args&&... args) {
    if (!pNode) 
        return;
    InternalPostOrder(pNode->m_pChild[0], std::forward<Func>(action), std::forward<Args>(args)...);
    InternalPostOrder(pNode->m_pChild[1], std::forward<Func>(action), std::forward<Args>(args)...);
    action(pNode->GetValueRef(), std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename ...Args>
void CBinaryTree<Traits>::ForEach(Func&& action, Args&&... args) {
    InOrderVariadic(std::forward<Func>(action), std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename ...Args>
typename CBinaryTree<Traits>::value_type* CBinaryTree<Traits>::FirstThat(Func&& criteria, Args&&... args) {
    std::lock_guard<std::mutex> lock(m_mtx);
    return InternalFirstThat(m_pRoot, std::forward<Func>(criteria), std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename ...Args>
typename CBinaryTree<Traits>::value_type* CBinaryTree<Traits>::InternalFirstThat(Node* pNode, Func&& criteria, Args&&... args) {
    if (!pNode) 
        return nullptr;

    if (criteria(pNode->GetValueRef(), std::forward<Args>(args)...))
        return &pNode->GetValueRef();
    
    value_type* foundInLeft = InternalFirstThat(pNode->m_pChild[0], std::forward<Func>(criteria), std::forward<Args>(args)...);
    if (foundInLeft) 
        return foundInLeft;

    return InternalFirstThat(pNode->m_pChild[1], std::forward<Func>(criteria), std::forward<Args>(args)...);
}

template <typename Traits>
std::ostream& operator<<(std::ostream& os, CBinaryTree<Traits>& tree) {
    tree.ForEach([&os](typename CBinaryTree<Traits>::value_type& value) {
        os << value << " ";
    });
    return os;
}

template <typename Traits>
std::istream& operator>>(std::istream& is, CBinaryTree<Traits>& tree) {
    typename CBinaryTree<Traits>::value_type value;
    is >> value;
    tree.Insert(std::move(value), 0);
    return is;
}



#endif // __BINARYTREE_H__