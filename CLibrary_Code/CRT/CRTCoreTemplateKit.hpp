/*- CLib - CRT Template Kit -----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-09-04                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_CLib_CRT_CRTTemplateKit_H__
#define __cnLibrary_CLib_CRT_CRTTemplateKit_H__

#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{


//- Class Allocation --------------------------------------------------------
struct cRTLAllocator
{
	void* operator new(tSize Size){		return malloc(Size);	}
	void operator delete(void*p){		free(p);	}

	void* operator new[](tSize Size){	return malloc(Size);	}
	void operator delete[](void*p){		free(p);	}
};

//---------------------------------------------------------------------------
// Array
//---------------------------------------------------------------------------

template<class T>
using cArrayBlock=cnDataStruct::cArrayBlock<TKRuntime::SystemAllocationOperator,T>;
//---------------------------------------------------------------------------
typedef cnDataStruct::cArrayBlock<TKRuntime::SystemAllocationOperator,void> cMemoryBlock;
typedef cnDataStruct::cArrayBuffer<TKRuntime::SystemAllocationOperator,void> cMemoryBuffer;

//---------------------------------------------------------------------------
// String
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
template<class TCharacter>
using cStringBuffer=cnString::cStringBuffer<TKRuntime::SystemAllocationOperator,TCharacter>;
//---------------------------------------------------------------------------
template<class TCharacter>
using cString=cnString::cString< cnString::cRefStringArrayTokenOperator<TKRuntime::SystemAllocationOperator,TCharacter> >;

template<class TFunction>
using cFunction=cnVar::cFunction<TFunction,TKRuntime::SystemAllocationOperator>;

//---------------------------------------------------------------------------

using cnAsync::cRingIndex;


//---------------------------------------------------------------------------
// Sequential Structure
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
template<class TElement>
using cSeqList=cnDataStruct::cSeqList<TKRuntime::SystemAllocationOperator,TElement>;
template< class TElement,class TItemOrderOperator=cnDataStruct::cDefaultItemOrderOperator<TElement> >
using cSeqSet=cnDataStruct::cSeqSet<TKRuntime::SystemAllocationOperator,TElement,TItemOrderOperator>;
//---------------------------------------------------------------------------
template<class TKey,class TValue,class TKeyOrderOperatom=cnDataStruct::cDefaultItemOrderOperator<TKey> >
using cSeqMap=cnDataStruct::cSeqMap<TKRuntime::SystemAllocationOperator,TKey,TKRuntime::SystemAllocationOperator,TValue,TKeyOrderOperatom>;
//---------------------------------------------------------------------------
template<class...TElements>
using cSeqMultiList=cnDataStruct::cSeqMultiList<cnDataStruct::cArrayAllocation<TKRuntime::SystemAllocationOperator,TElements>...>;

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
		cnClass::ManualConstruct(Node->Content,static_cast<TArgs&&>(Args)...);
		return Node;
	}
	static void DestroyNode(tNode *Node){
		cnClass::ManualDestruct(Node->Content);
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
// Atomic
//---------------------------------------------------------------------------

template<class T>
using cAtomicVar=cnAsync::cAtomicVariable<T>;

//---------------------------------------------------------------------------
template<class T>
using iAtomicPtr = cnClass::cAtomicPtrReference<T,cnClass::iPointerReferenceOperator>;
//---------------------------------------------------------------------------
template<class T>
using rAtomicPtr = cnClass::cAtomicPtrReference<T,cnClass::rPointerReferenceOperator>;

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
template<class TStdCoHandle>
struct TStdCoroutineHandleOperator
{

	typedef TStdCoHandle tHandle;


	static void Reset(tHandle &Handle)noexcept(true){
		Handle=tHandle();
	}
	template<class TAwaitHandle>
	static void Assign(tHandle &Handle,TAwaitHandle&& Await)noexcept(true){
		Handle=Await;
	}
	static bool IsAvailable(const tHandle &Handle)noexcept(true){
		return static_cast<bool>(Handle);
	}
	static void Resume(tHandle &Handle)noexcept(true){
		return Handle.resume();
	}
	static void Finish(tHandle &Handle)noexcept(true){
		return Handle.destroy();
	}
};

#if		cnLibrary_CRTFEATURE_COROUTINE>=201902L

typedef TStdCoroutineHandleOperator< std::coroutine_handle<> > cCoroutineHandleOperator;

#elif	cnLibrary_CRTFEATURE_EXPERIMENTAL_COROUTINE>=201500L

typedef TStdCoroutineHandleOperator< std::experimental::coroutine_handle<> > cCoroutineHandleOperator;

#else

typedef cnAsync::cVariantCoroutineHandleOperator<sizeof(void*)*2> cCoroutineHandleOperator;

#endif



template<class TRet>
using cResumable=cnAsync::cResumable<cCoroutineHandleOperator,TRet>;

template<class TRet>
using cCoroutine=cnAsync::cCoroutine<cCoroutineHandleOperator,TRet>;


//---------------------------------------------------------------------------
}	//	namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif


