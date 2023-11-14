/*- cnWin - WinSocket -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-06-13                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <WinSock2.h>
#include <WS2tcpip.h>
/*-------------------------------------------------------------------------*/
#include <cnWin\Win_Common.h>
#include <cnWin\Win_Thread.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
class cSocketLookupService
{
private:
	HANDLE fLookup;
	DWORD fLookupFlag;
	cnRTL::cMemoryBuffer fQuertSetMemory;

	WSAQUERYSET *QuerySetBuffer(void);
	WSAQUERYSET *QuerySetBuffer(uIntn Size);
	uIntn QuerySetBufferCapacity(void);

public:
	cSocketLookupService();
	~cSocketLookupService();

	bool Next(void);
	void End(void);

	// DNS

	bool DNSBegin(wchar_t *Name);
	uIntn DNSGetAddressCount(void);
	bool DNSGetAddress(uIntn Index,cnRTL::cnWinRTL::cWin32SocketAddressBuffer &AddressBuffer);
	iPtr<iSocketAddress> DNSCreateAddress(uIntn Index);


	// Bluetooth Device
	bool BluetoothDeviceBegin(void);
	const wchar_t* BluetoothDeviceGetName(void);
	bool BluetoothDeviceGetAddress(cnRTL::cnWinRTL::cWin32SocketAddressBuffer &AddressBuffer);
	iPtr<iSocketAddress> BluetoothDeviceCreateAddress(void);

	// Bluetooth Service
	bool BluetoothServiceBegin(LPSOCKADDR DeviceAddress,DWORD DeviceAddressLength);
};
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
namespace cnWinNT6{
//---------------------------------------------------------------------------
namespace WinsockIO{
//---------------------------------------------------------------------------
typedef cnRTL::cnWinRTL::cNT6ThreadPoolSocketStreamProtocol cSocketStreamProtocol;
typedef cnRTL::cnWinRTL::cNT6ThreadPoolSocketDatagramProtocol cSocketDatagramProtocol;
//---------------------------------------------------------------------------
}	//	namespace WinsockIO
//---------------------------------------------------------------------------
}	// namespace cnWinNT6
//---------------------------------------------------------------------------
#if cnWin_WINSOCKIO_VERSION < _WIN32_WINNT_VISTA
#undef	cnWin_WINSOCKIO_VERSION
#undef	cnWin_WINSOCKIO_NAMESPACE
#define	cnWin_WINSOCKIO_VERSION			_WIN32_WINNT_VISTA
#define	cnWin_WINSOCKIO_NAMESPACE		cnWinNT6::WinsockIO
#endif // cnWin_WINSOCKIO_VERSION < _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
#endif	// >= _WIN32_WINNT_VISTA

#if 0

//---------------------------------------------------------------------------
struct cSocketModule
{
	cSocketModule();
	~cSocketModule();

	cnRTL::cnWinRTL::cWinWaitObject Waiter;

	cnRTL::cInterfaceStaticImplementT<cSocketStreamProtocol> TCP;
	cnRTL::cInterfaceStaticImplementT<cSocketDatagramProtocol> UDP;

};
extern cSocketModule gSocketModule;
#endif // 0
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
