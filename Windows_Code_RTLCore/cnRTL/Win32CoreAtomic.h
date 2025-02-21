/*- CLib - CRT Atomic -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-06-04                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#include <cnRTL\Win32Interlocked.h>

/*-------------------------------------------------------------------------*/
#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------

template<uIntn IntegerSize>
struct TKRuntime::TAtomicInteger
{
	typedef typename cnVar::TIntegerOfSize<IntegerSize,false>::Type tInteger;
	typedef typename cnVar::TIntegerOfSize<IntegerSize,true>::Type tSInteger;
	typedef volatile tInteger tAtomic;
	typedef tInteger tNonAtomic;

	static tInteger Get(const tAtomic &a){
		return a;
	}
	static void Set(tAtomic &a,const tInteger &v){
		a=v;
	}

	static tInteger FreeLoad(const tAtomic &a){
		::MemoryBarrier();
		return a;
	}
	static tInteger AcquireLoad(const tAtomic &a){
		::MemoryBarrier();
		return a;
	}


	static void FreeStore(tAtomic &a,const tInteger &v){
		return ReleaseStore(a,v);
	}
	static void ReleaseStore(tAtomic &a,const tInteger &v){
		cnRTL::cnWinRTL::Interlocked::Xchg(a,v);
	}

	static tInteger FreeExchange(tAtomic &a,const tInteger &v){
		return BarrierExchange(a,v);
	}
	static tInteger AcquireExchange(tAtomic &a,const tInteger &v){
		return BarrierExchange(a,v);
	}
	static tInteger ReleaseExchange(tAtomic &a,const tInteger &v){
		return BarrierExchange(a,v);
	}
	static tInteger BarrierExchange(tAtomic &a,const tInteger &v){
		return cnRTL::cnWinRTL::Interlocked::Xchg(a,v);
	}

	static bool FreeCompareStore(tAtomic &a,tInteger c,const tInteger &v){
		return BarrierCompareStore(a,c,v);
	}
	static bool AcquireCompareStore(tAtomic &a,tInteger c,const tInteger &v){
		return BarrierCompareStore(a,c,v);
	}
	static bool ReleaseCompareStore(tAtomic &a,tInteger c,const tInteger &v){
		return BarrierCompareStore(a,c,v);
	}
	static bool BarrierCompareStore(tAtomic &a,tInteger c,const tInteger &v){
		return c==cnRTL::cnWinRTL::Interlocked::CmpXchg(a,v,c);
	}

	static bool FreeCompareExchange(tAtomic &a,tInteger &c,const tInteger &v){
		return BarrierCompareExchange(a,c,v);
	}
	static bool AcquireCompareExchange(tAtomic &a,tInteger &c,const tInteger &v){
		return BarrierCompareExchange(a,c,v);
	}
	static bool ReleaseCompareExchange(tAtomic &a,tInteger &c,const tInteger &v){
		return BarrierCompareExchange(a,c,v);
	}
	static bool BarrierCompareExchange(tAtomic &a,tInteger &c,const tInteger &v){
		tInteger oc=c;
		c=cnRTL::cnWinRTL::Interlocked::CmpXchg(a,v,c);
		return oc==c;
	}

	static tInteger FreeAdd(tAtomic &a,const tInteger &v){
		return BarrierAdd(a,v);
	}
	static tInteger FreeAddN(tAtomic &a,const tInteger &v){
		return BarrierAddN(a,v);
	}
	static tInteger AcquireAdd(tAtomic &a,const tInteger &v){
		return BarrierAdd(a,v);
	}
	static tInteger AcquireAddN(tAtomic &a,const tInteger &v){
		return BarrierAddN(a,v);
	}
	static tInteger ReleaseAdd(tAtomic &a,const tInteger &v){
		return BarrierAdd(a,v);
	}
	static tInteger ReleaseAddN(tAtomic &a,const tInteger &v){
		return BarrierAddN(a,v);
	}
	static tInteger BarrierAdd(tAtomic &a,const tInteger &v){
		return cnRTL::cnWinRTL::Interlocked::XchgAdd(a,v);
	}
	static tInteger BarrierAddN(tAtomic &a,const tInteger &v){
		return cnRTL::cnWinRTL::Interlocked::XchgAdd(a,v)+v;
	}

	static tInteger FreeSub(tAtomic &a,const tInteger &v){
		return BarrierSub(a,v);
	}
	static tInteger FreeSubN(tAtomic &a,const tInteger &v){
		return BarrierSubN(a,v);
	}
	static tInteger AcquireSub(tAtomic &a,const tInteger &v){
		return BarrierSub(a,v);
	}
	static tInteger AcquireSubN(tAtomic &a,const tInteger &v){
		return BarrierSubN(a,v);
	}
	static tInteger ReleaseSub(tAtomic &a,const tInteger &v){
		return BarrierSub(a,v);
	}
	static tInteger ReleaseSubN(tAtomic &a,const tInteger &v){
		return BarrierSubN(a,v);
	}
	static tInteger BarrierSub(tAtomic &a,const tInteger &v){
		return cnRTL::cnWinRTL::Interlocked::XchgAdd(a,static_cast<tInteger>(-static_cast<tSInteger>(v)));
	}
	static tInteger BarrierSubN(tAtomic &a,const tInteger &v){
		return cnRTL::cnWinRTL::Interlocked::XchgAdd(a,static_cast<tInteger>(-static_cast<tSInteger>(v)))-v;
	}
};

template<class TPointer>
struct TKRuntime::TAtomicLink
{
		
	typedef TPointer tPointer;
	typedef volatile tPointer tLink;

	static tPointer Get(const tLink &Link){		return Link;	}
	static void Set(tLink &Link,tPointer p){	Link=p;	}

	static tPointer AcquireLoad(const tLink &Link){
		::MemoryBarrier();
		return Link;
	}
	static void ReleaseStore(tLink &Link,tPointer p){
		cnRTL::cnWinRTL::Interlocked::Xchg(reinterpret_cast<volatile uIntn&>(Link),reinterpret_cast<uIntn>(p));
	}

	static tPointer Exchange(tLink &Link,tPointer New){
		return reinterpret_cast<tPointer>(cnRTL::cnWinRTL::Interlocked::Xchg(reinterpret_cast<volatile uIntn&>(Link),reinterpret_cast<uIntn>(New)));
	}
	
	template<class TSingleLinkItemOperator>
	static tPointer Append(tLink &Link,tPointer First,tPointer Last){
		uIntn TargetTop=reinterpret_cast<volatile uIntn&>(Link);
		tPointer CurTop;
		do{
			CurTop=reinterpret_cast<tPointer>(TargetTop);
			TSingleLinkItemOperator::SetNext(Last,CurTop);
			
			TargetTop=cnRTL::cnWinRTL::Interlocked::CmpXchg(reinterpret_cast<volatile uIntn&>(Link),reinterpret_cast<uIntn>(First),TargetTop);
		}while(reinterpret_cast<uIntn>(CurTop)!=TargetTop);
		return CurTop;
	}

	template<class TSingleLinkItemOperator>
	static tPointer Extract(tLink &Link,tPointer &NextTop){
		tPointer CurTop;
		uIntn TargetTop=reinterpret_cast<volatile uIntn&>(Link);
		do{
			if(TargetTop==0)
				return nullptr;

			CurTop=reinterpret_cast<tPointer>(TargetTop);
			NextTop=TSingleLinkItemOperator::GetNext(CurTop);

			TargetTop=cnRTL::cnWinRTL::Interlocked::CmpXchg(reinterpret_cast<volatile uIntn&>(Link),reinterpret_cast<uIntn>(NextTop),TargetTop);
		}while(reinterpret_cast<uIntn>(CurTop)!=TargetTop);
		return CurTop;
	}
};

//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
