﻿/*- Windows Interface -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-01-25                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#pragma warning(push)
#pragma warning(disable:4458)	// declaration of '' hides class member
#pragma warning(disable:4459)	// declaration of '' hides global declaration

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
struct IUnknown;

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
#include <Audioclient.h>
#endif	// _WIN32_WINNT_VISTA
#include <Mmsystem.h>

#pragma warning(pop)

#include <cnSystem\cnSystem.h>

#include <cnSystem\cnWinThread.h>
#include <cnSystem\cnWinFileSystem.h>
#include <cnSystem\cnWinNetwork.h>
#include <cnSystem\cnWinDatabase.h>
#include <cnSystem\cnWinUI.h>

#ifdef __cplusplus

//---------------------------------------------------------------------------
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#define	override
#endif
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iCOMInterface : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnClass::ImplementCastInterface(this,ID);	}
	
	virtual IUnknown* cnLib_FUNC GetCOMInterface(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iCOMApartmentThreading : public iReference
{
public:
	virtual iDispatch* cnLib_FUNC GetDispatch(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWaveFormat : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnClass::ImplementCastInterface(this,ID);	}

	virtual const WAVEFORMATEX*	cnLib_FUNC GetFormat(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
namespace cnWindows{
//---------------------------------------------------------------------------
rPtr<iMutex> cnLib_FUNC QueryLibraryMutex(void)noexcept(true);
//---------------------------------------------------------------------------
extern const wchar_t *DLLFileName_cnPLWindows;
//---------------------------------------------------------------------------
struct LibLoadResult
{
	static const ufInt8	Success					=0;
	static const ufInt8	MissingLibLoader		=1;
	static const ufInt8	LoaderVersionNotMatch	=2;
	static const ufInt8	MissingLibFile			=3;
	static const ufInt8	FileVersionNotMatch		=4;
};
//---------------------------------------------------------------------------
extern iTextStreamOutput*const DebugLogStream;
//---------------------------------------------------------------------------
rPtr<iMemoryHeap>		cnLib_FUNC CreateMemoryHeap(void)noexcept(true);
//---------------------------------------------------------------------------
rPtr<iTextEncodingConverter> cnLib_FUNC CodePageToUTF16(UINT SrcCodePage)noexcept(true);
rPtr<iTextEncodingConverter> cnLib_FUNC CodePageFromUTF16(UINT DestCodePage)noexcept(true);

//---------------------------------------------------------------------------
rPtr<iCOMApartmentThreading> cnLib_FUNC COMApartmentThreadingGet(void)noexcept(true);
rPtr<iCOMApartmentThreading> cnLib_FUNC COMApartmentThreadingQuery(iDispatch *Dispatch)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace cnWindows
//---------------------------------------------------------------------------
}	// namespace cnLibrary
	//---------------------------------------------------------------------------
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#undef	override
#endif
	
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
