#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include "../general/types.h"
#include "../util.h"
#include <mutex>
#include <vector>

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
    // Permitir que CBinaryTree acceda a los miembros privados de NodeBinaryTree
    template <typename T>
    friend class CBinaryTree;

    using  value_type  = typename Traits::value_type;
    using  Node        = NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;

private:
    value_type m_data;
    ref_type   m_ref;
    Node *m_pChild[2]  = {nullptr, nullptr};
public:
    NodeBinaryTree(){}
    NodeBinaryTree( value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref){   }
    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }

    // Agregar un campo para almacenar la referencia
    ref_type GetRef() const { return m_ref; }
};

template <typename Traits>
class CBinaryTree{
public:
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeBinaryTree<Traits>;
    using  CompareFunc = typename Traits::CompareFunc;

private:
    Node *m_pRoot = nullptr;
    CompareFunc comp;

    std::mutex mtx; // Mutex para operaciones concurrentes

    mutable std::vector<value_type> m_cache; // Cache para iteradores
public:
    CBinaryTree(){}

    // TODO: Copy constructor
    CBinaryTree(const CBinaryTree &another){
        m_pRoot = Copy(another.m_pRoot);
        comp = another.comp;
    }

    // TODO: Move constructor
    CBinaryTree(CBinaryTree &&another){
        m_pRoot = another.m_pRoot;
        comp = another.comp;
        another.m_pRoot = nullptr;
    }

    //Destructor
    ~CBinaryTree(){
        clear(m_pRoot);
    }

    void Remove(const value_type &val){
        std::lock_guard<std::mutex> lock(mtx); // Bloquea el mutex durante la operación
        m_pRoot = InternalRemove(m_pRoot, val);
    }

    //Inorder variadic
    template <typename... Func>
    void InOrder(Func... funcs){
        InOrder(m_pRoot, funcs...);
    }

    //Preorder variadic
    template <typename... Func>
    void PreOrder(Func... funcs){
        PreOrder(m_pRoot, funcs...);
    }

    //Postorder variadic
    template <typename... Func>
    void PostOrder(Func... funcs){
        PostOrder(m_pRoot, funcs...);
    }

    //ForEach
    template <typename... Func>
    void ForEach(Func... funcs) {
        InOrder(funcs...);
    }

    //FirstThat
    template <typename Predicate>
    value_type* FirstThat(Predicate pred) {
        return FirstThatInternal(m_pRoot, pred);
    }

    //Operador de acceso <<
    bool operator<<(const CBinaryTree &other)const{
        if(!m_pRoot || !other.m_pRoot){
            return false; // No se puede comparar si alguno de los árboles está vacío
        }

        return m_pRoot->GetValue() == other.m_pRoot->GetValue();
    }

    //Operador de acceso >>
    bool operator>>(const CBinaryTree &other)const{
        if(!m_pRoot || !other.m_pRoot){
            return false; // No se puede comparar si alguno de los árboles está vacío
        }

        return m_pRoot->GetValue() > other.m_pRoot->GetValue();
    }

    // Método para convertir el árbol a un vector (in-order)
    std::vector<value_type> ToVector() const{
        std::vector<value_type> result;
        ToVectorInternal(m_pRoot, result);
        return result;
    }

    void ToVectorInternal(Node* node, std::vector<value_type>& vec) const {
        if (!node) {
            return;
        }
        ToVectorInternal(node->m_pChild[0], vec);
        vec.push_back(node->GetValue());
        ToVectorInternal(node->m_pChild[1], vec);
    }

    // Iteradores
    auto begin(){
        m_cache = ToVector(); // Cache del árbol en un vector
        return m_cache.begin();
    }

    auto end(){
        return m_cache.end();
    }

    auto rbegin(){
        m_cache = ToVector(); // Cache del árbol en un vector
        return m_cache.rbegin();
    }

    auto rend(){
        return m_cache.rend();
    }

    // Rotaciones
    Node* RotateRight(Node* y) {
        Node* x = y->m_pChild[0];
        Node* T2 = x->m_pChild[1];

        // Realizar rotación
        x->m_pChild[1] = y;
        y->m_pChild[0] = T2;

        return x; // Nueva raíz después de la rotación
    }

    Node* RotateLeft(Node* x) {
        Node* y = x->m_pChild[1];
        Node* T2 = y->m_pChild[0];

        // Realizar rotación
        y->m_pChild[0] = x;
        x->m_pChild[1] = T2;

        return y; // Nueva raíz después de la rotación
    }

private:
    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref){
        if( !rParent ){
            rParent = new Node(val, ref);
            return;
        }
        auto path = comp(val, rParent->GetValue());
        InternalInsert(rParent->m_pChild[path], val, ref);
    }
public:
    void Insert(const value_type &val, ref_type ref){
        std::lock_guard<std::mutex> lock(mtx); // Bloquea el mutex durante la operación
        InternalInsert(m_pRoot, val, ref);
    }

//Lo nuevo que se esta implementando
private:
    void clear(Node* node){
        if (!node) {
            return;
        }
        clear(node->m_pChild[0]);
        clear(node->m_pChild[1]);
        delete node;
    }

    Node* Copy(Node* node) {
        if (!node) {
            return nullptr;
        }
        Node* newNode = new Node(node->GetValue(), node->GetRef());
        newNode->m_pChild[0] = Copy(node->m_pChild[0]);
        newNode->m_pChild[1] = Copy(node->m_pChild[1]);
        return newNode;
    }

    Node* FindMin(Node* node) {
        while (node && node->m_pChild[0]) {
            node = node->m_pChild[0];
        }
        return node;
    }

    Node* InternalRemove(Node* node, const value_type& val) {
        if (!node) return nullptr;
        auto path = comp(val, node->GetValue());

        if (val == node->GetValue()) {

            // Caso 1: sin hijo izquierdo
            if (!node->m_pChild[0]) {
                Node* temp = node->m_pChild[1];
                delete node;
                return temp;
            }

            // Caso 2: sin hijo derecho
            if (!node->m_pChild[1]) {
                Node* temp = node->m_pChild[0];
                delete node;
                return temp;
            }

            // Caso 3: dos hijos
            Node* successor = node->m_pChild[1];

            while (successor->m_pChild[0]) {
                successor = successor->m_pChild[0];
            }

            node->m_data = successor->m_data;
            node->m_ref  = successor->m_ref;

            node->m_pChild[1] =
                InternalRemove(node->m_pChild[1], successor->GetValue());
        }
        else {
            node->m_pChild[path] =
                InternalRemove(node->m_pChild[path], val);
        }

        return node;
    }

    //Inorder variadic
    template <typename... Func>
    void InOrder(Node* node, Func... funcs) {
        if (!node) {
            return;
        }
        
        InOrder(node->m_pChild[0], funcs...);
        (funcs(node->GetValue()), ...); // Llama a cada función con el valor del nodo
        InOrder(node->m_pChild[1], funcs...);
    }

    //Preorder variadic
    template <typename... Func>
    void PreOrder(Node* node, Func... funcs) {
        if (!node) {
            return;
        }
        
        (funcs(node->GetValue()), ...); // Llama a cada función con el valor del nodo
        PreOrder(node->m_pChild[0], funcs...);
        PreOrder(node->m_pChild[1], funcs...);
    }

    //Postorder variadic
    template <typename... Func>
    void PostOrder(Node* node, Func... funcs) {
        if (!node) {
            return;
        }
        
        PostOrder(node->m_pChild[0], funcs...);
        PostOrder(node->m_pChild[1], funcs...);
        (funcs(node->GetValue()), ...); // Llama a cada función con el valor del nodo
    }

    //FirstThat Internal
    template <typename Predicate>
    value_type* FirstThatInternal(Node* node, Predicate pred) {
        if (!node) {
            return nullptr;
        }
        
        auto left = FirstThatInternal(node->m_pChild[0], pred);
        if (left) {
            return left;
        }

        if (pred(node->GetValue())) {
            return &(node->GetValueRef());
        }

        return FirstThatInternal(node->m_pChild[1], pred);
    }
};


#endif // __BINARYTREE_H__