#ifndef __GENERAL_ITERATOR_H__
#define __GENERAL_ITERATOR_H__
#include "util.h"
#include <iterator>

template <typename Container>
struct GeneralIterator
{ public:
    using value_type  = typename Container::value_type;
    using Node        = typename Container::Node;
  using reference   = value_type&;
  using pointer     = value_type*;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

    Container  *m_pContainer = nullptr;
    Node       *m_node       = nullptr; // para que los iteradores derivados lo usen
    Size        m_pos        = -1;
  public:
    GeneralIterator(Container *pContainer = nullptr, Size pos=0) 
         : m_pContainer(pContainer), m_node(nullptr), m_pos(pos) {}

    GeneralIterator(const GeneralIterator<Container> &another)
         :  m_pContainer(another.m_pContainer),
            m_node (another.m_node),
            m_pos  (another.m_pos)
    {}

    virtual ~GeneralIterator(){};
    
  bool operator==(const GeneralIterator<Container> &another) const {
    return m_pContainer == another.m_pContainer &&
         m_node       == another.m_node &&
         m_pos        == another.m_pos;
  }

  bool operator!=(const GeneralIterator<Container> &another) const {
    return !(*this == another);
  }
    

  value_type &operator*(){
    if(m_node) return m_node->GetValueRef();
    for containers that
    
    return m_pContainer->m_data[m_pos].GetValueRef();
  }
};

#endif // __GENERAL_ITERATOR_H__
