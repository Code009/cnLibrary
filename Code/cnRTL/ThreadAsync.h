/*- cnRTL - Async Threading -----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-06                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_ThreadAsync_H__
#define __cnLibrary_cnRTL_ThreadAsync_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/ObjectReference.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnRTL_CONSTVAR	constexpr
#define	cnRTL_CONSTEXPR_FUNC constexpr
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnRTL_CONSTVAR	const
#define	cnRTL_CONSTEXPR_FUNC
#endif	// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
class cAsyncExecutionThreadSwitch
{
public:
	cAsyncExecutionThreadSwitch(iPtr<iAsyncExecution> Execution)
		: fExecution(cnVar::MoveCast(Execution)){}
	~cAsyncExecutionThreadSwitch(){}

	bool await_ready(void)const noexcept(true){	return fExecution==nullptr;		}
	template<class TCoHandle>
	void await_suspend(TCoHandle&& CoHandle){
		fCoHandle=static_cast<TCoHandle&&>(CoHandle);
		fExecution->Execute(nullptr,&fExecuteProcedure);
	}
	void await_resume(void)noexcept(true){}
private:
	iPtr<iAsyncExecution> fExecution;
	vtCoHandle fCoHandle;

	class cExecuteProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)override{
			auto Host=cnMemory::GetObjectFromMemberPointer(this,&cAsyncExecutionThreadSwitch::fExecuteProcedure);
			Host->fCoHandle.resume();
		}
	}fExecuteProcedure;

};
//---------------------------------------------------------------------------
inline cAsyncExecutionThreadSwitch ExecutionThreadSwitch(iPtr<iAsyncExecution> Execution)
{
	return cAsyncExecutionThreadSwitch(cnVar::MoveCast(Execution));
}
//---------------------------------------------------------------------------
class bcExclusiveProcedure
{
protected:
	cExclusiveFlag fExclusiveFlag;
	void ProcedureThreadRunLoop(void);

	// HostProcedure
	// return true to continue
	virtual bool Procedure(void)=0;

	bcExclusiveProcedure();
	~bcExclusiveProcedure();
public:
	void Run(void);
};
//---------------------------------------------------------------------------
class bcAsyncExclusiveProcedure : public bcExclusiveProcedure
{
private:
	class cProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void);
	}fProcedure;
protected:
	bcAsyncExclusiveProcedure();
	~bcAsyncExclusiveProcedure();
public:
	operator iProcedure*();

	void RunAsync(iAsyncProcedure *AsyncProc);
	void RunAsyncAfter(iAsyncProcedure *AsyncProc,uIntn Count);

};
//---------------------------------------------------------------------------
class cCancellationFlag
{
public:
	void Reset(void);
	bool Complete(void);
	bool Cancel(void);

	bool IsCancelled(void)const;
private:
	enum eFlag{
		cfNormal,cfCompleted,cfCancelled
	};

	cAtomicVar<eFlag> fFlag=cfNormal;
};
//---------------------------------------------------------------------------
template<class TFunc,class TFuntion>
class cFunctorRefInterface;

template<class TFunc,class TRet,class...TArgs>
class cFunctorRefInterface<TFunc,TRet (TArgs...)> : public iReference, public iFunction<TRet (TArgs...)>
{
public:
	TFunc Func;
	cFunctorRefInterface(TFunc &&pf):Func(static_cast<TFunc&&>(pf)){}
	virtual TRet cnLib_FUNC Execute(TArgs...Args)override{
		return Func(static_cast<TArgs&&>(Args)...);
	}
};

template<class TFunc>
class cDispatchFunctor
{
public:
	rPtr< cFunctorRefInterface<TFunc,void (void)> > Function;
	iPtr<iDispatch> Dispatch;

	void operator() (void){
		Dispatch->ExecuteAsync(Function,Function);
	}
};
template<class TFunc>
inline cDispatchFunctor<TFunc> DispatchFunctor(iDispatch *Dispatch,TFunc&& Func){
	cDispatchFunctor<TFunc> Functor={rCreate< cFunctorRefInterface<TFunc,void(void)> >(static_cast<TFunc&&>(Func)),Dispatch};
	return Functor;
}

template<class TFunc>
inline void DispatchFunctionExecuteAsync(iDispatch *Dispatch,TFunc&& Func){
	auto Function=rCreate< cFunctorRefInterface<TFunc,void(void)> >(static_cast<TFunc&&>(Func));
	Dispatch->ExecuteAsync(Function,Function);
}
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#undef	cnRTL_CONSTEXPR_FUNC
#undef	cnRTL_CONSTVAR
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
