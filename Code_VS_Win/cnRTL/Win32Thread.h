﻿/*- cnRTL - Windows - Thread ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-07-29                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinCommon.h>
#include <cnRTL\WinThread.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
class cSRWLockByCriticalSection : public cCriticalSection
{
public:

	void AcquireShared(void);
	bool TryAcquireShared(void);
	void ReleaseShared(void);
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
