#ifndef __BINARYTREEAVL_H__
#define __BINARYTREEAVL_H__

#include "binarytree.h"

template <typename T_Traits>
class CBinaryTreeAVL : public CBinaryTree<T_Traits> {
    using T_Nodo = typename CBinaryTree<T_Traits>::Nodo;
    using T_Dato = typename T_Traits::T_Dato;

public:
    void Insert(const T_Dato& v) override {
        lock_guard<mutex> lock(this->m_mtx); // Mantiene la concurrencia del padre
        insAVL(this->m_raiz, v);
    }

private:
    int h(T_Nodo* n) { return n ? 1 + max(h(n->hijos[0]), h(n->hijos[1])) : 0; }
    int b(T_Nodo* n) { return n ? h(n->hijos[0]) - h(n->hijos[1]) : 0; }

    void girar(T_Nodo*& n, bool der) {
        T_Nodo* temp = n->hijos[!der];
        n->hijos[!der] = temp->hijos[der];
        temp->hijos[der] = n;
        n = temp;
    }

    void balancear(T_Nodo*& n) {
        if (!n) return;
        int factor = b(n);
        if (factor > 1) {
            if (b(n->hijos[0]) < 0) girar(n->hijos[0], true);
            girar(n, false);
        } else if (factor < -1) {
            if (b(n->hijos[1]) > 0) girar(n->hijos[1], false);
            girar(n, true);
        }
    }

    void insAVL(T_Nodo*& n, const T_Dato& v) {
        if (!n) { n = new T_Nodo(v); return; }
        insAVL(n->hijos[v > n->valor], v);
        balancear(n);
    }
};

#endif