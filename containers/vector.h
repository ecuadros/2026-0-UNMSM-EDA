#ifndef __VECTOR_H__
#define __VECTOR_H__
#include <ostream>
//#include <algorithm>
#include "../util.h"

template < typename Traits >
class Vector{
    using value_type = typename Traits::value_type;
    using ref_value_type = typename Traits::ref_value_type;
    using pointer_value_type = typename Traits::pointer_value_type;
    // using iterator = T*;

    friend ArrayForwardIterator<Vector>;
    using forward_iterator =ArrayForwardIterator<Vector>;

    private:
    size_t m_size;
    size_t m_capacity;
    pointer_value_type m_data;
    
    public:
    Vector();
    explicit Vector( size_t );
    Vector( const Vector& );
    ~Vector();

    void pushback( value_type value );
    value_type popback();

    ref_value_type operator[] ( size_t );
    const ref_value_type operator[] ( size_t ) const ;
    Vector& operator=( const Vector& );
    Vector& operator+=( const Vector& );
    bool operator==( const Vector& );
    void reserve( size_t );

    size_t getSize() const { return m_size; } size_t getCapacity() const { return m_capacity; }
    forward_iterator begin(){ return forward_iterator(this); } 
    forward_iterator end(){ return forward_iterator(this, m_size); }
    
    void forEach( Operacion< Traits >, unsigned int );

    
    private:
    void inicializar( value_type* );
    void resize();

};

template < typename Traits >
Vector<Traits>::Vector(): m_size(0), m_capacity(0), m_data(nullptr){}

template < typename Traits >
Vector<Traits>::Vector( size_t capacity )
:m_size(0), m_capacity(capacity), m_data(nullptr){
    m_data = new value_type[capacity];
    inicializar( m_data );
}




template < typename Traits >
Vector<Traits>::Vector( const Vector& other )
: m_size(other.getSize()), m_capacity(other.getCapacity()), m_data(nullptr){
    m_data = new value_type[other.getCapacity()];
    for( size_t i = 0; i < other.getSize(); ++i )
        m_data[i] = other[i];

}

template < typename Traits >
Vector<Traits>::~Vector(){ delete[] m_data; }

template < typename Traits >
void Vector<Traits>::pushback( value_type value ){
    if( m_size == m_capacity )
        resize();
    m_data[m_size++] = value;    
}

template < typename Traits >
typename Traits::value_type Vector<Traits>::popback(){
    const unsigned int c = 3;
    unsigned int y = m_capacity / c;
    if( m_size < y )
        reserve(y*3 / 2);
    return m_data[--m_size];
}

template < typename Traits >
typename Traits::ref_value_type Vector<Traits>::operator[]( size_t index ){
    return m_data[index];
}

template < typename Traits >
const typename Traits::ref_value_type Vector<Traits>::operator[]( size_t index ) const {
    return m_data[index];
}

template < typename Traits >
Vector<Traits>& Vector<Traits>::operator=( const Vector& other ){
    m_size = other.getSize();
    m_capacity = other.getCapacity();
    delete[] m_data;

    m_data = new value_type[other.getCapacity()];
    for( size_t i = 0 ; i < other.getSize(); ++i ){
        m_data[i] = other[i];
    }
    return *this;
}
// template < typename T >
// Vector<T>& Vector<T>::operator+=( const Vector& )

template < typename Traits >
bool Vector<Traits>::operator==( const Vector& other ){
    bool flag = true;
    for( size_t i = 0; i < m_size && flag == true; ++i ){
        if( m_data[i] != other[i] )
            flag = false;
    }
    return flag;
}

template < typename Traits > 
void Vector<Traits>::reserve( size_t capacity ){
    value_type* new_data = new value_type[capacity];
    size_t new_size = std::min(capacity, m_size);
    for( size_t i = 0; i < new_size; ++i )
        new_data[i] = m_data[i];
    m_capacity = capacity;
    delete[] m_data;
    m_data = new_data;
    m_size = new_size;

}

template < typename Traits >
void Vector<Traits>::forEach( Operacion<Traits> op, unsigned int n ){
    for( size_t i = 0; i < m_size; ++i ){
        op(m_data[i],n);
    }
}

template < typename Traits >
std::ostream& operator<<( std::ostream& os, const Vector<Traits>& vector ){
    os << "Size: " << vector.getSize() << " - Capacity: " << vector.getCapacity() << "  ->  ";
    os << "[ ";
    for( size_t i = 0; i < vector.getSize(); i++ ){
        os << vector[i] << " ";
    }
    os << "]";
    return os;
}

template < typename Traits > 
void Vector<Traits>::inicializar( value_type* data ){
    for( size_t i = 0; i < m_capacity; ++i )
        data[i] = value_type();
}


template < typename Traits > 
void Vector<Traits>::resize(){
    size_t new_capacity = ( m_capacity == 0 ? 1 : m_capacity * 2 );
    reserve (new_capacity);
}


#endif // VECTOR_H
