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
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
