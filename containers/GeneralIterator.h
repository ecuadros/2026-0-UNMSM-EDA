#ifndef __GENERAL_ITERATOR_H__
#define __GENERAL_ITERATOR_H__
#include "../util.h"

template <typename Container>
struct GeneralIterator
{ public:
    using value_type  = typename Container::value_type;
    using Node        = typename Container::Node;

    Container  *m_pContainer = nullptr;
    Node       *m_pNode       = nullptr;
    Size        m_pos        = -1;
  public:
    GeneralIterator(Container *pContainer, Size pos=0) 
         : m_pContainer(pContainer), m_pos(pos) {}

    GeneralIterator(GeneralIterator<Container> &another)
         :  m_pContainer(another.m_pContainer),
            m_pNode (another.m_pNode),
            m_pos  (another.m_pos)
    {}
    virtual ~GeneralIterator(){};
    
    bool operator!=(const GeneralIterator<Container> &another){
        return m_pContainer != another.m_pContainer ||
               m_pos        != another.m_pos;         
    }
    virtual value_type &operator*(){
      static value_type dummy; 
      return dummy;
    }
};

#endif // __GENERAL_ITERATOR_H__
