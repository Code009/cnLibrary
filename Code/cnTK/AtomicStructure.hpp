/*- cnTK - Atomic Linked Structure ----------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-09-05                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_AtomicLinkStruct_HPP__
#define	__cnLibrary_cnTK_AtomicLinkStruct_HPP__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/Numerical.hpp>
#include <cnTK/Memory.hpp>
#include <cnTK/Atomic.hpp>
#include <cnTK/Interface.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Async_TH{
//---------------------------------------------------------------------------
template<class TAtomicIntegerOperator,uIntn RingIndexCount,uIntn RingIndex>
struct cRingIndexFunc
{
	static typename TAtomicIntegerOperator::tVariable LoadIndex(const typename TAtomicIntegerOperator::tAtomic (&Indices)[RingIndexCount],uIntn)noexcept(true){
		cnLib_STATIC_ASSERT(RingIndex<RingIndexCount,"Invalid RingIndex");
		return TAtomicIntegerOperator::AcquireLoad(Indices[RingIndex]);
	}
};
template<class TAtomicIntegerOperator,uIntn RingIndexCount>
struct cRingIndexFunc<TAtomicIntegerOperator,RingIndexCount,cnVar::TIntegerValue<uIntn>::Max>
{
	static typename TAtomicIntegerOperator::tVariable LoadIndex(const typename TAtomicIntegerOperator::tAtomic (&Indices)[RingIndexCount],uIntn TotalSize)noexcept(true){
		return static_cast<typename TAtomicIntegerOperator::tVariable>(TAtomicIntegerOperator::AcquireLoad(Indices[RingIndexCount-1])+TotalSize);
	}
};
template<class TAtomicIntegerOperator,uIntn RingIndexCount>
struct cRingIndexFunc<TAtomicIntegerOperator,RingIndexCount,RingIndexCount>
{
	static typename TAtomicIntegerOperator::tVariable LoadIndex(const typename TAtomicIntegerOperator::tAtomic (&Indices)[RingIndexCount],uIntn)noexcept(true){
		return TAtomicIntegerOperator::AcquireLoad(Indices[RingIndexCount-1]);
	}
};
//---------------------------------------------------------------------------
}	// namesapce Async_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnAsync{
//---------------------------------------------------------------------------

// Ring Index
//	design for producer/consumer pair of thread use
template<class TIndex,uIntn RingIndexCount,uIntn SizeBit=0>
class cRingIndex
{
public:
	cnLib_STATIC_ASSERT(SizeBit<cnVar::TIntegerValue<uIntn>::BitCount,"SizeBit too large");
	typedef TIndex tIndex;
	typedef TKRuntime::TAtomicInteger<TIndex> tAtomicIntegerOperator;
	cnLib_STATIC_ASSERT(RingIndexCount>=2,"require RingIndexCount>=2");

	cRingIndex()noexcept(true){
		Reset();
	}

	void Reset(void)noexcept(true){
		for(uIntn i=0;i<RingIndexCount;i++)
			fIndices[i]=0;
	}

	static uIntn GetTotalSize(void)noexcept(true){		return TotalSize;	}

	// GetSize
	//	get total size from ring RingIndex to ring RingIndex+Count
	// <RingIndex>		index of ring to accumulate
	// <Count>			count of ring to accumulate
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
	template<uIntn RingIndex,uIntn Count=1>
#else
	template<uIntn RingIndex, uIntn Count>
#endif
	tIndex GetSize(void)const noexcept(true)
	{
		// get current position
		tIndex CurEnd=LoadIndex<RingIndex-Count>();
		tIndex CurStart=LoadIndex<RingIndex>();
		// calculate size
		return static_cast<tIndex>(CurEnd-CurStart);
	}

	// LoadIndex
	//	get index value from ring
	// <RingIndex>			ring index
	// return index
	template<uIntn RingIndex>
	tIndex LoadIndex(void)const noexcept(true){
		return cnLib_THelper::Async_TH::cRingIndexFunc<tAtomicIntegerOperator,RingIndexCount,RingIndex>::LoadIndex(fIndices,TotalSize);
	}

	// StoreIndex
	//	set index value to ring
	// <RingIndex>			ring index
	// NewIndex		new index
	template<uIntn RingIndex>
	void StoreIndex(tIndex NewIndex)noexcept(true)
	{
		cnLib_ASSERT(IsValidIndex<RingIndex>(NewIndex));

		return tAtomicIntegerOperator::ReleaseStore(fIndices[RingIndex],NewIndex);
	}

	// Reserve
	//	reserve range
	// <RingIndex>		ring index
	// [out]Index		index of items reserved
	// Request			count of items requested
	// return count of items reserved
	template<uIntn RingIndex>
	tIndex Reserve(tIndex &Index,tIndex Request)const noexcept(true)
	{
		// get current position
		tIndex CurEnd=LoadIndex<RingIndex-1>();
		tIndex CurStart=LoadIndex<RingIndex>();
		// calculate size
		tIndex CurSize=static_cast<tIndex>(CurEnd-CurStart);

		// limit request size
		if(Request>CurSize)
			Request=CurSize;
		// get index
		Index=CurStart&SizeMask;
		return Request;
	}
	// Commit
	//	commit items
	// <RingIndex>			ring index
	// Size			count of items to commit
	template<uIntn RingIndex>
	void Commit(tIndex Size)noexcept(true)
	{
		// move pointer
		tIndex NewIndex=LoadIndex<RingIndex>();
		NewIndex+=Size;
		StoreIndex<RingIndex>(NewIndex);
	}
	// CommitChecked
	//	commit range with check
	// <RingIndex>			ring index
	// Size			count of items to commit
	template<uIntn RingIndex>
	tIndex CommitChecked(tIndex Size)noexcept(true)
	{
		// move pointer
		tIndex CurEnd=LoadIndex<RingIndex-1>();
		tIndex Index=LoadIndex<RingIndex>();
		tIndex Commited=CurEnd-Index;
		if(Commited>Size){
			Index=CurEnd;
		}
		else{
			Commited=Size;
			Index+=Size;
		}
		StoreIndex<RingIndex>(Index);
		return Commited;
	}
	// Reserve
	//	reserve one item
	// <RingIndex>		ring index
	// [out]Index		index of item reserved
	// return true if item reserved
	template<uIntn RingIndex>
	bool ReserveItem(tIndex &Index)const noexcept(true){
		return Reserve<RingIndex>(Index,1)!=0;
	}
	// CommitItem
	//	commit one item
	// <RingIndex>			ring index
	template<uIntn RingIndex>
	void CommitItem(void)noexcept(true){
		return Commit<RingIndex>(1);
	}
	// Rollback
	//	reset index of ring to the lowwer ring
	// <RingIndex>			ring index
	template<uIntn RingIndex>
	void Rollback(void)noexcept(true)
	{
		// get current data position
		tIndex RollbackIndex=LoadIndex<RingIndex+1>();
		StoreIndex<RingIndex>(RollbackIndex);
	}

	// IsValidIndex
	//	test if Index is valid in ring RingIndex
	// return true if it is valid
	template<uIntn RingIndex>
	bool IsValidIndex(tIndex Index)const noexcept(true)
	{
		tIndex Lowwer=LoadIndex<RingIndex+1>();
		tIndex Higher=LoadIndex<RingIndex-1>();
		tIndex Total=Higher-Lowwer;
		tIndex Current=Index-Lowwer;

		return Current<=Total;
	}

protected:

	typename tAtomicIntegerOperator::tAtomic fIndices[RingIndexCount];

	static cnLib_CONSTVAR uIntn TotalSize=1<<SizeBit;
	// SizeMask
	//	mask of ring buffer size
	//	must not change until buffer is not used
	static cnLib_CONSTVAR uIntn SizeMask=TotalSize-1;

};

// Ring Index
//	design for producer/consumer pair of thread use
template<class TIndex,uIntn RingIndexCount>
class cRingIndex<TIndex,RingIndexCount,0>
{
public:
	typedef TIndex tIndex;
	typedef TKRuntime::TAtomicInteger<TIndex> tAtomicIntegerOperator;
	cnLib_STATIC_ASSERT(RingIndexCount>=2,"require RingIndexCount>=2");

	cRingIndex()noexcept(true)
		: fSizeMask( cnVar::TIntegerValue<tIndex>::Max)
	{
		Reset();
	}

	void Reset(uIntn BufferSizeBits)noexcept(true){
		fSizeMask=(1<<BufferSizeBits)-1;
		Reset();
	}
	void Reset(void)noexcept(true){
		for(uIntn i=0;i<RingIndexCount;i++)
			fIndices[i]=0;
	}

	tIndex GetTotalSize(void)const noexcept(true){			return fSizeMask+1;	}

	// GetSize
	//	get total size from ring RingIndex to ring RingIndex+Count
	// <RingIndex>		index of ring to accumulate
	// <Count>			count of ring to accumulate
#ifndef cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
	template<uIntn RingIndex,uIntn Count=1>
#else
	template<uIntn RingIndex, uIntn Count>
#endif
	tIndex GetSize(void)const noexcept(true)
	{
		// get current position
		tIndex CurEnd=LoadIndex<RingIndex-Count>();
		tIndex CurStart=LoadIndex<RingIndex>();
		// calculate size
		return static_cast<tIndex>(CurEnd-CurStart);
	}


	// LoadIndex
	//	get index value from ring
	// <RingIndex>			ring index
	// return index
	template<uIntn RingIndex>
	tIndex LoadIndex(void)const noexcept(true){
		return cnLib_THelper::Async_TH::cRingIndexFunc<tAtomicIntegerOperator,RingIndexCount,RingIndex>::LoadIndex(fIndices,fSizeMask+1);
	}

	// StoreIndex
	//	set index value to ring
	// <RingIndex>			ring index
	// NewIndex		new index
	template<uIntn RingIndex>
	void StoreIndex(tIndex NewIndex)noexcept(true)
	{
		cnLib_ASSERT(IsValidIndex<RingIndex>(NewIndex));

		return tAtomicIntegerOperator::ReleaseStore(fIndices[RingIndex],NewIndex);
	}


	// Reserve
	//	reserve range
	// <RingIndex>		ring index
	// [out]Index		index of items reserved
	// Request			count of items requested
	// return count of items reserved
	template<uIntn RingIndex>
	tIndex Reserve(tIndex &Index,tIndex Request)const noexcept(true)
	{
		// get current position
		tIndex CurEnd=LoadIndex<RingIndex-1>();
		tIndex CurStart=LoadIndex<RingIndex>();
		// calculate size
		tIndex CurSize=static_cast<tIndex>(CurEnd-CurStart);

		// limit request size
		if(Request>CurSize)
			Request=CurSize;
		// get index
		Index=CurStart&fSizeMask;
		return Request;
	}
	// Commit
	//	commit range
	// <RingIndex>			ring index
	// Size			count of items to commit
	template<uIntn RingIndex>
	void Commit(tIndex Size)noexcept(true)
	{
		// move pointer
		tIndex NewIndex=LoadIndex<RingIndex>();
		NewIndex+=Size;
		StoreIndex<RingIndex>(NewIndex);
	}
	// CommitChecked
	//	commit range with check
	// <RingIndex>			ring index
	// Size			count of items to commit
	template<uIntn RingIndex>
	tIndex CommitChecked(tIndex Size)noexcept(true)
	{
		// move pointer
		tIndex CurEnd=LoadIndex<RingIndex-1>();
		tIndex Index=LoadIndex<RingIndex>();
		tIndex Commited=CurEnd-Index;
		if(Commited>Size){
			Index=CurEnd;
		}
		else{
			Commited=Size;
			Index+=Size;
		}
		StoreIndex<RingIndex>(Index);
		return Commited;
	}
	// Reserve
	//	reserve one item
	// <RingIndex>		ring index
	// [out]Index		index of item reserved
	// return true if item reserved
	template<uIntn RingIndex>
	bool ReserveItem(tIndex &Index)const noexcept(true){
		return Reserve<RingIndex>(Index,1)!=0;
	}
	// CommitItem
	//	commit one item
	// <RingIndex>			ring index
	template<uIntn RingIndex>
	void CommitItem(void)noexcept(true){
		return Commit<RingIndex>(1);
	}
	// Rollback
	//	reset index of ring to the lowwer ring
	// <RingIndex>			ring index
	template<uIntn RingIndex>
	void Rollback(void)noexcept(true)
	{
		// get current data position
		tIndex RollbackIndex=LoadIndex<RingIndex+1>();
		StoreIndex<RingIndex>(RollbackIndex);
	}

	// IsValidIndex
	//	test if Index is valid in ring RingIndex
	// return true if it is valid
	template<uIntn RingIndex>
	bool IsValidIndex(tIndex Index)const noexcept(true)
	{
		tIndex Lowwer=LoadIndex<RingIndex+1>();
		if(Index<Lowwer)
			return false;
		tIndex Higher=LoadIndex<RingIndex-1>();
		if(Index>Higher)
			return false;

		return true;
	}
protected:

	typename tAtomicIntegerOperator::tAtomic fIndices[RingIndexCount];
	tIndex fSizeMask;
};
//---------------------------------------------------------------------------
}	// namespace cnAsync
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnAsync{
//---------------------------------------------------------------------------

template<class TSingleLinkedItemOperator>
class cAtomicStack
{
public:
	typedef typename TSingleLinkedItemOperator::tItem tItem;
	typedef typename TKRuntime::template TAtomicLink<tItem*> tLinkOperator;

	cnLib_CONSTEXPR_FUNC cAtomicStack()noexcept(true) : fTop(nullptr){}


	// Push
	//	push item into stack
	// return new stack top
	tItem* Push(tItem *Item)noexcept(true){
		return tLinkOperator::template Append<TSingleLinkedItemOperator>(fTop,Item,Item);
	}
	// Push
	//	push item chain into stack. caller must ensure the chain from first to last is valid, otherwise the state of the stack is undefined.
	// First		first item of the chain
	// Last			last item of the chain
	// return new stack top
	tItem* PushChain(tItem *First,tItem *Last)noexcept(true){
		return tLinkOperator::template Append<TSingleLinkedItemOperator>(fTop,First,Last);
	}

	// Pop
	//	pop item from stack
	// return pointer to poped item
	tItem* Pop(void)noexcept(true){
		tItem *TopNext;
		return tLinkOperator::template Extract<TSingleLinkedItemOperator>(fTop,TopNext);
	}
	// Swap
	//	swap stack
	// NewTopItem		stack top of the new stack chain
	// return prev stack top
	tItem* Swap(tItem *NewTopItem)noexcept(true){
		return tLinkOperator::Exchange(fTop,NewTopItem);
	}

	// IsEmpty
	// return true if stack is empty
	bool	IsEmpty(void)const noexcept(true){
		return fTop==nullptr;
	}
private:
	typename tLinkOperator::tLink fTop;
};

template<class TSingleLinkedItemOperator>
class cAtomicStackSO
{
public:
	typedef typename TSingleLinkedItemOperator::tItem tItem;

	cnLib_CONSTEXPR_FUNC cAtomicStackSO()noexcept(true) : fTop(nullptr){}


	// Push
	//	push item into stack
	// return new stack top
	tItem* Push(tItem *Item)noexcept(true){
		return PushChain(Item,Item);
	}
	// Push
	//	push item chain into stack. caller must ensure the chain from first to last is valid, otherwise the state of the stack is undefined.
	// First		first item of the chain
	// Last			last item of the chain
	// return new stack top
	tItem* PushChain(tItem *First,tItem *Last)noexcept(true){
		tItem *CurTop=fTop.Acquire.Load();
		do{
			TSingleLinkedItemOperator::SetNext(Last,CurTop);
		}while(!fTop.Release.CmpXchg(CurTop,First));
		return CurTop;
	}

	// Pop
	//	pop item from stack
	// return pointer to poped item
	tItem* Pop(void)noexcept(true){
		tItem *CurTop=fTop.Acquire.Load();
		tItem *TopNext;
		do{
			if(CurTop==nullptr)
				return nullptr;

			TopNext=TSingleLinkedItemOperator::GetNext(CurTop);
		}while(!fTop.Release.CmpXchg(CurTop,TopNext));
		return CurTop;
	}
	// Swap
	//	swap stack
	// NewTopItem		stack top of the new stack chain
	// return prev stack top
	tItem* Swap(tItem *NewTopItem)noexcept(true){
		return fTop.Barrier.Xchg(NewTopItem);
	}

	// IsEmpty
	// return true if stack is empty
	bool	IsEmpty(void)const noexcept(true){
		return fTop==nullptr;
	}
private:
	cAtomicVariable<tItem*> fTop;
};

//---------------------------------------------------------------------------

template<class TSingleLinkedItemOperator>
class cAtomicQueue
{
public:
	typedef typename TSingleLinkedItemOperator::tItem tItem;
	typedef typename TKRuntime::template TAtomicLink<tItem*> tLinkOperator;

	cnLib_CONSTEXPR_FUNC cAtomicQueue()noexcept(true):fHead(nullptr),fTail(nullptr){}

	// Enqueue
	//	Enqueue item into queue tail
	void Enqueue(tItem *Item)noexcept(true){	return EnqueueChain(Item,Item);	}
	// Enqueue
	//	Enqueue item chain into queue tail. caller must ensure the chain from first to last is valid, otherwise the state of the queue is undefined.
	// First		first item of the chain
	// Last			last item of the chain
	void EnqueueChain(tItem *First,tItem *Last)noexcept(true){
		cnLib_ASSERT(First!=nullptr);
		cnLib_ASSERT(Last!=nullptr);

		TSingleLinkedItemOperator::SetNext(Last,nullptr);
		// Tail->Next=Item; Tail=Item
		tItem *OldTail;
		if((OldTail=fTail.Release.Xchg(Last))==nullptr){
			// the queue was empty and now in initial state
			// need link up head
			tLinkOperator::ReleaseStore(fHead,First);
		}
		else{
			// link up the queue
			TSingleLinkedItemOperator::SetNext(OldTail,First);
		}
	}
	// Dequeue
	//	dequeue item from queue head
	// return dequeued item, dequeued item's Next remain unchanged
	tItem* Dequeue(void)noexcept(true){
		// DequeuedItem=Head; Head=Head->Next;
		tItem *NextHead;
		tItem *CurHead=tLinkOperator::template Extract<TSingleLinkedItemOperator>(fHead,NextHead);
		if(CurHead==nullptr){
			return nullptr;
		}

		// test queue end
		if(NextHead!=nullptr){
			// tail not reached
			return CurHead;
		}

		// reached tail, try to reset to empty queue
		if(TailReset(CurHead)){
			return CurHead;
		}
		// item restore to head, no item dequeued
		tLinkOperator::ReleaseStore(fHead,CurHead);
		return nullptr;
	}

	// DequeueAll
	//	try to dequeue all available items in the queue
	// return head of dequeue chain
	tItem* DequeueAll(void)noexcept(true){
		// DequeuedItem=Head; Head=nullptr; Tail=nullptr;
		tItem *DequeuedHead=tLinkOperator::Exchange(fHead,nullptr);
		if(DequeuedHead==nullptr)
			return nullptr;

		// find tail
		tItem *DequeuePrev=nullptr;
		tItem *DequeuedTail=DequeuedHead;
		for(auto CurItemNext=TSingleLinkedItemOperator::GetNext(DequeuedTail);CurItemNext!=nullptr;CurItemNext=TSingleLinkedItemOperator::GetNext(DequeuedTail)){
			DequeuePrev=DequeuedTail;
			DequeuedTail=CurItemNext;
		}

		// reset tail
		if(TailReset(DequeuedTail)){
			// reset head
			return DequeuedHead;
		}
		// put back tail item
		tLinkOperator::ReleaseStore(fHead,DequeuedTail);
		// remove one pending item from tail of dequeue chain
		if(DequeuePrev!=nullptr){
			TSingleLinkedItemOperator::SetNext(DequeuePrev,nullptr);
			return DequeuedHead;
		}
		// no item dequeued
		return nullptr;
	}

	// IsEmpty
	// return true if stack is empty
	bool	IsEmpty(void)const noexcept(true){
		return tLinkOperator::AcquireLoad(fHead)==nullptr;
	}

private:
	typename tLinkOperator::tLink fHead;
	cAtomicVariable<tItem*> fTail;

	bool TailReset(tItem *DequeuedTail)noexcept(true){
		// try to reset to empty queue
		if(fTail.Acquire.CmpStore(DequeuedTail,nullptr)){
			// success, queue reseted to initial state
			return true;
		}
		// tail moved again
		tItem *NewHead=TSingleLinkedItemOperator::GetNext(DequeuedTail);
		if(NewHead!=nullptr){
			// got newly enqueued, put to head
			tLinkOperator::Set(fHead,NewHead);

			TSingleLinkedItemOperator::SetNext(DequeuedTail,nullptr);
			return true;
		}
		// restore dequeued tail to head
		tLinkOperator::Set(fHead,DequeuedTail);

		return false;
	}
};


template<class TSingleLinkedItemOperator>
class cAtomicQueueSO
{
public:
	typedef typename TSingleLinkedItemOperator::tItem tItem;

	cnLib_CONSTEXPR_FUNC cAtomicQueueSO()noexcept(true):fHead(nullptr),fTail(nullptr){}

	// Enqueue
	//	Enqueue item into queue tail
	void Enqueue(tItem *Item)noexcept(true){	return EnqueueChain(Item,Item);	}
	// Enqueue
	//	Enqueue item chain into queue tail. caller must ensure the chain from first to last is valid, otherwise the state of the queue is undefined.
	// First		first item of the chain
	// Last			last item of the chain
	void EnqueueChain(tItem *First,tItem *Last)noexcept(true){
		cnLib_ASSERT(First!=nullptr);
		cnLib_ASSERT(Last!=nullptr);

		TSingleLinkedItemOperator::SetNext(Last,nullptr);
		// Tail->Next=Item; Tail=Item
		tItem *OldTail;
		if((OldTail=fTail.Release.Xchg(Last))==nullptr){
			// the queue was empty and now in initial state
			// need link up head
			fHead.Release.Store(First);
		}
		else{
			// link up the queue
			TSingleLinkedItemOperator::SetNext(OldTail,First);
		}
	}
	// Dequeue
	//	dequeue item from queue head
	// return dequeued item, dequeued item's Next remain unchanged
	tItem* Dequeue(void)noexcept(true){
		// DequeuedItem=Head; Head=Head->Next;

		tItem *CurHead=fHead.Acquire.Load();
		tItem *NextHead;
		do{
			if(CurHead==nullptr)
				return nullptr;

			NextHead=TSingleLinkedItemOperator::GetNext(CurHead);
		}while(!fHead.Release.CmpXchg(CurHead,NextHead));

		// test queue end
		if(NextHead!=nullptr){
			// tail not reached
			return CurHead;
		}

		// reached tail, try to reset to empty queue
		if(TailReset(CurHead)){
			return CurHead;
		}
		// item restore to head, no item dequeued
		fHead.Release.Store(CurHead);
		return nullptr;
	}

	// DequeueAll
	//	try to dequeue all available items in the queue
	// return head of dequeue chain
	tItem* DequeueAll(void)noexcept(true){
		// DequeuedItem=Head; Head=nullptr; Tail=nullptr;
		tItem *DequeuedHead=fHead.Acquire.Xchg(nullptr);
		if(DequeuedHead==nullptr)
			return nullptr;

		// find tail
		tItem *DequeuePrev=nullptr;
		tItem *DequeuedTail=DequeuedHead;
		for(auto CurItemNext=TSingleLinkedItemOperator::GetNext(DequeuedTail);CurItemNext!=nullptr;CurItemNext=TSingleLinkedItemOperator::GetNext(DequeuedTail)){
			DequeuePrev=DequeuedTail;
			DequeuedTail=CurItemNext;
		}

		// reset tail
		if(TailReset(DequeuedTail)){
			// reset head
			return DequeuedHead;
		}
		// put back tail item
		fHead.Release.Store(DequeuedTail);
		// remove one pending item from tail of dequeue chain
		if(DequeuePrev!=nullptr){
			TSingleLinkedItemOperator::SetNext(DequeuePrev,nullptr);
			return DequeuedHead;
		}
		// no item dequeued
		return nullptr;
	}

	// IsEmpty
	// return true if stack is empty
	bool	IsEmpty(void)const noexcept(true){
		return fHead.Acquire.Load()==nullptr;
	}

private:
	cAtomicVariable<tItem*> fHead;
	cAtomicVariable<tItem*> fTail;

	bool TailReset(tItem *DequeuedTail)noexcept(true){
		// try to reset to empty queue
		if(fTail.Acquire.CmpStore(DequeuedTail,nullptr)){
			// success, queue reseted to initial state
			return true;
		}
		// tail moved again
		tItem *NewHead=TSingleLinkedItemOperator::GetNext(DequeuedTail);
		if(NewHead!=nullptr){
			// got newly enqueued, put to head
			fHead.Set(NewHead);

			TSingleLinkedItemOperator::SetNext(DequeuedTail,nullptr);
			return true;
		}
		// restore dequeued tail to head
		fHead.Set(DequeuedTail);

		return false;
	}
};
//---------------------------------------------------------------------------
template<class TSingleLinkedItemOperator>
class cSingleLinkedItemRecycler
{
public:
	typedef typename TSingleLinkedItemOperator::tItem tItem;
	cnLib_STATIC_ASSERT(cnVar::TCanDelete<tItem>::Value,"Recycler needs type can be deleted");

	cSingleLinkedItemRecycler()noexcept(true):fPool(cnVar::StaticInitializedSinglton<cRecyclePool>()){}

	tItem* Fetch(void)noexcept(true){
		return fPool->RecycleStack.Pop();
	}
	tItem* Create(void)noexcept(true){
		tItem *Object=fPool->RecycleStack.Pop();
		if(Object!=nullptr)
			return Object;

		return new tItem;
	}
	void Recycle(tItem *Object)noexcept(true){
		fPool->RecycleStack.Push(Object);
	}


private:
	class cRecyclePool
	{
	public:
		cnLib_CONSTEXPR_FUNC cRecyclePool()noexcept(true):fRefCount(0){}
		cAtomicStack<TSingleLinkedItemOperator> RecycleStack;
	
	
		void IncreaseReference(void)noexcept(true){
			fRefCount.Free++;
		}
		void DecreaseReference(void)noexcept(true){
			if(fRefCount.Free--==1){
				ClearObjects();
			}
		}
	protected:
	
		void ClearObjects(void)noexcept(true){
			auto ItemsToDelete=RecycleStack.Swap(nullptr);
			while(ItemsToDelete!=nullptr){
				auto DeleteObject=ItemsToDelete;
				ItemsToDelete=TSingleLinkedItemOperator::GetNext(ItemsToDelete);

				delete DeleteObject;
			}
		}

	private:
		cAtomicVariable<typename cnVar::TSelect<0,uIntn,TSingleLinkedItemOperator>::Type> fRefCount;
	};

	rPtr<cRecyclePool> fPool;
	
};
//---------------------------------------------------------------------------
}	// namespace cnAsync
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
