#ifndef __GENERAL_ITERATOR_H__
#define __GENERAL_ITERATOR_H__
#include "../util.h"

template <typename Container>
struct GeneralIterator
{ public:
    using value_type  = typename Container::value_type;
    using Node        = typename Container::Node;

    Container  *m_pContainer = nullptr;
    Node       *m_pCurrent       = nullptr;
    Size        m_pos        = -1;
  public:
    GeneralIterator(Container *pContainer, Size pos=0) 
         : m_pContainer(pContainer) {
           m_pCurrent = m_pContainer->m_data;
           m_pos = pos;
         }
    GeneralIterator(GeneralIterator<Container> &another)
         :  m_pContainer(another.m_pContainer),
            m_pCurrent (another.m_pCurrent),
            m_pos  (another.m_pos)
    {}
    virtual ~GeneralIterator(){};

    virtual bool operator!=(const GeneralIterator<Container>& another) const {
        return m_pCurrent != another.m_pCurrent;
    }

    virtual value_type& operator*() {
        return m_pCurrent->GetValueRef();
    }
};

template <typename Container>
class LinkedListForwardIterator : public GeneralIterator<Container> {
public:
    using Parent = GeneralIterator<Container>;
    using value_type = typename Container::value_type;

    LinkedListForwardIterator(Container* pContainer, typename Container::Node* pNode = nullptr)
        : Parent(pContainer, pNode) {}

    LinkedListForwardIterator& operator++() {
        if (this->m_pCurrent) {
            this->m_pCurrent = this->m_pCurrent->GetNext();
        }
        return *this;
    }

    LinkedListForwardIterator operator++(int) {
        LinkedListForwardIterator temp = *this;
        ++(*this);
        return temp;
    }

    value_type& operator*() {
        return this->m_pCurrent->GetValueRef();
    }
    
    bool operator!=(const LinkedListForwardIterator& another) const {
        return this->m_pCurrent != another.m_pCurrent;
    }
};

#endif // __GENERAL_ITERATOR_H__
