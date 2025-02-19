#pragma once

#ifdef cnLibrary_RTL_CORE
#pragma message(cnLib_FILE_LINE ": included other cnRTLCore" cnLibrary_RTL_CORE)
#error	included other cnRTLCore
#endif

#define	cnLibrary_RTL_CORE	"WinCRT"

#include <cnRTL\CRTHeader.h>

#include <cnTK\cnTK.h>
#include <cnTK\cnTK.OS.h>

#if !cnLib_WINCRT_ATOMIC\
	|| !cnLib_WINCRT_THREAD

#include <cnRTL\WinSDKHeader.h>

#endif


#include <CRT\CRTCoreRuntime.hpp>

#ifdef cnLib_WINCRT_ATOMIC
#include <CRT\CRTAtomic.hpp>
#else
#include <cnRTL\Win32CoreAtomic.h>
#endif // !cnLib_WINCRT_WIN32ATOMIC

#ifdef cnLib_WINCRT_THREAD
#include <CRT\CRTThread.hpp>
#else
#include <cnRTL\Win32CoreThread.h>
#endif // !cnLib_WINCRT_WIN32THREAD

#include <CRT\CRTCoreTemplateKit.hpp>

