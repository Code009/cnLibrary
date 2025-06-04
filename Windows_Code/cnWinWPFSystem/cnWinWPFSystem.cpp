#include "cnWinWPFSystem.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace cnWin; 

//---------------------------------------------------------------------------
//	cnSystem
//---------------------------------------------------------------------------

iModuleHandle *cnSystem::SystemQueryModule(iModuleReferrer *Referrer)noexcept(true)
{
	return gWindowsSystemManager->QueryHandle(Referrer);
}

void cnSystem::AssertionMessage(const char *Message)noexcept(true)
{
	if(::MessageBoxA(nullptr,Message,"assert",MB_ICONERROR|MB_YESNO)==IDYES){
		__debugbreak();
	}
}

void cnSystem::LogConnectRecorder(iLogRecorder *Recorder)noexcept(true)
{
	return gRTLLog.Connect(Recorder);
}
//- Default Heap ------------------------------------------------------------

void* cnSystem::DefaultHeap::Alloc(uIntn Alignment,uIntn Size)noexcept(true)
{
	return gDefaultMemoryHeap.Alloc(Alignment,Size);
}
bool cnSystem::DefaultHeap::Free(void *Pointer,uIntn Size,uIntn Alignment)noexcept(true)
{
	return gDefaultMemoryHeap.Free(Pointer,Size,Alignment);
}
bool cnSystem::DefaultHeap::Resize(void *Pointer,uIntn Size,uIntn NewSize)noexcept(true)
{
	return gDefaultMemoryHeap.Resize(Pointer,Size,NewSize);
}
void* cnSystem::DefaultHeap::Relocate(void *Pointer,uIntn Size,uIntn Alignment,uIntn NewSize,uIntn NewAlignment,bool &ManualCopy)noexcept(true)
{
	return gDefaultMemoryHeap.Relocate(Pointer,Size,NewAlignment,NewSize,NewAlignment,ManualCopy);
}
uIntn cnSystem::DefaultHeap::SizeOf(void *Pointer)noexcept(true)
{
	return gDefaultMemoryHeap.SizeOf(Pointer);
}

//---------------------------------------------------------------------------
//void cnLib_FUNC cnSystem::DebugLogStreamSetLogLevel(uInt8 Level)
//{
//	gDebugLogStream.SetLogLevel(Level);
//}
// 
// 

//- Time --------------------------------------------------------------------

uInt64 cnSystem::GetSystemTimeNow(void)noexcept(true)
{
	cNTFileTime ft;
	ft.SetNow();
	return ft.ToSystemTime();
}

bool cnSystem::UTCGregorianDateFromSystemTime(cDateTime &DateTime,uInt64 SystemTime)noexcept(true)
{
	cNTFileTime ft;
	ft.FromSystemTime(SystemTime);
	return ft.ToDateTime(DateTime);
}
bool cnSystem::UTCGregorianDateToSystemTime(uInt64 &SystemTime,const cDateTime &DateTime)noexcept(true)
{
	cNTFileTime ft;
	if(ft.FromDateTime(DateTime)==false)
		return false;
	SystemTime=ft.ToSystemTime();
	return true;
}
//- Thread sync -------------------------------------------------------------
rPtr<iMutex> cnSystem::CreateMutexLock(void)noexcept(true)
{
	return rCreate<cnRTL::impMutex<cCriticalSection>>();
}
//- Thread sync -------------------------------------------------------------
rPtr<iMutex> cnSystem::CreateRecursiveLock(void)noexcept(true)
{
	return rCreate<cnRTL::impMutex<cCriticalSection>>();
}
//- Thread sync -------------------------------------------------------------
rPtr<iSharedMutex> cnSystem::CreateSharedLock(void)noexcept(true)
{
	return rCreate<cnRTL::impSharedMutex<vcSRWLock>>();
}
//- Thread sync -------------------------------------------------------------
rPtr<iThreadLocalVariable> cnSystem::CreateThreadLocalVariable(void)noexcept(true)
{
	return rCreate<cNT6ThreadLocalVariable>();
}
//- Thread ------------------------------------------------------------------
iPtr<iThread> cnSystem::StartThread(iProcedure *ThreadProcedure)noexcept(true)
{
	return cThread::StartThread(ThreadProcedure);
}
//- Thread ------------------------------------------------------------------
iPtr<iDispatch> cnSystem::CreateDispatchThread(void)noexcept(true)
{
	return cnWin::CreateWindowMessageDispathThread();
}
//- Current Thread ----------------------------------------------------------
void cnSystem::CurrentThread::SwitchThread(void)noexcept(true)
{	::SwitchToThread();	}

void cnSystem::CurrentThread::SuspendFor(uInt64 Delay)noexcept(true)
{	return ::Sleep(Delay/Time_1ms);	}

iThread* cnSystem::CurrentThread::GetThread(void)noexcept(true)
{	return cThread::QueryCurrent();	}

bool cnSystem::CurrentThread::SetPriority(sInt8 Priority)noexcept(true)
{	return cThreadHandle::SetPriority(gCurrentThreadHandle,Priority);	}

sInt8 cnSystem::CurrentThread::GetPriority(void)noexcept(true)
{	return cThreadHandle::GetPriority(gCurrentThreadHandle);	}

//- Thread Pool -------------------------------------------------------------


//- File System -------------------------------------------------------------

rPtr<iSwapMemory> cnSystem::CreateSwapMemoryFromFile(iFile *File,uInt64 Size,eFileAccess AccessMode)noexcept(true)
{
	return cnWin::CreateSwapMemoryFromFile(File,Size,AccessMode);
}

//- Network -----------------------------------------------------------------

iPtr<iIPv4Address> cnSystem::CreateAddressIPv4(uInt32 IP,uInt16 Port)noexcept(true)
{
	return Win32CreateAddressIPv4(IP,Port);
}

iPtr<iSocketAddress> cnSystem::CreateSocketAddress(const sockaddr *addr,socklen_t addrlen)noexcept(true)
{
	return Win32CreateCopySocketAddress(addr,addrlen);
}

//---------------------------------------------------------------------------
// UI
//---------------------------------------------------------------------------
iPtr<iUIView> cnSystem::CreateUIView(void)noexcept(true)
{
	return WPFCreateUIView();
}
//---------------------------------------------------------------------------
rPtr<iPopupWindowControl> cnSystem::CreatePopupWindowControl(void)noexcept(true)
{
	return WPFCreatePopupWindowControl();
}
//---------------------------------------------------------------------------
iUIThread* cnSystem::CurrentUIThread::GetUIThread(void)noexcept(true)
{
	return cWPFUIThread::CurrentUIThread();
}
//---------------------------------------------------------------------------
iDispatch* cnSystem::CurrentUIThread::GetDispatch(bool HighPriority)noexcept(true)
{
	auto UIThread=cWPFUIThread::CurrentUIThread();
	if(UIThread==nullptr)
		return nullptr;

	return UIThread->GetDispatch(HighPriority);
}
//---------------------------------------------------------------------------
iUIKeyboard* cnSystem::CurrentUIThread::GetDefaultKeyboard(void)noexcept(true)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iUIMouse* cnSystem::CurrentUIThread::GetDefaultMouse(void)noexcept(true)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iInterface* cnLib_FUNC cnSystem::GetSysMouseCursor(eSysMouseCursor CursorID)noexcept(true)
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
rPtr<iClipboardReader> cnSystem::QueryClipboardReader(iUIView *View)noexcept(true)
{
#pragma message (cnLib_FILE_LINE ": TODO - iClipboardReader")
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iClipboardWriter> cnSystem::QueryClipboardWriter(iUIView *View)noexcept(true)
{
#pragma message (cnLib_FILE_LINE ": TODO - iClipboardWriter")
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iUIFont> cnSystem::QueryFont(const uChar16 *Name,uIntn NameLength,eUIFontStyle FontStyle,Float32 FontWeight)noexcept(true)
{
	return cnWin::DNetCreateFont(Name,NameLength,FontStyle,FontWeight);
}
//---------------------------------------------------------------------------
rPtr<iUITextLineLayout> cnSystem::CreateTextLineLayout(const uChar16 *Text,uIntn Length,const cUITextStyle &Style)noexcept(true)
{
	return DNetCreateTextLineLayout(Text,Length,Style);
}
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> cnSystem::CreateBitmapSourceFromFile(iFileName *FileName)noexcept(true)
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
rPtr<iUISimplePaintDevice> cnSystem::QueryUISimplePaintDevice(iUIView *ForView)noexcept(true)
{
#pragma message (cnLib_FILE_LINE ": TODO - QueryUISimplePaintDevice")
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iUISimpleViewContent> cnSystem::CreateUISimpleViewContent(iUISimplePainter *Painter)noexcept(true)
{
	return rCreate<cWPFUISimpleViewContent>(Painter);
}

//---------------------------------------------------------------------------
//	Audio
//---------------------------------------------------------------------------
iPtr<iInterface> cnSystem::CreateAudioFormat_PCM(uInt32 Freq,uInt8 bits,uInt8 Channels)noexcept(true)
{
#pragma message (cnLib_FILE_LINE ": TODO - CreateAudioFormat_PCM")
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iAudioDeviceWatcher>	cnSystem::QueryAudioDefaultInputDeviceWatcher(void)noexcept(true)
{
	return vnAudioDevice::QueryAudioDefaultInputDeviceWatcher();
}
//---------------------------------------------------------------------------
rPtr<iAudioDeviceWatcher>	cnSystem::QueryAudioDefaultOutputDeviceWatcher(void)noexcept(true)
{
	return vnAudioDevice::QueryAudioDefaultOutputDeviceWatcher();
}
//---------------------------------------------------------------------------
iPtr<iAudioDevice>			cnSystem::QueryAudioMainDevice(void)noexcept(true)
{
	return vnAudioDevice::QueryAudioMainDevice();
}

//---------------------------------------------------------------------------
//	Console
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//	cnWindows
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
rPtr<iMutex> cnWindows::QueryLibraryMutex(void)noexcept(true)
{
	return LibraryMutex::Query();
}
//---------------------------------------------------------------------------
rPtr<iTextEncodingConverter> cnWindows::CodePageToUTF16(UINT SrcCodePage)noexcept(true)
{
	auto Converter=rCreate<cTextEncodingConverter_MultiByteToUnicode>();
	Converter->SrcCodePage=SrcCodePage;
	return Converter;
}
rPtr<iTextEncodingConverter> cnWindows::CodePageFromUTF16(UINT DestCodePage)noexcept(true)
{
	auto Converter=rCreate<cTextEncodingConverter_UnicodeToMultiByte>();
	Converter->DestCodePage=DestCodePage;
	return Converter;
}


//---------------------------------------------------------------------------

rPtr<iCOMApartmentThreading> cnWindows::COMApartmentThreadingGet(void)noexcept(true)
{
	return gCOMApartmentThreadingModule.Get();
}

rPtr<iCOMApartmentThreading> cnWindows::COMApartmentThreadingQuery(iDispatch *Dispatch)noexcept(true)
{
	return gCOMApartmentThreadingModule.Query(Dispatch);
}

//- Heap---------------------------------------------------------------------
rPtr<iMemoryHeap> cnWindows::CreateMemoryHeap(void)noexcept(true)
{
	return rCreate<cWinMemoryHeap>(0,0,0);
}
//- Thread pool -------------------------------------------------------------
iPtr<iThreadPool> cnWindows::CreateThreadPool(void)noexcept(true)
{
	return iCreate<vcThreadPool>();
}
//---------------------------------------------------------------------------
iPtr<iWin32FileName> cnWindows::CreateFileName(const wchar_t *Win32FileName)noexcept(true)
{
	return iCreate<cWin32FileName>(Win32FileName);
}
iPtr<iWin32FileName> cnWindows::CreateTemporaryFileName(iFileName *FolderName,const wchar_t *Prefix)noexcept(true)
{
	auto WinFolderName=iCast<iWin32FileName>(FolderName);
	if(WinFolderName==nullptr)
		return nullptr;

	auto TempFileName=Win32FileMakeTemporaryFileName(WinFolderName->GetFileName(),Prefix);
	if(TempFileName->Length==0)
		return nullptr;
	return iCreate<cWin32FileName>(cnVar::MoveCast(TempFileName));
}

iPtr<iWin32FileName> cnWindows::GetSystemFileName(cnWindows::eSystemFile File)noexcept(true)
{
	auto FolderName=WindowsInterface::Win32FileMakeSystemFileName(File);
	if(FolderName->Length==0)
		return nullptr;
	return iCreate<cWin32FileName>(FolderName.Swap());
}

iPtr<iStream> cnWindows::OpenDeviceStream(const wchar_t *DeviceName,eFileAccess AccessMode,eFileCreate CreateMode)noexcept(true)
{
	return WindowsInterface::Win32FileOpenDeviceStream(DeviceName,AccessMode,CreateMode);
}

iPtr<iEndpoint> cnWindows::OpenDeviceEndpoint(const wchar_t *DeviceName,eFileAccess AccessMode,eFileCreate CreateMode)noexcept(true)
{
	return WindowsInterface::Win32FileOpenDeviceEndpoint(DeviceName,AccessMode,CreateMode);
}

//---------------------------------------------------------------------------
//- Database ----------------------------------------------------------------
//---------------------------------------------------------------------------

rPtr<iODBC> cnWindows::ODBC(void)noexcept(true)
{
	return rCreate<cODBC>();
}

//---------------------------------------------------------------------------
//- UI ----------------------------------------------------------------------
//---------------------------------------------------------------------------

rPtr<iWindowsConsoleHost> cnWindows::CreateWindowsConsoleHost(iDispatch *Dispatch)noexcept(true)
{
	return cnWinRTL::CreateWindowsConsoleHost(Dispatch);
}


iWindow* cnWindows::GetWindowFromHandle(HWND WindowHandle)noexcept(true)
{
#pragma message (cnLib_FILE_LINE ": TODO - GetWindowHostFromWindowHandle")
	return nullptr;
}

rPtr<iWindowsUIApplication> cnWindows::CreateWindowsUIApplication(void)noexcept(true)
{
	return cWPFUIApplication::CreateWindowsUIApplication();
}

iPtr<iUIThread> cnWindows::CreateUIThreadOnCurrentThread(void)noexcept(true)
{
	return cWPFUIThread::CreateOnCurrentThread();
}
iPtr<iUIThread> cnWindows::StartUIThread(void)noexcept(true)
{
	return cWPFUIThread::StartUIThread();
}

iPtr<iWindowProvider> cnWindows::CreateWindowProvider(void)noexcept(true)
{
	auto UIThread=cWPFUIThread::CurrentUIThread();
	if(UIThread==nullptr)
		return nullptr;
	return iCreate<cWPFWindow>(UIThread);
}

iPtr<iWindowClient> cnWindows::CreateWindowClient(void)noexcept(true)
{
	return WPFCreateWindowClient();
}

iPtr<iWindowFrame> cnWindows::CreateWindowFrame(void)noexcept(true)
{
	// no frame support
	return nullptr;
}

iPtr<iOwnerFocusWindowClient> cnWindows::CreateOwnerFocusWindowClient(void)noexcept(true)
{
#pragma message (cnLib_FILE_LINE ": TODO - rpCreateOwnerFocusWindowClientComponent")
	return nullptr;
}

iPtr<iWindow> cnWindows::CreateWindowHandle(HWND Parent,const wchar_t *WindowText,DWORD Style,DWORD ExStyle,LONG X,LONG Y,LONG Width,LONG Height,UINT ChildID)noexcept(true)
{
	auto UIThread=cWPFUIThread::CurrentUIThread();
	if(UIThread==nullptr)
		return nullptr;
	auto Window=iCreate<cWPFWindow>(UIThread);
	Style&=~WS_CHILD;
	if(Window->mCreate(Parent,WindowText,Style,ExStyle,X,Y,Width,Height)==false){
		return nullptr;
	}
	return Window;
}

//---------------------------------------------------------------------------
//- Device Context ----------------------------------------------------------
//---------------------------------------------------------------------------

rPtr<iDCPaintDevice> cnWindows::QueryDCPaintDevice(void)noexcept(true)
{
	auto Context=cWPFGDIModule::QueryThreadContext();
	return Context->Device;
}

rPtr<iDCViewContent> cnWindows::CreateDCViewContent(iDCPainter *Painter,uIntn DCViewContentOptions)noexcept(true)
{
	bool OutputAlpha=(0!=(DCViewContentOptions&DCViewContentOptions_OutputAlpha));
	return rCreate<cWPFDCViewContent>(Painter,OutputAlpha);
}

rPtr<iDXGIViewContent> cnWindows::CreateDXGIViewContent(iDXGIPainter *Painter)noexcept(true)
{
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	return rCreate<cWPFDXGIViewContent>(Painter);
#else
	return nullptr;
#endif	// _WIN32_WINNT_VISTA
}
//---------------------------------------------------------------------------
