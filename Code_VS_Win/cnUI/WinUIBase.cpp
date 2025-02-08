#include "WinUIBase.h"

using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
iPtr<iWindowClient> cnUI::CreateUIWindow(HWND Parent,const wchar_t *WindowText,DWORD Style,DWORD ExStyle)noexcept(true)
{
	auto WindowClient=cnWindows::CreateWindowClient();
	if(WindowClient==nullptr){
		return nullptr;
	}

	auto Window=cnWindows::CreateWindowHandle(Parent,WindowText,Style,ExStyle);
	if(Window==nullptr)
		return nullptr;

	Window->SetClient(WindowClient);
	return WindowClient;
}
//---------------------------------------------------------------------------
static bool PointInWindow(HWND hWnd,LPARAM lParam)noexcept(true){
	auto &Pos=MAKEPOINTS(lParam);
	RECT crc;
	::GetClientRect(hWnd,&crc);
	if(Pos.x<crc.left || Pos.x>crc.right){
		return false;
	}
	if(Pos.y<crc.top || Pos.y>crc.bottom){
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
WindowComponent::WindowComponent()noexcept(true)
{
}
//---------------------------------------------------------------------------
WindowComponent::~WindowComponent()noexcept(true)
{
}
//---------------------------------------------------------------------------
iWindow* WindowComponent::GetWindow(void)const noexcept(true)
{
	return fWindow;
}
//---------------------------------------------------------------------------
void WindowComponent::SetWindow(iWindow *Window)noexcept(true)
{
	if(fWindow!=nullptr){
		WindowClear();
	}
	fWindow=Window;
	if(fWindow!=nullptr){
		WindowSetup();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
WindowControl::WindowControl()noexcept(true)
{
}
//---------------------------------------------------------------------------
WindowControl::~WindowControl()noexcept(true)
{
	SetWindow(nullptr);
}
//---------------------------------------------------------------------------
void WindowControl::UpdateComponentOrder(void)noexcept(true)
{
	if(fWindow!=nullptr){
		fWindow->InsertMessageHandler(this,ComponentOrder);
	}
}
//---------------------------------------------------------------------------
void WindowControl::WindowSetup(void)noexcept(true)
{
	fWindow->InsertMessageHandler(this,ComponentOrder);
}
//---------------------------------------------------------------------------
void WindowControl::WindowClear(void)noexcept(true)
{
	fWindow->RemoveMessageHandler(this);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
WindowClient::WindowClient()noexcept(true)
{
	fWindowClient=cnWindows::CreateWindowClient();
}
//---------------------------------------------------------------------------
WindowClient::~WindowClient()noexcept(true)
{
	SetClientView(nullptr);
	SetWindow(nullptr);
}
//---------------------------------------------------------------------------
iWindowClient* WindowClient::GetWindowClient(void)const noexcept(true)
{
	return fWindowClient;
}
//---------------------------------------------------------------------------
iUIWindow* WindowClient::GetUIWindow(void)const noexcept(true)
{
	return iCast<iUIWindow>(fWindowClient);
}
//---------------------------------------------------------------------------
void WindowClient::WindowSetup(void)noexcept(true)
{
	WindowComponent::WindowSetup();

	fWindow->SetClient(fWindowClient);
}
//---------------------------------------------------------------------------
void WindowClient::WindowClear(void)noexcept(true)
{
	fWindow->SetClient(nullptr);

	WindowComponent::WindowClear();
}
//---------------------------------------------------------------------------
iUIView* WindowClient::GetClientView(void)const noexcept(true)
{
	auto Window=GetUIWindow();
	return Window->GetClient();
}
//---------------------------------------------------------------------------
void WindowClient::SetClientView(iUIView *View)noexcept(true)
{
	auto Window=GetUIWindow();
	Window->SetClient(View);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
WindowFrame::WindowFrame()noexcept(true)
{
	fWindowFrame=cnWindows::CreateWindowFrame();
}
//---------------------------------------------------------------------------
WindowFrame::~WindowFrame()noexcept(true)
{
	SetWindow(nullptr);
}
//---------------------------------------------------------------------------
iWindowFrame* WindowFrame::GetWindowFrame(void)const noexcept(true)
{
	return fWindowFrame;
}
//---------------------------------------------------------------------------
iUIWindow* WindowFrame::GetUIWindow(void)const noexcept(true)
{
	return iCast<iUIWindow>(fWindowFrame);
}
//---------------------------------------------------------------------------
void WindowFrame::WindowSetup(void)noexcept(true)
{
	WindowComponent::WindowSetup();

	fWindow->SetFrame(fWindowFrame);
}
//---------------------------------------------------------------------------
void WindowFrame::WindowClear(void)noexcept(true)
{
	fWindow->SetFrame(nullptr);

	WindowComponent::WindowClear();
}
//---------------------------------------------------------------------------
iUIView* WindowFrame::GetClientView(void)const noexcept(true)
{
	auto Window=GetUIWindow();
	return Window->GetClient();
}
//---------------------------------------------------------------------------
void WindowFrame::SetClientView(iUIView *View)noexcept(true)
{
	auto Window=GetUIWindow();
	Window->SetClient(View);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cPopupStaticWindow::cPopupStaticWindow()noexcept(true)
{
	fWindowClient=cnWindows::CreateWindowClient();
}
//---------------------------------------------------------------------------
cPopupStaticWindow::~cPopupStaticWindow()noexcept(true)
{
}
//---------------------------------------------------------------------------
iWindow* cPopupStaticWindow::GetWindow(void)const noexcept(true)
{
	return fWindow;
}
//---------------------------------------------------------------------------
HWND cPopupStaticWindow::GetWindowHandle(void)const noexcept(true)
{
	if(fWindow==nullptr)
		return nullptr;
	return fWindow->GetWindowHandle();
}
//---------------------------------------------------------------------------
iWindowClient* cPopupStaticWindow::GetWindowClient(void)const noexcept(true)
{
	return fWindowClient;
}
//---------------------------------------------------------------------------
iUIWindow* cPopupStaticWindow::GetUIWindow(void)const noexcept(true)
{
	return iCast<iUIWindow>(fWindowClient);
}
//---------------------------------------------------------------------------
iUIView* cPopupStaticWindow::GetClientView(void)const noexcept(true)
{
	auto Wnd=GetUIWindow();
	return Wnd->GetClient();
}
//---------------------------------------------------------------------------
bool cPopupStaticWindow::SetClientView(iUIView *View)noexcept(true)
{
	auto Wnd=GetUIWindow();
	return Wnd->SetClient(View);
}
//---------------------------------------------------------------------------
void cPopupStaticWindow::SetupOwnerWindow(HWND OwnerWindowHandle)noexcept(true)
{
	if(fWindow==nullptr){
		fWindow=cnWindows::CreateWindowHandle(OwnerWindowHandle,WindowTitle,WS_POPUP|WindowStyle,WindowExStyle);
	}
	else{
		HWND OwnerRootWindow=::GetAncestor(OwnerWindowHandle,GA_ROOT);
		HWND WindowHandle=fWindow->GetWindowHandle();
		cnUI::HWNDSetOwner(WindowHandle,OwnerRootWindow);
	}
}
//---------------------------------------------------------------------------
void cPopupStaticWindow::SetupOwner(iUIView *Owner)noexcept(true)
{
	HWND OwnerWindowHandle=GetWindowHandleFromUIView(Owner);
	SetupOwnerWindow(OwnerWindowHandle);
}
//---------------------------------------------------------------------------
void cPopupStaticWindow::MovePopup(iUIArea *Relative,const cUIPoint &WindowSize,const cUIPoint &AlignPos,const cUIPoint &AlignSize,eAroundAlignment Align)noexcept(true)
{
	HWND WindowHandle=GetWindowHandle();
	if(WindowHandle==nullptr){
		return;
	}
	return MoveWindowAlign(WindowHandle,Relative,WindowSize,AlignPos,AlignSize,Align);
}
//---------------------------------------------------------------------------
void cPopupStaticWindow::MovePopup(iUIArea *Relative,const cUIPoint &Pos,const cUIPoint &Size)noexcept(true)
{
	HWND WindowHandle=GetWindowHandle();
	if(WindowHandle==nullptr){
		return;
	}
	cUIPoint ScreenPos;
	if(Relative->GetPosition(nullptr,ScreenPos)==false)
		return;

	::MoveWindow(WindowHandle,ScreenPos.x+Pos.x,ScreenPos.y+Pos.y,Size.x,Size.y,FALSE);
}
//---------------------------------------------------------------------------
void cPopupStaticWindow::ShowPopup(void)noexcept(true)
{
	HWND WindowHandle=GetWindowHandle();
	if(WindowHandle==nullptr){
		return;
	}
	// show window and move to the top
	::SetWindowPos(WindowHandle,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);
}
//---------------------------------------------------------------------------
void cPopupStaticWindow::HidePopup(void)noexcept(true)
{
	HWND WindowHandle=GetWindowHandle();
	if(WindowHandle==nullptr){
		return;
	}
	::ShowWindow(WindowHandle,SW_HIDE);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cPopupWindow::cPopupWindow()noexcept(true)
{
	fWindowClient=cnWindows::CreateOwnerFocusWindowClient();
}
//---------------------------------------------------------------------------
cPopupWindow::~cPopupWindow()noexcept(true)
{
	if(fWindow!=nullptr){
		fWindow->RemoveMessageHandler(this);

		fWindow->SetClient(nullptr);
	}
}
//---------------------------------------------------------------------------
iWindow* cPopupWindow::GetWindow(void)const noexcept(true)
{
	return fWindow;
}
//---------------------------------------------------------------------------
HWND cPopupWindow::GetWindowHandle(void)const noexcept(true)
{
	if(fWindow==nullptr)
		return nullptr;
	return fWindow->GetWindowHandle();
}
//---------------------------------------------------------------------------
iWindowClient* cPopupWindow::GetWindowClient(void)const noexcept(true)
{
	return fWindowClient;
}
//---------------------------------------------------------------------------
iUIWindow* cPopupWindow::GetUIWindow(void)const noexcept(true)
{
	return iCast<iUIWindow>(fWindowClient);
}
//---------------------------------------------------------------------------
iUIView* cPopupWindow::GetClientView(void)const noexcept(true)
{
	auto Wnd=GetUIWindow();
	return Wnd->GetClient();
}
//---------------------------------------------------------------------------
bool cPopupWindow::SetClientView(iUIView *View)noexcept(true)
{
	auto Wnd=GetUIWindow();
	return Wnd->SetClient(View);
}
//---------------------------------------------------------------------------
void cPopupWindow::SetupOwnerWindow(HWND OwnerWindow)noexcept(true)
{
	if(fWindow==nullptr){
		fWindow=cnWindows::CreateWindowHandle(OwnerWindow,WindowTitle,WS_POPUP|WindowStyle,WindowExStyle);
		fWindow->InsertMessageHandler(this);
		fWindow->SetClient(fWindowClient);
	}
	else{
		// get root window
		HWND OwnerRootWindow=::GetAncestor(OwnerWindow,GA_ROOT);
		HWND WindowHandle=fWindow->GetWindowHandle();
		cnUI::HWNDSetOwner(WindowHandle,OwnerRootWindow);

		fWindowClient->UpdateOwner();
	}
}
//---------------------------------------------------------------------------
void cPopupWindow::SetupOwner(iUIView *Owner)noexcept(true)
{
	HWND OwnerWindowHandle=GetWindowHandleFromUIView(Owner);
	SetupOwnerWindow(OwnerWindowHandle);
}
//---------------------------------------------------------------------------
void cPopupWindow::MovePopup(iUIArea *Relative,const cUIPoint &WindowSize,const cUIPoint &AlignPos,const cUIPoint &AlignSize,eAroundAlignment Align)noexcept(true)
{
	HWND WindowHandle=GetWindowHandle();
	if(WindowHandle==nullptr){
		return;
	}
	return MoveWindowAlign(WindowHandle,Relative,WindowSize,AlignPos,AlignSize,Align);
}
//---------------------------------------------------------------------------
void cPopupWindow::MovePopup(iUIArea *Relative,const cUIPoint &Pos,const cUIPoint &Size)noexcept(true)
{
	HWND WindowHandle=GetWindowHandle();
	if(WindowHandle==nullptr){
		return;
	}
	cUIPoint ScreenPos;
	if(Relative->GetPosition(nullptr,ScreenPos)==false)
		return;

	::MoveWindow(WindowHandle,ScreenPos.x+Pos.x,ScreenPos.y+Pos.y,Size.x,Size.y,FALSE);
}
//---------------------------------------------------------------------------
void cPopupWindow::ShowPopup(void)noexcept(true)
{
	HWND WindowHandle=GetWindowHandle();
	if(WindowHandle==nullptr){
		return;
	}
	// show window and move to the top
	::SetWindowPos(WindowHandle,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);
}
//---------------------------------------------------------------------------
void cPopupWindow::HidePopup(void)noexcept(true)
{
	HWND WindowHandle=GetWindowHandle();
	if(WindowHandle==nullptr){
		return;
	}
	::ShowWindow(WindowHandle,SW_HIDE);
}
//---------------------------------------------------------------------------
bool cPopupWindow::IsPopuped(void)noexcept(true)
{
	HWND WindowHandle=GetWindowHandle();
	if(WindowHandle==nullptr){
		return false;
	}
	return FALSE!=::IsWindowVisible(WindowHandle);
}
//---------------------------------------------------------------------------
bool cPopupWindow::WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)noexcept(true)
{
	HWND WindowHandle=fWindow->GetWindowHandle();
	switch(MsgParam.Code){
	case WM_WINDOWPOSCHANGED:
		{
			auto *WindowPos=reinterpret_cast<WINDOWPOS*>(MsgParam.lParam);
			if(WindowPos->flags&SWP_SHOWWINDOW){
				::SetCapture(WindowHandle);
			}
			else if(WindowPos->flags&SWP_HIDEWINDOW){
				::ReleaseCapture();
			}
		}
		Result=0;
		return false;
	case WM_MOUSEACTIVATE:
		Result=MA_NOACTIVATE;
		return true;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		if(PointInWindow(WindowHandle,MsgParam.lParam)==FALSE){
			::ReleaseCapture();
			Result=0;
			return true;
		}
		break;
	case WM_CAPTURECHANGED:
		if(WindowHandle!=reinterpret_cast<HWND>(MsgParam.lParam)){
			::ShowWindow(WindowHandle,SW_HIDE);
		}
		break;
	}
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModalDialog::ModalDialog()noexcept(true)
{
	fDialogProvider=cnWindows::CreateModalDialogProvider();

	auto DialogWindow=iCast<iWindow>(fDialogProvider);
	DialogWindow->InsertMessageHandler(this);
}
//---------------------------------------------------------------------------
ModalDialog::~ModalDialog()noexcept(true)
{
	auto DialogWindow=iCast<iWindow>(fDialogProvider);
	DialogWindow->RemoveMessageHandler(this);
}
//---------------------------------------------------------------------------
INT_PTR ModalDialog::Modal(HWND Parent,const wchar_t *WindowText,LONG X,LONG Y,LONG Width,LONG Height,DWORD Style,DWORD ExStyle)noexcept(true)
{
	return fDialogProvider->ModalDialog(Parent,WindowText,X,Y,Width,Height,Style,ExStyle);
}
//---------------------------------------------------------------------------
INT_PTR ModalDialog::Modal(HWND Parent,HINSTANCE hInstance,LPCWSTR TemplateName)noexcept(true)
{
	return fDialogProvider->ModalDialogTemplate(Parent,hInstance,TemplateName);
}
//---------------------------------------------------------------------------
INT_PTR ModalDialog::Modal(HWND Parent,HINSTANCE hInstance,LPCDLGTEMPLATEW DialogTemplate)noexcept(true)
{
	return fDialogProvider->ModalDialogTemplateIndirect(Parent,hInstance,DialogTemplate);
}
//---------------------------------------------------------------------------
void ModalDialog::WindowAttached(void)noexcept(true)
{
	auto DialogWindow=iCast<iWindow>(fDialogProvider);
	fWindowHandle=DialogWindow->GetWindowHandle();
	RECT crc;
	::GetClientRect(fWindowHandle,&crc);
	ModalDialogCreateClient(fWindowHandle,crc.left,crc.top,crc.right,crc.bottom);
}
//---------------------------------------------------------------------------
void ModalDialog::WindowDetached(void)noexcept(true)
{
	fWindowHandle=nullptr;
}
//---------------------------------------------------------------------------
void ModalDialog::WindowMessageProcessed(LRESULT,const cWindowMessageParam &MsgParam)noexcept(true)
{
	switch(MsgParam.Code){
	case WM_WINDOWPOSCHANGED:
		{
			auto *WindowPos=reinterpret_cast<WINDOWPOS*>(MsgParam.lParam);
			if((WindowPos->flags&SWP_NOSIZE)==0){
				RECT crc;
				::GetClientRect(fWindowHandle,&crc);
				ModalDialogMoveClient(crc.left,crc.top,crc.right-crc.left,crc.bottom-crc.top);
			}
		}
		break;
	}
}
//---------------------------------------------------------------------------
bool ModalDialog::WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)noexcept(true)
{Result;
	switch(MsgParam.Code){
	case WM_CLOSE:
		::EndDialog(fWindowHandle,0);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
struct cShowModalWindowData
{
	iWindowProvider *ClientProvider;
	const wchar_t *Text;
};
//---------------------------------------------------------------------------
static INT_PTR CALLBACK DialogProcedure_ShowModalWindow(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam)noexcept(true)
{wParam;
	switch(Msg){
	case WM_INITDIALOG:
		{
			auto ShowData=reinterpret_cast<cShowModalWindowData*>(lParam);
			if(ShowData->Text!=nullptr){
				::SetWindowTextW(hDlg,ShowData->Text);
			}
			if(ShowData->ClientProvider!=nullptr){
				RECT crc;
				::GetClientRect(hDlg,&crc);
				ShowData->ClientProvider->WindowCreate(hDlg,nullptr,WS_CHILD|WS_VISIBLE,0,crc.left,crc.top,crc.right-crc.left,crc.bottom-crc.top,0);
			}
		}
		return FALSE;
	case WM_WINDOWPOSCHANGED:
		{
			auto WndPos=reinterpret_cast<const WINDOWPOS*>(lParam);
			if((WndPos->flags&SWP_NOSIZE)==0){
				HWND ClientWnd=::GetDlgItem(hDlg,0);
				if(ClientWnd!=nullptr){
					RECT crc;
					::GetClientRect(hDlg,&crc);
					::MoveWindow(ClientWnd,crc.left,crc.top,crc.right-crc.left,crc.bottom-crc.top,TRUE);
				}
			}
		}
		break;
	case WM_COMMAND:
		::EndDialog(hDlg,LOWORD(wParam));
		return TRUE;
	//case WM_CLOSE:
	//	::EndDialog(hDlg,0);
	//	return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------
INT_PTR cnUI::ShowModalWindow(iWindowProvider *ClientProvider,HWND Parent,const wchar_t *Text,int x,int y,int w,int h,DWORD Style,DWORD ExStyle)noexcept(true)
{
	struct cDialogTemplateData{
		DWORD Style;
		DWORD ExStyle;
		WORD  DialogItemCount;
		short x;
		short y;
		short cx;
		short cy;
		WORD NoMenu;
		WORD NoClass;
		WORD NoTitle;
	};
	cDialogTemplateData DlgTemplateData;
	DlgTemplateData.Style=Style & (~DS_SETFONT);
	DlgTemplateData.ExStyle=ExStyle;
	DlgTemplateData.DialogItemCount=0;
	DlgTemplateData.x=static_cast<short>(x);
	DlgTemplateData.y=static_cast<short>(y);
	DlgTemplateData.cx=static_cast<short>(w);
	DlgTemplateData.cy=static_cast<short>(h);
	DlgTemplateData.NoMenu=0;
	DlgTemplateData.NoClass=0;
	DlgTemplateData.NoTitle=0;

	cShowModalWindowData WindowShowData={
		ClientProvider,
		Text
	};

	return ::DialogBoxIndirectParamW(nullptr,reinterpret_cast<LPCDLGTEMPLATEW>(&DlgTemplateData),Parent,DialogProcedure_ShowModalWindow,reinterpret_cast<LPARAM>(&WindowShowData));
}
//---------------------------------------------------------------------------
