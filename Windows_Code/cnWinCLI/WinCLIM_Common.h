/*- cnWinCLI - Managed - Common -------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-04-14                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/

#include <cnWinCLI\cnWinCLI.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
class cGCRef : public cGCHandle
{
public:
	cGCRef()noexcept(true);
	cGCRef(const cGCRef &Src)noexcept(true)=delete;
	cGCRef(cGCRef &&Src)noexcept(true);
	cGCRef(cGCHandle &&Src)noexcept(true);
	~cGCRef()noexcept(true);

	bool mIsAllocated(void)const noexcept(true);
	void mClear(void)noexcept(true);
	void mFree(void)noexcept(true);

#if _MANAGED
	__forceinline System::Object^ Get(void){
		if(fHandle.IsAllocated)
			return fHandle.Target;
		return nullptr;
	}
	__forceinline void Set(System::Object ^Object){
		if(fHandle.IsAllocated){
			fHandle.Target=Object;
		}
		else{
			fHandle=System::Runtime::InteropServices::GCHandle::Alloc(Object);
		}
	}

#endif // _MANAGED

};
//---------------------------------------------------------------------------
class iCLIObject : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

	virtual const cGCHandle& cnLib_FUNC GetObjecHandle(void)noexcept(true)=0;

#if _MANAGED

	__forceinline System::Object^ Get(void)noexcept(true){
		return GetObjecHandle();
	}

	template<class T>
	__forceinline T Get(void)noexcept(true)
	{
		System::Object^ Target=GetObjecHandle();
		return dynamic_cast<T>(Target);
	}


#endif	// _MANAGED
};
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
private ref class rcNativeCaller_CLICommon sealed
{
public:
	rcNativeCaller_CLICommon(void *CPP);

	void iProcedure_Execute(void);
	void mbcCLICallback_Run(void);
private:
	void *CPP;
};
//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
class mbcActionDelegate
{
public:
#if _MANAGED
	System::Action^ GetAction(void)const noexcept(true);
#endif
protected:
#if _MANAGED
	mcGCHandle<System::Action> fAction;
#else
	cGCHandle fAction;
#endif // _MANAGED
};
//---------------------------------------------------------------------------
class mcNativeProcedureDelegate : public mbcActionDelegate
{
public:
	void mSetup(iProcedure *Procedure)noexcept(true);
	void mClear(void)noexcept(true);

protected:
#if _MANAGED
	friend rcNativeCaller_CLICommon;
#endif // _MANAGED
};
//---------------------------------------------------------------------------
class mbcCLICallback : public mbcActionDelegate
{
protected:
	void mSetup(void)noexcept(true);
	void mClear(void)noexcept(true);

#if _MANAGED
	friend rcNativeCaller_CLICommon;
#endif // _MANAGED

	virtual void CLIExecute(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
