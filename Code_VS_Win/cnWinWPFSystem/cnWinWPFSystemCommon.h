/*- Unit: Windows System Interface ----------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-02-10                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include "cnWinSystemHeader.h"
#include <cnWinSystem\cnWinSystem.h>

#include <Sc009Win\cnWin.h>
#include <Sc009WinNT5\NT5.h>
#include <Sc009WinNT6\NT6.h>
#include <cnPLWinDLL\cnWinDLL_SysFunc.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
extern cnRTL::cnWinRTL::cDependentRegistration gSystemDependentRegistration;

extern cnRTL::cReferenceStaticImplementT<cnRTL::cnWinRTL::cWin32DebugTextOutput> DebugTextOutput;
//---------------------------------------------------------------------------
extern cnRTL::cInterfaceStaticImplementT<vSocketStreamProtocol> TCP;
extern cnRTL::cInterfaceStaticImplementT<vSocketDatagramProtocol> UDP;
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
