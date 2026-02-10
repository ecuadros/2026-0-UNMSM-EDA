#ifndef __STACK_H__
#define __STACK_H__

#include <mutex>
#include <cassert>
#include <iostream>
#include "../general/types.h"
#include "../util.h"
#include "../containers/linkedlist.h"

using namespace std;

template <typename T>
struct StackTrait {
    using value_type = T;
};

template <typename T>
class CStack{
    using Traits = StackTrait<T>;
    using value_type = typename Traits::value_type;

    private:
    CLinkedList < AscendingTrait<value_type> > m_list;
    mutable std::mutex m_mutex;

    public:
    CStack() {};

    //Constructor copia
    CStack(const CStack &other) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_list = other.m_list;
    };

    //Move Constructor
    CStack(CStack &&other) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_list = std::move(other.m_list);
    };

    //Destructor seguro y virtual
    virtual ~CStack() {
        std::lock_guard<std::mutex> lock(m_mutex);
        // El destructor de CLinkedList se encargará de liberar la memoria
    };

    // Método para imprimir el stack (para operator<<)
    value_type top() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        assert(m_list.getSize() > 0);
        return m_list.get_front(); // Retorna el valor del tope (último elemento) 
    };

    //Push (LIFO)
    void push(const value_type &value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_list.push_front(const_cast<value_type&>(value)); // Agrega el nuevo valor al frente de la lista (tope del stack)
    };

    //Pop (LIFO)
    value_type pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        assert(m_list.getSize() > 0);

        value_type value = m_list.get_front(); // Obtener el valor del tope
        m_list.pop_front(); // Eliminar el primer elemento (tope del stack)
        return value; // Retornar el valor del tope
    };

    // Método para imprimir el stack (para operator<<)
    void print(std::ostream &os = std::cout) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        os << "[top] ";
        if(m_list.getSize() > 0) {
            os << m_list.get_front(); // Imprime el valor del tope
        } else {
            os << "(stack vacío)";
        }
    };

    //operator<<
    CStack& operator<<(const value_type& value) {
        push(value);
        return *this;
    };

    //operator>>
    CStack& operator>>(value_type& value) {
        value = top();
        pop();
        return *this;
    };
};

template <typename T>
ostream& operator<<(std::ostream& os, const CStack<T>& stack) {
    stack.print(os);
    return os;
}



#endif // __STACK_H__