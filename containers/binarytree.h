#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <mutex>
#include <utility>
#include <string>
#include "../general/types.h"
#include "../util.h"
#include "GeneralIterator.h"


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

template <typename Traits> class CBinaryTree;
template <typename Traits>
class NodeBinaryTree{

    using value_type  = typename Traits::value_type;
    using Node        = NodeBinaryTree<Traits>;
    using CompareFunc = typename Traits::CompareFunc;

    friend class CBinaryTree<Traits>;

protected:
    value_type m_data;
    ref_type   m_ref;

    Node *m_pChild[2] = {nullptr};
    Node *m_pParent= nullptr;
    size_t m_height = 1; // Altura del nodo (excusivo AVL)

public:
    NodeBinaryTree()  {}

    NodeBinaryTree(value_type val, ref_type ref, Node* parent )
        : m_data(val), m_ref(ref), m_pParent(parent) {}

    value_type  GetValue() const { return m_data; }
    value_type& GetValueRef()    { return m_data; }

    ref_type  GetRef() const { return m_ref; }
    ref_type& GetRefRef()    { return m_ref; }

    Node* GetChild(size_t i) const { return m_pChild[i]; }
    Node* GetParent() const        { return m_pParent; }
};

template <typename Container>
class BinaryTreeForwardIterator : public GeneralIterator<Container> {
    using Node   = typename Container::Node;
    using Parent = GeneralIterator<Container>;

public:
    BinaryTreeForwardIterator(Container* container, Node* aux) : Parent(container,0) {
        this->m_data = aux;
    }

    BinaryTreeForwardIterator& operator++(){
        Node* aux = this->m_data;
        if(!aux) return *this;

        if(aux->GetChild(1)){
            aux = aux->GetChild(1);
            while(aux->GetChild(0))
                aux = aux->GetChild(0);
            this->m_data = aux;
        }else{
            Node* parent = aux->GetParent();
            while(parent && aux == parent->GetChild(1)){
                aux = parent;
                parent = parent->GetParent();
            }
            this->m_data = parent;
        }
        return *this;
    }

    typename Container::value_type& operator*(){
        return this->m_data->GetValueRef();
    }

    bool operator!=(const BinaryTreeForwardIterator& other) const {
        return this->m_data != other.m_data;
    }
};

template <typename Container>
class BinaryTreeBackwardIterator : public GeneralIterator<Container> {
    using Node   = typename Container::Node;
    using Parent = GeneralIterator<Container>;

public:
    BinaryTreeBackwardIterator(Container* container, Node* aux) : Parent(container,0) {
        this->m_data = aux;
    }

    BinaryTreeBackwardIterator& operator++(){
        Node* aux = this->m_data;
        if(!aux) return *this;

        if(aux->GetChild(0)){
            aux = aux->GetChild(0);
            while(aux->GetChild(1))
                aux = aux->GetChild(1);
            this->m_data = aux;
        }else{
            Node* parent = aux->GetParent();
            while(parent && aux == parent->GetChild(0)){
                aux = parent;
                parent = parent->GetParent();
            }
            this->m_data = parent;
        }
        return *this;
    }

    typename Container::value_type& operator*(){
        return this->m_data->GetValueRef();
    }

    bool operator!=(const BinaryTreeBackwardIterator& other) const {
        return this->m_data != other.m_data;
    }
};

template <typename Traits>
class CBinaryTree {

public:
    using value_type  = typename Traits::value_type;
    using Node        = NodeBinaryTree<Traits>;
    using CompareFunc = typename Traits::CompareFunc;

    using forward_iterator  = BinaryTreeForwardIterator<CBinaryTree<Traits>>;
    using backward_iterator = BinaryTreeBackwardIterator<CBinaryTree<Traits>>;

protected:
    Node* m_pRoot = nullptr;
    CompareFunc comp;
    mutable std::mutex m_mutex;

public:
    CBinaryTree() {}

    // Copy constructor
    CBinaryTree(const CBinaryTree<Traits> &another){
        std::lock_guard<std::mutex> lock(another.m_mutex);

        for(auto it = another.begin(); it != another.end(); ++it){
            this->Insert(*it, it.m_data->GetRef());
        }
    }

    // Move constructor
    template <typename Traits>
    CBinaryTree<Traits>::CBinaryTree(CBinaryTree<Traits> &&another) noexcept{
        std::lock_guard<std::mutex> lock(another.m_mutex);

        m_pRoot = std::exchange(another.m_pRoot, nullptr);
    }

    // Destructor
    virtual ~CBinaryTree(){
        std::lock_guard<std::mutex> lock(m_mutex);
        DeleteTreeRecursive(m_pRoot);
        m_pRoot = nullptr;
    }

    void Insert(const value_type& val, ref_type ref){
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalInsert(m_pRoot, nullptr, val, ref);
    }

    void Remove(const value_type& val){
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalRemove(m_pRoot, val);
    }

    forward_iterator begin(){
        Node* aux = m_pRoot;
        if(aux) while(aux->GetChild(0)) aux = aux->GetChild(0);
        return forward_iterator(this, aux);
    }

    forward_iterator end(){
        return forward_iterator(this, nullptr);
    }

    backward_iterator rbegin(){
        std::lock_guard<std::mutex> lock(m_mutex);

        Node* pMax = m_pRoot;
        if(pMax){
            while(pMax->GetChild(1))
                pMax = pMax->GetChild(1);
        }
        return backward_iterator(this, pMax);
    }

    backward_iterator rend(){
        return backward_iterator(this, nullptr);
    }


    template <typename Func, typename ...Args>
    void InOrder(Func fn, Args&&... args){
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalInOrder(m_pRoot, fn, std::forward<Args>(args)...);
    }

    template <typename Func, typename ...Args>
    void PreOrder(Func fn, Args&&... args){
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalPreOrder(m_pRoot, fn, std::forward<Args>(args)...);
    }

    template <typename Func, typename ...Args>
    void PostOrder(Func fn, Args&&... args){
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalPostOrder(m_pRoot, fn, std::forward<Args>(args)...);
    }

    template <typename Func, typename ...Args>
    void Foreach(Func fn, Args&&... args){
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalInOrder(m_pRoot, fn, std::forward<Args>(args)...);
    }

    template <typename Func, typename ...Args>
    forward_iterator FirstThat(Func fn, Args&&... args){
        std::lock_guard<std::mutex> lock(m_mutex);
        Node* encontrado = InternalFirstThat(m_pRoot, fn, std::forward<Args>(args)...);

        if(encontrado) return forward_iterator(this, encontrado);
        return end();
    }

    friend std::ostream& operator<<(std::ostream &os, CBinaryTree<Traits> &tree){
        std::lock_guard<std::mutex> lock(tree.m_mutex);

        os << "[ ";

        for(auto it = tree.begin(); it != tree.end(); ++it){
            os << "(" << *it << ":" << it.m_data->GetRef() << "), ";
        }

        os << "]";
        return os;
    }

    friend std::istream& operator>>(std::istream &is, CBinaryTree<Traits> &tree){
        std::lock_guard<std::mutex> lock(tree.m_mutex);

        tree.DeleteTreeRecursive(tree.m_pRoot);
        tree.m_pRoot = nullptr;

        size_t n;
        std::cout << "¿Cuántos elementos?: ";
        std::cin >> n;

        for(size_t i = 0; i < n; ++i){
            typename Traits::value_type val;
            ref_type ref;

            std::cout << "Elemento " << i+1 << " (value ref): ";
            std::cin >> val >> ref;

            tree.InternalInsert(tree.m_pRoot, nullptr, val, ref);
        }
        return is;
    }

private:

    void InternalInsert(Node*& node, Node* parent, const value_type& val, ref_type ref){
        if(!node){
            node = new Node(val, ref, parent);
            return;
        }
        bool path = comp(val, node->GetValue());
        InternalInsert(node->m_pChild[path], node, val, ref);
    }

    void InternalRemove(Node*& node, const value_type& val){
        if(!node) return;

        if(comp(val,node->GetValue()))
            InternalRemove(node->m_pChild[1], val);

        else if(comp(node->GetValue(),val))
            InternalRemove(node->m_pChild[0], val);

        else{
            if(node->m_pChild[0] && node->m_pChild[1]){
                Node* aux = node->m_pChild[1];
                while(aux->m_pChild[0])
                    aux = aux->m_pChild[0];

                node->GetValueRef() = aux->GetValue();
                node->GetRefRef()   = aux->GetRef();

                InternalRemove(node->m_pChild[1], aux->GetValue());
            }else{
                Node* eliminar = node;
                Node* child = node->m_pChild[0] ? node->m_pChild[0] : node->m_pChild[1];
                node = child;
                if(node) node->m_pParent = eliminar->m_pParent;
                delete eliminar;
            }
        }
    }

    void DeleteTreeRecursive(Node* node){
        if(!node) return;
        DeleteTreeRecursive(node->m_pChild[0]);
        DeleteTreeRecursive(node->m_pChild[1]);
        delete node;
    }

    template <typename Func, typename ...Args>
    void InternalInOrder(Node* node, Func fn, Args&&... args){
        if(!node) return;

        InternalInOrder(node->m_pChild[0], fn, std::forward<Args>(args)...);
        fn(node->GetValueRef(), std::forward<Args>(args)...);
        InternalInOrder(node->m_pChild[1], fn, std::forward<Args>(args)...);
    }

    template <typename Func, typename ...Args>
    void InternalPreOrder(Node* node, Func fn, Args&&... args){
        if(!node) return;

        fn(node->GetValueRef(), std::forward<Args>(args)...);
        InternalPreOrder(node->m_pChild[0], fn, std::forward<Args>(args)...);
        InternalPreOrder(node->m_pChild[1], fn, std::forward<Args>(args)...);
    }

    template <typename Func, typename ...Args>
    void InternalPostOrder(Node* node, Func fn, Args&&... args){
        if(!node) return;

        InternalPostOrder(node->m_pChild[0], fn, std::forward<Args>(args)...);
        InternalPostOrder(node->m_pChild[1], fn, std::forward<Args>(args)...);
        fn(node->GetValueRef(), std::forward<Args>(args)...);
    }

    
    template <typename Func, typename ...Args>
    Node* InternalFirstThat(Node* node, Func fn, Args&&... args){
        if(!node) return nullptr;

        Node* pFound = InternalFirstThat(node->m_pChild[0], fn, std::forward<Args>(args)...);
        if(pFound) return pFound;

        if(fn(node->GetValueRef(), std::forward<Args>(args)...))
            return node;

        return InternalFirstThat(node->m_pChild[1], fn, std::forward<Args>(args)...);
    }
};

void DemoBinaryTree();

#endif // __BINARYTREE_H__
