#include <intrin.h>
#include "cnWinSystem.h"

using namespace cnLibrary;
using namespace cnWin;

typedef void (__cdecl *CPPInitProc)(void);

#pragma section("cnWin_INIT$0",read)
__declspec(allocate("cnWin_INIT$0"))	const CPPInitProc cnWin_InitSegS= (CPPInitProc)0;
#pragma section("cnWin_INIT$2",read)
__declspec(allocate("cnWin_INIT$2"))	const CPPInitProc cnWin_InitSegE= (CPPInitProc)0;

#pragma comment(linker, "/MERGE:cnWin_INIT=.rdata")

static const ufInt8 cnWinExitFunctionCapacity=64;
static CPPInitProc cnWin_ExitFunctions[cnWinExitFunctionCapacity];

static ufInt8 cnWin_ExitFunctionCount=0;

//---------------------------------------------------------------------------

// windows interface definitions

//---------------------------------------------------------------------------
//- Initialization ----------------------------------------------------------
//---------------------------------------------------------------------------

void	WindowsInterface::SystemInitialize(void)noexcept(true)
{
	const CPPInitProc* Proc=&cnWin_InitSegS+1;
	while(Proc<&cnWin_InitSegE){
		if((*Proc)!=nullptr){
			(*Proc)();
		}
		Proc++;
	}
}
void	WindowsInterface::SystemFinalize(void)noexcept(true)
{
	while(cnWin_ExitFunctionCount!=0){
		cnWin_ExitFunctionCount--;
		(*cnWin_ExitFunctions[cnWin_ExitFunctionCount])();
	}
}

int cnWin::Initialization_AtExit(void (__cdecl *Proc)(void))
{
	if(cnWin_ExitFunctionCount>=cnWinExitFunctionCapacity){
		// not enough function capacity;
		__debugbreak();
		return -1;
	}
	cnWin_ExitFunctions[cnWin_ExitFunctionCount]=Proc;
	cnWin_ExitFunctionCount++;
	return 0;

}
//---------------------------------------------------------------------------
//- File System -------------------------------------------------------------
//---------------------------------------------------------------------------

iPtr<iFileStream>		WindowsInterface::Win32FileOpenFileStream(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)
{
	return vnWin32FileSystem::OpenFileStream(FileName,AccessMode,CreateFlag);
}

iPtr<iStream>			WindowsInterface::Win32FileOpenDeviceStream(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)
{
	return vnWin32FileSystem::OpenDeviceStream(FileName,AccessMode,CreateFlag);
}

iPtr<iEndpoint>			WindowsInterface::Win32FileOpenDeviceEndpoint(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)
{
	return vnWin32FileSystem::OpenDeviceEndpoint(FileName,AccessMode,CreateFlag);
}

cnRTL::cStringBuffer<wchar_t>	WindowsInterface::Win32FileMakeSystemFileName(cnWindows::eSystemFile File)
{
	return vnWin32FileSystem::MakeSystemFileName(File);
}

rPtr<iAsyncNotification>	WindowsInterface::Win32FileOpenFileWatcher(const wchar_t *FolderName)
{
	return vnWin32FileSystem::OpenFileWatcher(FolderName);
}

rPtr<iFileObserver>			WindowsInterface::Win32FileOpenFileObserver(cnRTL::cString<wchar_t> FolderName)
{
	return vnWin32FileSystem::OpenFileObserver(cnVar::MoveCast(FolderName));
}

//---------------------------------------------------------------------------
//- Window Subclass ---------------------------------------------------------
//---------------------------------------------------------------------------

bool WindowsInterface::WindowLocalSubclass(HWND WindowHandle,bcWindowSubclass *Subclass)
{
	return vnWindowSubclass::SetLocal(WindowHandle,Subclass);
}
bool WindowsInterface::WindowSubclass(HWND WindowHandle,bcWindowSubclass *Subclass)
{
	return vnWindowSubclass::SetLocal(WindowHandle,Subclass);
}

void WindowsInterface::WindowSubclassRestore(HWND WindowHandle,bcWindowSubclass *Subclass)
{
	return vnWindowSubclass::Restore(WindowHandle,Subclass);
}

//---------------------------------------------------------------------------
