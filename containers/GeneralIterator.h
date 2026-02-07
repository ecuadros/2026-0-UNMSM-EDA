#ifndef __GENERAL_ITERATOR_H__
#define __GENERAL_ITERATOR_H__
#include "../util.h"
#include <type_traits>
#include <utility>

template <typename Container>
struct GeneralIterator
{
private:
    template <typename C, typename = void>
    struct has_m_data : std::false_type {};

    template <typename C>
    struct has_m_data<C, std::void_t<decltype(std::declval<C>().m_data)>>
        : std::true_type {};
public:
    using value_type  = typename Container::value_type;
    using Node        = typename Container::Node;

    Container  *m_pContainer = nullptr;
    Node       *m_data       = nullptr;
    Size        m_pos        = -1;
public:
    GeneralIterator(Container *pContainer, Size pos=0) 
         : m_pContainer(pContainer) {
           if constexpr (has_m_data<Container>::value) {
             m_data = m_pContainer->m_data;
           }
           m_pos = pos;
         }
    GeneralIterator(GeneralIterator<Container> &another)
         :  m_pContainer(another.m_pContainer),
            m_data (another.m_data),
            m_pos  (another.m_pos)
    {}
    virtual ~GeneralIterator(){};
    
    bool operator!=(const GeneralIterator<Container> &another){
        return m_pContainer != another.m_pContainer ||
               m_pos        != another.m_pos;         
    }
    virtual value_type &operator*(){
        static_assert(has_m_data<Container>::value,
                    "GeneralIterator::operator* requires Container::m_data");
        return m_data[m_pos].GetValueRef();
    }
};

#endif // __GENERAL_ITERATOR_H__
