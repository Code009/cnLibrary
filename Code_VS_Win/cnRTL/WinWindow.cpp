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
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_WINXP
//---------------------------------------------------------------------------
LRESULT NTXPWindowSubclass::cDefaultProcedureCaller::Execute(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return ::DefSubclassProc(hWnd,uMsg,wParam,lParam);
}
//---------------------------------------------------------------------------
static constexpr UINT SubclassID=0xC009;
static LRESULT CALLBACK SubclassProcedure(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam,UINT_PTR uIdSubclass,DWORD_PTR dwRefData)
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
bool NTXPWindowSubclass::SetLocal(HWND WindowHandle,bcWindowSubclass *Subclass)
{
	return Set(WindowHandle,Subclass);
}
//---------------------------------------------------------------------------
bool NTXPWindowSubclass::Set(HWND WindowHandle,bcWindowSubclass *Subclass)
{
	if(::SetWindowSubclass(WindowHandle,SubclassProcedure,SubclassID,reinterpret_cast<DWORD_PTR>(Subclass))==FALSE){
		return false;
	}
	Subclass->DefaultWindowProcedure.ConstructAs<cDefaultProcedureCaller>();
	Subclass->SubclassAttached(WindowHandle);
	return true;
}
//---------------------------------------------------------------------------
void NTXPWindowSubclass::Restore(HWND WindowHandle,bcWindowSubclass *Subclass)
{
	// window destroyed
	Subclass->SubclassDetached();

	::RemoveWindowSubclass(WindowHandle,SubclassProcedure,SubclassID);
}
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT >= _WIN32_WINNT_WINXP
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWindowUIState::cWindowUIState()
{
}
//---------------------------------------------------------------------------
cWindowUIState::~cWindowUIState()
{
}
//---------------------------------------------------------------------------
LONG cWindowUIState::WindowAttached(HWND WindowHandle)
{
	fWindowValid=true;

	LONG StyleValue=::GetWindowLongW(WindowHandle,GWL_STYLE);

	fWindowVisible=(StyleValue&WS_VISIBLE)!=0;
	fWindowActive=WindowHandle==::GetActiveWindow();

	return StyleValue;
}

//---------------------------------------------------------------------------
void cWindowUIState::WindowDetached(void)
{
	fWindowValid=false;
}
//---------------------------------------------------------------------------
void cWindowUIState::AfterChildWindowCreated(HWND WindowHandle)
{
	LONG StyleValue=::GetWindowLongW(WindowHandle,GWL_STYLE);
	fWindowVisible=(0!=(StyleValue&WS_VISIBLE));
}
//---------------------------------------------------------------------------
bool cWindowUIState::WindowMessageCheckState(const cWindowMessageParam &MsgParam)
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
eUIState cWindowUIState::GetUIState(void)const
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
int cnWinRTL::GetWindowDPI(HWND hWnd)
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
HWND cnWinRTL::GetWindowHandleFromUIWindow(iUIArea *Area)
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
HWND cnWinRTL::GetWindowHandleFromUIView(iUIView *View)
{
	if(View==nullptr)
		return nullptr;
	return GetWindowHandleFromUIWindow(View->GetWindow());
}
//---------------------------------------------------------------------------
iWindow* cnWinRTL::GetWindowFromUIWindow(iUIArea *Area)
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
iWindow* cnWinRTL::GetWindowFromUIView(iUIView *View)
{
	if(View==nullptr)
		return nullptr;
	return GetWindowFromUIWindow(View->GetWindow());
}
//---------------------------------------------------------------------------
