#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <mutex>
#include <algorithm>
#include "../general/types.h"
#include "../util.h"

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
class NodeBinaryTree {
public:
    using value_type = typename Traits::value_type;
    using Node       = NodeBinaryTree<Traits>;

    value_type m_data;
    ref_type   m_ref;
    Node* m_pChild[2] = {nullptr, nullptr};
    Node* m_pParent   = nullptr;
    int   m_height    = 1;

    NodeBinaryTree(value_type _value, ref_type _ref = -1, Node* _parent = nullptr)
        : m_data(_value), m_ref(_ref), m_pParent(_parent) {}

    value_type  GetValue() const { return m_data; }
    value_type& GetValueRef()    { return m_data; }
};

template <typename Traits>
class CBinaryTree {
public:
    using value_type  = typename Traits::value_type;
    using Node        = NodeBinaryTree<Traits>;
    using CompareFunc = typename Traits::CompareFunc;

    class Iterator {
        Node* m_pCurrent;
    public:
        Iterator(Node* pNode = nullptr) : m_pCurrent(pNode) {}
        value_type& operator*() { return m_pCurrent->m_data; }
        bool operator!=(const Iterator& other) const { return m_pCurrent != other.m_pCurrent; }
        bool operator==(const Iterator& other) const { return m_pCurrent == other.m_pCurrent; }
        
        Iterator& operator++() {
            if (m_pCurrent->m_pChild[1]) {
                m_pCurrent = m_pCurrent->m_pChild[1];
                while (m_pCurrent->m_pChild[0]) m_pCurrent = m_pCurrent->m_pChild[0];
            } else {
                Node* pParent = m_pCurrent->m_pParent;
                while (pParent && m_pCurrent == pParent->m_pChild[1]) {
                    m_pCurrent = pParent; pParent = pParent->m_pParent;
                }
                m_pCurrent = pParent;
            }
            return *this;
        }

        Iterator& operator--() {
            if (m_pCurrent->m_pChild[0]) {
                m_pCurrent = m_pCurrent->m_pChild[0];
                while (m_pCurrent->m_pChild[1]) m_pCurrent = m_pCurrent->m_pChild[1];
            } else {
                Node* pParent = m_pCurrent->m_pParent;
                while (pParent && m_pCurrent == pParent->m_pChild[0]) {
                    m_pCurrent = pParent; pParent = pParent->m_pParent;
                }
                m_pCurrent = pParent;
            }
            return *this;
        }
    };

private:
    Node* m_pRoot = nullptr;
    CompareFunc comp;
    mutable std::mutex mtx;

    int GetHeight(Node* n) { return n ? n->m_height : 0; }
    
    void UpdateHeight(Node* n) {
        if (n) n->m_height = 1 + std::max(GetHeight(n->m_pChild[0]), GetHeight(n->m_pChild[1]));
    }

    void Rotate(Node*& n, int side) {
        Node* temp = n->m_pChild[1 - side];
        n->m_pChild[1 - side] = temp->m_pChild[side];
        if (temp->m_pChild[side]) temp->m_pChild[side]->m_pParent = n;
        
        temp->m_pParent = n->m_pParent;
        temp->m_pChild[side] = n;
        n->m_pParent = temp;
        
        UpdateHeight(n);
        UpdateHeight(temp);
        n = temp;
    }

    void Balance(Node*& n) {
        if (!n) return;
        UpdateHeight(n);
        int bf = GetHeight(n->m_pChild[0]) - GetHeight(n->m_pChild[1]);
        
        if (bf > 1) {
            if (GetHeight(n->m_pChild[0]->m_pChild[0]) < GetHeight(n->m_pChild[0]->m_pChild[1])) {
                Rotate(n->m_pChild[0], 0);
            }
            Rotate(n, 1);
        } else if (bf < -1) { 
            if (GetHeight(n->m_pChild[1]->m_pChild[1]) < GetHeight(n->m_pChild[1]->m_pChild[0])) {
                Rotate(n->m_pChild[1], 1);
            }
            Rotate(n, 0);
        }
    }

    void InternalInsert(Node*& rParent, Node* pActualParent, const value_type& val, ref_type ref) {
        if (!rParent) {
            rParent = new Node(val, ref, pActualParent);
            return;
        }
        int path = comp(val, rParent->m_data);
        InternalInsert(rParent->m_pChild[path], rParent, val, ref);
    
        Balance(rParent);
    }

    Node* GetMin(Node* node) {
        while (node && node->m_pChild[0]) node = node->m_pChild[0];
        return node;
    }

    void InternalRemove(Node*& rNode, const value_type& val) {
        if (!rNode) return;

        if (comp(val, rNode->m_data)) { 
            InternalRemove(rNode->m_pChild[1], val);
        } else if (comp(rNode->m_data, val)) {
            InternalRemove(rNode->m_pChild[0], val);
        } else {
            if (!rNode->m_pChild[0] || !rNode->m_pChild[1]) {
                Node* temp = rNode->m_pChild[0] ? rNode->m_pChild[0] : rNode->m_pChild[1];
                if (temp) temp->m_pParent = rNode->m_pParent;
                delete rNode;
                rNode = temp;
            } else {
                Node* successor = GetMin(rNode->m_pChild[1]);
                rNode->m_data = successor->m_data;
                rNode->m_ref = successor->m_ref;
                InternalRemove(rNode->m_pChild[1], successor->m_data);
            }
        }
        
        if (rNode) Balance(rNode);
    }

    Node* CopyRecursive(Node* otherNode, Node* parent = nullptr) {
        if (!otherNode) return nullptr;
        Node* newNode = new Node(otherNode->m_data, otherNode->m_ref, parent);
        newNode->m_height = otherNode->m_height;
        newNode->m_pChild[0] = CopyRecursive(otherNode->m_pChild[0], newNode);
        newNode->m_pChild[1] = CopyRecursive(otherNode->m_pChild[1], newNode);
        return newNode;
    }

    void ClearRecursive(Node* node) {
        if (!node) return;
        ClearRecursive(node->m_pChild[0]);
        ClearRecursive(node->m_pChild[1]);
        delete node;
    }

public:
    CBinaryTree() = default;

    CBinaryTree(const CBinaryTree& other) {
        std::lock_guard<std::mutex> lock(other.mtx);
        m_pRoot = CopyRecursive(other.m_pRoot);
    }

    CBinaryTree(CBinaryTree&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.mtx);
        m_pRoot = other.m_pRoot;
        other.m_pRoot = nullptr;
    }

    CBinaryTree& operator=(const CBinaryTree& other) {
        if (this != &other) {
            std::unique_lock<std::mutex> lock1(mtx, std::defer_lock);
            std::unique_lock<std::mutex> lock2(other.mtx, std::defer_lock);
            std::lock(lock1, lock2); 

            ClearRecursive(m_pRoot); 
            m_pRoot = CopyRecursive(other.m_pRoot);
        }
        return *this;
    }

     CBinaryTree& operator=(CBinaryTree&& other) noexcept {
        if (this != &other) {
            std::unique_lock<std::mutex> lock1(mtx, std::defer_lock);
            std::unique_lock<std::mutex> lock2(other.mtx, std::defer_lock);
            std::lock(lock1, lock2);

            ClearRecursive(m_pRoot);
            m_pRoot = other.m_pRoot;
            other.m_pRoot = nullptr;
        }
        return *this;
    }

    virtual ~CBinaryTree() {
        std::lock_guard<std::mutex> lock(mtx);
        ClearRecursive(m_pRoot);
    }

    void Insert(const value_type& val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(mtx);
        InternalInsert(m_pRoot, nullptr, val, ref);
    }

    void Remove(const value_type& val) {
        std::lock_guard<std::mutex> lock(mtx);
        InternalRemove(m_pRoot, val);
    }

    bool Find(const value_type& val) const {
        std::lock_guard<std::mutex> lock(mtx);
        Node* curr = m_pRoot;
        while (curr) {
            if (val == curr->m_data) return true;
            curr = curr->m_pChild[comp(val, curr->m_data)];
        }
        return false;
    }

    Iterator begin() { 
        Node* curr = m_pRoot;
        while (curr && curr->m_pChild[0]) curr = curr->m_pChild[0];
        return Iterator(curr); 
    }
    Iterator end() { return Iterator(nullptr); }

    template<typename Func, typename... Args>
    void ForEach(Func f, Args... args) {
        std::lock_guard<std::mutex> lock(mtx);
        for(auto it = begin(); it != end(); ++it) f(*it, args...);
    }

    friend std::ostream& operator<<(std::ostream& os, CBinaryTree& tree) {
        tree.ForEach([&os](value_type& val) { os << val << " "; });
        return os;
    }

    friend std::istream& operator>>(std::istream& is, CBinaryTree& tree) {
        value_type val;
        if (is >> val) tree.Insert(val);
        return is;
    }
};

#endif