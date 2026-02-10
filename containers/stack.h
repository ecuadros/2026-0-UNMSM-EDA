#ifndef __STACK_H__
#define __STACK_H__

#include <iostream>
#include <iomanip>
#include <mutex>
#include <vector>
#include <stdexcept>
#include "../general/types.h"
#include "../util.h"

using namespace std;

template <typename _T>
struct StackTraits {
    using T = _T;
};

template <typename Traits>
class CStackNode {
    using value_type = typename Traits::T;
    using Node = CStackNode;

    value_type  m_data;
    ref_type    m_ref;
    CStackNode *m_pNext;

public:
    CStackNode() : m_ref(-1), m_pNext(nullptr) {}
    CStackNode(const value_type &val, ref_type ref, Node *next = nullptr)
        : m_data(val), m_ref(ref), m_pNext(next) {}

    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }
    ref_type    GetRef     () const { return m_ref;   }
    ref_type   &GetRefRef  () { return m_ref;   }
    Node      * GetNext     () const { return m_pNext;   }
    Node      *&GetNextRef  () { return m_pNext;   }

    Node &operator=(const Node &another){
        if (this == &another) return *this;

        m_data = another.GetValue();
        m_ref   = another.GetRef();
        return *this;
    }
    bool operator==(const Node &another) const
    { return m_data == another.GetValue();   }
    bool operator<(const Node &another) const
    { return m_data < another.GetValue();   }

};

template <typename Traits>
class CStack;

template <typename Traits>
ostream &operator<<(ostream &os, CStack<Traits> &container);

template <typename Traits>
istream &operator>>(istream &is, CStack<Traits> &container);

template <typename Traits>
class CStack{
    mutable mutex mtx;
    using value_type = typename Traits::T;
    using Node = CStackNode<Traits>;

    // para entenderlo mejor, el puntero apuntara a la cima de la pila
    // hara como una linked list al reves
    Node *m_pTop       = nullptr;
    Node *m_pBottom    = nullptr;
    size_t m_nElements = 0;

public:
    CStack(){}
    CStack(const value_type &val, ref_type ref) {
        push(val, ref);
    }
    virtual ~CStack() {
        // just delete everything
        clear_unlocked();
    }

    void push(const value_type &val, ref_type ref) {
        lock_guard lock(mtx);
        push_unlocked(val, ref);
    }
    value_type pop() {
        lock_guard lock(mtx);

        if (!m_pTop) throw runtime_error("CStack::pop on empty stack");
        value_type val = m_pTop->GetValue();
        Node *pTop = m_pTop;
        m_pTop = m_pTop->GetNext();
        // si se vacia el stack hacer que pBottom des nullptr
        if (!m_pTop) m_pBottom = nullptr;
        delete pTop;
        --m_nElements;
        return val;
    }

private:
    // helper para ayudar a la lectura (>>) y mantener el push
    void push_unlocked(const value_type &val, ref_type ref) {
        // gracias a ambos punteros se ahorra una iteracion O(n)
        Node *pNew = new Node(val, ref, m_pTop);
        if (!m_pTop) m_pBottom = pNew;
        m_pTop = pNew;
        ++m_nElements;
    }
    void clear_unlocked() {
        if (!m_pTop) return;

        Node *pTrav = m_pTop;
        Node *pTemp = nullptr;
        while (pTrav) {
            pTemp = pTrav;
            pTrav = pTrav->GetNextRef();
            delete pTemp;
        }
        m_pTop = nullptr;
        m_pBottom = nullptr;
        m_nElements = 0;
    }

    friend ostream &operator<< <Traits>(ostream &os, CStack<Traits> &container);
    friend istream &operator>> <Traits>(istream &is, CStack<Traits> &container);
};

// operador right shift
template <typename Traits>
ostream &operator<<(ostream &os, CStack<Traits> &container) {
    using value_type = typename CStack<Traits>::value_type;
    lock_guard<mutex> lock(container.mtx);
    os << "CStack: size = " << container.m_nElements << " [";
    // iterar los nodos con un traveler
    for (auto trav = container.m_pTop; trav; trav = trav->GetNext()) {
        if constexpr (std::is_same_v<value_type, std::string>) {
            os << "(" << std::quoted(trav->GetValue()) << ":" << trav->GetRef() << "),";
        } else {
            os << "(" << trav->GetValue() << ":" << trav->GetRef() << "),";
        }
    }
    os << "]" << endl;
    return os;}

// operador left shift
// lee del input stream esperando el formato en el que se escribe
template <typename Traits>
istream &operator>>(istream &is, CStack<Traits> &container) {
    using value_type = typename CStack<Traits>::value_type;
    // verificar el buen estado del stream
    if (!is) return is;

    vector<pair<value_type, ref_type>> items;
    try {
        // ignorar texto hasta el primer '['
        string bar;
        getline(is, bar, '[');

        char ch;
        while (is.get(ch) && ch != ']') {
            if (ch != '(') continue;
            value_type val;
            ref_type ref;

            if constexpr (std::is_same_v<value_type, std::string>) {
                // leer string con comillas y escapes
                is >> std::quoted(val);
                getline(is, bar, ':');
            } else {
                is >> val;  // leer el valor
                getline(is, bar, ':');
            }
            is >> ref;
            // leer la ref
            getline(is, bar, ')');
            items.emplace_back(val, ref);
        }
    } catch (const exception& e) {
        // en caso de error activar la flav del input stream
        is.setstate(ios::failbit);
    }
    // si la lectura salio bien, reemplaza el contenido del contenedor
    if (is) {
        lock_guard<mutex> lock(container.mtx);
        container.clear_unlocked();
        for (auto it = items.rbegin(); it != items.rend(); ++it) {
            container.push_unlocked(it->first, it->second);
        }
    }
    return is;
}

#endif // __STACK_H__
