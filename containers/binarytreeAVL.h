#ifndef __BINARYTREE_AVL_H__
#define __BINARYTREE_AVL_H__

#include "binarytree.h"
#include <algorithm>

template <typename Traits>
class CBinaryTreeAVL : public CBinaryTree<Traits>{
public:
    using Base = CBinaryTree<Traits>;
    using value_type  = typename Base::value_type;
    using Node        = typename Base::Node;
    using CompareFunc = typename Base::CompareFunc;

protected:
    using Base::m_mutex;
    using Base::m_pRoot;
    using Base::m_size;
    using Base::comp;

protected:
    Size GetNodeHeight(const Node *pNode) const;
    int GetBalanceFactor(const Node *pNode) const;
    void UpdateHeight(Node *pNode);

    void RotateRight(Node *&pNode);
    void RotateLeft(Node *&pNode);  
    void ApplyBalance(Node *&pNode);

    void InternalInsertAVL(Node *&pCurrent, Node *pParent, const value_type &val, ref_type ref);
    void InternalRemoveAVL(Node *&pCurrent, const value_type &val, bool &removed);
    void InternalInsertFromNode(const Node *pNode);

    Node *FirstInSubTree(Node *pNode) const;
    Size  ReBuildHeights(Node *pNode);

public:

    CBinaryTreeAVL() = default;
    CBinaryTreeAVL(const CBinaryTree<Traits>& another); // contruir AVL desde BST normal
    CBinaryTreeAVL(const CBinaryTreeAVL& another); // copy constructor
    CBinaryTreeAVL &operator=(const CBinaryTreeAVL& another); // copy assignment
    CBinaryTreeAVL(CBinaryTreeAVL &&another) noexcept: Base(std::move(another)) {} // move constructor
    CBinaryTreeAVL &operator=(CBinaryTreeAVL &&another) noexcept{ // move assignment
        Base::operator=(std::move(another));
        return *this;
    }
    
    void Insert(const value_type &val, ref_type ref){
        std::lock_guard<std::mutex> lock(m_mutex);
        InternalInsertAVL(m_pRoot, nullptr, val, ref);
    }

    bool Remove(const value_type &val){
        std::lock_guard<std::mutex> lock(m_mutex);
        bool removed = false;
        InternalRemoveAVL(m_pRoot, val, removed);
        
        if(removed){ 
            --m_size;
            if(m_pRoot) m_pRoot->GetParentRef() = nullptr; // aseguramos que el root no tenga padre
        }
        return removed;
    }

    // Útil para Fase 5: recalcular alturas si lees desde stream/base
    void FixHeights(){
        std::lock_guard<std::mutex> lock(m_mutex);
        ReBuildHeights(m_pRoot);
    }

    void Read(std::istream &is){
        Base::Read(is); // lee como un BST normal
        FixHeights();   // luego recalcula alturas para cumplir propiedad AVL
    }

    void Write(std::ostream &os) const{
        Base::Write(os); // escribe como un BST normal
    }

    friend std::ostream &operator<<(std::ostream &os, const CBinaryTreeAVL &t){
        t.Write(os);
        return os;
    }
    
    friend std::istream &operator>>(std::istream &is, CBinaryTreeAVL &t){
        t.Read(is);
        return is;
    }

};

template <typename Traits>
Size CBinaryTreeAVL<Traits>::GetNodeHeight(const Node *pNode) const {
    return  pNode ? pNode->GetHeight() : 0;
}

template <typename Traits>
int CBinaryTreeAVL<Traits>::GetBalanceFactor(const Node *pNode) const{
    if(!pNode) return 0;

    int hLeft  = static_cast<int>(GetNodeHeight(pNode->GetChild(0)));
    int hRight = static_cast<int>(GetNodeHeight(pNode->GetChild(1)));

    return hLeft - hRight;  
}

template <typename Traits>
void CBinaryTreeAVL<Traits>::UpdateHeight(Node *pNode){
    if(!pNode) return;

    Size hLeft  = GetNodeHeight(pNode->GetChild(0));
    Size hRight = GetNodeHeight(pNode->GetChild(1));

    pNode->GetHeightRef() = std::max(hLeft, hRight) + 1;
}

template <typename Traits>
void CBinaryTreeAVL<Traits>::RotateLeft(Node *&pNode){
    if(!pNode) return;

    Node *pOldRoot = pNode; //subarbol actual
    Node *pNewRoot = pOldRoot->GetChild(1); //hijo derecho sube

    if(!pNewRoot) return; // no se puede rotar si no hay hijo derecho

    Node *pParent = pOldRoot->GetParent(); // padre del subarbol actual
    Node *pBeta = pNewRoot->GetChild(0);

    pOldRoot->GetChildRef(1) = pBeta; // beta baja a ser hijo derecho de old root
    if(pBeta) pBeta->GetParentRef() = pOldRoot;

    pNewRoot->GetChildRef(0) = pOldRoot; // old root sube a ser hijo izquierdo de new root
    pNewRoot->GetParentRef() = pParent; // new root se conecta con el padre del subarbol actual(baja)
    pOldRoot->GetParentRef() = pNewRoot; 

    pNode = pNewRoot; // el nuevo root del subarbol es new root

    UpdateHeight(pOldRoot);
    UpdateHeight(pNewRoot); 

}

template <typename Traits>
void CBinaryTreeAVL<Traits>::RotateRight(Node *&pNode){
    if(!pNode) return;

    Node *pOldRoot = pNode; //subarbol actual
    Node *pNewRoot = pOldRoot->GetChild(0); //hijo izquierdo sube

    if(!pNewRoot) return; // no se puede rotar si no hay hijo izquierdo

    Node *pParent = pOldRoot->GetParent(); // padre del subarbol actual
    Node *pBeta = pNewRoot->GetChild(1);

    pOldRoot->GetChildRef(0) = pBeta; // beta baja a ser hijo izquierdo de old root
    if(pBeta) pBeta->GetParentRef() = pOldRoot;

    pNewRoot->GetChildRef(1) = pOldRoot; // old root sube a ser hijo derecho de new root
    pNewRoot->GetParentRef() = pParent; // new root se conecta con el padre del subarbol actual(baja)
    pOldRoot->GetParentRef() = pNewRoot; 

    pNode = pNewRoot; // el nuevo root del subarbol es new root

    UpdateHeight(pOldRoot);
    UpdateHeight(pNewRoot);
    
}

template <typename Traits>
void CBinaryTreeAVL<Traits>::ApplyBalance(Node *&pNode){
    if(!pNode) return;

    UpdateHeight(pNode); // primero actualizamos altura del nodo actual
    int balanceo = GetBalanceFactor(pNode);
    
    if(balanceo > 1){ // subarbol izquierdo pesado
        if(GetBalanceFactor(pNode->GetChild(0)) < 0){ // caso izq-dere
            RotateLeft(pNode->GetChildRef(0));
        }
        RotateRight(pNode); // rotación derecha en el nodo actual
    
    } else if(balanceo < -1){ // subarbol derecho pesado
        if(GetBalanceFactor(pNode->GetChild(1)) > 0){ // caso dere-izq
            RotateRight(pNode->GetChildRef(1)); // rotación derecha en el hijo derecho
        }
        RotateLeft(pNode); // rotación izquierda en el nodo actual
    }
}

template <typename Traits>
typename CBinaryTreeAVL<Traits>::Node *CBinaryTreeAVL<Traits>::FirstInSubTree(Node *pNode)const{
    if(!pNode) return nullptr;
    while(pNode->GetChild(0)){
        pNode = pNode->GetChild(0);
    }
        return pNode;
}

template <typename Traits>
Size CBinaryTreeAVL<Traits>::ReBuildHeights(Node *pNode){
    if(!pNode) return 0;

    Size hLeft = ReBuildHeights(pNode->GetChild(0));
    Size hRight = ReBuildHeights(pNode->GetChild(1));

    pNode->GetHeightRef() = std::max(hLeft, hRight) + 1;
    return pNode->GetHeight();
}

template <typename Traits>
void CBinaryTreeAVL<Traits>::InternalInsertFromNode(const Node *pNode){
    if(!pNode) return;

    Insert(pNode->GetValue(), pNode->GetRef());
    InternalInsertFromNode(pNode->GetChild(0));
    InternalInsertFromNode(pNode->GetChild(1));
}

template <typename Traits>
CBinaryTreeAVL<Traits>::CBinaryTreeAVL(const Base &another){
    InternalInsertFromNode(another.GetRoot());
}

template <typename Traits>
CBinaryTreeAVL<Traits>::CBinaryTreeAVL(const CBinaryTreeAVL &another){
    std::lock_guard<std::mutex> lock(another.m_mutex);
    InternalInsertFromNode(another.GetRoot());
}

template <typename Traits>
CBinaryTreeAVL<Traits> &
CBinaryTreeAVL<Traits>::operator=(const CBinaryTreeAVL &another){
    if(this == &another) return *this;

    CBinaryTreeAVL tmp(another);     // copia segura en temporal
    Base::operator=(std::move(tmp)); // mueve el temporal hacia *this
    return *this;
}

template <typename Traits>
void CBinaryTreeAVL<Traits>::InternalInsertAVL(Node *&pCurrent, Node *pParent, const value_type &val, ref_type ref){
    if(!pCurrent){
        pCurrent = new Node(val, ref, pParent);
        ++m_size;
        return;
    }
    size_t branch = static_cast<size_t>(comp(val, pCurrent->GetValue()));

    InternalInsertAVL(pCurrent->GetChildRef(branch), pCurrent, val, ref);

    ApplyBalance(pCurrent); // balanceamos el subarbol actual después de la inserción

    if(pCurrent)
        pCurrent->GetParentRef() = pParent; // aseguramos que el nodo actual tenga el padre correcto
}

template <typename Traits>
void CBinaryTreeAVL<Traits>::InternalRemoveAVL(Node *&pCurrent, const value_type &val, bool &removed){
    if(!pCurrent) return;

    if(val != pCurrent->GetValue()){
        size_t branch = static_cast<size_t>(comp(val, pCurrent->GetValue()));
        InternalRemoveAVL(pCurrent->GetChildRef(branch), val, removed);
        
        if(pCurrent && pCurrent->GetChild(branch)){ 
            pCurrent->GetChild(branch)->GetParentRef() = pCurrent; 
        }
    }else{
        Node *pLeft = pCurrent->GetChild(0);
        Node *pRight = pCurrent->GetChild(1);

        if(!pLeft || !pRight){
            Node *pDelete = pCurrent;
            Node *pChild = (pLeft ? pLeft : pRight);

            if(pChild){
                pChild->GetParentRef() = pCurrent->GetParent();
            }
            pCurrent = pChild; // reemplazamos el nodo actual por su hijo (o nullptr si no tiene hijos)

            pDelete->GetChildRef(0) = nullptr;
            pDelete->GetChildRef(1) = nullptr;
            pDelete->GetParentRef() = nullptr;

            delete pDelete;
            removed = true;
        }else{
            Node *pSucc = FirstInSubTree(pRight); // sucesor inorden (mínimo en el subárbol derecho)

            pCurrent->GetValueRef() = pSucc->GetValue(); // copiamos el valor del sucesor al nodo actual
            pCurrent->GetRefRef() = pSucc->GetRef(); // copiamos la referencia del sucesor al nodo actual

            InternalRemoveAVL(pCurrent->GetChildRef(1), pSucc->GetValue(), removed); 

            if(pCurrent->GetChild(1)){ 
                pCurrent->GetChild(1)->GetParentRef() = pCurrent; 
            }
        }
    }
    if(!pCurrent) return; // si el nodo actual fue eliminado, no hay que balancear

    ApplyBalance(pCurrent);

    if(pCurrent->GetChild(0)) pCurrent->GetChild(0)->GetParentRef() = pCurrent; // aseguramos que el hijo izquierdo tenga el padre correcto
    if(pCurrent->GetChild(1)) pCurrent->GetChild(1)->GetParentRef() = pCurrent; // aseguramos que el hijo derecho tenga el padre correcto
}
    

#endif // __BINARYTREE_AVL_H__


