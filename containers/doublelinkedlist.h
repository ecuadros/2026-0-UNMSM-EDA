#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__

#include <iostream>
#include "../general/types.h"
#include "../util.h"
#include <mutex>
#include <utility>
using namespace std;

template <typename Traits>
struct NodeDoubleLinkedList{
    using value_type = typename Traits::T;
    using  Node = NodeDoubleLinkedList<Traits>;

    value_type value;
    Node* next = nullptr;
    Node* prev = nullptr;

    NodeDoubleLinkedList(const value_type& v) : value(v) {}
    NodeDoubleLinkedList(value_type&& v) : value(std::move(v)) {}
};

template <typename Traits>
class DoubleLinkedList{
    public:
        using value_type = typename Traits::T;
        using  Node = NodeDoubleLinkedList<Traits>;

        DoubleLinkedList() = default;

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
        void clear();
        ~DoubleLinkedList() { clear(); }
        
        DoubleLinkedList(const DoubleLinkedList& other);
        DoubleLinkedList(DoubleLinkedList&& other) noexcept;

        value_type& operator[](Size i);
        const value_type& operator[](Size i) const;

        friend std::ostream& operator<<(std::ostream& os, const DoubleLinkedList& list) {
            std::lock_guard<std::mutex> lg(list.mtx);

            os << "DoubleLinkedList: size = " << list.sz << "\n";
            os << "[";

            bool first = true;
            for (Node* cur = list.root; cur != nullptr; cur = cur->next) {
                if (!first) os << ",";
                os << cur->value;
                first = false;
            }

            os << "]";
            return os;
        }
        friend std::istream& operator>>(std::istream& is, DoubleLinkedList& list) {
            Size n;
            if (!(is >> n)) return is;

            
            list.clear();

            for (Size i = 0; i < n; ++i) {
                value_type x;
                if (!(is >> x)) break;         
                list.push_back(x);              
            }
            return is;
        }    
        

        class ForwardIterator {
            Node* p = nullptr;

        public:
            ForwardIterator(Node* x = nullptr) : p(x) {}

            bool operator!=(const ForwardIterator& other) const {
                return p != other.p;
            }

            value_type& operator*() const {
                return p->value;
            }

            ForwardIterator& operator++() {
                if (p) p = p->next;
                return *this;
            }
        };

        ForwardIterator begin() {
            std::lock_guard<std::mutex> lg(mtx);
            return ForwardIterator(root);
        }

        ForwardIterator end() {
            return ForwardIterator(nullptr);
        }

    private: 
        Node* root = nullptr;
        Node* tail = nullptr;
        Size  sz   = 0;

        mutable std::mutex mtx;

        void push_back_unsafe(const value_type& v);
    };

template <typename Traits>
void DoubleLinkedList<Traits>::clear() {
    std::lock_guard<std::mutex> lg(mtx);

    Node* curr = root;
    while (curr) {
        Node* nxt = curr->next;
        delete curr;
        curr = nxt;
    }
    root = tail = nullptr;
    sz = 0;
}

template <typename Traits>
void DoubleLinkedList<Traits>::push_back_unsafe(const value_type& v) {
    Node* n = new Node(v);

    if (!root) {                 
        root = tail = n;
        sz = 1;
        return;
    }

    n->prev = tail;
    tail->next = n;
    tail = n;
    ++sz;
}

template <typename Traits>
DoubleLinkedList<Traits>::DoubleLinkedList(const DoubleLinkedList& other) {
    std::lock_guard<std::mutex> lock_other(other.mtx);

    Node* curr = other.root;
    while (curr) {
        push_back_unsafe(curr->value);
        curr = curr->next;
    }
}

template <typename Traits>
DoubleLinkedList<Traits>::DoubleLinkedList(DoubleLinkedList&& other) noexcept {
    std::lock_guard<std::mutex> lock_other(other.mtx);

    root = other.root;
    tail = other.tail;
    sz   = other.sz;

    other.root = other.tail = nullptr;
    other.sz = 0;
}

template <typename Traits>
const typename DoubleLinkedList<Traits>::value_type&
DoubleLinkedList<Traits>::operator[](Size i) const {
    std::lock_guard<std::mutex> lg(mtx);

    if (i < 0 || i >= sz) {
        std::cerr << "Index out of range: " << i << " (size=" << sz << ")"<<endl;
        static value_type dummy{};
        return dummy;
    }

    if (i <= sz / 2) {
        Node* curr = root;
        for (Size k = 0; k < i; ++k) curr = curr->next;
        return curr->value;
    } else {
        Node* curr = tail;
        for (Size k = sz - 1; k > i; --k) curr = curr->prev;
        return curr->value;
    }
}

template <typename Traits>
typename DoubleLinkedList<Traits>::value_type&
DoubleLinkedList<Traits>::operator[](Size i) {
    const auto& cref = static_cast<const DoubleLinkedList&>(*this)[i];
    return const_cast<value_type&>(cref);
}
#endif // __DOUBLE_LINKED_LIST_H__