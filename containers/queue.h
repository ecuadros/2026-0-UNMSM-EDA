#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <vector>

template <typename T>
class CQueue {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& v) : data(v), next(nullptr) {}
    };

    Node* front_ = nullptr;
    Node* back_  = nullptr;
    size_t size_ = 0;
    mutable std::mutex mtx_;

    void clear_unsafe() {
        while (front_) {
            Node* temp = front_;
            front_ = front_->next;
            delete temp;
        }
        back_ = nullptr;
        size_ = 0;
    }

public:
    CQueue() = default;

    CQueue(const CQueue& other) : front_(nullptr), back_(nullptr), size_(0) {
        std::lock_guard<std::mutex> lock(other.mtx_);

        std::vector<T> values;
        values.reserve(other.size_);
        Node* p = other.front_;
        while (p) {
            values.push_back(p->data);
            p = p->next;
        }

        for (const auto& v : values) {
            push(v);
        }
    }
    
    CQueue(CQueue&& other) noexcept : front_(nullptr), back_(nullptr), size_(0) {
        std::lock_guard<std::mutex> lock(other.mtx_);

        front_ = other.front_;
        back_  = other.back_;
        size_  = other.size_;

        other.front_ = nullptr;
        other.back_  = nullptr;
        other.size_  = 0;
    }

    ~CQueue() {
        std::lock_guard<std::mutex> lock(mtx_);
        clear_unsafe();
    }

    void push(const T& value) {
        std::lock_guard<std::mutex> lock(mtx_);
        Node* n = new Node(value);

        if (!back_) {
            front_ = back_ = n;
        } else {
            back_->next = n;
            back_ = n;
        }
        ++size_;
    }

    T pop() {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!front_) throw std::underflow_error("Queue vacia");

        Node* temp = front_;
        T value = temp->data;
        front_ = front_->next;

        if (!front_) back_ = nullptr; 
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

    friend std::ostream& operator<<(std::ostream& os, const CQueue& q) {
        std::lock_guard<std::mutex> lock(q.mtx_);
        os << "[";
        Node* p = q.front_;
        while (p) {
            os << p->data;
            if (p->next) os << ", ";
            p = p->next;
        }
        os << "]";
        return os;
    }

    friend CQueue& operator>>(CQueue& q, T& value) {
        std::lock_guard<std::mutex> lock(q.mtx_);
        if (!q.front_) throw std::underflow_error("Queue vacia");

        Node* temp = q.front_;
        value = temp->data;
        q.front_ = q.front_->next;

        if (!q.front_) q.back_ = nullptr;
        delete temp;
        --q.size_;

        return q;
    }
};

#endif // __QUEUE_H__