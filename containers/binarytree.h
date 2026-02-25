#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include <iostream>
#include <mutex>          // Punto 12: Concurrencia (mutex)
#include <stack>
#include <vector>
#include "../general/types.h"
#include "GeneralIterator.h"

using namespace std;

template <typename T_Traits>
class CBinaryTree {
public:
    using T_Dato = typename T_Traits::T_Dato;
    
    struct Nodo {
        T_Dato valor;
        Nodo *hijos[2]; // 0: Izq, 1: Der
        Nodo(T_Dato v) : valor(v) { hijos[0] = hijos[1] = nullptr; }
    };

protected:
    Nodo* m_raiz = nullptr;
    mutable mutex m_mtx; // Punto 12: Concurrencia

public:
    // Puntos 1 y 3: Constructor y Destructor
    CBinaryTree() = default;
    virtual ~CBinaryTree() { limpiar(m_raiz); }

    // Punto 2: Move Constructor
    CBinaryTree(CBinaryTree&& otro) noexcept {
        lock_guard<mutex> lock(otro.m_mtx);
        m_raiz = otro.m_raiz;
        otro.m_raiz = nullptr;
    }

    // Puntos 4 y 5: Iteradores (Forward y Backward)
    // Nota: Usamos una técnica de recolección simple para el ejemplo
    auto begin() { return forward_iterator(m_raiz); }
    auto end() { return forward_iterator(nullptr); }
    auto rbegin() { return backward_iterator(m_raiz); }
    auto rend() { return backward_iterator(nullptr); }

    // Puntos 6, 7 y 8: Recorridos con Variadic (In/Pre/Post)
    template <typename... Fs>
    void inorden(Fs&&... fns) { 
        lock_guard<mutex> lock(m_mtx); 
        recorrerIn(m_raiz, std::forward<Fs>(fns)...); 
    }

    template <typename... Fs>
    void preorden(Fs&&... fns) { 
        lock_guard<mutex> lock(m_mtx); 
        recorrerPre(m_raiz, std::forward<Fs>(fns)...); 
    }

    template <typename... Fs>
    void postorden(Fs&&... fns) { 
        lock_guard<mutex> lock(m_mtx); 
        recorrerPost(m_raiz, std::forward<Fs>(fns)...); 
    }

    // Punto 9: FirstThat con variadic
    template <typename P, typename... Fs>
    bool FirstThat(P&& predicado, Fs&&... fns) {
        lock_guard<mutex> lock(m_mtx);
        return buscarPrimero(m_raiz, predicado, std::forward<Fs>(fns)...);
    }

    // Puntos 10 y 11: Operadores << y >>
    friend ostream& operator<<(ostream& os, CBinaryTree& t) {
        t.inorden([&os](const T_Dato& v) { os << v << " "; });
        return os;
    }

    friend istream& operator>>(istream& is, CBinaryTree& t) {
        T_Dato v;
        if (is >> v) t.Insert(v);
        return is;
    }

    virtual void Insert(const T_Dato& v) {
        lock_guard<mutex> lock(m_mtx);
        insertar(m_raiz, v);
    }

private:
    void limpiar(Nodo* n) {
        if (!n) return;
        limpiar(n->hijos[0]);
        limpiar(n->hijos[1]);
        delete n;
    }

    void insertar(Nodo*& n, const T_Dato& v) {
        if (!n) { n = new Nodo(v); return; }
        insertar(n->hijos[v > n->valor], v);
    }

    // Lógica interna para Variadics (C++17 Fold Expressions)
    template <typename... Fs>
    void recorrerIn(Nodo* n, Fs&&... fns) {
        if (!n) return;
        recorrerIn(n->hijos[0], std::forward<Fs>(fns)...);
        (std::forward<Fs>(fns)(n->valor), ...);
        recorrerIn(n->hijos[1], std::forward<Fs>(fns)...);
    }

    template <typename... Fs>
    void recorrerPre(Nodo* n, Fs&&... fns) {
        if (!n) return;
        (std::forward<Fs>(fns)(n->valor), ...);
        recorrerPre(n->hijos[0], std::forward<Fs>(fns)...);
        recorrerPre(n->hijos[1], std::forward<Fs>(fns)...);
    }

    template <typename... Fs>
    void recorrerPost(Nodo* n, Fs&&... fns) {
        if (!n) return;
        recorrerPost(n->hijos[0], std::forward<Fs>(fns)...);
        recorrerPost(n->hijos[1], std::forward<Fs>(fns)...);
        (std::forward<Fs>(fns)(n->valor), ...);
    }
};

#endif