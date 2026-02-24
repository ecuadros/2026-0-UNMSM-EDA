#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <mutex>
#include <functional>
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
class NodeBinaryTree{
public:
    using value_type = typename Traits::value_type;
    using Node       = NodeBinaryTree<Traits>;

    value_type m_data;
    ref_type   m_ref;
    Node* m_pChild[2] = {nullptr, nullptr};

    NodeBinaryTree(value_type value, ref_type ref = -1)
        : m_data(value), m_ref(ref) {}

    value_type  GetValue() const { return m_data; }
    value_type& GetValueRef() { return m_data; }
};

template <typename Traits>
class CBinaryTree{
public:
    using value_type  = typename Traits::value_type;
    using Node        = NodeBinaryTree<Traits>;
    using CompareFunc = typename Traits::CompareFunc;

private:
    Node* m_pRoot = nullptr;
    CompareFunc comp;
    mutable std::mutex mtx;

public:
    CBinaryTree() = default;

    // Copy constructor
    CBinaryTree(const CBinaryTree& other){
        std::lock_guard<std::mutex> lock(other.mtx);
        m_pRoot = Copy(other.m_pRoot);
    }

    // Move constructor
    CBinaryTree(CBinaryTree&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.mtx);
        m_pRoot = other.m_pRoot;
        other.m_pRoot = nullptr;
    }

    // Destructor seguro
    ~CBinaryTree(){
        std::lock_guard<std::mutex> lock(mtx);
        Clear(m_pRoot);
    }

private:

    Node* Copy(Node* node){
        if(!node) return nullptr;
        Node* newNode = new Node(node->m_data, node->m_ref);
        newNode->m_pChild[0] = Copy(node->m_pChild[0]);
        newNode->m_pChild[1] = Copy(node->m_pChild[1]);
        return newNode;
    }

    void Clear(Node* node){
        if(!node) return;
        Clear(node->m_pChild[0]);
        Clear(node->m_pChild[1]);
        delete node;
    }

    void InternalInsert(Node*& rParent, const value_type &val, ref_type ref){
        if(!rParent){
            rParent = new Node(val, ref);
            return;
        }
        auto path = comp(val, rParent->GetValue());
        InternalInsert(rParent->m_pChild[path], val, ref);
    }

    Node* InternalRemove(Node* node, const value_type& val){
        if(!node) return nullptr;

        if(node->m_data == val){
            if(!node->m_pChild[0]){
                Node* temp = node->m_pChild[1];
                delete node;
                return temp;
            }
            if(!node->m_pChild[1]){
                Node* temp = node->m_pChild[0];
                delete node;
                return temp;
            }
            Node* successor = node->m_pChild[1];
            while(successor->m_pChild[0])
                successor = successor->m_pChild[0];
            node->m_data = successor->m_data;
            node->m_pChild[1] = InternalRemove(node->m_pChild[1], successor->m_data);
        }
        else{
            auto path = comp(val, node->m_data);
            node->m_pChild[path] = InternalRemove(node->m_pChild[path], val);
        }
        return node;
    }

    template<typename Func, typename... Args>
    void Inorder(Node* node, Func f, Args... args){
        if(!node) return;
        Inorder(node->m_pChild[0], f, args...);
        f(node->m_data, args...);
        Inorder(node->m_pChild[1], f, args...);
    }

    template<typename Func, typename... Args>
    void Preorder(Node* node, Func f, Args... args){
        if(!node) return;
        f(node->m_data, args...);
        Preorder(node->m_pChild[0], f, args...);
        Preorder(node->m_pChild[1], f, args...);
    }

    template<typename Func, typename... Args>
    void Postorder(Node* node, Func f, Args... args){
        if(!node) return;
        Postorder(node->m_pChild[0], f, args...);
        Postorder(node->m_pChild[1], f, args...);
        f(node->m_data, args...);
    }

    template<typename Predicate, typename... Args>
    Node* FirstThat(Node* node, Predicate pred, Args... args){
        if(!node) return nullptr;
        if(pred(node->m_data, args...))
            return node;
        Node* left = FirstThat(node->m_pChild[0], pred, args...);
        if(left) return left;
        return FirstThat(node->m_pChild[1], pred, args...);
    }

    Size Count(Node* node) const{
        if(!node) return 0;
        return 1 + Count(node->m_pChild[0]) + Count(node->m_pChild[1]);
    }

public:

    void Insert(const value_type &val, ref_type ref){
        std::lock_guard<std::mutex> lock(mtx);
        InternalInsert(m_pRoot, val, ref);
    }

    void Remove(const value_type &val){
        std::lock_guard<std::mutex> lock(mtx);
        m_pRoot = InternalRemove(m_pRoot, val);
    }

    template<typename Func, typename... Args>
    void Inorder(Func f, Args... args){
        std::lock_guard<std::mutex> lock(mtx);
        Inorder(m_pRoot, f, args...);
    }

    template<typename Func, typename... Args>
    void Preorder(Func f, Args... args){
        std::lock_guard<std::mutex> lock(mtx);
        Preorder(m_pRoot, f, args...);
    }

    template<typename Func, typename... Args>
    void Postorder(Func f, Args... args){
        std::lock_guard<std::mutex> lock(mtx);
        Postorder(m_pRoot, f, args...);
    }

    template<typename Func, typename... Args>
    void ForEach(Func f, Args... args){
        Inorder(f, args...);
    }

    template<typename Predicate, typename... Args>
    value_type* FirstThat(Predicate pred, Args... args){
        std::lock_guard<std::mutex> lock(mtx);
        Node* result = FirstThat(m_pRoot, pred, args...);
        return result ? &result->m_data : nullptr;
    }

    bool operator<(const CBinaryTree& other) const{
        std::scoped_lock lock(mtx, other.mtx);
        return Count(m_pRoot) < other.Count(other.m_pRoot);
    }

    bool operator>(const CBinaryTree& other) const{
        std::scoped_lock lock(mtx, other.mtx);
        return Count(m_pRoot) > other.Count(other.m_pRoot);
    }
};

#endif