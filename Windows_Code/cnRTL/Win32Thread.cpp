#include "Win32Thread.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

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
