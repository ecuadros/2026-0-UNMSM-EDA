#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>
#include <mutex>
#include "../general/types.h"
#include "../util.h"

class CQueue {
private:
    struct Node {
        ContainerElemType data;
        Node* next;
        Node(const ContainerElemType& val) : data(val), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    Size size;
    mutable std::mutex mtx;

    void clear() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        size = 0;
    }

public:

    CQueue() : head(nullptr), tail(nullptr), size(0) {}

    CQueue(const CQueue& other) : head(nullptr), tail(nullptr), size(0) {
        std::lock_guard<std::mutex> lock(other.mtx);
        Node* current = other.head;
        while (current) {
            push(current->data);
            current = current->next;
        }
    }

    CQueue(CQueue&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.mtx);
        head = other.head;
        tail = other.tail;
        size = other.size;

        other.head = nullptr;
        other.tail = nullptr;
        other.size = 0;
    }

    ~CQueue() {
        std::lock_guard<std::mutex> lock(mtx);
        clear();
    }

    void push(const ContainerElemType& value) {
        std::lock_guard<std::mutex> lock(mtx);

        Node* newNode = new Node(value);

        if (!tail) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }

        ++size;
    }

    bool pop(ContainerElemType& outValue) {
        std::lock_guard<std::mutex> lock(mtx);

        if (!head)
            return false;

        Node* temp = head;
        outValue = head->data;
        head = head->next;

        if (!head)
            tail = nullptr;

        delete temp;
        --size;

        return true;
    }

    bool operator<(const CQueue& other) const {
        std::lock_guard<std::mutex> lock1(mtx);
        std::lock_guard<std::mutex> lock2(other.mtx);
        return size < other.size;
    }

    bool operator>(const CQueue& other) const {
        std::lock_guard<std::mutex> lock1(mtx);
        std::lock_guard<std::mutex> lock2(other.mtx);
        return size > other.size;
    }

    Size getSize() const {
        std::lock_guard<std::mutex> lock(mtx);
        return size;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx);
        return size == 0;
    }
};

#endif // __QUEUE_H__