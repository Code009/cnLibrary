//#include "cnWinSystem.h"
#include "cnWinAPISystem.h"

cnWinSystem_INITSEG_SET

#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"ole32.lib")

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace cnWin;
using namespace cnWinNT6;

#define	cnLib_GlobalVar	__declspec(selectany)

//---------------------------------------------------------------------------
extern "C" IMAGE_DOS_HEADER __ImageBase;
cnLib_GlobalVar const HMODULE cnWin::gModuleHandle=reinterpret_cast<HMODULE>(&__ImageBase);
//---------------------------------------------------------------------------
cnLib_GlobalVar const cSysInfo cnWin::gSysInfo;
//cnLib_GlobalVar const cStartupInfo cnWin::gStartupInfo;
cnLib_GlobalVar cDependentRegistration cnWin::gSystemDependentRegistration;
//- Memory Heap -------------------------------------------------------------
// memory heap should be the first object to be initialized
cnLib_GlobalVar cReferenceStaticImplementT<tDefaultMemoryHeap> cnWin::gDefaultMemoryHeap;

//- Debug -------------------------------------------------------------------
//cnLib_GlobalVar cInterfaceStaticImplementT<cDebugLogStream> cnWin::gDebugLogStream;
//cnLib_GlobalVar cnRTL::cReferenceLog cnWin::gDebugReferenceLog;
//static cnRTL::cLogger _RTLLogger;
//cnLib_GlobalVar cnRTL::cLogger &cnRTL::gRTLLogger=_RTLLogger;
//---------------------------------------------------------------------------
cnLib_GlobalVar HANDLE cnWin::gCurrentProcessHandle=::GetCurrentProcess();
cnLib_GlobalVar HANDLE cnWin::gCurrentThreadHandle=::GetCurrentThread();

cnLib_GlobalVar const LARGE_INTEGER cnWindows::SystemTimeEpochAsFileTime={	0xd53e8000,0x019db1de	};	// 1970.1.1 0:0:0
//- thread ------------------------------------------------------------------


template<> __declspec(thread) cNT6TLSStaticThreadData* cNT6TLSStaticPointer<cThread>::gDataTLS=nullptr;	// add this definition to supresss dynamic initialization of tls
cnLib_GlobalVar cNT6TLSStaticPointer<cThread> gTLS_cThread;
iThreadLocalVariable *const cThread::gTLS=&gTLS_cThread;


template<> __declspec(thread) cNT6TLSStaticThreadData* cNT6TLSStaticPointer<cErrorReportRecord>::gDataTLS=nullptr;
cnLib_GlobalVar cNT6TLSStaticPointer<cErrorReportRecord> gTLS_ErrorReportRecord;
iThreadLocalVariable *const cErrorReportRecord::gTLSRecord=&gTLS_ErrorReportRecord;

//- thread pool -------------------------------------------------------------
cnLib_GlobalVar vcDefaultThreadPool vcDefaultThreadPool::gInstance;

//- COM ---------------------------------------------------------------------
cnLib_GlobalVar cCOMApartmentThreadingModule cnWin::gCOMApartmentThreadingModule;

//- file --------------------------------------------------------------------

//- network -----------------------------------------------------------------
//cnLib_GlobalVar cSocketModule cnWin::gSocketModule;
cnLib_GlobalVar cnRTL::cInterfaceStaticImplementT<vSocketStreamProtocol> cnWin::TCP(SOCK_STREAM,IPPROTO_TCP);
cnLib_GlobalVar cnRTL::cInterfaceStaticImplementT<vSocketDatagramProtocol> cnWin::UDP(SOCK_DGRAM,IPPROTO_UDP);
//- ui ----------------------------------------------------------------------

cnLib_GlobalVar cMessageThreadWindowClass cMessageThreadWindowClass::gMessageThreadWindowClass(L"cnLibraryMessageThreadWindow");

//cnLib_GlobalVar cWindowClass_cnWindow cnWin::gWindowClass_cnWindow;

//cnLib_GlobalVar cTLSPtr cUIThread::gUIThreadTLS;
//cnLib_GlobalVar cUIThreadMessage::cGlobal cUIThreadMessage::Global;
// default paint engine
//cnLib_GlobalVar cGDIPaintEngine cnWin::gGDIPaintEngine;
//cnLib_GlobalVar cD3D9ExPaintEngine gD3D9exPainEngine;

//---------------------------------------------------------------------------
//cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_Normal(nullptr,IDC_ARROW);
//cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_Busy(nullptr,IDC_WAIT);
//cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_BackgroundBusy(nullptr,IDC_APPSTARTING);
//cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_TextEdit(nullptr,IDC_IBEAM);
//cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_SizeA(nullptr,IDC_SIZEALL);
//cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_SizeV(nullptr,IDC_SIZENS);
//cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_SizeH(nullptr,IDC_SIZEWE);
//cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_SizeFS(nullptr,IDC_SIZENESW);
//cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_SizeBS(nullptr,IDC_SIZENWSE);

//- Interprocessor ----------------------------------------------------------
//cnLib_GlobalVar cInterfaceStaticImplementT<cIPCFileMappingProtocol> cnWin::gIPCFileMappingProtocol;
//- audio -------------------------------------------------------------------
//cnLib_GlobalVar cInterfaceStaticImplementT<cCoreAudioDefaultDevice> gCoreAudioDefaultDevice;

//- misc -------------------------------------------------------------------
cnLib_GlobalVar cnRTL::cReferenceStaticImplementT<cnWinRTL::cWin32DebugTextOutput> cnWin::DebugTextOutput;



//- cnSystem - Pointer ------------------------------------------------------

cnLib_GlobalVar iMemoryHeap*const		cnSystem::DefaultMemoryHeap		=&gDefaultMemoryHeap;

cnLib_GlobalVar	const sInt64			cnSystem::SystemTimeEpochSecondsSinceUnixEpoch=0;



// Thread

cnLib_GlobalVar	iThreadPool *const		cnSystem::DefaultThreadPool						=&vcDefaultThreadPool::gInstance;

// Network

cnLib_GlobalVar	iConnectionProtocol*const	cnSystem::TCP				=&cnWin::TCP;
cnLib_GlobalVar	iDatagramProtocol*const		cnSystem::UDP				=&cnWin::UDP;

// UI

// cnWindows

cnLib_GlobalVar	iTextStreamOutput*const		cnWindows::DebugLogStream	=&cnWin::DebugTextOutput;

cnLib_GlobalVar	iWinThreadPool *const		cnWindows::DefaultThreadPool			=&vcDefaultThreadPool::gInstance;


//---------------------------------------------------------------------------
