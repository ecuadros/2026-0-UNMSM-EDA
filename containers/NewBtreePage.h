#ifndef CBNEWTreePage_H
#define CBNEWTreePage_H

#include <vector>
#include <iostream>
#include <assert.h>
#include "general/types.h" 

template <typename keyType, typename ObjIDType>
class BNEWTree;

using namespace std;

enum bt_ErrorCode {bt_ok, bt_overflow, bt_underflow, bt_duplicate, bt_nofound, bt_rootmerged};

template <typename keyType, typename ObjIDType>
struct tagObjectInfo
{
       keyType                 key;
       ObjIDType               ObjID;
       long                    UseCounter;
       tagObjectInfo(const keyType     &_key, ObjIDType _ObjID)
               : key(_key), ObjID(_ObjID), UseCounter(0) {}
       tagObjectInfo()                          {}
       operator keyType                         ()     { return key; }
       long                    GetUseCounter() { return UseCounter;    }
};

template <typename keyType, typename ObjIDType>
class CBNEWTreePage 
{
       friend class BTree<keyType, ObjIDType>;

       typedef CBNEWTreePage<keyType, ObjIDType>    BTPage;
       typedef tagObjectInfo<keyType, ObjIDType> ObjectInfo;

       template <typename... Args>
       using lpfnForEach = void (*)(ObjectInfo &info, Size level, Args... args);

       template <typename... Args>
       using lpfnFirstThat = ObjectInfo *(*)(ObjectInfo &info, Size level, Args... args);

 public:
       CBNEWTreePage(Size maxKeys, bool unique = true);
       
       // --- Copy Constructor y Move Constructor (Requeridos) ---
       CBNEWTreePage(const CBNEWTreePage& other);
       CBNEWTreePage& operator=(const CBNEWTreePage& other);
       CBNEWTreePage(CBNEWTreePage&& other) noexcept;
       CBNEWTreePage& operator=(CBNEWTreePage&& other) noexcept;

       // --- Destructor Virtual (Requerido) ---
       virtual ~CBNEWTreePage();

       bt_ErrorCode    Insert (const keyType &key, const ObjIDType ObjID);
       bt_ErrorCode    Remove (const keyType &key, const ObjIDType ObjID);
       bool            Search (const keyType &key, ObjIDType &ObjID); 

       // --- Recorridos Variadic (Requeridos) ---
       template <typename... Args>
       void            InOrder(lpfnForEach<Args...> lpfn, Size level, Args... args);
       
       template <typename... Args>
       void            PreOrder(lpfnForEach<Args...> lpfn, Size level, Args... args);
       
       template <typename... Args>
       void            PostOrder(lpfnForEach<Args...> lpfn, Size level, Args... args);

       template <typename... Args>
       void            ForEach(lpfnForEach<Args...> lpfn, Size level, Args... args) {
           InOrder(lpfn, level, args...);
       }
       
       // --- FirstThat Variadic (Requerido) ---
       template <typename... Args>
       ObjectInfo* FirstThat(lpfnFirstThat<Args...> lpfn, Size level, Args... args);

protected:
       Size m_MinKeys; 
       Size m_MaxKeys, 
            m_MaxKeysForChilds; 
       bool m_Unique;
       bool m_isRoot;
       vector<ObjectInfo> m_Keys;
       vector<BTPage *>m_SubPages;
       Size m_KeyCount;
       
       void  Create();
       void  Reset ();
       void  Destroy () {   Reset(); delete this;}
       void  clear ();
       void  CopyFrom(const CBNEWTreePage& other); 

       bool  Redistribute1   (Size &pos);
       bool  Redistribute2   (Size pos);
       void  RedistributeR2L (Size pos);
       void  RedistributeL2R (Size pos);

       bool    TreatUnderflow  (Size &pos)
       {       return Redistribute1(pos) || Redistribute2(pos);}

       bt_ErrorCode    Merge  (Size pos);
       bt_ErrorCode    MergeRoot ();
       void  SplitChild (Size pos);

       ObjectInfo &GetFirstObjectInfo();

       bool Overflow()  { return m_KeyCount > m_MaxKeys; }
       bool Underflow() { return m_KeyCount < MinNumberOfKeys(); }
       bool IsFull()    { return m_KeyCount >= m_MaxKeys; }
       Size MinNumberOfKeys()  { return (2 * m_MaxKeys) / 3; }
       Size GetFreeCells()  { return m_MaxKeys - m_KeyCount; }
       Size& NumberOfKeys()  { return m_KeyCount; }
       Size GetNumberOfKeys()  { return m_KeyCount; }
       bool IsRoot()  { return m_MaxKeysForChilds != m_MaxKeys; }
       void SetMaxKeysForChilds(Size orderforchilds)
       {
               m_MaxKeysForChilds = orderforchilds;
       }

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
       void MovePage(BTPage * pChildPage,vector<ObjectInfo> & tmpKeys,vector<BTPage *> & tmpSubPages);
};

template <typename Container, typename ObjType>
Size binary_search(Container& container, Size first, Size last, ObjType &object)
{
       if( first >= last )
               return first;
       while( first < last )
       {
               Size mid = first + (last - first) / 2;
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
       for(Size i = size - 1 ; i > pos ; i--)
               container[i] = container[i-1];
       container[pos] =  object;
}

template <typename Container>
void remove(Container& container, Size pos)
{
       Size size = container.size();
       for(Size i = pos + 1 ; i < size ; i++)
               container[i-1] = container[i];
}

template <typename keyType, typename ObjIDType>
CBNEWTreePage<keyType, ObjIDType>::CBNEWTreePage(Size maxKeys, bool unique)
                                       : m_MaxKeys(maxKeys), m_Unique(unique), m_KeyCount(0)
{
       Create();
       SetMaxKeysForChilds(m_MaxKeys);
}

// --- Implementación: Copy Constructor (Requerido) ---
template <typename keyType, typename ObjIDType>
CBNEWTreePage<keyType, ObjIDType>::CBNEWTreePage(const CBNEWTreePage& other) : m_MaxKeys(other.m_MaxKeys), m_Unique(other.m_Unique), m_KeyCount(0) 
{
    Create();
    CopyFrom(other);
}

template <typename keyType, typename ObjIDType>
CBNEWTreePage<keyType, ObjIDType>& CBNEWTreePage<keyType, ObjIDType>::operator=(const CBNEWTreePage& other) 
{
    if(this != &other) { 
        Reset(); 
        CopyFrom(other); 
    }
    return *this;
}

template <typename keyType, typename ObjIDType>
void CBNEWTreePage<keyType, ObjIDType>::CopyFrom(const CBNEWTreePage& other) 
{
    m_MinKeys = other.m_MinKeys;
    m_MaxKeys = other.m_MaxKeys;
    m_MaxKeysForChilds = other.m_MaxKeysForChilds;
    m_Unique = other.m_Unique;
    m_isRoot = other.m_isRoot;
    m_KeyCount = other.m_KeyCount;
    m_Keys = other.m_Keys;
    
    m_SubPages.resize(other.m_SubPages.size(), nullptr);
    for(Size i = 0; i < other.m_SubPages.size(); ++i) {
        if(other.m_SubPages[i]) {
            m_SubPages[i] = new CBNEWTreePage(other.m_MaxKeys, other.m_Unique);
            m_SubPages[i]->CopyFrom(*other.m_SubPages[i]);
        }
    }
}

// --- Implementación: Move Constructor (Requerido) ---
template <typename keyType, typename ObjIDType>
CBNEWTreePage<keyType, ObjIDType>::CBNEWTreePage(CBNEWTreePage&& other) noexcept 
{
    m_MinKeys = other.m_MinKeys;
    m_MaxKeys = other.m_MaxKeys;
    m_MaxKeysForChilds = other.m_MaxKeysForChilds;
    m_Unique = other.m_Unique;
    m_isRoot = other.m_isRoot;
    m_KeyCount = other.m_KeyCount;
    m_Keys = std::move(other.m_Keys);
    m_SubPages = std::move(other.m_SubPages);
    
    other.m_KeyCount = 0;
    other.m_Keys.clear();
    other.m_SubPages.clear();
}

template <typename keyType, typename ObjIDType>
CBNEWTreePage<keyType, ObjIDType>& CBNEWTreePage<keyType, ObjIDType>::operator=(CBNEWTreePage&& other) noexcept 
{
    if(this != &other) {
        Reset();
        m_MinKeys = other.m_MinKeys;
        m_MaxKeys = other.m_MaxKeys;
        m_MaxKeysForChilds = other.m_MaxKeysForChilds;
        m_Unique = other.m_Unique;
        m_isRoot = other.m_isRoot;
        m_KeyCount = other.m_KeyCount;
        m_Keys = std::move(other.m_Keys);
        m_SubPages = std::move(other.m_SubPages);
        
        other.m_KeyCount = 0;
        other.m_Keys.clear();
        other.m_SubPages.clear();
    }
    return *this;
}

template <typename keyType, typename ObjIDType>
CBNEWTreePage<keyType, ObjIDType>::~CBNEWTreePage()
{
       Reset();
}

template <typename keyType, typename ObjIDType>
bt_ErrorCode CBNEWTreePage<keyType, ObjIDType>::Insert(const keyType& key, const ObjIDType ObjID)
{
       Size pos = binary_search(m_Keys, 0, m_KeyCount, key);
       bt_ErrorCode error = bt_ok;

       if( pos < m_KeyCount && (keyType)m_Keys[pos] == key && m_Unique)
               return bt_duplicate; 

       if( !m_SubPages[pos] ) 
       {
               ::insert_at(m_Keys, ObjectInfo(key, ObjID), pos);
               m_KeyCount++;
               if( Overflow() )
                       return bt_overflow;
               return bt_ok;
       }
       else
       {
               error = m_SubPages[pos]->Insert(key, ObjID);
               if( error == bt_overflow )
               {
                       if( !Redistribute1(pos) )
                               SplitChild(pos);
                       if( Overflow() )          
                               return bt_overflow;
                       return bt_ok;
               }
       }
       if( Overflow() ) 
               return bt_overflow;
       return bt_ok;
}

template <typename keyType, typename ObjIDType>
bool CBNEWTreePage<keyType, ObjIDType>::Redistribute1(Size &pos)
{
       if( m_SubPages[pos]->Underflow() )
       {
               Size nkol = 0, nkor = 0;
               if( pos > 0 )
                       nkol = m_SubPages[pos-1]->NumberOfKeys();
               if( pos < NumberOfKeys() )
                       nkor = m_SubPages[pos+1]->NumberOfKeys();

               if( nkol > nkor )
                       if( m_SubPages[pos-1]->NumberOfKeys() > m_SubPages[pos-1]->MinNumberOfKeys() )
                               RedistributeL2R(pos-1); 
                       else
                               if( pos == NumberOfKeys() )
                                       return (--pos, false);
                               else
                                       return false;
               else 
                       if( m_SubPages[pos+1]->NumberOfKeys() > m_SubPages[pos+1]->MinNumberOfKeys() )
                               RedistributeR2L(pos+1); 
                       else
                               if( pos == 0 )
                                       return (++pos, false);
                               else
                                       return false;
       }
       else 
       {
               Size fcol = GetFreeCellsOnLeft(pos),  
                    fcor = GetFreeCellsOnRight(pos);  

               if( !fcol && !fcor && m_SubPages[pos]->IsFull() )
                       return false;
               if( fcol > fcor ) 
                       RedistributeR2L(pos);
               else
                       RedistributeL2R(pos);
       }
       return true;
}

template <typename keyType, typename ObjIDType>
bool CBNEWTreePage<keyType, ObjIDType>::Redistribute2(Size pos)
{
       assert( pos > 0 && pos < NumberOfKeys()  );
       assert( m_SubPages[pos-1] != 0 && m_SubPages[pos] != 0 && m_SubPages[pos+1] != 0 );
       assert( m_SubPages[pos-1]->Underflow() ||
                       m_SubPages[ pos ]->Underflow() ||
                       m_SubPages[pos+1]->Underflow() );

       if( m_SubPages[pos-1]->Underflow() )
       {       
               RedistributeR2L(pos+1);
               RedistributeR2L(pos);
               if( m_SubPages[pos-1]->Underflow() )
                       return false;
       }
       else if( m_SubPages[pos+1]->Underflow() )
       {       
               RedistributeL2R(pos-1);
               RedistributeL2R(pos);
               if( m_SubPages[pos+1]->Underflow() )
                       return false;
       }
       else 
       {
               RedistributeL2R(pos-1);
               RedistributeR2L(pos+1);
               if( m_SubPages[pos]->Underflow() )
                       return false;
       }
       return true;
}

template <typename keyType, typename ObjIDType>
void CBNEWTreePage<keyType, ObjIDType>::RedistributeR2L(Size pos)  
{
       BTPage  *pSource = m_SubPages[ pos ],
                       *pTarget = m_SubPages[pos-1];

       while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
                 pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() )
       {
               ::insert_at(pTarget->m_Keys, m_Keys[pos-1], pTarget->NumberOfKeys()++);
               ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[0], pTarget->NumberOfKeys());

               m_Keys[pos-1] = pSource->m_Keys[0];

               ::remove(pSource->m_Keys    , 0);
               ::remove(pSource->m_SubPages, 0);
               pSource->NumberOfKeys()--;
       }
}

template <typename keyType, typename ObjIDType>
void CBNEWTreePage<keyType, ObjIDType>::RedistributeL2R(Size pos)
{
       BTPage  *pSource = m_SubPages[pos],
                       *pTarget = m_SubPages[pos+1];
       while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
                 pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() )
       {
               ::insert_at(pTarget->m_Keys, m_Keys[pos], 0);
               ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[pSource->NumberOfKeys()], 0);
               pTarget->NumberOfKeys()++;

               m_Keys[pos] = pSource->m_Keys[pSource->NumberOfKeys()-1];
               pSource->NumberOfKeys()--;
       }
}

template <typename keyType, typename ObjIDType>
void CBNEWTreePage<keyType, ObjIDType>::SplitChild(Size pos)
{
       BTPage  *pChild1 = 0, *pChild2 = 0;
       if( pos > 0 )                                   
               if( m_SubPages[pos-1]->IsFull() )
               {
                       pChild1 = m_SubPages[pos-1];
                       pChild2 = m_SubPages[pos--];
               }
       if( pos < GetNumberOfKeys() )   
               if( m_SubPages[pos+1]->IsFull() )
               {
                       pChild1 = m_SubPages[pos];
                       pChild2 = m_SubPages[pos+1];
               }

       vector<ObjectInfo> tmpKeys;
       vector<BTPage *>   tmpSubPages;

       MovePage(pChild1, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[pos]);

       MovePage(pChild2, tmpKeys, tmpSubPages);

       BTPage *pChild3 = 0;
       ObjectInfo oi1, oi2;
       SplitPageInto3(tmpKeys, tmpSubPages, pChild1, pChild2, pChild3, oi1, oi2);

       m_Keys    [pos] = oi1;
       m_SubPages[pos] = pChild1;

       ::insert_at(m_Keys, oi2, pos+1);
       ::insert_at(m_SubPages, pChild2, pos+1);
       NumberOfKeys()++;

       m_SubPages[pos+2] = pChild3;
}

template <typename keyType, typename ObjIDType>
void CBNEWTreePage<keyType, ObjIDType>::SplitPageInto3(vector<ObjectInfo>& tmpKeys,
                                                vector<BTPage *>  & tmpSubPages,
                                                BTPage* &     pChild1,
                                                BTPage* &     pChild2,
                                                BTPage* &     pChild3,
                                                ObjectInfo                & oi1,
                                                ObjectInfo                & oi2)
{
       assert(tmpKeys.size() >= 8);
       assert(tmpSubPages.size() >= 9);
       if( !pChild1 )
               pChild1 = new BTPage(m_MaxKeysForChilds, m_Unique);

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

       oi1 = tmpKeys[i++];

       if( !pChild2 )
               pChild2 = new BTPage(m_MaxKeysForChilds, m_Unique);
       pChild2->clear();
       
       nKeys += (tmpKeys.size()-2)/3 + 1;
       Size j = 0;
       for(; i < nKeys; i++, j++ )
       {
               pChild2->m_Keys    [j] = tmpKeys    [i];
               pChild2->m_SubPages[j] = tmpSubPages[i];
               pChild2->NumberOfKeys()++;
       }
       pChild2->m_SubPages[j] = tmpSubPages[i];

       oi2 = tmpKeys[i++];

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

template <typename keyType, typename ObjIDType>
bool CBNEWTreePage<keyType, ObjIDType>::SplitRoot()
{
       BTPage  *pChild1 = 0, *pChild2 = 0, *pChild3 = 0;
       ObjectInfo oi1, oi2;
       SplitPageInto3( m_Keys,m_SubPages,pChild1, pChild2, pChild3, oi1, oi2);
       clear();

       m_Keys    [0] = oi1;
       m_SubPages[0] = pChild1;
       NumberOfKeys()++;

       m_Keys    [1] = oi2;
       m_SubPages[1] = pChild2;
       NumberOfKeys()++;

       m_SubPages[2] = pChild3;
       return true;
}

template <typename keyType, typename ObjIDType>
bool CBNEWTreePage<keyType, ObjIDType>::Search(const keyType &key, ObjIDType &ObjID)
{
       Size pos = binary_search(m_Keys, 0, m_KeyCount, key);
       if( pos >= m_KeyCount ){
               if( m_SubPages[pos] )
                       return m_SubPages[pos]->Search(key, ObjID);
               else
                       return false;
       }
       if( key == m_Keys[pos].key )
       {
               ObjID = m_Keys[pos].ObjID;
               m_Keys[pos].UseCounter++;
               return true;
       }
       if( key < m_Keys[pos].key )
               if( m_SubPages[pos] )
                       return m_SubPages[pos]->Search(key, ObjID);
       return false;
}

// --- Implementación: Recorridos Variadic (Requeridos) ---

template <typename keyType, typename ObjIDType>
template <typename... Args>
void CBNEWTreePage<keyType, ObjIDType>::InOrder(lpfnForEach<Args...> lpfn, Size level, Args... args)
{
       for( Size i = 0 ; i < m_KeyCount ; i++)
       {
               if( m_SubPages[i] )
                       m_SubPages[i]->template InOrder<Args...>(lpfn, level+1, args...);
               lpfn(m_Keys[i], level, args...);
       }
       if( m_SubPages[m_KeyCount] )
               m_SubPages[m_KeyCount]->template InOrder<Args...>(lpfn, level+1, args...);
}

template <typename keyType, typename ObjIDType>
template <typename... Args>
void CBNEWTreePage<keyType, ObjIDType>::PreOrder(lpfnForEach<Args...> lpfn, Size level, Args... args)
{
       for( Size i = 0 ; i < m_KeyCount ; i++) {
               lpfn(m_Keys[i], level, args...);
       }
       for( Size i = 0 ; i <= m_KeyCount ; i++) {
               if( m_SubPages[i] )
                       m_SubPages[i]->template PreOrder<Args...>(lpfn, level+1, args...);
       }
}

template <typename keyType, typename ObjIDType>
template <typename... Args>
void CBNEWTreePage<keyType, ObjIDType>::PostOrder(lpfnForEach<Args...> lpfn, Size level, Args... args)
{
       for( Size i = 0 ; i <= m_KeyCount ; i++) {
               if( m_SubPages[i] )
                       m_SubPages[i]->template PostOrder<Args...>(lpfn, level+1, args...);
       }
       for( Size i = 0 ; i < m_KeyCount ; i++) {
               lpfn(m_Keys[i], level, args...);
       }
}

// --- Implementación: FirstThat Variadic (Requerido) ---

template <typename keyType, typename ObjIDType>
template <typename... Args>
typename CBNEWTreePage<keyType, ObjIDType>::ObjectInfo *
CBNEWTreePage<keyType, ObjIDType>::FirstThat(lpfnFirstThat<Args...> lpfn, Size level, Args... args)
{
       ObjectInfo *pTmp;
       for( Size i = 0 ; i < m_KeyCount ; i++){
               if( m_SubPages[i] ){
                       pTmp = m_SubPages[i]->template FirstThat<Args...>(lpfn, level+1, args...);
                       if( pTmp )
                           return pTmp;
               }
               if( lpfn(m_Keys[i], level, args...) )
                       return &m_Keys[i];
       }
        if( m_SubPages[m_KeyCount] )
        {       
                pTmp = m_SubPages[m_KeyCount]->template FirstThat<Args...>(lpfn, level+1, args...);
                if( pTmp ) 
                    return pTmp;
        }
        return nullptr;
}


template <typename keyType, typename ObjIDType>
bt_ErrorCode CBNEWTreePage<keyType, ObjIDType>::Remove(const keyType &key, const ObjIDType ObjID)
{
       bt_ErrorCode error = bt_ok;
       Size pos = binary_search(m_Keys, 0, m_KeyCount, key);
       if( pos < NumberOfKeys() && key == m_Keys[pos].key ) 
       {
               if( !m_SubPages[pos+1] )  
               {
                       ::remove(m_Keys, pos);
                       NumberOfKeys()--;
                       if( Underflow() )
                               return bt_underflow;
                       return bt_ok;
               }

               {
                       ObjectInfo &rFirstFromRight = m_SubPages[pos+1]->GetFirstObjectInfo();
                       swap(m_Keys[pos], rFirstFromRight);
                       error = m_SubPages[++pos]->Remove(key, ObjID);
               }
       }
       else if( pos == NumberOfKeys() ) 
               error = m_SubPages[pos]->Remove(key, ObjID);
       else if( key <= m_Keys[pos].key ){ 
               if( m_SubPages[pos] )
                       error = m_SubPages[pos]->Remove(key, ObjID);
               else
                       return bt_nofound;
       }
       if( error == bt_underflow ){
               if( TreatUnderflow(pos) )
                       return bt_ok;
               if( IsRoot() && NumberOfKeys() == 2 )
                       return MergeRoot();
               return Merge(pos);
       }
       if( error == bt_nofound )
               return bt_nofound;
       return bt_ok;
}

template <typename keyType, typename ObjIDType>
bt_ErrorCode CBNEWTreePage<keyType, ObjIDType>::Merge(Size pos)
{
       assert( m_SubPages[pos-1]->NumberOfKeys() +
                       m_SubPages[ pos ]->NumberOfKeys() +
                       m_SubPages[pos+1]->NumberOfKeys() ==
                       3*m_SubPages[ pos ]->MinNumberOfKeys() - 1);

       vector<ObjectInfo> tmpKeys;
       vector<BTPage *>   tmpSubPages;

       BTPage  *pChild1 = m_SubPages[pos-1],
                       *pChild2 = m_SubPages[ pos ],
                       *pChild3 = m_SubPages[pos+1];
       MovePage(pChild1, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[pos-1]);
       MovePage(pChild2, tmpKeys, tmpSubPages);
       tmpKeys    .push_back(m_Keys[ pos ]);
       MovePage(pChild3, tmpKeys, tmpSubPages);
       pChild3->Destroy();

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

template <typename keyType, typename ObjIDType>
bt_ErrorCode CBNEWTreePage<keyType, ObjIDType>::MergeRoot()
{
       Size pos = 1;
       assert( m_SubPages[pos-1]->NumberOfKeys() +
                       m_SubPages[ pos ]->NumberOfKeys() +
                       m_SubPages[pos+1]->NumberOfKeys() ==
                       3*m_SubPages[ pos ]->MinNumberOfKeys() - 1);

       BTPage  *pChild1 = m_SubPages[pos-1], *pChild2 = m_SubPages[ pos ], *pChild3 = m_SubPages[pos+1];
       Size nKeys = pChild1->NumberOfKeys() + pChild2->NumberOfKeys() + pChild3->NumberOfKeys() + 2;

       vector<ObjectInfo> tmpKeys;
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

       pChild1->Destroy();
       pChild2->Destroy();
       pChild3->Destroy();

       return bt_rootmerged;
}

template <typename keyType, typename ObjIDType>
typename CBNEWTreePage<keyType, ObjIDType>::ObjectInfo &
CBNEWTreePage<keyType, ObjIDType>::GetFirstObjectInfo()
{
       if( m_SubPages[0] )
               return m_SubPages[0]->GetFirstObjectInfo();
       return m_Keys[0];
}

template <typename keyType, typename ObjIDType>
void CBNEWTreePage<keyType, ObjIDType>::Create()
{
       Reset();
       m_Keys.resize(m_MaxKeys+1);
       m_SubPages.resize(m_MaxKeys+2, NULL);
       m_KeyCount = 0;
       m_MinKeys  = (2 * m_MaxKeys) / 3;
}

template <typename keyType, typename ObjIDType>
void CBNEWTreePage<keyType, ObjIDType>::Reset()
{
       for( Size i = 0 ; i < m_KeyCount ; i++ )
               delete m_SubPages[i];
       clear();
}

template <typename keyType, typename ObjIDType>
void CBNEWTreePage<keyType, ObjIDType>::clear()
{
       m_KeyCount = 0;
}

template <typename keyType, typename ObjIDType>
CBNEWTreePage<keyType, ObjIDType> * CreateBTreeNode (Size maxKeys, bool unique)
{
       return new CBNEWTreePage<keyType, ObjIDType> (maxKeys, unique);
}

template <typename keyType, typename ObjIDType>
void CBNEWTreePage<keyType, ObjIDType>::MovePage(BTPage *pChildPage, vector<ObjectInfo> &tmpKeys,vector<BTPage *> &tmpSubPages)
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

template <typename keyType, typename ObjIDType>
Size CBNEWTreePage<keyType, ObjIDType>::GetFreeCellsOnLeft(Size pos)
{
       if( pos > 0 )                                   
               return m_SubPages[pos-1]->GetFreeCells();
       return 0;
}

template <typename keyType, typename ObjIDType>
Size CBNEWTreePage<keyType, ObjIDType>::GetFreeCellsOnRight(Size pos)
{
       if( pos < GetNumberOfKeys() )   
               return m_SubPages[pos+1]->GetFreeCells();
       return 0;
}

#endif