/*- cnTK - Thread Synchronization -----------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-02-05                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_ThreadSync_HPP__
#define	__cnLibrary_cnTK_ThreadSync_HPP__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/Atomic.hpp>
#include <cnTK/Coroutine.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 3
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace TKRuntime{
//---------------------------------------------------------------------------

struct ThreadNotification;
//{
//	void Start(void)noexcept;
//	void Finish(void)noexcept;
//	void Wait(void)noexcept;
//	void Notify(void)noexcept;
//};

//---------------------------------------------------------------------------
}	// namespace TKRuntime
//---------------------------------------------------------------------------
namespace cnAsync{
//---------------------------------------------------------------------------

template<class TRet,class TCoroutineHandleOperator>
class cGenerator : private iCoroutinePromiseAwaiter
{
public:
	cGenerator()noexcept(true){}
	~cGenerator()noexcept(true){}

	typedef typename cResumable<TRet,TCoroutineHandleOperator>::pPtr pPtr;


	// construct by promise
	cGenerator(cResumable<TRet,TCoroutineHandleOperator> &&Src)noexcept(true):fPromise(Src.TakePromise()){}

	// move construct
	cGenerator(cGenerator &&Src)noexcept(true):fPromise(static_cast<pPtr&&>(Src.fPromise)){}

	// move assignment
	cGenerator& operator=(cGenerator &&Src)noexcept(true){
		fPromise=static_cast<pPtr&&>(Src.fPromise);
		return *this;
	}

	// check state

	operator typename cResumable<void,TCoroutineHandleOperator>::promise_type*()const noexcept(true){
		return fPromise;
	}

	// run function

	bool Run(void)const noexcept(true){
		fStateNotification.Start();

		bool RunResult=fPromise->Run(this);
		if(RunResult){
			fStateNotification.Wait();
		}
		fStateNotification.Finish();
		return RunResult;
	}
	bool operator () (void)const noexcept(true){
		return Run();
	}

	// Get current value

	const TRet& Value(void)const noexcept(true){
		return fPromise->fReturnValue;
	}
	TRet& Value(void)noexcept(true){
		return fPromise->fReturnValue;
	}

	const TRet& operator *()const noexcept(true){
		return fPromise->fReturnValue;
	}
	TRet& operator *()noexcept(true){
		return fPromise->fReturnValue;
	}

private:
	pPtr fPromise;
	virtual void NotifyCompletion(void)noexcept(true)override{
		fStateNotification.Notify();
	}
	typename cnVar::TSelect<1,TCoroutineHandleOperator,TKRuntime::ThreadNotification>::Type fStateNotification;
};
//---------------------------------------------------------------------------
template<class TCoroutineHandleOperator>
class cGenerator<void,TCoroutineHandleOperator> : private iCoroutinePromiseAwaiter
{
public:

	typedef typename cResumable<void,TCoroutineHandleOperator>::pPtr pPtr;

	cGenerator()noexcept(true){}
	~cGenerator()noexcept(true){}


	// construct by promise

	cGenerator(cResumable<void,TCoroutineHandleOperator> &&Src)noexcept(true):fPromise(Src.TakePromise()){}
	// move construct
	cGenerator(cGenerator &&Src)noexcept(true):fPromise(Src.fPromise){
		Src.fPromise=nullptr;
	}

	// move assignment
	cGenerator &operator=(cGenerator &&Src)noexcept(true){
		fPromise=static_cast<pPtr&&>(Src.fPromise);
		return *this;
	}

	// check state

	operator typename cResumable<void,TCoroutineHandleOperator>::promise_type*()const noexcept(true){
		return fPromise;
	}

	// run function

	bool Run(void)const noexcept(true){
		fStateNotification.Start();

		bool RunResult=fPromise->Run(this);
		if(RunResult){
			fStateNotification.Wait();
		}
		fStateNotification.Finish();
		return RunResult;
	}
	bool operator () (void)const noexcept(true){
		return Run();
	}

private:
	pPtr fPromise;
	virtual void NotifyCompletion(void)noexcept(true)override{
		fStateNotification.Notify();
	}
	typename cnVar::TSelect<1,TCoroutineHandleOperator,TKRuntime::ThreadNotification>::Type fStateNotification;
};

//---------------------------------------------------------------------------
}	// namespace cnAsync
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 3 */
/*-------------------------------------------------------------------------*/
#endif
