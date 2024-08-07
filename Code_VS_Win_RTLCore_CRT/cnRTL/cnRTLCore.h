#pragma once

#ifdef cnLibrary_RTL_CORE
#pragma message(cnLib_FILE_LINE ": included other cnRTLCore" cnLibrary_RTL_CORE)
#error	included other cnRTLCore
#endif

#define	cnLibrary_RTL_CORE	"WinCRT"


#include <cnRTL\CRTHeader.h>
#include <cnTK\cnTK.h>

#if defined(cnLib_WINCRT_WIN32ATOMIC)

#include <cnRTL\WinSDKHeader.h>

#endif


#include <CRT\CRTCoreRuntime.hpp>

#ifdef cnLib_WINCRT_WIN32ATOMIC
#include <cnRTL\Win32CoreAtomic.h>
#else
#include <CRT\CRTAtomic.hpp>
#endif // !cnLib_WINCRT_WIN32ATOMIC

#include <CRT\CRTCoreTemplateKit.hpp>

