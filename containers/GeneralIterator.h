#ifndef __GENERAL_ITERATOR_H__
#define __GENERAL_ITERATOR_H__

template <typename Container>
struct GeneralIterator{
public:
  using value_type = typename Container::value_type;
  using Node       = typename Container::Node;

  Container *m_pContainer = nullptr;
  Node      *m_pNode      = nullptr;

public:
  GeneralIterator(Container *pContainer, Node *pNode)
                 : m_pContainer(pContainer), m_pNode(pNode){}
  GeneralIterator(const GeneralIterator<Container> &another)
                 : m_pContainer(another.m_pContainer), m_pNode(another.m_pNode){}
  
  virtual ~GeneralIterator(){}

  bool operator == (const GeneralIterator<Container> &another) const {
        return m_pContainer == another.m_pContainer && m_pNode == another.m_pNode;
  }

  bool operator != (const GeneralIterator<Container> &another) const {
        return !(*this == another);
  }

  value_type &operator*(){
    return m_pNode->GetValueRef();
  }
};

#endif // __GENERAL_ITERATOR_H__