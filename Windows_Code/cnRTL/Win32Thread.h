/*- cnRTL - Windows - Win32 Thread ----------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-07-29                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#include <cnRTL\WinSDKHeader.h>

/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
uInt64 QueryPerformanceFrequencyCached(void)noexcept(true);
//---------------------------------------------------------------------------
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
class cSRWLockByCriticalSection : public cCriticalSection
{
public:

	void AcquireShared(void)noexcept(true);
	bool TryAcquireShared(void)noexcept(true);
	void ReleaseShared(void)noexcept(true);
};
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
namespace Win32RTLCore{
//---------------------------------------------------------------------------
class cSingleNotification
{
public:
	void Setup(void)noexcept(true);
	void Clear(void)noexcept(true);
	void Wait(void)noexcept(true);
	bool WaitFor(ufInt64 Duration)noexcept(true);
	//bool WaitUntil(ufInt64 Duration)noexcept(true);
	void Notify(void)noexcept(true);

private:
	HANDLE fNotifyThreadHandle;
	volatile CHAR fNotifyFlag;
	CHAR fAPCFlag;
	static VOID CALLBACK WaitNotifyAPC(ULONG_PTR Param)noexcept(true);

	bool WaitMS(DWORD Milliseconds)noexcept(true);
};
//---------------------------------------------------------------------------
}	// namespace Win32RTLCore
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
