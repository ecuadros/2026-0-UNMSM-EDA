#ifndef __GENERAL_ITERATOR_H__
#define __GENERAL_ITERATOR_H__

#include "util.h"

template <typename Container>
struct GeneralIterator { 
public:
    using value_type = typename Container::value_type;
    Container* m_pContainer = nullptr;
    size_t m_pos = 0;

    GeneralIterator(Container* pContainer, size_t pos = 0) 
        : m_pContainer(pContainer), m_pos(pos) {}

    virtual ~GeneralIterator() {}
    
    bool operator!=(const GeneralIterator<Container>& other) const {
        return m_pos != other.m_pos;         
    }

    virtual value_type& operator*() = 0; 
    virtual GeneralIterator& operator++() = 0;
};

#endif