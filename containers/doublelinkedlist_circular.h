#ifndef __DOUBLE_LINKED_LIST_CIRCULAR_H__
#define __DOUBLE_LINKED_LIST_CIRCULAR_H__

#include <iostream>
#include <mutex>
#include <utility>
#include "../general/types.h"

template <typename Traits>
struct NodeDoubleLinkedListCircular {
    using value_type = typename Traits::T;
    using Node = NodeDoubleLinkedListCircular<Traits>;

    value_type value;
    Node* next = nullptr;
    Node* prev = nullptr;

    NodeDoubleLinkedListCircular(const value_type& v) : value(v) {}
};

template <typename Traits>
class DoubleLinkedListCircular {
public:
    using value_type = typename Traits::T;
    using Node = NodeDoubleLinkedListCircular<Traits>;

    DoubleLinkedListCircular() = default;
    ~DoubleLinkedListCircular() { clear(); }

    Size getSize() const {
        std::lock_guard<std::mutex> lg(mtx);
        return sz;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lg(mtx);
        return sz == 0;
    }

    void push_back(const value_type& v) {
        std::lock_guard<std::mutex> lg(mtx);
        push_back_unsafe(v);
    }

    void clear() {
        std::lock_guard<std::mutex> lg(mtx);
        if (!root) return;

        Node* curr = root;
        for (Size i = 0; i < sz; ++i) {
            Node* nxt = curr->next;
            delete curr;
            curr = nxt;
        }
        root = tail = nullptr;
        sz = 0;
    }

    class ForwardIterator {
        Node* p;
        Size remaining;

    public:
        ForwardIterator(Node* n = nullptr, Size r = 0)
            : p(n), remaining(r) {}

        bool operator!=(const ForwardIterator& other) const {
            return remaining != other.remaining;
        }

        value_type& operator*() const {
            return p->value;
        }

        ForwardIterator& operator++() {
            p = p->next;
            --remaining;
            return *this;
        }
    };

    ForwardIterator begin() {
        std::lock_guard<std::mutex> lg(mtx);
        return ForwardIterator(root, sz);
    }

    ForwardIterator end() {
        return ForwardIterator(nullptr, 0);
    }

    
    const value_type& operator[](Size i) const {
        std::lock_guard<std::mutex> lg(mtx);

        if (i >= sz) {
            static value_type dummy{};
            return dummy;
        }

        Node* curr = root;
        for (Size k = 0; k < i; ++k)
            curr = curr->next;

        return curr->value;
    }

    value_type& operator[](Size i) {
        return const_cast<value_type&>(
            static_cast<const DoubleLinkedListCircular&>(*this)[i]
        );
    }

    
    friend std::ostream& operator<<(std::ostream& os,
                                    const DoubleLinkedListCircular& l) {
        std::lock_guard<std::mutex> lg(l.mtx);

        os << "DoubleLinkedListCircular: size = " << l.sz << "\n[";
        Node* curr = l.root;
        for (Size i = 0; i < l.sz; ++i) {
            if (i) os << ",";
            os << curr->value;
            curr = curr->next;
        }
        os << "]";
        return os;
    }

    friend std::istream& operator>>(std::istream& is,
                                    DoubleLinkedListCircular& l) {
        Size n;
        if (!(is >> n)) return is;

        l.clear();
        for (Size i = 0; i < n; ++i) {
            value_type x;
            is >> x;
            l.push_back(x);
        }
        return is;
    }

private:
    Node* root = nullptr;
    Node* tail = nullptr;
    Size sz = 0;

    mutable std::mutex mtx;

    void push_back_unsafe(const value_type& v) {
        Node* n = new Node(v);

        if (!root) {
            root = tail = n;
            n->next = n;
            n->prev = n;
            sz = 1;
            return;
        }

        n->prev = tail;
        n->next = root;
        tail->next = n;
        root->prev = n;
        tail = n;
        ++sz;
    }
};

#endif