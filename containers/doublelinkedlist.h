#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__

#include <iostream>
#include <type_traits>
#include <iomanip>
#include <mutex>
#include <vector>
#include "../general/types.h"
#include "GeneralIterator.h"
#include "linkedlist.h"
using namespace std;

// ForwardIterator para listas doblemente enlazadas
// BackwardIterator para listas doblemente enlazadas
template <typename Traits>
class NodeDoubleLinkedList {
    using  value_type  = typename Traits::value_type;
    using  Node        = NodeDoubleLinkedList<Traits>;

    value_type m_data;
    ref_type   m_ref;
    Node *m_pNext = nullptr;
    Node *m_pPrev = nullptr;
public:
    NodeDoubleLinkedList() {}
    NodeDoubleLinkedList(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref) {}
    NodeDoubleLinkedList(value_type _value, ref_type _ref, Node *pNext, Node *pPrev)
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
class DoubleLinkedListForwardIterator : public GeneralIterator<Container> {
public:
    using Parent     = GeneralIterator<Container>;
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

    Node *pCurrent = nullptr;

    DoubleLinkedListForwardIterator(Container *pContainer, Size pos=0)
        : GeneralIterator<Container>(pContainer, pos), pCurrent(pContainer->m_pRoot)
    {
        // se actualiza pCurrent dependiendo de donde se encuentre en pContainer
        size_t n = pContainer->m_nElements;
        if (!pCurrent) return;
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
    DoubleLinkedListForwardIterator(DoubleLinkedListForwardIterator<Container> &another)
        : GeneralIterator<Container>(another), pCurrent(another.pCurrent) {}

    value_type &operator*() override { return pCurrent->GetValueRef(); }
    DoubleLinkedListForwardIterator<Container> &operator++() {
        if (pCurrent) {
            pCurrent = pCurrent->GetNext();
            ++this->m_pos;
        }
        return *this;
    }
    DoubleLinkedListForwardIterator<Container> operator++(int) {
        DoubleLinkedListForwardIterator<Container> tmp(*this);
        ++(*this);
        return tmp;
    }
};

template <typename Container>
class DoubleLinkedListBackwardIterator : public GeneralIterator<Container> {
public:
    using Parent     = GeneralIterator<Container>;
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

    Node *pCurrent = nullptr;

    DoubleLinkedListBackwardIterator(Container *pContainer, Size pos=0)
        : GeneralIterator<Container>(pContainer, pos), pCurrent(pContainer->m_pLast)
    {
        size_t n = pContainer->m_nElements;
        if (!pCurrent) return;
        // actualize pCurrent dependiendo de su ubicacion en pContainer
        if (pos < static_cast<Size>(n / 2)) {
            for (Size i = 0; i < pos; ++i)
                pCurrent = pCurrent->GetPrev();
        } else {
            pCurrent = pContainer->m_pRoot;
            for (Size i = static_cast<Size>(n - 1); i > pos; --i)
                pCurrent = pCurrent->GetNext();
        }
    }
    DoubleLinkedListBackwardIterator(DoubleLinkedListBackwardIterator<Container> &another)
        : GeneralIterator<Container>(another), pCurrent(another.pCurrent) {}

    value_type &operator*() override { return pCurrent->GetValueRef(); }
    DoubleLinkedListBackwardIterator<Container> &operator++() {
        if (pCurrent) {
            pCurrent = pCurrent->GetPrev();
            --this->m_pos;
        }
        return *this;
    }
    DoubleLinkedListBackwardIterator<Container> operator++(int) {
        DoubleLinkedListBackwardIterator<Container> tmp(*this);
        ++(*this);
        return tmp;
    }
};


template <typename Traits>
class CDoubleLinkedList;

template <typename Traits>
ostream &operator<<(ostream &os, CDoubleLinkedList<Traits> &container);

template <typename Traits>
istream &operator>>(istream &is, CDoubleLinkedList<Traits> &container);


template <typename Traits>
class CDoubleLinkedList : public ListBase<Traits> {
    mutable mutex mtx;
public:
    using value_type       = typename Traits::value_type;
    using forward_iterator = DoubleLinkedListForwardIterator<CDoubleLinkedList<Traits>>;
    using backward_iterator = DoubleLinkedListBackwardIterator<CDoubleLinkedList<Traits>>;
    using Node = NodeDoubleLinkedList<Traits>;

    friend forward_iterator;
    friend backward_iterator;
    friend GeneralIterator<CDoubleLinkedList<Traits>>;

    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;

    // bloquear el mutex en los copy y move constructors

    CDoubleLinkedList(){}
    CDoubleLinkedList(const CDoubleLinkedList &to_copy)
    : m_pRoot(nullptr), m_pLast(nullptr) {
        lock_guard<mutex> lock(to_copy.mtx);
        _copyNodesFrom(to_copy);
    }
    CDoubleLinkedList(CDoubleLinkedList &&to_move)
    : m_pRoot(to_move.m_pRoot), m_pLast(to_move.m_pLast),
      m_nElements(to_move.m_nElements)
    {
        lock_guard<mutex> lock(to_move.mtx);
        to_move.m_pRoot     = nullptr;
        to_move.m_pLast     = nullptr;
        to_move.m_nElements = 0;
    }
    virtual ~CDoubleLinkedList() {
        clear_unlocked();
    }

    forward_iterator  begin()  { return forward_iterator(this); }
    forward_iterator  end()    { return forward_iterator(this, m_nElements); }
    backward_iterator rbegin() { return backward_iterator(this, 0); }
    backward_iterator rend()   { return backward_iterator(this, m_nElements); }

    size_t getSize() {
        lock_guard<mutex> lock(mtx);
        return m_nElements;
    }

    value_type &operator[](size_t index);
    CDoubleLinkedList &operator=(const CDoubleLinkedList &to_copy);

    void push_back(const value_type &val, ref_type ref);
    void insert(const value_type &val, ref_type ref, size_t index = static_cast<size_t>(-1));

    // foreach: bloquea el mutex
    template <typename ObjFunc, typename ...Args>
    void forEach(ObjFunc foo, Args... args) {
        lock_guard<mutex> lock(mtx);
        ::Foreach(begin(), end(), foo, args...);
    }
    // firstThat: bloquea el mutex
    template <typename ObjFunc, typename ...Args>
    value_type firstThat(ObjFunc foo, Args... args) {
        lock_guard<mutex> lock(mtx);
        return *::FirstThat(begin(), end(), foo, args...);
    }

    // funcion publica: bloquea el mutex
    void clear() {
        lock_guard<mutex> lock(mtx);
        clear_unlocked();
    }

private:
    void _internal_insert(Node *&rCurrentNode, const value_type &val, ref_type ref);
    void _insert_at_index(const value_type &val, ref_type ref, size_t index);

    void clear_unlocked() {
        // un siple clear
        auto trav = m_pRoot;
        while (trav) {
            auto temp = trav->GetNext();
            delete trav;
            trav = temp;
        }
        m_pRoot = m_pLast = nullptr;
        m_nElements = 0;
    }

    friend ostream &operator<< <>(ostream &os, CDoubleLinkedList<Traits> &container);
    friend istream &operator>> <>(istream &is, CDoubleLinkedList<Traits> &container);

    // copia los nodos, depende de que el mutex de to_copy no este bloqueado
    void _copyNodesFrom(const CDoubleLinkedList &to_copy) {
        if (this == &to_copy) return;
        clear_unlocked();

        Node* trav = to_copy.m_pRoot;
        while (trav) {
            value_type val = trav->GetValue();
            ref_type   ref = trav->GetRef();
            push_back(val, ref);
            trav = trav->GetNext();
        }
    }
};

// =========================
// === CDoublyLinkedList ===
// =========================

// simple pushback
template <typename Traits>
void CDoubleLinkedList<Traits>::push_back(const value_type &val, ref_type ref) {
    lock_guard<mutex> lock(mtx);
    if constexpr (Traits::ordered) {
        if ( m_pLast && this->compare(m_pLast->GetValueRef(), val) ) {
            _internal_insert(m_pRoot, val, ref);
            return;
        }
    }

    Node *pNewNode = new Node(val, ref, nullptr, m_pLast);
    if ( !m_pRoot ) {
        m_pRoot = m_pLast = pNewNode;
    } else {
        // ademas de cambiar el nextRef, tambien se cambia el prevRef
        m_pLast->GetNextRef() = pNewNode;
        pNewNode->GetPrevRef() = m_pLast;
        m_pLast = pNewNode;
    }
    ++m_nElements;
}


template <typename Traits>
void CDoubleLinkedList<Traits>::_internal_insert(
    Node *&rCurrentNode, const value_type &val, ref_type ref
    ) {
    // el caso en que se llega al ultimo nodo
    if (!rCurrentNode) {
        Node *pNew = new Node(val, ref, nullptr, m_pLast);

        if (!m_pRoot) m_pRoot = pNew;
        if (m_pLast) m_pLast->GetNextRef() = pNew;
        m_pLast = pNew;
        ++m_nElements;
        return;
    }

    if ( this->compare(rCurrentNode->GetValueRef(), val ) ) {
        Node *pNew = new Node(val, ref, rCurrentNode, rCurrentNode->GetPrev());
        // en caso se inserte al inicio, se cambia la ref de root
        // sino se cambia la referencia del nodo anterior
        if (rCurrentNode == m_pRoot) m_pRoot = pNew;
        else rCurrentNode->GetPrevRef()->GetNextRef() = pNew;
        // current node apunta a pNew hacia atras
        rCurrentNode->GetPrevRef() = pNew;
        ++m_nElements;
        return;
    }

    _internal_insert(rCurrentNode->GetNextRef(), val, ref);
}

template <typename Traits>
void CDoubleLinkedList<Traits>::insert(const value_type &val, ref_type ref, size_t index) {
    if (!getSize()) {
        push_back(val, ref);
        return;
    }

    lock_guard<mutex> lock(mtx);

    if constexpr (Traits::ordered) {
        _internal_insert(m_pRoot, val, ref);
    } else {
        if (index == static_cast<size_t>(-1)) {
            index = m_nElements;
        }
        _insert_at_index(val, ref, index);
    }
}

/*
 * metodo privado creada para manejar el caso en que se utilice una lista
 * no ordenada (UnorderedTrait)
 */
template <typename Traits>
void CDoubleLinkedList<Traits>::_insert_at_index(const value_type &val, ref_type ref, size_t index) {
    if (index > m_nElements) throw std::out_of_range("Index out of range");

    Node *pNew = new Node(val, ref, nullptr, nullptr);

    // logica para insertar al inicio
    if (!index) {
        pNew->GetNextRef() = m_pRoot;
        if (m_pRoot) m_pRoot->GetPrevRef() = pNew;
        m_pRoot = pNew;
        // por siacaso
        if (!m_pLast) m_pLast = pNew;
        ++m_nElements;
        return;
    }

    Node *trav = m_pRoot;
    // traveler termina un nodo antes del indice
    for (size_t i = 0; i + 1 < index; ++i) {
        trav = trav->GetNext();
    }
    // ajuste de punteros
    pNew->GetNextRef() = trav->GetNext();
    pNew->GetPrevRef() = trav;
    if (trav->GetNext()) trav->GetNext()->GetPrevRef() = pNew;
    trav->GetNextRef() = pNew;
    if (trav == m_pLast) m_pLast = pNew;
    ++m_nElements;
}

// implementado operador []
template <typename Traits>
typename CDoubleLinkedList<Traits>::value_type &
CDoubleLinkedList<Traits>::operator[](const size_t index) {
    lock_guard<mutex> lock(mtx);

    if (index >= m_nElements) throw std::out_of_range("Index out of range");

    Node *trav;
    // se decide de donde empezar a iterar dependiendo de index
    if (index < m_nElements / 2) {
        trav = m_pRoot;
        for (size_t i = 0; i < index; ++i)
            trav = trav->GetNext();
    } else {
        trav = m_pLast;
        for (size_t i = m_nElements - 1; i > index; --i)
            trav = trav->GetPrev();
    }
    return trav->GetValueRef();
}


// implementacion operador =
template <typename Traits>
CDoubleLinkedList<Traits>& CDoubleLinkedList<Traits>::operator=(const CDoubleLinkedList &to_copy) {
    // ahorrarse el trabajo si es la misma instancia
    if (this == &to_copy) return *this;
    // se saca snapshop de la linkedlist antes de copiarla
    std::vector<std::pair<value_type, ref_type>> items;
    {
        lock_guard<mutex> lock(to_copy.mtx);
        for (auto trav = to_copy.m_pRoot; trav; trav = trav->GetNext()) {
            items.emplace_back(trav->GetValue(), trav->GetRef());
        }
    }
    {
        lock_guard<mutex> lock(mtx);
        clear_unlocked();
    }

    // se copia
    for (const auto &item : items) push_back(item.first, item.second);

    return *this;
}

// operador right shift
template <typename Traits>
ostream &operator<<(ostream &os, CDoubleLinkedList<Traits> &container) {
    using value_type = typename CDoubleLinkedList<Traits>::value_type;

    lock_guard<mutex> lock(container.mtx);
    os << "CDoubleLinkedList: size = " << container.m_nElements << " [";
    for (auto trav = container.m_pRoot; trav; trav = trav->GetNext()) {
        if constexpr (std::is_same_v<value_type, std::string>) {
            os << "(" << std::quoted(trav->GetValue()) << ":" << trav->GetRef() << "),";
        } else {
            os << "(" << trav->GetValue() << ":" << trav->GetRef() << "),";
        }
    }
    os << "]" << endl;
    return os;
}

// operador left shift
template <typename Traits>
istream &operator>>(istream &is, CDoubleLinkedList<Traits> &container) {
    using value_type = typename CDoubleLinkedList<Traits>::value_type;
    if (!is) return is;

    CDoubleLinkedList<Traits> tmp;
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

#endif // __DOUBLE_LINKED_LIST_H__
