#include "Win32Thread.h"

#include "Win32Interlocked.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace Win32RTLCore;

//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnRTL{
namespace cnWinRTL{
//---------------------------------------------------------------------------
static LARGE_INTEGER gCachedPerformanceFrequency;
//---------------------------------------------------------------------------
}	// namespace cnLibrary
}	// namespace cnRTL
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
uInt64 cnWinRTL::QueryPerformanceFrequencyCached(void)noexcept(true)
{
	if(gCachedPerformanceFrequency.QuadPart==0){
		::QueryPerformanceFrequency(&gCachedPerformanceFrequency);
	}
	return gCachedPerformanceFrequency.QuadPart;
//	if(Value!=0){
//		return Value;
//	}
//	::QueryPerformanceFrequency(&gCachedPerformanceFrequency);
//	return gCachedPerformanceFrequency.QuadPart;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCriticalSection::cCriticalSection()noexcept(true)
{
	::InitializeCriticalSection(&fCriticalSection);
}
cCriticalSection::~cCriticalSection()noexcept(true)
{
	::DeleteCriticalSection(&fCriticalSection);
}
//---------------------------------------------------------------------------
void cCriticalSection::Acquire(void)noexcept(true)
{
	::EnterCriticalSection(&fCriticalSection);
}
//---------------------------------------------------------------------------
bool cCriticalSection::TryAcquire(void)noexcept(true)
{
	return ::TryEnterCriticalSection(&fCriticalSection)!=FALSE;
}
//---------------------------------------------------------------------------
void cCriticalSection::Release(void)noexcept(true)
{
	::LeaveCriticalSection(&fCriticalSection);
}
//---------------------------------------------------------------------------
void cSRWLockByCriticalSection::AcquireShared(void)noexcept(true)
{
	return cCriticalSection::Acquire();
}
//---------------------------------------------------------------------------
bool cSRWLockByCriticalSection::TryAcquireShared(void)noexcept(true)
{
	return cCriticalSection::TryAcquire();
}
//---------------------------------------------------------------------------
void cSRWLockByCriticalSection::ReleaseShared(void)noexcept(true)
{
	return cCriticalSection::Release();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cSingleNotification::Setup(void)noexcept(true)
{
	HANDLE ProcessHandle=::GetCurrentProcess();
	::DuplicateHandle(ProcessHandle,::GetCurrentThread(),ProcessHandle,&fNotifyThreadHandle,0,FALSE,DUPLICATE_SAME_ACCESS);
	fNotifyFlag=2;
	fAPCFlag=0;
}
//---------------------------------------------------------------------------
void cSingleNotification::Clear(void)noexcept(true)
{
	if(fNotifyFlag==1){
		// clear pending APC
		WaitMS(INFINITE);
	}
	::CloseHandle(fNotifyThreadHandle);
}
//---------------------------------------------------------------------------
void cSingleNotification::Wait(void)noexcept(true)
{
	WaitMS(INFINITE);
}
//---------------------------------------------------------------------------
bool cSingleNotification::WaitFor(ufInt64 Duration)noexcept(true)
{
	return WaitMS(static_cast<DWORD>(Duration/1000000));
}
//---------------------------------------------------------------------------
void cSingleNotification::Notify(void)noexcept(true)
{
	if(Interlocked::CmpXchg(fNotifyFlag,1,2)==2){
		::QueueUserAPC(WaitNotifyAPC,fNotifyThreadHandle,reinterpret_cast<ULONG_PTR>(this));
	}
}
//---------------------------------------------------------------------------
VOID CALLBACK cSingleNotification::WaitNotifyAPC(ULONG_PTR Param)noexcept(true)
{
	auto This=reinterpret_cast<cSingleNotification*>(Param);
	This->fAPCFlag=1;
	This->fNotifyFlag=0;
}
//---------------------------------------------------------------------------
bool cSingleNotification::WaitMS(DWORD Milliseconds)noexcept(true)
{
	while(fAPCFlag==0){
		DWORD WaitRet=SleepEx(Milliseconds,TRUE);
		if(WaitRet==0){
			// timeout
			return false;
		}
	}
	return true;
}
//---------------------------------------------------------------------------
