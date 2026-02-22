#ifndef __AVL_H__
#define __AVL_H__

#include "binarytree.h"
#include <cassert>
#include <string>

using namespace std;

template <typename Traits>
class CAVLTree : public CBinaryTree<Traits> {
    using Base = CBinaryTree<Traits>;
    using Node = typename Base::Node;
    using value_type = typename Base::value_type;
    using CompareFunc = typename Base::CompareFunc;

    CompareFunc comp;

    
    static size_t getHeight(Node *node) {
        return node ? node->m_height : 0;
    }

    
    void updateHeight(Node *node) {
        if (!node) return;
        size_t leftH = getHeight(Base::getChild(node, 0));
        size_t rightH = getHeight(Base::getChild(node, 1));
        node->m_height = 1 + (leftH > rightH ? leftH : rightH);
    }

    
    int calculateBalance(Node *node) const {
        if (!node) return 0;
        return static_cast<int>(getHeight(Base::getChild(node, 0))) -
               static_cast<int>(getHeight(Base::getChild(node, 1)));
    }

    
    Node *rotateRight(Node *target) {
        Node *leftChild = Base::getChild(target, 0);
        if (!leftChild) return target;

        Node *leftRightChild = Base::getChild(leftChild, 1);
        Base::getChildRef(leftChild, 1) = target;
        Base::getChildRef(target, 0) = leftRightChild;

        updateHeight(target);
        updateHeight(leftChild);
        return leftChild;
    }

    
    Node *rotateLeft(Node *target) {
        Node *rightChild = Base::getChild(target, 1);
        if (!rightChild) return target;

        Node *rightLeftChild = Base::getChild(rightChild, 0);
        Base::getChildRef(rightChild, 0) = target;
        Base::getChildRef(target, 1) = rightLeftChild;

        updateHeight(target);
        updateHeight(rightChild);
        return rightChild;
    }

    
    Node *rebalanceNode(Node *node) {
        if (!node) return nullptr;

        updateHeight(node);
        int balance = calculateBalance(node);

        
        if (balance > 1 && calculateBalance(Base::getChild(node, 0)) >= 0) {
            return rotateRight(node);
        }

       
        if (balance > 1 && calculateBalance(Base::getChild(node, 0)) < 0) {
            Base::getChildRef(node, 0) = rotateLeft(Base::getChild(node, 0));
            return rotateRight(node);
        }

        
        if (balance < -1 && calculateBalance(Base::getChild(node, 1)) <= 0) {
            return rotateLeft(node);
        }

       
        if (balance < -1 && calculateBalance(Base::getChild(node, 1)) > 0) {
            Base::getChildRef(node, 1) = rotateRight(Base::getChild(node, 1));
            return rotateLeft(node);
        }

        return node;
    }

   
    Node *insertBalanced(Node *node, const value_type &val, ref_type ref) {
        if (!node) return new Node(val, ref);

        size_t dir = comp(val, node->GetValue());
        Base::getChildRef(node, dir) = insertBalanced(Base::getChild(node, dir), val, ref);
        return rebalanceNode(node);
    }

    
    Node *findMaxNode(Node *node) const {
        if (!node) return nullptr;
        while (Base::getChild(node, 1)) {
            node = Base::getChild(node, 1);
        }
        return node;
    }

    
    Node *removeBalanced(Node *node, const value_type &val, bool &removed, value_type &removedVal) {
        if (!node) return nullptr;

        if (node->GetValue() == val) {
            return removeAtNode(node, removed, removedVal);
        }

        size_t dir = comp(val, node->GetValue());
        Base::getChildRef(node, dir) = removeBalanced(Base::getChild(node, dir), val, removed, removedVal);
        return rebalanceNode(node);
    }

    
    Node *removeAtNode(Node *node, bool &removed, value_type &removedVal) {
        removed = true;
        removedVal = node->GetValue();

        
        if (!Base::getChild(node, 0) || !Base::getChild(node, 1)) {
            Node *child = Base::getChild(node, Base::getChild(node, 0) ? 0 : 1);
            delete node;
            return child;
        }

        
        Node *pred = findMaxNode(Base::getChild(node, 0));
        node->GetValueRef() = pred->GetValue();
        node->GetRefRef() = pred->GetRef();

        bool dummyRemoved = false;
        value_type dummyVal{};
        Base::getChildRef(node, 0) = removeBalanced(
            Base::getChild(node, 0),
            pred->GetValue(),
            dummyRemoved,
            dummyVal
        );

        return rebalanceNode(node);
    }

public:
    CAVLTree() = default;

    CAVLTree(const CAVLTree &other) : Base(other) {}

    CAVLTree(CAVLTree &&other) noexcept : Base(std::move(other)) {}

    CAVLTree &operator=(const CAVLTree &other) {
        Base::operator=(other);
        return *this;
    }

    CAVLTree &operator=(CAVLTree &&other) noexcept {
        Base::operator=(std::move(other));
        return *this;
    }

    
    void Insert(const value_type &val, ref_type ref) override {
        lock_guard<mutex> lock(this->mtx);
        this->m_pRoot = insertBalanced(this->m_pRoot, val, ref);
    }

    
    value_type remove(value_type &val) override {
        lock_guard<mutex> lock(this->mtx);
        bool removed = false;
        value_type removedVal{};
        this->m_pRoot = removeBalanced(this->m_pRoot, val, removed, removedVal);
        return removed ? removedVal : value_type{};
    }

    
    static size_t validateBalanceHeight(Node *node) {
        if (!node) return 0;

        size_t leftH = validateBalanceHeight(Base::getChild(node, 0));
        size_t rightH = validateBalanceHeight(Base::getChild(node, 1));

        int diff = static_cast<int>(leftH) - static_cast<int>(rightH);
        assert(diff >= -1 && diff <= 1);

        return 1 + (leftH > rightH ? leftH : rightH);
    }

    static void validateTreeBalance(CAVLTree &tree) {
        validateBalanceHeight(tree.m_pRoot);
    }
};

void DemoAVLTree();

#endif // __AVL_H__
