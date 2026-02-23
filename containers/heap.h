#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <vector>
#include <mutex>
#include <algorithm>
#include <stdexcept>
#include "../general/types.h"
#include "../util.h"

template <typename T, typename Compare = std::less<T>>
class CHeap {
private:
    std::vector<T> data;
    Compare comp;
    mutable std::mutex mtx;
    void siftUp(int i) {
        while (i > 0 && comp(data[(i - 1) / 2], data[i])) {
            std::swap(data[i], data[(i - 1) / 2]);
            i = (i - 1) / 2;
        }
    }
    void siftDown(int i) {
        int targetIdx = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int n = data.size();
        if (left < n && comp(data[targetIdx], data[left])) 
            targetIdx = left;
        
        if (right < n && comp(data[targetIdx], data[right])) 
            targetIdx = right;

        if (i != targetIdx) {
            std::swap(data[i], data[targetIdx]);
            siftDown(targetIdx);
        }
    }
public:
    CHeap() = default;
    CHeap(const CHeap& other) {
        std::lock_guard<std::mutex> lock(other.mtx); 
        this->data = other.data;
        this->comp = other.comp;
    }

    CHeap(CHeap&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.mtx);
        this->data = std::move(other.data);
        this->comp = std::move(other.comp);
    }
    ~CHeap() {
        std::lock_guard<std::mutex> lock(mtx);
        data.clear(); 
    }
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(mtx);
        data.push_back(value);
        siftUp(data.size() - 1);
    }
    T pop() {
        std::lock_guard<std::mutex> lock(mtx);
        if (data.empty()) throw std::runtime_error("Heap is empty");

        T rootValue = data[0];
        data[0] = data.back();
        data.pop_back();

        if (!data.empty()) {
            siftDown(0);
        }

        return rootValue;
    }
    friend std::ostream& operator<<(std::ostream& os, const CHeap& h) {
        std::lock_guard<std::mutex> lock(h.mtx);
        os << "[";
        for (size_t i = 0; i < h.data.size(); ++i) {
            os << h.data[i] << (i == h.data.size() - 1 ? "" : ", ");
        }
        os << "]";
        return os;
    }

    friend std::istream& operator>>(std::istream& is, CHeap& h) {
        T temp;
        if (is >> temp) {
            h.push(temp);
        }
        return is;
    }
};
#endif