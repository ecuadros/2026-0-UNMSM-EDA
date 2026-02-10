#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__
#include <iostream>
#include <mutex>
#include "../general/types.h"
#include "../util.h"
#include "../foreach.h"
using namespace std;

// Traits
template <typename T, typename _Func>
struct DoubleListTrait {
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct DAscendingTrait : public DoubleListTrait<T, std::greater<T>> {};

template <typename T>
struct DDescendingTrait : public DoubleListTrait<T, std::less<T>> {};

// Forward Iterator
template <typename Container>
class DLForwardIterator {
public:
    using Node = typename Container::Node;
    using value_type = typename Container::value_type;

private:
    Node* m_ptr;

public:
    DLForwardIterator(Node* p = nullptr) : m_ptr(p) {}

    bool operator!=(const DLForwardIterator& other) const {
        return m_ptr != other.m_ptr;
    }

    bool operator==(const DLForwardIterator& other) const {
        return m_ptr == other.m_ptr;
    }

    value_type& operator*() {
        return m_ptr->GetValueRef();
    }

    DLForwardIterator& operator++() {
        if (m_ptr) m_ptr = m_ptr->GetNext();
        return *this;
    }

    DLForwardIterator operator++(int) {
        DLForwardIterator tmp = *this;
        ++(*this);
        return tmp;
    }
};

// Backward Iterator
template <typename Container>
class DLBackwardIterator {
public:
    using Node = typename Container::Node;
    using value_type = typename Container::value_type;

private:
    Node* m_ptr;

public:
    DLBackwardIterator(Node* p = nullptr) : m_ptr(p) {}

    bool operator!=(const DLBackwardIterator& other) const {
        return m_ptr != other.m_ptr;
    }

    bool operator==(const DLBackwardIterator& other) const {
        return m_ptr == other.m_ptr;
    }

    value_type& operator*() {
        return m_ptr->GetValueRef();
    }

    DLBackwardIterator& operator++() {
        if (m_ptr) m_ptr = m_ptr->GetPrev();
        return *this;
    }

    DLBackwardIterator operator++(int) {
        DLBackwardIterator tmp = *this;
        ++(*this);
        return tmp;
    }
};

// Node
template <typename Traits>
class NodeDoubleLinkedList {
public:
    using value_type = typename Traits::value_type;
    using Node       = NodeDoubleLinkedList<Traits>;

private:
    value_type m_data;
    ref_type   m_ref;
    Node*      m_pNext = nullptr;
    Node*      m_pPrev = nullptr;

public:
    NodeDoubleLinkedList() {}
    NodeDoubleLinkedList(value_type _value, ref_type _ref = -1)
        : m_data(_value), m_ref(_ref) {}

    value_type  GetValue()    const { return m_data; }
    value_type& GetValueRef()       { return m_data; }

    ref_type    GetRef()      const { return m_ref; }
    ref_type&   GetRefRef()         { return m_ref; }

    Node*       GetNext()     const { return m_pNext; }
    Node*&      GetNextRef()        { return m_pNext; }

    Node*       GetPrev()     const { return m_pPrev; }
    Node*&      GetPrevRef()        { return m_pPrev; }

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

// Lista Doblemente Enlazada (LDE) y Lista Doblemente Enlazada Circular (LDEC)
template <typename Traits>
class CDoubleLinkedList {
public:
    using value_type        = typename Traits::value_type;
    using Node              = NodeDoubleLinkedList<Traits>;
    using forward_iterator  = DLForwardIterator<CDoubleLinkedList<Traits>>;
    using backward_iterator = DLBackwardIterator<CDoubleLinkedList<Traits>>;

    friend forward_iterator;
    friend backward_iterator;

private:
    Node*   m_pRoot     = nullptr;
    Node*   m_pLast     = nullptr;
    size_t  m_nElements = 0;
    bool    m_isCircular = false;
    mutable std::mutex m_mutex; // Concurrencia

public:
    // Constructor
    CDoubleLinkedList(bool circular = false) : m_isCircular(circular) {}

    // Constructor copia
    CDoubleLinkedList(const CDoubleLinkedList& other) : m_isCircular(other.m_isCircular) {
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
    CDoubleLinkedList(CDoubleLinkedList&& other) noexcept
        : m_pRoot(other.m_pRoot),
          m_pLast(other.m_pLast),
          m_nElements(other.m_nElements),
          m_isCircular(other.m_isCircular) {
        other.m_pRoot = nullptr;
        other.m_pLast = nullptr;
        other.m_nElements = 0;
    }

    // Destructor seguro y virtual
    virtual ~CDoubleLinkedList() {
        clear();
    }

    CDoubleLinkedList& operator=(const CDoubleLinkedList& other) {
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

    CDoubleLinkedList& operator=(CDoubleLinkedList&& other) noexcept {
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

        Node* curr;
        if (index < m_nElements / 2) {
            curr = m_pRoot;
            for (size_t i = 0; i < index; ++i) {
                curr = curr->GetNext();
            }
        } else {
            curr = m_pLast;
            for (size_t i = m_nElements - 1; i > index; --i) {
                curr = curr->GetPrev();
            }
        }
        
        return curr->GetValueRef();
    }

    const value_type& operator[](size_t index) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (index >= m_nElements) {
            throw std::out_of_range("Index out of range");
        }

        Node* curr;
        if (index < m_nElements / 2) {
            curr = m_pRoot;
            for (size_t i = 0; i < index; ++i) {
                curr = curr->GetNext();
            }
        } else {
            curr = m_pLast;
            for (size_t i = m_nElements - 1; i > index; --i) {
                curr = curr->GetPrev();
            }
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

    // rbegin() / rend()
    backward_iterator rbegin() {
        return backward_iterator(m_pLast);
    }

    backward_iterator rend() {
        return backward_iterator(nullptr);
    }

    void push_back(const value_type& val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        Node* pNew = new Node(val, ref);
        
        if (!m_pRoot) {
            m_pRoot = pNew;
            m_pLast = pNew;
        } else {
            m_pLast->GetNextRef() = pNew;
            pNew->GetPrevRef() = m_pLast;
            m_pLast = pNew;
        }
        
        if (m_isCircular) {
            m_pLast->GetNextRef() = m_pRoot;
            m_pRoot->GetPrevRef() = m_pLast;
        }
        
        ++m_nElements;
    }

    void push_front(const value_type& val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        Node* pNew = new Node(val, ref);
        
        if (!m_pRoot) {
            m_pRoot = pNew;
            m_pLast = pNew;
        } else {
            pNew->GetNextRef() = m_pRoot;
            m_pRoot->GetPrevRef() = pNew;
            m_pRoot = pNew;
        }
        
        if (m_isCircular) {
            m_pLast->GetNextRef() = m_pRoot;
            m_pRoot->GetPrevRef() = m_pLast;
        }
        
        ++m_nElements;
    }

    void Insert(const value_type& val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalInsert(m_pRoot, val, ref);
    }

    void clear() {
        if (m_isCircular && m_pLast && m_pRoot) {
            m_pLast->GetNextRef() = nullptr;
            m_pRoot->GetPrevRef() = nullptr;
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
        if (!m_isCircular && m_pLast && m_pRoot) {
            m_pLast->GetNextRef() = m_pRoot;
            m_pRoot->GetPrevRef() = m_pLast;
            m_isCircular = true;
        }
    }

    void breakCircular() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_isCircular && m_pLast && m_pRoot) {
            m_pLast->GetNextRef() = nullptr;
            m_pRoot->GetPrevRef() = nullptr;
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
    friend ostream& operator<<(ostream& os, CDoubleLinkedList<Traits>& container) {
        std::lock_guard<std::mutex> lock(container.m_mutex);
        
        os << "CDoubleLinkedList: size = " << container.m_nElements 
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
    friend istream& operator>>(istream& is, CDoubleLinkedList<Traits>& container) {
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
            
            if (rParent) {
                pNew->GetPrevRef() = rParent->GetPrev();
                rParent->GetPrevRef() = pNew;
                
                if (pNew->GetPrev()) {
                    pNew->GetPrev()->GetNextRef() = pNew;
                }
            }
            
            rParent = pNew;
            
            if (!m_pRoot || pNew->GetPrev() == nullptr) {
                m_pRoot = pNew;
            }
            
            if (!m_pLast || pNew->GetNext() == nullptr) {
                m_pLast = pNew;
            }
            
            if (m_isCircular && m_pLast && m_pRoot) {
                m_pLast->GetNextRef() = m_pRoot;
                m_pRoot->GetPrevRef() = m_pLast;
            }
            
            ++m_nElements;
            return;
        }
        
        InternalInsert(rParent->GetNextRef(), val, ref);
    }
};

template <typename Traits, typename ObjFunc, typename... Args>
void Foreach(CDoubleLinkedList<Traits>& container, ObjFunc fn, Args... args) {
    container.Foreach(fn, args...);
}

template <typename Traits, typename ObjFunc, typename... Args>
auto FirstThat(CDoubleLinkedList<Traits>& container, ObjFunc fn, Args... args) {
    return container.FirstThat(fn, args...);
}

#endif // __DOUBLE_LINKED_LIST_H__
