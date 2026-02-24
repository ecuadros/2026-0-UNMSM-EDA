// btree.h
#ifndef __BTREE_H__
#define __BTREE_H__

#include <iostream>
#include "BTreePage.h"
#include <mutex>
#include <utility>
#include <vector>
#include <memory>

#define DEFAULT_BTREE_ORDER 3

template <typename keyType, typename ObjIDType = long>
class BTree 
// this is the full version of the BTree
{
       typedef CBTreePage <keyType, ObjIDType> BTNode;// useful shorthand
       /*struct ObjectInfo
       {
               keyType first;
               long    second;
               ObjectInfo *&operator->() { return this; }
       };*/

public:
       //typedef ObjectInfo iterator;
       typedef typename BTNode::lpfnForEach2    lpfnForEach2;
       typedef typename BTNode::lpfnForEach3    lpfnForEach3;
       typedef typename BTNode::lpfnFirstThat2  lpfnFirstThat2;
       typedef typename BTNode::lpfnFirstThat3  lpfnFirstThat3;
       typedef typename BTNode::ObjectInfo      ObjectInfo;

public:
       BTree(int order = DEFAULT_BTREE_ORDER, bool unique = true);

       //Destructor
       ~BTree();

       //int           Open (char * name, int mode);
       //int           Create (char * name, int mode);
       //int           Close ();
       bool            Insert (const keyType key, const ObjIDType ObjID);
       bool            Remove (const keyType key, const ObjIDType ObjID);
       ObjIDType       Search (const keyType key);
       long            size()  { return m_NumKeys; }
       long            height() { return m_Height;      }
       long            GetOrder() { return m_Order;     }

       void            Print (std::ostream &os) const;
       void            ForEach( lpfnForEach2 lpfn, void *pExtra1 );
       void            ForEach( lpfnForEach3 lpfn, void *pExtra1, void *pExtra2);
       ObjectInfo*     FirstThat( lpfnFirstThat2 lpfn, void *pExtra1 );
       ObjectInfo*     FirstThat( lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2);
       //typedef               ObjectInfo iterator;

protected:
       BTNode          m_Root;
       long            m_NumKeys; // number of keys
       bool            m_Unique;  // Accept the elements only once ?
       int             m_Order;   // order of tree
       int             m_Height;  // height of tree

       mutable std::mutex mtx; // mutex for thread safety

public:
       // Move constructor
       BTree(BTree&& other) noexcept : m_Root(std::move(other.m_Root)),
                                       m_NumKeys(other.m_NumKeys),
                                       m_Unique(other.m_Unique),
                                       m_Order(other.m_Order),
                                       m_Height(other.m_Height)
       {
               other.m_NumKeys = 0;
               other.m_Height = 0;
       }

       // Operador<< para imprimir el árbol
       friend std::ostream& operator<<(std::ostream& os, const BTree& tree) {
               tree.Print(os);
               return os;
       }

       // Iterador para recorrer el árbol
       class iterator {
              std::shared_ptr<std::vector<ObjectInfo*>> elements;
              long index;

       public:
              iterator(std::shared_ptr<std::vector<ObjectInfo*>> elems, long idx)
                     : elements(elems), index(idx) {}

              ObjectInfo& operator*() {
                     return *(*elements)[index];
              }
              iterator &operator++() {
                     ++index;
                     return *this;
              }
              iterator &operator--() {
                     --index;
                     return *this;
              }
              bool operator==(const iterator &other) const {
                     return elements == other.elements && index == other.index;
              }
              bool operator!=(const iterator &other) const {
                     return !(*this == other);
              }
       };

       // Métodos para obtener iteradores
       iterator begin(){
              std::lock_guard<std::mutex> lock(mtx);
              m_IteratorData = std::make_shared<std::vector<ObjectInfo*>>();
              CollectInOrder(*m_IteratorData);
              return iterator(m_IteratorData, 0);
       }
       iterator end(){
              return iterator(m_IteratorData, m_IteratorData->size());
       }

       // Métodos para obtener iteradores inversos
       iterator rbegin(){
              std::lock_guard<std::mutex> lock(mtx);
              m_IteratorData = std::make_shared<std::vector<ObjectInfo*>>();
              CollectInOrder(*m_IteratorData);
              
              if(m_IteratorData->empty()) {
                     return iterator(m_IteratorData, 0); // Devuelve begin() si el árbol está vacío
              }
              return iterator(m_IteratorData, m_IteratorData->size() - 1);
       }
       iterator rend(){
              return iterator(m_IteratorData, -1);
       }

       // In-order traversal methods
       void InOrder(lpfnForEach2 lpfn, void *pExtra1) {// Con 1 parámetro extra
              std::lock_guard<std::mutex> lock(mtx);
              m_Root.ForEach(lpfn, 0, pExtra1);
       }
       void InOrder(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2) {// Con 2 parámetros extra
              std::lock_guard<std::mutex> lock(mtx);
              m_Root.ForEach(lpfn, 0, pExtra1, pExtra2);
       }

       // Pre-order traversal methods
       void PreOrder(lpfnForEach2 lpfn, void *pExtra1) {
              std::lock_guard<std::mutex> lock(mtx);
              m_Root.ForEachPreOrder(lpfn, 0, pExtra1);
       }
       void PreOrder(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2) {
              std::lock_guard<std::mutex> lock(mtx);
              m_Root.ForEachPreOrder(lpfn, 0, pExtra1, pExtra2);
       }

       // Post-order traversal methods
       void PostOrder(lpfnForEach2 lpfn, void *pExtra1) {
              std::lock_guard<std::mutex> lock(mtx);
              m_Root.ForEachPostOrder(lpfn, 0, pExtra1);
       }
       void PostOrder(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2) {
              std::lock_guard<std::mutex> lock(mtx);
              m_Root.ForEachPostOrder(lpfn, 0, pExtra1, pExtra2);
       }

private:
       // Método para recolectar los elementos del árbol en orden
       void CollectInOrder(std::vector<ObjectInfo*>& vec){
              auto collector = [](const ObjectInfo &info, int level, void *pExtra) {
                     auto* v = static_cast<std::vector<ObjectInfo*>*>(pExtra);
                     v->push_back(const_cast<ObjectInfo*>(&info)); // Agrega el elemento al vector
              };
              m_Root.ForEach(collector, 0, &vec);
       }

       std::shared_ptr<std::vector<ObjectInfo*>> m_IteratorData; // shared pointer to hold data for iterators

};

static const int MaxHeight = 5;
template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::BTree(int order, bool unique)
                               : m_Root(2 * order  + 1, unique),
                                 m_NumKeys(0),
                                 m_Unique(unique),
                                 m_Order(order)
{
       m_Root.SetMaxKeysForChilds(order);
       m_Height = 1;
}

template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::~BTree() = default;

template <typename keyType, typename ObjIDType>
bool BTree<keyType, ObjIDType>::Insert(const keyType key, const ObjIDType ObjID)
{
       std::lock_guard<std::mutex> lock(mtx);
       bt_ErrorCode error = m_Root.Insert(key, ObjID);
       if( error == bt_duplicate )
               return false;
       m_NumKeys++;
       if( error == bt_overflow )
       {
               m_Root.SplitRoot();
               m_Height++;
       }
       return true;
}

template <typename keyType, typename ObjIDType>
bool BTree<keyType, ObjIDType>::Remove (const keyType key, const ObjIDType ObjID)
{
       std::lock_guard<std::mutex> lock(mtx); 
       bt_ErrorCode error = m_Root.Remove(key, ObjID);
       if( error == bt_duplicate || error == bt_nofound )
               return false;
       m_NumKeys--;

       if( error == bt_rootmerged )
               m_Height--;
       return true;
}

template <typename keyType, typename ObjIDType>
ObjIDType BTree<keyType, ObjIDType>::Search (const keyType key)
{
       std::lock_guard<std::mutex> lock(mtx); 
       ObjIDType ObjID {};
       m_Root.Search(key, ObjID);
       return ObjID;
}


template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::ForEach(lpfnForEach2 lpfn, void *pExtra1)
{
       std::lock_guard<std::mutex> lock(mtx);
       m_Root.ForEach(lpfn, 0, pExtra1);
}

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::ForEach(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2)
{
       std::lock_guard<std::mutex> lock(mtx);
       m_Root.ForEach(lpfn, 0, pExtra1, pExtra2);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::ObjectInfo *
BTree<keyType, ObjIDType>::FirstThat(lpfnFirstThat2 lpfn, void *pExtra1)
{
       std::lock_guard<std::mutex> lock(mtx);
       return m_Root.FirstThat(lpfn, 0, pExtra1);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::ObjectInfo *
BTree<keyType, ObjIDType>::FirstThat(lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2)
{
       std::lock_guard<std::mutex> lock(mtx);
       return m_Root.FirstThat(lpfn, 0, pExtra1, pExtra2);
}

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::Print(std::ostream &os)const{
       m_Root.Print(os);
}

void DemoBTree();

#endif // __BTREE_H__