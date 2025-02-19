#pragma once

#pragma warning(push)
#pragma warning(disable:4458)	// declaration of '' hides class member
#pragma warning(disable:4459)	// declaration of '' hides global declaration

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#ifndef _WIN32_WINNT_WIN2K
#define _WIN32_WINNT_WIN2K                  0x0500
#endif

#ifndef _WIN32_WINNT_WINXP
#define _WIN32_WINNT_WINXP                  0x0501
#endif

#ifndef _WIN32_WINNT_WS03
#define _WIN32_WINNT_WS03                   0x0502
#endif

#ifndef _WIN32_WINNT_VISTA
#define _WIN32_WINNT_VISTA                  0x0600
#endif

#ifndef _WIN32_WINNT_WIN7
#define _WIN32_WINNT_WIN7                   0x0601
#endif

#ifndef _WIN32_WINNT_WIN8
#define _WIN32_WINNT_WIN8                   0x0602
#endif

#ifndef _WIN32_WINNT_WINBLUE
#define _WIN32_WINNT_WINBLUE                0x0603
#endif

#ifndef _WIN32_WINNT_WIN10
#define _WIN32_WINNT_WIN10                  0x0A00 /* ABRACADABRA_THRESHOLD*/
#endif

#if !defined(WDK_NTDDI_VERSION) || WDK_NTDDI_VERSION == NTDDI_WINBLUE

// resolve IUnknown not defined in windows 8.1
struct IUnknown;

#endif // !WDK_NTDDI_VERSION


#define	_HIDE_GLOBAL_ASYNC_STATUS		// hide global def in AsyncInfo.h

#pragma warning(pop)

#include <AclAPI.h>
	// WinSecurity

#include <WinSock2.h>
	// WinSocketAddress
	// WinSocketIO

#include <Mswsock.h>
	// WinSocketIO

#include <WS2tcpip.h>
	// WinSocketAddress

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA

#include <mmdeviceapi.h>
	// WinCoreAudioDevice

#endif

#include <ShlObj.h>
	// WinCOMShell

#include <shlwapi.h>
	// WinCOMShell

#include <shellapi.h>
	// WinCOMShell

#include <wincodec.h>
	// WinCOMWIC


#include <sql.h>
	// WinODBC


// compability maro for win3.x
#undef	Yield