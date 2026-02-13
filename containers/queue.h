#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <vector>
#include <iomanip>
#include "../general/types.h"
#include "../util.h"

using namespace std;

template <typename _T>
struct QueueTraits {
    using T = _T;
};

template <typename Traits>
class CQueueNode {
    using value_type = typename Traits::T;
    using Node = CQueueNode;

    value_type  m_data;
    ref_type    m_ref;
    CQueueNode *m_pNext;

public:
    CQueueNode() : m_ref(-1), m_pNext(nullptr) {}
    CQueueNode(const value_type &val, ref_type ref, Node *next = nullptr)
        : m_data(val), m_ref(ref), m_pNext(next) {}

    value_type  GetValue   () const { return m_data; }
    value_type &GetValueRef() { return m_data; }
    ref_type    GetRef     () const { return m_ref;   }
    ref_type   &GetRefRef  () { return m_ref;   }
    Node      * GetNext    () const { return m_pNext;   }
    Node      *&GetNextRef () { return m_pNext;   }

    Node &operator=(const Node &another){
        if (this == &another) return *this;

        m_data = another.GetValue();
        m_ref  = another.GetRef();
        return *this;
    }
    bool operator==(const Node &another) const
    { return m_data == another.GetValue();   }
    bool operator<(const Node &another) const
    { return m_data < another.GetValue();   }

};

template <typename Traits>
class CQueue;

template <typename Traits>
ostream &operator<<(ostream &os, CQueue<Traits> &container);

template <typename Traits>
istream &operator>>(istream &is, CQueue<Traits> &container);

template <typename Traits>
class CQueue {
    mutable mutex mtx;
    using value_type = typename Traits::T;
    using Node = CQueueNode<Traits>;

    Node *m_pFirst     = nullptr;
    Node *m_pLast      = nullptr;
    size_t m_nElements = 0;

public:
    // Constructor por defecto
    CQueue() {}
    
    // Constructor con un elemento inicial
    CQueue(const value_type &val, ref_type ref) {
        push(val, ref);
    }

    // Constructor de copia
    CQueue(const CQueue& other) {
        lock_guard<mutex> lock(other.mtx);
        duplicate_structure(other);
    }

    // Constructor de movimiento
    CQueue(CQueue&& other) noexcept {
        lock_guard<mutex> lock(other.mtx);
        m_pFirst = other.m_pFirst;
        m_pLast = other.m_pLast;
        m_nElements = other.m_nElements;
        
        other.m_pFirst = nullptr;
        other.m_pLast = nullptr;
        other.m_nElements = 0;
    }

    // Destructor
    virtual ~CQueue() {
        lock_guard<mutex> lock(mtx);
        release_all_nodes();
    }

    // Operador de asignación por copia
    CQueue& operator=(const CQueue& other) {
        // Verificación de auto-asignación
        if (this == &other) {
            return *this;
        }

        // Crear buffer temporal con los elementos del origen
        vector<pair<value_type, ref_type>> temp_buffer;
        {
            lock_guard<mutex> lock_source(other.mtx);
            Node *current = other.m_pFirst;
            while (current != nullptr) {
                temp_buffer.emplace_back(current->GetValue(), current->GetRef());
                current = current->GetNext();
            }
        }

        // Limpiar la cola actual y agregar los nuevos elementos
        {
            lock_guard<mutex> lock_this(mtx);
            release_all_nodes();
        }

        for (const auto &element : temp_buffer) {
            push(element.first, element.second);
        }

        return *this;
    }

    // Operador de asignación por movimiento
    CQueue& operator=(CQueue&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        // Bloquear ambos mutex en orden consistente
        scoped_lock double_lock(mtx, other.mtx);

        release_all_nodes();

        m_pFirst = other.m_pFirst;
        m_pLast = other.m_pLast;
        m_nElements = other.m_nElements;

        other.m_pFirst = nullptr;
        other.m_pLast = nullptr;
        other.m_nElements = 0;

        return *this;
    }

    // Agregar elemento al final de la cola
    void push(const value_type &val, ref_type ref) {
        lock_guard<mutex> lock(mtx);
        insert_at_end(val, ref);
    }

    // Extraer elemento del frente de la cola
    value_type pop() {
        lock_guard<mutex> lock(mtx);

        // Validar que la cola no esté vacía
        if (m_pFirst == nullptr) {
            throw runtime_error("CQueue::pop - Cannot pop from empty queue");
        }

        value_type result = m_pFirst->GetValue();
        Node *node_to_delete = m_pFirst;
        
        m_pFirst = m_pFirst->GetNext();
        
        // Si la cola quedó vacía, actualizar m_pLast
        if (m_pFirst == nullptr) {
            m_pLast = nullptr;
        }
        
        delete node_to_delete;
        --m_nElements;
        
        return result;
    }

private:
    // Método auxiliar para insertar sin bloqueo (usado internamente)
    void insert_at_end(const value_type &val, ref_type ref) {
        Node *new_node = new Node(val, ref);
        
        if (m_pLast != nullptr) {
            m_pLast->GetNextRef() = new_node;
            m_pLast = new_node;
        } else {
            // La cola estaba vacía
            m_pFirst = new_node;
            m_pLast = new_node;
        }
        
        ++m_nElements;
    }

    // Método auxiliar para duplicar la estructura de otra cola
    void duplicate_structure(const CQueue& source) {
        if (source.m_pFirst == nullptr) {
            return;
        }

        Node *cursor = source.m_pFirst;
        while (cursor != nullptr) {
            insert_at_end(cursor->GetValue(), cursor->GetRef());
            cursor = cursor->GetNext();
        }
    }

    // Método auxiliar para liberar todos los nodos
    void release_all_nodes() {
        Node *current = m_pFirst;
        Node *next_node = nullptr;
        
        while (current != nullptr) {
            next_node = current->GetNext();
            delete current;
            current = next_node;
        }
        
        m_pFirst = nullptr;
        m_pLast = nullptr;
        m_nElements = 0;
    }

    friend ostream &operator<< <Traits>(ostream &os, CQueue<Traits> &container);
    friend istream &operator>> <Traits>(istream &is, CQueue<Traits> &container);
};

// Operador de salida (<<)
template <typename Traits>
ostream &operator<<(ostream &os, CQueue<Traits> &container) {
    using value_type = typename CQueue<Traits>::value_type;
    using Node = CQueueNode<Traits>;
    
    lock_guard<mutex> lock(container.mtx);
    
    os << "CQueue: size = " << container.m_nElements << " [";
    
    Node *current = container.m_pFirst;
    while (current != nullptr) {
        if constexpr (std::is_same_v<value_type, std::string>) {
            os << "(" << std::quoted(current->GetValue()) << ":" << current->GetRef() << "),";
        } else {
            os << "(" << current->GetValue() << ":" << current->GetRef() << "),";
        }
        current = current->GetNext();
    }
    
    os << "]" << endl;
    return os;
}

// Operador de entrada (>>)
template <typename Traits>
istream &operator>>(istream &is, CQueue<Traits> &container) {
    using value_type = typename CQueue<Traits>::value_type;
    
    if (!is) return is;

    vector<pair<value_type, ref_type>> elements;
    
    try {
        string buffer;
        // Saltar hasta encontrar '['
        getline(is, buffer, '[');

        char character;
        while (is.get(character) && character != ']') {
            // Buscar inicio de elemento '('
            if (character != '(') continue;
            
            value_type val;
            ref_type ref;
            
            if constexpr (std::is_same_v<value_type, std::string>) {
                is >> std::quoted(val);
                getline(is, buffer, ':');
            } else {
                is >> val;
                getline(is, buffer, ':');
            }
            
            is >> ref;
            getline(is, buffer, ')');
            
            elements.emplace_back(val, ref);
        }
    } catch (const exception& e) {
        is.setstate(ios::failbit);
    }

    // Si la lectura fue exitosa, reemplazar contenido
    if (is) {
        lock_guard<mutex> lock(container.mtx);
        container.release_all_nodes();
        
        for (const auto &elem : elements) {
            container.insert_at_end(elem.first, elem.second);
        }
    }
    
    return is;
}

#endif // __QUEUE_H__