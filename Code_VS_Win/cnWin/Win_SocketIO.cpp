#include <cnRTL\WinSDKHeader.h>
#include <Mswsock.h>
#include <WS2tcpip.h>
// For guids
//#include <svcguid.h>

#include "Win_SocketIO.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace cnWin;
using namespace cnWinNT6;

//---------------------------------------------------------------------------
#if 0 && _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iConnection> WinSocketIO::MakeStreamConnection(SOCKET Socket,iPtr<iSocketAddress> RemoteAddress,iPtr<iSocketAddress> LocalAddress)
{
	auto SocketIO=rCreate<Windows::cNT6ThreadPoolClientSocketIOHandle>();
	if(SocketIO->Open(Socket)==false){
		::closesocket(Socket);
		return nullptr;
	}

	return Windows::NT6ThreadPoolSocketMakeStreamConnection(cnVar::MoveCast(SocketIO),cnVar::MoveCast(LocalAddress),cnVar::MoveCast(RemoteAddress));
}
//---------------------------------------------------------------------------
iPtr<iConnection> WinSocketIO::MakeEndpointConnection(SOCKET Socket,iPtr<iSocketAddress> RemoteAddress,iPtr<iSocketAddress> LocalAddress)
{
	auto SocketIO=rCreate<Windows::cNT6ThreadPoolClientSocketIOHandle>();
	if(SocketIO->Open(Socket)==false){
		::closesocket(Socket);
		return nullptr;
	}

	return Windows::NT6ThreadPoolSocketMakeEndpointConnection(cnVar::MoveCast(SocketIO),cnVar::MoveCast(LocalAddress),cnVar::MoveCast(RemoteAddress));
}
//---------------------------------------------------------------------------
static iPtr<iConnectionConnector> MakeConnector(NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress,int SocketType,int SocketProtocol)
{
	int AddressFamily;
	if(LocalAddress==nullptr){
		cnLib_ASSERT(RemoteAddress!=nullptr);
		cWin32SocketAddressBuffer AddressBuffer;
		// create any local address
		auto RemoteSockaddr=RemoteAddress->SockAddr();
		AddressFamily=RemoteSockaddr->sa_family;

		if(AddressBuffer.SetAddressAny(static_cast<ADDRESS_FAMILY>(AddressFamily))==false)
			return nullptr;

		LocalAddress=AddressBuffer.CreateSocketAddress();
	}
	else{
		auto LocalSockaddr=LocalAddress->SockAddr();
		AddressFamily=LocalSockaddr->sa_family;
	}

	
	return iCreate<Windows::cNT6ThreadPoolSocketConnectionConnector>(cnVar::MoveCast(LocalAddress),MakeConnectionFunction,AddressFamily,SocketType,SocketProtocol);
}
//---------------------------------------------------------------------------
iPtr<iConnectionConnector>		WinSocketIO::MakeStreamConnector(iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress,int SocketType,int SocketProtocol)
{
	return MakeConnector(NT6ThreadPoolSocketMakeStreamConnection,cnVar::MoveCast(LocalAddress),cnVar::MoveCast(RemoteAddress),SocketType,SocketProtocol);
}
//---------------------------------------------------------------------------
iPtr<iConnectionConnector>		WinSocketIO::MakeEndpointConnector(iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress,int SocketType,int SocketProtocol)
{
	return MakeConnector(NT6ThreadPoolSocketMakeEndpointConnection,cnVar::MoveCast(LocalAddress),cnVar::MoveCast(RemoteAddress),SocketType,SocketProtocol);
}
//---------------------------------------------------------------------------
static iPtr<iConnectionListener> MakeListener(NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,iPtr<iSocketAddress> LocalAddress,int SocketType,int SocketProtocol)
{
	if(LocalAddress==nullptr)
		return nullptr;

	auto addr=LocalAddress->SockAddr();
	int addrlen=LocalAddress->SockAddrLen();

	SOCKET NewSocket=::WSASocketW(addr->sa_family,SocketType,SocketProtocol,NULL,0,WSA_FLAG_OVERLAPPED);
	if(NewSocket==INVALID_SOCKET){
		//create socket failed
		return nullptr;
	}

	// bind address
	if(::bind(NewSocket,addr,addrlen)==SOCKET_ERROR){
		::closesocket(NewSocket);
		return nullptr;
	}
	// start listen
	if(listen(NewSocket, SOMAXCONN)==SOCKET_ERROR){
		::closesocket(NewSocket);
		return nullptr;
	}

	DWORD FuncPtrSize;
	GUID FunctionID;
	
	LPFN_ACCEPTEX Func_AcceptEx=nullptr;
	FunctionID=WSAID_ACCEPTEX;
	if(::WSAIoctl(NewSocket,SIO_GET_EXTENSION_FUNCTION_POINTER,&FunctionID,sizeof(FunctionID),&Func_AcceptEx,sizeof(Func_AcceptEx),&FuncPtrSize,nullptr,nullptr)!=0){
		::closesocket(NewSocket);
		return nullptr;
	}
	
	FunctionID=WSAID_GETACCEPTEXSOCKADDRS;
	LPFN_GETACCEPTEXSOCKADDRS Func_GetAcceptExSockaddrs=nullptr;
	if(::WSAIoctl(NewSocket,SIO_GET_EXTENSION_FUNCTION_POINTER,&FunctionID,sizeof(FunctionID),&Func_GetAcceptExSockaddrs,sizeof(Func_GetAcceptExSockaddrs),&FuncPtrSize,nullptr,nullptr)!=0){
		::closesocket(NewSocket);
		return nullptr;
	}

	auto SocketIO=rCreate<Windows::cNT6ThreadPoolSocketIOHandle>();
	if(SocketIO->Open(NewSocket)==false){
		::closesocket(NewSocket);
		return nullptr;
	}

	return iCreate<Windows::cNT6ThreadPoolSocketConnectionListener>(cnVar::MoveCast(SocketIO),Func_AcceptEx,Func_GetAcceptExSockaddrs,MakeConnectionFunction,cnVar::MoveCast(LocalAddress),SocketType,SocketProtocol);
}
//---------------------------------------------------------------------------
iPtr<iConnectionListener>	WinSocketIO::MakeStreamListener(iPtr<iSocketAddress> LocalAddress,int SocketType,int SocketProtocol)
{
	return MakeListener(NT6ThreadPoolSocketMakeStreamConnection,cnVar::MoveCast(LocalAddress),SocketType,SocketProtocol);
}
//---------------------------------------------------------------------------
iPtr<iConnectionListener>	WinSocketIO::MakeEndpointListener(iPtr<iSocketAddress> LocalAddress,int SocketType,int SocketProtocol)
{
	return MakeListener(NT6ThreadPoolSocketMakeEndpointConnection,cnVar::MoveCast(LocalAddress),SocketType,SocketProtocol);
}
//---------------------------------------------------------------------------
iPtr<iConnectionQueue>		WinSocketIO::MakeStreamListenQueue(iPtr<iSocketAddress> LocalAddress,int SocketType,int SocketProtocol)
{
	auto Listener=MakeListener(NT6ThreadPoolSocketMakeStreamConnection,cnVar::MoveCast(LocalAddress),SocketType,SocketProtocol);
	if(Listener==nullptr)
		return nullptr;

	return iCreate<cConnectionQueueFromListener>(Listener);
}
//---------------------------------------------------------------------------
iPtr<iConnectionQueue>		WinSocketIO::MakeEndpointListenQueue(iPtr<iSocketAddress> LocalAddress,int SocketType,int SocketProtocol)
{
	auto Listener=MakeListener(NT6ThreadPoolSocketMakeEndpointConnection,cnVar::MoveCast(LocalAddress),SocketType,SocketProtocol);
	if(Listener==nullptr)
		return nullptr;

	return iCreate<cConnectionQueueFromListener>(Listener);
}
//---------------------------------------------------------------------------
iPtr<iMultipointStream>		WinSocketIO::MakeDatagramStream(SOCKET Socket,iPtr<iSocketAddress> LocalAddress)
{
	auto SocketIO=rCreate<Windows::cNT6ThreadPoolSocketIOHandle>();
	if(SocketIO->Open(Socket)==false){
		::closesocket(Socket);
		return nullptr;
	}
	return iCreate<Windows::cNTSocketOverlappedIOHandleDatagramStream>(cnVar::MoveCast(SocketIO),cnVar::MoveCast(LocalAddress));
}
//---------------------------------------------------------------------------
iPtr<iMultipointQueue>		WinSocketIO::MakeDatagramQueue(SOCKET Socket,iPtr<iSocketAddress> LocalAddress)
{
	auto SocketIO=rCreate<Windows::cNT6ThreadPoolSocketIOHandle>();
	if(SocketIO->Open(Socket)==false){
		::closesocket(Socket);
		return nullptr;
	}

	auto Stream=iCreate<Windows::cNTSocketOverlappedIOHandleDatagramStream>(cnVar::MoveCast(SocketIO),cnVar::MoveCast(LocalAddress));
	return iCreate<cMultipointQueueFromStream>(Stream);
}
//---------------------------------------------------------------------------
#endif	// >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
