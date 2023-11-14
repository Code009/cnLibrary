#include "stdafx.h"

#include "cnWinSystem.h"
#include <cnSystem\cnWindows.h>

#include <cnWin\cnWin.h>
#include <cnWinNT5\NT5.h>
//#include <cnWinNT6\NT6.h>
//#include <cnPLWinDLL\cnWinDLL_SysFunc.h>

//#include <cnRTL\Debug.h>

//#include <cnRTL\WinCPPInit.h>

//cnWin_INITSEG_DEFINE(cnWin,64,"cnWin_INIT$0","cnWin_INIT$9")
//#pragma comment(linker, "/MERGE:cnWin_INIT=.rdata")

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin; 

//---------------------------------------------------------------------------
void cnWin::CPPInitialize(void)
{
	//cnWin_Initialize();
}
//---------------------------------------------------------------------------
void cnWin::CPPFinalize(void)
{
	//cnWin_Finalize();
}


//---------------------------------------------------------------------------

rPtr<iAsyncTimer> WindowsInterface::DefaultThreadPoolCreateTimer(iReference *Reference,iProcedure *Procedure)
{
	return vcDefaultThreadPool::gInstance.CreateTimer(Reference,Procedure);
}

rPtr<iThreadPoolHandleWaiter> WindowsInterface::DefaultThreadPoolCreateHandleWaiter(iReference *Reference,iFunction<void (DWORD)> *Callback)
{
	return vcDefaultThreadPool::gInstance.CreateHandleWaiter(Reference,Callback);
}

rPtr<iThreadPoolHandleRepeatWaiter> WindowsInterface::DefaultThreadPoolCreateHandleRepeatWaiter(iReference *Reference,iProcedure *Procedure)
{
	return vcDefaultThreadPool::gInstance.CreateHandleRepeatWaiter(Reference,Procedure);
}

//---------------------------------------------------------------------------

iPtr<iFileStream>		WindowsInterface::Win32FileOpenFileStream(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)
{
	return vWin32FileSystem::OpenFileStream(FileName,AccessMode,CreateFlag);
}

iPtr<iEndpoint>			WindowsInterface::Win32FileOpenFileEndpoint(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)
{
	return vWin32FileSystem::OpenFileEndpoint(FileName,AccessMode,CreateFlag);
}

iPtr<iStream>			WindowsInterface::Win32FileOpenDeviceStream(const wchar_t *FileName,eFileAccess AccessMode)
{
	return vWin32FileSystem::OpenDeviceStream(FileName,AccessMode);
}

iPtr<iEndpoint>			WindowsInterface::Win32FileOpenDeviceEndpoint(const wchar_t *FileName,eFileAccess AccessMode)
{
	return vWin32FileSystem::OpenDeviceEndpoint(FileName,AccessMode);
}

cnRTL::cStringBuffer<wchar_t> WindowsInterface::Win32FileMakeSystemFileName(cnWindows::eSystemFile File)
{
	return vWin32FileSystem::MakeSystemFileName(File);
}

//---------------------------------------------------------------------------

bool			WindowsInterface::WinSocketConnectionTypeIsSupported(iTypeID ConnectionID)
{
	return vWinSocketIO::ConnectionTypeIsSupported(ConnectionID);
}

//iPtr<iStream>				WindowsInterface::WinSocketMakeStream(SOCKET Socket)
//{
//	return vWinSocketIO::WinSocketMakeStream(Socket);
//}
iPtr<iConnection>			WindowsInterface::WinSocketMakeConnection(SOCKET Socket,iTypeID ConnectionIID,iPtr<iSocketAddress> RemoteAddress,iPtr<iSocketAddress> LocalAddress)
{
	return vWinSocketIO::MakeConnection(Socket,ConnectionIID,cnVar::MoveCast(RemoteAddress),cnVar::MoveCast(LocalAddress));
}
iPtr<iConnectionTask>		WindowsInterface::WinSocketMakeConnectTask(SOCKET Socket,iTypeID ConnectionIID,iPtr<iSocketAddress> RemoteAddress,iPtr<iSocketAddress> LocalAddress)
{
	return vWinSocketIO::MakeConnectTask(Socket,ConnectionIID,cnVar::MoveCast(RemoteAddress),cnVar::MoveCast(LocalAddress));
}

iPtr<iConnectionListener>	WindowsInterface::WinSocketMakeListener(SOCKET Socket,iTypeID ConnectionIID,iPtr<iSocketAddress> LocalAddress,int SocketType,int SocketProtocol)
{
	return vWinSocketIO::MakeListener(Socket,ConnectionIID,cnVar::MoveCast(LocalAddress),SocketType,SocketProtocol);
}
iPtr<iConnectionQueue>		WindowsInterface::WinSocketMakeListenQueue(SOCKET Socket,iTypeID ConnectionIID,iPtr<iSocketAddress> LocalAddress,int SocketType,int SocketProtocol)
{
	return vWinSocketIO::MakeListenQueue(Socket,ConnectionIID,cnVar::MoveCast(LocalAddress),SocketType,SocketProtocol);
}

iPtr<iMultipointStream>		WindowsInterface::WinSocketMakeDatagramStream(SOCKET Socket,iPtr<iSocketAddress> LocalAddress)
{
	return vWinSocketIO::MakeDatagramStream(Socket,cnVar::MoveCast(LocalAddress));
}
iPtr<iMultipointQueue>		WindowsInterface::WinSocketMakeDatagramQueue(SOCKET Socket,iPtr<iSocketAddress> LocalAddress)
{
	return vWinSocketIO::MakeDatagramQueue(Socket,cnVar::MoveCast(LocalAddress));
}

//---------------------------------------------------------------------------

bool WindowsInterface::WindowLocalSubclass(HWND WindowHandle,bcWindowSubclass *Subclass)
{
	return vWindowSubclass::SetLocal(WindowHandle,Subclass);
}

bool WindowsInterface::WindowSubclass(HWND WindowHandle,bcWindowSubclass *Subclass)
{
	return vWindowSubclass::SetLocal(WindowHandle,Subclass);
}

void WindowsInterface::WindowSubclassRestore(HWND WindowHandle,bcWindowSubclass *Subclass)
{
	return vWindowSubclass::Restore(WindowHandle,Subclass);
}


//---------------------------------------------------------------------------
//	cnSystem
//---------------------------------------------------------------------------

//- Default Heap ------------------------------------------------------------

#ifdef	cnLib_DEBUG
#define	DEFAULTHEAP	gDefaultDebugMemoryHeap
#else
#define	DEFAULTHEAP	gDefaultMemoryHeap
#endif
//---------------------------------------------------------------------------
void* cnSystem::DefaultHeap::Alloc(uIntn Size,uIntn Alignment)
{
	return DEFAULTHEAP.Alloc(Size,Alignment);
}
bool cnSystem::DefaultHeap::Free(void *Pointer,uIntn Size,uIntn Alignment)
{
	return DEFAULTHEAP.Free(Pointer,Size,Alignment);
}
bool cnSystem::DefaultHeap::Resize(void *Pointer,uIntn Size,uIntn NewSize)
{
	return DEFAULTHEAP.Resize(Pointer,Size,NewSize);
}
void* cnSystem::DefaultHeap::Relocate(void *Pointer,uIntn Size,uIntn Alignment,uIntn NewSize,uIntn NewAlignment,bool &ManualCopy)
{
	return DEFAULTHEAP.Relocate(Pointer,Size,Alignment,NewSize,NewAlignment,ManualCopy);
}
uIntn cnSystem::DefaultHeap::SizeOf(void *Pointer)
{
	return DEFAULTHEAP.SizeOf(Pointer);
}
#undef	DEFAULTHEAP

//---------------------------------------------------------------------------
rPtr<iTextEncodingConverter> cnSystem::UnicodeTranscoder(uInt8 ToEncodingSize,uInt8 FromEncodingSize)
{
#pragma message (cnLib_FILE_LINE ": TODO - UnicodeTranscoder")
	
	return nullptr;
	//return RetFromPtr<iCharacterSetConverter>(cnRTL::UnicodeTranscoder(ToEncodingSize,FromEncodingSize));
}
//---------------------------------------------------------------------------
//void cnLib_FUNC cnSystem::DebugLogStreamSetLogLevel(uInt8 Level)
//{
//	gDebugLogStream.SetLogLevel(Level);
//}
//---------------------------------------------------------------------------
void cnSystem::DebugLogReferenceInc(void *Object,uInt32 Tag)
{
#pragma message (cnLib_FILE_LINE ": TODO - DebugLogReferenceInc")
	//gDebugReferenceLog.Add(Object,Tag);
}
void cnSystem::DebugLogReferenceDec(void *Object,uInt32 Tag)
{
#pragma message (cnLib_FILE_LINE ": TODO - DebugLogReferenceDec")
	//gDebugReferenceLog.Sub(Object,Tag);
}

//- Time --------------------------------------------------------------------

sInt64 cnSystem::GetSystemTimeNow(void)
{
	cFileTime ft;
	ft.SetNow();
	return ft.ToSystemTime();
}

bool cnSystem::UTCGregorianDateFromSystemTime(cDateTime &DateTime,sInt64 SystemTime)
{
	cFileTime ft;
	ft.FromSystemTime(SystemTime);
	return ft.ToDateTime(DateTime);
}
bool cnSystem::UTCGregorianDateToSystemTime(sInt64 &SystemTime,const cDateTime &DateTime)
{
	cFileTime ft;
	if(ft.FromDateTime(DateTime)==false)
		return false;
	SystemTime=ft.ToSystemTime();
	return true;
}
//- Thread sync -------------------------------------------------------------
rPtr<iMutex> cnSystem::CreateMutexLock(void)
{
	return rCreate<cCSLock>();
}
//- Thread sync -------------------------------------------------------------
rPtr<iMutex> cnSystem::CreateRecursiveLock(void)
{
	return rCreate<cCSLock>();
}
//- Thread sync -------------------------------------------------------------
rPtr<iSharedMutex> cnSystem::CreateSharedLock(void)
{
	return rCreate<vcSRWLock>();
}
//- Thread sync -------------------------------------------------------------
rPtr<iThreadLocalVariable> cnSystem::CreateThreadLocalVariable(void (cnLib_FUNC *Destructor)(void*))
{
#pragma message (cnLib_FILE_LINE ": TODO - tls")
	return nullptr;
}
//- Thread ------------------------------------------------------------------
iPtr<iThread> cnSystem::StartThread(iProcedure *ThreadProcedure)
{
	return cThread::StartThread(ThreadProcedure);
}
//- Thread ------------------------------------------------------------------
iPtr<iDispatch> cnSystem::CreateDispatchThread(void)
{
#pragma message (cnLib_FILE_LINE ": TODO - rpCreateDispatchThread")
	return nullptr;
}
//- Current Thread ----------------------------------------------------------
bool cnSystem::CurrentThread::SleepUntil(const iTimepoint *Time,sInt64 Delay)
{
	return cCurrentThread::SleepUntil(Time,Delay);
}
void cnSystem::CurrentThread::SwitchThread(void)
{	::SwitchToThread();	}

iThread* cnSystem::CurrentThread::GetThread(void)
{	return cThread::QueryCurrent();	}

bool cnSystem::CurrentThread::SetPriority(sInt8 Priority)
{	return cCurrentThread::SetPriority(Priority);	}

sInt8 cnSystem::CurrentThread::GetPriority(void)
{	return cCurrentThread::GetPriority();	}

//---------------------------------------------------------------------------
rPtr<iSwapMemory> cnSystem::CreateSwapMemoryFromFile(iFile *File,uInt64 Size,eFileAccess AccessMode)
{
	return cnWin::CreateSwapMemoryFromFile(File,Size,AccessMode);
}
//- Network -----------------------------------------------------------------
iPtr<iIPv4Address> cnSystem::CreateAddressIPv4(uInt32 IP,uInt16 Port)
{
	return cnWin::CreateAddressIPv4(IP,Port);
}
//---------------------------------------------------------------------------
// Socket
//---------------------------------------------------------------------------
iPtr<iSocketAddress> cnLib_FUNC cnSystem::CreateSocketAddress(const sockaddr *addr,socklen_t addrlen)
{
	return cnWin::CreateSocketAddress(addr,addrlen);
}
//---------------------------------------------------------------------------
// UI
//---------------------------------------------------------------------------
iPtr<iUIView> cnSystem::CreateUIView(void)
{
#pragma message (cnLib_FILE_LINE ": TODO - iUIView")
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iPopupWindowControl> cnSystem::CreatePopupWindowControl(void)
{
#pragma message (cnLib_FILE_LINE ": TODO - iPopupWindowControl")
	return nullptr;
}
//---------------------------------------------------------------------------
iUIThread* cnSystem::CurrentUIThread::GetUIThread(void)
{
#pragma message (cnLib_FILE_LINE ": TODO - iUIThread")
	return nullptr;
}
//---------------------------------------------------------------------------
iDispatch* cnSystem::CurrentUIThread::GetDispatch(bool HighPriority)
{
#pragma message (cnLib_FILE_LINE ": TODO - UIThread - iDispatch")
	return nullptr;
}
//---------------------------------------------------------------------------
iInterface* cnLib_FUNC cnSystem::GetSysMouseCursor(eSysMouseCursor CursorID)
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
rPtr<iClipboardReader> cnSystem::QueryClipboardReader(iUIView *View)
{
#pragma message (cnLib_FILE_LINE ": TODO - iClipboardReader")
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iClipboardWriter> cnSystem::QueryClipboardWriter(iUIView *View)
{
#pragma message (cnLib_FILE_LINE ": TODO - iClipboardWriter")
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iInterface> cnSystem::QueryFont(const uChar16 *Name,uIntn NameLength)
{
#pragma message (cnLib_FILE_LINE ": TODO - QueryFont")
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iUITextLineLayout> cnSystem::CreateTextLineLayout(const uChar16 *Text,uIntn Length,const cUITextStyle &Style)
{
#pragma message (cnLib_FILE_LINE ": TODO - rpCreateTextLineLayout")
	return nullptr;
	//auto Layout=rCreate<cGDITextLayout>();
	//auto Device=gGDIPaintEngine.GetDCPaintDevice();
	//auto ScreenIC=Device->AcquireScreenIC();
	//Layout->SetupText(ScreenIC,Text,Length,Style);
	//Device->ReleaseScreenIC();
	//return RetFromPtr<iUITextLineLayout>(cnVar::MoveCast(Layout));
}
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> cnSystem::CreateBitmapSourceFromFile(iFileName *FileName)
{
	auto WinFileName=iCast<iWin32FileName>(FileName);
	if(WinFileName==nullptr)
		return nullptr;


	auto Bitmap=WICOpenImageFile(WinFileName->GetFileName());
	return iCreate<cWICBitmapDataSource>(Bitmap);
}
//---------------------------------------------------------------------------
//rRet<iUISimplePaintDevice> cnSystem::rpQueryUISimplePaintDevice(iUIView *ForView)
//{
//	if(ForView!=nullptr){
//		auto Window=iCast<iWindow>(ForView->GetWindow());
//		if(Window!=nullptr){
//			auto PaintEngine=Window->GetPaintEngine();
//			if(PaintEngine!=nullptr){
//				return PaintEngine->rpQuerySimplePaintDevice();
//			}
//		}
//		// fail to get paint engine from view
//	}
//	auto UIThread=cUIThread::CurrentUIThread();
//	if(UIThread!=nullptr){
//		// get thread default paint engine
//		return UIThread->DefaultPaintEngine->rpQuerySimplePaintDevice();
//	}
//	// gdi paint engine as last resort
//	return rRetFromPtr<iUISimplePaintDevice>(gGDIPaintEngine.GetSimplePaintDevice());
//}
//---------------------------------------------------------------------------
rPtr<iUISimpleViewContent> cnSystem::CreateUISimpleViewContent(void)
{
#pragma message (cnLib_FILE_LINE ": TODO - iUISimpleViewContent")
	return nullptr;
}

//---------------------------------------------------------------------------
//	Audio
//---------------------------------------------------------------------------
iPtr<iInterface> cnSystem::CreateAudioFormat_PCM(uInt32 Freq,uInt8 bits,uInt8 Channels)
{
#pragma message (cnLib_FILE_LINE ": TODO - CreateAudioFormat_PCM")
	return nullptr;
}
//---------------------------------------------------------------------------
//	cnWindows
//---------------------------------------------------------------------------
static LONG cnWindowsInitializeCount=0;
//---------------------------------------------------------------------------
uIntn cnWindows::reInitialize(void)
{
	if(::_InterlockedExchangeAdd(&cnWindowsInitializeCount,1)==0){
		// initialize
		//cnWin_Initialize();
	}
	// always success
	return static_cast<uIntn>(LibLoadResult::Success);
}
//---------------------------------------------------------------------------
void cnWindows::Finalize(void)
{
	if(cnWindowsInitializeCount==0)
		return;
	if(::_InterlockedExchangeAdd(&cnWindowsInitializeCount,-1)!=1)
		return;
	//cnWin_Finalize();
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
//- Heap---------------------------------------------------------------------
rPtr<iMemoryHeap> cnWindows::CreateMemoryHeap(void)
{
	return rCreate<cMemoryHeap>(0,0,0);
}
//- Thread pool -------------------------------------------------------------
iPtr<iThreadPool> cnWindows::CreateThreadPool(void)
{
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	return iCreate<vcThreadPool>();
#else
	return nullptr;
#endif
}
//---------------------------------------------------------------------------
iPtr<iWin32FileName> cnWindows::CreateFileName(const wchar_t *Win32FileName)
{
	return iCreate<cWin32FileName>(Win32FileName);
}
//- File --------------------------------------------------------------------
iPtr<iWin32FileName> cnWindows::CreateTemporaryFileName(iFileName *FolderName,const wchar_t *Prefix)
{
	auto WinFolderName=iCast<iWin32FileName>(FolderName);
	if(WinFolderName==nullptr)
		return nullptr;

	auto TempFileName=cnWin::Win32FileMakeTemporaryFileName(WinFolderName->GetFileName(),Prefix);
	if(TempFileName->Length==0)
		return nullptr;
	return iCreate<cWin32FileName>(cnVar::MoveCast(TempFileName));
}
//---------------------------------------------------------------------------
iPtr<iWin32FileName> cnWindows::GetSystemFileName(cnWindows::eSystemFile SystemFile)
{
	auto FolderName=WindowsInterface::Win32FileMakeSystemFileName(SystemFile);
	if(FolderName->Length==0)
		return nullptr;
	return iCreate<cWin32FileName>(FolderName.Swap());
}
//---------------------------------------------------------------------------
iPtr<iStream> cnWindows::OpenDeviceStream(const wchar_t *DeviceName)
{
	return vWin32FileSystem::OpenDeviceStream(DeviceName,FileAccess::Read|FileAccess::Write);
}
//---------------------------------------------------------------------------
iPtr<iEndpoint> cnWindows::OpenDeviceEndpoint(const wchar_t *DeviceName)
{
	return vWin32FileSystem::OpenDeviceEndpoint(DeviceName,FileAccess::Read|FileAccess::Write);
}
//---------------------------------------------------------------------------
rPtr<iODBC> cnWindows::ODBC(void)
{
	return rCreate<cODBC>();
}
//---------------------------------------------------------------------------
iWindow* cnWindows::GetWindowFromHandle(HWND WindowHandle)
{
#pragma message (cnLib_FILE_LINE ": TODO - GetWindowFromHandle")
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iWindowsUIApplication> cnWindows::CreateWindowsUIApplication(void)
{
#pragma message (cnLib_FILE_LINE ": TODO - rpCreateWindowsUIApplication")
	return nullptr;
	//auto UIThread=cUIThread::CreateOnCurrentThread();
	//if(UIThread==nullptr)
	//	return rNullRet<iWindowsUIApplication>();
	//return rCreateRet<iWindowsUIApplication,cWindowsUIApplication>(UIThread);
}
//---------------------------------------------------------------------------
iPtr<iUIThread> cnWindows::CreateUIThreadOnCurrentThread(void)
{
#pragma message (cnLib_FILE_LINE ": TODO - rpCreateWindowsUIApplication")
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iUIThread> cnWindows::StartUIThread(void)
{
#pragma message (cnLib_FILE_LINE ": TODO - rpStartUIThread")
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iWindowProvider> cnWindows::CreateWindowProvider(void)
{
#pragma message (cnLib_FILE_LINE ": TODO - rpCreateWindowProvider")
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iWindowClient> cnWindows::CreateWindowClient(void)
{
#pragma message (cnLib_FILE_LINE ": TODO - CreateWindowClient")
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iWindowFrame> cnWindows::CreateWindowFrame(void)
{
#pragma message (cnLib_FILE_LINE ": TODO - CreateWindowFrame")
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iOwnerFocusWindowClient> cnWindows::CreateOwnerFocusWindowClient(void)
{
#pragma message (cnLib_FILE_LINE ": TODO - rpCreateOwnerFocusWindowClientComponent")
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iWindow> cnWindows::CreateHWND(HWND Parent,const wchar_t *WindowText,DWORD Style,DWORD ExStyle,LONG X,LONG Y,LONG Width,LONG Height,UINT ChildID)
{
#pragma message (cnLib_FILE_LINE ": TODO - rpCreateWindowHost")
	return nullptr;
	//auto UIThread=cUIThread::CurrentUIThread();
	//if(UIThread==nullptr)
	//	return rNullRet<iWindowHost>();
	//if((Style&WS_CHILD)==0){
	//	ChildID=0;
	//}
	//auto WindowHandle=cnWin::Create_cnWindow(Parent,WindowText,Style,ExStyle,X,Y,Width,Height,reinterpret_cast<HMENU>(static_cast<uIntn>(ChildID)));
	//if(WindowHandle==nullptr)
	//	return rNullRet<iWindowHost>();
	//auto WindowHost=rCreate<cSubclassWindowHost>();
	//if(vWindowCreator::LocalSubclass(WindowHandle,WindowHost)){
	//	return RetFromPtr<iWindowHost>(WindowHost);
	//}
	//::DestroyWindow(WindowHandle);
	//return rNullRet<iWindowHost>();
}
//---------------------------------------------------------------------------
rPtr<iDCPaintDevice> cnWindows::QueryDCPaintDevice(void)
{
#pragma message (cnLib_FILE_LINE ": TODO - QueryDCPaintDevice")
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iUIDCViewContent> cnWindows::CreateDCViewContent(uIntn DCViewContentOptions)
{
#pragma message (cnLib_FILE_LINE ": TODO - rpCreateUIDCViewContent")
	return nullptr;
	//return rCreateRet<iUIDCViewContent,cGDIUIDCViewContent>();
}
//---------------------------------------------------------------------------
rPtr<iDXGIViewContent> cnWindows::CreateDXGIViewContent(void)
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
