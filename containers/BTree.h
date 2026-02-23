// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "BTreePage.h"
#include "../foreach.h"
#include "../general/types.h"

#define DEFAULT_BTREE_ORDER 3

template <typename keyType, typename ObjIDType>
class BTree;

template <typename keyType, typename ObjIDType>
ostream &operator<<(ostream &os, BTree<keyType, ObjIDType> &tree);

template <typename keyType, typename ObjIDType>
istream &operator>>(istream &is, BTree<keyType, ObjIDType> &tree);

template <typename keyType, typename ObjIDType = LSize>
class BTree 
// this is the full version of the BTree
{
       using BTNode = CBTreePage<keyType, ObjIDType>; // useful shorthand
       /*struct ObjectInfo
       {
               keyType first;
               long    second;
               ObjectInfo *&operator->() { return this; }
       };*/

public:
       using key_type = keyType;
       using obj_id_type = ObjIDType;
       using size_type = Size;
       using long_size_type = LSize;
       using forward_iterator = ForwardBTreeIterator<keyType, ObjIDType>;
       using backward_iterator = BackwardBTreeIterator<keyType, ObjIDType>;

       //typedef ObjectInfo iterator;
       typedef typename BTNode::lpfnForEach2    lpfnForEach2;
       typedef typename BTNode::lpfnForEach3    lpfnForEach3;
       typedef typename BTNode::lpfnFirstThat2  lpfnFirstThat2;
       typedef typename BTNode::lpfnFirstThat3  lpfnFirstThat3;
       typedef typename BTNode::ObjectInfo      ObjectInfo;

       BTree(Size order = DEFAULT_BTREE_ORDER, bool unique = true);
       ~BTree();
       //int           Open (char * name, int mode);
       //int           Create (char * name, int mode);
       //int           Close ();
       bool            Insert (const keyType key, const ObjIDType ObjID);
       bool            Remove (const keyType key, const ObjIDType ObjID);
       ObjIDType       Search (const keyType key);
       LSize           size();
       LSize           height();
       LSize           GetOrder();

       void            Print (ostream &os);
       void            ForEach( lpfnForEach2 lpfn, void *pExtra1 );
       void            ForEach( lpfnForEach3 lpfn, void *pExtra1, void *pExtra2);
       ObjectInfo*     FirstThat( lpfnFirstThat2 lpfn, void *pExtra1 );
       ObjectInfo*     FirstThat( lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2);

       template <typename Func, typename ...Args>
       void            ForEach( forward_iterator start, forward_iterator end, Func func, Args&& ...args );
       template <typename Func, typename ...Args>
       ObjectInfo*     FirstThat( forward_iterator start, forward_iterator end, Func func, Args&& ...args);
       template <typename Func, typename ...Args>
       void            inorderTraversal(Func func, Args&& ...args);
       template <typename Func, typename ...Args>
       void            preorderTraversal(Func func, Args&& ...args);
       template <typename Func, typename ...Args>
       void            postorderTraversal(Func func, Args&& ...args);

       forward_iterator begin();
       forward_iterator end();
       backward_iterator rbegin();
       backward_iterator rend();

protected:
       mutable mutex   mtx;
       BTNode          m_Root;
       LSize           m_NumKeys; // number of keys
       bool            m_Unique;  // Accept the elements only once ?
       LSize           m_Order;   // order of tree
       LSize           m_Height;  // height of tree

       friend class ForwardBTreeIterator<keyType, ObjIDType>;
       friend class BackwardBTreeIterator<keyType, ObjIDType>;
       friend ostream &operator<< <>(ostream &os, BTree<keyType, ObjIDType> &tree);
       friend istream &operator>> <>(istream &is, BTree<keyType, ObjIDType> &tree);

private:
       static void _serialize_page(ostream &os, BTNode *page, bool &isFirst);
       void _clear_unlocked();
       bool _insert_unlocked(keyType key, ObjIDType ObjID);

       template <typename Func, typename ...Args>
       static void _inorderTraversal(BTNode *page, Func &func, Args&&...args);
       template <typename Func, typename ...Args>
       static void _preorderTraversal(BTNode *page, Func &func, Args&&...args);
       template <typename Func, typename ...Args>
       static void _postorderTraversal(BTNode *page, Func &func, Args&&...args);
};

const Size MaxHeight = 5;
template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::BTree(Size order, bool unique)
                               : m_Root(2 * order  + 1, unique),
                                 m_NumKeys(0),
                                 m_Unique(unique),
                                 m_Order(order)
{
       m_Root.SetMaxKeysForChilds(order);
       m_Height = 1;
}

template <typename keyType, typename ObjIDType>
BTree<keyType, ObjIDType>::~BTree()
{
}

// funciones basicas bloqueadas por el mutex

template <typename keyType, typename ObjIDType>
LSize BTree<keyType, ObjIDType>::size() {
       lock_guard<mutex> lock(mtx);
       return m_NumKeys;
}

template <typename keyType, typename ObjIDType>
LSize BTree<keyType, ObjIDType>::height() {
       lock_guard<mutex> lock(mtx);
       return m_Height;
}

template <typename keyType, typename ObjIDType>
LSize BTree<keyType, ObjIDType>::GetOrder() {
       lock_guard<mutex> lock(mtx);
       return m_Order;
}

// foreach y firstthat basadas en iteradores

template<typename keyType, typename ObjIDType>
template<typename Func, typename ...Args>
void BTree<keyType, ObjIDType>::ForEach(forward_iterator start, forward_iterator end, Func func, Args &&...args) {
       ::Foreach(start, end, func, std::forward<Args>(args)...);
}

template<typename keyType, typename ObjIDType>
template<typename Func, typename... Args>
typename BTree<keyType, ObjIDType>::ObjectInfo *
BTree<keyType, ObjIDType>::FirstThat(forward_iterator start, forward_iterator end, Func func, Args &&... args) {
       auto it = ::FirstThat(start, end, func, std::forward<Args>(args)...);
       return (it == end) ? nullptr : it.operator->();
}

// funciones publicas para los traversals

template <typename keyType, typename ObjIDType>
template <typename Func, typename ...Args>
void BTree<keyType, ObjIDType>::inorderTraversal(Func func, Args &&...args) {
       lock_guard<mutex> lock(mtx);
       _inorderTraversal(&m_Root, func, std::forward<Args>(args)...);
}

template <typename keyType, typename ObjIDType>
template <typename Func, typename ...Args>
void BTree<keyType, ObjIDType>::preorderTraversal(Func func, Args &&...args) {
       lock_guard<mutex> lock(mtx);
       _preorderTraversal(&m_Root, func, std::forward<Args>(args)...);
}

template <typename keyType, typename ObjIDType>
template <typename Func, typename ...Args>
void BTree<keyType, ObjIDType>::postorderTraversal(Func func, Args &&...args) {
       lock_guard<mutex> lock(mtx);
       _postorderTraversal(&m_Root, func, std::forward<Args>(args)...);
}

// funciones privadas para su implementacion

template <typename keyType, typename ObjIDType>
template <typename Func, typename ...Args>
void BTree<keyType, ObjIDType>::_inorderTraversal(BTNode *page, Func &func, Args&&...args) {
       if (!page || page->m_KeyCount == 0) return;
       // avanza a la subpagina de la clave
       for (Size i = 0; i < page->m_KeyCount; ++i) {
               _inorderTraversal(page->m_SubPages[i], func, std::forward<Args>(args)...);
               // luego aplica la funcion
               func(page->m_Keys[i], std::forward<Args>(args)...);
       }
       // y luego continua con la ultima subpagina
       _inorderTraversal(page->m_SubPages[page->m_KeyCount], func, std::forward<Args>(args)...);
}

template <typename keyType, typename ObjIDType>
template <typename Func, typename ...Args>
void BTree<keyType, ObjIDType>::_preorderTraversal(BTNode *page, Func &func, Args&&...args) {
       if (!page || page->m_KeyCount == 0) return;
       // primero aplica la funcion para todas las claves de la pagina
       for (Size i = 0; i < page->m_KeyCount; ++i) {
               func(page->m_Keys[i], std::forward<Args>(args)...);
       }
       // luego avanza a la siguiente pagina
       for (Size i = 0; i <= page->m_KeyCount; ++i) {
               _preorderTraversal(page->m_SubPages[i], func, std::forward<Args>(args)...);
       }
}

template <typename keyType, typename ObjIDType>
template <typename Func, typename ...Args>
void BTree<keyType, ObjIDType>::_postorderTraversal(BTNode *page, Func &func, Args&&...args) {
       if (!page || page->m_KeyCount == 0) return;
       // primero avanza hasta la ultima pagina
       for (Size i = 0; i <= page->m_KeyCount; ++i) {
               _postorderTraversal(page->m_SubPages[i], func, std::forward<Args>(args)...);
       }
       // y aplica la funcion desde atras hacia adelante
       for (Size i = 0; i < page->m_KeyCount; ++i) {
               func(page->m_Keys[i], std::forward<Args>(args)...);
       }
}

// funcion publica del insert
template <typename keyType, typename ObjIDType>
bool BTree<keyType, ObjIDType>::Insert(const keyType key, const ObjIDType ObjID)
{
       lock_guard<mutex> lock(mtx);
       return _insert_unlocked(key, ObjID);
}
// funcion privada de la logica del insert
template <typename keyType, typename ObjIDType>
bool BTree<keyType, ObjIDType>::_insert_unlocked(const keyType key, const ObjIDType ObjID) {
       // depende de la logica del insert de la Page
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
       lock_guard<mutex> lock(mtx);
       // depende de la logica del remove de la Page
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
       lock_guard<mutex> lock(mtx);
       ObjIDType ObjID{};  // esto antes usaba -1
       m_Root.Search(key, ObjID);
       return ObjID;
}

// funciones previamente implementadas tambien tienen bloqueo del mutex
template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::ForEach(lpfnForEach2 lpfn, void *pExtra1)
{
       lock_guard<mutex> lock(mtx);
       m_Root.ForEach(lpfn, 0, pExtra1);
}

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::ForEach(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2)
{
       lock_guard<mutex> lock(mtx);
       m_Root.ForEach(lpfn, 0, pExtra1, pExtra2);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::ObjectInfo *
BTree<keyType, ObjIDType>::FirstThat(lpfnFirstThat2 lpfn, void *pExtra1)
{
       lock_guard<mutex> lock(mtx);
       return m_Root.FirstThat(lpfn, 0, pExtra1);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::ObjectInfo *
BTree<keyType, ObjIDType>::FirstThat(lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2)
{
       lock_guard<mutex> lock(mtx);
       return m_Root.FirstThat(lpfn, 0, pExtra1, pExtra2);
}

template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::Print(ostream &os){
       lock_guard<mutex> lock(mtx);
       m_Root.Print(os);
}

// funcion auxiliar para el clear
template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::_clear_unlocked() {
       m_Root.Reset();  // reset del root
       m_NumKeys = 0;
       m_Height = 1;
       m_Root.SetMaxKeysForChilds(static_cast<Size>(m_Order));
}

// auxiliar para serializar una pagina con formato en una linea
template <typename keyType, typename ObjIDType>
void BTree<keyType, ObjIDType>::_serialize_page(ostream &os, BTNode *page, bool &isFirst) {
       if (!page || page->m_KeyCount == 0) return;
       // itera cada llave de la pagina
       for (Size i = 0; i < page->m_KeyCount; ++i) {
               // entra a la subpagina (recorrido pre orden)
               _serialize_page(os, page->m_SubPages[i], isFirst);
               if (!isFirst) os << ",";
               os << "(";
               if constexpr (std::is_same_v<keyType, std::string>) os << std::quoted(page->m_Keys[i].key);
               else os << page->m_Keys[i].key;
               os << ":";
               if constexpr (std::is_same_v<ObjIDType, std::string>) os << std::quoted(page->m_Keys[i].ObjID);
               else os << page->m_Keys[i].ObjID;
               os << ")";
               isFirst = false;
       }
       // termina el trabajo
       _serialize_page(os, page->m_SubPages[page->m_KeyCount], isFirst);
}
// overload del operador <<
template <typename keyType, typename ObjIDType>
ostream &operator<<(ostream &os, BTree<keyType, ObjIDType> &tree) {
       lock_guard<mutex> lock(tree.mtx);
       os << "BTree [";
       bool isFirst = true;
       BTree<keyType, ObjIDType>::_serialize_page(os, &tree.m_Root, isFirst);
       os << "]";
       return os;
}

template <typename keyType, typename ObjIDType>
istream &operator>>(istream &is, BTree<keyType, ObjIDType> &tree) {
       if (!is) return is;

       string bar;  // dummy
       getline(is, bar, '[');
       lock_guard<mutex> lock(tree.mtx);
       tree._clear_unlocked();

       is >> ws;
       if (!is) {
               tree._clear_unlocked();
               return is;
       }
       if (is.peek() == ']') {
               is.get();
               return is;
       }

       while (is) {
               is >> ws;
               char ch = 0;
               if (!is.get(ch) || ch != '(') {
                       is.setstate(ios::failbit);
                       break;
               }
               keyType key {};
               ObjIDType objId {};
               if constexpr (std::is_same_v<keyType, std::string>) is >> std::quoted(key);
               else is >> key;
               if (!is) break;
               is >> ws;
               if (!is.get(ch) || ch != ':') {
                       is.setstate(ios::failbit);
                       break;
               }
               if constexpr (std::is_same_v<ObjIDType, std::string>) is >> std::quoted(objId);
               else is >> objId;
               if (!is) break;
               is >> ws;
               if (!is.get(ch) || ch != ')') {
                       is.setstate(ios::failbit);
                       break;
               }

               if (!tree._insert_unlocked(key, objId)) {
                       is.setstate(ios::failbit);
                       break;
               }

               is >> ws;
               const int next = is.peek();
               if (next == ',') {
                       is.get();
                       continue;
               }
               if (next == ']') {
                       is.get();
                       break;
               }
               is.setstate(ios::failbit);
               break;
       }

       if (!is) tree._clear_unlocked();
       return is;
}

// iteradores:

template <typename keyType, typename ObjIDType>
class ForwardBTreeIterator {
       using Tree = BTree<keyType, ObjIDType>;
       using Page = CBTreePage<keyType, ObjIDType>;
public:
       using ObjectInfo = typename Tree::ObjectInfo;
private:
       struct Frame {
               Page *page;
               Size keyIndex;
       };

       Tree *m_tree;
       vector<Frame> m_stack;
       ObjectInfo *m_current = nullptr;
       // acumula las paginas en orden (pre orden)
       void push_leftmost(Page *page) {
               while (page) {
                       m_stack.push_back({page, 0});
                       page = page->m_SubPages[0];
               }
       }
       // las libera y retorna el valor de la clave
       void settle() {
               while (!m_stack.empty()) {
                       Frame &f = m_stack.back();
                       // si no sobrepasa el limite de la pagina, retorna el valor
                       // skip si es una clave vacia
                       if (f.keyIndex < f.page->m_KeyCount) {
                               m_current = &f.page->m_Keys[f.keyIndex];
                               return;
                       }
                       m_stack.pop_back();
               }
               m_current = nullptr;
       }

public:
       ForwardBTreeIterator(Tree *tree, bool is_end) : m_tree(tree) {
               if (!is_end && m_tree && m_tree->m_Root.m_KeyCount > 0) {
                       // agarra las paginas desde la inicializacion
                       push_leftmost(&m_tree->m_Root);
                       settle();
               }
       }

       ObjectInfo &operator*() const { return *m_current; }
       ObjectInfo *operator->() const { return m_current; }

       ForwardBTreeIterator &operator++() {
               if (!m_current || m_stack.empty()) return *this;
               Frame &f = m_stack.back();
               const Size keyIndex = f.keyIndex;
               Page *page = f.page;

               f.keyIndex++;
               if (page->m_SubPages[keyIndex + 1]) {
                       push_leftmost(page->m_SubPages[keyIndex + 1]);
               }
               settle();
               return *this;
       }

       ForwardBTreeIterator operator++(int) {
               ForwardBTreeIterator tmp = *this;
               ++(*this);
               return tmp;
       }

       bool operator==(const ForwardBTreeIterator &other) const {
               return m_tree == other.m_tree && m_current == other.m_current;
       }

       bool operator!=(const ForwardBTreeIterator &other) const {
               return !(*this == other);
       }
};

template <typename keyType, typename ObjIDType>
class BackwardBTreeIterator {
       using Tree = BTree<keyType, ObjIDType>;
       using Page = CBTreePage<keyType, ObjIDType>;
public:
       using ObjectInfo = typename Tree::ObjectInfo;
private:
       struct Frame {
               Page *page;
               Size keyIndex;
       };

       Tree *m_tree;
       vector<Frame> m_stack;
       ObjectInfo *m_current = nullptr;

       void push_rightmost(Page *page) {
               // hace lo mismo, pero en vez de agarrar las paginas desde adelante
               // lo hace desde el final, se obtiene un orden invertido
               while (page) {
                       m_stack.push_back({page, page->m_KeyCount - 1});
                       page = page->m_SubPages[page->m_KeyCount];
               }
       }

       void settle() {
               while (!m_stack.empty()) {
                       Frame &f = m_stack.back();
                       if (f.keyIndex >= 0) {
                               m_current = &f.page->m_Keys[f.keyIndex];
                               return;
                       }
                       m_stack.pop_back();
               }
               m_current = nullptr;
       }

public:
       BackwardBTreeIterator(Tree *tree, bool is_end) : m_tree(tree) {
               if (!is_end && m_tree && m_tree->m_Root.m_KeyCount > 0) {
                       push_rightmost(&m_tree->m_Root);
                       settle();
               }
       }

       ObjectInfo &operator*() const { return *m_current; }
       ObjectInfo *operator->() const { return m_current; }

       BackwardBTreeIterator &operator++() {
               if (!m_current || m_stack.empty()) return *this;
               Frame &f = m_stack.back();
               const Size keyIndex = f.keyIndex;
               Page *page = f.page;

               f.keyIndex--;
               if (page->m_SubPages[keyIndex]) {
                       push_rightmost(page->m_SubPages[keyIndex]);
               }
               settle();
               return *this;
       }

       BackwardBTreeIterator operator++(int) {
               BackwardBTreeIterator tmp = *this;
               ++(*this);
               return tmp;
       }

       bool operator==(const BackwardBTreeIterator &other) const {
               return m_tree == other.m_tree && m_current == other.m_current;
       }

       bool operator!=(const BackwardBTreeIterator &other) const {
               return !(*this == other);
       }
};

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::forward_iterator BTree<keyType, ObjIDType>::begin() {
       return forward_iterator(this, false);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::forward_iterator BTree<keyType, ObjIDType>::end() {
       return forward_iterator(this, true);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::backward_iterator BTree<keyType, ObjIDType>::rbegin() {
       return backward_iterator(this, false);
}

template <typename keyType, typename ObjIDType>
typename BTree<keyType, ObjIDType>::backward_iterator BTree<keyType, ObjIDType>::rend() {
       return backward_iterator(this, true);
}

void DemoBTree();

#endif
