#ifndef __STACK_H__
#define __STACK_H__

#include <iostream>
#include <mutex>
#include <stdexcept>
#include "../general/types.h"
#include "../util.h"
using namespace std;

template <typename T>
struct StackTrait {
    using value_type = T;
};

template <typename Traits>
class NodeStack {

    using value_type = typename Traits::value_type;
    using Node       = NodeStack<Traits>;

private:
    value_type m_data;
    ref_type   m_ref;
    Node      *m_pNext = nullptr;

public:
    NodeStack() {}
    NodeStack(value_type _value, ref_type _ref = -1, Node *_pNext = nullptr)
        : m_data(_value), m_ref(_ref), m_pNext(_pNext) {}

    value_type  GetValue   () const { return m_data;  }
    value_type &GetValueRef()       { return m_data;  }

    ref_type    GetRef     () const { return m_ref;   }
    ref_type   &GetRefRef  ()       { return m_ref;   }

    Node      * GetNext    () const { return m_pNext; }
    Node      *&GetNextRef ()       { return m_pNext; }

    Node &operator=(const Node &another) {
        m_data = another.GetValue();
        m_ref  = another.GetRef();
        return *this;
    }
    bool operator==(const Node &another) const
    { return m_data == another.GetValue(); }

    bool operator<(const Node &another) const
    { return m_data < another.GetValue(); }
};

template <typename Traits>
class CStack {

    using value_type = typename Traits::value_type;
    using Node       = NodeStack<Traits>;

    Node  *m_pTop      = nullptr;
    size_t m_nElements = 0;
    mutable mutex m_mutex;

public:
    CStack() {}

    //Constructor copia
    CStack(const CStack<Traits> &another);

    //Move Constructor
    CStack(CStack<Traits> &&another) noexcept;

    //Destructor seguro
    ~CStack();

    void       push(const value_type &val, ref_type ref = -1);
    value_type pop();

    size_t getSize() const { return m_nElements; }

    friend ostream &operator<<(ostream &os, CStack<Traits> &container) {
        lock_guard<mutex> lock(container.m_mutex);
        os << "CStack (LIFO): size = " << container.m_nElements << endl;
        os << "[";
        for (Node *p = container.m_pTop; p != nullptr; p = p->GetNext())
            os << "(" << p->GetValue() << ":" << p->GetRef() << "),";
        os << "]" << endl;
        return os;
    }

    friend istream &operator>>(istream &is, CStack<Traits> &container) {
        value_type val;
        ref_type   ref;
        is >> val >> ref;
        container.push(val, ref);
        return is;
    }
};

//Constructor copia
template <typename Traits>
CStack<Traits>::CStack(const CStack<Traits> &another) {
    lock_guard<mutex> lock(another.m_mutex);

    if (!another.m_pTop)
        return;

    using Node = NodeStack<Traits>;
    size_t n = another.m_nElements;
    Node **tmp = new Node*[n];

    Node *src = another.m_pTop;
    for (size_t i = 0; i < n; ++i, src = src->GetNext())
        tmp[i] = src;

    //Insercion desde el fondo hacia el tope para respetar el orden LIFO
    for (int i = (int)n - 1; i >= 0; --i) {
        m_pTop = new Node(tmp[i]->GetValue(), tmp[i]->GetRef(), m_pTop);
        ++m_nElements;
    }

    delete[] tmp;
}

//Move Constructor
template <typename Traits>
CStack<Traits>::CStack(CStack<Traits> &&another) noexcept
    : m_pTop(another.m_pTop), m_nElements(another.m_nElements) {
    another.m_pTop      = nullptr;
    another.m_nElements = 0;
}

//Destructor seguro
template <typename Traits>
CStack<Traits>::~CStack() {
    while (m_pTop) {
        Node *tmp = m_pTop;
        m_pTop    = m_pTop->GetNext();
        delete tmp;
    }
}

template <typename Traits>
void CStack<Traits>::push(const value_type &val, ref_type ref) {
    lock_guard<mutex> lock(m_mutex);
    m_pTop = new Node(val, ref, m_pTop);
    ++m_nElements;
}

template <typename Traits>
typename CStack<Traits>::value_type CStack<Traits>::pop() {
    lock_guard<mutex> lock(m_mutex);
    if (!m_pTop)
        throw runtime_error("CStack: pop() en stack vacío");
    Node      *tmp = m_pTop;
    value_type val = tmp->GetValue();
    m_pTop         = m_pTop->GetNext();
    delete tmp;
    --m_nElements;
    return val;
}

void DemoStack();

#endif // __STACK_H__