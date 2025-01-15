/*- cnWin - Interface -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2022-03-17                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\cnRTL.h>
#include <cnRTL\WinRTL.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
	using cnRTL::aPtr;
	using cnRTL::arPtr;
	using cnRTL::aCls;
	using cnRTL::aClsRef;
	using cnRTL::aClsConstRef;
	using cnRTL::aClsWeakRef;
	using cnRTL::arCls;
	using cnRTL::arClsRef;
	using cnRTL::arClsConstRef;
	using cnRTL::arClsWeakRef;

	//---------------------------------------------------------------------------
	using cnRTL::COMPtr;
	using cnRTL::COMInnerPtr;
	using cnRTL::COMInnerObject;

	typedef cnRTL::cnWinRTL::bcWindowSubclass bcWindowSubclass;
//---------------------------------------------------------------------------
namespace WindowsInterface{
//---------------------------------------------------------------------------

// Initiailization

void	SystemInitialize(void)noexcept(true);
void	SystemFinalize(void)noexcept(true);

//---------------------------------------------------------------------------

// FileSystem

//---------------------------------------------------------------------------
cnRTL::cStringBuffer<wchar_t> Win32FileMakeSystemFileName(cnWindows::eSystemFile File);
//---------------------------------------------------------------------------
iPtr<iFileStream>			Win32FileOpenFileStream(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag);
iPtr<iStream>				Win32FileOpenDeviceStream(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag);
iPtr<iEndpoint>				Win32FileOpenDeviceEndpoint(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag);
rPtr<iAsyncNotification>	Win32FileOpenFileWatcher(const wchar_t *FolderName);
rPtr<iFileObserver>			Win32FileOpenFileObserver(cnRTL::cString<wchar_t> FolderName);
//---------------------------------------------------------------------------

// Windows Class

//---------------------------------------------------------------------------
// LocalSubclass
//	subclass a window with the local class
bool WindowLocalSubclass(HWND WindowHandle,bcWindowSubclass *Subclass);
// Subclass
//	subclass a window
bool WindowSubclass(HWND WindowHandle,bcWindowSubclass *Subclass);
// SubclassRestore
//	restore subclassed window
void WindowSubclassRestore(HWND WindowHandle,bcWindowSubclass *Subclass);
//---------------------------------------------------------------------------
}	// namespace WindowsInterface
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
