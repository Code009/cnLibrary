/*- cnWin - WinSocket -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-07-15                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnWin\WinSDKHeader.h>
#include <Mswsock.h>

#include <cnWin\Win_Common.h>
#include <cnWin\Win_Thread.h>
#include <cnWin\Win_Socket.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
#if 0 && _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
namespace cnWinNT6{
//---------------------------------------------------------------------------
namespace WinSocketIO{
//---------------------------------------------------------------------------
iPtr<iConnection>			MakeStreamConnection(SOCKET Socket,iPtr<iSocketAddress> RemoteAddress,iPtr<iSocketAddress> LocalAddress);
iPtr<iConnection>			MakeEndpointConnection(SOCKET Socket,iPtr<iSocketAddress> RemoteAddress,iPtr<iSocketAddress> LocalAddress);
iPtr<iConnectionConnector>	MakeStreamConnector(iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress,int SocketType,int SocketProtocol);
iPtr<iConnectionConnector>	MakeEndpointConnector(iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress,int SocketType,int SocketProtocol);
iPtr<iConnectionListener>	MakeStreamListener(iPtr<iSocketAddress> LocalAddress,int SocketType,int SocketProtocol);
iPtr<iConnectionListener>	MakeEndpointListener(iPtr<iSocketAddress> LocalAddress,int SocketType,int SocketProtocol);
iPtr<iConnectionQueue>		MakeStreamListenQueue(iPtr<iSocketAddress> LocalAddress,int SocketType,int SocketProtocol);
iPtr<iConnectionQueue>		MakeEndpointListenQueue(iPtr<iSocketAddress> LocalAddress,int SocketType,int SocketProtocol);
iPtr<iMultipointStream>		MakeDatagramStream(SOCKET Socket,iPtr<iSocketAddress> LocalAddress);
iPtr<iMultipointQueue>		MakeDatagramQueue(SOCKET Socket,iPtr<iSocketAddress> LocalAddress);
//---------------------------------------------------------------------------
}	//	namespace WinSocketIO
//---------------------------------------------------------------------------
}	// namespace cnWinNT6
//---------------------------------------------------------------------------
namespace vWinSocketIO=cnWinNT6::WinSocketIO;
//---------------------------------------------------------------------------
#endif	// >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
