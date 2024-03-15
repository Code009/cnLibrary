﻿/*- Windows - Threading ---------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-01-25                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnSystem\cnThread.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#define	override
#endif
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
//- FileTime ----------------------------------------------------------------
class cnLib_INTERFACE iFileTime : public iTimepoint
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual FILETIME cnLib_FUNC GetFileTime(void)=0;
	virtual LARGE_INTEGER cnLib_FUNC GetTimeValue(void)=0;
};
//- Waitable object ---------------------------------------------------------
class cnLib_INTERFACE iWaitableObject : public iInterface
{
public:

	virtual HANDLE cnLib_FUNC GetWaitHandle(void)=0;
};
//- thread ------------------------------------------------------------------
class cnLib_INTERFACE iWinThread : public iWaitableObject
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual DWORD cnLib_FUNC GetThreadID(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iThreadPoolHandleWaiter : public iReference
{
public:
	virtual void SetWait(HANDLE WaitHandle,PFILETIME Timeout)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWinThreadPool : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual rPtr<iThreadPoolHandleWaiter> CreateHandleWaiter(iReference *Reference,iFunction<void (DWORD)> *Callback)=0;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace cnWindows{
//---------------------------------------------------------------------------
extern const LARGE_INTEGER SystemTimeEpochAsFileTime;
extern iWinThreadPool *const DefaultThreadPool;
//---------------------------------------------------------------------------
iPtr<iThreadPool>			cnLib_FUNC CreateThreadPool(void);
//---------------------------------------------------------------------------
}	// namespace cnWindows
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#undef	override
#endif
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
