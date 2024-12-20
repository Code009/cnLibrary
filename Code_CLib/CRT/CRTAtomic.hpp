/*- CLib - CRT Atomic -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-06-04                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_CLib_CRT_CRTAtomic_H__
#define __cnLibrary_CLib_CRT_CRTAtomic_H__

/*-------------------------------------------------------------------------*/
#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------


template<uIntn IntegerSize>
struct TKRuntime::TAtomicInteger
{
	typedef typename cnVar::TIntegerOfSize<IntegerSize,false>::Type tInteger;
	typedef volatile std::atomic<tInteger> tAtomic;
	typedef tInteger tNonAtomic;

	template<class T>
	struct tDebugCast
	{
		typedef std::atomic<T> Type;
	};
	typedef std::atomic<tInteger> tDebugDisplay;

	static tInteger Get(const tAtomic &a){
		return a;
	}
	static void Set(tAtomic &a,const tInteger &v){
		a=v;
	}

	static tInteger FreeLoad(const tAtomic &a){
		return a.load(std::memory_order::memory_order_relaxed);
	}
	static tInteger AcquireLoad(const tAtomic &a){
		return a.load(std::memory_order::memory_order_acquire);
	}

	static bool WatchEqual(const tAtomic &a,const tInteger &Value,uIntn Count)noexcept{
		for(uIntn i=0;i<Count;i++){
			if(a.load(std::memory_order::memory_order_relaxed)==Value){
				return true;
			}
		}
		return false;
	}
	static bool WatchNotEqual(const tAtomic &a,const tInteger &Value,uIntn Count)noexcept{
		for(uIntn i=0;i<Count;i++){
			if(a.load(std::memory_order::memory_order_relaxed)!=Value){
				return true;
			}
		}
		return false;
	}


	static void FreeStore(tAtomic &a,const tInteger &v){
		a.store(v,std::memory_order::memory_order_relaxed);
	}
	static void ReleaseStore(tAtomic &a,const tInteger &v){
		a.store(v,std::memory_order::memory_order_release);
	}

	static tInteger FreeExchange(tAtomic &a,const tInteger &v){
		return a.exchange(v,std::memory_order::memory_order_relaxed);
	}
	static tInteger AcquireExchange(tAtomic &a,const tInteger &v){
		return a.exchange(v,std::memory_order::memory_order_acquire);
	}
	static tInteger ReleaseExchange(tAtomic &a,const tInteger &v){
		return a.exchange(v,std::memory_order::memory_order_release);
	}
	static tInteger BarrierExchange(tAtomic &a,const tInteger &v){
		return a.exchange(v,std::memory_order::memory_order_acq_rel);
	}

	static bool FreeCompareStore(tAtomic &a,tInteger c,const tInteger &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_relaxed);
	}
	static bool AcquireCompareStore(tAtomic &a,tInteger c,const tInteger &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_acquire);
	}
	static bool ReleaseCompareStore(tAtomic &a,tInteger c,const tInteger &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_release);
	}
	static bool BarrierCompareStore(tAtomic &a,tInteger c,const tInteger &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_acq_rel);
	}

	static bool FreeCompareExchange(tAtomic &a,tInteger &c,const tInteger &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_relaxed);
	}
	static bool AcquireCompareExchange(tAtomic &a,tInteger &c,const tInteger &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_acquire);
	}
	static bool ReleaseCompareExchange(tAtomic &a,tInteger &c,const tInteger &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_release);
	}
	static bool BarrierCompareExchange(tAtomic &a,tInteger &c,const tInteger &v){
		return a.compare_exchange_strong(c,v,std::memory_order::memory_order_acq_rel);
	}

	static tInteger FreeAdd(tAtomic &a,const tInteger &v){
		return a.fetch_add(v,std::memory_order::memory_order_relaxed);
	}
	static tInteger FreeAddN(tAtomic &a,const tInteger &v){
		return a.fetch_add(v,std::memory_order::memory_order_relaxed)+v;
	}
	static tInteger AcquireAdd(tAtomic &a,const tInteger &v){
		return a.fetch_add(v,std::memory_order::memory_order_acquire);
	}
	static tInteger AcquireAddN(tAtomic &a,const tInteger &v){
		return a.fetch_add(v,std::memory_order::memory_order_acquire)+v;
	}
	static tInteger ReleaseAdd(tAtomic &a,const tInteger &v){
		return a.fetch_add(v,std::memory_order::memory_order_release);
	}
	static tInteger ReleaseAddN(tAtomic &a,const tInteger &v){
		return a.fetch_add(v,std::memory_order::memory_order_release)+v;
	}
	static tInteger BarrierAdd(tAtomic &a,const tInteger &v){
		return a.fetch_add(v,std::memory_order::memory_order_acq_rel);
	}
	static tInteger BarrierAddN(tAtomic &a,const tInteger &v){
		return a.fetch_add(v,std::memory_order::memory_order_acq_rel)+v;
	}

	static tInteger FreeSub(tAtomic &a,const tInteger &v){
		return a.fetch_sub(v,std::memory_order::memory_order_relaxed);
	}
	static tInteger FreeSubN(tAtomic &a,const tInteger &v){
		return a.fetch_sub(v,std::memory_order::memory_order_relaxed)-v;
	}
	static tInteger AcquireSub(tAtomic &a,const tInteger &v){
		return a.fetch_sub(v,std::memory_order::memory_order_acquire);
	}
	static tInteger AcquireSubN(tAtomic &a,const tInteger &v){
		return a.fetch_sub(v,std::memory_order::memory_order_acquire)-v;
	}
	static tInteger ReleaseSub(tAtomic &a,const tInteger &v){
		return a.fetch_sub(v,std::memory_order::memory_order_release);
	}
	static tInteger ReleaseSubN(tAtomic &a,const tInteger &v){
		return a.fetch_sub(v,std::memory_order::memory_order_release)-v;
	}
	static tInteger BarrierSub(tAtomic &a,const tInteger &v){
		return a.fetch_sub(v,std::memory_order::memory_order_acq_rel);
	}
	static tInteger BarrierSubN(tAtomic &a,const tInteger &v){
		return a.fetch_sub(v,std::memory_order::memory_order_acq_rel)-v;
	}
};


template<class TPointer>
struct TKRuntime::TAtomicLink
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

//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
