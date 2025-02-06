#include "Win_Common.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

// surpress constructor and destructor
cnVar::cStaticVariable< cnWinRTL::cWinLibraryReference<cWindowsLibraryInitialization> > cnWin::gWindowsSystemReference;

//- System Info -------------------------------------------------------------
cSysInfo::cSysInfo()
{
	::GetSystemInfo(this);
}
//- System Info - Startup Info ----------------------------------------------
//cStartupInfo::cStartupInfo()
//{
//	::GetStartupInfoW(this);
//}
//---------------------------------------------------------------------------
DWORD cnWin::GetMemPageSize(void)
{
	return gSysInfo.dwPageSize;
}
//---------------------------------------------------------------------------
DWORD cnWin::GetMemAllocationGranularity(void)
{
	return gSysInfo.dwAllocationGranularity;
}
//---------------------------------------------------------------------------
BOOL cnWin::DuplicateCurrentThreadHandle(HANDLE &OutHandle,DWORD Access,BOOL InheritHandle)
{
	cnLib_ASSERT(&OutHandle!=nullptr);

	return DuplicateHandle(gCurrentProcessHandle,gCurrentThreadHandle,gCurrentProcessHandle,&OutHandle,Access,InheritHandle,0);
}
BOOL cnWin::DuplicateCurrentThreadHandleSameAccess(HANDLE &OutHandle,BOOL InheritHandle)
{
	cnLib_ASSERT(&OutHandle!=nullptr);

	return DuplicateHandle(gCurrentProcessHandle,gCurrentThreadHandle,gCurrentProcessHandle,&OutHandle,0,InheritHandle,DUPLICATE_SAME_ACCESS);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cWindowsLibraryInitialization::Initialize(void)noexcept(true)
{
	WindowsInterface::SystemInitialize();
}
//---------------------------------------------------------------------------
void cWindowsLibraryInitialization::Finalize(void)noexcept(true)
{
	WindowsInterface::SystemFinalize();
}
//---------------------------------------------------------------------------
