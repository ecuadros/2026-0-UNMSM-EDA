#ifndef __STACK_H__
#define __STACK_H__

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
class NodeStack {
public:
    using value_type = typename Traits::value_type;

private:
    value_type m_data;
    NodeStack *m_pNext = nullptr;

public:
    NodeStack() : m_pNext(nullptr) {}
    NodeStack(value_type _value, NodeStack* _pNext = nullptr)
        : m_data(_value), m_pNext(_pNext) {}

    value_type  GetValue() const { return m_data; }
    value_type& GetValueRef()    { return m_data; }

    NodeStack* GetNext() const  { return m_pNext; }
    NodeStack*& GetNextRef()     { return m_pNext; }
};

template <typename Traits>
class CStack{
public:
    using value_type = typename Traits::value_type;
    using Node       = NodeStack<Traits>;

protected:
    Node *m_pTop = nullptr; 
    size_t m_nElements = 0;

    mutable mutex m_mutex;

public:
    CStack() : m_pTop(nullptr), m_nElements(0) {}

    // Constructor Copia
    CStack(const CStack<Traits> &another) : m_pTop(nullptr), m_nElements(0) {
        lock_guard<mutex> lock(another.m_mutex);
        if (!another.m_pTop) return;

        m_pTop = new Node(another.m_pTop->GetValue());
        Node* pDest = m_pTop;
        Node* pSrc  = another.m_pTop->GetNext();

        while (pSrc) {
            pDest->GetNextRef() = new Node(pSrc->GetValue());
            pDest = pDest->GetNext();
            pSrc  = pSrc->GetNext();
        }
        m_nElements = another.m_nElements;
    }

    // Move Constructor
    CStack(CStack<Traits> &&another) noexcept 
        : m_pTop(nullptr), m_nElements(0) 
    {
        lock_guard<mutex> lock(another.m_mutex);
        m_pTop = exchange(another.m_pTop, nullptr);
        m_nElements = exchange(another.m_nElements, 0);
    }

    // Safe Destructor
    virtual ~CStack() {
        lock_guard<mutex> lock(m_mutex);
        while (m_pTop) {
            Node* pNext = m_pTop->GetNext();
            delete m_pTop;
            m_pTop = pNext;
        }
        m_nElements = 0;
    }

    // Push
    void push(const value_type &val) {
        lock_guard<mutex> lock(m_mutex);
        Node* pNew = new Node(val, m_pTop);
        m_pTop = pNew;
        m_nElements++;
    }

    // Pop
    void pop() {
        lock_guard<mutex> lock(m_mutex);
        if (m_pTop) {
            Node* pTemp = m_pTop;
            m_pTop = m_pTop->GetNext();
            delete pTemp;
            m_nElements--;
        }
    }

    value_type& top() {
        lock_guard<mutex> lock(m_mutex);
        assert(m_pTop != nullptr && "Stack is empty");
        return m_pTop->GetValueRef();
    }

    bool empty() const {
        lock_guard<mutex> lock(m_mutex);
        return m_pTop == nullptr;
    }

    size_t size() const {
        lock_guard<mutex> lock(m_mutex);
        return m_nElements;
    }

    // Operator <<
    friend ostream& operator<<(ostream &os, CStack<Traits> &stack) {
        lock_guard<mutex> lock(stack.m_mutex); 
        os << "Stack Size: " << stack.m_nElements << endl;
        os << "[TOP] -> ";
        Node* pCurrent = stack.m_pTop;
        while (pCurrent) {
            os << pCurrent->GetValue() << " -> ";
            pCurrent = pCurrent->GetNext();
        }
        os << "NULL" << endl;
        return os;
    }

     //Operator >>
    friend istream& operator>>(istream &is, CStack<Traits> &stack) {
        value_type val;
        
        while (is >> val) { stack.push(val); }
        is.clear(); 
        return is;
    }
};

void DemoStack();

#endif // __STACK_H__