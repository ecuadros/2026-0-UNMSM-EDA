#ifndef __AVL_H__
#define __AVL_H__

#include "binarytree.h"
#include <algorithm>


// CLASE AVL (HEREDA DE BINARYTREE)

template <typename Traits>
class CAVL : public CBinaryTree<Traits> {
public:
    using value_type = typename Traits::value_type;
    using Node = NodeBinaryTree<Traits>;
    using CompareFunc = typename Traits::CompareFunc;
    
private:
    
    // MÉTODOS AUXILIARES PARA AVL

    int GetHeight(Node *node) const {
        return node ? node->m_height : 0;
    }
    
    // Actualizar altura de un nodo
    void UpdateHeight(Node *node) {
        if (node) {
            node->m_height = 1 + std::max(GetHeight(node->m_pChild[0]), 
                                          GetHeight(node->m_pChild[1]));
        }
    }
    

    int GetBalance(Node *node) const {
        return node ? GetHeight(node->m_pChild[0]) - GetHeight(node->m_pChild[1]) : 0;
    }
    
    
    // ROTACIONES
    
    
    // Rotación Simple a la Derecha 
    Node* RotateRight(Node *y) {
        Node *x = y->m_pChild[0];
        Node *T2 = x->m_pChild[1];
        
        // Realizar rotación
        x->m_pChild[1] = y;
        y->m_pChild[0] = T2;
        
        // Actualizar alturas
        UpdateHeight(y);
        UpdateHeight(x);
        
        return x;  // Nueva raíz
    }
    
    // Rotación Simple a la Izquierda 
    Node* RotateLeft(Node *x) {
        Node *y = x->m_pChild[1];
        Node *T2 = y->m_pChild[0];
        
        // Realizar rotación
        y->m_pChild[0] = x;
        x->m_pChild[1] = T2;
        
        // Actualizar alturas
        UpdateHeight(x);
        UpdateHeight(y);
        
        return y;  // Nueva raíz
    }
   
    
    //INSERT CON TRAITS
    
    Node* InternalInsertAVL(Node *node, const value_type &val, ref_type ref) {
        if (!node) {
            ++(this->m_nElements);
            return new Node(val, ref);
        }
        
        if (val == node->GetValue()) {
            return node; 
        }
        
        // Magia de Traits
        size_t path = this->comp(val, node->GetValue());
        node->m_pChild[path] = InternalInsertAVL(node->m_pChild[path], val, ref);
        
        UpdateHeight(node);
        int balance = GetBalance(node);
        
        // Casos de rotación dinámicos 
        if (balance > 1) { 
            size_t path_left = this->comp(val, node->m_pChild[0]->GetValue());
            if (path_left == 0) { 
                return RotateRight(node);
            } else {              
                node->m_pChild[0] = RotateLeft(node->m_pChild[0]);
                return RotateRight(node);
            }
        }
        
        if (balance < -1) { 
            size_t path_right = this->comp(val, node->m_pChild[1]->GetValue());
            if (path_right == 1) { 
                return RotateLeft(node);
            } else {               
                node->m_pChild[1] = RotateRight(node->m_pChild[1]);
                return RotateLeft(node);
            }
        }
        
        return node;
    }

    // REMOVE CON REBALANCEO ESTRICTO AVL
    Node* InternalRemoveAVL(Node *node, const value_type &val) {
        if (!node) return nullptr;

        // 1. Búsqueda y Eliminación estándar
        if (val == node->GetValue()) {
            // Caso 0 o 1 hijo
            if (!node->m_pChild[0] || !node->m_pChild[1]) {
                Node *temp = node->m_pChild[0] ? node->m_pChild[0] : node->m_pChild[1];
                delete node;
                --(this->m_nElements);
                node = temp; // Si no tenía hijos, node se vuelve nullptr
            } 
            // Caso 2 hijos
            else {
                // Buscamos el sucesor inorden (el mínimo del subárbol derecho [1])
                Node *temp = node->m_pChild[1];
                while (temp->m_pChild[0]) {
                    temp = temp->m_pChild[0];
                }
                
                // Copiamos el valor del sucesor al nodo actual
                node->GetValueRef() = temp->GetValue();
                
                // Eliminamos el sucesor recursivamente
                node->m_pChild[1] = InternalRemoveAVL(node->m_pChild[1], temp->GetValue());
            }
        } else {
            // Usamos tu magia de Traits para navegar
            size_t path = this->comp(val, node->GetValue());
            node->m_pChild[path] = InternalRemoveAVL(node->m_pChild[path], val);
        }

        // Si el árbol tenía un solo nodo y lo acabamos de borrar
        if (!node) return nullptr;

        // 2. Actualizar Altura del nodo actual
        UpdateHeight(node);

        // 3. Obtener el factor de balance
        int balance = GetBalance(node);

        // 4. Casos de Rotación (El "Core" del AVL en borrado)

        // Desbalanceado por la izquierda (LL o LR)
        if (balance > 1) {
            int balanceLeft = GetBalance(node->m_pChild[0]);
            if (balanceLeft >= 0) { 
                return RotateRight(node); // Caso LL
            } else { 
                node->m_pChild[0] = RotateLeft(node->m_pChild[0]); // Caso LR
                return RotateRight(node);
            }
        }

        // Desbalanceado por la derecha (RR o RL)
        if (balance < -1) {
            int balanceRight = GetBalance(node->m_pChild[1]);
            if (balanceRight <= 0) { 
                return RotateLeft(node); // Caso RR
            } else { 
                node->m_pChild[1] = RotateRight(node->m_pChild[1]); // Caso RL
                return RotateLeft(node);
            }
        }

        return node;
    }
    
    // Mostrar estructura del árbol 
    void InternalPrintTree(Node *node, std::ostream &os, std::string indent, bool last) {
        if (node) {
            os << indent;
            if (last) {
                os << "R----";
                indent += "     ";
            } else {
                os << "L----";
                indent += "|    ";
            }
            os << node->GetValue() << " (h:" << node->m_height 
               << ", bf:" << GetBalance(node) << ")" << std::endl;
            
            InternalPrintTree(node->m_pChild[0], os, indent, false);
            InternalPrintTree(node->m_pChild[1], os, indent, true);
        }
    }
    
    // Verificar balanceo recursivo
    bool InternalIsBalanced(Node *node) const {
        if (!node) return true;
        
        int balance = GetBalance(node);
        if (balance > 1 || balance < -1) return false;
        
        return InternalIsBalanced(node->m_pChild[0]) && 
               InternalIsBalanced(node->m_pChild[1]);
    }

public:
    CAVL() : CBinaryTree<Traits>() {}
    
     // INSERT 

    void Insert(const value_type &val, ref_type ref = -1) override {
        std::lock_guard<std::mutex> lock(this->m_mutex);
        this->m_pRoot = InternalInsertAVL(this->m_pRoot, val, ref);
    }

    // REMOVE
    void Remove(const value_type &val) override {
        std::lock_guard<std::mutex> lock(this->m_mutex);
        this->m_pRoot = InternalRemoveAVL(this->m_pRoot, val);
    }
    
    // Verificar si está balanceado
    
    bool isBalanced() const {
        return InternalIsBalanced(this->m_pRoot);
    }
    

    // Mostrar estructura del árbol
    
    void PrintTree(std::ostream &os = std::cout) {
        std::lock_guard<std::mutex> lock(this->m_mutex);
        os << "AVL Tree Structure:" << std::endl;
        if (this->m_pRoot) {
            InternalPrintTree(this->m_pRoot, os, "", true);
        } else {
            os << "(empty)" << std::endl;
        }
    }
    
    //  Operator << SIN DEADLOCK
    
    friend std::ostream& operator<<(std::ostream& os, CAVL<Traits>& tree) {
        
        os << "CAVL Tree: size = " << tree.getSize() 
           << ", balanced = " << (tree.isBalanced() ? "YES" : "NO") << std::endl;
        os << "[ ";
        
        
        tree.Inorden([&os](auto &val) {
            os << val << " ";
        });
        
        os << "]" << std::endl;
        return os;
    }
};

#endif // __AVL_H__