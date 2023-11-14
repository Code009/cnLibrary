#pragma once

#include <cnRTL\WinSDKHeader.h>

#pragma warning(push)
#pragma warning(disable:4458)	// declaration of '' hides class member
#pragma warning(disable:4459)	// declaration of '' hides global declaration

// COM / OLE

#include <Ole2.h>
#include <Shlobj.h>

// socket

#include <Winsock2.h>

// core audio

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA

#include <Audioclient.h>
#include <Audiopolicy.h>

#endif


// wmm audio

#include <Mmsystem.h>
#include <Mmdeviceapi.h>


// odbc

#include <sql.h>
#include <sqlext.h>
#include <odbcinst.h>

// wic

#include <wincodec.h>

// d3d9

#include <d3d9.h>

// dxgi

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
#include <dxgi.h>
#include <d3d11.h>
#endif

#ifdef	_DEBUG
#include <crtdbg.h>
#endif
 

#pragma warning(pop)
