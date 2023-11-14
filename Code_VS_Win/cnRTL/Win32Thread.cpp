#include "Win32Thread.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

//---------------------------------------------------------------------------
void cSRWLockByCriticalSection::AcquireShared(void)
{
	return cCriticalSection::Acquire();
}
//---------------------------------------------------------------------------
bool cSRWLockByCriticalSection::TryAcquireShared(void)
{
	return cCriticalSection::TryAcquire();
}
//---------------------------------------------------------------------------
void cSRWLockByCriticalSection::ReleaseShared(void)
{
	return cCriticalSection::Release();
}
//---------------------------------------------------------------------------
