#ifndef __AVL_H__
#define __AVL_H__

#include "binarytree.h"

// Traits
template <typename T, typename _Func, typename _Ref = long>
struct AVLTrait : public BTreeTrait<T, _Func, _Ref> {
};

template <typename T, typename _Ref = long>
struct AscendingAVLTrait : public BTreeTrait<T, std::less<T>, _Ref> {
};

template <typename T, typename _Ref = long>
struct DescendingAVLTrait : public BTreeTrait<T, std::greater<T>, _Ref> {
};

// Forward declarations
template <typename Traits> class CAVL;
template <typename Traits> ostream &operator<<(ostream &os, CAVL<Traits> &container);
template <typename Traits> istream &operator>>(istream &is, CAVL<Traits> &container);

// CAVL
template <typename Traits>
class CAVL : public CBTree<Traits> {
public:
    using  value_type = typename Traits::value_type;
    using  ref_type   = typename Traits::ref_type;
    using  Node       = NodeBTree<Traits>;

protected:
    using CBTree<Traits>::m_pRoot;
    using CBTree<Traits>::m_nElements;
    using CBTree<Traits>::m_mtx;
    using CBTree<Traits>::m_comp;

public:
    CAVL();
    CAVL(const CAVL &otro);
    CAVL(CAVL &&otro) noexcept;
    virtual ~CAVL();

    // operator<<
    friend ostream &operator<< <>(ostream &os, CAVL<Traits> &container);

    // operator>>
    friend istream &operator>> <>(istream &is, CAVL<Traits> &container);

protected:
    int Height(Node *pNode);
    int BalanceFactor(Node *pNode);

    void RotateRight(Node *&rNode);
    void RotateLeft(Node *&rNode);
    void RotateLeftRight(Node *&rNode);
    void RotateRightLeft(Node *&rNode);

    void InternalBalance(Node *&rNode);

    void InternalInsert(Node *&rParent, const value_type &val, ref_type ref, Node *pParent) override;
    void InternalRemove(Node *&rNode, const value_type &val) override;
};

// constructor
template <typename Traits>
CAVL<Traits>::CAVL() : CBTree<Traits>() {}

// constructor copia
template <typename Traits>
CAVL<Traits>::CAVL(const CAVL &otro) : CBTree<Traits>(otro) {}

// move constructor
template <typename Traits>
CAVL<Traits>::CAVL(CAVL &&otro) noexcept : CBTree<Traits>(std::move(otro)) {}

// destructor
template <typename Traits>
CAVL<Traits>::~CAVL() {}

// Height
template <typename Traits>
int CAVL<Traits>::Height(Node *pNode) {
    if (!pNode) return -1;
    int hLeft = Height(pNode->GetLeft());
    int hRight = Height(pNode->GetRight());
    return 1 + (hLeft > hRight ? hLeft : hRight);
}

// BalanceFactor
template <typename Traits>
int CAVL<Traits>::BalanceFactor(Node *pNode) {
    if (!pNode) return 0;
    return Height(pNode->GetLeft()) - Height(pNode->GetRight());
}

// RotateRight
template <typename Traits>
void CAVL<Traits>::RotateRight(Node *&rNode) {
    Node *pLeft = rNode->GetLeft();
    rNode->GetLeftRef() = pLeft->GetRight();
    if (pLeft->GetRight())
        pLeft->GetRight()->GetParentRef() = rNode;
    pLeft->GetRightRef() = rNode;
    pLeft->GetParentRef() = rNode->GetParent();
    rNode->GetParentRef() = pLeft;
    rNode = pLeft;
}

// RotateLeft
template <typename Traits>
void CAVL<Traits>::RotateLeft(Node *&rNode) {
    Node *pRight = rNode->GetRight();
    rNode->GetRightRef() = pRight->GetLeft();
    if (pRight->GetLeft())
        pRight->GetLeft()->GetParentRef() = rNode;
    pRight->GetLeftRef() = rNode;
    pRight->GetParentRef() = rNode->GetParent();
    rNode->GetParentRef() = pRight;
    rNode = pRight;
}

// RotateLeftRight
template <typename Traits>
void CAVL<Traits>::RotateLeftRight(Node *&rNode) {
    RotateLeft(rNode->GetLeftRef());
    RotateRight(rNode);
}

// RotateRightLeft
template <typename Traits>
void CAVL<Traits>::RotateRightLeft(Node *&rNode) {
    RotateRight(rNode->GetRightRef());
    RotateLeft(rNode);
}

// InternalBalance
template <typename Traits>
void CAVL<Traits>::InternalBalance(Node *&rNode) {
    if (!rNode) return;
    int bf = BalanceFactor(rNode);
    if (bf > 1) {
        if (BalanceFactor(rNode->GetLeft()) >= 0)
            RotateRight(rNode);
        else
            RotateLeftRight(rNode);
    } else if (bf < -1) {
        if (BalanceFactor(rNode->GetRight()) <= 0)
            RotateLeft(rNode);
        else
            RotateRightLeft(rNode);
    }
}

// InternalInsert
template <typename Traits>
void CAVL<Traits>::InternalInsert(Node *&rParent, const value_type &val, ref_type ref, Node *pParent) {
    CBTree<Traits>::InternalInsert(rParent, val, ref, pParent);
    InternalBalance(rParent);
}

// InternalRemove
template <typename Traits>
void CAVL<Traits>::InternalRemove(Node *&rNode, const value_type &val) {
    CBTree<Traits>::InternalRemove(rNode, val);
    InternalBalance(rNode);
}

// operator<<
template <typename Traits>
ostream &operator<<(ostream &os, CAVL<Traits> &container) {
    os << "CAVL: size = " << container.getSize() << endl;
    container.InternalPrint(os, container.m_pRoot, "", false);
    return os;
}

// operator>>
template <typename Traits>
istream &operator>>(istream &is, CAVL<Traits> &container) {
    using value_type = typename Traits::value_type;
    using ref_type   = typename Traits::ref_type;
    size_t size;
    if( is >> size ){
        value_type value;
        ref_type ref;
        for( size_t i = 0; i < size; ++i ){
            is >> value >> ref;
            container.Insert(value, ref);
        }
    }
    return is;
}

#endif // __AVL_H__
