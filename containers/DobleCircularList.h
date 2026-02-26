#ifndef __DOBLE_CIRUCLAR_lIST_H__
#define __DOBLE_CIRUCLAR_lIST_H__
#include "DobleLinkedList.h"

template<typename Traits>
class CDoubleCircularList : public CDoubleLinkedList<Traits>{
public:
    using Node = typename CDoubleLinkedList<Traits>::Node;
    using value_type =typename Traits::value_type;

    CDoubleCircularList() = default;

    ~CDoubleCircularList(){
        std::lock_guard<std::mutex> lock(this->m_mutex);
        BreakCircle();
    }
    void push_back(const value_type &val, ref_type ref=0){
        std::lock_guard<std::mutex> lock(this->m_mutex);
        BreakCircle();

        Node *pNewNode = new Node(val, ref, nullptr, this->m_pLast);
        if (!this->m_pRoot) { this->m_pRoot = pNewNode; }
        else this->m_pLast->GetNextRef() = pNewNode;
        this->m_pLast = pNewNode;
        ++this->m_nElements;
        CloseCircle();
    }
    void Insert(const value_type &val, ref_type ref=0){
        std::lock_guard<std::mutex> lock(this->m_mutex);
        BreakCircle();
        this->InternalInsertDouble(this->m_pRoot, nullptr, val, ref);
        CloseCircle();
    }
    friend ostream &operator<<(ostream &os, CDoubleCircularList<Traits> &container){
        os << "CDoubleCircularList: size = " << container.getSize() << endl;
        os << "[";
        if(container.m_pRoot){
            Node* pNode =container.m_pRoot; //declaracion pNode
            do{
                os<<pNode-> GetValue()<<" ";
                pNode = pNode->GetNext();
            }while (pNode != container.m_pRoot && pNode != nullptr);
        }
        os<<"]"<<endl;
        return os;
    }
private:
    void BreakCircle(){
        if(this->m_pLast && this->m_pRoot){
            this->m_pLast->GetNextRef() = nullptr;
            this->m_pRoot->GetPrevRef() = nullptr;
        }
    }
    void CloseCircle(){
        if(this->m_pLast && this->m_pRoot){
            this->m_pLast->GetNextRef() = this->m_pRoot;
            this->m_pRoot->GetPrevRef() = this->m_pLast;
        }
    }
};

#endif