#ifndef __STACK_H__
#define __STACK_H__

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <vector> 

template <typename T>
class CStack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& v, Node* n=nullptr) : data(v), next(n) {}
    };

    Node* top_ = nullptr;
    size_t size_ = 0;
    mutable std::mutex mtx_;

    void clear_unsafe() {
        while (top_) {
            Node* temp = top_;
            top_ = top_->next;
            delete temp;
        }
        size_ = 0;
    }

public:
    CStack() = default;

    CStack(const CStack& other) : top_(nullptr), size_(0) {
        std::lock_guard<std::mutex> lock(other.mtx_);

        if (!other.top_) return;

        std::vector<T> values;
        Node* current = other.top_;

        while (current) {
            values.push_back(current->data);
            current = current->next;
        }

        for (int i = values.size() - 1; i >= 0; --i) {
            push(values[i]);
        }
    }

    CStack(CStack&& other) noexcept : top_(nullptr), size_(0) {
        std::lock_guard<std::mutex> lock(other.mtx_);

        top_ = other.top_;
        size_ = other.size_;

        other.top_ = nullptr;
        other.size_ = 0;
    }

    ~CStack() {
        std::lock_guard<std::mutex> lock(mtx_);
        clear_unsafe();
    }
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(mtx_);
        top_ = new Node(value, top_);
        ++size_;
    }

    T pop() {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!top_) throw std::underflow_error("Stack vacio");

        Node* temp = top_;
        T value = temp->data;
        top_ = top_->next;
        delete temp;
        --size_;
        return value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return size_ == 0;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return size_;
    }

      friend std::ostream& operator<<(std::ostream& os, const CStack& s) {
        std::lock_guard<std::mutex> lock(s.mtx_);

        os << "[";
        Node* current = s.top_;

        while (current) {
            os << current->data;
            if (current->next)
                os << ", ";
            current = current->next;
        }

        os << "]";
        return os;
    }
    friend CStack& operator>>(CStack& s, T& value) {
        std::lock_guard<std::mutex> lock(s.mtx_);

        if (!s.top_)
            throw std::underflow_error("Stack vacio");

        Node* temp = s.top_;
        value = temp->data;
        s.top_ = temp->next;
        delete temp;
        --s.size_;

        return s;
    }
};

#endif
