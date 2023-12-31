﻿/*- Windows - Network System ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-01-25                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnSystem\cnNetwork.h>
#include <cnSystem\Socket.h>


#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWinSocketMulticastAddress : public iSocketAddress
{
public:
	cnLib_INTERFACE_DEFINE(iWinSocketMulticastAddress,iSocketAddress)

	virtual uIntn cnLib_FUNC AddMember(SOCKET Socket)=0;
	virtual uIntn cnLib_FUNC DropMember(SOCKET Socket)=0;
};
//---------------------------------------------------------------------------
namespace cnWindows{
//---------------------------------------------------------------------------
}	// namespace cnWindows
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
