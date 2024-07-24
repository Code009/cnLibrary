/*- cnTK - Coroutine ------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-05-06                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Coroutine_HPP__
#define	__cnLibrary_cnTK_Coroutine_HPP__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/Atomic.hpp>
#include <cnTK/Memory.hpp>
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

			// notify awaiter
			iCoroutinePromiseAwaiter *Awaiter=Owner.fAwaiter.Free.Xchg(reinterpret_cast<iCoroutinePromiseAwaiter*>(&Owner));
			if(Awaiter!=nullptr){
				Awaiter->NotifyCompletion();
			}

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
	void unhandled_exception(void)noexcept(true){	}

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
		cAwaiter(cAwaiter &Src)noexcept(true):fPromise(Src.fPromise){}
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

		void return_void()noexcept(true){}
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
		cAwaiter(cAwaiter &Src)noexcept(true):fPromise(Src.fPromise){}
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
			Token->OnFinish();
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
			TCoroutineHandleOperator::Assign(Owner.fCoHandle,cnLib_UREFCAST(TAwaitCoHandle)(CoHandle));

			Owner.OnPhaseFinish(3);
			Owner.OnFinish();
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
			TCoroutineHandleOperator::Assign(Owner.fCoHandle,cnLib_UREFCAST(TAwaitCoHandle)(CoHandle));

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

	bool Run(void)noexcept(true){
		if(fRunStateFlag.Acquire.CmpStore(0,1)==false){
			return false;
		}
		TCoroutineHandleOperator::Resume(fCoHandle);
		return true;
	}
	bool AwaitIsReady(void)noexcept(true){
		return fRunStateFlag.Free.Load()==2;
	}
	bool SetupAwait(iCoroutinePromiseAwaiter *Awaiter)noexcept(true){
		iCoroutinePromiseAwaiter *PrevAwaiter=fAwaiter.Acquire.Xchg(Awaiter);
		if(PrevAwaiter==reinterpret_cast<iCoroutinePromiseAwaiter*>(this)){
			// routine is completed already
			fAwaiter.Release.Store(nullptr);
			return false;
		}
		else{
			// registered
			return true;
		}
	}

	bool IsRunnable(void)noexcept(true){
		return fRunStateFlag.Free.Load()!=3;
	}

private:
	typename TCoroutineHandleOperator::tHandle fCoHandle;
	cAtomicVariable<typename cnVar::TSelect<0,iCoroutinePromiseAwaiter*,TCoroutineHandleOperator>::Type> fAwaiter;
	// fRunStateFlag
	//	0=paused(runnable),1=running,2=ready(notify awaiter)
	cAtomicVariable<typename cnVar::TSelect<0,ufInt8,TCoroutineHandleOperator>::Type> fRunStateFlag;
	cAtomicVariable<typename cnVar::TSelect<0,ufInt8,TCoroutineHandleOperator>::Type> fFinishFlag;

	void OnPhaseFinish(ufInt8 NextState)noexcept(true){
		fRunStateFlag.Release.Store(NextState);
		// notify awaiter
		iCoroutinePromiseAwaiter *Awaiter=fAwaiter.Acquire.Xchg(reinterpret_cast<iCoroutinePromiseAwaiter*>(this));
		if(Awaiter!=nullptr){
			fAwaiter.Release.Store(nullptr);
			Awaiter->NotifyCompletion();
		}
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
		typename bcResumablePromise<TCoroutineHandleOperator>::cYieldSuspension yield_value(T cnLib_UREF Value)
			noexcept(noexcept(fReturnValue=cnLib_UREFCAST(T)(Value)))
		{
			fReturnValue=cnLib_UREFCAST(T)(Value);
			return this->YieldSuspend();
		}

		template<class T>
		void return_value(T cnLib_UREF Value)
			noexcept(noexcept(fReturnValue=cnLib_UREFCAST(T)(Value)))
		{
			fReturnValue=cnLib_UREFCAST(T)(Value);
		}

		TRet fReturnValue;
	};
	typedef cnVar::cPtrOwner< cResumablePromiseOwnerTokenOperator<promise_type> > pPtr;

	cResumable()noexcept(true){}
	~cResumable()noexcept(true){}


	// construct by promise
	cResumable(pPtr &&p)noexcept(true):fPromise(static_cast<pPtr&&>(p)){}

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

	operator bool()const noexcept(true){
		return fPromise->IsRunnable();
	}


	class cAwaiter : public iCoroutinePromiseAwaiter
	{
	public:
		cAwaiter(promise_type *p)noexcept(true):fPromise(p){}

		bool await_ready(void)const noexcept(true){
			if(fPromise==nullptr)
				return true;
			return fPromise->AwaitIsReady();
		}
		template<class TAwaitCoHandle>
		bool await_suspend(TAwaitCoHandle cnLib_UREF CoHandle){
			TCoroutineHandleOperator::Assign(fCoHandle,cnLib_UREFCAST(TAwaitCoHandle)(CoHandle));
			return fPromise->SetupAwait(this);
		}
		TRet await_resume(void)noexcept(true){	return static_cast<TRet&&>(fPromise->fReturnValue);	}

		virtual void NotifyCompletion(void)noexcept(true)override{
			TCoroutineHandleOperator::Resume(fCoHandle);
		}

	protected:
		typename TCoroutineHandleOperator::tHandle fCoHandle;
		promise_type *fPromise;
	};

	// run function

	cAwaiter operator () (void)const noexcept(true){
		if(fPromise->Run()==false)
			return nullptr;
		return fPromise.Pointer();
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
		cnVar::cPtrOwner< cResumablePromiseOwnerTokenOperator<promise_type> > get_return_object()noexcept(true)
		{	return cnVar::cPtrOwner< cResumablePromiseOwnerTokenOperator<promise_type> >::TakeFromManual(this);	}

		typename bcResumablePromise<TCoroutineHandleOperator>::cYieldSuspension yield_value(void)noexcept(true){	return this->YieldSuspend();		}
		typename bcResumablePromise<TCoroutineHandleOperator>::cYieldSuspension yield_value(int)noexcept(true){		return this->YieldSuspend();		}

		void return_void(void)noexcept(true){}
	};
	typedef cnVar::cPtrOwner< cResumablePromiseOwnerTokenOperator<promise_type> > pPtr;

	cResumable()noexcept(true){}
	~cResumable()noexcept(true){}


	// construct by promise
	cResumable(pPtr &&p)noexcept(true):fPromise(static_cast<pPtr&&>(p)){}

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

	operator bool()const noexcept(true){
		return fPromise->IsRunnable();
	}


	class cAwaiter : public iCoroutinePromiseAwaiter
	{
	public:
		cAwaiter(promise_type *p)noexcept(true):fPromise(p){}

		bool await_ready(void)const noexcept(true){
			if(fPromise==nullptr)
				return true;
			return fPromise->AwaitIsReady();
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

	cAwaiter operator () (void)const noexcept(true){
		if(fPromise->Run()==false)
			return nullptr;
		return fPromise.Pointer();
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
		Handle.template Replace<iHandle>();
	}
	template<class TAwaitCoHandle>
	static void Assign(tHandle &Handle,TAwaitCoHandle&& Await)noexcept(true){
		Handle.template Replace< cCoHandle<TAwaitCoHandle> >(static_cast<TAwaitCoHandle&&>(Await));
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

