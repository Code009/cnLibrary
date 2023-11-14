#include "stdafx.h"

#include "cnSc009WinSystemCommon.h"


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace Sc009;
using namespace cnWin; 


//---------------------------------------------------------------------------
//	cnSystem
//---------------------------------------------------------------------------

iDependentRegistration* cnSystem::GetSystemDependentRegistration(void)
{
	return &gSystemDependentRegistration;
}

//---------------------------------------------------------------------------
//- Thread Pool -------------------------------------------------------------
//---------------------------------------------------------------------------

rPtr<iAsyncTimer> WindowsInterface::DefaultThreadPoolCreateTimer(iReference *Reference,iProcedure *Procedure)
{
	return vcDefaultThreadPool::gInstance.CreateTimer(Reference,Procedure);
}

rPtr<iThreadPoolHandleWaiter> WindowsInterface::DefaultThreadPoolCreateHandleWaiter(iReference *Reference,iFunction<void (DWORD)> *Callback)
{
	return vcDefaultThreadPool::gInstance.CreateHandleWaiter(Reference,Callback);
}

//---------------------------------------------------------------------------
//	cnSystem
//---------------------------------------------------------------------------

//- Default Heap ------------------------------------------------------------

//---------------------------------------------------------------------------
void* cnSystem::DefaultHeap::Alloc(uIntn Size,uIntn Alignment)
{
	return gDefaultMemoryHeap.Alloc(Size,Alignment);
}
bool cnSystem::DefaultHeap::Free(void *Pointer,uIntn Size,uIntn Alignment)
{
	return gDefaultMemoryHeap.Free(Pointer,Size,Alignment);
}
bool cnSystem::DefaultHeap::Resize(void *Pointer,uIntn Size,uIntn NewSize)
{
	return gDefaultMemoryHeap.Resize(Pointer,Size,NewSize);
}
void* cnSystem::DefaultHeap::Relocate(void *Pointer,uIntn Size,uIntn Alignment,uIntn NewSize,uIntn NewAlignment,bool &ManualCopy)
{
	return gDefaultMemoryHeap.Relocate(Pointer,Size,Alignment,NewSize,NewAlignment,ManualCopy);
}
uIntn cnSystem::DefaultHeap::SizeOf(void *Pointer)
{
	return gDefaultMemoryHeap.SizeOf(Pointer);
}

//---------------------------------------------------------------------------

// Time

//---------------------------------------------------------------------------
sInt64 cnSystem::GetSystemTimeNow(void)
{
	cNTFileTime ft;
	ft.SetNow();
	return ft.ToSystemTime();
}
//---------------------------------------------------------------------------
bool cnSystem::UTCGregorianDateFromSystemTime(cDateTime &DateTime,sInt64 SystemTime)
{
	cNTFileTime ft;
	ft.FromSystemTime(SystemTime);
	return ft.ToDateTime(DateTime);
}
bool cnSystem::UTCGregorianDateToSystemTime(sInt64 &SystemTime,const cDateTime &DateTime)
{
	cNTFileTime ft;
	if(ft.FromDateTime(DateTime)==false)
		return false;
	SystemTime=ft.ToSystemTime();
	return true;
}
//- Thread sync -------------------------------------------------------------
rPtr<iMutex> cnSystem::CreateMutexLock(void)
{
	return rCreate< impMutex<cCriticalSection> >();
}
//- Thread sync -------------------------------------------------------------
rPtr<iMutex> cnSystem::CreateRecursiveLock(void)
{
	return rCreate< impMutex<cCriticalSection> >();
}
//- Thread sync -------------------------------------------------------------
rPtr<iSharedMutex> cnSystem::CreateSharedLock(void)
{
	return rCreate< impSharedMutex<vcSRWLock> >();
}
//- Thread sync -------------------------------------------------------------
rPtr<iThreadLocalVariable> cnSystem::CreateThreadLocalVariable(void (cnLib_FUNC *Destructor)(void*))
{
	return rCreate<cTLSInterface>(Destructor);
}
//- Thread ------------------------------------------------------------------
iPtr<iThread> cnSystem::StartThread(iProcedure *ThreadProcedure)
{
	return cThread::StartThread(ThreadProcedure);
}
//- Thread ------------------------------------------------------------------
iPtr<iDispatch> cnSystem::CreateDispatchThread(void)
{
	return cWinSc009ThreadDispatch::StartDispatchThread();
}
//- Current Thread ----------------------------------------------------------
void cnSystem::CurrentThread::SwitchThread(void)
{	::SwitchToThread();	}

bool cnSystem::CurrentThread::SuspendFor(uInt64 Delay)
{	return CurrentThreadSuspendFor(Delay);	}
bool cnSystem::CurrentThread::SleepUntil(iTimepoint *Time,sInt64 Delay)
{	return CurrentThreadSleepUntil(Time,Delay);	}

iThread* cnSystem::CurrentThread::GetThread(void)
{	return cThread::QueryCurrent();	}

bool cnSystem::CurrentThread::SetPriority(sInt8 Priority)
{	return cThreadHandle::SetPriority(gCurrentThreadHandle,Priority);	}

sInt8 cnSystem::CurrentThread::GetPriority(void)
{	return cThreadHandle::GetPriority(gCurrentThreadHandle);	}


//- File System -------------------------------------------------------------

rPtr<iSwapMemory> cnSystem::CreateSwapMemoryFromFile(iFile *File,uInt64 Size,eFileAccess AccessMode)
{
	return cnWin::CreateSwapMemoryFromFile(File,Size,AccessMode);
}

//- Network -----------------------------------------------------------------

iPtr<iIPv4Address> cnSystem::CreateAddressIPv4(uInt32 IP,uInt16 Port)
{
	return Win32CreateAddressIPv4(IP,Port);
}

iPtr<iSocketAddress> cnSystem::CreateSocketAddress(const sockaddr *addr,socklen_t addrlen)
{
	return Win32CreateCopySocketAddress(addr,addrlen);
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// UI
//---------------------------------------------------------------------------
iPtr<iUIView> cnSystem::CreateUIView(void)
{
	auto CurrentThread=cUIThread::CurrentUIThread();
	if(CurrentThread==nullptr)
		return nullptr;
	return iCreate<cWinViewport>(CurrentThread);
}
//---------------------------------------------------------------------------
rPtr<iPopupWindowControl> cnSystem::CreatePopupWindowControl(void)
{

	auto CurrentThread=cUIThread::CurrentUIThread();
	if(CurrentThread==nullptr)
		return nullptr;
	return rCreate<cPopupWindowControl>(CurrentThread);
}
//---------------------------------------------------------------------------
iUIThread* cnSystem::CurrentUIThread::GetUIThread(void)
{
	return cUIThread::CurrentUIThread();
}
//---------------------------------------------------------------------------
iDispatch* cnSystem::CurrentUIThread::GetDispatch(bool HighPriority)
{
	auto UIThread=cUIThread::CurrentUIThread();
	return UIThread->GetDispatch(HighPriority);
}
//---------------------------------------------------------------------------
iUIKeyboard* cnSystem::CurrentUIThread::GetDefaultKeyboard(void)
{
	return &gWindowsUIModule.Keyboard;
}
//---------------------------------------------------------------------------
iUIMouse* cnSystem::CurrentUIThread::GetDefaultMouse(void)
{
	return &gWindowsUIModule.Mouse;
}
//---------------------------------------------------------------------------
iInterface* cnSystem::GetSysMouseCursor(eSysMouseCursor CursorID)
{
	switch(CursorID){
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
	}
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iClipboardReader> cnSystem::QueryClipboardReader(iUIView *View)
{
	return cnWin::QueryClipboardReader(View);
}
//---------------------------------------------------------------------------
rPtr<iClipboardWriter> cnSystem::QueryClipboardWriter(iUIView *View)
{
	return cnWin::QueryClipboardWriter(View);
}
//---------------------------------------------------------------------------
iPtr<iUIFont> cnSystem::QueryFont(const uChar16 *Name,uIntn NameLength,eUIFontStyle FontStyle,Float32 FontWeight)
{
	auto Font=iCreate<cUIFontInfo>();
	Font->SetFontInfo(cString<uChar16>(Name,NameLength),FontStyle,FontWeight);
	return Font;
}
//---------------------------------------------------------------------------
rPtr<iUITextLineLayout> cnSystem::CreateTextLineLayout(const uChar16 *Text,uIntn Length,const cUITextStyle &Style)
{
	auto Layout=rCreate<cGDITextLayout>();
	auto Device=gGDIPaintEngine.GetDCPaintDevice();
	auto ScreenIC=Device->GetScreenICHandle();
	Layout->SetupText(ScreenIC,Text,Length,Style);
	return Layout;
}
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> cnSystem::CreateBitmapSourceFromFile(iFileName *FileName)
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
rPtr<iUISimplePaintDevice> cnSystem::QueryUISimplePaintDevice(iUIView *ForView)
{
	auto ViewWindow=cnWinRTL::GetWindowFromUIView(ForView);
	if(ViewWindow!=nullptr){
		auto Window=iCast<iSc009Window>(ViewWindow);
		if(Window!=nullptr){
			auto PaintEngine=Window->GetPaintEngine();
			if(PaintEngine!=nullptr){
				return PaintEngine->QuerySimplePaintDevice();
			}
		}
		// fail to get paint engine from view
	}
	auto UIThread=cUIThread::CurrentUIThread();
	if(UIThread!=nullptr){
		// get thread default paint engine
		return UIThread->DefaultPaintEngine->QuerySimplePaintDevice();
	}
	// gdi paint engine as last resort
	return gGDIPaintEngine.GetSimplePaintDevice();
}
//---------------------------------------------------------------------------
rPtr<iUISimpleViewContent> cnSystem::CreateUISimpleViewContent(void)
{
	auto CurrentThread=cUIThread::CurrentUIThread();
	if(CurrentThread==nullptr)
		return nullptr;
	return rCreate<Sc009::cUIViewportUISimpleViewContent>(CurrentThread);
}

//---------------------------------------------------------------------------
//	Audio
//---------------------------------------------------------------------------
iPtr<iInterface> cnSystem::CreateAudioFormat_PCM(uInt32 Freq,uInt8 bits,uInt8 Channels)
{
	auto Format=iCreate<cWaveFormatEx>();
	WAVEFORMATEX *wf=Format->Edit();
	wf->wFormatTag=WAVE_FORMAT_PCM;
	wf->nChannels=Channels;
	wf->wBitsPerSample=bits;
	wf->nSamplesPerSec=Freq;
	wf->nBlockAlign=Channels*bits/8;
	wf->nAvgBytesPerSec=Freq*wf->nBlockAlign;
	wf->cbSize=0;
	return Format;
}
//---------------------------------------------------------------------------
rPtr<iAudioDeviceWatcher>	cnSystem::QueryAudioDefaultInputDeviceWatcher(void)
{
	return vnAudioDevice::QueryAudioDefaultInputDeviceWatcher();
}
//---------------------------------------------------------------------------
rPtr<iAudioDeviceWatcher>	cnSystem::QueryAudioDefaultOutputDeviceWatcher(void)
{
	return vnAudioDevice::QueryAudioDefaultOutputDeviceWatcher();
}
//---------------------------------------------------------------------------
iPtr<iAudioDevice>			cnSystem::QueryAudioMainDevice(void)
{
	return vnAudioDevice::QueryAudioMainDevice();
}
//---------------------------------------------------------------------------
//	cnWindows
//---------------------------------------------------------------------------
static LONG cnWindowsInitializeCount=0;
//---------------------------------------------------------------------------
ufInt8 cnLib_FUNC cnWindows::Initialize(void)
{
	if(::_InterlockedExchangeAdd(&cnWindowsInitializeCount,1)==0){
		// initialize
		cnWin::CPPInitialize();
	}
	// always success
	return LibLoadResult::Success;
}
//---------------------------------------------------------------------------
void cnLib_FUNC cnWindows::Finalize(void)
{
	if(cnWindowsInitializeCount==0)
		return;
	if(::_InterlockedExchangeAdd(&cnWindowsInitializeCount,-1)!=1)
		return;
	gSystemDependentRegistration.Shutdown();
	cnWin::CPPFinalize();
}
//---------------------------------------------------------------------------
rPtr<iTextEncodingConverter> cnWindows::CodePageToUTF16(UINT SrcCodePage)
{
	auto Converter=rCreate<cTextEncodingConverter_MultiByteToUnicode>();
	Converter->SrcCodePage=SrcCodePage;
	return Converter;
}
rPtr<iTextEncodingConverter> cnWindows::CodePageFromUTF16(UINT DestCodePage)
{
	auto Converter=rCreate<cTextEncodingConverter_UnicodeToMultiByte>();
	Converter->DestCodePage=DestCodePage;
	return Converter;
}

//---------------------------------------------------------------------------

rPtr<iCOMApartmentThreading> cnWindows::COMApartmentThreadingGet(void)
{
	return gCOMApartmentThreadingModule.Get();
}

rPtr<iCOMApartmentThreading> cnWindows::COMApartmentThreadingQuery(iDispatch *Dispatch)
{
	return gCOMApartmentThreadingModule.Query(Dispatch);
}

//---------------------------------------------------------------------------
rPtr<iMemoryHeap> cnWindows::CreateMemoryHeap(void)
{
	return rCreate<cWinMemoryHeap>(0,0,0);
}
//- Thread pool -------------------------------------------------------------
iPtr<iThreadPool> cnWindows::CreateThreadPool(void)
{
	return iCreate<vcThreadPool>();
}

//- File Name ---------------------------------------------------------------

iPtr<iWin32FileName> cnWindows::CreateFileName(const wchar_t *Win32FileName)
{
	return iCreate<cWin32FileName>(Win32FileName);
}

iPtr<iWin32FileName> cnWindows::CreateTemporaryFileName(iFileName *FolderName,const wchar_t *Prefix)
{
	auto WinFolderName=iCast<iWin32FileName>(FolderName);
	if(WinFolderName==nullptr)
		return nullptr;

	auto TempFileName=Win32FileMakeTemporaryFileName(WinFolderName->GetFileName(),Prefix);
	if(TempFileName->Length==0)
		return nullptr;
	return iCreate<cWin32FileName>(cnVar::MoveCast(TempFileName));
}

iPtr<iWin32FileName> cnWindows::GetSystemFileName(cnWindows::eSystemFile File)
{
	auto FolderName=WindowsInterface::Win32FileMakeSystemFileName(File);
	if(FolderName->Length==0)
		return nullptr;
	return iCreate<cWin32FileName>(FolderName.Swap());
}

iPtr<iStream> cnWindows::OpenDeviceStream(const wchar_t *DeviceName,eFileAccess AccessMode,eFileCreate CreateFlag)
{
	return WindowsInterface::Win32FileOpenDeviceStream(DeviceName,AccessMode,CreateFlag);
}

iPtr<iEndpoint> cnWindows::OpenDeviceEndpoint(const wchar_t *DeviceName,eFileAccess AccessMode,eFileCreate CreateFlag)
{
	return WindowsInterface::Win32FileOpenDeviceEndpoint(DeviceName,AccessMode,CreateFlag);
}


//---------------------------------------------------------------------------
//- Database ----------------------------------------------------------------
//---------------------------------------------------------------------------

rPtr<iODBC> cnWindows::ODBC(void)
{
	return rCreate<cODBC>();
}

//---------------------------------------------------------------------------
//- UI ----------------------------------------------------------------------
//---------------------------------------------------------------------------

iWindow* cnWindows::GetWindowFromHandle(HWND WindowHandle)
{
	return bcSc009Window::GetWindowFromHandle(WindowHandle);
}

rPtr<iWindowsUIApplication> cnWindows::CreateWindowsUIApplication(void)
{
	auto UIThread=cUIThread::CreateOnCurrentThread();
	if(UIThread==nullptr)
		return nullptr;
	return rCreate<cWindowsUIApplication>(UIThread);
}

iPtr<iUIThread> cnWindows::CreateUIThreadOnCurrentThread(void)
{
	return cUIThread::CreateOnCurrentThread();
}

iPtr<iUIThread> cnWindows::StartUIThread(void)
{
	return cUIThread::StartUIThread();
}

iPtr<iWindowProvider> cnWindows::CreateWindowProvider(void)
{
	auto CurrentThread=cUIThread::CurrentUIThread();
	if(CurrentThread==nullptr)
		return nullptr;
	return iCreate<cSc009WindowProvider>(CurrentThread);
}
//---------------------------------------------------------------------------
iPtr<iExtWindowProvider> cnWindows::CreateExtWindowProvider(void)
{
	auto CurrentThread=cUIThread::CurrentUIThread();
	if(CurrentThread==nullptr)
		return nullptr;
	return iCreate<cSc009ExtWindowProvider>(CurrentThread);
}
//---------------------------------------------------------------------------
iPtr<iWindowClient> cnWindows::CreateWindowClient(void)
{
	auto CurrentThread=cUIThread::CurrentUIThread();
	if(CurrentThread==nullptr)
		return nullptr;
	return iCreate<cUIWindowSc009WindowClient>(CurrentThread);
}
//---------------------------------------------------------------------------
iPtr<iWindowFrame> cnWindows::CreateWindowFrame(void)
{
	return nullptr;
	//return iCreateRet<iWindowFrame,cViewportWindowFrame>();
}
//---------------------------------------------------------------------------
iPtr<iOwnerFocusWindowClient> cnWindows::CreateOwnerFocusWindowClient(void)
{
	auto CurrentThread=cUIThread::CurrentUIThread();
	if(CurrentThread==nullptr)
		return nullptr;
	return iCreate<cUIWindowSc009OwnerFocusWindowClient>(CurrentThread);
}
//---------------------------------------------------------------------------
iPtr<iWindow> cnWindows::CreateHWND(HWND Parent,const wchar_t *WindowText,DWORD Style,DWORD ExStyle,LONG X,LONG Y,LONG Width,LONG Height,UINT ChildID)
{
	auto UIThread=cUIThread::CurrentUIThread();
	if(UIThread==nullptr)
		return nullptr;
	if((Style&WS_CHILD)==0){
		ChildID=0;
	}
	auto WindowHandle=cnWin::Create_cnWindow(Parent,WindowText,Style,ExStyle,X,Y,Width,Height,reinterpret_cast<HMENU>(static_cast<uIntn>(ChildID)));
	if(WindowHandle==nullptr)
		return nullptr;
	auto WindowHost=iCreate<cSc009Window>(UIThread);
	if(WindowsInterface::WindowLocalSubclass(WindowHandle,WindowHost)){
		return WindowHost;
	}
	::DestroyWindow(WindowHandle);
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iDCPaintDevice> cnWindows::QueryDCPaintDevice(void)
{
	return gGDIPaintEngine.GetDCPaintDevice();
}
//---------------------------------------------------------------------------
rPtr<iDCViewContent> cnWindows::CreateDCViewContent(uIntn DCViewContentOptions)
{
	auto UIThread=cUIThread::CurrentUIThread();
	if(UIThread==nullptr)
		return nullptr;
	return rCreate<cGDIDCViewContent>(UIThread);
}
//---------------------------------------------------------------------------
rPtr<iDXGIViewContent> cnWindows::CreateDXGIViewContent(void)
{
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	auto UIThread=cUIThread::CurrentUIThread();
	if(UIThread==nullptr)
		return nullptr;
	return rCreate<cnWinNT6::cDXGIViewContent>(UIThread);
#else
	return nullptr;
#endif	// _WIN32_WINNT_VISTA
}
//---------------------------------------------------------------------------
// WinSc009
//---------------------------------------------------------------------------
iPtr<iAddress> cnWindows::CreateInterprocessAddress(const wchar_t *Name)
{
	return iCreate<cIPCFileMappingAddress>(Name);
}
//---------------------------------------------------------------------------
