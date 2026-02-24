#ifndef BTREE_ITERATOR_H
#define BTREE_ITERATOR_H

#include <memory>
#include <vector>
#include <iterator>
#include <cstddef>

template <typename ValueType>
class BTreeSnapshotIterator
{
public:
    typedef ValueType                                value_type;
    typedef const ValueType&                         reference;
    typedef const ValueType*                         pointer;
    typedef std::ptrdiff_t                           difference_type;
    typedef std::bidirectional_iterator_tag          iterator_category;

    BTreeSnapshotIterator() : m_Pos(0) {}

    BTreeSnapshotIterator(std::shared_ptr<std::vector<ValueType> > data, std::size_t pos)
        : m_Data(data), m_Pos(pos) {}

    reference operator*() const
    {
        return (*m_Data)[m_Pos];
    }

    pointer operator->() const
    {
        return &(*m_Data)[m_Pos];
    }

    BTreeSnapshotIterator& operator++()
    {
        ++m_Pos;
        return *this;
    }

    BTreeSnapshotIterator operator++(int)
    {
        BTreeSnapshotIterator tmp(*this);
        ++(*this);
        return tmp;
    }

    BTreeSnapshotIterator& operator--()
    {
        --m_Pos;
        return *this;
    }

    BTreeSnapshotIterator operator--(int)
    {
        BTreeSnapshotIterator tmp(*this);
        --(*this);
        return tmp;
    }

    bool operator==(const BTreeSnapshotIterator& other) const
    {
        return m_Data.get() == other.m_Data.get() && m_Pos == other.m_Pos;
    }

    bool operator!=(const BTreeSnapshotIterator& other) const
    {
        return !(*this == other);
    }

private:
    std::shared_ptr<std::vector<ValueType> > m_Data;
    std::size_t m_Pos;
};

#endif