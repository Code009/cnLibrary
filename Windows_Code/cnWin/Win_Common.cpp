#include "Win_Common.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

// surpress constructor and destructor
cnVar::cStaticVariable< cnWinRTL::cWinModuleManager<cWindowsSystemInitialization> > cnWin::gWindowsSystemManager;

//- System Info -------------------------------------------------------------
cSysInfo::cSysInfo()noexcept(true)
{
	::GetSystemInfo(this);
}
//- System Info - Startup Info ----------------------------------------------
//cStartupInfo::cStartupInfo()noexcept(true)
//{
//	::GetStartupInfoW(this);
//}
//---------------------------------------------------------------------------
DWORD cnWin::GetMemPageSize(void)noexcept(true)
{
	return gSysInfo.dwPageSize;
}
//---------------------------------------------------------------------------
DWORD cnWin::GetMemAllocationGranularity(void)noexcept(true)
{
	return gSysInfo.dwAllocationGranularity;
}
//---------------------------------------------------------------------------
BOOL cnWin::DuplicateCurrentThreadHandle(HANDLE &OutHandle,DWORD Access,BOOL InheritHandle)noexcept(true)
{
	cnLib_ASSERT(&OutHandle!=nullptr);

	return DuplicateHandle(gCurrentProcessHandle,gCurrentThreadHandle,gCurrentProcessHandle,&OutHandle,Access,InheritHandle,0);
}
BOOL cnWin::DuplicateCurrentThreadHandleSameAccess(HANDLE &OutHandle,BOOL InheritHandle)noexcept(true)
{
	cnLib_ASSERT(&OutHandle!=nullptr);

	return DuplicateHandle(gCurrentProcessHandle,gCurrentThreadHandle,gCurrentProcessHandle,&OutHandle,0,InheritHandle,DUPLICATE_SAME_ACCESS);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cWindowsSystemInitialization::Initialize(void)noexcept(true)
{
	WindowsInterface::SystemInitialize();
}
//---------------------------------------------------------------------------
void cWindowsSystemInitialization::Finalize(void)noexcept(true)
{
	WindowsInterface::SystemFinalize();
}
//---------------------------------------------------------------------------
