#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include <mutex>
#include "../general/types.h"
#include "../util.h"
#include "../foreach.h"
using namespace std;

// Traits
template <typename T, typename _Func>
struct ListTrait {
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingTrait : public ListTrait<T, std::greater<T>> {};

template <typename T>
struct DescendingTrait : public ListTrait<T, std::less<T>> {};

// Forward Iterator
template <typename Container>
class LinkedListForwardIterator {
public:
    using Node = typename Container::Node;
    using value_type = typename Container::value_type;

private:
    Node* m_ptr;

public:
    LinkedListForwardIterator(Node* p = nullptr) : m_ptr(p) {}

    bool operator!=(const LinkedListForwardIterator& other) const {
        return m_ptr != other.m_ptr;
    }

    bool operator==(const LinkedListForwardIterator& other) const {
        return m_ptr == other.m_ptr;
    }

    value_type& operator*() {
        return m_ptr->GetValueRef();
    }

    LinkedListForwardIterator& operator++() {
        if (m_ptr) m_ptr = m_ptr->GetNext();
        return *this;
    }

    LinkedListForwardIterator operator++(int) {
        LinkedListForwardIterator tmp = *this;
        ++(*this);
        return tmp;
    }
};

// Node
template <typename Traits>
class NodeLinkedList {
public:
    using value_type = typename Traits::value_type;
    using Node       = NodeLinkedList<Traits>;

private:
    value_type m_data;
    ref_type   m_ref;
    Node*      m_pNext = nullptr;

public:
    NodeLinkedList() {}
    NodeLinkedList(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref) {}

    value_type  GetValue()    const { return m_data; }
    value_type& GetValueRef()       { return m_data; }

    ref_type    GetRef()      const { return m_ref; }
    ref_type&   GetRefRef()         { return m_ref; }

    Node*       GetNext()     const { return m_pNext; }
    Node*&      GetNextRef()        { return m_pNext; }

    Node& operator=(const Node& other) {
        m_data = other.GetValue();
        m_ref  = other.GetRef();
        return *this;
    }

    bool operator==(const Node& other) const {
        return m_data == other.GetValue();
    }

    bool operator<(const Node& other) const {
        return m_data < other.GetValue();
    }
};

// Lista Enlazada (LE) y Lista Enlazada Circular (LEC)
template <typename Traits>
class CLinkedList {
public:
    using value_type       = typename Traits::value_type;
    using Node             = NodeLinkedList<Traits>;
    using forward_iterator = LinkedListForwardIterator<CLinkedList<Traits>>;

    friend forward_iterator;

private:
    Node*   m_pRoot     = nullptr;
    Node*   m_pLast     = nullptr;
    size_t  m_nElements = 0;
    bool    m_isCircular = false;
    mutable std::mutex m_mutex; // Concurrencia

public:
    // Constructor
    CLinkedList(bool circular = false) : m_isCircular(circular) {}

    // Constructor copia
    CLinkedList(const CLinkedList& other) : m_isCircular(other.m_isCircular) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        Node* curr = other.m_pRoot;
        size_t count = 0;
        
        while (curr && count < other.m_nElements) {
            value_type v = curr->GetValue();
            push_back(v, curr->GetRef());
            curr = curr->GetNext();
            count++;
        }
    }

    // Move Constructor
    CLinkedList(CLinkedList&& other) noexcept
        : m_pRoot(other.m_pRoot),
          m_pLast(other.m_pLast),
          m_nElements(other.m_nElements),
          m_isCircular(other.m_isCircular) {
        other.m_pRoot = nullptr;
        other.m_pLast = nullptr;
        other.m_nElements = 0;
    }

    // Destructor seguro y virtual
    virtual ~CLinkedList() {
        clear();
    }

    CLinkedList& operator=(const CLinkedList& other) {
        if (this != &other) {
            std::lock(m_mutex, other.m_mutex);
            std::lock_guard<std::mutex> lock1(m_mutex, std::adopt_lock);
            std::lock_guard<std::mutex> lock2(other.m_mutex, std::adopt_lock);
            
            clear();
            m_isCircular = other.m_isCircular;
            
            Node* curr = other.m_pRoot;
            size_t count = 0;
            
            while (curr && count < other.m_nElements) {
                value_type v = curr->GetValue();
                push_back(v, curr->GetRef());
                curr = curr->GetNext();
                count++;
            }
        }
        return *this;
    }

    CLinkedList& operator=(CLinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            m_pRoot = other.m_pRoot;
            m_pLast = other.m_pLast;
            m_nElements = other.m_nElements;
            m_isCircular = other.m_isCircular;
            
            other.m_pRoot = nullptr;
            other.m_pLast = nullptr;
            other.m_nElements = 0;
        }
        return *this;
    }

    // Operator []
    value_type& operator[](size_t index) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (index >= m_nElements) {
            throw std::out_of_range("Index out of range");
        }

        Node* curr = m_pRoot;
        for (size_t i = 0; i < index; ++i) {
            curr = curr->GetNext();
        }
        
        return curr->GetValueRef();
    }

    const value_type& operator[](size_t index) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (index >= m_nElements) {
            throw std::out_of_range("Index out of range");
        }

        Node* curr = m_pRoot;
        for (size_t i = 0; i < index; ++i) {
            curr = curr->GetNext();
        }
        
        return curr->GetValue();
    }

    // begin() / end()
    forward_iterator begin() {
        return forward_iterator(m_pRoot);
    }

    forward_iterator end() {
        return forward_iterator(nullptr);
    }

    void push_back(const value_type& val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        Node* pNew = new Node(val, ref);
        
        if (!m_pRoot) {
            m_pRoot = pNew;
            m_pLast = pNew;
        } else {
            m_pLast->GetNextRef() = pNew;
            m_pLast = pNew;
        }
        
        if (m_isCircular) {
            m_pLast->GetNextRef() = m_pRoot;
        }
        
        ++m_nElements;
    }

    void Insert(const value_type& val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalInsert(m_pRoot, val, ref);
    }

    void clear() {
        if (m_isCircular && m_pLast) {
            m_pLast->GetNextRef() = nullptr;
        }

        Node* curr = m_pRoot;
        while (curr) {
            Node* next = curr->GetNext();
            delete curr;
            curr = next;
        }

        m_pRoot = nullptr;
        m_pLast = nullptr;
        m_nElements = 0;
    }

    size_t getSize() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_nElements;
    }

    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_nElements == 0;
    }

    bool isCircular() const {
        return m_isCircular;
    }

    void makeCircular() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_isCircular && m_pLast) {
            m_pLast->GetNextRef() = m_pRoot;
            m_isCircular = true;
        }
    }

    void breakCircular() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_isCircular && m_pLast) {
            m_pLast->GetNextRef() = nullptr;
            m_isCircular = false;
        }
    }

    // Foreach
    template <typename ObjFunc, typename... Args>
    void Foreach(ObjFunc fn, Args... args) {
        ::Foreach(begin(), end(), fn, args...);
    }

    // FirstThat
    template <typename ObjFunc, typename... Args>
    forward_iterator FirstThat(ObjFunc fn, Args... args) {
        return ::FirstThat(begin(), end(), fn, args...);
    }

    // Operator <<
    friend ostream& operator<<(ostream& os, CLinkedList<Traits>& container) {
        std::lock_guard<std::mutex> lock(container.m_mutex);
        
        os << "CLinkedList: size = " << container.m_nElements 
           << (container.m_isCircular ? " (Circular)" : "") << endl;
        os << "[";
        
        if (container.m_pRoot) {
            Node* curr = container.m_pRoot;
            size_t count = 0;
            
            while (curr && count < container.m_nElements) {
                os << "(" << curr->GetValue() << ":" << curr->GetRef() << ")";
                curr = curr->GetNext();
                count++;
                
                if (count < container.m_nElements) {
                    os << ",";
                }
            }
        }
        
        os << "]" << endl;
        return os;
    }

    // Operator >>
    friend istream& operator>>(istream& is, CLinkedList<Traits>& container) {
        std::lock_guard<std::mutex> lock(container.m_mutex);
        
        container.clear();
        
        size_t n;
        is >> n;

        for (size_t i = 0; i < n; ++i) {
            value_type val;
            ref_type ref;
            is >> val >> ref;
            container.push_back(val, ref);
        }

        return is;
    }

private:
    void InternalInsert(Node*& rParent, const value_type& val, ref_type ref) {
        if (!rParent || rParent->GetValue() > val) {
            Node* pNew = new Node(val, ref);
            pNew->GetNextRef() = rParent;
            rParent = pNew;
            
            if (!m_pLast || pNew->GetNext() == nullptr) {
                m_pLast = pNew;
            }
            
            if (m_isCircular && m_pLast) {
                m_pLast->GetNextRef() = m_pRoot;
            }
            
            ++m_nElements;
            return;
        }
        
        InternalInsert(rParent->GetNextRef(), val, ref);
    }
};

template <typename Traits, typename ObjFunc, typename... Args>
void Foreach(CLinkedList<Traits>& container, ObjFunc fn, Args... args) {
    container.Foreach(fn, args...);
}

template <typename Traits, typename ObjFunc, typename... Args>
auto FirstThat(CLinkedList<Traits>& container, ObjFunc fn, Args... args) {
    return container.FirstThat(fn, args...);
}

#endif // __LINKEDLIST_H__
