/*- cnTK - Coroutine ------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-05-06                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Coroutine_H__
#define	__cnLibrary_cnTK_Coroutine_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.h>
#include <cnTK/Atomic.h>
#include <cnTK/Variant.h>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 2
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnAsync{
//---------------------------------------------------------------------------
//TResumable
//{
//	operator bool()const noexcept;
//	bool operator () (void);
//	bool Run(void) (void);
//};
//---------------------------------------------------------------------------
//TGenerator<TRet> : TResumable
//{
//	const TRet& Value(void)const noexcept;
//	const TRet& operator *()const noexcept;
//};
//---------------------------------------------------------------------------
class cCoHandleVariant
{
public:
	cCoHandleVariant()noexcept(true){
		fOperator.ConstructAs<cVoidOperator>();
	}

	cCoHandleVariant(tNullptr)noexcept(true){
		fOperator.ConstructAs<cVoidOperator>();
	}
	~cCoHandleVariant()noexcept(true){
		fOperator->~cVoidOperator();
	}

	// construct from coroutine handle
	template<class TCoHandle>
	cCoHandleVariant(const TCoHandle &Handle)noexcept(cnLib_NOEXCEPTEXPR(fOperator.ConstructAs<cCoOperator<TCoHandle> >(Handle)))
	{	return fOperator.ConstructAs<cCoOperator<TCoHandle> >(Handle);	}

	// assign as null routine
	cCoHandleVariant& operator=(tNullptr)noexcept(true){
		fOperator->~cVoidOperator();
		fOperator.ConstructAs<cVoidOperator>();
		return *this;
	}

	// assign coroutine handle
	template<class TCoHandle>
	cCoHandleVariant& operator=(const TCoHandle &Handle)noexcept(cnLib_NOEXCEPTEXPR(TCoHandle(Handle))){
		fOperator->~cVoidOperator();
		fOperator.ConstructAs<cCoOperator<TCoHandle> >(Handle);
		return *this;
	}

	// operator ==
	//	compare with nullptr
	bool operator ==(tNullptr)noexcept(true){
		return !fOperator->Exists();
	}
	// operator !=
	//	compare with nullptr
	bool operator !=(tNullptr)noexcept(true){
		return fOperator->Exists();
	}

	// operator bool
	// return if routine exists
	operator bool ()const noexcept(true){
		return fOperator->Exists();
	}

	bool done(void)const noexcept(true){
		return fOperator->Done();
	}

	void resume(void)noexcept(true){
		fOperator->Resume();
	}

	void destroy(void)noexcept(true){
		fOperator->Destroy();
	}

private:
	// cVoidOperator
	//	act as null routine
	struct cVoidOperator
	{
		void *Pointer;
		virtual ~cVoidOperator()noexcept(true){}
		virtual bool Done(void)const noexcept(true){	return true;	}
		virtual void Resume(void)noexcept(true){}
		virtual void Destroy(void)noexcept(true){}
		virtual bool Exists(void)const noexcept(true){	return false;	}
	};

	// cCoOperator
	//	operate coroutine handle
	template<class THandler>
	struct cCoOperator
	{
		THandler Handler;
		cCoOperator(THandler h)noexcept(true):Handler(h){}

		virtual ~cCoOperator()noexcept(true){}
		virtual bool Done(void)const noexcept(true){
			return Handler.done();
		}
		virtual void Resume(void)noexcept(true){
			Handler.resume();
		}
		virtual void Destroy(void)noexcept(true){
			Handler.destroy();
		}
		virtual bool Exists(void)const noexcept(true){
			return static_cast<bool>(Handler);
		}
	};
	cnVar::cPolymorphicInterface<cVoidOperator,sizeof(void*)*2,void*> fOperator;

};
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
class cFreePromise
{
public:
	// promise
	cAwaiterReadyAlways initial_suspend(void)noexcept(true){	return {};	}
	cAwaiterReadyAlways final_suspend(void)noexcept(true){	return {};	}
	void unhandled_exception(void)noexcept(true){}
};
//---------------------------------------------------------------------------
class cFreeCoroutine
{
public:
	class promise_type : public cFreePromise
	{
	public:
		cFreeCoroutine get_return_object(void)noexcept(true){	return {};	}
		void return_void(void)noexcept(true){}
	};
};
//---------------------------------------------------------------------------
// TCoHandle
//{
//
//	bool operator ==(tNullptr);
//	bool operator !=(tNullptr);
//	operator bool();
//	bool done(void);
//	void resume(void);
//	void destroy(void);
//};
//---------------------------------------------------------------------------
template<class TRet,class TCoHandle>
class cResumable
{
public:
	class promise_type
	{
	public:
		promise_type* get_return_object()noexcept(true){	return this;	}

		// promise

		struct cInitialAwaiter
		{
			promise_type *Owner;
			bool await_ready(void)const noexcept(true){	return false;	}
			template<class TAwaitCoHandle>
			void await_suspend(TAwaitCoHandle cnLib_UREF CoHandle)noexcept(true){
				Owner->fCoHandle=cnLib_UREFCAST(TAwaitCoHandle)(CoHandle);
			}
			void await_resume(void)noexcept(true){}
		};

		cInitialAwaiter initial_suspend(void)noexcept(true){	return {this};		}
		promise_type& final_suspend(void)noexcept(true){		return *this;		}
		void unhandled_exception(void)noexcept(true){}

		template<class T>
		promise_type& yield_value(T cnLib_UREF Value){
			fReturnValue=cnLib_UREFCAST(T)(Value);
			return *this;
		}

		template<class T>
		void return_value(T cnLib_UREF Value){
			fReturnValue=cnLib_UREFCAST(T)(Value);
		}


		// await for co_yield and final_suspend

		bool await_ready(void)const noexcept(true){	return false;	}
		template<class TAwaitCoHandle>
		void await_suspend(TAwaitCoHandle cnLib_UREF)noexcept(true){}
		void await_resume(void)noexcept(true){}

	private:
		friend class cnAsync::cResumable<TRet,TCoHandle>;

		TCoHandle fCoHandle;
		TRet fReturnValue;

	};

	cResumable()noexcept(true){}
	~cResumable()noexcept(true){
		if(fPromise!=nullptr){
			fPromise->fCoHandle.destroy();
		}
	}


	// construct by promise
	cResumable(promise_type *p)noexcept(true):fPromise(p){}

	cResumable(const cResumable&)=delete;
	cResumable &operator=(const cResumable&)=delete;

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// move construct
	cResumable(cResumable &&Src)noexcept(true):fPromise(Src.fPromise){
		Src.fPromise=nullptr;
	}

	// move assignment
	cResumable &operator=(cResumable &&Src)noexcept(true){
		fPromise=Src.fPromise;
		Src.fPromise=nullptr;
		return *this;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// check state

	operator promise_type*()const noexcept(true){
		return fPromise;
	}

	// run function

	bool Run(void)const noexcept(true){
		if(fPromise->fCoHandle.done())
			return false;
		fPromise->fCoHandle.resume();
		return true;
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
	promise_type *fPromise;
};
//---------------------------------------------------------------------------
struct cResumableYieldVoid{
	cResumableYieldVoid()noexcept(true){}
	cResumableYieldVoid(int)noexcept(true){}
};
//---------------------------------------------------------------------------
template<class TCoHandle>
class cResumable<void,TCoHandle>
{
public:

	class promise_type
	{
	public:
		promise_type* get_return_object()noexcept(true){	return this;	}

		// promise

		struct cInitialAwaiter
		{
			promise_type *Owner;
			bool await_ready(void)const noexcept(true){	return false;	}
			template<class TAwaitCoHandle>
			void await_suspend(TAwaitCoHandle cnLib_UREF CoHandle)noexcept(true){
				Owner->fCoHandle=cnLib_UREFCAST(TAwaitCoHandle)(CoHandle);
			}
			void await_resume(void)noexcept(true){}
		};

		cInitialAwaiter initial_suspend(void)noexcept(true){	return {this};		}
		promise_type& final_suspend(void)noexcept(true){		return *this;		}
		void unhandled_exception(void)noexcept(true){}

		promise_type& yield_value(void)noexcept(true){		return *this;		}

		promise_type& yield_value(cResumableYieldVoid)noexcept(true){		return *this;		}

		void return_void(void)noexcept(true){}

		// await for co_yield and final_suspend

		bool await_ready(void)const noexcept(true){	return false;	}
		template<class TAwaitCoHandle>
		void await_suspend(TAwaitCoHandle cnLib_UREF)noexcept(true){}
		void await_resume(void)noexcept(true){}

	private:
		friend class cnAsync::cResumable<void,TCoHandle>;

		TCoHandle fCoHandle;
	};

	cResumable()noexcept(true){}
	~cResumable()noexcept(true){
		if(fPromise!=nullptr){
			fPromise->fCoHandle.destroy();
		}
	}

	// construct by promise
	cResumable(promise_type *p)noexcept(true):fPromise(p){}

	cResumable(const cResumable&)=delete;
	cResumable &operator=(const cResumable&)=delete;

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	// move construct
	cResumable(cResumable &&Src)noexcept(true):fPromise(Src.fPromise){
		Src.fPromise=nullptr;
	}

	// move assignment
	cResumable &operator=(cResumable &&Src)noexcept(true){
		fPromise=Src.fPromise;
		Src.fPromise=nullptr;
		return *this;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// check state

	operator promise_type*()const noexcept(true){
		return fPromise;
	}

	// run function

	bool Run(void)const noexcept(true){
		if(fPromise->fCoHandle.done())
			return false;
		fPromise->fCoHandle.resume();
		return true;
	}
	bool operator () (void)const noexcept(true){
		return Run();
	}

	//operator cResumableYieldVoid ()const noexcept(true){
	//	return {};
	//}

private:
	promise_type *fPromise;
};
//---------------------------------------------------------------------------
template<class T>
struct cCoroutinePromiseOwnerTokenOperator : cnVar::bcPointerOwnerTokenOperator<T*>
{
	static void Release(T* const &Token)noexcept(true){
		if(Token!=nullptr)
			Token->OwnerFinish();
	}
};
//---------------------------------------------------------------------------
template<class TCoHandle>
class bcCoroutinePromise
{
public:
	class bcAwaiter
	{
	public:
		void PromiseDone(void)noexcept(true){
			fAwaitHandle.resume();
		}
	protected:
		TCoHandle fAwaitHandle;

		bool SetupWait(bcCoroutinePromise *Promise)noexcept(true){
			return Promise->SetupWait(this);
		}
	};

	bcCoroutinePromise()noexcept(true)
		: fFinishFlag(0)
		, fAwaiter(nullptr)
	{}
	~bcCoroutinePromise()noexcept(true){}

	cAwaiterReadyAlways initial_suspend(void)noexcept(true){	return {};	}
	bcCoroutinePromise& final_suspend(void)noexcept(true){		return *this;	}
	void unhandled_exception(void)noexcept(true){	OnReturn();	}

	// final awaiter

	bool await_ready(void)const noexcept(true){
		return fFinalReady;
	}
	template<class THandle>
	void await_suspend(THandle cnLib_UREF CoHandle)noexcept(true){
		fFinalHandle=cnLib_UREFCAST(THandle)(CoHandle);
		// coordinate with OwnerFinish
		if(fFinishFlag.Free.Xchg(1)){
			// OwnerFinish passed
			CoHandle.destroy();
		}
		else{
			// let OwnerFinish destroy FinalHandle
		}
	}
	void await_resume(void)noexcept(true){}	// never resume from final suspend

	void OwnerFinish(void)noexcept(true){
		// coordinate with final suspend
		if(fFinishFlag.Free.Xchg(1)){
			// suspended, destroy handle
			fFinalHandle.destroy();
		}
		else{
			// still running wait for it suspend then destroy
		}
	}

protected:
	void OnReturn(void)noexcept(true){
		// notify awaiter
		bcAwaiter *Awaiter=fAwaiter.Acquire.Xchg(reinterpret_cast<bcAwaiter*>(1));
		if(Awaiter!=nullptr){
			Awaiter->PromiseDone();

			// freed
			fFinalReady=true;
		}
		else{
			// wait for OwnerFinish
			fFinalReady=false;
		}
	}
private:
	cnAsync::cAtomicVariable<typename cnVar::TSelect<0,bcAwaiter*,TCoHandle>::Type> fAwaiter;
	cnAsync::cAtomicVariable<typename cnVar::TSelect<0,ufInt8,TCoHandle>::Type> fFinishFlag;
	bool fFinalReady;
	TCoHandle fFinalHandle;

	bool SetupWait(bcAwaiter *Awaiter)noexcept(true){
		bcAwaiter *PrevAwaiter=fAwaiter.Free.Xchg(Awaiter);
		if(PrevAwaiter==reinterpret_cast<bcAwaiter*>(1)){
			// returned
			return false;
		}
		else{
			// registered
			return true;
		}
	}

};
//---------------------------------------------------------------------------
template<class TRet,class TCoHandle>
class cCoroutinePromise : public bcCoroutinePromise<TCoHandle>
{
public:
	typedef cnVar::cPtrOwner< cCoroutinePromiseOwnerTokenOperator<cCoroutinePromise> > pPtr;
	pPtr get_return_object()noexcept(true){	return pPtr::TakeFromManual(this);	}

	class cAwaiter : public bcCoroutinePromise<TCoHandle>::bcAwaiter
	{
	public:
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
		cAwaiter(pPtr &&p)noexcept(true):fPromise(static_cast<pPtr&&>(p)){}
#else	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L
		cAwaiter(pPtr &p):fPromise(p){}
#endif	//cnLibrary_CPPFEATURE_RVALUE_REFERENCES

		~cAwaiter()noexcept(true){}


		bool await_ready(void)const noexcept(true){
			return fPromise==nullptr;
		}
		template<class TAwaitCoHandle>
		bool await_suspend(TAwaitCoHandle cnLib_UREF CoHandle){
			this->fAwaitHandle=cnLib_UREFCAST(TAwaitCoHandle)(CoHandle);
			return this->SetupWait(fPromise);
		}
		TRet& await_resume(void)noexcept(true){
			return fPromise->fReturnValue;
		}
	protected:
		pPtr fPromise;
	};

	template<class T>
	void return_value(T cnLib_UREF Value)noexcept(true){
		fReturnValue=cnLib_UREFCAST(T)(Value);
		this->OnReturn();
	}
private:
	TRet fReturnValue;
};
//---------------------------------------------------------------------------
template<class TCoHandle>
class cCoroutinePromise<void,TCoHandle> : public bcCoroutinePromise<TCoHandle>
{
public:
	typedef cnVar::cPtrOwner< cCoroutinePromiseOwnerTokenOperator<cCoroutinePromise> > pPtr;
	pPtr get_return_object()noexcept(true){	return pPtr::TakeFromManual(this);	}

	class cAwaiter : public bcCoroutinePromise<TCoHandle>::bcAwaiter
	{
	public:
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
		cAwaiter(pPtr &&p)noexcept(true):fPromise(static_cast<pPtr&&>(p)){}
#else	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L
		cAwaiter(pPtr &p)noexcept(true):fPromise(p){}
#endif	//cnLibrary_CPPFEATURE_RVALUE_REFERENCES
		~cAwaiter()noexcept(true){}


		bool await_ready(void)const noexcept(true){
			return fPromise==nullptr;
		}
		template<class TAwaitCoHandle>
		bool await_suspend(TAwaitCoHandle cnLib_UREF CoHandle){
			this->fAwaitHandle=cnLib_UREFCAST(TAwaitCoHandle)(CoHandle);
			return this->SetupWait(fPromise);
		}
		void await_resume(void)noexcept(true){}
	protected:
		pPtr fPromise;
	};

	void return_void(void)noexcept(true){
		this->OnReturn();
	}
};
//---------------------------------------------------------------------------
template<class TRet,class TCoHandle>
class cCoroutine
{
public:
	typedef cCoroutinePromise<TRet,TCoHandle> promise_type;
	typedef typename promise_type::pPtr pPtr;

#if cnLibrary_CPPFEATURE_COROUTINE >= 201902L

	typename promise_type::cAwaiter operator co_await()noexcept(true){
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
	cCoroutine(cCoroutine &&Src)noexcept(true):fPromise(static_cast<pPtr&&>(Src.fPromise)){
		Src.fPromise=nullptr;
	}

	// move
	cCoroutine &operator=(cCoroutine &&Src)noexcept(true){
		fPromise=static_cast<pPtr&&>(Src.fPromise);
		if(fPromise!=nullptr){
			Src.fPromise=nullptr;
		}
		return *this;
	}
	cCoroutine(const cCoroutine&)=delete;
	cCoroutine &operator=(const cCoroutine&)=delete;
#else
	cCoroutine(cCoroutine &Src)noexcept(true):fPromise(Src.fPromise){
	
	cCoroutine &operator=(cCoroutine &Src)noexcept(true){
		fPromise=Src.fPromise;
		if(fPromise!=nullptr){
			Src.fPromise=nullptr;
		}
		return *this;
	}

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	// check state

	operator promise_type*()const noexcept(true){	return fPromise;	}

protected:
	pPtr fPromise;
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

