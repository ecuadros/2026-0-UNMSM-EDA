#ifndef __STACK_H__
#define __STACK_H__

#include <iostream>
#include <mutex>
#include <cassert>
#include <utility>


template <typename T, typename _Func>
struct StackTrait{
    using value_type = T;
    using Func       = _Func;
};


template <typename Traits>
class NodeStack{

    using value_type = typename Traits::value_type;
    using Node       = NodeStack<Traits>;

private:
    value_type m_data;
    Node      *m_pNext = nullptr;

public:
    NodeStack(){}
    NodeStack(value_type val) : m_data(val){}

    value_type  GetValue() const { return m_data; }
    value_type &GetValueRef()    { return m_data; }

    Node*  GetNext() const { return m_pNext; }
    Node*& GetNextRef()    { return m_pNext; }
};



template <typename Traits>
class CStack{

public:
    using value_type = typename Traits::value_type;
    using Node       = NodeStack<Traits>;

private:
    Node     *m_pTop = nullptr;
    size_t    m_nElements = 0;
    mutable   std::mutex m_mutex;

public:
    CStack(){}

    // Copy Constructor

    CStack(const CStack &another){
        std::lock_guard<std::mutex> lock(another.m_mutex);

        assert( another.m_pTop != nullptr );
        Node *temp = another.m_pTop;
        Node *prev = nullptr;

        while (temp){
            Node *pNew = new Node(temp->GetValue());
            pNew->GetNextRef() = prev;
            prev = pNew;
            temp = temp->GetNext();
        }

        m_pTop = prev;
        m_nElements = another.m_nElements;
    }


    // Move Constructor

    CStack(CStack &&another) noexcept{
        std::lock_guard<std::mutex> lock(another.m_mutex);

        m_pTop      = std::exchange(another.m_pTop, nullptr);
        m_nElements = std::exchange(another.m_nElements, 0);
    }

    // Destructor

    virtual ~CStack(){
        clear();
    }

    void push(const value_type &val){
        std::lock_guard<std::mutex> lock(m_mutex);

        Node *pNew = new Node(val);
        pNew->GetNextRef() = m_pTop;
        m_pTop = pNew;

        ++m_nElements;
    }

    void pop(){
        std::lock_guard<std::mutex> lock(m_mutex);

        assert(m_pTop != nullptr);

        Node *pDelete = m_pTop;
        m_pTop = m_pTop->GetNext();

        delete pDelete;
        --m_nElements;
    }

    value_type &top(){
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_pTop->GetValueRef();
    }

    const value_type &top() const{
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_pTop->GetValue();
    }

    bool empty() const{
        return m_nElements == 0;
    }

    Size getSize() const{
        return m_nElements;
    }

    void clear() noexcept{
        std::lock_guard<std::mutex> lock(m_mutex);

        Node *temp = m_pTop;
        while (temp){
            Node *n = temp->GetNext();
            delete temp;
            temp = n;
        }

        m_pTop = nullptr;
        m_nElements = 0;
    }

    // Operator<<
    

    friend std::ostream &operator<<(std::ostream &os, const CStack &stack){
        std::lock_guard<std::mutex> lock(stack.m_mutex);

        os << "CStack: size = " << stack.m_nElements << std::endl;
        os << "Top -> ";

        Node *temp = stack.m_pTop;
        while (temp){
            os << temp->GetValue();
            if (temp->GetNext()) 
            os << " -> ";
            temp = temp->GetNext();
        }
        
        return os;
    }

    // Operator>>

    friend std::istream &operator>>(std::istream &is, CStack &stack){
        std::lock_guard<std::mutex> lock(stack.m_mutex);

        stack.clear();

        std::cout << "¿Cuántos elementos desea añadir al stack? ";
        size_t count;
        is >> count;

        for (size_t i = 0; i < count; ++i){
            value_type val;

            std::cout << "Elemento " << (i + 1) << ": ";
            is >> val;

            stack.push(val);
        }

        return is;
    }
};

void DemoStack();

#endif // __STACK_H__
