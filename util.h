#ifndef __UTIL_H__
#define __UTIL_H__


template < typename T >
struct Trait_{
    using value_type = T;
    using ref_value_type = T&;
    using pointer_value_type = T*;
};

template < typename Traits >
using Operacion = void(*)(typename Traits::ref_value_type, unsigned int);

template < typename T >
void potencia(T& elemento, unsigned int n ){
    T aux = 1;
    for( size_t i = 0; i < n; ++i)
        aux *=elemento;
    elemento = aux;    
}



template < typename T > 
T TestProducto( T elemento ){
    return elemento;
}
template < typename T, typename ...Args > 
T TestProducto( T elemento, Args ...args ){
    return elemento * TestProducto( args... );
}

template < typename Container >
class ArrayForwardIterator{
    using pointer_value_type = typename Container::pointer_value_type;
    using value_type = typename Container::value_type;

    private:
    Container* m_container;
    size_t pos;

    public:

    ArrayForwardIterator( Container* pContainer )
    :m_container( pContainer ), pos(0){}
    
    ArrayForwardIterator( Container* pContainer, size_t p )
    :m_container( pContainer ), pos(p){}

    ArrayForwardIterator& operator++(){
        if( pos < m_container->getSize() )
            ++pos;
        return *this;
    }

    bool operator!=( const ArrayForwardIterator& other) const {
        return pos != other.pos;
    }

    value_type& operator*(){
        return (*m_container)[pos];
    }

};

#endif // __UTIL_H__
