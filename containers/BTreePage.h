#ifndef CBTreePage_H
#define CBTreePage_H
#include <vector>
#include <iostream>
#include <assert.h>
#include "../general/types.h"

template <typename keyType, typename ObjIDType>
class BTree;


using namespace std;

enum bt_ErrorCode { bt_ok, bt_overflow, bt_underflow, bt_duplicate, bt_nofound, bt_rootmerged };

template <typename Traits> class BTree;

//Templates y Traits
template <typename Traits> class BTree;
struct tagObjectInfo {
    using value_type = typename Traits::value_type;
    
    value_type key;
    ref_type ObjID;
    Size UseCounter;

    tagObjectInfo(const value_type& _key, ref_type _ObjID)
        : key(_key), ObjID(_ObjID), UseCounter(0) {}
    
    tagObjectInfo() : UseCounter(0) {}
    
    operator value_type() const { return key; }
    value_type& GetValueRef() { return key; }
};


template <typename Traits> 
class CBTreePage {
       friend class BTree<Traits>;

        using BTPage      = CBTreePage<Traits>;
        using ObjectInfo  = tagObjectInfo<Traits>;
        using value_type  = typename Traits::value_type;
        using Compare     = typename Traits::CompareFunc;

        typedef void (*lpfnForEach2)(ObjectInfo &info, Size level, void *pExtra1);
        typedef ObjectInfo* (*lpfnFirstThat2)(ObjectInfo &info, Size level, void *pExtra1);

public:
    // Copy and Move Constructor  + Destructor
    CBTreePage(Size maxKeys, bool unique = true);
    CBTreePage(const CBTreePage& another);
    CBTreePage(CBTreePage&& another) noexcept;
    virtual ~CBTreePage() { Reset(); }

    bt_ErrorCode Insert(const value_type& key, ref_type ObjID);
    void ForEach(lpfnForEach2 lpfn, Size level, void *pExtra1);
    ObjectInfo* FirstThat(lpfnFirstThat2 lpfn, Size level, void *pExtra1);
    void Print(ostream& os);

    Size SearchInPage(const value_type& key, bool& found) {
        Size pos = 0;
        Compare comp;
        while (pos < m_KeyCount && comp(m_Keys[pos].key, key)) {
            pos++;
        }
        found = (pos < m_KeyCount && m_Keys[pos].key == key);
        return pos;
    }

    // In-Order
    template <typename Func, typename... Args>
    void InOrder(Func fn, Size level, Args&&... args) {
        for (Size i = 0; i < m_KeyCount; i++) {
            if (m_SubPages[i]) m_SubPages[i]->InOrder(fn, level + 1, forward<Args>(args)...);
            fn(m_Keys[i], level, forward<Args>(args)...);
        }
        if (m_SubPages[m_KeyCount]) m_SubPages[m_KeyCount]->InOrder(fn, level + 1, forward<Args>(args)...);
    }
    // Pre-Order
    template <typename Func, typename... Args>
    void PreOrder(Func fn, Size level, Args&&... args) {
        for (Size i = 0; i < m_KeyCount; i++) {
            fn(m_Keys[i], level, forward<Args>(args)...);
            if (m_SubPages[i]) m_SubPages[i]->PreOrder(fn, level + 1, forward<Args>(args)...);
        }
        if (m_SubPages[m_KeyCount]) m_SubPages[m_KeyCount]->PreOrder(fn, level + 1, forward<Args>(args)...);
    }
    // Post-Order
    template <typename Func, typename... Args>
    void PostOrder(Func fn, Size level, Args&&... args) {
        if (m_SubPages[0]) m_SubPages[0]->PostOrder(fn, level + 1, forward<Args>(args)...);
        for (Size i = 0; i < m_KeyCount; i++) {
            if (m_SubPages[i+1]) m_SubPages[i+1]->PostOrder(fn, level + 1, forward<Args>(args)...);
            fn(m_Keys[i], level, forward<Args>(args)...);
        }
    }

    // Print mejorado
    void PrintSide(Size level, ostream& os) {
        for (int i = (int)m_KeyCount; i >= 0; i--) {
            if (m_SubPages[i]) m_SubPages[i]->PrintSide(level + 1, os);
            if (i > 0) {
                os << string(level * 8, ' ') << "|-- [" << m_Keys[i-1].key << "]\n";
            }
        }
    }

    bt_ErrorCode Insert(const value_type& key, const ref_type ObjID);
    bt_ErrorCode Remove(const value_type& key, const ref_type ObjID);

protected:
       Size  m_MinKeys; // minimum number of keys in a node
       Size  m_MaxKeys, // maximum number of keys in a node
                m_MaxKeysForChilds; // just to distinguish the root
       bool m_Unique;
       bool m_isRoot;

       vector<ObjectInfo> m_Keys;
       vector<BTPage *>m_SubPages;
       
       Size  m_KeyCount;
       void  Create();
       void  Reset ();
       void  Destroy () {   Reset(); delete this;}
       void  clear ();

       bool  Redistribute1   (int &pos);
       bool  Redistribute2   (int pos);
       void  RedistributeR2L (int pos);
       void  RedistributeL2R (int pos);

       bool    TreatUnderflow  (int &pos)
       {       return Redistribute1(pos) || Redistribute2(pos);}

       bt_ErrorCode    Merge  (int pos);
       bt_ErrorCode    MergeRoot ();
       void  SplitChild (int pos);

       ObjectInfo &GetFirstObjectInfo();

       bool Overflow()  { return m_KeyCount > m_MaxKeys; }
       bool Underflow() { return m_KeyCount < MinNumberOfKeys(); }
       bool IsFull()    { return m_KeyCount >= m_MaxKeys; }
       Size  MinNumberOfKeys()  { return 2*m_MaxKeys/3.0; }
       Size  GetFreeCells()  { return m_MaxKeys - m_KeyCount; }
       Size& NumberOfKeys()  { return m_KeyCount; }
       Size  GetNumberOfKeys()  { return m_KeyCount; }
       bool IsRoot()  { return m_MaxKeysForChilds != m_MaxKeys; }
       void SetMaxKeysForChilds(Size orderforchilds){m_MaxKeysForChilds = orderforchilds;}

       Size GetFreeCellsOnLeft(Size pos);
       Size GetFreeCellsOnRight(Size pos);

private:
       bool SplitRoot();
       void SplitPageInto3(vector<ObjectInfo>   & tmpKeys,
                                               vector<BTPage *>  & SubPages,
                                               BTPage           *& pChild1,
                                               BTPage           *& pChild2,
                                               BTPage           *& pChild3,
                                               ObjectInfo        & oi1,
                                               ObjectInfo        & oi2);
       void MovePage(BTPage *  pChildPage,vector<ObjectInfo> & tmpKeys,vector<BTPage *> & tmpSubPages);
};

template <typename Container, typename ObjType>
Size binary_search(Container& container, Size first, Size last, ObjType &object)
{
       if( first >= last ) return first;
       while( first < last )
       {
               Size mid = (first+last)/2;
               if( object == (ObjType)container[mid ] )
                       return mid;
               if( object > (ObjType)container[mid ] )
                       first = mid+1;
               else
                       last  = mid;
       }
       if( object <= (ObjType)container[first] )
               return first;
       return last;
}
 
template <typename Container, typename ObjType>
void insert_at(Container& container, const ObjType &object, Size pos)
{
       Size size = container.size();
       for(Size i = size-2 ; i >= pos ; i--)
               container[i+1] = container[i];
       container[pos] =  object;
		
}

template <typename Container>
void remove(Container& container, Size pos)
{
       Size size = container.size();
       for(Size i = pos+1 ; i < size ; i++)
               container[i-1] = container[i];
}

template <typename Traits>
CBTreePage<Traits>::CBTreePage(Size maxKeys, bool unique): m_MaxKeys(maxKeys), m_Unique(unique), m_KeyCount(0) {
       Create();
       SetMaxKeysForChilds(m_MaxKeys);
}

template <typename Traits>
CBTreePage<Traits>::~CBTreePage() {Reset();}

template <typename Traits>
CBTreePage<Traits>::CBTreePage(const CBTreePage<Traits>& another)
    : m_MinKeys(another.m_MinKeys), m_MaxKeys(another.m_MaxKeys),
      m_MaxKeysForChilds(another.m_MaxKeysForChilds), m_Unique(another.m_Unique),
      m_isRoot(another.m_isRoot), m_KeyCount(another.m_KeyCount) 
{
    m_Keys = another.m_Keys;
    m_SubPages.resize(another.m_SubPages.size(), nullptr);
    
    for (Size i = 0; i <= m_KeyCount; i++) {
        if (another.m_SubPages[i]) {
            m_SubPages[i] = new CBTreePage<Traits>(*another.m_SubPages[i]);
        }
    }
}

template <typename Traits>
CBTreePage<Traits>::CBTreePage(CBTreePage<Traits>&& another) noexcept
    : m_MinKeys(another.m_MinKeys), m_MaxKeys(another.m_MaxKeys),
      m_MaxKeysForChilds(another.m_MaxKeysForChilds), m_Unique(another.m_Unique),
      m_isRoot(another.m_isRoot), m_KeyCount(another.m_KeyCount),
      m_Keys(move(another.m_Keys)),
      m_SubPages(move(another.m_SubPages))
{ another.m_KeyCount = 0;}

template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::Insert(const keyType& key, const ObjIDType ObjID)
{
        bool found = false;
        Size pos = SearchInPage(key, found);
        bt_ErrorCode error = bt_ok;

       if( found && m_Unique ) return bt_duplicate;

        if( !m_SubPages[pos] ) {
                ::insert_at(m_Keys, ObjectInfo(key, ObjID), pos);
                m_KeyCount++;
                if( Overflow() ) return bt_overflow;
                return bt_ok;
        }
        else
        {
                // recursive insertion
                error = m_SubPages[pos]->Insert(key, ObjID);
                if( error == bt_overflow )
                {
                        if( !Redistribute1(pos) )
                                SplitChild(pos);
                        if( Overflow() ) return bt_overflow;
                        return bt_ok;
                }
        }
        if( Overflow() ) return bt_overflow;
        return bt_ok;
}


template <typename Traits>
bool CBTreePage<Traits>::Redistribute1(Size &pos)
{
        if( m_SubPages[pos]->Underflow() )
        {
                        Size nkol = 0,
                        nkor = 0;
                // is this the first element or there are more elements on right brother
                if( pos > 0 ) nkol = m_SubPages[pos-1]->NumberOfKeys();
                if( pos < NumberOfKeys() ) nkor = m_SubPages[pos+1]->NumberOfKeys();

                if( nkol > nkor )
                        if( m_SubPages[pos-1]->NumberOfKeys() > m_SubPages[pos-1]->MinNumberOfKeys() )
                                RedistributeL2R(pos-1); // bring elements from left brother
                        else
                                if( pos == NumberOfKeys() )
                                        return (--pos, false);
                                else
                                        return false;
                else //nkol < nkor )
                        if( m_SubPages[pos+1]->NumberOfKeys() > m_SubPages[pos+1]->MinNumberOfKeys() )
                                RedistributeR2L(pos+1); // bring elements from right brother
                        else
                                if( pos == 0 )
                                        return (++pos, false);
                                else
                                        return false;
        }
        else // it is due to overflow
        {
                Size fcol = GetFreeCellsOnLeft(pos),   // Free Cells On Left
                fcor = GetFreeCellsOnRight(pos);  // Free Cells On Right

                if( !fcol && !fcor && m_SubPages[pos]->IsFull() )
                        return false;
                if( fcol > fcor ) // There is more space on left
                        RedistributeR2L(pos);
                else
                        RedistributeL2R(pos);

        }
        return true;
}

// Redistribute2 function
// it considers two brothers m_SubPages[pos-1] && m_SubPages[pos+1]
// if it fails the only way is merge !
template <typename Traits>
bool CBTreePage<Traits>::Redistribute2(Size pos)
{
       assert( pos > 0 && pos < NumberOfKeys()  );
       assert( m_SubPages[pos-1] != 0 && m_SubPages[pos] != 0 && m_SubPages[pos+1] != 0 );
       return true;
}

template <typename Traits>
void CBTreePage<Traits>::RedistributeR2L(Size pos) {
       BTPage  *pSource = m_SubPages[ pos ], *pTarget = m_SubPages[pos-1];

        while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
              pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() ) {
            ::insert_at(pTarget->m_Keys, m_Keys[pos-1], pTarget->NumberOfKeys()++);
            ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[0], pTarget->NumberOfKeys());
            m_Keys[pos-1] = pSource->m_Keys[0];
            ::remove(pSource->m_Keys, 0);
            ::remove(pSource->m_SubPages, 0);
            pSource->NumberOfKeys()--;
       }
}

template <typename Traits>
void CBTreePage<Traits>::RedistributeR2L(Size pos)  
{
       BTPage  *pSource = m_SubPages[ pos ],
                       *pTarget = m_SubPages[pos-1];

       while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
                 pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() )
       {
               // Move from this page to the down-left page \/
               ::insert_at(pTarget->m_Keys, m_Keys[pos-1], pTarget->NumberOfKeys()++);
               // Move the pointer leftest pointer to the rightest position
               ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[0], pTarget->NumberOfKeys());

               // Move the leftest element to the root
               m_Keys[pos-1] = pSource->m_Keys[0];

               // Remove the leftest element from rigth page
               ::remove(pSource->m_Keys    , 0);
               ::remove(pSource->m_SubPages, 0);
               pSource->NumberOfKeys()--;
       }
}

template <typename Traits>
void CBTreePage<Traits>::RedistributeL2R(Size pos)
{
       BTPage  *pSource = m_SubPages[pos],
                       *pTarget = m_SubPages[pos+1];
       while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
                 pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() )
       {
               // Move from this page to the down-RIGHT page \/
               ::insert_at(pTarget->m_Keys, m_Keys[pos], 0);
               // Move the pointer rightest pointer to the leftest position
               ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[pSource->NumberOfKeys()], 0);
               pTarget->NumberOfKeys()++;

               // Move the rightest element to the root
               m_Keys[pos] = pSource->m_Keys[pSource->NumberOfKeys()-1];

               // Remove the leftest element from rigth page
               // it is not necessary erase because m_KeyCount controls
               pSource->NumberOfKeys()--;
       }
}

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::SplitChild(int pos)
{
       // FIRST: deciding the second page to split
        BTPage  *pChild1 = 0, *pChild2 = 0;
        if( pos > 0 )                                   // is left page full ?
                if( m_SubPages[pos-1]->IsFull() )
                {
                        pChild1 = m_SubPages[pos-1];
                        pChild2 = m_SubPages[pos--];
                }
        if( pos < GetNumberOfKeys() )   // is right page full ?
                if( m_SubPages[pos+1]->IsFull() )
                {
                        pChild1 = m_SubPages[pos];
                        pChild2 = m_SubPages[pos+1];
                }

        // SECOND: copy both pages to a temporal one
        // Create two tmp vector
        vector<ObjectInfo> tmpKeys;
        //tmpKeys.resize(nKeys);
        vector<BTPage *>   tmpSubPages;
        //tmpKeys.resize(nKeys+1);

        // copy from left child
        MovePage(pChild1, tmpKeys, tmpSubPages);
        // copy a key from parent
        tmpKeys.push_back(m_Keys[pos]);

        // copy from right child
        MovePage(pChild2, tmpKeys, tmpSubPages);

        BTPage *pChild3 = 0;
        ObjectInfo oi1, oi2;
        SplitPageInto3(tmpKeys, tmpSubPages, pChild1, pChild2, pChild3, oi1, oi2);

        // copy the first element to the root
        m_Keys    [pos] = oi1;
        m_SubPages[pos] = pChild1;

        // copy the second element to the root
        ::insert_at(m_Keys, oi2, pos+1);
        ::insert_at(m_SubPages, pChild2, pos+1);
        NumberOfKeys()++;

        m_SubPages[pos+2] = pChild3;
}

template <typename Traits>
void CBTreePage<Traits>::SplitPageInto3(vector<ObjectInfo>& tmpKeys,
                                                vector<BTPage *>  & tmpSubPages,
                                                BTPage*                   &     pChild1,
                                                BTPage*                   &     pChild2,
                                                BTPage*                   &     pChild3,
                                                ObjectInfo                & oi1,
                                                ObjectInfo                & oi2)
{
       assert(tmpKeys.size() >= 8);
       assert(tmpSubPages.size() >= 9);
       if( !pChild1 )
               pChild1 = new BTPage(m_MaxKeysForChilds, m_Unique);

       // Split tmpKeys page into 3 pages
       // copy 1/3 elements to the first child
       pChild1->clear();
       Size nKeys = (tmpKeys.size()-2)/3;
       Size i = 0;
       for( ; i < nKeys; i++ )
       {
               pChild1->m_Keys    [i] = tmpKeys    [i];
               pChild1->m_SubPages[i] = tmpSubPages[i];
               pChild1->NumberOfKeys()++;
       }
       pChild1->m_SubPages[i] = tmpSubPages[i];

       // first element to go up !
       oi1 = tmpKeys[i++];

       if( !pChild2 )
               pChild2 = new BTPage(m_MaxKeysForChilds, m_Unique);
       pChild2->clear();
       // copy 1/3 to the second child
       nKeys += (tmpKeys.size()-2)/3 + 1;
       int j = 0;
       for(; i < nKeys; i++, j++ )
       {
               pChild2->m_Keys    [j] = tmpKeys    [i];
               pChild2->m_SubPages[j] = tmpSubPages[i];
               pChild2->NumberOfKeys()++;
       }
       pChild2->m_SubPages[j] = tmpSubPages[i];

       // copy the second element to the root
       oi2 = tmpKeys[i++];

       // copy 1/3 to the third child
       if( !pChild3 )
               pChild3 = new BTPage(m_MaxKeysForChilds, m_Unique);
       pChild3->clear();
       nKeys = tmpKeys.size();
       for(j = 0; i < nKeys; i++, j++)
       {
               pChild3->m_Keys    [j] = tmpKeys    [i];
               pChild3->m_SubPages[j] = tmpSubPages[i];
               pChild3->NumberOfKeys()++;
       }
       pChild3->m_SubPages[j] = tmpSubPages[i];
}

template <typename Traits>
bool CBTreePage<Traits>::SplitRoot()
{
        BTPage  *pChild1 = 0, *pChild2 = 0, *pChild3 = 0;
        ObjectInfo oi1, oi2;
        SplitPageInto3( m_Keys,m_SubPages,pChild1, pChild2, pChild3, oi1, oi2);
        clear();

        // copy the first element to the root
        m_Keys    [0] = oi1;
        m_SubPages[0] = pChild1;
        NumberOfKeys()++;

        // copy the second element to the root
        m_Keys    [1] = oi2;
        m_SubPages[1] = pChild2;
        NumberOfKeys()++;

        m_SubPages[2] = pChild3;
        return true;
}

template <typename Traits>
bool CBTreePage<Traits>::Search(const value_type &key, ref_type &ObjID)
{
       bool found = false;
        Size pos = SearchInPage(key, found);

        if (found) {
                ObjID = m_Keys[pos].ObjID;
                m_Keys[pos].UseCounter++;
                return true;
        }

        if (m_SubPages[pos]) 
                return m_SubPages[pos]->Search(key, ObjID);
        
        return false;
}

/*template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::ForEachReverse(lpfnForEach2 lpfn, int level, void *pExtra1)
{
       if( m_SubPages[m_KeyCount] )
               m_SubPages[m_KeyCount]->ForEach(lpfn, level+1, pExtra1);
       for( int i = m_KeyCount-1 ; i >= 0  ; i--)
       {
               lpfn(m_Keys[i], level, pExtra1);
               if( m_SubPages[i] )
                       m_SubPages[i]->ForEach(lpfn, level+1, pExtra1);
       }
}*/

template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::ForEach(lpfnForEach2 lpfn, Size level, void *pExtra1)
{
       for(Size i = 0 ; i < m_KeyCount ; i++)
       {
               if( m_SubPages[i] )
                       m_SubPages[i]->ForEach(lpfn, level+1, pExtra1);
        }
}
template <typename keyType, typename ObjIDType>
void CBTreePage<keyType, ObjIDType>::ForEach(lpfnForEach3 lpfn, Size level, void *pExtra1, void *pExtra2)
{
       for(Size i = 0 ; i < m_KeyCount ; i++)
       {
               if( m_SubPages[i] )
                       m_SubPages[i]->ForEach(lpfn, level+1, pExtra1, pExtra2);
       }
}

template <typename keyType, typename ObjIDType>
typename CBTreePage<keyType, ObjIDType>::ObjectInfo *
CBTreePage<keyType, ObjIDType>::FirstThat(lpfnFirstThat2 lpfn,
                                          Size level, void *pExtra1)
{
       ObjectInfo *pTmp;
       for(Size i = 0 ; i < m_KeyCount ; i++)
       {
               if( m_SubPages[i] ){
                        pTmp = m_SubPages[i]->FirstThat(lpfn, level+1, pExtra1);
                       if( pTmp )
                               return pTmp;
               }
               if( lpfn(m_Keys[i], level, pExtra1) )
                       return &m_Keys[i];
       }
       if( m_SubPages[m_KeyCount] ){
                pTmp = m_SubPages[m_KeyCount]->FirstThat(lpfn, level+1, pExtra1);
               if( pTmp )
                       return pTmp;
       }
       return 0;
}

template <typename keyType, typename ObjIDType>
typename CBTreePage<keyType, ObjIDType>::ObjectInfo *
CBTreePage<keyType, ObjIDType>::FirstThat(lpfnFirstThat3 lpfn, Size level, void *pExtra1, void *pExtra2)
{
       ObjectInfo *pTmp;
       for(Size i = 0 ; i < m_KeyCount ; i++){
               if( m_SubPages[i] ){
                       pTmp = m_SubPages[i]->FirstThat(lpfn, level+1, pExtra1, pExtra2);
                       if( pTmp )
                           return pTmp;
               }
               if( lpfn(m_Keys[i], level, pExtra1, pExtra2) )
                       return &m_Keys[i];
       }
        if( m_SubPages[m_KeyCount] )
        {       pTmp = m_SubPages[m_KeyCount]->FirstThat(lpfn, level+1, pExtra1, pExtra2);
                if( pTmp ) 
                return pTmp;
        }
        return 0;
}

template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::Remove(const value_type &key, const ref_type ObjID) {
       bt_ErrorCode error = bt_ok;
       bool found = false;
       Size pos = SearchInPage(key, found);

       if( found ) {
                if( !m_SubPages[pos+1] ) {  
                        ::remove(m_Keys, pos);
                        NumberOfKeys()--;
                        if( Underflow() ) return bt_underflow;
                        return bt_ok;
                }
                ObjectInfo &rFirstFromRight = m_SubPages[pos+1]->GetFirstObjectInfo();
                swap(m_Keys[pos], rFirstFromRight);
                error = m_SubPages[++pos]->Remove(key, ObjID);
        }
        else { 
                if( m_SubPages[pos] ) error = m_SubPages[pos]->Remove(key, ObjID);
                else return bt_nofound;
        }
        
        if( error == bt_underflow ) {
                if( TreatUnderflow(pos) ) return bt_ok;
                if( IsRoot() && NumberOfKeys() == 2 ) return MergeRoot();
                return Merge(pos);
        }
        if( error == bt_nofound ) return bt_nofound;
        return bt_ok;
}


template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::Merge(Size pos)
{
       assert( m_SubPages[pos-1]->NumberOfKeys() +
                       m_SubPages[ pos ]->NumberOfKeys() +
                       m_SubPages[pos+1]->NumberOfKeys() ==
                       3*m_SubPages[ pos ]->MinNumberOfKeys() - 1);

       // FIRST: Put all the elements into a vector
       vector<ObjectInfo> tmpKeys;
       //tmpKeys.resize(nKeys);
       vector<BTPage *>   tmpSubPages;

       BTPage  *pChild1 = m_SubPages[pos-1],
                       *pChild2 = m_SubPages[ pos ],
                       *pChild3 = m_SubPages[pos+1];
       MovePage(pChild1, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[pos-1]);
       MovePage(pChild2, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[ pos ]);
       MovePage(pChild3, tmpKeys, tmpSubPages);
       pChild3->Destroy();;

       // Move 1/2 elements to pChild1
       Size nKeys = pChild1->GetFreeCells();
       Size i = 0;
       for( ; i < nKeys ; i++ )
       {
               pChild1->m_Keys    [i] = tmpKeys    [i];
               pChild1->m_SubPages[i] = tmpSubPages[i];
               pChild1->NumberOfKeys()++;
       }
       pChild1->m_SubPages[i] = tmpSubPages[i];

       m_Keys    [pos-1] = tmpKeys[i];
       m_SubPages[pos-1] = pChild1;

       ::remove(m_Keys    , pos);
       ::remove(m_SubPages, pos);
       NumberOfKeys()--;

       nKeys = pChild2->GetFreeCells();
       Size j = ++i;
       for(i = 0 ; i < nKeys ; i++, j++ )
       {
               pChild2->m_Keys    [i] = tmpKeys    [j];
               pChild2->m_SubPages[i] = tmpSubPages[j];
               pChild2->NumberOfKeys()++;
       }
       pChild2->m_SubPages[i] = tmpSubPages[j];
       m_SubPages[ pos ]          = pChild2;

       if( Underflow() )
               return bt_underflow;
       return bt_ok;
}

template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::MergeRoot()
{
       Size pos = 1;
       assert( m_SubPages[pos-1]->NumberOfKeys() +
                       m_SubPages[ pos ]->NumberOfKeys() +
                       m_SubPages[pos+1]->NumberOfKeys() ==
                       3*m_SubPages[ pos ]->MinNumberOfKeys() - 1);

       BTPage  *pChild1 = m_SubPages[pos-1], *pChild2 = m_SubPages[ pos ], *pChild3 = m_SubPages[pos+1];
       Size nKeys = pChild1->NumberOfKeys() + pChild2->NumberOfKeys() + pChild3->NumberOfKeys() + 2;

       // FIRST: Put all the elements into a vector
       vector<ObjectInfo> tmpKeys;
       //tmpKeys.resize(nKeys);
       vector<BTPage *>   tmpSubPages;

       MovePage(pChild1, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[pos-1]);
       MovePage(pChild2, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[ pos ]);
       MovePage(pChild3, tmpKeys, tmpSubPages);

       clear();
       Size i = 0;
       for( ; i < nKeys ; i++ ){
               m_Keys    [i] = tmpKeys    [i];
               m_SubPages[i] = tmpSubPages[i];
               NumberOfKeys()++;
       }
       m_SubPages[i] = tmpSubPages[i];

       //Print(cout);
       pChild1->Destroy();
       pChild2->Destroy();
       pChild3->Destroy();

       return bt_rootmerged;
}

template <typename Traits>
typename CBTreePage<Traits>::ObjectInfo &
CBTreePage<Traits>::GetFirstObjectInfo() {
       if( m_SubPages[0] )
               return m_SubPages[0]->GetFirstObjectInfo();
       return m_Keys[0];
}

template <typename Traits>
void CBTreePage<Traits>::Print(ostream & os) {
    auto printFn = [](ObjectInfo& info, Size level, ostream& out) {
        for (Size i = 0; i < level; i++) out << "\t";
        if (level > 0) out << " |-- ";
        out << "[" << info.key << "]" << "\n";
    };   
    InOrder(printFn, 0, os);
}

template <typename Traits>
void CBTreePage<Traits>::Create() {
       Reset();
       m_Keys.resize(m_MaxKeys+1);
       m_SubPages.resize(m_MaxKeys+2, nullptr);
       m_KeyCount = 0;
       m_MinKeys  = MinNumberOfKeys();
}

template <typename Traits>
void CBTreePage<Traits>::Reset() {
        if (m_SubPages.empty()) {
           clear();
           return;
       }

        for( Size i = 0 ; i <= m_KeyCount ; i++ ) {
                if (m_SubPages[i])
                        delete m_SubPages[i];
        }

        for( Size i = 0 ; i < (Size)m_SubPages.size() ; i++ ) {
               m_SubPages[i] = nullptr;
        }
        clear();
}

template <typename Traits>
void CBTreePage<Traits>::clear()
{
       //m_Keys.clear();
       //m_SubPages.clear();
       m_KeyCount = 0;
}

template <typename Traits>
CBTreePage<Traits> * CreateBTreeNode (Size maxKeys, Size unique) {
       return new CBTreePage<Traits> (maxKeys, unique);
}

template <typename Traits>
void CBTreePage<Traits>::MovePage(BTPage *pChildPage, vector<ObjectInfo> &tmpKeys,vector<BTPage *> &tmpSubPages)
{
       Size nKeys = pChildPage->GetNumberOfKeys();
       Size i = 0;
       for( ; i < nKeys; i++ )
       {
               tmpKeys    .push_back(pChildPage->m_Keys[i]);
               tmpSubPages.push_back(pChildPage->m_SubPages[i]);
       }
       tmpSubPages.push_back(pChildPage->m_SubPages[i]);
       pChildPage->clear();
}

template <typename Traits>
Size CBTreePage<Traits>::GetFreeCellsOnLeft(Size pos)
{
       if( pos > 0 )                                   // there is some page on left ?
               return m_SubPages[pos-1]->GetFreeCells();
       return 0;
}

template <typename Traits>
Size CBTreePage<Traits>::GetFreeCellsOnRight(Size pos)
{
       if( pos < GetNumberOfKeys() )   // there is some page on right ?
               return m_SubPages[pos+1]->GetFreeCells();
       return 0;
}

#endif //__CBTreePage_H__