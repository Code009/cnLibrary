#pragma once


#include <memory>
#include <string>
#include <math.h>


#if _MSC_VER < 1900

#error "cnLibrary - CRT - need Visual C++ 2015 (14.0) Toolset v140 or later"

#endif	// _MSC_VER < 1900
//	_MSC_VER : 1900			Visual C++ 2015 (14.0)	Toolset v140

#include <functional>
#include <iterator>
#include <algorithm>

//	_MSC_FULL_VER : 190023506			Visual C++ 2015 (14.0)	Toolset v140 update 1
//	_MSC_FULL_VER : 190023918			Visual C++ 2015 (14.0)	Toolset v140 update 2
//	_MSC_FULL_VER : 190024210			Visual C++ 2015 (14.0)	Toolset v140 update 3
//	_MSC_VER : 1910			Visual C++ 2017 (15.0)	Toolset v141
//	_MSC_VER : 1911			Visual C++ 2017 (15.3)	Toolset v141
//	_MSC_VER : 1912			Visual C++ 2017 (15.5)	Toolset v141
//	_MSC_VER : 1913			Visual C++ 2017 (15.6)	Toolset v141
//	_MSC_VER : 1914			Visual C++ 2017 (15.7)	Toolset v141
//	_MSC_VER : 1915			Visual C++ 2017 (15.8)	Toolset v141
#if _MSC_VER < 1916
#endif	// _MSC_VER < 1916
//	_MSC_VER : 1916			Visual C++ 2017 (15.9)	Toolset v141


#if _MSC_VER < 1920

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

#define cnLibrary_CRTFEATURE_EXPERIMENTAL_COROUTINE	201500L

#include <experimental/coroutine>

#endif // _RESUMABLE_FUNCTIONS_SUPPORTED

#endif	// _MSC_VER < 1920

//	_MSC_VER : 1920			Visual Studio 2019 RTW (16.0)
#if _MSC_VER >= 1920

//	use C++20 Feature-test macros from Visual C++ 2019 (16.0)

#include <CRT/CRTFeatureCheck.h>

#if cnLibrary_CRTFEATURE_COROUTINE>=201902L

#include <coroutine>

#endif // cnLibrary_CPPFEATURE_COROUTINE>=201902L


#endif // _MSC_VER >= 1920

//	_MSC_VER : 1930			Visual Studio 2022 RTW 17.0
#if _MSC_VER >= 1930

#ifndef cnLib_WINCRT_WIN32ATOMIC

// atomic needs alignment to work, which is not supported before VS2022
#include <atomic>

#endif // !cnLib_WINCRT_WIN32ATOMIC



#else

#ifndef cnLib_WINCRT_WIN32ATOMIC
#define	cnLib_WINCRT_WIN32ATOMIC	// use win32 atomic before VS2022
#endif

#endif // _MSC_VER >= 1930


#define	cnCRT_BYTEORDER_LITTLEENDIAN	1
#define	cnCRT_BYTEORDER_BIGENDIAN		0

