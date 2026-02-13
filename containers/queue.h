#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>
#include <cassert>
#include <utility>
#include <mutex>
#include <fstream>
#include "../general/types.h"
#include "../util.h"
#include "../general/traits.h"

using namespace std;

template <typename Traits>
class NodeQueue {
public:
    using value_type = typename Traits::value_type;

private:
    value_type m_data;
    NodeQueue *m_pNext = nullptr;

public:
    NodeQueue() : m_pNext(nullptr) {}
    NodeQueue(value_type _value, NodeQueue* _pNext = nullptr)
        : m_data(_value), m_pNext(_pNext) {}

    value_type  GetValue() const { return m_data; }
    value_type& GetValueRef()    { return m_data; }

    NodeQueue* GetNext() const  { return m_pNext; }
    NodeQueue*& GetNextRef()    { return m_pNext; }
};

template <typename Traits>
class CQueue{
public:
    using value_type = typename Traits::value_type;
    using Node       = NodeQueue<Traits>;

protected:
    Node *m_pFront = nullptr;
    Node *m_pRear  = nullptr;
    size_t m_nElements = 0;

    mutable mutex m_mutex;

public:
    CQueue() : m_pFront(nullptr), m_pRear(nullptr), m_nElements(0) {}

    // Constructor Copia
    CQueue(const CQueue<Traits> &another) : m_pFront(nullptr), m_pRear(nullptr), m_nElements(0) {
        lock_guard<mutex> lock(another.m_mutex);
        if (!another.m_pFront) return;

        Node* pSrc = another.m_pFront;
        while (pSrc) {
            Node* pNew = new Node(pSrc->GetValue());
            
            if (!m_pFront) {
                m_pFront = pNew;
                m_pRear  = pNew;
            } else {
                m_pRear->GetNextRef() = pNew;
                m_pRear = pNew;
            }
            
            pSrc = pSrc->GetNext();
        }
    m_nElements = another.m_nElements;
    }

    // Move Constructor
    CQueue(CQueue<Traits> &&another) noexcept {
        lock_guard<mutex> lock(another.m_mutex);

        m_pFront    = exchange(another.m_pFront, nullptr);
        m_pRear     = exchange(another.m_pRear, nullptr);
        m_nElements = exchange(another.m_nElements, 0);
    }

    // Safe Destructor
    virtual ~CQueue() {
        lock_guard<mutex> lock(m_mutex);
        while (m_pFront) {
            Node* pNext = m_pFront->GetNext();
            delete m_pFront;
            m_pFront = pNext;
        }
        m_pRear = nullptr;
        m_nElements = 0;
    }

    // Push
    void push(const value_type &val) {
        lock_guard<mutex> lock(m_mutex);
        Node* pNew = new Node(val, nullptr);

        if (m_pFront == nullptr) {
            m_pFront = pNew;
            m_pRear  = pNew;
        } else {
            m_pRear->GetNextRef() = pNew;
            m_pRear = pNew;
        }
        m_nElements++;
    }

    // Pop
    value_type pop() {
        lock_guard<mutex> lock(m_mutex);

        // (!= nullptr) explicito por el assert
        assert(m_pFront != nullptr && "Queue is empty");

        value_type valor = m_pFront->GetValue();

        Node* pTemp = m_pFront;
        m_pFront = m_pFront->GetNext();

        if (m_pFront == nullptr)
            m_pRear = nullptr;
        delete pTemp;
        m_nElements--;

        return valor;
    }

    value_type& front() {
        lock_guard<mutex> lock(m_mutex);
        assert(m_pFront != nullptr && "Queue is empty");
        return m_pFront->GetValueRef();
    }

    value_type& back() {
        lock_guard<mutex> lock(m_mutex);
        assert(m_pRear != nullptr && "Queue is empty");
        return m_pRear->GetValueRef();
    }

    bool empty() const {
        lock_guard<mutex> lock(m_mutex);
        return m_pFront == nullptr;
    }

    size_t size() const {
        lock_guard<mutex> lock(m_mutex);
        return m_nElements;
    }

    // Operator <<
    friend ostream& operator<<(ostream &os, CQueue<Traits> &queue) {
        lock_guard<mutex> lock(queue.m_mutex); 
        os << "Queue Size: " << queue.m_nElements << endl;
        os << "[FRONT] -> ";
        Node* pCurrent = queue.m_pFront;
        while (pCurrent) {
            os << pCurrent->GetValue() << " -> ";
            pCurrent = pCurrent->GetNext();
        }
        os << "[REAR]" << endl;
        return os;
    }

    // Operator >>
        friend istream& operator>>(istream &is, CQueue<Traits> &queue) {
            value_type val;
            while (is >> val) { 
                queue.push(val); 
            }
            is.clear(); 
            return is;
        }

};

void DemoQueue();

#endif // __QUEUE_H__