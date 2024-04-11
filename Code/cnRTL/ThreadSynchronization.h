/*- cnRTL - Thread Synchronization ----------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-06                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_ThreadSynchronization_H__
#define __cnLibrary_cnRTL_ThreadSynchronization_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
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
class cWaitObject
{
public:
	cWaitObject()noexcept(true);
	~cWaitObject()noexcept(true);

	void Acquire(void)noexcept(true);
	void Release(void)noexcept(true);

	bool Check(void)noexcept(true);
	void Wait(void)noexcept(true);
	bool WaitUntil(uInt64 SystemTime)noexcept(true);
private:
	cAtomicVar<uIntn> fRefCount;
	iThread *fWaitThread=nullptr;
	bool fWaitFlag;
};
//---------------------------------------------------------------------------
class cWaitReference : public iReference, public cWaitObject
{
public:
	cWaitReference()noexcept(true);
	~cWaitReference()noexcept(true);
	virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
	virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;
};
//---------------------------------------------------------------------------
class cThreadOneTimeNotifier
{
public:
	cThreadOneTimeNotifier()noexcept(true);
	~cThreadOneTimeNotifier()noexcept(true);

	void Reset(void)noexcept(true);
	void Wait(void)noexcept(true);
	void Notify(void)noexcept(true);
protected:
	iThread *fNotifyThread;
	bool fWaiting;
};
//---------------------------------------------------------------------------
class cAffixedVariableSet
{
public:
	cAffixedVariableSet()noexcept(true);
	~cAffixedVariableSet()noexcept(true);
	
	rPtr<iVariable> QueryAffixedVariable(const void *Token)noexcept(true);

private:

	class cVariable : public iVariable
	{
	public:

		cVariable *Parent;
		cVariable *Child[2];
		ufInt8 Branch;
		ufInt8 Color;


		cVariable(cAffixedVariableSet *Owner,const void *Token)noexcept(true);
		~cVariable()noexcept(true);

		bool MakeReference(void)noexcept(true);
		virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
		virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;

		virtual void cnLib_FUNC Clear(void)noexcept(true)override;
		virtual void* cnLib_FUNC Get(void)noexcept(true)override;
		virtual void cnLib_FUNC Set(iReference *Reference,void *Value)noexcept(true)override;

		struct cItemOrderOperator
		{
			static sfInt8 Compare(const cVariable &Item,const cVariable &Value)noexcept(true);
			static sfInt8 Compare(const cVariable &Item,const void *Token)noexcept(true);

		};
	protected:
		cAffixedVariableSet *fOwner;
		const void *fToken;
		rPtr<iReference> fReference;
		void *fValue;
		cAtomicVar<uIntn> fRefCount;
	};

	rPtr<iMutex> fSetMutex;
	cnRTL::cLinkItemSet<cVariable,cVariable::cItemOrderOperator> fSet;

	void VariableReleased(cVariable *Variable)noexcept(true);
};
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

