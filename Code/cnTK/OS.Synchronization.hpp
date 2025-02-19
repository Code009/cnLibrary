/*- cnTK - Synchronization ------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2025-02-13                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Synchronization_HPP__
#define	__cnLibrary_cnTK_Synchronization_HPP__
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
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace TKRuntime{
//---------------------------------------------------------------------------

//struct AllocationOperator;
//	: TAllocationOperator
//{
//	static uIntn SizeOf(void *Pointer)noexcept;
//};

struct Thread;
//{
//	typedef tThreadID;
//	//typedef tThreadReference;
// 
//	static void Yield(void)noexcept;
//	static void SpinWait(uIntn SpinCount)noexcept;
//	static void Suspend(uInt64 Duration)noexcept;
//
//	static tThreadID GetID(void)noexcet;
// 
//	tSingleNotification;
//	{
//		void Setup(void)noexcept;
//		void Clear(void)noexcept;
//		void Wait(void)noexcept;
//		bool Wait(ufInt64 Duration)noexcept;
//		void Notify(void)noexcept;
//	};
//};

//---------------------------------------------------------------------------
}	// namespace TKRuntime
//---------------------------------------------------------------------------
namespace cnAsync{
//---------------------------------------------------------------------------
template<uIntn SpinCount>
class cSpinLock
{
public:
	typedef typename cnVar::TSelect<0&SpinCount,TKRuntime::Thread>::Type TKRuntimeThread;
	cnLib_CONSTEXPR_FUNC cSpinLock()noexcept(true):fOwned(false){}
#ifdef cnLib_DEBUG
	~cSpinLock()noexcept(true){	cnLib_ASSERT(fOwned==false);	}
#endif // cnLib_DEBUG

	void Acquire(void)noexcept(true){
		while(fOwned.Barrier.CmpStore(false,true)==false){
			TKRuntimeThread::SpinWait(SpinCount);
			if(fOwned.Barrier.CmpStore(false,true)==false){
				TKRuntimeThread::Yield();
			}
		}
	}
	bool TryAcquire(void)noexcept(true){
		return fOwned.Barrier.CmpStore(false,true);
	}
	void Release(void)noexcept(true){
		fOwned.Release.Store(false);
	}
protected:
	cAtomicVariable<typename cnVar::TSelect<0&SpinCount,bool>::Type> fOwned=false;
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
