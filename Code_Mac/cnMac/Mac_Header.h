/*--- Mac - Common Header -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-09-08                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#include <cnSystem/cnMac.h>
#include <cnSystem/Socket.h>
#include <cnRTL/cnRTL.h>
#include <cnRTL/ocRTL.h>


/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnMac{
//---------------------------------------------------------------------------
using cnRTL::autoPtr;

using cnRTL::rClsRef;
using cnRTL::rClsPtr;
using cnRTL::rClsConstRef;
using cnRTL::rClsCreate;

using cnRTL::cfPtr;
#ifdef	__OBJC__
using cnRTL::ocPtr;
#endif
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
