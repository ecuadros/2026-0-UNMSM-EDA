#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <mutex>
#include <stack>
#include "../general/types.h"
#include "../util.h"
#include <string>


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
template<typename Traits> class CBinaryTree; //para el uso del protected

template <typename Traits>
class NodeBinaryTree{
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;
public:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pChild[2]  = {nullptr, nullptr};
    size_t     m_height = 1;

    friend class CBinaryTree<Traits>;

public:
    NodeBinaryTree(){}
    NodeBinaryTree( value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref),  m_height(1){   }
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }
};

template <typename Traits>
class CBinaryTree {
public:
    using value_type = typename Traits::value_type;
    using Node = NodeBinaryTree<Traits>;
    using CompareFunc = typename Traits::CompareFunc;
    using size_type = size_t;
protected:
    Node *m_pRoot = nullptr;
    mutable std::mutex mutex;
    CompareFunc comp;

public:
    CBinaryTree() {
    }

    // TODO: Copy constructor
    CBinaryTree(CBinaryTree &another) {
        std::lock_guard <std::mutex> lock(another.mutex);
        m_pRoot = InternalCopy(another.m_pRoot);

    }

    // TODO: Move constructor
    CBinaryTree(CBinaryTree &&another)

    noexcept{
        std::lock_guard <std::mutex> lock(another.mutex);
        m_pRoot = another.m_pRoot;
        another.m_pRoot = nullptr;
    }

    //destructor seguro
    virtual ~CBinaryTree() {
        std::lock_guard <std::mutex> lock(mutex);
        DestroyTree(m_pRoot);
    }
    void Print2D(Node* nodo, std::string prefix = "", bool isRight = true, bool isRoot = true) const {
        // si el nodo es null, no hacemos nada
        if (nodo == nullptr) return;
        Print2D(nodo->m_pChild[1], prefix + (isRoot ? "" : (isRight ? "    " : "|   ")), false, false);
        cout << prefix;
        if(!isRoot){
          cout<<(isRight ? " \\-- " : " /-- ");
        }
        cout << nodo->GetValue() << "\n";
        Print2D(nodo->m_pChild[0], prefix + (isRoot ? "" : (isRight ? "|   " : "    ")), true, false);
    }

private:
    // Clonación profunda para el Constructor Copia
    Node *InternalCopy(Node *pNode) {
        if (!pNode) return nullptr;
        Node *newNode = new Node(pNode->GetValue(), pNode->m_ref);
        newNode->m_pChild[0] = InternalCopy(pNode->m_pChild[0]);
        newNode->m_pChild[1] = InternalCopy(pNode->m_pChild[1]);
        return newNode;
    }

    void DestroyTree(Node *pNode) {
        if (pNode) {
            DestroyTree(pNode->m_pChild[0]);
            DestroyTree(pNode->m_pChild[1]);
            delete pNode;
        }
    }

    //insert y remove
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref) {
        if (!rParent) {
            rParent = new Node(val, ref);
            return;
        }
        if (val == rParent->GetValue()) return;
        auto path = comp(val, rParent->GetValue());
        InternalInsert(rParent->m_pChild[path], val, ref);
    }

    void InternalRemove(Node *&rParent, const value_type &val) {
        if (!rParent) return;
        if (val == rParent->GetValue()) {
            if (!rParent->m_pChild[0] && !rParent->m_pChild[1]) {
                delete rParent;
                rParent = nullptr;
            } else if (!rParent->m_pChild[0]) {
                Node *temp = rParent;
                rParent = rParent->m_pChild[1];
                delete temp;
            } else if (!rParent->m_pChild[1]) {
                Node *temp = rParent;
                rParent = rParent->m_pChild[0];
                delete temp;
            } else {
                Node *&replace = FindMin(rParent->m_pChild[1]);
                rParent->GetValueRef() = replace->GetValue();
                InternalRemove(replace, replace->GetValue());
            }
            return;
        }
        auto path = comp(val, rParent->GetValue());
        InternalRemove(rParent->m_pChild[path], val);
    }
    Node*& FindMin(Node*& node) {
        if (!node->m_pChild[0]) return node;
        return FindMin(node->m_pChild[0]);
    }

//Recorridos Inorden, Preorden y Postorden
    //recorrido Inorden  IZQ-RAIZ-DER
    template<typename Callable, typename... Args>
    void InternalInorden(Node* pNode, Callable func, Args&&... args){
        if(!pNode) return;
        InternalInorden(pNode->m_pChild[0], func, std::forward<Args>(args)...);     //IZQUIERDA
        func(pNode->GetValueRef(), std::forward<Args>(args)...);                       //RAIZ
        InternalInorden(pNode->m_pChild[1], func, std::forward<Args>(args)...);     //DERECHA
    }
    //recorrido Preorden RAIZ-IZQ-DER
    template<typename Callable, typename... Args>
    void InternalPreorden(Node* pNode, Callable func, Args&&... args){
        if(!pNode) return;
        func(pNode->GetValueRef(), std::forward<Args>(args)...);                // RAIZ
        InternalPreorden(pNode->m_pChild[0], func, std::forward<Args>(args)...);// IZQ
        InternalPreorden(pNode->m_pChild[1], func, std::forward<Args>(args)...);// DER
    }
    //recorrido Postorden IZQ-DER-RAIZ
    template<typename Callable, typename... Args>
    void InternalPostorden(Node* pNode, Callable func, Args&&... args){
        if(!pNode) return;
        InternalPostorden(pNode->m_pChild[0], func, std::forward<Args>(args)...); // IZQ
        InternalPostorden(pNode->m_pChild[1], func, std::forward<Args>(args)...); // DER
        func(pNode->GetValueRef(), std::forward<Args>(args)...);                  // RAIZ
    }
    //FistThat
    template<typename Callable, typename... Args>
    value_type* InternalFirstThat(Node* pNode, Callable func, Args&&... args){
        if(!pNode) return nullptr;
        // Evalua la raiz primero
        if(func(pNode->GetValueRef(), std::forward<Args>(args)...)) return &(pNode->GetValueRef());

        // Busca en la izquierda
        if(auto res = InternalFirstThat(pNode->m_pChild[0], func, std::forward<Args>(args)...)) return res;

        // Busca en la derecha
        return InternalFirstThat(pNode->m_pChild[1], func, std::forward<Args>(args)...);
    }
public:
    void Insert(const value_type &val, ref_type ref = 0){
        std::lock_guard<std::mutex> lock(mutex);
        InternalInsert(m_pRoot, val, ref);
    }
    void Remove(const value_type &val){
        std::lock_guard<std::mutex> lock(mutex);
        InternalRemove(m_pRoot, val);
    }

    //variadic template
    template <typename Callable, typename... Args>
    void Inorden(Callable func, Args&&... args){
        std::lock_guard<std::mutex> lock(mutex);
        InternalInorden(m_pRoot, func, std::forward<Args>(args)...);
    }
    template <typename Callable, typename... Args>
    void Preorden(Callable func, Args&&... args) {
        std::lock_guard <std::mutex> lock(mutex);
        InternalPreorden(m_pRoot, func, std::forward<Args>(args)...);
    }
    template <typename Callable, typename... Args>
    void Postorden(Callable func, Args&&... args) {
        std::lock_guard <std::mutex> lock(mutex);
        InternalPostorden(m_pRoot, func, std::forward<Args>(args)...);
    }
    template<typename Callable, typename... Args>
    void ForEach(Callable func, Args&&... args) {
        Inorden(func, std::forward<Args>(args)...);
    }

    template <typename Callable, typename... Args>
    value_type* FirstThat(Callable func, Args&&... args) {
        std::lock_guard <std::mutex> lock(mutex);
        return InternalFirstThat(m_pRoot, func, std::forward<Args>(args)...);
    }
    //iteradores begin() y end()
    //uso de forech y firstTaht
    class CForwardIterator {
        std::stack<Node*> pila;
        void push_all_left(Node* p) { while(p) { pila.push(p); p = p->m_pChild[0]; } }
    public:
        CForwardIterator(Node* root) { push_all_left(root); }
        CForwardIterator() = default;
        value_type& operator*() { return pila.top()->GetValueRef(); }
        CForwardIterator& operator++() {
            Node* p = pila.top(); pila.pop();
            push_all_left(p->m_pChild[1]);
            return *this;
        }
        bool operator!=(const CForwardIterator& other) const {
            if(pila.empty() && other.pila.empty()) return false;
            if(pila.empty() || other.pila.empty()) return true;
            return pila.top() != other.pila.top();
        }
    };
    class CBackwardIterator {
        std::stack<Node*> pila;
        void push_all_right(Node* p) { while(p) { pila.push(p); p = p->m_pChild[1]; } }
    public:
        CBackwardIterator(Node* root) { push_all_right(root); }
        CBackwardIterator() = default;
        value_type& operator*() { return pila.top()->GetValueRef(); }
        CBackwardIterator& operator++() {
            Node* p = pila.top(); pila.pop();
            push_all_right(p->m_pChild[0]);
            return *this;
        }
        bool operator!=(const CBackwardIterator& other) const {
            if(pila.empty() && other.pila.empty()) return false;
            if(pila.empty() || other.pila.empty()) return true;
            return pila.top() != other.pila.top();
        }
    };

    CForwardIterator begin() { return CForwardIterator(m_pRoot); }
    CForwardIterator end() { return CForwardIterator(); }

    CBackwardIterator rbegin() { return CBackwardIterator(m_pRoot); }
    CBackwardIterator rend() { return CBackwardIterator(); }

    //operadores whirte y read
    template<typename U>
    friend ostream& operator<<(ostream& os, CBinaryTree<U>& tree) {
        os << "Tree (Inorden): "<<endl;
        tree.Print2D(tree.m_pRoot);
        os << endl;
        return os;
    }

    friend istream& operator>>(istream& is, CBinaryTree<Traits>& tree) {
        value_type val;
        cout << "Ingrese valor para Tree: ";
        is >> val;
        tree.Insert(val);
        return is;
    }
};


#endif // __BINARYTREE_H__