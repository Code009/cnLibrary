#pragma once

#define	WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cnWinCLI\cnWinCLI.h>
#include <cnRTL\cnRTLCore.h>
#include <cnRTL\RuntimeFunction.h>
#include <cnRTL\WinRTL.h>

#if 0

//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
// cnRTL/AsyncTask.h
class cAsyncTaskState
{
public:
	cAsyncTaskState()noexcept(true);
	~cAsyncTaskState()noexcept(true);

	void Reset(void)noexcept(true);
	void SetDone(void)noexcept(true);

	bool IsDone(void)const noexcept(true);
	bool SetNotify(iProcedure *NotifyProcedure)noexcept(true);

protected:

	cAtomicVar<iProcedure*> fNotifyProcedure;
	cAtomicVar<bool> fNotified;
};
//---------------------------------------------------------------------------
// cnRTL/AsyncTask.h
class cAsyncTask : public iAsyncTask
{
public:
	cAsyncTask()noexcept(true);
	~cAsyncTask()noexcept(true);

	virtual bool cnLib_FUNC IsDone(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)noexcept(true)override;

protected:
	cAsyncTaskState fTaskState;
};
//---------------------------------------------------------------------------
// cnRTL/AsyncTask.h
class cManualAsyncTask : public cAsyncTask
{
public:
	cManualAsyncTask()noexcept(true);
	~cManualAsyncTask()noexcept(true);

	void Reset(void)noexcept(true);
	void SetDone(void)noexcept(true);
};
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
// cnRTL/WinThread.h
class cWinExclusiveFlag
{
public:
	cWinExclusiveFlag(bool InitalRun=false)noexcept(true);

	bool Acquire(void)noexcept(true);
	bool Release(void)noexcept(true);

	void Continue(void)noexcept(true);

	bool IsRunning(void)const noexcept(true);
};
//---------------------------------------------------------------------------
// cnRTL/WinThread.h
class cCriticalSection 
{
public:
	cCriticalSection()noexcept(true);
	~cCriticalSection()noexcept(true);

	cCriticalSection(const cCriticalSection&)=delete;

	void Acquire(void)noexcept(true);
	bool TryAcquire(void)noexcept(true);
	void Release(void)noexcept(true);

protected:
	CRITICAL_SECTION fCriticalSection;
};
//---------------------------------------------------------------------------
// cnRTL/WinGDI.h
struct cGDIBitmapSection
{
	cGDIBitmapSection()noexcept(true);
	~cGDIBitmapSection()noexcept(true);
	
	void Clear(void)noexcept(true);
	void Setup(HDC DC,int NewWidth,int NewHeight)noexcept(true);

	HANDLE Section;
	HBITMAP Bitmap;
	int Width;
	int Height;
	uIntn Stride;
	uIntn BitmapSize;
	void *BitmapData;
};
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif // _MANAGED
#endif // 0
//---------------------------------------------------------------------------
