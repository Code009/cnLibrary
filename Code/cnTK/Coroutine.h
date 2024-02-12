/*- cnTK - Coroutine ------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-05-06                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Coroutine_H__
#define	__cnLibrary_cnTK_Coroutine_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.h>
#include <cnTK/Atomic.h>
#include <cnTK/Memory.h>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 2
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnAsync{
//---------------------------------------------------------------------------
//TResumable<TRet>
//{
//	operator bool()const noexcept;
//	Awaitable<TRet> operator () (void);
//	Awaitable<TRet> Run(void) (void);
//	const TRet& Value(void)const noexcept;
//	const TRet& operator *()const noexcept;
//};
//---------------------------------------------------------------------------
//TResumable<void>
//{
//	operator bool()const noexcept;
//	Awaitable operator () (void);
//	Awaitable Run(void) (void);
//};
//---------------------------------------------------------------------------
//TGenerator<TRet>
//{
//	operator bool()const noexcept;
//	bool operator () (void);
//	bool Run(void) (void);
//	const TRet& Value(void)const noexcept;
//	const TRet& operator *()const noexcept;
//};
//---------------------------------------------------------------------------
//TGenerator<void>
//{
//	operator bool()const noexcept;
//	bool operator () (void);
//	bool Run(void) (void);
//};


//TCoroutineHandleOperator
//{
//	typedef tHandle;
//	static void Reset(tHandle &Handle);
//	static void Assign(tHandle &Handle,T&& Handle);
//	static bool IsAvailable(const tHandle &Handle);
//	static void Resume(tHandle &Handle);
//	static void Finish(tHandle &Handle);
//};
//---------------------------------------------------------------------------
struct cAwaiterReadyAlways
{
	bool await_ready(void)const noexcept(true){ return true; }
	template<class TAwaitCoHandle>
	void await_suspend(TAwaitCoHandle cnLib_UREF)const noexcept(true){}
	void await_resume(void)const noexcept(true){}
};
//---------------------------------------------------------------------------
struct cAwaiterSuspendForever
{
	bool await_ready(void)const noexcept(true){ return false; }
	template<class TAwaitCoHandle>
	void await_suspend(TAwaitCoHandle cnLib_UREF)const noexcept(true){}
	void await_resume(void)const noexcept(true){}
};
//---------------------------------------------------------------------------
template<class TCoroutineHandleOperator>
class cFreeCoroutine
{
public:
	struct cTerminateSuspension
	{
		bool await_ready(void)const noexcept(true){ return false; }
		template<class TAwaitCoHandle>
		void await_suspend(TAwaitCoHandle cnLib_UREF CoHandle)const noexcept(true){
			 
			typename TCoroutineHandleOperator::tHandle FinalHandle;
			TCoroutineHandleOperator::Assign(FinalHandle,CoHandle);
			TCoroutineHandleOperator::Finish(FinalHandle);
		}
		void await_resume(void)const noexcept(true){}
	};

	class promise_type
	{
	public:
		cFreeCoroutine get_return_object(void)noexcept(true){	return cFreeCoroutine();	}
		// promise
		cAwaiterReadyAlways initial_suspend(void)noexcept(true){	return cAwaiterReadyAlways();	}
		cTerminateSuspension final_suspend(void)noexcept(true){		return cTerminateSuspension();	}
		void unhandled_exception(void)noexcept(true){}
		void return_void(void)noexcept(true){}
	};
};
//---------------------------------------------------------------------------
class iCoroutinePromiseAwaiter
{
public:
	virtual void NotifyCompletion(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
template<class T>
struct cCoroutinePromiseOwnerTokenOperator : cnVar::bcPointerOwnerTokenOperator<T*>
{
	static void Release(T* const &Token)noexcept(true){
		if(Token!=nullptr)
			Token->OnFinish();
	}
};	
//---------------------------------------------------------------------------
template<class TCoroutineHandleOperator>
class bcCoroutinePromise
{
public:

	// final awaiter

	struct cFinalSuspension
	{
		cFinalSuspension(bcCoroutinePromise &Owner):Owner(Owner){}

		bool await_ready(void)const noexcept(true){	return false;	}
		template<class TAwaitCoHandle>
		void await_suspend(TAwaitCoHandle cnLib_UREF CoHandle)noexcept(true){
			TCoroutineHandleOperator::Assign(Owner.fFinalHandle,cnLib_UREFCAST(TAwaitCoHandle)(CoHandle));

			Owner.OnFinish();
		}
		void await_resume(void)noexcept(true){}	// never resume from final suspend
		bcCoroutinePromise &Owner;
	};

	bcCoroutinePromise()noexcept(true)
		: fFinishFlag(0)
		, fAwaiter(nullptr)
	{}

	cAwaiterReadyAlways initial_suspend(void)noexcept(true){	return cAwaiterReadyAlways();	}
	cFinalSuspension final_suspend(void)noexcept(true){		return *this;	}
	void unhandled_exception(void)noexcept(true){	OnReturn();	}

	bool IsDone(void)const noexcept(true){	return TCoroutineHandleOperator::IsAvailable(fFinalHandle);	}

	bool SetupAwait(iCoroutinePromiseAwaiter *Awaiter)noexcept(true){
		iCoroutinePromiseAwaiter *PrevAwaiter=nullptr;
		if(fAwaiter.Free.CmpXchg(PrevAwaiter,Awaiter)==false){
			// failed to set awaiter
			return false;
		}
		if(PrevAwaiter==reinterpret_cast<iCoroutinePromiseAwaiter*>(this)){
			// routine is completed already
			return false;
		}
		else{
			// registered
			return true;
		}
	}


protected:
	void OnReturn(void)noexcept(true){
		// notify awaiter
		iCoroutinePromiseAwaiter *Awaiter=fAwaiter.Free.Xchg(reinterpret_cast<iCoroutinePromiseAwaiter*>(this));
		if(Awaiter!=nullptr){
			Awaiter->NotifyCompletion();
		}
	}

private:
	cAtomicVariable<typename cnVar::TSelect<0,iCoroutinePromiseAwaiter*,TCoroutineHandleOperator>::Type> fAwaiter;
	cAtomicVariable<typename cnVar::TSelect<0,ufInt8,TCoroutineHandleOperator>::Type> fFinishFlag;
	typename TCoroutineHandleOperator::tHandle fFinalHandle;

	template<class T> friend struct cnAsync::cCoroutinePromiseOwnerTokenOperator;
	void OnFinish(void)noexcept(true){
		// will be called once by owner and once by final suspension
		if(fFinishFlag.Free.Xchg(1)){
			// second call, destroy handle
			TCoroutineHandleOperator::Finish(fFinalHandle);
		}
		else{
			// still running, wait for owner or final suspension
		}
	}
};

//---------------------------------------------------------------------------
template<class TRet,class TCoroutineHandleOperator>
class cCoroutine
{
public:
	class promise_type : public bcCoroutinePromise<TCoroutineHandleOperator>
	{
	public:
		cnVar::cPtrOwner< cCoroutinePromiseOwnerTokenOperator<promise_type> > get_return_object()noexcept(true)
		{	return cnVar::cPtrOwner< cCoroutinePromiseOwnerTokenOperator<promise_type> >::TakeFromManual(this);	}

		template<class T>
		void return_value(T cnLib_UREF Value)noexcept(true){
			fReturnValue=cnLib_UREFCAST(T)(Value);
			this->OnReturn();
		}
		TRet fReturnValue;
	};
	typedef cnVar::cPtrOwner< cCoroutinePromiseOwnerTokenOperator<promise_type> > pPtr;

	class cAwaiter : public iCoroutinePromiseAwaiter
	{
	public:
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
		cAwaiter(pPtr &&p)noexcept(true):fPromise(static_cast<pPtr&&>(p)){}
		cAwaiter(cAwaiter &&Src)noexcept(true):fPromise(static_cast<pPtr&&>(Src.fPromise)){}
#else	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L
		cAwaiter(pPtr &p):fPromise(p){}
		cAwaiter(cAwaiter &Src)noexcept(true):fPromise(Src.fPromise)){}
#endif	//cnLibrary_CPPFEATURE_RVALUE_REFERENCES


		bool await_ready(void)const noexcept(true){
			if(fPromise==nullptr)
				return true;
			return fPromise->IsDone();
		}
		template<class TAwaitCoHandle>
		bool await_suspend(TAwaitCoHandle cnLib_UREF CoHandle){
			TCoroutineHandleOperator::Assign(fCoHandle,cnLib_UREFCAST(TAwaitCoHandle)(CoHandle));
			return fPromise->SetupAwait(this);
		}
		TRet& await_resume(void)noexcept(true){
			return fPromise->fReturnValue;
		}

		virtual void NotifyCompletion(void)noexcept(true)override{
			TCoroutineHandleOperator::Resume(fCoHandle);
		}

	protected:
		typename TCoroutineHandleOperator::tHandle fCoHandle;
		pPtr fPromise;
	};


#if cnLibrary_CPPFEATURE_COROUTINE >= 201902L

	typename cAwaiter operator co_await()noexcept(true){
		return cnLib_UREFCAST(pPtr)(fPromise);
	}
#endif	// cnLibrary_CPPFEATURE_COROUTINE >= 201902L

	cCoroutine()noexcept(true){}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// construct by promise
	cCoroutine(pPtr &&p)noexcept(true):fPromise(static_cast<pPtr&&>(p)){}
#else	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L
	cCoroutine(pPtr &p)noexcept(true):fPromise(p){}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES


#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// copy
	cCoroutine(cCoroutine &&Src)noexcept(true):fPromise(static_cast<pPtr&&>(Src.fPromise)){}

	// move
	cCoroutine &operator=(cCoroutine &&Src)noexcept(true){
		fPromise=static_cast<pPtr&&>(Src.fPromise);
		return *this;
	}
#else
	cCoroutine(cCoroutine &Src)noexcept(true):fPromise(Src.fPromise){}

	cCoroutine &operator=(cCoroutine &Src)noexcept(true){
		fPromise=Src.fPromise;
		return *this;
	}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// check state

	operator promise_type*()const noexcept(true){	return fPromise;	}
	pPtr&& TakePromise(void){	return static_cast<pPtr&&>(fPromise);	}

protected:
	pPtr fPromise;
};
//---------------------------------------------------------------------------
template<class TCoroutineHandleOperator>
class cCoroutine<void,TCoroutineHandleOperator>
{
public:
	class promise_type : public bcCoroutinePromise<TCoroutineHandleOperator>
	{
	public:
		cnVar::cPtrOwner< cCoroutinePromiseOwnerTokenOperator<promise_type> > get_return_object()noexcept(true)
		{	return cnVar::cPtrOwner< cCoroutinePromiseOwnerTokenOperator<promise_type> >::TakeFromManual(this);	}

		void return_void()noexcept(true){	this->OnReturn();	}
	};
	typedef cnVar::cPtrOwner< cCoroutinePromiseOwnerTokenOperator<promise_type> > pPtr;
	
	class cAwaiter : public iCoroutinePromiseAwaiter
	{
	public:
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
		cAwaiter(pPtr &&p)noexcept(true):fPromise(static_cast<pPtr&&>(p)){}
		cAwaiter(cAwaiter &&Src)noexcept(true):fPromise(static_cast<pPtr&&>(Src.fPromise)){}
#else	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L
		cAwaiter(pPtr &p):fPromise(p){}
		cAwaiter(cAwaiter &Src)noexcept(true):fPromise(Src.fPromise)){}
#endif	//cnLibrary_CPPFEATURE_RVALUE_REFERENCES


		bool await_ready(void)const noexcept(true){
			if(fPromise==nullptr)
				return true;
			return fPromise->IsDone();
		}
		template<class TAwaitCoHandle>
		bool await_suspend(TAwaitCoHandle cnLib_UREF CoHandle){
			TCoroutineHandleOperator::Assign(fCoHandle,cnLib_UREFCAST(TAwaitCoHandle)(CoHandle));
			return fPromise->SetupAwait(this);
		}
		void await_resume(void)noexcept(true){}

		virtual void NotifyCompletion(void)noexcept(true)override{
			TCoroutineHandleOperator::Resume(fCoHandle);
		}

	protected:
		typename TCoroutineHandleOperator::tHandle fCoHandle;
		pPtr fPromise;
	};


#if cnLibrary_CPPFEATURE_COROUTINE >= 201902L

	typename cAwaiter operator co_await()noexcept(true){
		return cnLib_UREFCAST(pPtr)(fPromise);
	}
#endif	// cnLibrary_CPPFEATURE_COROUTINE >= 201902L

	cCoroutine()noexcept(true){}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// construct by promise
	cCoroutine(pPtr &&p)noexcept(true):fPromise(static_cast<pPtr&&>(p)){}
#else	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L
	cCoroutine(pPtr &p)noexcept(true):fPromise(p){}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES


#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// copy
	cCoroutine(cCoroutine &&Src)noexcept(true):fPromise(static_cast<pPtr&&>(Src.fPromise)){}

	// move
	cCoroutine &operator=(cCoroutine &&Src)noexcept(true){
		fPromise=static_cast<pPtr&&>(Src.fPromise);
		return *this;
	}
#else
	cCoroutine(cCoroutine &Src)noexcept(true):fPromise(Src.fPromise){}

	cCoroutine &operator=(cCoroutine &Src)noexcept(true){
		fPromise=Src.fPromise;
		return *this;
	}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// check state

	operator promise_type*()const noexcept(true){	return fPromise;	}

	pPtr&& TakePromise(void){	return static_cast<pPtr&&>(fPromise);	}
protected:
	pPtr fPromise;
};
//---------------------------------------------------------------------------
template<class T>
struct cResumablePromiseOwnerTokenOperator : cnVar::bcPointerOwnerTokenOperator<T*>
{
	static void Release(T* const &Token)noexcept(true){
		if(Token!=nullptr)
			Token->Terminate();
	}
};	
//---------------------------------------------------------------------------
template<class TCoroutineHandleOperator>
class bcResumablePromise
{
public:

	struct cInitialSuspension
	{
		bcResumablePromise &Owner;
		cInitialSuspension(bcResumablePromise &Owner):Owner(Owner){}

		bool await_ready(void)const noexcept(true){	return false;	}
		template<class TAwaitCoHandle>
		void await_suspend(TAwaitCoHandle cnLib_UREF CoHandle)noexcept(true){
			TCoroutineHandleOperator::Assign(Owner.fCoHandle,cnLib_UREFCAST(TAwaitCoHandle)(CoHandle));
		}
		void await_resume(void)noexcept(true){}
	};

	struct cFinalSuspension
	{
		cFinalSuspension(bcResumablePromise &Owner):Owner(Owner){}

		bool await_ready(void)const noexcept(true){	return false;	}
		template<class TAwaitCoHandle>
		void await_suspend(TAwaitCoHandle cnLib_UREF CoHandle)noexcept(true){
			TCoroutineHandleOperator::Assign(fOwner.fCoHandle,cnLib_UREFCAST(TAwaitCoHandle)(CoHandle));

			Owner.OnPhaseFinish();
			Owner.Terminate(1);
		}
		void await_resume(void)noexcept(true){}	// never resume from final suspend

		bcResumablePromise &Owner;
	};

	struct cYieldSuspension
	{
		cYieldSuspension(bcResumablePromise &Owner):Owner(Owner){}

		bool await_ready(void)const noexcept(true){	return false;	}
		template<class TAwaitCoHandle>
		void await_suspend(TAwaitCoHandle cnLib_UREF CoHandle)noexcept(true){
			TCoroutineHandleOperator::Assign(fOwner.fCoHandle,cnLib_UREFCAST(TAwaitCoHandle)(CoHandle));

			Owner.OnPhaseFinish(0);
		}
		void await_resume(void)noexcept(true){}	// never resume from final suspend

		bcResumablePromise &Owner;
	};

	bcResumablePromise()noexcept(true)
		: fRunStateFlag(0)
		, fFinishFlag(0)
		, fAwaiter(nullptr)
	{}
	~bcResumablePromise()noexcept(true){}

	cInitialSuspension initial_suspend(void)noexcept(true){	return *this;	}
	cFinalSuspension final_suspend(void)noexcept(true){	return *this;	}
	void unhandled_exception(void)noexcept(true){	}
	
	cYieldSuspension YieldSuspend(void)noexcept(true){	return *this;	}

	bool Run(iCoroutinePromiseAwaiter *Awaiter)noexcept(true){
		if(fRunStateFlag.Acquire.CmpStore(PrevState=0,2)==false){
			return false;
		}
		fAwaiter=Awaiter;

		TCoroutineHandleOperator::Resume(fCoHandle);
		return true;
	}

private:
	typename TCoroutineHandleOperator::tHandle fCoHandle;
	iCoroutinePromiseAwaiter *fAwaiter;
	cAtomicVariable<typename cnVar::TSelect<0,ufInt8,TCoroutineHandleOperator>::Type> fRunStateFlag;
	cAtomicVariable<typename cnVar::TSelect<0,ufInt8,TCoroutineHandleOperator>::Type> fFinishFlag;

	void OnPhaseFinish(ufInt8 NextState)noexcept(true){
		fRunStateFlag.Release.Store(NextState);
		fAwaiter->NotifyCompletion();
	}

	template<class T> friend struct cnAsync::cResumablePromiseOwnerTokenOperator;
	void OnFinish(void)noexcept(true){
		// will be called once by owner and once by final suspension
		if(fFinishFlag.Free.Xchg(1)){
			// second call, destroy handle
			TCoroutineHandleOperator::Finish(fCoHandle);
		}
		else{
			// still running, wait for owner or final suspension
		}
	}
};
//---------------------------------------------------------------------------
template<class TRet,class TCoroutineHandleOperator>
class cResumable
{
public:
	class promise_type : public bcResumablePromise<TCoroutineHandleOperator>
	{
	public:
		cnVar::cPtrOwner< cResumablePromiseOwnerTokenOperator<promise_type> > get_return_object()noexcept(true)
		{	return cnVar::cPtrOwner< cResumablePromiseOwnerTokenOperator<promise_type> >::TakeFromManual(this);	}

		template<class T>
		bcResumablePromise<TCoroutineHandleOperator>::cYieldSuspension yield_value(T cnLib_UREF Value)
			noexcept(cnLib_NOEXCEPTEXPR(fReturnValue=cnLib_UREFCAST(T)(Value)))
		{
			fReturnValue=cnLib_UREFCAST(T)(Value);
			return this->YieldSuspend();
		}

		template<class T>
		void return_value(T cnLib_UREF Value)
			noexcept(cnLib_NOEXCEPTEXPR(fReturnValue=cnLib_UREFCAST(T)(Value)))
		{
			fReturnValue=cnLib_UREFCAST(T)(Value);			
		}

		TRet fReturnValue;
	};
	typedef cnVar::cPtrOwner< cResumablePromiseOwnerTokenOperator<promise_type> > pPtr;

	cResumable()noexcept(true){}
	~cResumable()noexcept(true){}


	// construct by promise
	cResumable(promise_type *p)noexcept(true):fPromise(p){}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// move construct
	cResumable(cResumable &&Src)noexcept(true):fPromise(Src.fPromise){}

	// move assignment
	cResumable &operator=(cResumable &&Src)noexcept(true){
		fPromise=Src.fPromise;
		return *this;
	}
	pPtr&& TakePromise(void){	return static_cast<pPtr&&>(fPromise);	}

// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
#else
// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L

// move construct
	cResumable(cResumable &Src)noexcept(true):fPromise(Src.fPromise){}

	// move assignment
	cResumable &operator=(cResumable &Src)noexcept(true){
		fPromise=Src.fPromise;
		return *this;
	}

	pPtr& TakePromise(void){	return fPromise;	}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L

	// check state

	operator promise_type*()const noexcept(true){
		return fPromise;
	}


	class cAwaiter : public iCoroutinePromiseAwaiter
	{
	public:
		cAwaiter(promise_type *p)noexcept(true):fPromise(p){}

		bool await_ready(void)const noexcept(true){
			if(fPromise==nullptr)
				return true;
			if(fPromise->Run(this)){
				return false;
			}
			return true;
		}
		template<class TAwaitCoHandle>
		bool await_suspend(TAwaitCoHandle cnLib_UREF CoHandle){
			TCoroutineHandleOperator::Assign(fCoHandle,cnLib_UREFCAST(TAwaitCoHandle)(CoHandle));
			return fPromise->SetupAwait(this);
		}
		void await_resume(void)noexcept(true){}

		virtual void NotifyCompletion(void)noexcept(true)override{
			TCoroutineHandleOperator::Resume(fCoHandle);
		}

	protected:
		typename TCoroutineHandleOperator::tHandle fCoHandle;
		promise_type *fPromise;
	};

	// run function

	cAwaiter Run(void)const noexcept(true){
		return fPromise;
	}
	cAwaiter operator () (void)const noexcept(true){
		return fPromise;
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
};
//---------------------------------------------------------------------------
template<class TCoroutineHandleOperator>
class cResumable<void,TCoroutineHandleOperator>
{
public:

	class promise_type : public bcResumablePromise<TCoroutineHandleOperator>
	{
	public:
		promise_type* get_return_object()noexcept(true){	return this;	}

		bcResumablePromise<TCoroutineHandleOperator>::cYieldSuspension yield_value(void)noexcept(true){	return this->YieldSuspend();		}
		bcResumablePromise<TCoroutineHandleOperator>::cYieldSuspension yield_value(int)noexcept(true){	return this->YieldSuspend();		}

		void return_void(void)noexcept(true){}
	};
	typedef cnVar::cPtrOwner< cResumablePromiseOwnerTokenOperator<promise_type> > pPtr;

	cResumable()noexcept(true){}
	~cResumable()noexcept(true){}


	// construct by promise
	cResumable(promise_type *p)noexcept(true):fPromise(p){}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// move construct
	cResumable(cResumable &&Src)noexcept(true):fPromise(static_cast<pPtr&&>(Src.fPromise)){}

	// move assignment
	cResumable& operator=(cResumable &&Src)noexcept(true){
		fPromise=Src.fPromise;
		Src.fPromise=nullptr;
		return *this;
	}
	
	pPtr&& TakePromise(void){	return static_cast<pPtr&&>(fPromise);	}

#else

	// move construct
	cResumable(cResumable &Src)noexcept(true):fPromise(Src.fPromise){}

	// move assignment
	cResumable& operator=(cResumable &Src)noexcept(true){
		fPromise=Src.fPromise;
		return *this;
	}

	pPtr& TakePromise(void){	return fPromise;	}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// check state

	operator promise_type*()const noexcept(true){
		return fPromise;
	}



	class cAwaiter : public iCoroutinePromiseAwaiter
	{
	public:
		cAwaiter(promise_type *p)noexcept(true):fPromise(p){}

		bool await_ready(void)const noexcept(true){
			if(fPromise==nullptr)
				return true;
			if(fPromise->Run(this)){
				return false;
			}
			return true;
		}
		template<class TAwaitCoHandle>
		bool await_suspend(TAwaitCoHandle cnLib_UREF CoHandle){
			TCoroutineHandleOperator::Assign(fCoHandle,cnLib_UREFCAST(TAwaitCoHandle)(CoHandle));
			return fPromise->SetupAwait(this);
		}
		void await_resume(void)noexcept(true){}

		virtual void NotifyCompletion(void)noexcept(true)override{
			TCoroutineHandleOperator::Resume(fCoHandle);
		}

	protected:
		typename TCoroutineHandleOperator::tHandle fCoHandle;
		promise_type *fPromise;
	};

	// run function

	cAwaiter Run(void)const noexcept(true){
		return fPromise;
	}
	cAwaiter operator () (void)const noexcept(true){
		return fPromise;
	}

private:
	pPtr fPromise;
};
//---------------------------------------------------------------------------
template<uIntn Size,class TAlignment=uIntn>
struct cVariantCoroutineHandleOperator
{
	struct iHandle
	{
		virtual ~iHandle(){};
		virtual bool IsAvailable(void)const noexcept(true){	return false;	}
		virtual void Resume(void)const noexcept(true){}
		virtual void Finish(void)const noexcept(true){}
	};

	typedef cnVar::cPolymorphicClass<iHandle,Size,TAlignment> tHandle;

	template<class THandle>
	struct cCoHandle : iHandle
	{
		THandle Handle;
		cCoHandle(THandle h)noexcept(true):Handle(h){}

		virtual ~cCoHandle()noexcept(true){}
		virtual bool IsAvailable(void)const noexcept(true) override{
			return static_cast<bool>(Handle);
		}
		virtual void Resume(void)const noexcept(true) override{
			Handle.resume();
		}
		virtual void Finish(void)const noexcept(true) override{
			Handle.destroy();
		}
	};


	static void Reset(tHandle &Handle)noexcept(true){
		Handle.Replace<iHandle>();
	}
	template<class TAwaitCoHandle>
	static void Assign(tHandle &Handle,TAwaitCoHandle&& Await)noexcept(true){
		Handle.Replace< cCoHandle<TAwaitCoHandle> >(static_cast<TAwaitCoHandle&&>(Await));
	}
	static bool IsAvailable(const tHandle &Handle)noexcept(true){
		return Handle->IsAvailable();
	}
	static void Resume(const tHandle &Handle)noexcept(true){
		return Handle->Resume();
	}
	static void Finish(const tHandle &Handle)noexcept(true){
		return Handle->Finish();
	}
};
//---------------------------------------------------------------------------
}	// namespace cnAsync
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif  /* cnLibrary_CPPFEATURELEVEL >= 2 */
/*-------------------------------------------------------------------------*/
#endif

