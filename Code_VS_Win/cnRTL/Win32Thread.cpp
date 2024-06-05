#include "Win32Thread.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCriticalSection::cCriticalSection()noexcept
{
	::InitializeCriticalSection(&fCriticalSection);
}
cCriticalSection::~cCriticalSection()noexcept
{
	::DeleteCriticalSection(&fCriticalSection);
}
//---------------------------------------------------------------------------
void cCriticalSection::Acquire(void)noexcept
{
	::EnterCriticalSection(&fCriticalSection);
}
//---------------------------------------------------------------------------
bool cCriticalSection::TryAcquire(void)noexcept
{
	return ::TryEnterCriticalSection(&fCriticalSection)!=FALSE;
}
//---------------------------------------------------------------------------
void cCriticalSection::Release(void)noexcept
{
	::LeaveCriticalSection(&fCriticalSection);
}
//---------------------------------------------------------------------------
void cSRWLockByCriticalSection::AcquireShared(void)noexcept
{
	return cCriticalSection::Acquire();
}
//---------------------------------------------------------------------------
bool cSRWLockByCriticalSection::TryAcquireShared(void)noexcept
{
	return cCriticalSection::TryAcquire();
}
//---------------------------------------------------------------------------
void cSRWLockByCriticalSection::ReleaseShared(void)noexcept
{
	return cCriticalSection::Release();
}
//---------------------------------------------------------------------------
