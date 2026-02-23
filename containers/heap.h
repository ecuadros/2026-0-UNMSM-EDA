#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <vector>
#include <mutex>
#include <algorithm>
#include <iomanip>
#include <type_traits>
#include "../general/types.h"
#include "../util.h"

using namespace std;

template <typename T, typename Comparator>
struct HeapTrait {
    using value_type = T;
    using Func       = Comparator;
};

template <typename T>
struct AscendingHeapTrait : HeapTrait<T, std::greater<T>> {};

template <typename T>
struct DescendingHeapTrait : HeapTrait<T, std::less<T>> {};

template <typename Traits> class CHeap;
template <typename Traits> ostream& operator<<(ostream &os, CHeap<Traits> &h);
template <typename Traits> istream& operator>>(istream &is, CHeap<Traits> &h);

template <typename Traits>
class CHeap {
public:
    using value_type = typename Traits::value_type;
    using compareFunc = typename Traits::Func;

private:
    struct Node {
        value_type m_value;
        ref_type m_ref;

        Node() : m_value(), m_ref(-1) {}
        Node(value_type v, ref_type r = -1) : m_value(v), m_ref(r) {}
        
        value_type GetValue() const { return m_value; }
        ref_type GetRef() const { return m_ref; }
    };

    vector<Node> heap;
    mutable recursive_mutex m_mutex; //Concurrencia

    static size_t _getParent(size_t i) { return i == 0 ? 0 : (i - 1) / 2; }
    static size_t _getLeft(size_t i)   { return 2 * i + 1; }
    static size_t _getRight(size_t i)  { return 2 * i + 2; }

    bool invariant(const value_type &parent, const value_type &child) const {
        return compareFunc{}(parent, child);
    }

    void heapifyUp(size_t idx) {
        while (idx > 0) {
            size_t p = _getParent(idx);
            if (!invariant(heap[p].m_value, heap[idx].m_value)) {
                swap(heap[idx], heap[p]);
                idx = p;
            } else break;
        }
    }

    void heapifyDown(size_t idx) {
        size_t size = heap.size();
        while (true) {
            size_t l = _getLeft(idx), r = _getRight(idx), target = idx;
            if (l < size && !invariant(heap[target].m_value, heap[l].m_value)) target = l;
            if (r < size && !invariant(heap[target].m_value, heap[r].m_value)) target = r;
            
            if (target != idx) {
                swap(heap[idx], heap[target]);
                idx = target;
            } else break;
        }
    }

public:
    CHeap() = default;

    // Constructor Copia
    CHeap(const CHeap &other) {
        lock_guard<recursive_mutex> lock(other.m_mutex);
        heap = other.heap;
    }

    // Move Constructor
    CHeap(CHeap &&other) noexcept {
        lock_guard<recursive_mutex> lock(other.m_mutex);
        heap = move(other.heap);
    }

    // Destructor Seguro
    virtual ~CHeap() {
        lock_guard<recursive_mutex> lock(m_mutex);
        heap.clear();
    }

    //  Push
    void push(value_type value, ref_type ref = -1) {
        lock_guard<recursive_mutex> lock(m_mutex);
        heap.emplace_back(value, ref);
        heapifyUp(heap.size() - 1);
    }

    // Pop
    value_type pop() {
        lock_guard<recursive_mutex> lock(m_mutex);
        if (heap.empty()) return value_type{};
        
        value_type top = heap.front().m_value;
        heap[0] = move(heap.back());
        heap.pop_back();
        
        if (!heap.empty()) heapifyDown(0);
        return top;
    }

    size_t size() const { lock_guard<recursive_mutex> lock(m_mutex); return heap.size(); }

    friend ostream& operator<< <Traits>(ostream &os, CHeap<Traits> &h);
    friend istream& operator>> <Traits>(istream &is, CHeap<Traits> &h);
};

// Operator <<
template <typename Traits>
ostream& operator<<(ostream &os, CHeap<Traits> &h) {
    lock_guard<recursive_mutex> lock(h.m_mutex);
    os << "CHeap: size = " << h.heap.size() << " [";
    for (size_t i = 0; i < h.heap.size(); ++i) {
        if constexpr (is_same_v<typename Traits::value_type, string>)
            os << "(" << quoted(h.heap[i].m_value) << ":" << h.heap[i].m_ref << ")";
        else
            os << "(" << h.heap[i].m_value << ":" << h.heap[i].m_ref << ")";
        if (i < h.heap.size() - 1) os << ",";
    }
    os << "]";
    return os;
}

// Operator >>
template <typename Traits>
istream& operator>>(istream &is, CHeap<Traits> &h) {
    string bar;
    if (!getline(is, bar, '[')) return is;
    
    vector<pair<typename Traits::value_type, ref_type>> items;
    char ch;
    while (is >> ch && ch != ']') {
        if (ch != '(') continue;
        typename Traits::value_type val;
        ref_type ref;
        if constexpr (is_same_v<typename Traits::value_type, string>) is >> quoted(val);
        else is >> val;
        is.ignore(1, ':');
        is >> ref;
        is.ignore(1, ')');
        items.push_back({val, ref});
    }

    if (!items.empty()) {
        lock_guard<recursive_mutex> lock(h.m_mutex);
        h.heap.clear();
        for (auto &it : items) h.heap.emplace_back(it.first, it.second);
        for (int i = h.heap.size()/2 - 1; i >= 0; --i) h.heapifyDown(i);
    }
    return is;
}

#endif