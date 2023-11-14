#include <cnRTL\WinSDKHeader.h>
#define INITGUID
#include <Mswsock.h>
#include <ws2bth.h>
#include <WS2tcpip.h>
// For guids
#include <svcguid.h>

#include "Win_Socket.h"

#pragma comment(lib,"ws2_32.lib")

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace cnWin;

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
using namespace cnWinNT6;
#endif	// >= _WIN32_WINNT_VISTA

// disable buffers
	//int opt=0;
	//::setsockopt(SocketIO->Handle,SOL_SOCKET,SO_SNDBUF,(char*)&opt,sizeof(opt));
	//::setsockopt(SocketIO->Handle,SOL_SOCKET,SO_RCVBUF,(char*)&opt,sizeof(opt));

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSocketLookupService::cSocketLookupService()
{
	fLookup=INVALID_HANDLE_VALUE;
}
//---------------------------------------------------------------------------
cSocketLookupService::~cSocketLookupService()
{
	End();
}
//---------------------------------------------------------------------------
WSAQUERYSET *cSocketLookupService::QuerySetBuffer(void)
{
	return static_cast<WSAQUERYSET*>(fQuertSetMemory[0]);
}
//---------------------------------------------------------------------------
WSAQUERYSET *cSocketLookupService::QuerySetBuffer(uIntn Size)
{
	fQuertSetMemory.SetSize(0);
	fQuertSetMemory.SetSize(Size);
	return static_cast<WSAQUERYSET*>(fQuertSetMemory[0]);
}
//---------------------------------------------------------------------------
uIntn cSocketLookupService::QuerySetBufferCapacity(void)
{
	return fQuertSetMemory.GetCapacity();
}
//---------------------------------------------------------------------------
bool cSocketLookupService::Next(void)
{
	auto qs=QuerySetBuffer(sizeof(WSAQUERYSET));
	// query next
	DWORD dwSize=static_cast<DWORD>(QuerySetBufferCapacity());
	auto ret=::WSALookupServiceNextW(fLookup,fLookupFlag,&dwSize,qs);
	if(ret!=ERROR_SUCCESS){
		auto Error=WSAGetLastError();
		if(Error==WSAEFAULT){
			// resize buffer
			qs=QuerySetBuffer(dwSize);
			dwSize=static_cast<DWORD>(QuerySetBufferCapacity());
			ret=WSALookupServiceNextW(fLookup,fLookupFlag,&dwSize,qs);
			if(ret!=ERROR_SUCCESS)
				return false;
		}
		else{
			return false;
		}
	}
	cnLib_ASSERT(ret==ERROR_SUCCESS);
	return true;
}
//---------------------------------------------------------------------------
void cSocketLookupService::End(void)
{
	if(fLookup!=INVALID_HANDLE_VALUE){
		::WSALookupServiceEnd(fLookup);
		fLookup=INVALID_HANDLE_VALUE;
	}
}
//---------------------------------------------------------------------------
bool cSocketLookupService::DNSBegin(wchar_t *Name)
{
	static constexpr uIntn qsSize=sizeof(WSAQUERYSET);
	auto qs=QuerySetBuffer(qsSize);
	cnMemory::ZeroFill(*qs);
	qs->dwSize=qsSize;
	// host name service guid
	GUID ServiceGUID=SVCID_INET_HOSTADDRBYNAME;
	//GUID ServiceGUID=SVCID_HOSTNAME;
	qs->lpServiceClassId=&ServiceGUID;
	qs->dwNameSpace=NS_DNS;
	qs->lpszServiceInstanceName=Name;
	//qs->dwNameSpace=NS_ALL;
	AFPROTOCOLS protocols[2]={
		{AF_INET,IPPROTO_TCP},
		//{AF_INET6,IPPROTO_TCP},
		{AF_INET,IPPROTO_UDP},
		//{AF_INET,IPPROTO_UDP},
		//{AF_INET6,IPPROTO_UDP},
	};
	qs->lpafpProtocols=protocols;
	qs->dwNumberOfProtocols=2;
	fLookupFlag=LUP_RETURN_ADDR;
	auto ret=WSALookupServiceBeginW(qs,fLookupFlag,&fLookup);
	//auto error=::WSAGetLastError();
	return ret==ERROR_SUCCESS;
}
//---------------------------------------------------------------------------
uIntn cSocketLookupService::DNSGetAddressCount(void)
{
	auto qs=QuerySetBuffer();
	return qs->dwNumberOfCsAddrs;
}
//---------------------------------------------------------------------------
bool cSocketLookupService::DNSGetAddress(uIntn Index,cWin32SocketAddressBuffer &AddressBuffer)
{
	auto qs=QuerySetBuffer();
	if(Index<qs->dwNumberOfCsAddrs){
		auto AddrLength=qs->lpcsaBuffer->RemoteAddr.iSockaddrLength;
		AddressBuffer.SetSockAddrLen(AddrLength);
		cnMemory::Copy(AddressBuffer.SockAddr(),qs->lpcsaBuffer->RemoteAddr.lpSockaddr,AddrLength);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
iPtr<iSocketAddress> cSocketLookupService::DNSCreateAddress(uIntn Index)
{
	auto qs=QuerySetBuffer();
	if(Index<qs->dwNumberOfCsAddrs){
		auto ReturnAddress=Win32CreateCopySocketAddress(qs->lpcsaBuffer->RemoteAddr.lpSockaddr,qs->lpcsaBuffer->RemoteAddr.iSockaddrLength);
		return ReturnAddress;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
bool cSocketLookupService::BluetoothDeviceBegin(void)
{
	static constexpr uIntn qsSize=sizeof(WSAQUERYSET);
	auto qs=QuerySetBuffer(qsSize);
	cnMemory::ZeroFill(*qs);
	qs->dwSize=qsSize;
	// host name service guid
	//qs->lpServiceClassId=;
	qs->dwNameSpace=NS_BTH;

	fLookupFlag=LUP_CONTAINERS|LUP_RETURN_TYPE|LUP_RETURN_NAME|LUP_RETURN_ADDR;
	DWORD flag=fLookupFlag|LUP_FLUSHCACHE;
	auto ret=WSALookupServiceBeginW(qs,flag,&fLookup);
	//auto err=::WSAGetLastError();
	return ret==ERROR_SUCCESS;
}
//---------------------------------------------------------------------------
const wchar_t* cSocketLookupService::BluetoothDeviceGetName(void)
{
	auto qs=QuerySetBuffer();
	return qs->lpszServiceInstanceName;
}
//---------------------------------------------------------------------------
bool cSocketLookupService::BluetoothDeviceGetAddress(cWin32SocketAddressBuffer &AddressBuffer)
{
	auto qs=QuerySetBuffer();
	if(qs->dwNumberOfCsAddrs>0){
		auto AddrLength=qs->lpcsaBuffer->RemoteAddr.iSockaddrLength;
		AddressBuffer.SetSockAddrLen(AddrLength);
		cnMemory::Copy(AddressBuffer.SockAddr(),qs->lpcsaBuffer->RemoteAddr.lpSockaddr,AddrLength);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
iPtr<iSocketAddress> cSocketLookupService::BluetoothDeviceCreateAddress(void)
{
	auto qs=QuerySetBuffer();
	if(qs->dwNumberOfCsAddrs>0){
		auto ReturnAddress=Win32CreateCopySocketAddress(qs->lpcsaBuffer->RemoteAddr.lpSockaddr,qs->lpcsaBuffer->RemoteAddr.iSockaddrLength);
		return ReturnAddress;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
//00030000-0000-1000-8000-00805F9B34FB
DEFINE_GUID(g_guidServiceClass_RFCOMM, 0x03000000, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(g_guidServiceClass, 0xb62c4e8d, 0x62cc, 0x404b, 0xbb, 0xbf, 0xbf, 0x3e, 0x3b, 0xbb, 0x13, 0x74);
bool cSocketLookupService::BluetoothServiceBegin(LPSOCKADDR DeviceAddress,DWORD DeviceAddressLength)
{
	auto qs=QuerySetBuffer(sizeof(WSAQUERYSET));
	cnMemory::ZeroFill(*qs);
	qs->dwSize=sizeof(WSAQUERYSET);
	// host name service guid
	qs->lpServiceClassId=(LPGUID)&g_guidServiceClass_RFCOMM;
	qs->dwNameSpace=NS_BTH;
	//WSAPROTOCOL_INFOW pi;
	wchar_t AddressString[200];
	DWORD AddressStringLength=200;
	WSAAddressToStringW(DeviceAddress,DeviceAddressLength,NULL,AddressString,&AddressStringLength);
	qs->lpszContext=AddressString;

	fLookupFlag=LUP_RETURN_NAME|LUP_RETURN_ADDR;
	DWORD flag=fLookupFlag|LUP_FLUSHCACHE;
	auto ret=WSALookupServiceBeginW(qs,flag,&fLookup);
	//auto err=::WSAGetLastError();
	return ret==ERROR_SUCCESS;
}
//---------------------------------------------------------------------------
#if	0

iPtr<iConnection>		cSocketStreamProtocol::Connect(iTypeID ConnectionIID,iAddress *RemoteAddress,iAddress *LocalAddress)
{
	if(WindowsInterface::WinSocketConnectionTypeIsSupported(ConnectionIID)==false)
		return nullptr;

	auto RemoteSocketAddress=CreateCopySocketAddress(RemoteAddress);
	if(RemoteSocketAddress==nullptr)
		return nullptr;

	auto raddr=RemoteSocketAddress->SockAddr();
	auto raddrlen=RemoteSocketAddress->SockAddrLen();

	SOCKET NewSocket;

	auto LocalSocketAddress=CreateCopySocketAddress(LocalAddress);
	if(LocalSocketAddress!=nullptr){
		auto laddr=LocalSocketAddress->SockAddr();
		int laddrlen=LocalSocketAddress->SockAddrLen();
		NewSocket=::WSASocketW(laddr->sa_family,fType,fProtocol,NULL,0,WSA_FLAG_OVERLAPPED);
		if(NewSocket==INVALID_SOCKET)
			return nullptr;
		if(::bind(NewSocket,laddr,laddrlen)!=0){
			::closesocket(NewSocket);
			return nullptr;
		}
	}
	else{
		NewSocket=::WSASocketW(raddr->sa_family,fType,fProtocol,NULL,0,WSA_FLAG_OVERLAPPED);
		if(NewSocket==INVALID_SOCKET)
			return nullptr;
	}

	if(::connect(NewSocket,raddr,raddrlen)!=0){
		::closesocket(NewSocket);
		return nullptr;
	}
	//BOOL Val=1;
	//::setsockopt(NewSocket,SOL_SOCKET,SO_DONTLINGER,(const char*)&Val,1);

	return WindowsInterface::WinSocketMakeConnection(NewSocket,ConnectionIID,cnVar::MoveCast(RemoteSocketAddress),LocalSocketAddress);
}
//---------------------------------------------------------------------------
iPtr<iConnectionTask>		cSocketStreamProtocol::ConnectAsync(iTypeID ConnectionIID,iAddress *RemoteAddress,iAddress *LocalAddress)
{
	if(WindowsInterface::WinSocketConnectionTypeIsSupported(ConnectionIID)==false)
		return nullptr;

	auto RemoteSocketAddress=CreateCopySocketAddress(RemoteAddress);
	if(RemoteSocketAddress==nullptr)
		return nullptr;


	auto LocalSocketAddress=CreateCopySocketAddress(LocalAddress);

	SOCKET NewSocket;
	if(LocalSocketAddress!=nullptr){
		auto laddr=LocalSocketAddress->SockAddr();
		int laddrlen=LocalSocketAddress->SockAddrLen();
		NewSocket=::WSASocketW(laddr->sa_family,fType,fProtocol,NULL,0,WSA_FLAG_OVERLAPPED);
		if(NewSocket==INVALID_SOCKET)
			return nullptr;
		if(::bind(NewSocket,laddr,laddrlen)!=0){
			::closesocket(NewSocket);
			return nullptr;
		}
	}
	else{
		auto raddr=RemoteSocketAddress->SockAddr();
		NewSocket=::WSASocketW(raddr->sa_family,fType,fProtocol,NULL,0,WSA_FLAG_OVERLAPPED);
		if(NewSocket==INVALID_SOCKET)
			return nullptr;
	}

	return WindowsInterface::WinSocketMakeConnectTask(NewSocket,ConnectionIID,cnVar::MoveCast(RemoteSocketAddress),cnVar::MoveCast(LocalSocketAddress));

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSocketModule::cSocketModule()
	: TCP(SOCK_STREAM,IPPROTO_TCP)
	, UDP(SOCK_DGRAM,IPPROTO_UDP)
{
}
//---------------------------------------------------------------------------
cSocketModule::~cSocketModule()
{
#if 0 && defined(cnLib_DEBUG)
	Waiter.Wait(INFINITE);
#else
	Waiter.Wait(3000);
#endif // 0 && defined(cnLib_DEBUG)
}
#endif
//---------------------------------------------------------------------------
