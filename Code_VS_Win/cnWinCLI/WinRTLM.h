#pragma once

#define	WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cnWinCLI\cnWinCLI.h>
#include <cnRTL\cnRTLCore.h>
#include <cnRTL\RuntimeFunction.h>


//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
namespace cnLibrary{

struct TKRuntime::ThreadNotification
{
	CRITICAL_SECTION CS;
	CONDITION_VARIABLE CondVar;

	ThreadNotification(){
		::InitializeCriticalSection(&CS);
		::InitializeConditionVariable(&CondVar);
	}
	~ThreadNotification(){
		::DeleteCriticalSection(&CS);
	}

	void Start(void){
		Notified=false;
	}
	void Finish(void){
	}
	void Wait(void){
		if(Notified)
			return;

		::EnterCriticalSection(&CS);
		::SleepConditionVariableCS(&CondVar,&CS,INFINITE);
		::LeaveCriticalSection(&CS);
	}
	void Notify(void){
		Notified=true;
		::WakeConditionVariable(&CondVar);
	}

	bool Notified;
};
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
// cnRTL/AsyncTask.h
class cAsyncTaskState
{
public:
	cAsyncTaskState();
	~cAsyncTaskState();

	void Reset(void);
	void SetDone(void);

	bool IsDone(void)const;
	bool SetNotify(iProcedure *NotifyProcedure);

protected:

	cAtomicVar<iProcedure*> fNotifyProcedure;
	cAtomicVar<bool> fNotified;
};
//---------------------------------------------------------------------------
// cnRTL/AsyncTask.h
class cAsyncTask : public iAsyncTask
{
public:
	cAsyncTask();
	~cAsyncTask();

	virtual bool cnLib_FUNC IsDone(void)override;
	virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;

protected:
	cAsyncTaskState fTaskState;
};
//---------------------------------------------------------------------------
// cnRTL/AsyncTask.h
class cManualAsyncTask : public cAsyncTask
{
public:
	cManualAsyncTask();
	~cManualAsyncTask();

	void Reset(void);
	void SetDone(void);
};
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
// cnRTL/WinThread.h
class cWinExclusiveFlag
{
public:
	cWinExclusiveFlag(bool InitalRun=false);

	bool Acquire(void);
	bool Release(void);

	void Continue(void);

	bool IsRunning(void)const;
};
//---------------------------------------------------------------------------
// cnRTL/WinThread.h
class cCriticalSection 
{
public:
	cCriticalSection();
	~cCriticalSection();

	cCriticalSection(const cCriticalSection&)=delete;

	void Acquire(void);
	bool TryAcquire(void);
	void Release(void);

protected:
	CRITICAL_SECTION fCriticalSection;
};
//---------------------------------------------------------------------------
// cnRTL/WinGDI.h
struct cGDIBitmapSection
{
	cGDIBitmapSection();
	~cGDIBitmapSection();
	
	void Clear(void);
	void Setup(HDC DC,int NewWidth,int NewHeight);

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
//---------------------------------------------------------------------------
