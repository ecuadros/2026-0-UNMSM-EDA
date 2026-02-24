#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <vector>
#include <mutex>
#include <algorithm>
#include "../general/types.h"
#include "../util.h"

class CHeap {
private:
    std::vector<ContainerElemType> data;
    bool isMinHeap;
    mutable std::mutex mtx;

    bool compare(const ContainerElemType& a, const ContainerElemType& b) const {
        if (isMinHeap)
            return a < b;
        return a > b;
    }

    void heapifyUp(Size index) {
        while (index > 0) {
            Size parent = (index - 1) / 2;
            if (compare(data[index], data[parent])) {
                std::swap(data[index], data[parent]);
                index = parent;
            } else break;
        }
    }

    void heapifyDown(Size index) {
        Size left, right, target;
        Size n = data.size();

        while (true) {
            left = 2 * index + 1;
            right = 2 * index + 2;
            target = index;

            if (left < n && compare(data[left], data[target]))
                target = left;

            if (right < n && compare(data[right], data[target]))
                target = right;

            if (target != index) {
                std::swap(data[index], data[target]);
                index = target;
            } else break;
        }
    }

public:

    CHeap(bool minHeap = false) : isMinHeap(minHeap) {}

    CHeap(const CHeap& other) {
        std::lock_guard<std::mutex> lock(other.mtx);
        data = other.data;
        isMinHeap = other.isMinHeap;
    }

    CHeap(CHeap&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.mtx);
        data = std::move(other.data);
        isMinHeap = other.isMinHeap;
    }

    ~CHeap() {
        std::lock_guard<std::mutex> lock(mtx);
        data.clear();
    }

    void push(const ContainerElemType& value) {
        std::lock_guard<std::mutex> lock(mtx);
        data.push_back(value);
        heapifyUp(data.size() - 1);
    }

    bool pop(ContainerElemType& outValue) {
        std::lock_guard<std::mutex> lock(mtx);

        if (data.empty())
            return false;

        outValue = data[0];
        data[0] = data.back();
        data.pop_back();

        if (!data.empty())
            heapifyDown(0);

        return true;
    }

    bool operator<(const CHeap& other) const {
        std::scoped_lock lock(mtx, other.mtx);
        return data.size() < other.data.size();
    }

    bool operator>(const CHeap& other) const {
        std::scoped_lock lock(mtx, other.mtx);
        return data.size() > other.data.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx);
        return data.empty();
    }

    Size size() const {
        std::lock_guard<std::mutex> lock(mtx);
        return data.size();
    }
};

#endif // __HEAP_H__