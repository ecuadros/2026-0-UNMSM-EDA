//
// Created by aroon on 2/8/26.
//

#ifndef __CIRCULARLINKEDLIST_H__
#define __CIRCULARLINKEDLIST_H__

#include "linkedlist.h"


template <typename Traits>
class CircularLinkedList_Node {
    using value_type = typename Traits::value_type;
    using Node       = CircularLinkedList_Node<Traits>;
    value_type m_data;
    ref_type   m_ref;
    Node *pNext = nullptr;

public:
    CircularLinkedList_Node() {}
    CircularLinkedList_Node(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref) {}
    CircularLinkedList_Node(value_type _value, ref_type _ref, Node *pNext)
        : m_data(_value), m_ref(_ref), pNext(pNext) {}

    value_type  GetValue() const { return m_data; }
    value_type &GetValueRef() { return m_data; }
    ref_type    GetRef() const { return m_ref; }
    ref_type   &GetRefRef() { return m_ref; }
    Node       *GetNext() const { return pNext; }
    Node      *&GetNextRef() { return pNext; }

    Node &operator=(const Node &another) {
        m_data = another.GetValue();
        m_ref = another.GetRef();
        return *this;
    }
    bool operator==(const Node &another) const { return m_data == another.GetValue(); }
    bool operator<(const Node &another) const { return m_data < another.GetValue(); }
};


template <typename Container>
class CircularLinkedListForwardIterator : public GeneralIterator<Container> {
public:
    using Parent     = GeneralIterator<Container>;
    using value_type = typename Parent::value_type;
    using Node       = typename Parent::Node;

    Node *pCurrent = nullptr;

    CircularLinkedListForwardIterator(Container *pContainer, Size pos=0)
        : GeneralIterator<Container>(pContainer, pos),
          pCurrent(pContainer ? pContainer->m_pRoot : nullptr) {
        if (!pCurrent) return;
        for (Size i = 0; i < pos; ++i) pCurrent = pCurrent->GetNext();
    }
    CircularLinkedListForwardIterator(CircularLinkedListForwardIterator<Container> &another)
        : GeneralIterator<Container>(another), pCurrent(another.pCurrent) {}

    value_type &operator*() override { return pCurrent->GetValueRef(); }
    CircularLinkedListForwardIterator<Container> &operator++() {
        if (pCurrent) {
            pCurrent = pCurrent->GetNext();
            ++this->m_pos;
        }
        return *this;
    }
    CircularLinkedListForwardIterator<Container> operator++(int) {
        CircularLinkedListForwardIterator<Container> tmp(*this);
        ++(*this);
        return tmp;
    }
};

template <typename Container>
class CircularLinkedListBackwardIterator: public GeneralIterator<Container> {
public:
    using Parent     = GeneralIterator<Container>;
    using value_type = typename Parent::value_type;
    using Node       = typename Parent::Node;
    Node *pCurrent = nullptr;

    CircularLinkedListBackwardIterator(Container *pContainer, Size pos=0)
        : GeneralIterator<Container>(pContainer, pos),
          pCurrent(pContainer->m_pLast)
    {
        if (!pCurrent) return;
        for (Size i = 0; i < pos; ++i) {
            auto trav = pContainer->m_pRoot;
            while (trav && trav->GetNext() != pCurrent) trav = trav->GetNext();
            pCurrent = trav;
        }
    }
    CircularLinkedListBackwardIterator(CircularLinkedListBackwardIterator<Container> &another)
        : GeneralIterator<Container>(another), pCurrent(another.pCurrent) {}

    value_type &operator*() override { return pCurrent->GetValueRef(); }
    CircularLinkedListBackwardIterator<Container> &operator++() {
        if (pCurrent) {
            auto trav = this->m_pContainer->m_pRoot;
            while (trav && trav->GetNext() != pCurrent) trav = trav->GetNext();
            pCurrent = trav;
            ++this->m_pos;
        }
        return *this;
    }
    CircularLinkedListBackwardIterator<Container> operator++(int) {
        CircularLinkedListBackwardIterator<Container> tmp(*this);
        ++(*this);
        return tmp;
    }
};


template <typename Traits>
class CCircularLinkedList;

template <typename Traits>
ostream &operator<<(ostream &os, CCircularLinkedList<Traits> &container);

template <typename Traits>
istream &operator>>(istream &is, CCircularLinkedList<Traits> &container);


template <typename Traits>
class CCircularLinkedList : public ListBase<Traits> {
    mutable mutex mtx;
public:
    using value_type        = typename Traits::value_type;
    using forward_iterator  = CircularLinkedListForwardIterator<CCircularLinkedList<Traits>>;
    using backward_iterator = CircularLinkedListBackwardIterator<CCircularLinkedList<Traits>>;
    friend forward_iterator;
    friend backward_iterator;
    using Node = CircularLinkedList_Node<Traits>;
    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;

    CCircularLinkedList() {}
    // copy constructor
    CCircularLinkedList(const CCircularLinkedList &to_copy) {
        lock_guard<mutex> lock(to_copy.mtx);
        _copyNodesFrom(to_copy);
    }
    // move constructor
    CCircularLinkedList(CCircularLinkedList &&to_move)
        : m_pRoot(to_move.m_pRoot), m_pLast(to_move.m_pLast), m_nElements(to_move.m_nElements) {
        lock_guard<mutex> lock(to_move.mtx);
        to_move.m_pRoot = nullptr;
        to_move.m_pLast = nullptr;
        to_move.m_nElements = 0;
    }
    // destructor
    virtual ~CCircularLinkedList() {
        clear_unlocked();
    }

    forward_iterator  begin()  { return forward_iterator(this); }
    forward_iterator  end()    { return forward_iterator(this, m_nElements); }
    backward_iterator rbegin() { return backward_iterator(this, 0); }
    backward_iterator rend()   { return backward_iterator(this, m_nElements); }

    // operador []
    value_type &operator[](size_t index) {
        lock_guard<mutex> lock(mtx);
        if (index >= m_nElements) throw std::out_of_range("Index out of range");
        Node *pTrav = m_pRoot;
        for (size_t i = 0; i < index; ++i) pTrav = pTrav->GetNext();
        return pTrav->GetValueRef();
    }
    // operador de asignacion, why not?
    CCircularLinkedList &operator=(const CCircularLinkedList &to_copy);

    // push_back
    void push_back(const value_type &val, ref_type ref);

    // insert, funcion publica
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
        return ::FirstThat(*this, foo, args...);
    }

    void clear() {
        lock_guard<mutex> lock(mtx);
        clear_unlocked();
    }

private:
    void _internal_insert(Node *&rCurrentNode, const value_type &val, ref_type ref);

    void _insert_at_index(const value_type &val, ref_type ref, size_t index);

    void clear_unlocked() {
        if (!m_pRoot) return;
        Node *pTrav = m_pRoot;
        for (size_t i = 0; i < m_nElements; ++i) {
            Node *temp = pTrav->GetNext();
            delete pTrav;
            pTrav = temp;
        }
        m_pRoot = m_pLast = nullptr;
        m_nElements = 0;
    }

    void _copyNodesFrom(const CCircularLinkedList &to_copy) {
        if (this == &to_copy) return;
        clear_unlocked();

        Node *pTrav = to_copy.m_pRoot;
        for (size_t i = 0; i < to_copy.m_nElements; ++i) {
            value_type val = pTrav->GetValue();
            ref_type ref = pTrav->GetRef();
            push_back(val, ref);
            pTrav = pTrav->GetNext();
        }
    }

    friend ostream &operator<< <>(ostream &os, CCircularLinkedList<Traits> &container);
    friend istream &operator>> <>(istream &is, CCircularLinkedList<Traits> &container);
};


// ===========================
// === CCircularLinkedList ===
// ===========================

template <typename Traits>
CCircularLinkedList<Traits> &
CCircularLinkedList<Traits>::operator=(const CCircularLinkedList &to_copy) {
    if (this == &to_copy) return *this;
    // se saca snapshot de la linked list a copiar
    std::vector<std::pair<value_type, ref_type>> items;
        {
        lock_guard<mutex> lock(to_copy.mtx);
        Node *pTrav = to_copy.m_pRoot;
        for (size_t i = 0; i < to_copy.m_nElements; ++i) {
            items.emplace_back(pTrav->GetValue(), pTrav->GetRef());
            pTrav = pTrav->GetNext();
        }
        }
    // limpia los nodos de la instancia donde se copiara
        {
        lock_guard<mutex> lock(mtx);
        clear_unlocked();
        }
    // se añaden los nodos
    for (const auto &item : items) {
        push_back(item.first, item.second);
    }
    return *this;
}

template <typename Traits>
void CCircularLinkedList<Traits>::push_back(
    const value_type &val, ref_type ref)
{
    // bloquea elmutex
    lock_guard<mutex> lock(mtx);
    // esto se define en compile-time, si la lista es ordenada o no
    if constexpr (Traits::ordered) {
        // si es ordenada, utiliza el _internal_insert
        // porque se pondria algo al final que no respete el orden?
        if (m_pLast && this->compare(m_pLast->GetValueRef(), val)) {
            _internal_insert(m_pRoot, val, ref);
            return;
        }
    }
    // si no es ordenada, poner al final
    Node *pNewNode = new Node(val, ref);
    if (!m_pRoot) {
        // pNewNode apunta a si mismo (unico nodo)
        m_pRoot = m_pLast = pNewNode;
        pNewNode->GetNextRef() = pNewNode;
    } else {
        // actuailizar m_pLast
        m_pLast->GetNextRef() = pNewNode;
        m_pLast = pNewNode;
        m_pLast->GetNextRef() = m_pRoot;
    }
    ++m_nElements;
}

template <typename Traits>
void CCircularLinkedList<Traits>::insert(
    const value_type &val, ref_type ref, size_t index)
{
    // antes de bloquear el lock, verifica si no hay elementos
    if (!m_nElements) {
        // seria el equivalente a un push_back
        push_back(val, ref);
        return;
    }
    // bloquea el mutex
    lock_guard<mutex> lock(mtx);
    // dependiendo si la lista es ordenada, se inserta siguiendo orden
    // o en algun lugar en especifico (al final por default)
    if constexpr (Traits::ordered) {
        _internal_insert(m_pRoot, val, ref);
    } else {
        if (index == static_cast<size_t>(-1)) index = m_nElements;
        _insert_at_index(val, ref, index);
    }
}

// no bloquea el mutex, este se bloquea en insert
template <typename Traits>
void CCircularLinkedList<Traits>::_internal_insert(
    Node *&rCurrentNode, const value_type &val, ref_type ref)
{
    // funcion recursiva: se manejan los casos primero
    // y la llamada recursiva se encuentra al final

    /*
    if (!m_pRoot) {
        Node *pNew = new Node(val, ref);
        m_pRoot = m_pLast = pNew;
        pNew->GetNextRef() = pNew;
        ++m_nElements;
        return;
    }
    */

    // si se llego al final de la lista
    if (rCurrentNode == m_pLast) {
        Node *pNew = new Node(val, ref);
        // el nuevo nodo apunta a root y se actualiza todo
        pNew->GetNextRef() = m_pRoot;
        m_pLast->GetNextRef() = pNew;
        m_pLast = pNew;
        ++m_nElements;
        return;
    }
    // esta en orden en algun lugar de la lista
    if (this->compare(rCurrentNode->GetValueRef(), val)) {
        Node *pNew = new Node(val, ref);
        // si esta en la raiz
        if (rCurrentNode == m_pRoot) {
            pNew->GetNextRef() = m_pRoot;
            m_pRoot = pNew;
            m_pLast->GetNextRef() = m_pRoot;
        } else {
            /*
            Node *prev = m_pRoot;
            while (prev->GetNext() != rCurrentNode) prev = prev->GetNext();
            pNew->GetNextRef() = rCurrentNode;
            prev->GetNextRef() = pNew;
            */
            pNew->GetNextRef() = rCurrentNode;
            rCurrentNode = pNew;
        }
        ++m_nElements;
        return;
    }

    _internal_insert(rCurrentNode->GetNextRef(), val, ref);
}

template <typename Traits>
void CCircularLinkedList<Traits>::_insert_at_index(
    const value_type &val, ref_type ref, size_t index)
{
    // si el indice es mayor, suelta un error
    if (index > m_nElements) throw std::out_of_range("Index out of range");

    Node *pNew = new Node(val, ref);
    // si el index es 0
    if (!index) {
        /*
        if (!m_pRoot) {
            m_pRoot = m_pLast = pNew;
            pNew->GetNextRef() = pNew;
            ++m_nElements;
            return;
        }
        */
        pNew->GetNextRef() = m_pRoot;
        m_pRoot = pNew;
        m_pLast->GetNextRef() = m_pRoot;
        ++m_nElements;
        return;
    }

    // si el indice no es 0
    Node *pTrav = m_pRoot;
    for (auto i = 0; i + 1 < index; ++i) pTrav = pTrav->GetNext();
    // pnew apunta al siguiente de ptrav
    // y ptrav conecta con pnew
    pNew->GetNextRef() = pTrav->GetNext();
    pTrav->GetNextRef() = pNew;

    if (pTrav == m_pLast) {
        m_pLast = pNew;
        m_pLast->GetNextRef() = m_pRoot;
    }
    ++m_nElements;
}

template <typename Traits>
ostream &operator<<(ostream &os, CCircularLinkedList<Traits> &container) {
    lock_guard<mutex> lock(container.mtx);
    os << "CCircularLinkedList: size = " << container.m_nElements << " [";
    auto trav = container.m_pRoot;
    for (size_t i = 0; i < container.m_nElements; ++i) {
        if constexpr (std::is_same_v<typename CCircularLinkedList<Traits>::value_type, std::string>) {
            os << "(" << std::quoted(trav->GetValue()) << ":" << trav->GetRef() << "),";
        } else {
            os << "(" << trav->GetValue() << ":" << trav->GetRef() << "),";
        }
        trav = trav->GetNext();
    }
    os << "]" << endl;
    return os;
}


// lee del input stream esperando el formato en el que se escribe
template <typename Traits>
istream &operator>>(istream &is, CCircularLinkedList<Traits> &container) {
    if (!is) return is;
    try {
        CCircularLinkedList<Traits> tmp;
        string bar;
        getline(is, bar, '[');

        char ch;
        while (is.get(ch) && ch != ']') {
            if (ch != '(') continue;

            typename CCircularLinkedList<Traits>::value_type val;
            ref_type ref;

            if constexpr (std::is_same_v<typename CCircularLinkedList<Traits>::value_type, std::string>) {
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
        container = std::move(tmp);
    } catch (const exception& e) {
        is.setstate(ios::failbit);
    }
    return is;
}


#endif // __CIRCULARLINKEDLIST_H__
