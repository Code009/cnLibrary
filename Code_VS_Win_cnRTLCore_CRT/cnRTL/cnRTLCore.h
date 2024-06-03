#pragma once

#ifdef cnLibrary_RTL_CORE
#pragma message(cnLib_FILE_LINE ": included other cnRTLCore" cnLibrary_RTL_CORE)
#error	included other cnRTLCore
#endif

#define	cnLibrary_RTL_CORE	"WinCRT"


#include <cnRTL/CRTHeader.h>
#include <cnTK/cnTK.h>

#include <CRT/CRTCoreRuntime.hpp>
#include <CRT/CRTCoreTemplateKit.hpp>

