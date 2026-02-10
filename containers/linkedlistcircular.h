#ifndef __LD_CIRCULAR__H__
#define __LD_CIRCULAR__H__
#include "linkedlist.h"
template<typename Traits>
class CCircularList : public CLinkedList<Traits>{
public:
    using Node = typename CLinkedList<Traits>::Node;
    using value_type = typename CLinkedList<Traits>::value_type;

    CCircularList()= default;
    //destructor
    ~CCircularList(){
        std::lock_guard<std::mutex> lock(this->m_mutex);
        if(this -> m_pLast){
            //rompemos el ciclo antes que el destructor del padre actue
            this->m_pLast->GetNextRef() = nullptr;
        }
    }
void push_back(const value_type &val, ref_type ref=0){
        std::lock_guard<std::mutex> lock(this->m_mutex);
        //abrir
        if(this->m_pLast)   this -> m_pLast->GetNextRef()=nullptr;
        //insertar
        Node *pNewNode = new Node(val, ref);
        if( !this->m_pRoot )
            this->m_pRoot = pNewNode;
        else
            this->m_pLast->GetNextRef() = pNewNode;
        this->m_pLast = pNewNode;
        ++this->m_nElements;
        //cerrar
        if(this->m_pLast)   this -> m_pLast->GetNextRef()=this->m_pRoot;
    }
void Insert(const value_type &val, ref_type ref=0){
        std::lock_guard<std::mutex> lock(this->m_mutex);
        //romper el circulo
        if(this->m_pLast)   this -> m_pLast->GetNextRef()=nullptr;
        //insertar
        this->InternalInsert(this->m_pRoot, val, ref);
        //restaurar el circulo
        if(this->m_pLast)   this -> m_pLast->GetNextRef()=this->m_pRoot;

    }
    friend ostream &operator<<(ostream &os, CCircularList<Traits> &container){
        os << "CCircularList: size = " << container.getSize() << endl;
        os << "[";
        if(container.m_pRoot){
            Node* pNode =container.m_pRoot; //declaracion pNode
            do{
                os<<pNode-> GetValue()<<" ";
                pNode = pNode->GetNext();
            }while (pNode != container.m_pRoot && pNode != nullptr);
            os<<"]"<<endl;
        }
        return os;
    }
};







#endif