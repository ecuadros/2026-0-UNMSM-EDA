#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include <iostream>
#include <mutex>
#include <memory>
#include "general/types.h"
#include "util.h"
#include "GeneralIterator.h"

using namespace std;

template <typename T, typename _Func>
struct ListTrait {
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct AscendingTrait : public ListTrait<T, std::greater<T>> {};

template <typename T>
struct DescendingTrait : public ListTrait<T, std::less<T>> {};

template <typename Traits>
class NodeLinkedList {
    using value_type = typename Traits::value_type;
    using Node       = NodeLinkedList<Traits>;

private:
    value_type m_data;
    ref_type   m_ref;
    Node      *m_pNext = nullptr;

public:
    NodeLinkedList() {}
    NodeLinkedList(value_type _value, ref_type _ref = -1, Node *pNext = nullptr)
        : m_data(_value), m_ref(_ref), m_pNext(pNext) {}

    value_type  GetValue()    const { return m_data; }
    value_type& GetValueRef()       { return m_data; }

    ref_type    GetRef()      const { return m_ref; }
    ref_type&   GetRefRef()         { return m_ref; }

    Node*       GetNext()     const { return m_pNext; }
    Node*&      GetNextRef()        { return m_pNext; }

    Node& operator=(const Node& another) {
        m_data  = another.GetValue();
        m_ref   = another.GetRef();
        m_pNext = another.GetNext();
        return *this;
    }

    bool operator==(const Node& another) const {
        return m_data == another.GetValue();
    }

    bool operator<(const Node& another) const {
        return m_data < another.GetValue();
    }
};

template <typename Container>
class LinkedListForwardIterator {
public:
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

private:
    Node* m_pCurrent;
    Node* m_pRoot;  // Para detectar ciclos en listas circulares
    bool  m_isCircular;
    bool  m_hasLooped;

public:
    LinkedListForwardIterator(Node* pNode, Node* pRoot = nullptr, bool isCircular = false)
        : m_pCurrent(pNode), m_pRoot(pRoot), m_isCircular(isCircular), m_hasLooped(false) {}

    bool operator!=(const LinkedListForwardIterator& other) const {
        if (m_isCircular) {
            return !(m_hasLooped && m_pCurrent == other.m_pCurrent);
        }
        return m_pCurrent != other.m_pCurrent;
    }

    bool operator==(const LinkedListForwardIterator& other) const {
        return !(*this != other);
    }

    value_type& operator*() {
        return m_pCurrent->GetValueRef();
    }

    LinkedListForwardIterator& operator++() {
        if (m_pCurrent) {
            m_pCurrent = m_pCurrent->GetNext();
            if (m_isCircular && m_pCurrent == m_pRoot) {
                m_hasLooped = true;
            }
        }
        return *this;
    }

    LinkedListForwardIterator operator++(int) {
        LinkedListForwardIterator temp = *this;
        ++(*this);
        return temp;
    }
};

template <typename Traits>
class CLinkedList {
public:
    using value_type        = typename Traits::value_type;
    using Node              = NodeLinkedList<Traits>;
    using forward_iterator  = LinkedListForwardIterator<CLinkedList<Traits>>;

    friend forward_iterator;

private:
    Node*       m_pRoot;
    Node*       m_pLast;
    size_t      m_nElements;
    bool        m_isCircular;
    mutable std::mutex m_mutex;

public:
    CLinkedList(bool circular = false)
        : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0), m_isCircular(circular) {}

    CLinkedList(const CLinkedList& other)
        : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0), m_isCircular(other.m_isCircular) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        
        if (!other.m_pRoot) return;

        Node* pCurrent = other.m_pRoot;
        size_t count = 0;
        
        do {
            push_back(pCurrent->GetValue(), pCurrent->GetRef());
            pCurrent = pCurrent->GetNext();
            count++;
            
            if (other.m_isCircular && count >= other.m_nElements) break;
            
        } while (pCurrent && pCurrent != other.m_pRoot);
    }

    CLinkedList(CLinkedList&& other) noexcept
        : m_pRoot(other.m_pRoot),
          m_pLast(other.m_pLast),
          m_nElements(other.m_nElements),
          m_isCircular(other.m_isCircular) {
        
        other.m_pRoot = nullptr;
        other.m_pLast = nullptr;
        other.m_nElements = 0;
    }

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
            
            Node* pCurrent = other.m_pRoot;
            size_t count = 0;
            
            if (pCurrent) {
                do {
                    push_back(pCurrent->GetValue(), pCurrent->GetRef());
                    pCurrent = pCurrent->GetNext();
                    count++;
                    
                    if (other.m_isCircular && count >= other.m_nElements) break;
                    
                } while (pCurrent && pCurrent != other.m_pRoot);
            }
        }
        return *this;
    }

    CLinkedList& operator=(CLinkedList&& other) noexcept {
        if (this != &other) {
            std::lock_guard<std::mutex> lock(m_mutex);
            
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

    value_type& operator[](size_t index) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (index >= m_nElements) {
            throw std::out_of_range("Index out of range");
        }

        Node* pCurrent = m_pRoot;
        for (size_t i = 0; i < index; ++i) {
            pCurrent = pCurrent->GetNext();
        }
        
        return pCurrent->GetValueRef();
    }

    const value_type& operator[](size_t index) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (index >= m_nElements) {
            throw std::out_of_range("Index out of range");
        }

        Node* pCurrent = m_pRoot;
        for (size_t i = 0; i < index; ++i) {
            pCurrent = pCurrent->GetNext();
        }
        
        return pCurrent->GetValue();
    }

    forward_iterator begin() {
        return forward_iterator(m_pRoot, m_pRoot, m_isCircular);
    }

    forward_iterator end() {
        if (m_isCircular) {
            return forward_iterator(m_pRoot, m_pRoot, m_isCircular);
        }
        return forward_iterator(nullptr, m_pRoot, m_isCircular);
    }

    void push_back(const value_type& val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        Node* pNewNode = new Node(val, ref);
        
        if (!m_pRoot) {
            m_pRoot = pNewNode;
            m_pLast = pNewNode;
        } else {
            m_pLast->GetNextRef() = pNewNode;
            m_pLast = pNewNode;
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
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pRoot) return;

        if (m_isCircular && m_pLast) {
            m_pLast->GetNextRef() = nullptr;
        }

        Node* pCurrent = m_pRoot;
        while (pCurrent) {
            Node* pNext = pCurrent->GetNext();
            delete pCurrent;
            pCurrent = pNext;
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

    template <typename ObjFunc, typename... Args>
    void Foreach(ObjFunc of, Args... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pRoot) return;

        Node* pCurrent = m_pRoot;
        size_t count = 0;
        
        do {
            of(pCurrent->GetValueRef(), args...);
            pCurrent = pCurrent->GetNext();
            count++;
            
            if (m_isCircular && count >= m_nElements) break;
            
        } while (pCurrent && pCurrent != m_pRoot);
    }

    template <typename ObjFunc, typename... Args>
    forward_iterator FirstThat(ObjFunc of, Args... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pRoot) return end();

        Node* pCurrent = m_pRoot;
        size_t count = 0;
        
        do {
            if (of(pCurrent->GetValueRef(), args...)) {
                return forward_iterator(pCurrent, m_pRoot, m_isCircular);
            }
            pCurrent = pCurrent->GetNext();
            count++;
            
            if (m_isCircular && count >= m_nElements) break;
            
        } while (pCurrent && pCurrent != m_pRoot);

        return end();
    }

    friend ostream& operator<<(ostream& os, CLinkedList<Traits>& container) {
        std::lock_guard<std::mutex> lock(container.m_mutex);
        
        os << "CLinkedList: size = " << container.m_nElements 
           << (container.m_isCircular ? " (Circular)" : "") << endl;
        os << "[";
        
        if (container.m_pRoot) {
            Node* pCurrent = container.m_pRoot;
            size_t count = 0;
            
            do {
                os << "(" << pCurrent->GetValue() << ":" << pCurrent->GetRef() << ")";
                pCurrent = pCurrent->GetNext();
                count++;
                
                if (pCurrent && ((!container.m_isCircular) || count < container.m_nElements)) {
                    os << ",";
                }
                
                if (container.m_isCircular && count >= container.m_nElements) break;
                
            } while (pCurrent && pCurrent != container.m_pRoot);
        }
        
        os << "]" << endl;
        return os;
    }

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
            Node* pNew = new Node(val, ref, rParent);
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
