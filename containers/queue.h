#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <vector>
#include <iomanip>
#include "../general/types.h"
#include "../util.h"

using namespace std;

template <typename _T>
struct QueueTraits {
    using T = _T;
};

template <typename Traits>
class CQueueNode {
    using value_type = typename Traits::T;
    using Node = CQueueNode;

    value_type  m_data;
    ref_type    m_ref;
    CQueueNode *m_pNext;

public:
    CQueueNode() : m_ref(-1), m_pNext(nullptr) {}
    CQueueNode(const value_type &val, ref_type ref, Node *next = nullptr)
        : m_data(val), m_ref(ref), m_pNext(next) {}

    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }
    ref_type    GetRef     () const { return m_ref;   }
    ref_type   &GetRefRef  () { return m_ref;   }
    Node      * GetNext    () const { return m_pNext;   }
    Node      *&GetNextRef () { return m_pNext;   }

    Node &operator=(const Node &another){
        if (this == &another) return *this;

        m_data = another.GetValue();
        m_ref  = another.GetRef();
        return *this;
    }
    bool operator==(const Node &another) const
    { return m_data == another.GetValue();   }
    bool operator<(const Node &another) const
    { return m_data < another.GetValue();   }

};

template <typename Traits>
class CQueue;

template <typename Traits>
ostream &operator<<(ostream &os, CQueue<Traits> &container);

template <typename Traits>
istream &operator>>(istream &is, CQueue<Traits> &container);

template <typename Traits>
class CQueue {
    mutable mutex mtx;
    using value_type = typename Traits::T;
    using Node = CQueueNode<Traits>;

    Node *m_pFirst     = nullptr;
    Node *m_pLast      = nullptr;
    size_t m_nElements = 0;
public:
    CQueue() {}
    CQueue(const value_type &val, ref_type ref) {
        push(val, ref);
    }
    virtual ~CQueue() {
        clear_unlocked();
    }

    void push(const value_type &val, ref_type ref) {
        lock_guard lock(mtx);
        push_unlocked(val, ref);
    }
    value_type pop() {
        lock_guard lock(mtx);

        if (!m_pFirst) throw runtime_error("CQueue::pop on empty queue");

        value_type val = m_pFirst->GetValue();
        Node *pFirst = m_pFirst;
        m_pFirst = m_pFirst->GetNext();
        // si se vacia la queue hacer que pLast sea nullptr
        if (!m_pFirst) m_pLast = nullptr;
        delete pFirst;
        --m_nElements;
        return val;
    }

private:
    // helper para ayudar a la lectura (>>) y mantener el push
    void push_unlocked(const value_type &val, ref_type ref) {
        // gracias a ambos punteros se ahorra una iteracion O(n)
        Node *pNew = new Node(val, ref);
        if (m_pLast) m_pLast->GetNextRef() = pNew;
        else m_pFirst = pNew;
        m_pLast = pNew;

        ++m_nElements;
    }
    void clear_unlocked() {
        if (!m_pFirst) return;

        Node *pTrav = m_pFirst;
        Node *pTemp = nullptr;
        while (pTrav) {
            pTemp = pTrav;
            pTrav = pTrav->GetNextRef();
            delete pTemp;
        }
        m_pFirst = nullptr;
        m_pLast = nullptr;
        m_nElements = 0;
    }

    friend ostream &operator<< <Traits>(ostream &os, CQueue<Traits> &container);
    friend istream &operator>> <Traits>(istream &is, CQueue<Traits> &container);
};

// operador right shift
template <typename Traits>
ostream &operator<<(ostream &os, CQueue<Traits> &container) {
    using value_type = typename CQueue<Traits>::value_type;
    lock_guard<mutex> lock(container.mtx);
    os << "CQueue: size = " << container.m_nElements << " [";
    // iterar los nodos con un traveler
    for (auto trav = container.m_pFirst; trav; trav = trav->GetNext()) {
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
istream &operator>>(istream &is, CQueue<Traits> &container) {
    using value_type = typename CQueue<Traits>::value_type;
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
        for (const auto &item : items) {
            container.push_unlocked(item.first, item.second);
        }
    }
    return is;
}

#endif // __QUEUE_H__
