//
// Created by aroon on 2/8/26.
//

#ifndef __CIRCULARLINKEDLIST_H__
#define __CIRCULARLINKEDLIST_H__

#include "linkedlist.h"


template <typename Traits>
class CircularLinkedList_Node : public LLBasicNode<Traits> {
    using value_type = typename Traits::value_type;
    using Node       = CircularLinkedList_Node<Traits>;
    Node *pNext = nullptr;

public:
    CircularLinkedList_Node() = default;
    CircularLinkedList_Node(value_type _value, ref_type _ref = -1)
        : LLBasicNode<Traits>(_value, _ref) {}
    CircularLinkedList_Node(value_type _value, ref_type _ref, Node *pNext)
        : LLBasicNode<Traits>(_value, _ref), pNext(pNext) {}

    Node       *GetNext() const { return pNext; }
    Node      *&GetNextRef() { return pNext; }

    Node &operator=(const Node &another) {
        this->m_data = another.GetValue();
        this->m_ref = another.GetRef();
        return *this;
    }
};


template <typename Container>
class CircularLinkedListForwardIterator
    : public LLBasicIterator<CircularLinkedListForwardIterator<Container>, Container> {
public:
    using Base = LLBasicIterator<CircularLinkedListForwardIterator<Container>, Container>;
    using Parent = typename Base::Parent;
    using Node = typename Base::Node;

    CircularLinkedListForwardIterator(Container *pContainer, Size pos=0)
        : LLBasicIterator<CircularLinkedListForwardIterator<Container>, Container>(pContainer, pos) {
        this->pCurrent = pContainer->m_pRoot;
        if (!this->pCurrent) return;
        for (Size i = 0; i < pos; ++i) this->pCurrent = this->pCurrent->GetNext();
    }
    CircularLinkedListForwardIterator(CircularLinkedListForwardIterator<Container> &another)
        : LLBasicIterator<CircularLinkedListForwardIterator<Container>, Container>(another) {
        this->pCurrent = another.pCurrent;
    }

    void advance() {
        if (this->pCurrent) {
            this->pCurrent = this->pCurrent->GetNext();
            ++this->m_pos;
        }
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
    friend forward_iterator;
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
    value_type firstThat(ObjFunc foo, Args... args) {
        lock_guard<mutex> lock(mtx);
        return *::FirstThat(*this, foo, args...);
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
    // bloquea el mutex
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

    // si esta en orden en algun lugar de la lista
    if (this->compare(rCurrentNode->GetValueRef(), val)) {
        Node *pNew = new Node(val, ref);
        // si esta en la raiz
        if (rCurrentNode == m_pRoot) {
            pNew->GetNextRef() = m_pRoot;
            m_pRoot = pNew;
            m_pLast->GetNextRef() = m_pRoot;
        } else {
            pNew->GetNextRef() = rCurrentNode;
            rCurrentNode = pNew;
        }
        ++m_nElements;
        return;
    }
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
        pNew->GetNextRef() = m_pRoot;
        m_pRoot = pNew;
        m_pLast->GetNextRef() = m_pRoot;
        ++m_nElements;
        return;
    }

    // si el indice no es 0
    Node *pTrav = m_pRoot;
    for (size_t i = 0; i + 1 < index; ++i) pTrav = pTrav->GetNext();
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
    using value_type = typename CCircularLinkedList<Traits>::value_type;
    // bloquear el mutex
    lock_guard<mutex> lock(container.mtx);

    os << "CCircularLinkedList: size = " << container.m_nElements << " [";
    auto trav = container.m_pRoot;

    for (size_t i = 0; i < container.m_nElements; ++i) {
        // si se trata de una lista de strings, se guardan con std::quoted
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


// lee del input stream esperando el formato en el que se escribe
template <typename Traits>
istream &operator>>(istream &is, CCircularLinkedList<Traits> &container) {
    using value_type = typename CCircularLinkedList<Traits>::value_type;
    if (!is) return is;

    CCircularLinkedList<Traits> tmp;
    try {
        // bloquea el mutex dentro del try/catch
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
    // hacer esto al final sin miedo de deadlocks
    container = std::move(tmp);
    return is;
}


#endif // __CIRCULARLINKEDLIST_H__
