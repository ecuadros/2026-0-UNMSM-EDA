#ifndef __BTREE_H__
#define __BTREE_H__

#include <iostream>
#include <mutex>
#include <utility>
#include <vector>
#include "BTreePage.h"
#include "GeneralIterator.h"
#include "../general/types.h"

#define DEFAULT_BTREE_ORDER 3

template <typename T>
struct BTreeTraitAscending {
    using value_type  = T;
    using CompareFunc = std::greater<T>; 
};

template <typename T>
struct BTreeTraitDescending {
    using value_type  = T;
    using CompareFunc = std::less<T>;
};

template <typename Container>
class BTreeForwardIterator : public GeneralIterator<Container> {

    using Pagina = typename Container::BTNode;
    using Base   = GeneralIterator<Container>;

    std::vector< std::pair<Pagina*, Size> > m_ruta;

    void DescenderMasIzquierda(Pagina* paginaActual){
        while(paginaActual && paginaActual->GetNumberOfKeys() > 0){
            m_ruta.push_back({paginaActual, 0});
            paginaActual = paginaActual->m_SubPages[0];
        }
        ActualizarPosicionBase();
    }

    void ActualizarPosicionBase(){
        if(m_ruta.empty()){
            this->m_data = nullptr;
            this->m_pos  = -1;
        }else{
            this->m_data = m_ruta.back().first->m_Keys.data();
            this->m_pos  = m_ruta.back().second;
        }
    }

public:
    BTreeForwardIterator(Container* arbol, bool esFin = false)
        : Base(arbol)
    {
        if(!esFin && arbol->m_NumKeys > 0)
            DescenderMasIzquierda(&arbol->m_Root);
        else{
            this->m_data = nullptr;
            this->m_pos  = -1;
        }
    }

    BTreeForwardIterator& operator++(){

        if(m_ruta.empty()) return *this;

        auto& cima = m_ruta.back();
        Pagina* paginaActual = cima.first;
        Size indiceSiguiente = cima.second + 1;

        cima.second++;

        if(paginaActual->m_SubPages[indiceSiguiente]){
            DescenderMasIzquierda(paginaActual->m_SubPages[indiceSiguiente]);
        }
        else{
            while(!m_ruta.empty()){
                if(m_ruta.back().second < m_ruta.back().first->GetNumberOfKeys())
                    break;
                m_ruta.pop_back();
            }
            ActualizarPosicionBase();
        }

        return *this;
    }

    bool operator!=(const BTreeForwardIterator &otro) const {
        return this->m_data != otro.m_data || this->m_pos != otro.m_pos;
    }
};

template <typename Container>
class BTreeBackwardIterator : public GeneralIterator<Container> {

    using Pagina = typename Container::BTNode;
    using Base   = GeneralIterator<Container>;

    std::vector< std::pair<Pagina*, Size> > m_ruta;

    void DescenderMasDerecha(Pagina* paginaActual){
        while(paginaActual && paginaActual->GetNumberOfKeys() > 0){
            Size ultimoIndice = paginaActual->GetNumberOfKeys() - 1;
            m_ruta.push_back({paginaActual, ultimoIndice});
            paginaActual = paginaActual->m_SubPages[ultimoIndice + 1];
        }
        ActualizarPosicionBase();
    }

    void ActualizarPosicionBase(){
        if(m_ruta.empty()){
            this->m_data = nullptr;
            this->m_pos  = -1;
        }else{
            this->m_data = m_ruta.back().first->m_Keys.data();
            this->m_pos  = m_ruta.back().second;
        }
    }

public:
    BTreeBackwardIterator(Container* arbol, bool esFin = false)
        : Base(arbol)
    {
        if(!esFin && arbol->m_NumKeys > 0)
            DescenderMasDerecha(&arbol->m_Root);
        else{
            this->m_data = nullptr;
            this->m_pos  = -1;
        }
    }

    BTreeBackwardIterator& operator++(){

        if(m_ruta.empty()) return *this;

        auto& cima = m_ruta.back();
        Pagina* paginaActual = cima.first;
        Size indiceIzquierdo = cima.second;

        cima.second--;

        if(paginaActual->m_SubPages[indiceIzquierdo]){
            DescenderMasDerecha(paginaActual->m_SubPages[indiceIzquierdo]);
        }
        else{
            while(!m_ruta.empty() && m_ruta.back().second < 0){
                m_ruta.pop_back();
            }
            ActualizarPosicionBase();
        }

        return *this;
    }

    bool operator!=(const BTreeBackwardIterator &otro) const {
        return this->m_data != otro.m_data || this->m_pos != otro.m_pos;
    }
};


// ------- CLASE PRINCIPAL -------
template <typename Traits>
class BTree {

public:
    using value_type = typename Traits::value_type;
    using BTNode     = CBTreePage<Traits>;
    using ObjectInfo = typename BTNode::ObjectInfo;

    using forward_iterator  = BTreeForwardIterator< BTree<Traits> >;
    using backward_iterator = BTreeBackwardIterator< BTree<Traits> >;

protected:
    BTNode m_Root;
    Size   m_NumKeys;
    Size   m_Order;
    Size   m_Height;
    bool   m_Unico;

    mutable std::mutex m_mutex;

public:


    BTree(Size orden = DEFAULT_BTREE_ORDER, bool unico = true)
        : m_Root(2*orden + 1, unico),
          m_NumKeys(0),
          m_Order(orden),
          m_Height(1),
          m_Unico(unico)
    {
        m_Root.SetMaxKeysForChilds(orden);
    }

    BTree(const BTree& otro){
        std::lock_guard<std::mutex> lock(otro.m_mutex);
        CopiarDesde(otro);
    }

    BTree(BTree&& otro) noexcept{
        std::lock_guard<std::mutex> lock(otro.m_mutex);
        MoverDesde(std::move(otro));
    }

    virtual ~BTree(){
        std::lock_guard<std::mutex> lock(m_mutex);
        Limpiar();
    }

private:

    void CopiarDesde(const BTree& otro){
        m_Root     = otro.m_Root;
        m_NumKeys  = otro.m_NumKeys;
        m_Order    = otro.m_Order;
        m_Height   = otro.m_Height;
        m_Unico    = otro.m_Unico;
    }

    void MoverDesde(BTree&& otro){
        m_Root    = std::move(otro.m_Root);
        m_NumKeys = std::exchange(otro.m_NumKeys, 0);
        m_Order   = otro.m_Order;
        m_Height  = std::exchange(otro.m_Height, 0);
        m_Unico   = otro.m_Unico;
    }

    void Limpiar(){
        m_Root.Reset();
        m_NumKeys = 0;
        m_Height  = 1;
    }

public:

    forward_iterator begin(){ return forward_iterator(this,false); }
    forward_iterator end()  { return forward_iterator(this,true); }

    backward_iterator rbegin(){ return backward_iterator(this,false); }
    backward_iterator rend()  { return backward_iterator(this,true); }


    bool Insert(const value_type clave, const ref_type referencia){
        std::lock_guard<std::mutex> lock(m_mutex);

        bt_ErrorCode codigo = m_Root.Insert(clave, referencia);

        if(codigo == bt_duplicate) return false;

        m_NumKeys++;

        if(codigo == bt_overflow){
            m_Root.SplitRoot();
            m_Height++;
        }

        return true;
    }

    bool Remove(const value_type clave, const ref_type referencia){
        std::lock_guard<std::mutex> lock(m_mutex);

        bt_ErrorCode codigo = m_Root.Remove(clave, referencia);

        if(codigo == bt_duplicate || codigo == bt_nofound)
            return false;

        m_NumKeys--;

        if(codigo == bt_rootmerged)
            m_Height--;

        return true;
    }

    ref_type Search(const value_type clave){
        std::lock_guard<std::mutex> lock(m_mutex);
        ref_type ref = -1;
        m_Root.Search(clave, ref);
        return ref;
    }


    template <typename Func, typename... Args>
    void InOrder(Func funcion, Args&&... args){
        m_Root.InOrder(funcion, 0, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void PreOrder(Func funcion, Args&&... args){
        m_Root.PreOrder(funcion, 0, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void PostOrder(Func funcion, Args&&... args){
        m_Root.PostOrder(funcion, 0, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    ObjectInfo* FirstThat(Func funcion, Args&&... args){
        return m_Root.FirstThat(funcion, 0, std::forward<Args>(args)...);
    }


    friend std::ostream& operator<<(std::ostream& os, BTree& arbol){

        os << "[ ";

        auto imprimir = [](ObjectInfo& info, Size nivel, std::ostream& salida){
            salida << "(" << info.key << ":" << info.ObjID << ") ";
        };

        arbol.InOrder(imprimir, os);

        os << "]";
        return os;
    }

    friend std::istream& operator>>(std::istream& is, BTree& arbol){

        arbol.Limpiar();

        char caracter;
        value_type clave;
        ref_type referencia;

        while(is >> caracter && caracter != '['){}

        while(is >> caracter && caracter != ']'){
            if(caracter == '('){
                is >> clave;
                is >> caracter; // :
                is >> referencia;
                arbol.Insert(clave, referencia);
                while(is >> caracter && caracter != ')');
            }
        }

        return is;
    }
};

void DemoBTree();

#endif // __BTREE_H__
