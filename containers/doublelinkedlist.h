#ifndef __DOUBLE_LINKED_LIST_H__
#define __DOUBLE_LINKED_LIST_H__

#include <iostream>
#include <mutex>
#include <memory>
#include "../general/types.h"
#include "../util.h"

using namespace std;

template <typename T, typename _Func>
struct DoubleListTrait {
    using value_type = T;
    using Func       = _Func;
};

template <typename T>
struct DoubleAscendingTrait : public DoubleListTrait<T, std::greater<T>> {};

template <typename T>
struct DoubleDescendingTrait : public DoubleListTrait<T, std::less<T>> {};

template <typename Traits>
class NodeDoubleLinkedList {
    using value_type = typename Traits::value_type;
    using Node       = NodeDoubleLinkedList<Traits>;

private:
    value_type m_data;
    ref_type   m_ref;
    Node      *m_pNext = nullptr;
    Node      *m_pPrev = nullptr;

public:
    NodeDoubleLinkedList() {}
    NodeDoubleLinkedList(value_type _value, ref_type _ref = -1, Node *pNext = nullptr, Node *pPrev = nullptr)
        : m_data(_value), m_ref(_ref), m_pNext(pNext), m_pPrev(pPrev) {}

    value_type  GetValue()    const { return m_data; }
    value_type& GetValueRef()       { return m_data; }

    ref_type    GetRef()      const { return m_ref; }
    ref_type&   GetRefRef()         { return m_ref; }

    Node*       GetNext()     const { return m_pNext; }
    Node*&      GetNextRef()        { return m_pNext; }

    Node*       GetPrev()     const { return m_pPrev; }
    Node*&      GetPrevRef()        { return m_pPrev; }

    Node& operator=(const Node& another) {
        m_data  = another.GetValue();
        m_ref   = another.GetRef();
        m_pNext = another.GetNext();
        m_pPrev = another.GetPrev();
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
class DoubleLinkedListForwardIterator {
public:
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

private:
    Node* m_pCurrent;
    Node* m_pRoot;
    bool  m_isCircular;
    bool  m_hasLooped;

public:
    DoubleLinkedListForwardIterator(Node* pNode, Node* pRoot = nullptr, bool isCircular = false)
        : m_pCurrent(pNode), m_pRoot(pRoot), m_isCircular(isCircular), m_hasLooped(false) {}

    bool operator!=(const DoubleLinkedListForwardIterator& other) const {
        if (m_isCircular) {
            return !(m_hasLooped && m_pCurrent == other.m_pCurrent);
        }
        return m_pCurrent != other.m_pCurrent;
    }

    bool operator==(const DoubleLinkedListForwardIterator& other) const {
        return !(*this != other);
    }

    value_type& operator*() {
        return m_pCurrent->GetValueRef();
    }

    DoubleLinkedListForwardIterator& operator++() {
        if (m_pCurrent) {
            m_pCurrent = m_pCurrent->GetNext();
            if (m_isCircular && m_pCurrent == m_pRoot) {
                m_hasLooped = true;
            }
        }
        return *this;
    }

    DoubleLinkedListForwardIterator operator++(int) {
        DoubleLinkedListForwardIterator temp = *this;
        ++(*this);
        return temp;
    }
};

template <typename Container>
class DoubleLinkedListBackwardIterator {
public:
    using value_type = typename Container::value_type;
    using Node       = typename Container::Node;

private:
    Node* m_pCurrent;
    Node* m_pLast;
    bool  m_isCircular;
    bool  m_hasLooped;

public:
    DoubleLinkedListBackwardIterator(Node* pNode, Node* pLast = nullptr, bool isCircular = false)
        : m_pCurrent(pNode), m_pLast(pLast), m_isCircular(isCircular), m_hasLooped(false) {}

    bool operator!=(const DoubleLinkedListBackwardIterator& other) const {
        if (m_isCircular) {
            return !(m_hasLooped && m_pCurrent == other.m_pCurrent);
        }
        return m_pCurrent != other.m_pCurrent;
    }

    bool operator==(const DoubleLinkedListBackwardIterator& other) const {
        return !(*this != other);
    }

    value_type& operator*() {
        return m_pCurrent->GetValueRef();
    }

    DoubleLinkedListBackwardIterator& operator++() {
        if (m_pCurrent) {
            m_pCurrent = m_pCurrent->GetPrev();
            if (m_isCircular && m_pCurrent == m_pLast) {
                m_hasLooped = true;
            }
        }
        return *this;
    }

    DoubleLinkedListBackwardIterator operator++(int) {
        DoubleLinkedListBackwardIterator temp = *this;
        ++(*this);
        return temp;
    }
};

template <typename Traits>
class CDoubleLinkedList {
public:
    using value_type         = typename Traits::value_type;
    using Node               = NodeDoubleLinkedList<Traits>;
    using forward_iterator   = DoubleLinkedListForwardIterator<CDoubleLinkedList<Traits>>;
    using backward_iterator  = DoubleLinkedListBackwardIterator<CDoubleLinkedList<Traits>>;

    friend forward_iterator;
    friend backward_iterator;

private:
    Node*       m_pRoot;
    Node*       m_pLast;
    size_t      m_nElements;
    bool        m_isCircular;
    mutable std::mutex m_mutex;

public:
    CDoubleLinkedList(bool circular = false)
        : m_pRoot(nullptr), m_pLast(nullptr), m_nElements(0), m_isCircular(circular) {}

    CDoubleLinkedList(const CDoubleLinkedList& other)
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

    CDoubleLinkedList(CDoubleLinkedList&& other) noexcept
        : m_pRoot(other.m_pRoot),
          m_pLast(other.m_pLast),
          m_nElements(other.m_nElements),
          m_isCircular(other.m_isCircular) {
        
        other.m_pRoot = nullptr;
        other.m_pLast = nullptr;
        other.m_nElements = 0;
    }

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

    CDoubleLinkedList& operator=(CDoubleLinkedList&& other) noexcept {
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

        Node* pCurrent;
        if (index < m_nElements / 2) {
            pCurrent = m_pRoot;
            for (size_t i = 0; i < index; ++i) {
                pCurrent = pCurrent->GetNext();
            }
        } else {
            pCurrent = m_pLast;
            for (size_t i = m_nElements - 1; i > index; --i) {
                pCurrent = pCurrent->GetPrev();
            }
        }
        
        return pCurrent->GetValueRef();
    }

    const value_type& operator[](size_t index) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (index >= m_nElements) {
            throw std::out_of_range("Index out of range");
        }

        Node* pCurrent;
        if (index < m_nElements / 2) {
            pCurrent = m_pRoot;
            for (size_t i = 0; i < index; ++i) {
                pCurrent = pCurrent->GetNext();
            }
        } else {
            pCurrent = m_pLast;
            for (size_t i = m_nElements - 1; i > index; --i) {
                pCurrent = pCurrent->GetPrev();
            }
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

    backward_iterator rbegin() {
        return backward_iterator(m_pLast, m_pLast, m_isCircular);
    }

    backward_iterator rend() {
        if (m_isCircular) {
            return backward_iterator(m_pLast, m_pLast, m_isCircular);
        }
        return backward_iterator(nullptr, m_pLast, m_isCircular);
    }

    void push_back(const value_type& val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        Node* pNewNode = new Node(val, ref);
        
        if (!m_pRoot) {
            m_pRoot = pNewNode;
            m_pLast = pNewNode;
        } else {
            m_pLast->GetNextRef() = pNewNode;
            pNewNode->GetPrevRef() = m_pLast;
            m_pLast = pNewNode;
        }
        
        if (m_isCircular) {
            m_pLast->GetNextRef() = m_pRoot;
            m_pRoot->GetPrevRef() = m_pLast;
        }
        
        ++m_nElements;
    }

    void push_front(const value_type& val, ref_type ref = -1) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        Node* pNewNode = new Node(val, ref);
        
        if (!m_pRoot) {
            m_pRoot = pNewNode;
            m_pLast = pNewNode;
        } else {
            pNewNode->GetNextRef() = m_pRoot;
            m_pRoot->GetPrevRef() = pNewNode;
            m_pRoot = pNewNode;
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
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (!m_pRoot) return;

        if (m_isCircular && m_pLast) {
            m_pLast->GetNextRef() = nullptr;
            m_pRoot->GetPrevRef() = nullptr;
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

    friend ostream& operator<<(ostream& os, CDoubleLinkedList<Traits>& container) {
        std::lock_guard<std::mutex> lock(container.m_mutex);
        
        os << "CDoubleLinkedList: size = " << container.m_nElements 
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
            Node* pNew = new Node(val, ref, rParent);
            
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
