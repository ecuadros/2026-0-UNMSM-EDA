#ifndef __ARBOL_2_3__H
#define __ARBOL_2_3__H
#include <mutex>
#include "../general/types.h"

template <typename Traits>
class Node23{
public:
    using value_type = typename Traits::value_type;

    value_type keys[8];

    Node23* children[9];
    Node23* parent;
    int count;
    bool isLeaf;

    //constructor del nodo
    Node23(bool leaf =true) {
        parent = nullptr;
        isLeaf = leaf;
        count = 0;
        for (int i = 0; i < 9; ++i) {
            children[i] = nullptr;
        }
    }
};
//arbol 2-3
template <typename Traits>
class Arbol23 {
public:
    using value_type = typename Traits::value_type;
    using Node = Node23<Traits>;
    using CompareFunc = typename Traits::CompareFunc;

protected:
    Node* m_pRoot = nullptr;
    mutable std::mutex mutex;
    CompareFunc comp;

    //destructor
    void DestroyTree(Node* pNode) {
        if(pNode){
            if(!pNode->isLeaf){
                for(int i=0; i<pNode->count; ++i){
                    DestroyTree(pNode->children[i]);
                }
            }
            delete pNode;
        }
    }

    //insercion en la hoja
    void InsertIntoNode(Node* node, const value_type& val){
        int i = node->count-1;
        while(i>=0 && comp(node -> keys[i], val)){
            node -> keys[i + 1]= node -> keys[i];
            --i;
        }
        node -> keys[i+1] = val;
        node-> count++;
    }
    void InternalInsert(Node* node, const value_type& val){
        if(node->isLeaf){
            InsertIntoNode(node, val);
            CheckOverflow(node);
        }else{
            int i= 0;
            while(i<node->count && comp(val,node->keys[i])){
                ++i;
            }
            InternalInsert(node->children[i], val);
        }
    }
    void CheckOverflow(Node* node) {
        //Raíz llega a 8
        if (node == m_pRoot && node->count == 8) {
            SplitMegaRoot(node);
            return;
        }
        // Nodo normal llega a 4
        if (node != m_pRoot && node->count == 4) {
            HandleNodeOverflow(node);
        }
    }
    void HandleNodeOverflow(Node* node) {
        Node* parent = node->parent;

        int myIndex = 0;
        while (myIndex <= parent->count && parent->children[myIndex] != node) {
            ++myIndex;
        }

        if (myIndex > 0) {
            Node* leftSibling = parent->children[myIndex - 1];

            if (leftSibling->count < 3) {
                GiveToLeftSibling(node, parent, myIndex);
            } else {
                MegaMergeSplit(leftSibling, node, parent, myIndex);
            }
        } else {
            SplitStandardNode(node, parent, myIndex);
        }
    }
    void GiveToLeftSibling(Node* node, Node* parent, int myIndex) {
        Node* leftSibling = parent->children[myIndex - 1];

        leftSibling->keys[leftSibling->count] = parent->keys[myIndex - 1];
        leftSibling->children[leftSibling->count + 1] = node->children[0];
        if (leftSibling->children[leftSibling->count + 1]) {
            leftSibling->children[leftSibling->count + 1]->parent = leftSibling;
        }
        leftSibling->count++;

        parent->keys[myIndex - 1] = node->keys[0];

        for (int i = 0; i < node->count - 1; i++) node->keys[i] = node->keys[i + 1];
        for (int i = 0; i < node->count; i++) node->children[i] = node->children[i + 1];
        node->count--;
    }
    void MegaMergeSplit(Node* leftSibling, Node* node, Node* parent, int myIndex) {
        value_type pool[8];
        pool[0] = leftSibling->keys[0]; pool[1] = leftSibling->keys[1]; pool[2] = leftSibling->keys[2];
        pool[3] = parent->keys[myIndex - 1]; // El padre baja
        pool[4] = node->keys[0]; pool[5] = node->keys[1]; pool[6] = node->keys[2]; pool[7] = node->keys[3];

        Node* childPool[9] = {nullptr};
        if (!node->isLeaf) {
            for(int i=0; i<4; i++) childPool[i] = leftSibling->children[i];
            for(int i=0; i<5; i++) childPool[i+4] = node->children[i];
        }

        value_type up1 = pool[2];
        value_type up2 = pool[5];

        leftSibling->count = 2;
        leftSibling->keys[0] = pool[0]; leftSibling->keys[1] = pool[1];

        Node* midNode = new Node(node->isLeaf);
        midNode->parent = parent;
        midNode->count = 2;
        midNode->keys[0] = pool[3]; midNode->keys[1] = pool[4];

        node->count = 2;
        node->keys[0] = pool[6]; node->keys[1] = pool[7];

        if (!node->isLeaf) {
            leftSibling->children[0] = childPool[0]; leftSibling->children[1] = childPool[1]; leftSibling->children[2] = childPool[2];
            midNode->children[0] = childPool[3]; if(childPool[3]) childPool[3]->parent = midNode;
            midNode->children[1] = childPool[4]; if(childPool[4]) childPool[4]->parent = midNode;
            midNode->children[2] = childPool[5]; if(childPool[5]) childPool[5]->parent = midNode;
            node->children[0] = childPool[6]; if(childPool[6]) childPool[6]->parent = node;
            node->children[1] = childPool[7]; if(childPool[7]) childPool[7]->parent = node;
            node->children[2] = childPool[8]; if(childPool[8]) childPool[8]->parent = node;
        }

        parent->keys[myIndex - 1] = up1;

        for (int i = parent->count; i > myIndex; i--) parent->keys[i] = parent->keys[i - 1];
        for (int i = parent->count + 1; i > myIndex; i--) parent->children[i] = parent->children[i - 1];

        parent->keys[myIndex] = up2;
        parent->children[myIndex] = midNode;
        parent->count++;

        CheckOverflow(parent);
    }
    //split normal
    void SplitStandardNode(Node* node, Node* parent, int myIndex) {
        Node* rightNode = new Node(node->isLeaf);
        rightNode->parent = parent;
        rightNode->keys[0] = node->keys[3];
        rightNode->count = 1;

        if (!node->isLeaf) {
            rightNode->children[0] = node->children[3]; if(rightNode->children[0]) rightNode->children[0]->parent = rightNode;
            rightNode->children[1] = node->children[4]; if(rightNode->children[1]) rightNode->children[1]->parent = rightNode;
        }

        value_type upVal = node->keys[2];
        node->count = 2;

        for (int i = parent->count; i > myIndex; i--) parent->keys[i] = parent->keys[i - 1];
        for (int i = parent->count + 1; i > myIndex + 1; i--) parent->children[i] = parent->children[i - 1];

        parent->keys[myIndex] = upVal;
        parent->children[myIndex + 1] = rightNode;
        parent->count++;

        CheckOverflow(parent);
    }
    void SplitMegaRoot(Node* root) {
        Node* newRoot = new Node(false);

        Node* leftChild = new Node(root->isLeaf);
        Node* midChild = new Node(root->isLeaf);
        Node* rightChild = new Node(root->isLeaf);

        leftChild->keys[0] = root->keys[0]; leftChild->keys[1] = root->keys[1]; leftChild->count = 2;
        midChild->keys[0] = root->keys[3]; midChild->keys[1] = root->keys[4]; midChild->count = 2;
        rightChild->keys[0] = root->keys[6]; rightChild->keys[1] = root->keys[7]; rightChild->count = 2;

        if (!root->isLeaf) {
            leftChild->children[0] = root->children[0]; if(leftChild->children[0]) leftChild->children[0]->parent = leftChild;
            leftChild->children[1] = root->children[1]; if(leftChild->children[1]) leftChild->children[1]->parent = leftChild;
            leftChild->children[2] = root->children[2]; if(leftChild->children[2]) leftChild->children[2]->parent = leftChild;

            midChild->children[0] = root->children[3]; if(midChild->children[0]) midChild->children[0]->parent = midChild;
            midChild->children[1] = root->children[4]; if(midChild->children[1]) midChild->children[1]->parent = midChild;
            midChild->children[2] = root->children[5]; if(midChild->children[2]) midChild->children[2]->parent = midChild;

            rightChild->children[0] = root->children[6]; if(rightChild->children[0]) rightChild->children[0]->parent = rightChild;
            rightChild->children[1] = root->children[7]; if(rightChild->children[1]) rightChild->children[1]->parent = rightChild;
            rightChild->children[2] = root->children[8]; if(rightChild->children[2]) rightChild->children[2]->parent = rightChild;
        }

        newRoot->keys[0] = root->keys[2];
        newRoot->keys[1] = root->keys[5];
        newRoot->count = 2;

        newRoot->children[0] = leftChild; leftChild->parent = newRoot;
        newRoot->children[1] = midChild;  midChild->parent = newRoot;
        newRoot->children[2] = rightChild; rightChild->parent = newRoot;

        m_pRoot = newRoot;
        delete root;
    }
    //imprimir:
    void PrintNode(Node* node, std::string prefix = "", bool isTail = true) const {
        if (!node) return;
        std::cout << prefix << (isTail ? "\\-- " : "|-- ") << "[";
        for (int i = 0; i < node->count; i++) {
            std::cout << node->keys[i] << (i < node->count - 1 ? "|" : "");
        }
        std::cout << "]\n";

        if (!node->isLeaf) {
            for (int i = 0; i <= node->count; i++) {
                PrintNode(node->children[i], prefix + (isTail ? "    " : "|   "), i == node->count);
            }
        }
    }
public:
    //Constructor y Destructor
    Arbol23() {}

    virtual ~Arbol23() {
        std::lock_guard<std::mutex> lock(mutex);
        DestroyTree(m_pRoot);
    }

    // --- MÉTODO INSERT PÚBLICO ---
    void Insert(const value_type& val) {
        std::lock_guard<std::mutex> lock(mutex);

        if (m_pRoot == nullptr) {
            m_pRoot = new Node(true);
            m_pRoot->keys[0] = val;
            m_pRoot->count = 1;
            return;
        }
        InternalInsert(m_pRoot, val);
    }
    template<typename U>
    friend std::ostream& operator<<(std::ostream& os, const Arbol23<U>& tree) {
        std::lock_guard<std::mutex> lock(tree.mutex);
        os << "Arbol 2-3: \n";
        tree.PrintNode(tree.m_pRoot);
        return os;
    }
};
#endif