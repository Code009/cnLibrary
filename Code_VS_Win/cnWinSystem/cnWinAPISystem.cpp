#include "cnWinAPISystem.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace cnWin; 


//---------------------------------------------------------------------------

//rPtr<iAsyncTimer> WindowsInterface::DefaultThreadPoolCreateTimer(iReference *Reference,iProcedure *Procedure)
//{
//	return vcDefaultThreadPool::gInstance.CreateTimer(Reference,Procedure);
//}
//
//rPtr<iThreadPoolHandleWaiter> WindowsInterface::DefaultThreadPoolCreateHandleWaiter(iReference *Reference,iFunction<void (DWORD)> *Callback)
//{
//	return vcDefaultThreadPool::gInstance.CreateHandleWaiter(Reference,Callback);
//}
//
//rPtr<iThreadPoolHandleRepeatWaiter> WindowsInterface::DefaultThreadPoolCreateHandleRepeatWaiter(iReference *Reference,iProcedure *Procedure)
//{
//	return vcDefaultThreadPool::gInstance.CreateHandleRepeatWaiter(Reference,Procedure);
//}

//---------------------------------------------------------------------------
//	cnSystem
//---------------------------------------------------------------------------

void cnSystem::AssertionMessage(const char *Message)noexcept
{
	if(::MessageBoxA(nullptr,Message,"assert",MB_ICONERROR|MB_YESNO)==IDYES){
		__debugbreak();
	}
}
//- Default Heap ------------------------------------------------------------

void* cnSystem::DefaultHeap::Alloc(uIntn Alignment,uIntn Size)noexcept
{
	return gDefaultMemoryHeap.Alloc(Alignment,Size);
}
bool cnSystem::DefaultHeap::Free(void *Pointer,uIntn Size,uIntn Alignment)noexcept
{
	return gDefaultMemoryHeap.Free(Pointer,Size,Alignment);
}
bool cnSystem::DefaultHeap::Resize(void *Pointer,uIntn Size,uIntn NewSize)noexcept
{
	return gDefaultMemoryHeap.Resize(Pointer,Size,NewSize);
}
void* cnSystem::DefaultHeap::Relocate(void *Pointer,uIntn Size,uIntn Alignment,uIntn NewSize,uIntn NewAlignment,bool &ManualCopy)noexcept
{
	return gDefaultMemoryHeap.Relocate(Pointer,Size,NewAlignment,NewSize,NewAlignment,ManualCopy);
}
uIntn cnSystem::DefaultHeap::SizeOf(void *Pointer)noexcept
{
	return gDefaultMemoryHeap.SizeOf(Pointer);
}

//- Time --------------------------------------------------------------------

uInt64 cnSystem::GetSystemTimeNow(void)noexcept
{
	cNTFileTime ft;
	ft.SetNow();
	return ft.ToSystemTime();
}

bool cnSystem::UTCGregorianDateFromSystemTime(cDateTime &DateTime,uInt64 SystemTime)noexcept
{
	cNTFileTime ft;
	ft.FromSystemTime(SystemTime);
	return ft.ToDateTime(DateTime);
}
bool cnSystem::UTCGregorianDateToSystemTime(uInt64 &SystemTime,const cDateTime &DateTime)noexcept
{
	cNTFileTime ft;
	if(ft.FromDateTime(DateTime)==false)
		return false;
	SystemTime=ft.ToSystemTime();
	return true;
}
//- Thread sync -------------------------------------------------------------
rPtr<iMutex> cnSystem::CreateMutexLock(void)noexcept
{
	return rCreate<cnRTL::impMutex<cCriticalSection>>();
}
//- Thread sync -------------------------------------------------------------
rPtr<iMutex> cnSystem::CreateRecursiveLock(void)noexcept
{
	return rCreate<cnRTL::impMutex<cCriticalSection>>();
}
//- Thread sync -------------------------------------------------------------
rPtr<iSharedMutex> cnSystem::CreateSharedLock(void)noexcept
{
	return rCreate<cnRTL::impSharedMutex<vcSRWLock>>();
}
//- Thread sync -------------------------------------------------------------
rPtr<iThreadLocalVariable> cnSystem::CreateThreadLocalVariable(void)noexcept
{
	return rCreate<cNT6ThreadLocalVariable>();
}
//- Thread ------------------------------------------------------------------
iPtr<iThread> cnSystem::StartThread(iProcedure *ThreadProcedure)noexcept
{
	return cThread::StartThread(ThreadProcedure);
}
//- Thread ------------------------------------------------------------------

iPtr<iDispatch> cnSystem::CreateDispatchThread(void)noexcept
{
	return cnWin::CreateWindowMessageDispathThread();
}

//- Current Thread ----------------------------------------------------------
void cnSystem::CurrentThread::SwitchThread(void)noexcept
{	::SwitchToThread();	}

void cnSystem::CurrentThread::SuspendFor(uInt64 Delay)noexcept
{	return ::Sleep(Delay/Time_1ms);	}
bool cnSystem::CurrentThread::SleepUntil(uInt64 SystemTime)noexcept
{	return CurrentThreadSleepUntil(SystemTime);	}

iThread* cnSystem::CurrentThread::GetThread(void)noexcept
{	return cThread::QueryCurrent();	}

bool cnSystem::CurrentThread::SetPriority(sInt8 Priority)noexcept
{	return cThreadHandle::SetPriority(gCurrentThreadHandle,Priority);	}

sInt8 cnSystem::CurrentThread::GetPriority(void)noexcept
{	return cThreadHandle::GetPriority(gCurrentThreadHandle);	}

//- Thread Pool -------------------------------------------------------------


//- File System -------------------------------------------------------------

rPtr<iSwapMemory> cnSystem::CreateSwapMemoryFromFile(iFile *File,uInt64 Size,eFileAccess AccessMode)noexcept
{
	return cnWin::CreateSwapMemoryFromFile(File,Size,AccessMode);
}

//- Network -----------------------------------------------------------------

iPtr<iIPv4Address> cnSystem::CreateAddressIPv4(uInt32 IP,uInt16 Port)noexcept
{
	return Win32CreateAddressIPv4(IP,Port);
}

iPtr<iSocketAddress> cnSystem::CreateSocketAddress(const sockaddr *addr,socklen_t addrlen)noexcept
{
	return Win32CreateCopySocketAddress(addr,addrlen);
}

//---------------------------------------------------------------------------
// UI
//---------------------------------------------------------------------------

#if 0

iPtr<iUIView> cnSystem::CreateUIView(void)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - CreateUIView")
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iPopupWindowControl> cnSystem::CreatePopupWindowControl(void)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - iPopupWindowControl")
	return nullptr;
}
//---------------------------------------------------------------------------
iUIThread* cnSystem::CurrentUIThread::GetUIThread(void)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - iUIThread")
	return nullptr;
}
//---------------------------------------------------------------------------
iDispatch* cnSystem::CurrentUIThread::GetDispatch(bool HighPriority)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - UIThread - iDispatch")
	return nullptr;
}
//---------------------------------------------------------------------------
iUIKeyboard* cnSystem::CurrentUIThread::GetDefaultKeyboard(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
iUIMouse* cnSystem::CurrentUIThread::GetDefaultMouse(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
iInterface* cnLib_FUNC cnSystem::GetSysMouseCursor(eSysMouseCursor CursorID)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - GetSysMouseCursor")
	/*switch(CursorID){
	case SysMouseCursor::Normal:
		return &gMouseCursor_Normal;
	case SysMouseCursor::Busy:
		return &gMouseCursor_Busy;
	case SysMouseCursor::BackgroundBusy:
		return &gMouseCursor_BackgroundBusy;
	case SysMouseCursor::TextEdit:
		return &gMouseCursor_TextEdit;
	case SysMouseCursor::SizeA:
		return &gMouseCursor_SizeA;
	case SysMouseCursor::SizeH:
		return &gMouseCursor_SizeH;
	case SysMouseCursor::SizeV:
		return &gMouseCursor_SizeV;
	case SysMouseCursor::SizeFS:
		return &gMouseCursor_SizeFS;
	case SysMouseCursor::SizeBS:
		return &gMouseCursor_SizeBS;
	}*/
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iClipboardReader> cnSystem::QueryClipboardReader(iUIView *View)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - iClipboardReader")
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iClipboardWriter> cnSystem::QueryClipboardWriter(iUIView *View)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - iClipboardWriter")
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iUIFont> cnSystem::QueryFont(const uChar16 *Name,uIntn NameLength,eUIFontStyle FontStyle,Float32 FontWeight)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - QueryFont")
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iUITextLineLayout> cnSystem::CreateTextLineLayout(const uChar16 *Text,uIntn Length,const cUITextStyle &Style)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - CreateTextLineLayout")
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> cnSystem::CreateBitmapSourceFromFile(iFileName *FileName)noexcept
{
	auto WinFileName=iCast<iWin32FileName>(FileName);
	if(WinFileName==nullptr)
		return nullptr;


	auto Bitmap=WICOpenImageFile(WinFileName->GetFileName());
	if(Bitmap==nullptr)
		return nullptr;
	return iCreate<cWICBitmapDataSource>(Bitmap);
}
//---------------------------------------------------------------------------
rPtr<iUISimplePaintDevice> cnSystem::QueryUISimplePaintDevice(iUIView *ForView)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - QueryUISimplePaintDevice")
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iUISimpleViewContent> cnSystem::CreateUISimpleViewContent(iUISimplePainter *Painter)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - CreateUISimpleViewContent")
	return nullptr;
}

#endif // 0

//---------------------------------------------------------------------------
//	Audio
//---------------------------------------------------------------------------
iPtr<iInterface> cnSystem::CreateAudioFormat_PCM(uInt32 Freq,uInt8 bits,uInt8 Channels)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - CreateAudioFormat_PCM")
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iAudioDeviceWatcher>	cnSystem::QueryAudioDefaultInputDeviceWatcher(void)noexcept
{
	return vnAudioDevice::QueryAudioDefaultInputDeviceWatcher();
}
//---------------------------------------------------------------------------
rPtr<iAudioDeviceWatcher>	cnSystem::QueryAudioDefaultOutputDeviceWatcher(void)noexcept
{
	return vnAudioDevice::QueryAudioDefaultOutputDeviceWatcher();
}
//---------------------------------------------------------------------------
iPtr<iAudioDevice>			cnSystem::QueryAudioMainDevice(void)noexcept
{
	return vnAudioDevice::QueryAudioMainDevice();
}

//---------------------------------------------------------------------------
//	Console
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//	cnWindows
//---------------------------------------------------------------------------
static LONG cnWindowsInitializeCount=0;
//---------------------------------------------------------------------------
ufInt8 cnLib_FUNC cnWindows::Initialize(void)noexcept
{
	if(::_InterlockedExchangeAdd(&cnWindowsInitializeCount,1)==0){
		// initialize
		cnWin::CPPInitialize();
	}
	// always success
	return static_cast<uIntn>(LibLoadResult::Success);
}
//---------------------------------------------------------------------------
void cnLib_FUNC cnWindows::Finalize(void)noexcept
{
	if(cnWindowsInitializeCount==0)
		return;
	if(::_InterlockedExchangeAdd(&cnWindowsInitializeCount,-1)!=1)
		return;
	gSystemDependentRegistration.Shutdown();
	cnWin::CPPFinalize();
}
//---------------------------------------------------------------------------
rPtr<iTextEncodingConverter> cnWindows::CodePageToUTF16(UINT SrcCodePage)noexcept
{
	auto Converter=rCreate<cTextEncodingConverter_MultiByteToUnicode>();
	Converter->SrcCodePage=SrcCodePage;
	return Converter;
}
rPtr<iTextEncodingConverter> cnWindows::CodePageFromUTF16(UINT DestCodePage)noexcept
{
	auto Converter=rCreate<cTextEncodingConverter_UnicodeToMultiByte>();
	Converter->DestCodePage=DestCodePage;
	return Converter;
}


//---------------------------------------------------------------------------

rPtr<iCOMApartmentThreading> cnWindows::COMApartmentThreadingGet(void)noexcept
{
	return gCOMApartmentThreadingModule.Get();
}

rPtr<iCOMApartmentThreading> cnWindows::COMApartmentThreadingQuery(iDispatch *Dispatch)noexcept
{
	return gCOMApartmentThreadingModule.Query(Dispatch);
}

//- Heap---------------------------------------------------------------------
rPtr<iMemoryHeap> cnWindows::CreateMemoryHeap(void)noexcept
{
	return rCreate<cWinMemoryHeap>(0,0,0);
}
//- Thread pool -------------------------------------------------------------
iPtr<iThreadPool> cnWindows::CreateThreadPool(void)noexcept
{
	return iCreate<vcThreadPool>();
}
//---------------------------------------------------------------------------
iPtr<iWin32FileName> cnWindows::CreateFileName(const wchar_t *Win32FileName)noexcept
{
	return iCreate<cWin32FileName>(Win32FileName);
}
iPtr<iWin32FileName> cnWindows::CreateTemporaryFileName(iFileName *FolderName,const wchar_t *Prefix)noexcept
{
	auto WinFolderName=iCast<iWin32FileName>(FolderName);
	if(WinFolderName==nullptr)
		return nullptr;

	auto TempFileName=Win32FileMakeTemporaryFileName(WinFolderName->GetFileName(),Prefix);
	if(TempFileName->Length==0)
		return nullptr;
	return iCreate<cWin32FileName>(cnVar::MoveCast(TempFileName));
}

iPtr<iWin32FileName> cnWindows::GetSystemFileName(cnWindows::eSystemFile File)noexcept
{
	auto FolderName=WindowsInterface::Win32FileMakeSystemFileName(File);
	if(FolderName->Length==0)
		return nullptr;
	return iCreate<cWin32FileName>(FolderName.Swap());
}

iPtr<iStream> cnWindows::OpenDeviceStream(const wchar_t *DeviceName,eFileAccess AccessMode,eFileCreate CreateMode)noexcept
{
	return WindowsInterface::Win32FileOpenDeviceStream(DeviceName,AccessMode,CreateMode);
}

iPtr<iEndpoint> cnWindows::OpenDeviceEndpoint(const wchar_t *DeviceName,eFileAccess AccessMode,eFileCreate CreateMode)noexcept
{
	return WindowsInterface::Win32FileOpenDeviceEndpoint(DeviceName,AccessMode,CreateMode);
}

//---------------------------------------------------------------------------
//- Database ----------------------------------------------------------------
//---------------------------------------------------------------------------

rPtr<iODBC> cnWindows::ODBC(void)noexcept
{
	return rCreate<cODBC>();
}

//---------------------------------------------------------------------------
//- UI ----------------------------------------------------------------------
//---------------------------------------------------------------------------

rPtr<iWindowsConsoleHost> cnWindows::CreateWindowsConsoleHost(iDispatch *Dispatch)noexcept
{
	return cnWinRTL::CreateWindowsConsoleHost(Dispatch);
}

#if 0

iWindow* cnWindows::GetWindowFromHandle(HWND WindowHandle)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - GetWindowHostFromWindowHandle")
	return nullptr;
}

rPtr<iWindowsUIApplication> cnWindows::CreateWindowsUIApplication(void)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - CreateWindowsUIApplication")
	return nullptr;
}

iPtr<iUIThread> cnWindows::CreateUIThreadOnCurrentThread(void)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - CreateUIThreadOnCurrentThread")
	return nullptr;
}
iPtr<iUIThread> cnWindows::StartUIThread(void)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - StartUIThread")
	return nullptr;
}

iPtr<iWindowProvider> cnWindows::CreateWindowProvider(void)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - rpCreateWindowProvider")
	return nullptr;
}

iPtr<iWindowClient> cnWindows::CreateWindowClient(void)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - CreateWindowClient")
	return nullptr;
}

iPtr<iWindowFrame> cnWindows::CreateWindowFrame(void)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - rpCreateWindowFrameComponent")
	return nullptr;
}

iPtr<iOwnerFocusWindowClient> cnWindows::CreateOwnerFocusWindowClient(void)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - rpCreateOwnerFocusWindowClientComponent")
	return nullptr;
}
#endif // 0

iPtr<iWindow> cnWindows::CreateWindowHandle(HWND Parent,const wchar_t *WindowText,DWORD Style,DWORD ExStyle,LONG X,LONG Y,LONG Width,LONG Height,UINT ChildID)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - CreateWindowHandle")
	return nullptr;
}

//---------------------------------------------------------------------------
//- Device Context ----------------------------------------------------------
//---------------------------------------------------------------------------

rPtr<iDCPaintDevice> cnWindows::QueryDCPaintDevice(void)noexcept
{
	return nullptr;
}

rPtr<iDCViewContent> cnWindows::CreateDCViewContent(iDCPainter *Painter,uIntn DCViewContentOptions)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - CreateDCViewContent")
	return nullptr;
//	bool OutputAlpha=(0!=(DCViewContentOptions&DCViewContentOptions_OutputAlpha));
//	return cnWin::DNetCreateDCViewContent(OutputAlpha);
}

rPtr<iDXGIViewContent> cnWindows::CreateDXGIViewContent(iDXGIPainter *Painter)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - rpCreateDXGIViewContent")
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	return nullptr;
	//return rCreateRet<iDXGIViewContent,cnWinNT6::cDXGIViewContent>();
#else
	return nullptr;
#endif	// _WIN32_WINNT_VISTA
}


//---------------------------------------------------------------------------
