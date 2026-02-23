#ifndef __AVL_H__
#define __AVL_H__

#include "binarytree.h"

template <typename Traits>
class CAVL : public CBinaryTree<Traits> {
    using Node = typename CBinaryTree<Traits>::Node;
    using value_type = typename CBinaryTree<Traits>::value_type;

private:
    int _h(Node* n) { return n ? n->m_height : 0; }
    
    void _updateH(Node* n) { 
        if(n) n->m_height = 1 + std::max(_h(n->m_pChild[0]), _h(n->m_pChild[1])); 
    }

    // Rotación
    void _rotate(Node*& n, bool side) {
        Node* aux = n->m_pChild[!side];
        n->m_pChild[!side] = aux->m_pChild[side];
        aux->m_pChild[side] = n;
        _updateH(n); 
        _updateH(aux);
        n = aux;
    }

    void _balance(Node*& n) {
        if (!n) return;
        _updateH(n);
        int fe = _h(n->m_pChild[0]) - _h(n->m_pChild[1]);
        
        if (fe > 1) { // Izquierda pesada
            if (_h(n->m_pChild[0]->m_pChild[0]) < _h(n->m_pChild[0]->m_pChild[1]))
                _rotate(n->m_pChild[0], 0); // Rotación Izquierda-Derecha
            _rotate(n, 1);
        } else if (fe < -1) { // Derecha pesada
            if (_h(n->m_pChild[1]->m_pChild[1]) < _h(n->m_pChild[1]->m_pChild[0]))
                _rotate(n->m_pChild[1], 1); // Rotación Derecha-Izquierda
            _rotate(n, 0);
        }
    }
    // Insert
    void _avlInsert(Node*& rNode, const value_type& val, ref_type ref) {
        if (!rNode) {
            rNode = new Node(val, ref);
            return;
        }
        bool side = this->comp(rNode->m_data, val);
        _avlInsert(rNode->m_pChild[side], val, ref);
        _balance(rNode);
    }

public:
    CAVL() : CBinaryTree<Traits>() {}

    void Insert(const value_type& val, ref_type ref = -1) override {
        std::lock_guard<std::recursive_mutex> lock(this->m_mutex);
        _avlInsert(this->m_pRoot, val, ref);
    }
};

#endif