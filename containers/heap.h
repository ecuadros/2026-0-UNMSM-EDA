#ifndef __HEAP_H__
#define __HEAP_H__

#include <functional>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "../general/types.h"
#include "array.h"

using namespace std;


template <typename T, typename _Func>
struct HeapTrait {
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingHeapTrait : HeapTrait<T, std::greater<T> >{
};

template <typename T>
struct DescendingHeapTrait : HeapTrait<T, std::less<T> >{
};

template <typename Traits>
class CHeap;

template <typename Traits>
ostream& operator<<(ostream &os, CHeap<Traits> &container);

template <typename Traits>
istream& operator>>(istream &is, CHeap<Traits> &container);

template <typename Traits>
class CHeap {
    mutable mutex mtx;
    using value_type = typename Traits::value_type;
    using compareFunc = typename Traits::Func;

    struct Node {
        value_type m_value;
        ref_type m_ref;

        Node(){}
        Node( value_type _value, ref_type _ref = -1)
            : m_value(_value), m_ref(_ref){   }

        value_type  GetValue   () const { return m_value; }
        value_type &GetValueRef() { return m_value; }

        ref_type    GetRef     () const { return m_ref;   }
        ref_type   &GetRefRef  () { return m_ref;   }
        Node &operator=(const Node &another){
            m_value = another.GetValue();
            m_ref   = another.GetRef();
            return *this;
        }
        bool operator==(const Node &another) const
        { return m_value == another.GetValue();   }
        bool operator<(const Node &another) const
        { return m_value < another.GetValue();   }
    };
    vector<Node> heap;

public:
    CHeap() = default;
    CHeap(const CHeap &to_copy);
    CHeap(CHeap &&to_move) noexcept ;
    virtual ~CHeap();
    CHeap &operator=(const CHeap &to_copy);

    void push(value_type value, ref_type ref);
    value_type pop();
    size_t size() const {
        lock_guard lock(mtx);
        return heap.size();
    }

    friend ostream& operator<< <>(ostream &os, CHeap<Traits> &container);
    friend istream& operator>> <>(istream &is, CHeap<Traits> &container);

private:
    static bool invariant(const value_type &parent, const value_type &child) {
        return compareFunc{}(parent, child);
    }

    void _clear();

    static size_t _getParent(const size_t index) {
        return index == 0 ? 0 : (index-1)/2;
    }
    static size_t _getLeftChild(const size_t index) {
        return 2*index + 1;
    }
    static size_t _getRightChild(const size_t index) {
        return 2*index + 2;
    }

    void _bubble_up(size_t index);
    void _bubble_down(size_t index);

    void _swap(size_t a, size_t b) {
        Node tmp = heap[a];
        heap[a] = heap[b];
        heap[b] = tmp;
    }
};

template<typename Traits>
void CHeap<Traits>::_bubble_up(size_t index) {
    if (index == 0) return;
    size_t parent_i = _getParent(index);

    if (heap.size() > index &&
        !invariant(heap[parent_i].GetValue(), heap[index].GetValue())) {
        _swap(index, parent_i);
        _bubble_up(parent_i);
    }
}

template<typename Traits>
void CHeap<Traits>::push(value_type value, ref_type ref) {
    lock_guard lock(mtx);
    heap.emplace_back(Node(value, ref));
    // si el heap estaba vacio no hay nada que hacer
    if (heap.size() <= 1) return;

    _bubble_up(heap.size() - 1);
}

template<typename Traits>
void CHeap<Traits>::_bubble_down(size_t index) {
    const size_t size = heap.size();
    while (true) {
        size_t left_i = _getLeftChild(index);
        size_t right_i = _getRightChild(index);
        // si no hay hijos no hay nada que hacer
        if (left_i >= heap.size()) break;

        size_t best_i = left_i;
        // se escoge al menor/mayor para intercambiar
        if (right_i < size &&
            invariant(heap[right_i].GetValue(), heap[left_i].GetValue())) {
            best_i = right_i;
        }
        // si ya esta "ordenado", se rompe el ciclo
        if (invariant(heap[index].GetValue(), heap[best_i].GetValue())) break;
        // si no, se intercambia con el mejor escogido y continua el ciclo
        _swap(index, best_i);
        index = best_i;
    }
}


template<typename Traits>
typename CHeap<Traits>::value_type CHeap<Traits>::pop() {
    lock_guard lock(mtx);
    // si el heap esta vacio no hay nada que hacer
    if (heap.empty()) return value_type {};

    // si solo hay un nodo, se hace pop sin mas
    value_type top = heap.front().GetValue();
    if (heap.size() == 1) {
        heap.pop_back();
        return top;
    }
    // si hay mas nodos, se procede a bubble down
    _swap(0, heap.size() - 1);
    heap.pop_back();
    _bubble_down(0);
    return top;
}

template<typename Traits>
CHeap<Traits>::CHeap(const CHeap &to_copy) {
    lock_guard lock(to_copy.mtx);
    heap = to_copy.heap;
}

template<typename Traits>
CHeap<Traits>::CHeap(CHeap &&to_move) noexcept {
    lock_guard lock(to_move.mtx);
    heap = std::exchange(to_move.heap, {});
}

template <typename Traits>
CHeap<Traits>::~CHeap() {
    lock_guard lock(mtx);
    _clear();
}

template<typename Traits>
CHeap<Traits> &CHeap<Traits>::operator=(const CHeap &to_copy) {
    if (this == &to_copy) return *this;
    // se copia a un vector temporal
    vector<Node> items;
    {
        lock_guard lock(to_copy.mtx);
        items = to_copy.heap;
    }
    // si sale bien se hace move a this->heap
    {
        lock_guard lock(mtx);
        heap = std::move(items);
    }
    return *this;
}

template<typename Traits>
void CHeap<Traits>::_clear() {
    heap.clear();
}

template <typename Traits>
ostream &operator<<(ostream &os, CHeap<Traits> &container) {
    using value_type = typename CHeap<Traits>::value_type;
    lock_guard<mutex> lock(container.mtx);
    os << "CHeap: size = " << container.heap.size() << " [";
    for (const auto &node : container.heap) {
        if constexpr (std::is_same_v<value_type, std::string>) {
            os << "(" << std::quoted(node.GetValue()) << ":" << node.GetRef() << "),";
        } else {
            os << "(" << node.GetValue() << ":" << node.GetRef() << "),";
        }
    }
    os << "]" << endl;
    return os;
}

template <typename Traits>
istream &operator>>(istream &is, CHeap<Traits> &container) {
    using value_type = typename CHeap<Traits>::value_type;
    if (!is) return is;

    vector<pair<value_type, ref_type>> items;
    try {
        string bar;
        getline(is, bar, '[');

        char ch;
        while (is.get(ch) && ch != ']') {
            if (ch != '(') continue;
            value_type val;
            ref_type ref;

            if constexpr (std::is_same_v<value_type, std::string>) {
                is >> std::quoted(val);
                getline(is, bar, ':');
            } else {
                is >> val;
                getline(is, bar, ':');
            }
            is >> ref;
            getline(is, bar, ')');
            items.emplace_back(val, ref);
        }
    } catch (const exception&) {
        is.setstate(ios::failbit);
    }

    if (is) {
        lock_guard<mutex> lock(container.mtx);
        container._clear();
        for (const auto &item : items) {
            container.heap.emplace_back(typename CHeap<Traits>::Node(item.first, item.second));
        }
        if (container.heap.size() > 1) {
            for (size_t i = container.heap.size() / 2; i-- > 0;) {
                container._bubble_down(i);
            }
        }
    }
    return is;
}

void DemoHeap();


#endif // __HEAP_H__
