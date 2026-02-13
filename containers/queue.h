#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>
#include <mutex>
#include <stdexcept>
#include "../general/types.h"
#include "../util.h"
using namespace std;

template <typename T>
struct QueueTrait {
    using value_type = T;
};

template <typename Traits>
class NodeQueue {

    using value_type = typename Traits::value_type;
    using Node       = NodeQueue<Traits>;

private:
    value_type m_data;
    ref_type   m_ref;
    Node      *m_pNext = nullptr;

public:
    NodeQueue() {}
    NodeQueue(value_type _value, ref_type _ref = -1, Node *_pNext = nullptr)
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
class CQueue {

    using value_type = typename Traits::value_type;
    using Node       = NodeQueue<Traits>;

    Node  *m_pFront    = nullptr;
    Node  *m_pBack     = nullptr;
    size_t m_nElements = 0;
    mutable mutex m_mutex;

public:
    CQueue() {}

    //Constructor copia
    CQueue(const CQueue<Traits> &another);

    //Move Constructor
    CQueue(CQueue<Traits> &&another) noexcept;

    //Destructor seguro
    virtual ~CQueue();

    void       push(const value_type &val, ref_type ref = -1);
    value_type pop();

    size_t getSize() const { return m_nElements; }

    friend ostream &operator<<(ostream &os, CQueue<Traits> &container) {
        lock_guard<mutex> lock(container.m_mutex);
        os << "CQueue (FIFO): size = " << container.m_nElements << endl;
        os << "[";
        for (Node *p = container.m_pFront; p != nullptr; p = p->GetNext())
            os << "(" << p->GetValue() << ":" << p->GetRef() << "),";
        os << "]" << endl;
        return os;
    }

    friend istream &operator>>(istream &is, CQueue<Traits> &container) {
        value_type val;
        ref_type   ref;
        is >> val >> ref;
        container.push(val, ref);
        return is;
    }
};

//Constructor copia
template <typename Traits>
CQueue<Traits>::CQueue(const CQueue<Traits> &another) {
    lock_guard<mutex> lock(another.m_mutex);

    for (Node *src = another.m_pFront; src != nullptr; src = src->GetNext()) {
        Node *pNewNode = new Node(src->GetValue(), src->GetRef());
        if (!m_pFront)
            m_pFront = pNewNode;
        else
            m_pBack->GetNextRef() = pNewNode;
        m_pBack = pNewNode;
        ++m_nElements;
    }
}

//Move Constructor
template <typename Traits>
CQueue<Traits>::CQueue(CQueue<Traits> &&another) noexcept
    : m_pFront(another.m_pFront), m_pBack(another.m_pBack),
      m_nElements(another.m_nElements) {
    another.m_pFront    = nullptr;
    another.m_pBack     = nullptr;
    another.m_nElements = 0;
}

//Destructor seguro
template <typename Traits>
CQueue<Traits>::~CQueue() {
    while (m_pFront) {
        Node *tmp  = m_pFront;
        m_pFront   = m_pFront->GetNext();
        delete tmp;
    }
}

template <typename Traits>
void CQueue<Traits>::push(const value_type &val, ref_type ref) {
    lock_guard<mutex> lock(m_mutex);
    Node *pNewNode = new Node(val, ref);
    if (!m_pFront)
        m_pFront = pNewNode;
    else
        m_pBack->GetNextRef() = pNewNode;
    m_pBack = pNewNode;
    ++m_nElements;
}

template <typename Traits>
typename CQueue<Traits>::value_type CQueue<Traits>::pop() {
    lock_guard<mutex> lock(m_mutex);
    if (!m_pFront)
        throw runtime_error("CQueue: pop() en queue vacío");
    Node      *tmp = m_pFront;
    value_type val = tmp->GetValue();
    m_pFront       = m_pFront->GetNext();
    if (!m_pFront)
        m_pBack = nullptr;
    delete tmp;
    --m_nElements;
    return val;
}

void DemoQueue();

#endif // __QUEUE_H__