/*- cnLibCRT - CRT Template Kit -------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-09-04                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_CRTTemplateKit_H__
#define __cnLibrary_cnRTL_CRTTemplateKit_H__

#include <cnRTL/CRTCoreCommon.h>
#include <cnRTL/CRTCoreMemory.h>
#include <cnRTL/CRTCoreString.h>

#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------

struct TKRuntime::tAtomic
{
	static void WatchPause(void){
	}
};

template<class T>
struct TKRuntime::tAtomicInteger
{
	typedef std::atomic<T> tAtomic;
	typedef T tVariable;

	static tVariable Get(const tAtomic &a){
		return a;
	}
	static void Set(tAtomic &a,const tVariable &v){
		a=v;
	}

	static tVariable FreeLoad(const tAtomic &a){
		return a.load(std::memory_order::memory_order_relaxed);
	}
	static tVariable AcquireLoad(const tAtomic &a){
		return a.load(std::memory_order::memory_order_acquire);
	}

	static void FreeStore(tAtomic &a,const tVariable &v){
		a.store(v,std::memory_order::memory_order_relaxed);
	}
	static void ReleaseStore(tAtomic &a,const tVariable &v){
		a.store(v,std::memory_order::memory_order_release);
	}

	static tVariable FreeExchange(tAtomic &a,const tVariable &v){
		return a.exchange(v,std::memory_order::memory_order_relaxed);
	}
	static tVariable AcquireExchange(tAtomic &a,const tVariable &v){
		return a.exchange(v,std::memory_order::memory_order_acquire);
	}
	static tVariable ReleaseExchange(tAtomic &a,const tVariable &v){
		return a.exchange(v,std::memory_order::memory_order_release);
	}
	static tVariable BarrierExchange(tAtomic &a,const tVariable &v){
		return a.exchange(v,std::memory_order::memory_order_acq_rel);
	}

	static bool FreeCompareStore(tAtomic &a,tVariable c,const tVariable &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_relaxed);
	}
	static bool AcquireCompareStore(tAtomic &a,tVariable c,const tVariable &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_acquire);
	}
	static bool ReleaseCompareStore(tAtomic &a,tVariable c,const tVariable &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_release);
	}
	static bool BarrierCompareStore(tAtomic &a,tVariable c,const tVariable &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_acq_rel);
	}

	static bool FreeCompareExchange(tAtomic &a,tVariable &c,const tVariable &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_relaxed);
	}
	static bool AcquireCompareExchange(tAtomic &a,tVariable &c,const tVariable &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_acquire);
	}
	static bool ReleaseCompareExchange(tAtomic &a,tVariable &c,const tVariable &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_release);
	}
	static bool BarrierCompareExchange(tAtomic &a,tVariable &c,const tVariable &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_acq_rel);
	}

	static tVariable FreeAdd(tAtomic &a,const tVariable &v){
		return a.fetch_add(v,std::memory_order::memory_order_relaxed);
	}
	static tVariable FreeAddN(tAtomic &a,const tVariable &v){
		return a.fetch_add(v,std::memory_order::memory_order_relaxed)+v;
	}
	static tVariable AcquireAdd(tAtomic &a,const tVariable &v){
		return a.fetch_add(v,std::memory_order::memory_order_acquire);
	}
	static tVariable AcquireAddN(tAtomic &a,const tVariable &v){
		return a.fetch_add(v,std::memory_order::memory_order_acquire)+v;
	}
	static tVariable ReleaseAdd(tAtomic &a,const tVariable &v){
		return a.fetch_add(v,std::memory_order::memory_order_release);
	}
	static tVariable ReleaseAddN(tAtomic &a,const tVariable &v){
		return a.fetch_add(v,std::memory_order::memory_order_release)+v;
	}
	static tVariable BarrierAdd(tAtomic &a,const tVariable &v){
		return a.fetch_add(v,std::memory_order::memory_order_acq_rel);
	}
	static tVariable BarrierAddN(tAtomic &a,const tVariable &v){
		return a.fetch_add(v,std::memory_order::memory_order_acq_rel)+v;
	}

	static tVariable FreeSub(tAtomic &a,const tVariable &v){
		return a.fetch_sub(v,std::memory_order::memory_order_relaxed);
	}
	static tVariable FreeSubN(tAtomic &a,const tVariable &v){
		return a.fetch_sub(v,std::memory_order::memory_order_relaxed)-v;
	}
	static tVariable AcquireSub(tAtomic &a,const tVariable &v){
		return a.fetch_sub(v,std::memory_order::memory_order_acquire);
	}
	static tVariable AcquireSubN(tAtomic &a,const tVariable &v){
		return a.fetch_sub(v,std::memory_order::memory_order_acquire)-v;
	}
	static tVariable ReleaseSub(tAtomic &a,const tVariable &v){
		return a.fetch_sub(v,std::memory_order::memory_order_release);
	}
	static tVariable ReleaseSubN(tAtomic &a,const tVariable &v){
		return a.fetch_sub(v,std::memory_order::memory_order_release)-v;
	}
	static tVariable BarrierSub(tAtomic &a,const tVariable &v){
		return a.fetch_sub(v,std::memory_order::memory_order_acq_rel);
	}
	static tVariable BarrierSubN(tAtomic &a,const tVariable &v){
		return a.fetch_sub(v,std::memory_order::memory_order_acq_rel)-v;
	}
};


template<class TPointer>
struct TKRuntime::tAtomicLink
{
		
	typedef TPointer tPointer;
	typedef std::atomic<TPointer> tLink;

	static tPointer Get(const tLink &Link){		return Link;	}
	static void Set(tLink &Link,tPointer p){	Link=p;	}

	static tPointer AcquireLoad(const tLink &Link){	return Link.load();	}
	static void ReleaseStore(tLink &Link,tPointer p){	Link.store(p);	}

	static tPointer Exchange(tLink &Link,tPointer New){	return static_cast<tPointer>(Link.exchange(New));	}
	
	template<class TSingleLinkItemOperator>
	static tPointer Append(tLink &Link,tPointer First,tPointer Last){

		tPointer CurTop=Link.load();
		do{
			TSingleLinkItemOperator::SetNext(Last,CurTop);
			
		}while(!Link.compare_exchange_weak(CurTop,First));
		return CurTop;
	}

	template<class TSingleLinkItemOperator>
	static tPointer Extract(tLink &Link,tPointer &NextTop){
		tPointer CurTop=Link.load();
		do{
			if(CurTop==nullptr)
				return nullptr;

			NextTop=TSingleLinkItemOperator::GetNext(CurTop);
		}while(!Link.compare_exchange_weak(CurTop,NextTop));
		return CurTop;
	}
};



template<class TElement>
struct TKRuntime::tVectorOperatorEnumeration
{
	typedef cnVar::TTypePack<
		cnVar::cScalerFloatVectorOperator<TElement>
	> tFloatOperatorPack;

	typedef cnVar::TTypePack<
		cnVar::cScalerIntegerVectorOperator<TElement>
	> tIntegerOperatorPack;
};
//---------------------------------------------------------------------------
namespace cnRTL{

//---------------------------------------------------------------------------
// Array
//---------------------------------------------------------------------------

template<class T>
using cArrayBlock=cnDataStruct::cArrayBlock<cAllocationOperator,T>;
//---------------------------------------------------------------------------
typedef cnDataStruct::cArrayBlock<cAllocationOperator,void> cMemoryBlock;
typedef cnDataStruct::cArrayBuffer<cAllocationOperator,void> cMemoryBuffer;



//---------------------------------------------------------------------------
// Atomic
//---------------------------------------------------------------------------

template<class T>
using cAtomicVar=cnAsync::cAtomicVariable<T>;

//---------------------------------------------------------------------------
template<class T>
using iAtomicPtr = cnVar::cAtomicPtrReference<cnVar::iRefTokenOperator<T>>;
//---------------------------------------------------------------------------
template<class T>
using rAtomicPtr = cnVar::cAtomicPtrReference<cnVar::rRefTokenOperator<T>>;


//---------------------------------------------------------------------------

using cnAsync::cRingIndex;

//---------------------------------------------------------------------------
// String
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
template<class TCharacter>
using cStringBuffer=cnString::cStringBuffer<cAllocationOperator,TCharacter>;
//---------------------------------------------------------------------------
template<class TCharacter>
using cString=cnString::cString< cnString::cRefStringArrayTokenOperator<cAllocationOperator,TCharacter> >;

template<class TFunction,uIntn StorageSize=cnVar::TFunctionStorageDefaultSize::Value>
using cFunction=cnVar::cFunction<TFunction,cnVar::cFunctionStorageOperator<cAllocationOperator,TFunction,StorageSize> >;

//---------------------------------------------------------------------------
// Sequential Structure
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
template<class T>
using cArrayBlock=cnDataStruct::cArrayBlock<cAllocationOperator,T>;
//---------------------------------------------------------------------------
template<class TElement>
using cSeqList=cnDataStruct::cSeqList<cAllocationOperator,TElement>;
template< class TElement,class TItemOrderOperator=cnDataStruct::cDefaultItemOrderOperator<TElement> >
using cSeqSet=cnDataStruct::cSeqSet<cAllocationOperator,TElement,TItemOrderOperator>;
//---------------------------------------------------------------------------
template<class TKey,class TValue,class TKeyOrderOperatom=cnDataStruct::cDefaultItemOrderOperator<TKey> >
using cSeqMap=cnDataStruct::cSeqMap<cAllocationOperator,TKey,cAllocationOperator,TValue,TKeyOrderOperatom>;
//---------------------------------------------------------------------------
template<class...TElements>
using cSeqMultiList=cnDataStruct::cSeqMultiList<cnDataStruct::cArrayAllocation<cAllocationOperator,TElements>...>;

//---------------------------------------------------------------------------
// Linked Structure
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// TLinkContentNode : TLinkItem
//{
//	T Content;
//};
//---------------------------------------------------------------------------
template<class TLinkContentNode>
struct cLinkNodeContentOperator
{
	typedef decltype(TLinkContentNode::Content) tContent;
	typedef TLinkContentNode tNode;

	template<class...TArgs>
	static tNode* CreateNode(TArgs&&...Args){
		tNode *Node=static_cast<tNode*>(::malloc(sizeof(tNode)));
		cnVar::ManualConstruct(Node->Content,static_cast<TArgs&&>(Args)...);
		return Node;
	}
	static void DestroyNode(tNode *Node){
		cnVar::ManualDestruct(Node->Content);
		::free(Node);
	}

	static tContent* ContentPtr(tNode *Node){
		return cnMemory::AddressOf(Node->Content);
	}

};

//---------------------------------------------------------------------------
template<class TItem>
using cLinkItemStack=cnDataStruct::cLinkItemStack< cnDataStruct::cSingleLinkItemOperator<TItem> >;

template<class TItem>
using cLinkItemQueue=cnDataStruct::cLinkItemQueue< cnDataStruct::cSingleLinkItemOperator<TItem> >;

template<class TItem>
using cLinkItemList=cnDataStruct::cLinkItemList< cnDataStruct::cDualLinkItemOperator<TItem> >;

template< class TItem,class TElementOperator=cnDataStruct::cDefaultItemOrderOperator<TItem> >
using cLinkItemSet=cnDataStruct::cLinkItemSet<cnDataStruct::cColorBinaryTreeNodeOperator<TItem>,TElementOperator>;
//---------------------------------------------------------------------------
template<class T>
struct cSingleLinkElement
{
	cSingleLinkElement *Next;

	T Content;
};
template<>
struct cSingleLinkElement<void>
{
	cSingleLinkElement *Next;
};

template<class T>
struct cDualLinkElement
{
	cDualLinkElement *Next;
	cDualLinkElement *Prev;

	T Content;
};
template<>
struct cDualLinkElement<void>
{
	cDualLinkElement *Next;
	cDualLinkElement *Prev;
};

template<class T>
using cLinkStack=cnDataStruct::cLinkStack< cnDataStruct::cSingleLinkItemOperator< cSingleLinkElement<T> >,cLinkNodeContentOperator< cSingleLinkElement<T> > >;
template<class T>
using cLinkQueue=cnDataStruct::cLinkQueue< cnDataStruct::cSingleLinkItemOperator< cSingleLinkElement<T> >,cLinkNodeContentOperator< cSingleLinkElement<T> > >;

template<class T>
using cLinkList=cnDataStruct::cLinkList< cnDataStruct::cDualLinkItemOperator< cDualLinkElement<T> >,cLinkNodeContentOperator< cDualLinkElement<T> > >;


//---------------------------------------------------------------------------

template<class T>
struct cBinaryTreeNode
{
	cBinaryTreeNode *Parent;
	cBinaryTreeNode *Child[2];
	ufInt8 Branch;

	T Content;
};
//---------------------------------------------------------------------------
template<class T>
struct cColorBinaryTreeNode
{
	cColorBinaryTreeNode *Parent;
	cColorBinaryTreeNode *Child[2];
	ufInt8 Branch;
	ufInt8 Color;

	T Content;
};

template<class TElement,class TElementOperator=cnDataStruct::cDefaultItemOrderOperator<TElement> >
using cLinkSet=cnDataStruct::cLinkSet<
	cnDataStruct::cColorBinaryTreeNodeOperator< cColorBinaryTreeNode<TElement> >,
	cLinkNodeContentOperator< cColorBinaryTreeNode<TElement> >,
	TElementOperator
>;

template <class TKey,class TValue,class TKeyItemOperator=cnDataStruct::cDefaultItemOrderOperator<TKey>  >
using cLinkMap=cnDataStruct::cLinkMap<
	cnDataStruct::cColorBinaryTreeNodeOperator< cColorBinaryTreeNode< cnDataStruct::cLinkMapItem<TKey,TValue> > >,
	cLinkNodeContentOperator< cColorBinaryTreeNode< cnDataStruct::cLinkMapItem<TKey,TValue> > >,
	TKeyItemOperator
>;

//---------------------------------------------------------------------------
// Atomic Linked Structure
//---------------------------------------------------------------------------
template<class T>
using cAtomicStack=cnAsync::cAtomicStack< cnDataStruct::cSingleLinkItemOperator<T> >;
template<class T>
using cAtomicStackSO=cnAsync::cAtomicStackSO< cnDataStruct::cSingleLinkItemOperator<T> >;

template<class T>
using cAtomicQueue=cnAsync::cAtomicQueue< cnDataStruct::cSingleLinkItemOperator<T> >;
template<class T>
using cAtomicQueueSO=cnAsync::cAtomicQueueSO< cnDataStruct::cSingleLinkItemOperator<T> >;


//---------------------------------------------------------------------------
//	Vector
//---------------------------------------------------------------------------


template<class TElement,uIntn ElementCount>
using cVector=cnVar::cVector<TElement,ElementCount>;
	
template<class TElement,uIntn RowCount,uIntn ColCount>
using cMatrixR=cnVar::cMatrixR<TElement,RowCount,ColCount>;
template<class TElement,uIntn RowCount,uIntn ColCount>
using cMatrixC=cnVar::cMatrixC<TElement,RowCount,ColCount>;

template<class TElement,uIntn ElementCount>
using cIntegerVector=cnVar::cIntegerVector<TElement,ElementCount>;

//---------------------------------------------------------------------------
//	Coroutine
//---------------------------------------------------------------------------


#if		cnLibrary_CRTFEATURE_COROUTINE>=201902L

typedef std::coroutine_handle<> vtCoHandle;

#elif	cnLibrary_CRTFEATURE_EXPERIMENTAL_COROUTINE>=201500L

typedef std::experimental::coroutine_handle<> vtCoHandle;

#else

typedef cnAsync::cCoHandleVariant vtCoHandle;

#endif

template<class TRet>
using cResumable=cnAsync::cResumable<TRet,vtCoHandle>;

template<class TRet>
using cCoroutine=cnAsync::cCoroutine<TRet,vtCoHandle>;

//---------------------------------------------------------------------------


template<class TStreamWriteBuffer>
using cMemoryCachedStreamWriteBuffer=cnStream::cMemoryCachedStreamWriteBuffer<cAllocationOperator,TStreamWriteBuffer>;

//---------------------------------------------------------------------------
}	//	namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif


