#include "stdafx.h"

#include <cnWinNT5\NT5_Thread.h>
#include <cnWinNT5\NT5_WMMAudio.h>
#include <cnWinNT5\NT5_WindowClass.h>
#include <cnWinNT5\NT5_Socket.h>

#include <cnWin\cnWin.h>

#include <cnPLWinDLL\cnWinDLL_SysFunc.h>

#include <cnRTL\cnRTL.h>
#include <cnSystem\cnSystem.h>

#include <cnRTL\winCPPInit.h>

#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"ole32.lib")

cnWin_INITSEG_SET(cnWin,"cnWin_INIT$2")

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;
using namespace cnWinNT5;

#define	cnLib_GlobalVar	__declspec(selectany)

//---------------------------------------------------------------------------
extern "C" IMAGE_DOS_HEADER __ImageBase;
cnLib_GlobalVar const HMODULE cnWin::gModuleHandle=reinterpret_cast<HMODULE>(&__ImageBase);
//---------------------------------------------------------------------------
cnLib_GlobalVar const cSysInfo cnWin::gSysInfo;
cnLib_GlobalVar const cStartupInfo cnWin::gStartupInfo;

//- Memory Heap -------------------------------------------------------------
// memory heap should be the first object to be initialized
#ifdef	cnLib_DEBUG
cnLib_GlobalVar cReferenceStaticImplementT<cCRTDebugMemoryHeap> cnWin::gDefaultDebugMemoryHeap;
#else
cnLib_GlobalVar cReferenceStaticImplementT<cDefaultMemoryHeap> cnWin::gDefaultMemoryHeap;
#endif

//- Debug -------------------------------------------------------------------
cnLib_GlobalVar cInterfaceStaticImplementT<cDebugLogStream> cnWin::gDebugLogStream;
cnLib_GlobalVar cnRTL::cReferenceLog cnWin::gDebugReferenceLog;

static cnRTL::cLogger _RTLLogger;
cnLib_GlobalVar cnRTL::cLogger &cnRTL::gRTLLogger=_RTLLogger;
//---------------------------------------------------------------------------
cnLib_GlobalVar HANDLE cnWin::gCurrentProcessHandle=::GetCurrentProcess();
cnLib_GlobalVar HANDLE cnWin::gCurrentThreadHandle=::GetCurrentThread();
//---------------------------------------------------------------------------
cnLib_GlobalVar const cFileTime cnWin::gRefTimeUnix=GetRefTimeUnix();
//- thread ------------------------------------------------------------------
cnLib_GlobalVar cThreadModule cnWin::gThreadModule;
cnLib_GlobalVar cThreadManager cThreadManager::gInstance;

cnLib_GlobalVar cTLSPtr<cThread> cThread::gTLS;

cnLib_GlobalVar Sc009::cTLSManager &Versioned::gTLSManager=cThreadManager::gInstance.TLSManager;

//- thread pool -------------------------------------------------------------
cnLib_GlobalVar cDefaultThreadPool cDefaultThreadPool::gInstance;
//- file --------------------------------------------------------------------
cnLib_GlobalVar cWinFolder_Application cnWin::gSysFolder_Application;
cnLib_GlobalVar cWinFolder_Document cnWin::gSysFolder_Document;
cnLib_GlobalVar cWinFolder_Temp cnWin::gSysFolder_Temp;
//- Interprocess ------------------------------------------------------------
cnLib_GlobalVar cInterfaceStaticImplementT<cIPCFileMappingProtocol> cnWin::gIPCFileMappingProtocol;
//- network -----------------------------------------------------------------
cnLib_GlobalVar cInterfaceStaticImplementT<cSocketStreamProtocol> gSocketTCP(SOCK_STREAM,IPPROTO_TCP);
cnLib_GlobalVar cInterfaceStaticImplementT<cSocketDatagramProtocol> gSocketUDP(SOCK_DGRAM,IPPROTO_UDP);
//- Audio -------------------------------------------------------------------
cnLib_GlobalVar cnRTL::cInterfaceStaticImplementT<cWMMDefaultAudioDevice> gDefDevice;
//- ui ----------------------------------------------------------------------
cnLib_GlobalVar cWindowClass_cnWindow cnWin::gWindowClass_cnWindow;
cnLib_GlobalVar cWindowCodeBlockAllocator cnWinNT5::gWPCodeAllocator;

cnLib_GlobalVar cTLSPtr cUIThread::gUIThreadTLS;
cnLib_GlobalVar cUIThreadMessage::cGlobal cUIThreadMessage::Global;
// default paint engine
cnLib_GlobalVar cGDIPaintEngine cnWin::gGDIPaintEngine;


//---------------------------------------------------------------------------
cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_Normal(nullptr,IDC_ARROW);
cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_Busy(nullptr,IDC_WAIT);
cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_BackgroundBusy(nullptr,IDC_APPSTARTING);
cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_TextEdit(nullptr,IDC_IBEAM);
cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_SizeA(nullptr,IDC_SIZEALL);
cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_SizeV(nullptr,IDC_SIZENS);
cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_SizeH(nullptr,IDC_SIZEWE);
cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_SizeFS(nullptr,IDC_SIZENESW);
cnLib_GlobalVar	cMouseCursorSetter cnWin::gMouseCursor_SizeBS(nullptr,IDC_SIZENWSE);

//- misc -------------------------------------------------------------------



//- cnSystem - Pointer ------------------------------------------------------


#ifdef	cnLib_DEBUG
cnLib_GlobalVar iMemoryHeap *const				cnSystem::DefaultMemoryHeap=	&gDefaultDebugMemoryHeap;
#else
cnLib_GlobalVar iMemoryHeap *const				cnSystem::DefaultMemoryHeap=	&gDefaultMemoryHeap;
#endif


cnLib_GlobalVar iLogStream *const				cnSystem::DebugLogStream=&gDebugLogStream;

static cFileTimeNow gTimeNow;
cnLib_GlobalVar const iTimepoint *const			cnSystem::TimeNow=&gTimeNow;
cnLib_GlobalVar	const sInt64					cnSystem::EpochSecondsSinceUnix=0;


// ThreadPool

cnLib_GlobalVar iThreadPool *const				cnSystem::ThreadPool=	&cDefaultThreadPool::gInstance;

// Net

cnLib_GlobalVar	iConnectionProtocol *const		cnSystem::TCP=					&gSocketTCP;
cnLib_GlobalVar	iDatagramProtocol *const		cnSystem::UDP=					&gSocketUDP;

// UI

// Audio

cnLib_GlobalVar iAudioDevice*const				cnSystem::DefaultAudioDevice=	&gDefDevice;

// cnWindows

cnLib_GlobalVar	iFileName *const				cnWindows::Folder_Application=	&gSysFolder_Application;
cnLib_GlobalVar	iFileName *const				cnWindows::Folder_Document=		&gSysFolder_Document;
cnLib_GlobalVar	iFileName *const				cnWindows::Folder_Temp=			&gSysFolder_Temp;

cnLib_GlobalVar	iConnectionProtocol *const		cnWindows::InterprocessProtocol=&gIPCFileMappingProtocol;

cnLib_GlobalVar	iGDIPaintEngine*const			cnWindows::GDIPaintEngine		=&gGDIPaintEngine;
cnLib_GlobalVar	iDXGIPaintEngine*const			cnWindows::D3D9ExPaintEngine	=nullptr;
//---------------------------------------------------------------------------
cnLib_GlobalVar	const pSystemVar cnWin::gSystemVar={
	
	// cnSystem

#ifdef	cnLib_DEBUG
	&gDefaultDebugMemoryHeap,
#else
	&gDefaultMemoryHeap,
#endif
	&gDebugLogStream,
	&cDefaultThreadPool::gInstance,
	&gTimeNow,
	0,
	&gSocketTCP,
	&gSocketUDP,
	&gIPCFileMappingProtocol,

	// cnWindows

	&gSysFolder_Application,
	&gSysFolder_Document,
	&gSysFolder_Temp,

	&gGDIPaintEngine,
	nullptr,
};
//---------------------------------------------------------------------------
cnLib_GlobalVar cProcedureStack cnWin::gLibExitNotify;
//---------------------------------------------------------------------------
