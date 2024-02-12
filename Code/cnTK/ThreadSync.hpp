/*- cnTK - Thread Synchronization -----------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-02-05                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_ThreadSync_H__
#define	__cnLibrary_cnTK_ThreadSync_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.h>
#include <cnTK/Atomic.h>
#include <cnTK/Coroutine.h>
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
//	void Start(void);
//	void Finish(void);
//	void Wait(void);
//	void Notify(void);
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
	TKRuntime::ThreadNotification fStateNotification;
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
		StateNotification.Start();

		bool RunResult=fPromise->Run(this);
		if(RunResult){
			StateNotification.Wait();
		}
		StateNotification.Finish();
		return RunResult;
	}
	bool operator () (void)const noexcept(true){
		return Run();
	}

private:
	pPtr fPromise;
	virtual void NotifyCompletion(void)noexcept(true)override{
		StateNotification.Notify();
	}
	TKRuntime::ThreadNotification StateNotification;
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
