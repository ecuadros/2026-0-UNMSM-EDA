#ifndef __DOBLE_LINKED_LIST_H__
#define __DOBLE_LINKED_LIST_H__
#include "linkedlist.h"
//backward iterator
template<typename Node>
class backward_iterator{
public:
    using value_type = typename Node::value_type;
    using pointer    = value_type*;
    using reference  = value_type&;
private:
    Node* m_pNode;
public:
    backward_iterator(Node* node) : m_pNode(node) {}
    //sobrecargamos los operadores
    bool operator!=(const backward_iterator& other) const { return m_pNode != other.m_pNode; }
    bool operator==(const backward_iterator& other) const { return m_pNode != other.m_pNode; }

    // (*it)
    reference operator*() { return m_pNode->GetValueRef(); }
    //(++it)
    backward_iterator& operator++() {
        if (m_pNode) m_pNode = m_pNode->GetPrev();
        return *this;
    }
};
template<typename Traits>
class CDoubleLinkedList : public CLinkedList<Traits>{
public:
    using Node       = NodeLinkedList<Traits>;
    using value_type = typename Traits::value_type;
    using reverse_iterator = backward_iterator<Node>;

    CDoubleLinkedList() = default;
    //Constructor copia
    CDoubleLinkedList(const CDoubleLinkedList& other) {
        std::lock_guard <std::mutex> lock(other.m_mutex);
        Node* curr = other.m_pRoot;
        while(curr){
        this->push_back(curr->GetValueRef(), curr->GetRef());
        curr = curr->GetNext();
        }
    }
    //move Constructor
    CDoubleLinkedList(CDoubleLinkedList&& other) noexcept {
        std::lock_guard <std::mutex> lock(other.m_mutex);
        this->m_pRoot = other.m_pRoot;
        this->m_pLast = other.m_pLast;
        this->m_nElements = other.m_nElements;
        //dejamos la otra vacia
        other.m_pRoot = nullptr;
        other.m_pLast = nullptr;
        other.m_nElements = 0;
    }
    //Destructor
    virtual ~CDoubleLinkedList(){
        this -> clear();
    }
    //iterator
    reverse_iterator rbegin() { return reverse_iterator(this->m_pLast); }
    reverse_iterator rend() { return reverse_iterator(nullptr); }
    //insercion
    void push_back(const value_type &val, ref_type ref=0) {
        std::lock_guard <std::mutex> lock(this->m_mutex);
        Node *pNewNode = new Node(val, ref, nullptr, this->m_pLast);
        if (!this->m_pRoot) {
            this->m_pRoot = pNewNode;
        } else {
            this->m_pLast->GetNextRef() = pNewNode;
        } //el actual ultimo apunta al primero
            this->m_pLast = pNewNode;
            ++this->m_nElements;

    }
        //insertar ordenado
    void Insert(const value_type &val, ref_type ref = 0) {
        std::lock_guard <std::mutex> lock(this->m_mutex);
        InternalInsertDouble(this->m_pRoot, nullptr, val, ref);
    }
        //impresion reversa
    void PrintReverseDouble() {
        std::lock_guard <std::mutex> lock(this->m_mutex);
        cout << "[";
        Node* curr = this->m_pLast;
        while (curr) {
            cout << curr->GetValue() << " ";
            curr = curr->GetPrev();
        }
        cout << "]" << endl;
    }
protected:
    void InternalInsertDouble(Node *&rCurrent, Node *rPrevNode, const value_type &val, ref_type ref){
        if (!rCurrent || typename Traits::Func()(val, rCurrent->GetValue())) {
            //crear un nodo siguiente, su prev debe apuntar al nuevo
            Node *pNew = new Node(val, ref, rCurrent, rPrevNode);
            //si hay un nodo siguente debe apuntar al nuevo
            if (rCurrent) rCurrent->GetPrevRef() = pNew;
            else  this->m_pLast = pNew;
            //el puuntero del padre ahora apunta al nuevo
            rCurrent = pNew;
            ++this -> m_nElements;
            return;
        }
        InternalInsertDouble(rCurrent->GetNextRef(), rCurrent, val, ref);
    }

};
#endif