#pragma once

#define	_HAS_EXCEPTIONS	0	// Disable CRT exceptions

// NT6
// Allow use of features specific to Windows Vista or later
#define _WIN32_WINNT	_WIN32_WINNT_VISTA

#include <cnWin\WinSDKHeader.h>
#include <cnRTL\CRTHeader.h>

#include <cnRTL\WinRTL.h>
#include <cnWin\cnWin.h>

#pragma comment(lib,"cnWin.lib")
#pragma detect_mismatch("cnLibrary.cnWindows","cnLibrary.cnWinNT6.Native")
