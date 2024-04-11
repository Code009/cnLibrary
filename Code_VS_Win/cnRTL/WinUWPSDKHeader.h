#pragma once

#pragma warning(push)
#pragma warning(disable:4471)	// a forward declaration of an unscoped enumeration must have an underlying type

#include <windows.foundation.h>
#include <windows.storage.streams.h>
#include <weakreference.h>

#include <roapi.h>
#if WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION >=0x50000
#include <robuffer.h>
#endif

#pragma warning(pop)

#if _WIN32_WINNT >= _WIN32_WINNT_WIN10

#include <windows.devices.bluetooth.h>
	// WinUWPGATT

#include <windows.devices.bluetooth.advertisement.h>
	// WinUWPGATT

#endif // _WIN32_WINNT_WIN10
