#ifndef __STACK_H__
#define __STACK_H__

#include <iostream>
#include <mutex>
#include "../general/types.h"
#include "../util.h"

using namespace std;

class CStack {

    struct Node {
        ContainerElemType data;
        ref_type ref;
        Node* next;

        Node(const ContainerElemType& d, ref_type r, Node* n = nullptr)
            : data(d), ref(r), next(n) {}
    };

private:
    Node* m_top = nullptr;
    size_t m_size = 0;
    std::mutex mtx;

public:

    CStack() = default;

    CStack(const CStack& other) {
        Node* temp = nullptr;
        Node* cur = other.m_top;

        while (cur) {
            temp = new Node(cur->data, cur->ref, temp);
            cur = cur->next;
        }

        while (temp) {
            push(temp->data, temp->ref);
            Node* t = temp;
            temp = temp->next;
            delete t;
        }
    }

    CStack(CStack&& other) noexcept {
        m_top = other.m_top;
        m_size = other.m_size;

        other.m_top = nullptr;
        other.m_size = 0;
    }

    virtual ~CStack() {
        while (m_top) {
            Node* temp = m_top;
            m_top = m_top->next;
            delete temp;
        }
    }

    void push(const ContainerElemType& val, ref_type ref) {
        std::lock_guard<std::mutex> lock(mtx);
        m_top = new Node(val, ref, m_top);
        ++m_size;
    }

    bool pop() {
        std::lock_guard<std::mutex> lock(mtx);

        if (!m_top) return false;

        Node* temp = m_top;
        m_top = m_top->next;
        delete temp;
        --m_size;
        return true;
    }

    ContainerElemType& top() {
        return m_top->data;
    }

    size_t size() const {
        return m_size;
    }

    bool empty() const {
        return m_size == 0;
    }

    friend ostream& operator<<(ostream& os, const CStack& s) {
        Node* cur = s.m_top;
        os << "[";
        while (cur) {
            os << "(" << cur->data << ":" << cur->ref << ")";
            if (cur->next) os << ",";
            cur = cur->next;
        }
        os << "]";
        return os;
    }

    friend istream& operator>>(istream& is, CStack& s) {
        ContainerElemType val;
        ref_type ref;
        is >> val >> ref;
        s.push(val, ref);
        return is;
    }
};

#endif // __STACK_H__