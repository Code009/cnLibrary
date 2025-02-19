#include "WinWindow.h"

#if _WIN32_WINNT >= _WIN32_WINNT_WINXP
#include <CommCtrl.h>
#pragma comment(lib,"Comctl32.lib")
#endif

//#if _WIN32_WINNT >= _WIN32_WINNT_WINBLUE
#include <ShellScalingApi.h>
#pragma comment(lib,"Shcore.lib")
//#endif

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

//---------------------------------------------------------------------------
bcWindowClass::operator LPCWSTR ()noexcept(true){
	return reinterpret_cast<LPCWSTR>(fAtom);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWindowClass::cWindowClass(const WNDCLASSEXW *ClassInfo)noexcept(true)
{
	fModuleHandle=ClassInfo->hInstance;
	fAtom=RegisterClassExW(ClassInfo);
}
//---------------------------------------------------------------------------
cWindowClass::cWindowClass(HINSTANCE ModuleHandle,const wchar_t *ClassName,WNDPROC WndClassProc,UINT style)noexcept(true)
{
	fModuleHandle=ModuleHandle;
	// register window class for UIObject
	WNDCLASSEXW WindowClassInfo;
	cnMemory::ZeroFill(&WindowClassInfo.cbClsExtra,sizeof(WNDCLASSEXW)-cnMemory::MemberOffset(&WNDCLASSEXW::cbClsExtra));
	WindowClassInfo.cbSize=sizeof(WNDCLASSEXW);
	WindowClassInfo.style=style;
	WindowClassInfo.lpfnWndProc=WndClassProc;
	WindowClassInfo.hInstance=ModuleHandle;
	WindowClassInfo.lpszClassName=ClassName;

	fAtom=RegisterClassExW(&WindowClassInfo);
}
//---------------------------------------------------------------------------
cWindowClass::~cWindowClass()noexcept(true)
{
	if(fAtom!=0)
		UnregisterClassW(reinterpret_cast<LPCWSTR>(fAtom),fModuleHandle);	
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_WINXP
//---------------------------------------------------------------------------
LRESULT NTXPWindowSubclass::cDefaultProcedureCaller::Execute(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)noexcept(true)
{
	return ::DefSubclassProc(hWnd,uMsg,wParam,lParam);
}
//---------------------------------------------------------------------------
static constexpr UINT SubclassID=0xC009;
static LRESULT CALLBACK SubclassProcedure(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,UINT_PTR uIdSubclass,DWORD_PTR dwRefData)noexcept(true)
{	hWnd,uIdSubclass;
	auto WindowSubclass=reinterpret_cast<bcWindowSubclass*>(dwRefData);
	if(uMsg!=WM_DESTROY){
		return WindowSubclass->SubclassMessage(uMsg,wParam,lParam);
	}
	// remove subclass
	NTXPWindowSubclass::Restore(hWnd,WindowSubclass);
	
	return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------
bool NTXPWindowSubclass::SetLocal(HWND WindowHandle,bcWindowSubclass *Subclass)noexcept(true)
{
	return Set(WindowHandle,Subclass);
}
//---------------------------------------------------------------------------
bool NTXPWindowSubclass::Set(HWND WindowHandle,bcWindowSubclass *Subclass)noexcept(true)
{
	if(::SetWindowSubclass(WindowHandle,SubclassProcedure,SubclassID,reinterpret_cast<DWORD_PTR>(Subclass))==FALSE){
		return false;
	}
	Subclass->DefaultWindowProcedure.ConstructAs<cDefaultProcedureCaller>();
	Subclass->SubclassAttached(WindowHandle);
	return true;
}
//---------------------------------------------------------------------------
void NTXPWindowSubclass::Restore(HWND WindowHandle,bcWindowSubclass *Subclass)noexcept(true)
{
	// window destroyed
	Subclass->SubclassDetached();

	::RemoveWindowSubclass(WindowHandle,SubclassProcedure,SubclassID);
}
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT >= _WIN32_WINNT_WINXP
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWindowUIState::cWindowUIState()noexcept(true)
{
}
//---------------------------------------------------------------------------
cWindowUIState::~cWindowUIState()noexcept(true)
{
}
//---------------------------------------------------------------------------
LONG cWindowUIState::WindowAttached(HWND WindowHandle)noexcept(true)
{
	fWindowValid=true;

	LONG StyleValue=::GetWindowLongW(WindowHandle,GWL_STYLE);

	fWindowVisible=(StyleValue&WS_VISIBLE)!=0;
	fWindowActive=WindowHandle==::GetActiveWindow();

	return StyleValue;
}

//---------------------------------------------------------------------------
void cWindowUIState::WindowDetached(void)noexcept(true)
{
	fWindowValid=false;
}
//---------------------------------------------------------------------------
void cWindowUIState::AfterChildWindowCreated(HWND WindowHandle)noexcept(true)
{
	LONG StyleValue=::GetWindowLongW(WindowHandle,GWL_STYLE);
	fWindowVisible=(0!=(StyleValue&WS_VISIBLE));
}
//---------------------------------------------------------------------------
bool cWindowUIState::WindowMessageCheckState(const cWindowMessageParam &MsgParam)noexcept(true)
{
	switch(MsgParam.Code){
	case WM_STYLECHANGED:
		if(MsgParam.wParam==GWL_STYLE){
			auto StyleValue=reinterpret_cast<STYLESTRUCT*>(MsgParam.lParam);
			// test if visible flag changed
			auto NewFlag=StyleValue->styleNew&WS_VISIBLE;
			auto OldFlag=StyleValue->styleOld&WS_VISIBLE;
			if(NewFlag!=OldFlag){
				fWindowVisible=NewFlag!=0;
				return true;
			}
		}
		break;
	case WM_PAINT:
		if(fWindowVisible==false){
			fWindowVisible=true;
			return true;
		}
		break;
	case WM_ACTIVATE:
		switch(MsgParam.wParam){
		default:
		case WA_INACTIVE:
			fWindowActive=false;
			break;
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			fWindowActive=true;
			break;
		}
		return true;
	case WM_WINDOWPOSCHANGED:
		{
			auto wpos=reinterpret_cast<const WINDOWPOS*>(MsgParam.lParam);
			// visible flag
			if(wpos->flags&(SWP_SHOWWINDOW|SWP_HIDEWINDOW)){
				if(wpos->flags&SWP_HIDEWINDOW){
					fWindowVisible=false;
				}
				else{
					fWindowVisible=true;
				}
				return true;
			}
		}
		break;
	}
	return false;
}
//---------------------------------------------------------------------------
eUIState cWindowUIState::GetUIState(void)const noexcept(true)
{
	if(fWindowValid==false)
		return UIState::Null;
	
	if(fWindowVisible==false){
		return UIState::Background;
	}

	if(fWindowActive==false){
		return UIState::Inactive;
	}

	return UIState::Active;
}
//---------------------------------------------------------------------------
int cnWinRTL::GetWindowDPI(HWND hWnd)noexcept(true)
{
//#if _WIN32_WINNT >= _WIN32_WINNT_WINBLUE
	auto monitor=::MonitorFromWindow(hWnd,MONITOR_DEFAULTTONEAREST);
	UINT newDpiX;
	UINT newDpiY;
	if (FAILED(::GetDpiForMonitor(monitor,MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI,&newDpiX,&newDpiY)))
	{
		newDpiX = 96;
		newDpiY = 96;
	}
	return newDpiX;
//#endif
}
//---------------------------------------------------------------------------
HWND cnWinRTL::GetWindowHandleFromUIWindow(iUIArea *Area)noexcept(true)
{
	if(Area==nullptr)
		return nullptr;
	auto WindowHandleProperty=iCast<iWindowHandleProperty>(Area);
	if(WindowHandleProperty==nullptr){
		return nullptr;
	}
	return WindowHandleProperty->GetWindowHandle();
}
//---------------------------------------------------------------------------
HWND cnWinRTL::GetWindowHandleFromUIView(iUIView *View)noexcept(true)
{
	if(View==nullptr)
		return nullptr;
	return GetWindowHandleFromUIWindow(View->GetWindow());
}
//---------------------------------------------------------------------------
iWindow* cnWinRTL::GetWindowFromUIWindow(iUIArea *Area)noexcept(true)
{
	auto WindowClient=iCast<iWindowClient>(Area);
	if(WindowClient!=nullptr)
		return WindowClient->GetWindow();
	auto WindowFrame=iCast<iWindowClient>(Area);
	if(WindowFrame!=nullptr)
		return WindowFrame->GetWindow();

	return nullptr;
}
//---------------------------------------------------------------------------
iWindow* cnWinRTL::GetWindowFromUIView(iUIView *View)noexcept(true)
{
	if(View==nullptr)
		return nullptr;
	return GetWindowFromUIWindow(View->GetWindow());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const UINT cWindowMessageThread::Message_Noop			=WM_USER;
const UINT cWindowMessageThread::Message_Function		=WM_USER+1;
const UINT cWindowMessageThread::Message_Execute		=WM_USER+2;
const UINT cWindowMessageThread::Message_ExecuteRef		=WM_USER+3;
const UINT cWindowMessageThread::Message_QuitLoop		=WM_USER+4;
//---------------------------------------------------------------------------
cWindowMessageThread::cWindowMessageThread()noexcept(true)
{
}
//---------------------------------------------------------------------------
cWindowMessageThread::~cWindowMessageThread()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cWindowMessageThread::Setup(HWND MessageWindow,DWORD MessageThreadID)noexcept(true)
{
	fMessageThreadID=MessageThreadID;
	fMessageWindow=MessageWindow;
}
//---------------------------------------------------------------------------
void cWindowMessageThread::Clear(void)noexcept(true)
{
	fMessageWindow=nullptr;
	fMessageThreadID=0;
}
//---------------------------------------------------------------------------
HWND cWindowMessageThread::GetMessageWindow(void)const noexcept(true)
{
	return fMessageWindow;
}
//---------------------------------------------------------------------------
bool cWindowMessageThread::MessageWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)noexcept(true)
{hWnd;
	switch(uMsg){
	case WM_TIMER:
		reinterpret_cast<iProcedure*>(wParam)->Execute();
		return true;
	case Message_Function:
		reinterpret_cast<void (*)(void *)noexcept(true)>(lParam)(reinterpret_cast<void*>(wParam));
		return true;
	case Message_Execute:
		reinterpret_cast<iProcedure*>(lParam)->Execute();
		return true;
	case Message_ExecuteRef:
		reinterpret_cast<iProcedure*>(lParam)->Execute();
		rDecReference(reinterpret_cast<iReference*>(wParam),'mexe');
		return true;
	case Message_QuitLoop:
		::PostQuitMessage(static_cast<int>(wParam));
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void cWindowMessageThread::PostQuitMessageLoop(int ExitCode)noexcept(true)
{
	if(IsCurrentThread()){
		::PostQuitMessage(ExitCode);
	}
	else{
		::PostMessageW(fMessageWindow,Message_QuitLoop,ExitCode,0);
	}
}
//---------------------------------------------------------------------------
int cWindowMessageThread::MessageLoop(void)noexcept(true)
{
	MSG msg;
	while(::GetMessageW(&msg,nullptr,0,0)){
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	}
	return static_cast<int>(msg.wParam);
}
//---------------------------------------------------------------------------
bool cWindowMessageThread::IsCurrentThread(void)const noexcept(true)
{
	return ::GetCurrentThreadId()==fMessageThreadID;
}
//---------------------------------------------------------------------------
void cWindowMessageThread::SendNoop(void)noexcept(true)
{
	::SendMessageW(fMessageWindow,Message_Noop,0,0);
}
//---------------------------------------------------------------------------
void cWindowMessageThread::PostNoop(void)noexcept(true)
{
	::PostMessageW(fMessageWindow,Message_Noop,0,0);
}
//---------------------------------------------------------------------------
void cWindowMessageThread::Execute(iReference *Reference,iProcedure *Procedure)noexcept(true)
{
	rIncReference(Reference,'mexe');
	if(::PostMessageW(fMessageWindow,Message_ExecuteRef,reinterpret_cast<WPARAM>(Reference),reinterpret_cast<LPARAM>(Procedure))==FALSE){
		rDecReference(Reference,'mexe');
	}
}
//---------------------------------------------------------------------------
void cWindowMessageThread::ExecuteNoRef(iProcedure *Procedure)noexcept(true)
{
	::PostMessageW(fMessageWindow,Message_Execute,0,reinterpret_cast<LPARAM>(Procedure));
}
//---------------------------------------------------------------------------
void cWindowMessageThread::ExecuteSync(iProcedure *Procedure)noexcept(true)
{
	::SendMessageW(fMessageWindow,Message_Execute,0,reinterpret_cast<LPARAM>(Procedure));
}
//---------------------------------------------------------------------------
void cWindowMessageThread::SendFunction(void *Parameter,void (*Function)(void *Parameter)noexcept(true))noexcept(true)
{
	::SendMessageW(fMessageWindow,Message_Function,reinterpret_cast<WPARAM>(Parameter),reinterpret_cast<LPARAM>(Function));
}
//---------------------------------------------------------------------------
void cWindowMessageThread::PostFunction(void *Parameter,void (*Function)(void *Parameter)noexcept(true))noexcept(true)
{
	::PostMessageW(fMessageWindow,Message_Function,reinterpret_cast<WPARAM>(Parameter),reinterpret_cast<LPARAM>(Function));
}
//---------------------------------------------------------------------------
bool cWindowMessageThread::SetTimer(iProcedure *Procedure,UINT Interval)noexcept(true)
{
	return ::SetTimer(fMessageWindow,reinterpret_cast<UINT_PTR>(Procedure),Interval,nullptr)!=FALSE;
}
//---------------------------------------------------------------------------
bool cWindowMessageThread::KillTimer(iProcedure *Procedure)noexcept(true)
{
	return ::KillTimer(fMessageWindow,reinterpret_cast<UINT_PTR>(Procedure))!=FALSE;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWindowMessageQueueDispatch::cWindowMessageQueueDispatch(aClsRef<cWindowMessageThread> MessageThread)noexcept(true)
	: fMessageThread(cnVar::MoveCast(MessageThread))
{
}
//---------------------------------------------------------------------------
cWindowMessageQueueDispatch::~cWindowMessageQueueDispatch()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cWindowMessageQueueDispatch::Execute(iReference *Reference,iProcedure *Procedure)noexcept(true)
{
	return fMessageThread->Execute(Reference,Procedure);
}
//---------------------------------------------------------------------------
rPtr<iAsyncProcedure> cWindowMessageQueueDispatch::CreateWork(iReference *Reference,iProcedure *ThreadProcedure)noexcept(true)
{
	if(Reference!=nullptr){
		return rCreate<cAsyncProcedure>(fMessageThread,Reference,ThreadProcedure);
	}
	else{
		return rCreate<cAsyncProcedureNoRef>(fMessageThread,ThreadProcedure);
	}
}
//---------------------------------------------------------------------------
rPtr<iAsyncTimer> cWindowMessageQueueDispatch::CreateTimer(iReference *Reference,iProcedure *ThreadProcedure)noexcept(true)
{
	return rCreate<cAsyncTimer>(fMessageThread,Reference,ThreadProcedure);
}
//---------------------------------------------------------------------------
bool cWindowMessageQueueDispatch::IsCurrentThread(void)noexcept(true)
{
	return fMessageThread->IsCurrentThread();
}
//---------------------------------------------------------------------------
void cWindowMessageQueueDispatch::ExecuteSync(iProcedure *Procedure)noexcept(true)
{
	return fMessageThread->ExecuteSync(Procedure);
}
//---------------------------------------------------------------------------
iPtr<iAsyncTask> cWindowMessageQueueDispatch::ExecuteAsync(iReference *Reference,iProcedure *Procedure)noexcept(true)
{
	return nullptr;
}
//---------------------------------------------------------------------------
cWindowMessageQueueDispatch::cAsyncProcedure::cAsyncProcedure(aClsRef<cWindowMessageThread> MessageThread,iReference *Reference,iProcedure *Procedure)noexcept(true)
	: fMessageThread(cnVar::MoveCast(MessageThread))
	, fReference(Reference)
	, fProcedure(Procedure)
{
}
//---------------------------------------------------------------------------
void cWindowMessageQueueDispatch::cAsyncProcedure::Start(void)noexcept(true)
{
	fMessageThread->Execute(fReference,fProcedure);
}
//---------------------------------------------------------------------------
cWindowMessageQueueDispatch::cAsyncProcedureNoRef::cAsyncProcedureNoRef(aClsRef<cWindowMessageThread> MessageThread,iProcedure *Procedure)noexcept(true)
	: fMessageThread(cnVar::MoveCast(MessageThread))
	, fProcedure(Procedure)
{
}
//---------------------------------------------------------------------------
void cWindowMessageQueueDispatch::cAsyncProcedureNoRef::Start(void)noexcept(true)
{
	fMessageThread->ExecuteNoRef(fProcedure);
}
//---------------------------------------------------------------------------
cWindowMessageQueueDispatch::cAsyncTimer::cAsyncTimer(aClsRef<cWindowMessageThread> MessageThread,iReference *Reference,iProcedure *Procedure)noexcept(true)
	: fMessageThread(cnVar::MoveCast(MessageThread))
	, fReference(Reference)
	, fProcedure(Procedure)
	, fTimerState(0)
	, fThreadPoolTimer(cnSystem::DefaultThreadPool->CreateTimer(&fInnerReference,&fThreadPoolTimerProcedure))
{
}
//---------------------------------------------------------------------------
cWindowMessageQueueDispatch::cAsyncTimer::~cAsyncTimer()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cWindowMessageQueueDispatch::cAsyncTimer::VirtualStarted(void)noexcept(true)
{
	cDualReference::VirtualStarted();
}
//---------------------------------------------------------------------------
void cWindowMessageQueueDispatch::cAsyncTimer::VirtualStopped(void)noexcept(true)
{
	ClearTimer();
	cDualReference::VirtualStopped();
}
//---------------------------------------------------------------------------
void cWindowMessageQueueDispatch::cAsyncTimer::Start(uInt64 DueTime,uInt64 Period)noexcept(true)
{
	StartTimer(DueTime,Period);
}
//---------------------------------------------------------------------------
void cWindowMessageQueueDispatch::cAsyncTimer::Stop(void)noexcept(true)
{
	ClearTimer();
}
//---------------------------------------------------------------------------
void cWindowMessageQueueDispatch::cAsyncTimer::StartTimer(uInt64 DueTime,uInt64 Period)noexcept(true)
{
	ufInt8 PrevState=0;
	if(fTimerState.Acquire.CmpXchg(PrevState,1)){
		if(fReference!=nullptr)
			rIncReference(fReference,'time');
	}
	else{
		if(PrevState!=2)
			return;	// transisting state
		// try to restart timer
		if(fTimerState.Acquire.CmpStore(2,1)==false){
			// failed to restart timer
			return;
		}
	}
	fDueTime=DueTime;
	fPeriod=Period;
	fThreadPoolTimer->Start(fDueTime,0);

	fTimerState.Release.Store(2);
}
//---------------------------------------------------------------------------
void cWindowMessageQueueDispatch::cAsyncTimer::ClearTimer(void)noexcept(true)
{
	if(fTimerState.Acquire.CmpStore(2,3)==false){
		return;
	}

	fThreadPoolTimer->Stop();

	if(fReference!=nullptr)
		rDecReference(fReference,'time');

	fTimerState.Release.Store(0);
}
//---------------------------------------------------------------------------
void cWindowMessageQueueDispatch::cAsyncTimer::cThreadPoolTimerProcedure::Execute(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cAsyncTimer::fThreadPoolTimerProcedure);
	return Host->ThreadPoolTimerHit();
}
//---------------------------------------------------------------------------
void cWindowMessageQueueDispatch::cAsyncTimer::ThreadPoolTimerHit(void)noexcept(true)
{
	if(fTimerState.Acquire.Load()!=2)
		return;
			
	fMessageThread->Execute(&fInnerReference,&fTimerDispatchProcedure);
}
//---------------------------------------------------------------------------
void cWindowMessageQueueDispatch::cAsyncTimer::cTimerDispatchProcedure::Execute(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cAsyncTimer::fTimerDispatchProcedure);
	return Host->TimerHit();
}
//---------------------------------------------------------------------------
void cWindowMessageQueueDispatch::cAsyncTimer::TimerHit(void)noexcept(true)
{
	fProcedure->Execute();

	if(fPeriod==0){
		return;
	}
	auto Now=cnSystem::GetSystemTimeNow();
	fDueTime+=fPeriod;
	if(fDueTime<Now){
		fDueTime=Now;
		fMessageThread->Execute(&fInnerReference,&fTimerDispatchProcedure);
	}
	else{
		fThreadPoolTimer->Start(fDueTime,0);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWindowMessageUIThread::cWindowMessageUIThread(aClsRef<cWindowMessageThread> MessageThread)noexcept(true)
	: fMessageThread(cnVar::MoveCast(MessageThread))
	, fDispatch(iCreate<cWindowMessageQueueDispatch>(fMessageThread))
{
}
//---------------------------------------------------------------------------
cWindowMessageUIThread::~cWindowMessageUIThread()noexcept(true)
{
}
//---------------------------------------------------------------------------
aCls<cWindowMessageThread>* cWindowMessageUIThread::GetMessageThread(void)const noexcept(true)
{
	return fMessageThread;
}
//---------------------------------------------------------------------------
bool cWindowMessageUIThread::IsCurrentThread(void)noexcept(true)
{
	return fMessageThread->IsCurrentThread();
}
//---------------------------------------------------------------------------
iDispatch* cWindowMessageUIThread::GetDispatch(bool)noexcept(true)
{
	return fDispatch;
}
//---------------------------------------------------------------------------
iUIKeyboard* cWindowMessageUIThread::GetDefaultKeyboard(void)noexcept(true)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iUIMouse* cWindowMessageUIThread::GetDefaultMouse(void)noexcept(true)
{
	return nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWindowMessageUIApplication::cWindowMessageUIApplication(iPtr<cWindowMessageUIThread> Thread)noexcept(true)
	: fUIThread(cnVar::MoveCast(Thread))
{
}
//---------------------------------------------------------------------------
cWindowMessageUIApplication::~cWindowMessageUIApplication()noexcept(true)
{
}
//---------------------------------------------------------------------------
iUIThread* cWindowMessageUIApplication::GetMainUIThread(void)noexcept(true)
{
	return fUIThread;
}
//---------------------------------------------------------------------------
bool cWindowMessageUIApplication::InsertHandler(iWindowsUISessionHandler *SessionHandler)noexcept(true)
{
	if(fUIThread->GetMessageThread()->IsCurrentThread()==false)
		return false;

	fHandlers.Insert(SessionHandler);
	return true;
}
//---------------------------------------------------------------------------
bool cWindowMessageUIApplication::RemoveHandler(iWindowsUISessionHandler *SessionHandler)noexcept(true)
{
	if(fUIThread->GetMessageThread()->IsCurrentThread()==false)
		return false;

	fHandlers.Remove(SessionHandler);
	return true;
}
//---------------------------------------------------------------------------
void cWindowMessageUIApplication::UIMain(void)noexcept(true)
{
	auto MessageThread=fUIThread->GetMessageThread();
	if(MessageThread->IsCurrentThread()==false)
		return;

	if(fUISession)
		return;

	fUISession=true;
	fUITerminate=false;

	cSeqList<iWindowsUISessionHandler*> Handlers;
	Handlers=fHandlers.Storage();
	for(auto Handler : Handlers){
		Handler->UISessionStart();
	}
	// message loop
	while(!fUITerminate){
		MessageThread->MessageLoop();
	}


	fUISession=false;

	Handlers=fHandlers.Storage();
	for(auto Handler : Handlers){
		Handler->UISessionExit();
	}
}
//---------------------------------------------------------------------------
void cWindowMessageUIApplication::CloseUISession(void)noexcept(true)
{
	if(fUISession){
		fUITerminate=true;
		auto MessageThread=fUIThread->GetMessageThread();
		MessageThread->PostQuitMessageLoop(0);
	}
}
//---------------------------------------------------------------------------
