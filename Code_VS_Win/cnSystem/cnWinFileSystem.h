/*- Windows - File System -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-01-25                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnSystem\cnMemory.h>
#include <cnSystem\cnFileSystem.h>


#ifdef __cplusplus

//---------------------------------------------------------------------------
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#define	override
#endif
//---------------------------------------------------------------------------
namespace cnLibrary{
//- file name ---------------------------------------------------------------
class cnLib_INTERFACE iWin32FileName : public iFileName
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface<iFileName>(this,ID);	}

	virtual const wchar_t*	cnLib_FUNC GetFileName(void)=0;
	virtual uIntn			cnLib_FUNC GetFileNameLength(void)=0;
	virtual void			cnLib_FUNC SetFileName(const wchar_t *FileName,uIntn Length)=0;
};
//- system handle -----------------------------------------------------------
class cnLib_INTERFACE iWinFileHandle : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual HANDLE cnLib_FUNC GetFileHandle(void)=0;
};
//---------------------------------------------------------------------------
namespace cnWindows{
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt16,SystemFile)
{
	Execuable,
	ApplicationFolder,
	DocumentFolder,
	TempFolder,
}cnLib_ENUM_END(SystemFile);
//---------------------------------------------------------------------------
iPtr<iWin32FileName> cnLib_FUNC CreateFileName(const wchar_t *FileName);
iPtr<iWin32FileName> cnLib_FUNC CreateTemporaryFileName(iFileName *FolderName,const wchar_t *Prefix=0);
iPtr<iWin32FileName> cnLib_FUNC GetSystemFileName(eSystemFile File);

//---------------------------------------------------------------------------
iPtr<iStream> cnLib_FUNC OpenDeviceStream(const wchar_t *DeviceName,eFileAccess AccessMode=FileAccess::Read|FileAccess::Write,eFileCreate CreateMode=FileCreate::Open);
iPtr<iEndpoint> cnLib_FUNC OpenDeviceEndpoint(const wchar_t *DeviceName,eFileAccess AccessMode=FileAccess::Read|FileAccess::Write,eFileCreate CreateMode=FileCreate::Open);

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
