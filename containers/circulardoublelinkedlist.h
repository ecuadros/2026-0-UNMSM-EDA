#ifndef __CIRCULAR_DOUBLE_LINKED_LIST_H__
#define __CIRCULAR_DOUBLE_LINKED_LIST_H__

#include <type_traits>
#include <iomanip>
#include <mutex>
#include <vector>
#include "../general/types.h"
#include "GeneralIterator.h"
#include "linkedlist.h"

using namespace std;

// reutilizo todo lo que ya hay

template <typename Traits>
class NodeCircularDoubleLinkedList {
    using value_type = typename Traits::value_type;
    using Node = NodeCircularDoubleLinkedList<Traits>;

    value_type m_data;
    ref_type m_ref;
    Node *m_pNext = nullptr;
    Node *m_pPrev = nullptr;
public:
    NodeCircularDoubleLinkedList() {}
    NodeCircularDoubleLinkedList(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref) {}
    NodeCircularDoubleLinkedList(value_type _value, ref_type _ref, Node *pNext, Node *pPrev)
        : m_data(_value), m_ref(_ref), m_pNext(pNext), m_pPrev(pPrev) {}

    value_type  GetValue() const { return m_data; }
    value_type &GetValueRef() { return m_data; }

    ref_type    GetRef() const { return m_ref; }
    ref_type   &GetRefRef() { return m_ref; }

    Node      * GetNext() const { return m_pNext; }
    Node      *&GetNextRef() { return m_pNext; }

    Node      * GetPrev() const { return m_pPrev; }
    Node      *&GetPrevRef() { return m_pPrev; }

    Node &operator=(const Node &another) {
        m_data = another.GetValue();
        m_ref = another.GetRef();
        return *this;
    }
    bool operator==(const Node &another) const { return m_data == another.GetValue(); }
    bool operator<(const Node &another) const { return m_data < another.GetValue(); }
};


template <typename Container>
class CircularDoubleLinkedListForwardIterator : public GeneralIterator<Container> {
public:
    using Parent     = GeneralIterator<Container>;
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

    Node *pCurrent = nullptr;

    CircularDoubleLinkedListForwardIterator(Container *pContainer, Size pos=0)
        : GeneralIterator<Container>(pContainer, pos), pCurrent(pContainer->m_pRoot)
    {
        if (!pCurrent) return;
        size_t n = pContainer->m_nElements;
        if (pos < static_cast<Size>(n / 2)) {
            // ir desde el inicio
            for (Size i = 0; i < pos; ++i)
                pCurrent = pCurrent->GetNext();
        } else {
            pCurrent = pContainer->m_pLast;
            // ir desde el final
            for (Size i = static_cast<Size>(n - 1); i > pos; --i)
                pCurrent = pCurrent->GetPrev();
        }
    }
    CircularDoubleLinkedListForwardIterator(CircularDoubleLinkedListForwardIterator<Container> &another)
        : GeneralIterator<Container>(another), pCurrent(another.pCurrent) {}

    value_type &operator*() override { return pCurrent->GetValueRef(); }
    CircularDoubleLinkedListForwardIterator<Container> &operator++() {
        if (pCurrent) {
            pCurrent = pCurrent->GetNext();
            ++this->m_pos;
        }
        return *this;
    }
    CircularDoubleLinkedListForwardIterator<Container> operator++(int) {
        CircularDoubleLinkedListForwardIterator<Container> tmp(*this);
        ++(*this);
        return tmp;
    }
};


template <typename Container>
class CircularDoubleLinkedListBackwardIterator : public GeneralIterator<Container> {
public:
    using Parent     = GeneralIterator<Container>;
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

    Node *pCurrent = nullptr;

    CircularDoubleLinkedListBackwardIterator(Container *pContainer, Size pos=0)
        : GeneralIterator<Container>(pContainer, pos), pCurrent(nullptr)
    {
        size_t n = pContainer->m_nElements;

        if (pos < 0 || pos >= static_cast<Size>(n)) {
            pCurrent = nullptr;
            return;
        }
        // ubicar el nodo en la posicion pos
        if (pos < static_cast<Size>(n / 2)) {
            pCurrent = pContainer->m_pRoot;
            for (Size i = 0; i < pos; ++i)
                pCurrent = pCurrent->GetNext();
        } else {
            pCurrent = pContainer->m_pLast;
            for (Size i = static_cast<Size>(n - 1); i > pos; --i)
                pCurrent = pCurrent->GetPrev();
        }
    }
    CircularDoubleLinkedListBackwardIterator(CircularDoubleLinkedListBackwardIterator<Container> &another)
        : GeneralIterator<Container>(another), pCurrent(another.pCurrent) {}

    // override necesario del GeneralIterator
    value_type &operator*() override { return pCurrent->GetValueRef(); }
    CircularDoubleLinkedListBackwardIterator<Container> &operator++() {
        if (pCurrent) {
            pCurrent = pCurrent->GetPrev();
            --this->m_pos;
        }
        return *this;
    }
    CircularDoubleLinkedListBackwardIterator<Container> operator++(int) {
        CircularDoubleLinkedListBackwardIterator<Container> tmp(*this);
        ++(*this);
        return tmp;
    }
};


template <typename Traits>
class CCircularDoubleLinkedList;

template <typename Traits>
ostream &operator<<(ostream &os, CCircularDoubleLinkedList<Traits> &container);

template <typename Traits>
istream &operator>>(istream &is, CCircularDoubleLinkedList<Traits> &container);


template <typename Traits>
class CCircularDoubleLinkedList : public ListBase<Traits> {
    mutable mutex mtx;
public:
    using value_type = typename Traits::value_type;
    using forward_iterator = CircularDoubleLinkedListForwardIterator<CCircularDoubleLinkedList<Traits>>;
    using Node = NodeCircularDoubleLinkedList<Traits>;

    friend forward_iterator;
    friend GeneralIterator<CCircularDoubleLinkedList<Traits>>;

    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;

    CCircularDoubleLinkedList() {}
    CCircularDoubleLinkedList(const CCircularDoubleLinkedList &to_copy) {
        lock_guard<mutex> lock(to_copy.mtx);
        _copyNodesFrom(to_copy);
    }
    CCircularDoubleLinkedList(CCircularDoubleLinkedList &&to_move)
        : m_pRoot(to_move.m_pRoot), m_pLast(to_move.m_pLast), m_nElements(to_move.m_nElements) {
        lock_guard<mutex> lock(to_move.mtx);
        to_move.m_pRoot = nullptr;
        to_move.m_pLast = nullptr;
        to_move.m_nElements = 0;
    }
    virtual ~CCircularDoubleLinkedList() { clear_unlocked(); }

    forward_iterator begin() { return forward_iterator(this); }
    forward_iterator end() { return forward_iterator(this, m_nElements); }

    value_type &operator[](size_t index) {
        lock_guard<mutex> lock(mtx);
        if (index >= m_nElements) throw std::out_of_range("Index out of range");
        Node *trav = m_pRoot;
        for (size_t i = 0; i < index; ++i) trav = trav->GetNext();
        return trav->GetValueRef();
    }

    CCircularDoubleLinkedList &operator=(const CCircularDoubleLinkedList &to_copy);

    void push_back(const value_type &val, ref_type ref);
    void insert(const value_type &val, ref_type ref, size_t index = static_cast<size_t>(-1));

    size_t getSize() const {
        lock_guard<mutex> lock(mtx);
        return m_nElements;
    }

    template <typename ObjFunc, typename ...Args>
    void forEach(ObjFunc foo, Args... args) {
        lock_guard<mutex> lock(mtx);
        ::Foreach(*this, foo, args...);
    }

    template <typename ObjFunc, typename ...Args>
    auto firstThat(ObjFunc foo, Args... args) {
        lock_guard<mutex> lock(mtx);
        return *::FirstThat(*this, foo, args...);
    }

    void clear() {
        lock_guard<mutex> lock(mtx);
        clear_unlocked();
    }

private:
    void _internal_insert(const value_type &val, ref_type ref);
    void _insert_at_index(const value_type &val, ref_type ref, size_t index);

    void clear_unlocked() {
        if (!m_pRoot) return;
        Node *trav = m_pRoot;
        for (size_t i = 0; i < m_nElements; ++i) {
            Node *temp = trav->GetNext();
            delete trav;
            trav = temp;
        }
        m_pRoot = m_pLast = nullptr;
        m_nElements = 0;
    }

    void _copyNodesFrom(const CCircularDoubleLinkedList &to_copy) {
        if (this == &to_copy) return;
        clear_unlocked();
        Node *trav = to_copy.m_pRoot;
        for (size_t i = 0; i < to_copy.m_nElements; ++i) {
            push_back(trav->GetValue(), trav->GetRef());
            trav = trav->GetNext();
        }
    }

    friend ostream &operator<< <>(ostream &os, CCircularDoubleLinkedList<Traits> &container);
    friend istream &operator>> <>(istream &is, CCircularDoubleLinkedList<Traits> &container);
};


template <typename Traits>
void CCircularDoubleLinkedList<Traits>::push_back(const value_type &val, ref_type ref) {
    lock_guard<mutex> lock(mtx);

    if constexpr (Traits::ordered) {
        if (m_pLast && this->compare(m_pLast->GetValueRef(), val)) {
            _internal_insert(val, ref);
            return;
        }
    }

    Node *pNew = new Node(val, ref, nullptr, nullptr);
    if (!m_pRoot) {
        m_pRoot = m_pLast = pNew;
        pNew->GetNextRef() = pNew;
        pNew->GetPrevRef() = pNew;
    } else {
        pNew->GetPrevRef() = m_pLast;
        pNew->GetNextRef() = m_pRoot;
        m_pLast->GetNextRef() = pNew;
        m_pRoot->GetPrevRef() = pNew;
        m_pLast = pNew;
    }
    ++m_nElements;
}


template <typename Traits>
void CCircularDoubleLinkedList<Traits>::_internal_insert(const value_type &val, ref_type ref) {
    if (!m_pRoot) {
        Node *pNew = new Node(val, ref, nullptr, nullptr);
        m_pRoot = m_pLast = pNew;
        pNew->GetNextRef() = pNew;
        pNew->GetPrevRef() = pNew;
        ++m_nElements;
        return;
    }

    Node *curr = m_pRoot;
    for (size_t i = 0; i < m_nElements; ++i) {
        if (this->compare(curr->GetValueRef(), val)) {
            Node *pNew = new Node(val, ref, curr, curr->GetPrev());
            curr->GetPrevRef()->GetNextRef() = pNew;
            curr->GetPrevRef() = pNew;
            if (curr == m_pRoot) m_pRoot = pNew;
            ++m_nElements;
            return;
        }
        curr = curr->GetNext();
    }

    Node *pNew = new Node(val, ref, m_pRoot, m_pLast);
    m_pLast->GetNextRef() = pNew;
    m_pRoot->GetPrevRef() = pNew;
    m_pLast = pNew;
    ++m_nElements;
}


template <typename Traits>
void CCircularDoubleLinkedList<Traits>::insert(const value_type &val, ref_type ref, size_t index) {
    if (!m_nElements) {
        push_back(val, ref);
        return;
    }

    lock_guard<mutex> lock(mtx);
    if constexpr (Traits::ordered) {
        _internal_insert(val, ref);
    } else {
        if (index == static_cast<size_t>(-1)) index = m_nElements;
        _insert_at_index(val, ref, index);
    }
}


template <typename Traits>
void CCircularDoubleLinkedList<Traits>::_insert_at_index(const value_type &val, ref_type ref, size_t index) {
    if (index > m_nElements) throw std::out_of_range("Index out of range");

    Node *pNew = new Node(val, ref, nullptr, nullptr);

    if (!m_pRoot) {
        m_pRoot = m_pLast = pNew;
        pNew->GetNextRef() = pNew;
        pNew->GetPrevRef() = pNew;
        ++m_nElements;
        return;
    }

    if (index == 0) {
        pNew->GetNextRef() = m_pRoot;
        pNew->GetPrevRef() = m_pLast;
        m_pRoot->GetPrevRef() = pNew;
        m_pLast->GetNextRef() = pNew;
        m_pRoot = pNew;
        ++m_nElements;
        return;
    }

    if (index == m_nElements) {
        pNew->GetPrevRef() = m_pLast;
        pNew->GetNextRef() = m_pRoot;
        m_pLast->GetNextRef() = pNew;
        m_pRoot->GetPrevRef() = pNew;
        m_pLast = pNew;
        ++m_nElements;
        return;
    }

    Node *trav = m_pRoot;
    for (size_t i = 0; i < index; ++i) trav = trav->GetNext();
    pNew->GetNextRef() = trav;
    pNew->GetPrevRef() = trav->GetPrev();
    trav->GetPrevRef()->GetNextRef() = pNew;
    trav->GetPrevRef() = pNew;
    ++m_nElements;
}


template <typename Traits>
CCircularDoubleLinkedList<Traits> &
CCircularDoubleLinkedList<Traits>::operator=(const CCircularDoubleLinkedList &to_copy) {
    if (this == &to_copy) return *this;

    std::vector<std::pair<value_type, ref_type>> items;
    {
        lock_guard<mutex> lock(to_copy.mtx);
        Node *trav = to_copy.m_pRoot;
        for (size_t i = 0; i < to_copy.m_nElements; ++i) {
            items.emplace_back(trav->GetValue(), trav->GetRef());
            trav = trav->GetNext();
        }
    }
    {
        lock_guard<mutex> lock(mtx);
        clear_unlocked();
    }
    for (const auto &item : items) push_back(item.first, item.second);
    return *this;
}


template <typename Traits>
ostream &operator<<(ostream &os, CCircularDoubleLinkedList<Traits> &container) {
    using value_type = typename CCircularDoubleLinkedList<Traits>::value_type;
    lock_guard<mutex> lock(container.mtx);
    os << "CCircularDoubleLinkedList: size = " << container.m_nElements << " [";
    auto trav = container.m_pRoot;
    for (size_t i = 0; i < container.m_nElements; ++i) {
        if constexpr (std::is_same_v<value_type, std::string>) {
            os << "(" << std::quoted(trav->GetValue()) << ":" << trav->GetRef() << "),";
        } else {
            os << "(" << trav->GetValue() << ":" << trav->GetRef() << "),";
        }
        trav = trav->GetNext();
    }
    os << "]" << endl;
    return os;
}


template <typename Traits>
istream &operator>>(istream &is, CCircularDoubleLinkedList<Traits> &container) {
    using value_type = typename CCircularDoubleLinkedList<Traits>::value_type;
    if (!is) return is;

    CCircularDoubleLinkedList<Traits> tmp;
    try {
        lock_guard<mutex> lock(container.mtx);
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

            tmp.push_back(val, ref);
        }
    } catch (const exception& e) {
        is.setstate(ios::failbit);
    }
    container = std::move(tmp);
    return is;
}

#endif // __CIRCULAR_DOUBLE_LINKED_LIST_H__
