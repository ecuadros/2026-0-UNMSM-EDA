#ifndef __AVL_H__
#define __AVL_H__

#include "binarytree.h"
#include <string>

using namespace std;


template <typename Traits>
class CAVLTree : public CBinaryTree<Traits> {
    using Base = CBinaryTree<Traits>;
    using Node = typename Base::Node;
    using value_type = typename Base::value_type;
    using CompareFunc = typename Base::CompareFunc;

    // obtener la altura de un nodo
    static size_t height(Node *node) {
        return node ? node->m_height : 0;
    }

    void update_height(Node *node) {
        if (!node) return;
        size_t leftHeight = height(Base::child(node, 0));
        size_t rightHeight = height(Base::child(node, 1));
        // setea la altura segun el hijo mas profundo
        node->m_height = 1 + static_cast<Size>(leftHeight > rightHeight ? leftHeight : rightHeight);
    }
    // la resta de ambas alturas es el balanceo del nodo
    Size balance(Node *node) const {
        size_t leftHeight = height(Base::child(node, 0));
        size_t rightHeight = height(Base::child(node, 1));
        return static_cast<Size>(leftHeight) - static_cast<Size>(rightHeight);
    }

    // funciones para las rotaciones

    Node *rotate_right(Node *target) {
        Node *targetLeftChild = Base::child(target, 0);
        Node *targetLCRightChild = Base::child(targetLeftChild, 1);

        Base::child_ref(targetLeftChild, 1) = target;
        Base::child_ref(target, 0) = targetLCRightChild;

        update_height(target);
        update_height(targetLeftChild);
        return targetLeftChild;
    }

    Node *rotate_left(Node *target) {
        Node *targetRightChild = Base::child(target, 1);
        Node *targetRCLeftChild = Base::child(targetRightChild, 0);

        Base::child_ref(targetRightChild, 0) = target;
        Base::child_ref(target, 1) = targetRCLeftChild;

        update_height(target);
        update_height(targetRightChild);
        return targetRightChild;
    }

    // funcion interna para la insercion en AVL

    Node *insert_avl(Node *node, const value_type &val, ref_type ref) {
        if (!node) return new Node(val, ref);

        size_t path = comp(val, node->GetValue());
        // se inserta de manera recursiva para chequear en cada nivel del arbol
        Base::child_ref(node, path) = insert_avl(Base::child(node, path), val, ref);

        // se actualiza la altura
        update_height(node);
        const Size nodeBalance = balance(node);

        // Caso 1, el arbol pesa a la izquierda y el hijo cayo en el subarbol izquierdo
        if (nodeBalance > 1 && comp(val, Base::child(node, 0)->GetValue()))
            return rotate_right(node);
        // 2: el arbol pesa a la derecha y el hijo cayo en el subarbol derecho
        if (nodeBalance < -1 && !comp(val, Base::child(node, 1)->GetValue()))
            return rotate_left(node);
        // 3: pesa a la izquierda y el hijo cayo en el subarbol derecho
        if (nodeBalance > 1 && !comp(val, Base::child(node, 0)->GetValue())) {
            // se estabiliza el arbol derecho
            Base::child_ref(node, 0) = rotate_left(Base::child(node, 0));
            // y se rota a la derecha
            return rotate_right(node);
        }
        // 4: pesa a la derecha y el hijo cayo en el subarbol izquierdo
        if (nodeBalance < -1 && comp(val, Base::child(node, 1)->GetValue())) {
            Base::child_ref(node, 1) = rotate_right(Base::child(node, 1));
            return rotate_left(node);
        }

        return node;
    }
    // retorna el nodo mas a la derecha
    Node *max_node(Node *node) const {
        if (!node) return nullptr;
        while (Base::child(node, 1)) node = Base::child(node, 1);
        return node;
    }

    // funcion principal de la logica del remove
    Node *remove_avl(Node *node, const value_type &val, bool &removed, value_type &removed_val) {
        if (!node) return nullptr;

        // cuando se encuentra el valor objetivo
        if (node->GetValue() == val) {
            return remove_here(node, removed, removed_val);
        }
        // sino sigue buscando el valor objetivo
        size_t path = comp(val, node->GetValue());
        Base::child_ref(node, path) = remove_avl(Base::child(node, path), val, removed, removed_val);
        // rebalancear
        return rebalance(node);
    }

    Node *remove_here(Node *node, bool &removed, value_type &removed_val) {
        removed = true;
        removed_val = node->GetValue();

        // si no hay al menos un hijo se elimina y se retorna el sub-hijo, si lo hay
        if (!Base::child(node, 0) || !Base::child(node, 1)) {
            Node *child = Base::child(node, Base::child(node, 0) ? 0 : 1);
            delete node;
            return child;
        }
        // si lo hay, busca el maximo en el subarbol izquierdo
        Node *pred = max_node(Base::child(node, 0));
        // se toma referencias a los valores del maximo
        node->GetValueRef() = pred->GetValue();
        node->GetRefRef() = pred->GetRef();
        bool dummy_removed = false;
        value_type dummy_val {};
        // se hace remove_avl para que cuando actualice removed_val
        // se actualice el nodo objetivo por las referencia
        Base::child_ref(node, 0) = remove_avl(Base::child(node, 0), pred->GetValue(), dummy_removed, dummy_val);

        return rebalance(node);
    }

    // logica de balanceo para el remove
    // la logice en el insert es ligeramente diferente (se chequea a donde va el hijo)
    Node *rebalance(Node *node) {
        if (!node) return nullptr;
        // chequeo de balanceo
        update_height(node);
        Size nodeBalance = balance(node);

        if (nodeBalance > 1 && balance(Base::child(node, 0)))
            return rotate_right(node);
        if (nodeBalance > 1 && balance(Base::child(node, 0)) < 0) {
            Base::child_ref(node, 0) = rotate_left(Base::child(node, 0));
            return rotate_right(node);
        }
        if (nodeBalance < -1 && balance(Base::child(node, 1)) <= 0)
            return rotate_left(node);
        if (nodeBalance < -1 && balance(Base::child(node, 1)) > 0) {
            Base::child_ref(node, 1) = rotate_right(Base::child(node, 1));
            return rotate_left(node);
        }
        return node;
    }

public:
    CAVLTree() = default;
    CAVLTree(const CAVLTree &other) : Base(other) {}
    CAVLTree(CAVLTree &&other) noexcept : Base(std::move(other)) {}
    CAVLTree &operator=(const CAVLTree &other) {
        Base::operator=(other);
        return *this;
    }
    CAVLTree &operator=(CAVLTree &&other) noexcept {
        Base::operator=(std::move(other));
        return *this;
    }

    friend ostream &operator<<(ostream &os, CBinaryTree<Traits> &tree) {
        lock_guard<mutex> lock(tree.mtx);
        os << "AVL Tree [";
        tree._serialize_node(os, tree.m_pRoot);
        os << "]";
        return os;
    }

    friend istream &operator>>(istream &is, CBinaryTree<Traits> &tree) {
        if (!is) return is;

        string bar;  // dummy
        getline(is, bar, '[');
        lock_guard<mutex> lock(tree.mtx);  // seguro contra concurrencia
        tree._clear_unlocked();

        // deserializa todo
        tree.m_pRoot = tree._deserialize_node(is);
        // si el parseo salio mal, se limpia el arbol
        if (!is) tree._clear_unlocked();
        else {
            char ch;
            while (is.get(ch)) {
                if (ch == ']') break;
                if (ch == ',') continue;
                is.setstate(ios::failbit);
                tree._clear_unlocked();
                break;
            }
        }
        return is;
    }

    // overrides para las funciones insert y remove

    void Insert(const value_type &val, const ref_type ref) override {
        lock_guard lock(this->mtx);
        this->m_pRoot = insert_avl(this->m_pRoot, val, ref);
    }

    value_type remove(value_type &val) override {
        lock_guard lock(this->mtx);
        bool removed = false;
        value_type removed_val {};
        this->m_pRoot = remove_avl(this->m_pRoot, val, removed, removed_val);
        return removed ? removed_val : value_type {};
    }
};


#endif // __AVL_H__
