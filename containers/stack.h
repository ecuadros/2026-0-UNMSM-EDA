#ifndef __STACK_H__
#define __STACK_H__

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <vector>
#include <stdexcept>
#include "../general/types.h"
#include "../util.h"

using namespace std;

// TRAITS 
template <typename _T>
struct StackTraits {
    using T = _T;
};

//  STACK 
template <typename Traits>
class CStackNode {
    using value_type = typename Traits::T;
    using Node = CStackNode;

    value_type  m_data;
    ref_type    m_ref;
    CStackNode *m_pNext;

public:
    
    CStackNode() : m_ref(-1), m_pNext(nullptr) {}
    
  
    CStackNode(const value_type &val, ref_type ref, Node *next = nullptr)
        : m_data(val), m_ref(ref), m_pNext(next) {}

    
    value_type  GetValue   () const { return m_data;  }
    value_type &GetValueRef()       { return m_data;  }
    ref_type    GetRef     () const { return m_ref;   }
    ref_type   &GetRefRef  ()       { return m_ref;   }
    Node       *GetNext    () const { return m_pNext; }
    Node      *&GetNextRef ()       { return m_pNext; }

    
    Node &operator=(const Node &another) {
        m_data = another.GetValue();
        m_ref  = another.GetRef();
        return *this;
    }
    
    bool operator==(const Node &another) const {
        return m_data == another.GetValue();
    }
    
    bool operator<(const Node &another) const {
        return m_data < another.GetValue();
    }
};


template <typename Traits>
class CStack;

template <typename Traits>
ostream &operator<<(ostream &os, CStack<Traits> &container);

template <typename Traits>
istream &operator>>(istream &is, CStack<Traits> &container);

//  CLASE STACK
template <typename Traits>
class CStack {
    mutable mutex mtx;
    using value_type = typename Traits::T;
    using Node = CStackNode<Traits>;

    Node  *m_pTop;         
    size_t m_nElements;    

public:
    //  CONSTRUCTORES
    
    // Constructor por defecto
    CStack() : m_pTop(nullptr), m_nElements(0) {}
    
    // Constructor con valor inicial
    CStack(const value_type &val, ref_type ref) : m_pTop(nullptr), m_nElements(0) {
        push(val, ref);
    }
    
    // Constructor copia
    CStack(const CStack& other) : m_pTop(nullptr), m_nElements(0) {
        lock_guard<mutex> lock(other.mtx);
        copy_nodes_from(other);
    }

    // Move Constructor
    CStack(CStack&& other) noexcept
        : m_pTop(other.m_pTop),
          m_nElements(other.m_nElements) {
        // Dejar el objeto origen vacío
        other.m_pTop = nullptr;
        other.m_nElements = 0;
    }

    // DESTRUCTOR 
    
    // Destructor seguro 
    virtual ~CStack() {
        clear_unlocked();
    }

    
    CStack& operator=(const CStack& other) {
    
        if (this == &other) {
            return *this;
        }
        
       
        vector<pair<value_type, ref_type>> items;
        {
            lock_guard<mutex> lock(other.mtx);
            Node *pTrav = other.m_pTop;
            for (size_t i = 0; i < other.m_nElements; ++i) {
                items.emplace_back(pTrav->GetValue(), pTrav->GetRef());
                pTrav = pTrav->GetNext();
            }
        }
        
        
        reverse(items.begin(), items.end());
        
        
        {
            lock_guard<mutex> lock(mtx);
            clear_unlocked();
        }
        
       
        for (const auto &item : items) {
            push(item.first, item.second);
        }

        return *this;
    }

    
    CStack& operator=(CStack&& other) noexcept {
        
        if (this == &other) {
            return *this;
        }

        
        lock_guard<mutex> lock_this(mtx);
        lock_guard<mutex> lock_other(other.mtx);

        
        clear_unlocked();

        // Transferir recursos
        m_pTop = other.m_pTop;
        m_nElements = other.m_nElements;

        // Dejar el origen vacío
        other.m_pTop = nullptr;
        other.m_nElements = 0;

        return *this;
    }

    
    
    /**
     * @brief 
     * @param val 
     * @param ref 
     
     */
    void push(const value_type &val, ref_type ref) {
        lock_guard<mutex> lock(mtx);
        push_unlocked(val, ref);
    }
    
   
    
    /**
     * @brief Remueve y retorna el elemento del tope
     * @return Valor del elemento removido
     * @throws runtime_error si el stack está vacío
     * @complexity O(1)
     */
    value_type pop() {
        lock_guard<mutex> lock(mtx);

        if (!m_pTop) {
            throw runtime_error("CStack::pop() - Stack is empty");
        }
        
        
        value_type val = m_pTop->GetValue();
        
       
        Node *pTop = m_pTop;
        m_pTop = m_pTop->GetNext();
        delete pTop;
        --m_nElements;
        
        return val;
    }

    
    
    /**
     * @brief Verifica si el stack está vacío
     * @return true si está vacío, false en caso contrario
     */
    bool empty() const {
        lock_guard<mutex> lock(mtx);
        return m_pTop == nullptr;
    }
    
    /**
     * @brief Retorna el número de elementos en el stack
     * @return Cantidad de elementos
     */
    size_t size() const {
        lock_guard<mutex> lock(mtx);
        return m_nElements;
    }
    
    /**
     * @brief Limpia todos los elementos del stack
     */
    void clear() {
        lock_guard<mutex> lock(mtx);
        clear_unlocked();
    }

private:
   
    
    /**
     * @brief Push sin bloqueo de mutex (para uso interno)
     */
    void push_unlocked(const value_type &val, ref_type ref) {
        Node *pNew = new Node(val, ref, m_pTop);
        m_pTop = pNew;
        ++m_nElements;
    }
    
    /**
     * @brief Clear sin bloqueo de mutex (para uso interno)
     */
    void clear_unlocked() {
        if (!m_pTop) return;

        Node *pTrav = m_pTop;
        Node *pTemp = nullptr;
        
        // Recorrer y eliminar todos los nodos
        while (pTrav) {
            pTemp = pTrav;
            pTrav = pTrav->GetNextRef();
            delete pTemp;
        }
        
        m_pTop = nullptr;
        m_nElements = 0;
    }
    
    /**
     * @brief Copia los nodos de otro stack
     * @param other Stack del cual copiar
     * @note Asume que el mutex del otro stack ya está bloqueado
     */
    void copy_nodes_from(const CStack& other) {
        if (this == &other) return;
        if (!other.m_pTop) return;
        
        
        Node *pTrav = other.m_pTop;
        vector<pair<value_type, ref_type>> temp;
        
        
        while (pTrav) {
            temp.emplace_back(pTrav->GetValue(), pTrav->GetRef());
            pTrav = pTrav->GetNextRef();
        }
        
        
        reverse(temp.begin(), temp.end());
        
      
        for (const auto &[val, ref] : temp) {
            this->push_unlocked(val, ref);
        }
    }

    
    
    friend ostream &operator<< <Traits>(ostream &os, CStack<Traits> &container);
    friend istream &operator>> <Traits>(istream &is, CStack<Traits> &container);
};



/**
 * @brief Serializa el stack a un stream de salida
 * @param os 
 * @param container Stack a serializar
 * @return Stream de salida
 * @note 
 */
template <typename Traits>
ostream &operator<<(ostream &os, CStack<Traits> &container) {
    using value_type = typename CStack<Traits>::value_type;
    
    lock_guard<mutex> lock(container.mtx);
    
    os << "CStack: size = " << container.m_nElements << " [";
    
    
    for (auto trav = container.m_pTop; trav; trav = trav->GetNext()) {
        
        if constexpr (std::is_same_v<value_type, std::string>) {
            os << "(" << std::quoted(trav->GetValue()) << ":" << trav->GetRef() << ")";
        } else {
            os << "(" << trav->GetValue() << ":" << trav->GetRef() << ")";
        }
        
        
        if (trav->GetNext()) {
            os << ",";
        }
    }
    
    os << "]";
    return os;
}



/**
 * @brief Deserializa un stack desde un stream de entrada
 * @param is 
 * @param container 
 * @return 
 * @note 
 */
template <typename Traits>
istream &operator>>(istream &is, CStack<Traits> &container) {
    using value_type = typename CStack<Traits>::value_type;
    
    
    if (!is) return is;

    vector<pair<value_type, ref_type>> items;
    
    try {
        
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
            
            items.emplace_back(val, ref);
        }
    } catch (const exception& e) {
        
        is.setstate(ios::failbit);
    }
    
    
    if (is) {
        lock_guard<mutex> lock(container.mtx);
        container.clear_unlocked();
        
        
        for (auto it = items.rbegin(); it != items.rend(); ++it) {
            container.push_unlocked(it->first, it->second);
        }
    }
    
    return is;
}

#endif // __STACK_H__