#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <type_traits>
#include <iomanip>
#include <mutex>
#include <vector>
#include "../general/types.h"
#include "GeneralIterator.h"


using namespace std;

// TODO: Traits para listas enlazadas
template <typename T, typename _Func>
struct ListTrait{
    using value_type = T;
    using Func       = _Func;
    static constexpr bool ordered = true;
};

template <typename T>
struct AscendingTrait :
    ListTrait<T, std::greater<T> >{
};

template <typename T>
struct DescendingTrait :
    ListTrait<T, std::less<T> >{
};

template <typename T>
struct UnorderedTrait :
    ListTrait<T, std::less<T> >{
    static constexpr bool ordered = false;
};

// Iterators para listas enlazadas

template <typename Traits>
class LLBasicNode {
    using value_type = typename Traits::value_type;
public:
    value_type m_data;
    ref_type   m_ref;

    LLBasicNode() = default;
    LLBasicNode(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref) {}

    value_type  GetValue() const { return m_data; }
    value_type &GetValueRef() { return m_data; }

    ref_type    GetRef() const { return m_ref; }
    ref_type   &GetRefRef() { return m_ref; }
};


template <typename Derived, typename Container>
/*
 * Abstraccion de los iteradores
 * esta clase contiene lo que se repite en todos los iteradores
 * de las LinkedLists implementadas
 * template Derived: es el iterador derivado
 * container es el container
 */
class LLBasicIterator : public GeneralIterator<Container> {
public:
    using Parent = GeneralIterator<Container>;
    using value_type = typename Container::value_type;
    using Node = typename Container::Node;

    LLBasicIterator(Container *pContainer, Size pos=0)
        : GeneralIterator<Container>(pContainer, pos) {}

    value_type &operator*() override { return pCurrent->GetValueRef(); }
    Derived &operator++() {
        static_cast<Derived*>(this)->advance();
        return *static_cast<Derived*>(this);
    }
    Derived operator++(int) {
        Derived tmp(*static_cast<Derived*>(this));
        ++(*this);
        return tmp;
    }

protected:
    // no se deberia acceder al pCurrent desde afuera
    // pero si por las clases heredadas
    Node *pCurrent = nullptr;
};

template <typename Traits>
class NodeLinkedList : public LLBasicNode<Traits> {

    using  value_type  = typename Traits::value_type;
    using  Node        = NodeLinkedList<Traits>;
private:
    Node *m_pNext = nullptr;

public:
    NodeLinkedList() = default;
    NodeLinkedList(value_type _value, ref_type _ref = -1)
        : LLBasicNode<Traits>(_value, _ref) {}
    NodeLinkedList(value_type _value, ref_type _ref, Node *pNext)
        : LLBasicNode<Traits>(_value, _ref), m_pNext(pNext) {}

    Node      * GetNext     () const { return m_pNext; }
    Node      *&GetNextRef  () { return m_pNext; }

    Node &operator=(const Node &another) {
        this->m_data = another.GetValue();
        this->m_ref = another.GetRef();
        return *this;
    }
};

template <typename Container>
class LinkedListForwardIterator
    : public LLBasicIterator<LinkedListForwardIterator<Container>, Container> {
public:
    using Base = LLBasicIterator<LinkedListForwardIterator<Container>, Container>;
    using Parent = typename Base::Parent;
    using Node = typename Base::Node;

    LinkedListForwardIterator(Container *pContainer, Size pos=0)
        : LLBasicIterator<LinkedListForwardIterator<Container>, Container>(pContainer, pos)
    {
        this->pCurrent = pContainer->m_pRoot;
        for (Size i = 0; i < pos; ++i) this->pCurrent = this->pCurrent->GetNext();
    }
    LinkedListForwardIterator(LinkedListForwardIterator<Container> &another)
        : LLBasicIterator<LinkedListForwardIterator<Container>, Container>(another)
    {
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
class CLinkedList;

template <typename Traits>
ostream &operator<<(ostream &os, CLinkedList<Traits> &container);

template <typename Traits>
istream &operator>>(istream &is, CLinkedList<Traits> &container);

template <typename Traits>
class ListBase {
public:
    using value_type = typename Traits::value_type;
    using Node = NodeLinkedList<Traits>;

protected:
    bool compare(const value_type &a, const value_type &b) const {
        if constexpr (Traits::ordered) {
            typename Traits::Func compareFunc;
            return compareFunc(a, b);
        }
        return false;
    }
};

template <typename Traits>
class CLinkedList : public ListBase<Traits> {
    // one mutex per instance
    mutable mutex mtx;
public:
    using value_type       = typename Traits::value_type;
    using forward_iterator = LinkedListForwardIterator<CLinkedList<Traits>>;

    friend forward_iterator;
    friend GeneralIterator<CLinkedList<Traits>>;
    using Node = NodeLinkedList<Traits>;

    Node *m_pRoot = nullptr;
    Node *m_pLast = nullptr;
    size_t m_nElements = 0;
    CLinkedList(){}
    // TODO: Constructor copia
    CLinkedList(const CLinkedList &to_copy);
    // TODO: Move Constructor
    CLinkedList(CLinkedList &&to_move);


    // TODO: Destructor seguro y virtual
    virtual ~CLinkedList();
    // TODO: Concurrencia (mutex)
    // TODO: Iterators begin() end()
    forward_iterator begin();
    forward_iterator end();

    // TODO: Operadores de acceso [] done
    value_type &operator[](size_t index);

    CLinkedList &operator=(const CLinkedList &to_copy);

    void push_back(const value_type &val, ref_type ref);

    void Insert(const value_type &val, ref_type ref, size_t index = static_cast<size_t>(-1));
    size_t getSize();

    // TODO: forEach y firstThat

    // forEach
    // invoca a la variante de iteradores para ahorrar una call
    template <typename ObjFunc, typename ...Args>
    void forEach(ObjFunc foo, Args... args) {
        // esta operacion bloquea el mutex
        lock_guard<mutex> lock(mtx);
        ::Foreach(begin(), end(), foo, args...);
    }

    // firstThat
    template <typename ObjFunc, typename ...Args>
    value_type firstThat(ObjFunc foo, Args... args) {
        lock_guard<mutex> lock(mtx);
        return *::FirstThat(begin(), end(), foo, args...);
    }

    // borra nodos bajo lock para evitar carreras
    void clear() {
        lock_guard<mutex> lock(mtx);
        clear_unlocked();
    }

private:
    void InternalInsert(Node *&rCurrentNode, const value_type &val, ref_type ref);
    void InsertAtIndex(const value_type &val, ref_type ref, size_t index);

    // requiere que el caller tenga el lock
    void clear_unlocked() {
        auto trav = m_pRoot;
        while (trav) {
            auto temp = trav->GetNext();
            delete trav;
            trav = temp;
        }
        m_pRoot = m_pLast = nullptr;
        m_nElements = 0;
    }

    // TODO: Persistencia (write)
    friend ostream &operator<< <>(ostream &os, CLinkedList<Traits> &container);
    // TODO: Persistencia (read)
    // lee el mismo formato en que se escribe
    friend istream &operator>> <>(istream &is, CLinkedList<Traits> &container);

    /*
     * Helper function
     * copia los nodos de otra linked list
     */
    void _copyNodesFrom(const CLinkedList &to_copy) {
        // si se asigna a si mismo se regresa a si mismo
        if (this == &to_copy) return;

        // limpia sin lock (caller debe bloquear)
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

// ===================
// === CLinkedList ===
// ===================

template <typename Traits>
CLinkedList<Traits>::CLinkedList(const CLinkedList &to_copy): m_pRoot(nullptr), m_pLast(nullptr) {
    // bloquea solo al objeto fuente para copiar un estado consistente
    lock_guard<mutex> lock(to_copy.mtx);
    // usa la helper function implementada
    _copyNodesFrom(to_copy);
}

template <typename Traits>
CLinkedList<Traits>::CLinkedList(CLinkedList &&to_move)
: m_pRoot(to_move.m_pRoot), m_pLast(to_move.m_pLast), m_nElements(to_move.m_nElements) {
    // bloquea solo al objeto fuente antes de mover
    lock_guard<mutex> lock(to_move.mtx);
    // solo mueve los punteros y el numero de elementos
    // reinicia los de la lista a mover
    to_move.m_pRoot     = nullptr;
    to_move.m_pLast     = nullptr;
    to_move.m_nElements = 0;
}

// destructor implementado
template <typename Traits>
CLinkedList<Traits>::~CLinkedList() {
    // no se bloquea en el destructor: asumir sincronizacion externa
    clear_unlocked();
}

/*
 * los iteradores no bloquean la LinkedList
 * los iteradores son inseguros a cambio de simplicidad
 * se toma como inspiracion el ConcurrentMap de java
 */

template <typename Traits>
typename CLinkedList<Traits>::forward_iterator CLinkedList<Traits>::begin() {
    return forward_iterator(this);
}

template <typename Traits>
typename CLinkedList<Traits>::forward_iterator CLinkedList<Traits>::end() {
    return forward_iterator(this, m_nElements);
}

template <typename Traits>
size_t CLinkedList<Traits>::getSize() {
    // bloquear por si acaso
    lock_guard<mutex> lock(mtx);
    return m_nElements;
}

template <typename Traits>
void CLinkedList<Traits>::push_back(const value_type &val, ref_type ref) {
    // aqui definitivamente se bloquea
    lock_guard<mutex> lock(mtx);

    // si el valor a añadir no sigue el orden (ascendente/descendente)
    // si es ascendente:  val > m_pLast->GetValue()
    // si es descendente: val < m_pLast->GetValue()
    // si invertimos los operandos y es verdadero, no está en orden
    // el if constexpr se aplica para la clase: para las CLinkedList con
    // UnorderedTrait, esta condicional no existe
    if constexpr (Traits::ordered) {
        if ( m_pLast && this->compare(m_pLast->GetValueRef(), val) ) {
            InternalInsert(m_pRoot, val, ref);
            return;
        }
    }

    // si no tiene la flag ordered, se prosigue con el push_back normal
    Node *pNewNode = new Node(val, ref);
    if ( !m_pRoot ) m_pRoot = m_pLast = pNewNode;
    else {
        // que el ultimo nodo actual apunte al nuevo nodo
        m_pLast->GetNextRef() = pNewNode;
        // que el puntero de la lista apunte al ultimo nodo
        m_pLast = pNewNode;
    }
    ++m_nElements;
}


template <typename Traits>
void CLinkedList<Traits>::InternalInsert(
    Node *&rCurrentNode, const value_type &val, ref_type ref
    ) {
    // TODO: Agregar algo para el caso de circular
    // crea un nuevo nodo
    Node *pNew = new Node(val, ref);

    // caso ultimo nodo
    if ( !rCurrentNode ) {
        rCurrentNode = pNew;
        m_pLast = pNew;  // no olvidarse de reasignar m_pLast
        ++m_nElements;
        return;
    }
    // caso base, el valor debe insertarse antes del nodo actual
    if ( this->compare(rCurrentNode->GetValueRef(), val ) ) {
        pNew->GetNextRef() = rCurrentNode;
        rCurrentNode = pNew;
        ++m_nElements;
        return;
    }

    InternalInsert(rCurrentNode->GetNextRef(), val, ref);
}

template <typename Traits>
void CLinkedList<Traits>::Insert(const value_type &val, ref_type ref, size_t index) {
    // se utiliza getSize para verificar si la lista esta vacia
    // porque es una operacion cubierta con un lock_guard
    if (!getSize()) {
        push_back(val, ref);
        return;
    }

    // el lock del getSize() ya se deshizo y se bloquea denuevo al insertar
    lock_guard<mutex> lock(mtx);

    // si es una lista no ordenada, se pasa al else (se usa el indice)
    // si es una lista ordenada, se procede con la logica regular
    if constexpr (Traits::ordered) InternalInsert(m_pRoot, val, ref);
    else {
        // si algun chistoso pone de indice -1
        // se le perdonara y añadira el item al final
        if (index == static_cast<size_t>(-1)) {
            index = m_nElements;
        }
        InsertAtIndex(val, ref, index);
    }
}

/*
 * metodo privado creada para manejar el caso en que se utilice una lista no ordenada
 * (UnorderedTrait)
 */
template <typename Traits>
void CLinkedList<Traits>::InsertAtIndex(const value_type &val, ref_type ref, size_t index) {
    if (index > m_nElements) throw std::out_of_range("Index out of range");

    // crear nodo, no hay que manejar el caso de lista vacia
    // en Insert ya se maneja llamando a push_back
    Node *pNew = new Node(val, ref);

    // manejar el caso de que index = 0
    if (!index) {
        pNew->GetNextRef() = m_pRoot;
        m_pRoot = pNew;
        ++m_nElements;
        return;
    }
    /*
     * como se tiene que conectar:
     * el anterior a pNew
     * pNew al siguiente
     * trav termina en el anterior al indice de pNew y se conecta
     * pNew->GetNextRef() a trav->GetNext()
     * y trav->GetNextRef() a pNew
     */
    Node *trav = m_pRoot;
    for (size_t i = 0; i + 1 < index; ++i) {
        trav = trav->GetNext();
    }
    pNew->GetNextRef() = trav->GetNext();
    trav->GetNextRef() = pNew;
    if (trav == m_pLast) m_pLast = pNew;
    ++m_nElements;
}

// implementado operador []
template <typename Traits>
typename CLinkedList<Traits>::value_type &
CLinkedList<Traits>::operator[](const size_t index) {
    // aqui tambien se bloquea
    lock_guard<mutex> lock(mtx);

    if (index >= m_nElements) {
        throw std::out_of_range("Index out of range");
    }
    Node *trav = m_pRoot;
    for (size_t i = 0; i < index; ++i)
        trav = trav->GetNext();
    // WARNING: se retorna una referencia que puede invalidarse con modificaciones concurrentes
    return trav->GetValueRef();
}

// implementacion operador =
template <typename Traits>
CLinkedList<Traits>& CLinkedList<Traits>::operator=(const CLinkedList &to_copy) {
    if (this == &to_copy) return *this;

    // se toma una snapshot para evitar deadlocks
    // (llamada a este operador en la implementacion del operador >>)
    std::vector<std::pair<value_type, ref_type>> items;
    {
        lock_guard<mutex> lock(to_copy.mtx);
        for (auto trav = to_copy.m_pRoot; trav; trav = trav->GetNext()) {
            items.emplace_back(trav->GetValue(), trav->GetRef());
        }
    }
    // ahora se limpia el contenido bloqueando el mutex
    {
        lock_guard<mutex> lock(mtx);
        clear_unlocked();
    }
    // se reconstruye la linked list usando push_back
    for (const auto &item : items) {
        push_back(item.first, item.second);
    }
    return *this;
}

// operador right shift
template <typename Traits>
ostream &operator<<(ostream &os, CLinkedList<Traits> &container) {
    using value_type = typename CLinkedList<Traits>::value_type;

    lock_guard<mutex> lock(container.mtx);
    os << "CLinkedList: size = " << container.m_nElements << " [";
    // cambio: variable del loop es un traveler del que se extrae el value y el Ref
    for (auto trav = container.m_pRoot; trav; trav = trav->GetNext()) {
        // si es una lista de strings: se pasan con std::quoted
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
// lee del input stream esperando el formato en el que se escribe
template <typename Traits>
istream &operator>>(istream &is, CLinkedList<Traits> &container) {
    using value_type = typename CLinkedList<Traits>::value_type;
    // verificar el buen estado del stream
    if (!is) return is;

    // crea un contenedor temporal
    CLinkedList<Traits> tmp;
    try {
        // este lock_guard deberia desaparecer fuera del try
        lock_guard<mutex> lock(container.mtx);
        // ignorar texto hasta el primer '['
        string bar;
        getline(is, bar, '[');
        // leer elementos continuamente
        char ch;
        while (is.get(ch) && ch != ']') {
            if (ch != '(') continue;

            value_type val;
            ref_type ref;
            // si se leeran strings, se leen con std::quoted
            if constexpr (std::is_same_v<value_type, std::string>) {
                // leer string con comillas y escapes
                is >> std::quoted(val);
                getline(is, bar, ':');
            } else {
                is >> val;  // leer el valor
                getline(is, bar, ':');
            }
            is >> ref;  // leer la ref
            getline(is, bar, ')');

            tmp.push_back(val, ref);
        }
    } catch (const exception& e) {
        // si algo paso, setea el estado del stream en failbit
        // (hubo fallo al leer el contenido)
        is.setstate(ios::failbit);
    }
    // si la lectura salio bien, intercambia los contenidos sin deadlock
    container = std::move(tmp);
    return is;
}

#endif // __LINKEDLIST_H__
