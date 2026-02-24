#ifndef __AVL_H__
#define __AVL_H__

#include "binarytree.h"


// TRAITS PARA AVL 

template <typename T>
using AscendingTraitAVL = AscendingTrait<T>;

template <typename T>
using DescendingTraitAVL = DescendingTrait<T>;


// CLASE AVL 

template <typename Traits>
class CAVL : public CBinaryTree<Traits> {
    using Base = CBinaryTree<Traits>;
    using Node = typename Base::Node;
    using value_type = typename Base::value_type;
    
private:
    
    // Acá se calcula el factor de balance

    int obtenerBalance(Node *nodo) {
        if (!nodo) {
            return 0;
        }
        return Base::altura(nodo->m_pChild[0]) - Base::altura(nodo->m_pChild[1]);
    }
    
 
    // ROTACIONES SIMPLES

    
    // Rotación simple derecha
    Node* rotacionDerecha(Node *y) {
        Node *x = y->m_pChild[0];
        Node *B = x->m_pChild[1];
        
        // Realizar rotación
        x->m_pChild[1] = y;
        y->m_pChild[0] = B;
        
        // Actualizar padres
        x->m_pParent = y->m_pParent;
        y->m_pParent = x;
        if (B) {
            B->m_pParent = y;
        }
        
        // Actualizar alturas
        Base::actualizarAltura(y);
        Base::actualizarAltura(x);
        
        return x;
    }
    
    // Rotación simple izquierda
    Node* rotacionIzquierda(Node *x) {
        Node *y = x->m_pChild[1];
        Node *B = y->m_pChild[0];
        
        // Realizar rotación
        y->m_pChild[0] = x;
        x->m_pChild[1] = B;
        
        // Actualizar padres
        y->m_pParent = x->m_pParent;
        x->m_pParent = y;
        if (B) {
            B->m_pParent = x;
        }
        
        // Actualizar alturas
        Base::actualizarAltura(x);
        Base::actualizarAltura(y);
        
        return y;
    }
    

    // BALANCEO 

    Node* balancear(Node *nodo) {
        if (!nodo) {
            return nullptr;
        }
        
        int balance = obtenerBalance(nodo);
        
        
        if (balance > 1) {
            int balanceHijoIzq = obtenerBalance(nodo->m_pChild[0]);
            
            
            if (balanceHijoIzq < 0) {
                nodo->m_pChild[0] = rotacionIzquierda(nodo->m_pChild[0]);
            }
            
            
            return rotacionDerecha(nodo);
        }
        
        
        if (balance < -1) {
            int balanceHijoDer = obtenerBalance(nodo->m_pChild[1]);
            
            
            if (balanceHijoDer > 0) {
                nodo->m_pChild[1] = rotacionDerecha(nodo->m_pChild[1]);
            }
            
            
            return rotacionIzquierda(nodo);
        }
        
        
        return nodo;
    }
    
protected:
    
    // Insert con balanceo
    
    virtual Node* insertarNodo(Node *nodo, const value_type &valor, ref_type ref, Node *parent) override {
        
        nodo = Base::insertarNodo(nodo, valor, ref, parent);
        
        
        Base::actualizarAltura(nodo);
        
       
        return balancear(nodo);
    }
    
    
    //Remove con balanceo
    
    virtual Node* eliminarNodo(Node *nodo, const value_type &valor) override {
        
        nodo = Base::eliminarNodo(nodo, valor);
        
        if (!nodo) {
            return nullptr;
        }
        
        
        Base::actualizarAltura(nodo);
        
        
        return balancear(nodo);
    }

public:
    // Constructor por defecto
    CAVL() : Base() {}
    
    
    // Constructor Copia 
    
    CAVL(const CAVL &other) : Base(other) {}
    
   
    // Move Constructor 
    
    CAVL(CAVL &&other) noexcept : Base(std::move(other)) {}
    
    
    // Destructor (hereda de Base)
    
    virtual ~CAVL() = default;
    
    
    // Verificar si está balanceado
    
    bool estaBalanceado() {
        std::lock_guard<std::mutex> lock(Base::m_mutex);
        return verificarBalance(Base::m_pRoot);
    }
    
private:
    bool verificarBalance(Node *nodo) {
        if (!nodo) {
            return true;
        }
        
        int balance = obtenerBalance(nodo);
        if (balance < -1 || balance > 1) {
            return false;
        }
        
        return verificarBalance(nodo->m_pChild[0]) && verificarBalance(nodo->m_pChild[1]);
    }

public:
   
    // Impresión Tabulada CON ALTURAS
    
    void printTabuladoConAlturas(std::ostream &os = std::cout) {
        std::lock_guard<std::mutex> lock(Base::m_mutex);
        if (Base::m_pRoot) {
            os << "AVL Tree (con alturas):" << std::endl;
            imprimirConAlturas(Base::m_pRoot, 0, os);
        } else {
            os << "(árbol vacío)" << std::endl;
        }
    }
    
private:
    void imprimirConAlturas(Node *nodo, int nivel, std::ostream &os) {
        if (nodo) {
            imprimirConAlturas(nodo->m_pChild[1], nivel + 1, os);
            
            for (int i = 0; i < nivel; ++i) {
                os << "\t";
            }
            os << nodo->m_data << " (h=" << nodo->m_height << ")" << std::endl;
            
            imprimirConAlturas(nodo->m_pChild[0], nivel + 1, os);
        }
    }
};


// Usa  automáticamente el operator<< de CBinaryTree por herencia


#endif // __AVL_H__