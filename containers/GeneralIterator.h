#ifndef __GENERAL_ITERATOR_H__
#define __GENERAL_ITERATOR_H__
#include <iostream>
#include "../util.h"

using namespace std;

template <typename Container>
struct GeneralIterator
{ 
  protected:
    using value_type  = typename Container::value_type;
    using Node        = typename Container::Node;

    Container  *m_pContainer = nullptr;
    Size        m_pos        = -1;
    Node       *m_data       = nullptr;
  public:
    GeneralIterator(Container *pContainer, Size pos=0, Node *pm_data = nullptr) 
         : m_pContainer(pContainer), m_pos(pos), m_data(pm_data) {
         }
    GeneralIterator(GeneralIterator<Container> &another)
         :  m_pContainer(another.m_pContainer),
            m_pos  (another.m_pos),
            m_data (another.m_data)
    {}
    virtual ~GeneralIterator(){};
    
    virtual bool operator!=(const GeneralIterator<Container> &another){
        return m_pContainer != another.m_pContainer ||
               m_pos        != another.m_pos;         
    }
    virtual value_type &operator*() = 0;

    friend ostream &operator<<(ostream &os, GeneralIterator<Container> &iterator){
        os << *iterator << endl;
        return os;
    }

};

#endif // __GENERAL_ITERATOR_H__
