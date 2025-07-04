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
#include <cnTK/Interface.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 2
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnAsync{
//---------------------------------------------------------------------------
//template<class TRet>
//TAsyncTask
//{
//	bool IsDone(void)noexcept;
//	bool Await(iProcedure *CompletionNotify)noexcept;
//	TRet& Result(void)noexcept;
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
template<class TRet,class TDistinct>
class bcPromiseReturnValue
{
public:
	template<class T>
	void return_value(T cnLib_UREF Value)noexcept(noexcept(fReturnValue=(cnLib_UREFCAST(T)(Value))))
	{
		fReturnValue=cnLib_UREFCAST(T)(Value);
	}

	TRet& Result(void)noexcept(true){
		return fReturnValue;
	}

protected:
	TDistinct fReturnValue;
};
//---------------------------------------------------------------------------
template<class TRet>
class bcPromiseReturnValue<TRet,void>
{
public:
	void return_void()noexcept(true){}
	TRet Result(void)noexcept(true){}
};
//---------------------------------------------------------------------------
struct cPromisePointerOwnerOperator
{
	template<class T>
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
			iProcedure *CompletionNotify=Owner.fCompletionNotify.Barrier.Xchg(reinterpret_cast<iProcedure*>(&Owner));
			if(CompletionNotify!=nullptr){
				CompletionNotify->Execute();
			}

			Owner.OnFinish();
		}
		void await_resume(void)noexcept(true){}	// never resume from final suspend
		bcCoroutinePromise &Owner;
	};

	bcCoroutinePromise()noexcept(true)
		: fFinishFlag(false)
		, fCompletionNotify(nullptr)
	{}

	cAwaiterReadyAlways initial_suspend(void)noexcept(true){	return cAwaiterReadyAlways();	}
	cFinalSuspension final_suspend(void)noexcept(true){		return *this;	}
	void unhandled_exception(void)noexcept(true){}

	bool IsDone(void)const noexcept(true){	return TCoroutineHandleOperator::IsAvailable(fFinalHandle);	}

	bool Await(iProcedure *CompletionNotify)noexcept(true){
		iProcedure *PrevNotifyProc=nullptr;
		if(fCompletionNotify.Barrier.CmpXchg(PrevNotifyProc,CompletionNotify)){
			// registered
			return true;
		}
		// routine is completed already, or procedure has been set
		if(PrevNotifyProc==reinterpret_cast<iProcedure*>(this)){
			// routine is completed already
			fCompletionNotify.Release.Store(nullptr);
			return false;
		}
		else{
			// failed to set awaiter
			return false;
		}
	}

private:
	cAtomicVariable<typename cnVar::TSelect<0,iProcedure*,TCoroutineHandleOperator>::Type> fCompletionNotify;
	cAtomicVariable<typename cnVar::TSelect<0,bool,TCoroutineHandleOperator>::Type> fFinishFlag;
	typename TCoroutineHandleOperator::tHandle fFinalHandle;

	friend struct cnAsync::cPromisePointerOwnerOperator;
	void OnFinish(void)noexcept(true){
		// will be called once by owner and once by final suspension
		if(fFinishFlag.Free.Xchg(true)){
			// second call, destroy handle
			TCoroutineHandleOperator::Finish(fFinalHandle);
		}
		else{
			// still running, wait for owner or final suspension
		}
	}
};
//---------------------------------------------------------------------------
template<class TCoroutineHandleOperator,class TRet>
class cCoroutinePromise
	: public bcCoroutinePromise<TCoroutineHandleOperator>
	, public bcPromiseReturnValue<TRet,typename cnVar::TRemoveCV<TRet>::Type>
{
public:
	cnClass::cPtrOwner<cCoroutinePromise,cPromisePointerOwnerOperator> get_return_object()noexcept(true)
	{	return cnClass::cPtrOwner<cCoroutinePromise,cPromisePointerOwnerOperator>::TakeFromManual(this);	}

};
//---------------------------------------------------------------------------
template<class TCoroutineHandleOperator,class TRet>
class cCoroutine
{
public:
	typedef cCoroutinePromise<TCoroutineHandleOperator,TRet> promise_type;
	typedef cnClass::cPtrOwner<promise_type,cPromisePointerOwnerOperator> pPtr;
	
	cCoroutine()noexcept(true){}
	cCoroutine(pPtr cnLib_MOVEREF Promise)noexcept(true):fPromise(cnLib_UREFCAST(pPtr)(Promise)){}

	// copy
	cCoroutine(cCoroutine cnLib_MOVEREF Src)noexcept(true):fPromise(cnLib_UREFCAST(pPtr)(Src.fPromise)){}

	// move
	cCoroutine &operator=(cCoroutine cnLib_MOVEREF Src)noexcept(true){
		fPromise=cnLib_UREFCAST(pPtr)(Src.fPromise);
		return *this;
	}

	// promise reference
	operator pPtr cnLib_MOVEREF ()noexcept(true){	return cnLib_UREFCAST(pPtr)(fPromise);	}

	// check available
	operator promise_type*()const noexcept(true){	return fPromise;	}

	
	class cAwaiter : private iProcedure
	{
	public:
		cAwaiter(pPtr cnLib_MOVEREF Promise)noexcept(true):fPromise(cnLib_UREFCAST(pPtr)(Promise)){}
		cAwaiter(cAwaiter cnLib_MOVEREF Src)noexcept(true):fPromise(cnLib_UREFCAST(pPtr)(Src.fPromise)){}

		bool await_ready(void)const noexcept(true){
			if(fPromise==nullptr)
				return true;
			return fPromise->IsDone();
		}
		template<class TAwaitCoHandle>
		bool await_suspend(TAwaitCoHandle cnLib_UREF CoHandle){
			TCoroutineHandleOperator::Assign(fCoHandle,cnLib_UREFCAST(TAwaitCoHandle)(CoHandle));
			return fPromise->Await(this);
		}
		TRet await_resume(void)noexcept(true){
			return fPromise->Result();
		}


	protected:
		virtual void cnLib_FUNC Execute(void)noexcept(true)override{
			TCoroutineHandleOperator::Resume(fCoHandle);
		}

		typename TCoroutineHandleOperator::tHandle fCoHandle;
		pPtr fPromise;
	};


#if cnLibrary_CPPFEATURE_COROUTINE >= 201902L

	typename cAwaiter operator co_await()noexcept(true){
		return cnLib_UREFCAST(pPtr)(fPromise);
	}
#endif	// cnLibrary_CPPFEATURE_COROUTINE >= 201902L

protected:
	pPtr fPromise;
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
		, fFinishFlag(false)
		, fCompletionNotify(nullptr)
	{}
	~bcResumablePromise()noexcept(true){}

	cInitialSuspension initial_suspend(void)noexcept(true){	return *this;	}
	cFinalSuspension final_suspend(void)noexcept(true){	return *this;	}
	void unhandled_exception(void)noexcept(true){	}
	
	cYieldSuspension YieldSuspend(void)noexcept(true){	return *this;	}

	bool IsRunnable(void)noexcept(true){
		return fRunStateFlag.Free.Load()!=3;
	}

	bool Run(void)noexcept(true){
		if(fRunStateFlag.Acquire.CmpStore(0,1)==false){
			return false;
		}
		TCoroutineHandleOperator::Resume(fCoHandle);
		return true;
	}

	bool IsDone(void)noexcept(true){
		return fRunStateFlag.Free.Load()==2;
	}

	bool Await(iProcedure *CompletionNotify)noexcept(true){
		iProcedure *PrevNotifyProc=nullptr;
		if(fCompletionNotify.Barrier.CmpXchg(PrevNotifyProc,CompletionNotify)){
			// registered
			return true;
		}
		// routine is completed already, or procedure has been set
		if(PrevNotifyProc==reinterpret_cast<iProcedure*>(this)){
			// routine is completed already
			fCompletionNotify.Release.Store(nullptr);
			return false;
		}
		else{
			// failed to set awaiter
			return false;
		}
	}

private:
	typename TCoroutineHandleOperator::tHandle fCoHandle;
	cAtomicVariable<typename cnVar::TSelect<0,iProcedure*,TCoroutineHandleOperator>::Type> fCompletionNotify;
	// fRunStateFlag
	//	0=paused(runnable),1=running,2=ready(notify awaiter)
	cAtomicVariable<typename cnVar::TSelect<0,ufInt8,TCoroutineHandleOperator>::Type> fRunStateFlag;
	cAtomicVariable<typename cnVar::TSelect<0,bool,TCoroutineHandleOperator>::Type> fFinishFlag;

	void OnPhaseFinish(ufInt8 NextState)noexcept(true){
		fRunStateFlag.Release.Store(NextState);
		// notify awaiter
		iProcedure *CompletionNotify=fCompletionNotify.Barrier.Xchg(reinterpret_cast<iProcedure*>(this));
		if(CompletionNotify!=nullptr){
			fCompletionNotify.Release.Store(nullptr);
			CompletionNotify->Execute();
		}
	}

	friend struct cnAsync::cPromisePointerOwnerOperator;
	void OnFinish(void)noexcept(true){
		// will be called once by owner and once by final suspension
		if(fFinishFlag.Free.Xchg(true)){
			// second call, destroy handle
			TCoroutineHandleOperator::Finish(fCoHandle);
		}
		else{
			// still running, wait for owner or final suspension
		}
	}
};
//---------------------------------------------------------------------------
template<class TCoroutineHandleOperator,class TRet,class TDistinct>
class cResumablePromise
	: public bcResumablePromise<TCoroutineHandleOperator>
	, public bcPromiseReturnValue<TRet,TDistinct>
{
public:
	cnClass::cPtrOwner<cResumablePromise,cPromisePointerOwnerOperator> get_return_object()noexcept(true)
	{	return cnClass::cPtrOwner<cResumablePromise,cPromisePointerOwnerOperator>::TakeFromManual(this);	}

	template<class T>
	typename bcResumablePromise<TCoroutineHandleOperator>::cYieldSuspension yield_value(T cnLib_UREF Value)
		noexcept(noexcept(this->return_value(cnLib_UREFCAST(T)(Value))))
	{
		this->return_value(cnLib_UREFCAST(T)(Value));
		return this->YieldSuspend();
	}
};
//---------------------------------------------------------------------------
template<class TCoroutineHandleOperator,class TRet>
class cResumablePromise<TCoroutineHandleOperator,TRet,void>
	: public bcResumablePromise<TCoroutineHandleOperator>
	, public bcPromiseReturnValue<TRet,void>
{
public:
	cnClass::cPtrOwner<cResumablePromise,cPromisePointerOwnerOperator> get_return_object()noexcept(true)
	{	return cnClass::cPtrOwner<cResumablePromise,cPromisePointerOwnerOperator>::TakeFromManual(this);	}

	typename bcResumablePromise<TCoroutineHandleOperator>::cYieldSuspension yield_value(void)noexcept(true)
	{
		return this->YieldSuspend();
	}

	typename bcResumablePromise<TCoroutineHandleOperator>::cYieldSuspension yield_value(int)noexcept(true)
	{
		return this->YieldSuspend();
	}

};
//---------------------------------------------------------------------------
template<class TCoroutineHandleOperator,class TRet>
class cResumable
{
public:
	typedef cResumablePromise<TCoroutineHandleOperator,TRet,typename cnVar::TRemoveCV<TRet>::Type> promise_type;
	typedef cnClass::cPtrOwner<promise_type,cPromisePointerOwnerOperator> pPtr;

	cResumable()noexcept(true){}
	// construct by promise
	cResumable(pPtr cnLib_MOVEREF Promise)noexcept(true):fPromise(cnLib_UREFCAST(pPtr)(Promise)){}

	// copy
	cResumable(cResumable cnLib_MOVEREF Src)noexcept(true):fPromise(cnLib_UREFCAST(pPtr)(Src.fPromise)){}

	// move
	cResumable &operator=(cResumable cnLib_MOVEREF Src)noexcept(true){
		fPromise=cnLib_UREFCAST(pPtr)(Src.fPromise);
		return *this;
	}

	// promise reference
	operator pPtr cnLib_MOVEREF ()noexcept(true){	return cnLib_UREFCAST(pPtr)(fPromise);	}

	// check available
	operator promise_type*()const noexcept(true){	return fPromise;	}

	// check state

	operator bool()const noexcept(true){
		return fPromise->IsRunnable();
	}


	class cAwaiter : private iProcedure
	{
	public:
		cAwaiter(promise_type *p)noexcept(true):fPromise(p){}

		bool await_ready(void)const noexcept(true){
			if(fPromise==nullptr)
				return true;
			return fPromise->IsDone();
		}
		template<class TAwaitCoHandle>
		bool await_suspend(TAwaitCoHandle cnLib_UREF CoHandle){
			TCoroutineHandleOperator::Assign(fCoHandle,cnLib_UREFCAST(TAwaitCoHandle)(CoHandle));
			return fPromise->Await(this);
		}
		TRet await_resume(void)noexcept(true){
			return fPromise->Result();
		}

	protected:
		virtual void cnLib_FUNC Execute(void)noexcept(true)override{
			TCoroutineHandleOperator::Resume(fCoHandle);
		}

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
template<class TCoroutineHandleOperator,class TRet>
class cGenerator
{
public:
	cGenerator()noexcept(true){}
	~cGenerator()noexcept(true){}

	typedef cnClass::cPtrOwner<typename cResumable<TCoroutineHandleOperator,TRet>::promise_type,cPromisePointerOwnerOperator> pPtr;


	// construct by promise
	cGenerator(pPtr cnLib_MOVEREF Src)noexcept(true):fPromise(Src.TakePromise()){}

	// move construct
	cGenerator(cGenerator cnLib_MOVEREF Src)noexcept(true):fPromise(cnLib_UREFCAST(pPtr)(Src.fPromise)){}

	// move assignment
	cGenerator& operator=(cGenerator cnLib_MOVEREF Src)noexcept(true){
		fPromise=cnLib_UREFCAST(pPtr)(fPromise);
		return *this;
	}

	// check state

	operator typename cResumable<TCoroutineHandleOperator,TRet>::promise_type*()const noexcept(true){
		return fPromise;
	}

	bool operator () (iProcedure *CompletionNotify)const noexcept(true){
		if(fPromise->IsRunnable()==false)
			return false;

		if(fPromise->Run()==false)
			return false;

		if(fPromise->Await(CompletionNotify)){
			CompletionNotify->Execute();
		}
		return true;
	}

	TRet& operator *()noexcept(true){
		return fPromise->Result();
	}

	
	// promise reference
	operator pPtr cnLib_MOVEREF ()noexcept(true){	return cnLib_UREFCAST(pPtr)(fPromise);	}

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
	static void Assign(tHandle &Handle,TAwaitCoHandle cnLib_UREF Await)noexcept(true){
		Handle.template Replace< cCoHandle<TAwaitCoHandle> >(cnLib_UREFCAST(TAwaitCoHandle)(Await));
	}
	static bool IsAvailable(const tHandle &Handle)noexcept(true){
		return Handle->IsAvailable();
	}
	static void Resume(tHandle &Handle)noexcept(true){
		return Handle->Resume();
	}
	static void Finish(tHandle &Handle)noexcept(true){
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

