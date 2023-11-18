/*- cnNet - Common Definitions --------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2010-06-17                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnNet_NetCommon_H__
#define __cnLibrary_cnNet_NetCommon_H__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnSystem.h>
#include <cnRTL/String.h>
#include <cnRTL/ObjectReference.h>
#include <cnRTL/StreamProtocol.h>
#include <cnRTL/NetConnection.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnNet{
//---------------------------------------------------------------------------
	using cnRTL::cStringBuffer;
	using cnRTL::cString;

	using cnRTL::cMemoryBuffer;
	using cnRTL::cSeqList;
	using cnRTL::cSeqMap;
	using cnRTL::cLinkList;

	using cnRTL::iCreate;
	using cnRTL::rCreate;

	using cnRTL::aPtr;
	using cnRTL::aCreate;

	using cnRTL::cDualReference;
	
	using cnRTL::aCls;
	using cnRTL::aClsRef;
	using cnRTL::aClsConstRef;
	using cnRTL::aClsWeakRef;
	
	using cnRTL::cAtomicVar;
	using cnRTL::cAtomicStack;
	using cnRTL::cAtomicQueue;
	using cnRTL::cAtomicStackSO;
	using cnRTL::cAtomicQueueSO;

	using cnRTL::cTime;

	using cnRTL::iProtocolProcessor;
	using cnRTL::iProtocolProvider;
	
	using cnRTL::bcProtocolProcessor;
	using cnRTL::bcProtocolQueueProcessor;
	using cnRTL::bcConnectionQueueProcessor;
//---------------------------------------------------------------------------
constexpr uInt32 IPv4Addr(uInt8 a3,uInt8 a2,uInt8 a1,uInt8 a0)
{
	return ( a3<<24 ) | ( a2<<16 ) | ( a1<<8 ) | a0;
}
//---------------------------------------------------------------------------
constexpr uInt32 IPv4AddrR(uInt8 a3,uInt8 a2,uInt8 a1,uInt8 a0)
{
	return ( a0<<24 ) | ( a1<<16 ) | ( a2<<8 ) | a3;
}
//---------------------------------------------------------------------------
}	// namespace cnNet
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	/*#ifdef __cplusplus*/
#endif


