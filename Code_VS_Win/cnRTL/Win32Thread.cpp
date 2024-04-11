#include "Win32Thread.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

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
