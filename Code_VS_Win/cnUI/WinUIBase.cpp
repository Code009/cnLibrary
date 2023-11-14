#include "WinUIBase.h"

using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
iPtr<iWindowClient> cnUI::CreateUIWindow(HWND Parent,const wchar_t *WindowText,DWORD Style,DWORD ExStyle)
{
	auto WindowClient=cnWindows::CreateWindowClient();
	if(WindowClient==nullptr){
		return nullptr;
	}

	auto Window=cnWindows::CreateHWND(Parent,WindowText,Style,ExStyle);
	if(Window==nullptr)
		return nullptr;

	WindowClient->SetWindow(Window);
	return WindowClient;
}
//---------------------------------------------------------------------------
static bool PointInWindow(HWND hWnd,LPARAM lParam){
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
WindowComponent::WindowComponent()
{
}
//---------------------------------------------------------------------------
WindowComponent::~WindowComponent()
{
}
//---------------------------------------------------------------------------
iWindow* WindowComponent::GetWindow(void)const
{
	return fWindow;
}
//---------------------------------------------------------------------------
void WindowComponent::SetWindow(iWindow *Window)
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
WindowControl::WindowControl()
{
}
//---------------------------------------------------------------------------
WindowControl::~WindowControl()
{
	SetWindow(nullptr);
}
//---------------------------------------------------------------------------
void WindowControl::UpdateComponentOrder(void)
{
	if(fWindow!=nullptr){
		fWindow->InsertMessageHandler(this,ComponentOrder);
	}
}
//---------------------------------------------------------------------------
void WindowControl::WindowSetup(void)
{
	fWindow->InsertMessageHandler(this,ComponentOrder);
}
//---------------------------------------------------------------------------
void WindowControl::WindowClear(void)
{
	fWindow->RemoveMessageHandler(this);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
WindowClient::WindowClient()
{
	fWindowClient=cnWindows::CreateWindowClient();
}
//---------------------------------------------------------------------------
WindowClient::~WindowClient()
{
	SetClientView(nullptr);
	SetWindow(nullptr);
}
//---------------------------------------------------------------------------
iWindowClient* WindowClient::GetWindowClient(void)const
{
	return fWindowClient;
}
//---------------------------------------------------------------------------
iUIWindow* WindowClient::GetUIWindow(void)const
{
	return iCast<iUIWindow>(fWindowClient);
}
//---------------------------------------------------------------------------
void WindowClient::WindowSetup(void)
{
	WindowComponent::WindowSetup();

	fWindowClient->SetWindow(fWindow);
}
//---------------------------------------------------------------------------
void WindowClient::WindowClear(void)
{
	fWindowClient->SetWindow(nullptr);

	WindowComponent::WindowClear();
}
//---------------------------------------------------------------------------
iUIView* WindowClient::GetClientView(void)const
{
	auto Window=GetUIWindow();
	return Window->GetClient();
}
//---------------------------------------------------------------------------
void WindowClient::SetClientView(iUIView *View)
{
	auto Window=GetUIWindow();
	Window->SetClient(View);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
WindowFrame::WindowFrame()
{
	fWindowFrame=cnWindows::CreateWindowFrame();
}
//---------------------------------------------------------------------------
WindowFrame::~WindowFrame()
{
	SetWindow(nullptr);
}
//---------------------------------------------------------------------------
iWindowFrame* WindowFrame::GetWindowFrame(void)const
{
	return fWindowFrame;
}
//---------------------------------------------------------------------------
iUIWindow* WindowFrame::GetUIWindow(void)const
{
	return iCast<iUIWindow>(fWindowFrame);
}
//---------------------------------------------------------------------------
void WindowFrame::WindowSetup(void)
{
	WindowComponent::WindowSetup();

	fWindowFrame->SetWindow(fWindow);
}
//---------------------------------------------------------------------------
void WindowFrame::WindowClear(void)
{
	fWindowFrame->SetWindow(nullptr);

	WindowComponent::WindowClear();
}
//---------------------------------------------------------------------------
iUIView* WindowFrame::GetClientView(void)const
{
	auto Window=GetUIWindow();
	return Window->GetClient();
}
//---------------------------------------------------------------------------
void WindowFrame::SetClientView(iUIView *View)
{
	auto Window=GetUIWindow();
	Window->SetClient(View);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cPopupStaticWindow::cPopupStaticWindow()
{
	fWindowClient=cnWindows::CreateWindowClient();
}
//---------------------------------------------------------------------------
cPopupStaticWindow::~cPopupStaticWindow()
{
}
//---------------------------------------------------------------------------
iWindow* cPopupStaticWindow::GetWindow(void)const
{
	return fWindow;
}
//---------------------------------------------------------------------------
HWND cPopupStaticWindow::GetWindowHandle(void)const
{
	if(fWindow==nullptr)
		return nullptr;
	return fWindow->GetWindowHandle();
}
//---------------------------------------------------------------------------
iWindowClient* cPopupStaticWindow::GetWindowClient(void)const
{
	return fWindowClient;
}
//---------------------------------------------------------------------------
iUIWindow* cPopupStaticWindow::GetUIWindow(void)const
{
	return iCast<iUIWindow>(fWindowClient);
}
//---------------------------------------------------------------------------
iUIView* cPopupStaticWindow::GetClientView(void)const
{
	auto Wnd=GetUIWindow();
	return Wnd->GetClient();
}
//---------------------------------------------------------------------------
bool cPopupStaticWindow::SetClientView(iUIView *View)
{
	auto Wnd=GetUIWindow();
	return Wnd->SetClient(View);
}
//---------------------------------------------------------------------------
void cPopupStaticWindow::SetupOwnerWindow(HWND OwnerWindowHandle)
{
	if(fWindow==nullptr){
		fWindow=cnWindows::CreateHWND(OwnerWindowHandle,WindowTitle,WS_POPUP|WindowStyle,WindowExStyle);
	}
	else{
		HWND OwnerRootWindow=::GetAncestor(OwnerWindowHandle,GA_ROOT);
		HWND WindowHandle=fWindow->GetWindowHandle();
		cnUI::HWNDSetOwner(WindowHandle,OwnerRootWindow);
	}
}
//---------------------------------------------------------------------------
void cPopupStaticWindow::SetupOwner(iUIView *Owner)
{
	HWND OwnerWindowHandle=GetWindowHandleFromUIView(Owner);
	SetupOwnerWindow(OwnerWindowHandle);
}
//---------------------------------------------------------------------------
void cPopupStaticWindow::MovePopup(iUIArea *Relative,const cUIPoint &WindowSize,const cUIPoint &AlignPos,const cUIPoint &AlignSize,eAroundAlignment Align)
{
	HWND WindowHandle=GetWindowHandle();
	if(WindowHandle==nullptr){
		return;
	}
	return MoveWindowAlign(WindowHandle,Relative,WindowSize,AlignPos,AlignSize,Align);
}
//---------------------------------------------------------------------------
void cPopupStaticWindow::MovePopup(iUIArea *Relative,const cUIPoint &Pos,const cUIPoint &Size)
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
void cPopupStaticWindow::ShowPopup(void)
{
	HWND WindowHandle=GetWindowHandle();
	if(WindowHandle==nullptr){
		return;
	}
	// show window and move to the top
	::SetWindowPos(WindowHandle,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);
}
//---------------------------------------------------------------------------
void cPopupStaticWindow::HidePopup(void)
{
	HWND WindowHandle=GetWindowHandle();
	if(WindowHandle==nullptr){
		return;
	}
	::ShowWindow(WindowHandle,SW_HIDE);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cPopupWindow::cPopupWindow()
{
	fWindowClient=cnWindows::CreateOwnerFocusWindowClient();
}
//---------------------------------------------------------------------------
cPopupWindow::~cPopupWindow()
{
	if(fWindow!=nullptr){
		fWindow->RemoveMessageHandler(this);

		fWindowClient->SetWindow(nullptr);
	}
}
//---------------------------------------------------------------------------
iWindow* cPopupWindow::GetWindow(void)const
{
	return fWindow;
}
//---------------------------------------------------------------------------
HWND cPopupWindow::GetWindowHandle(void)const
{
	if(fWindow==nullptr)
		return nullptr;
	return fWindow->GetWindowHandle();
}
//---------------------------------------------------------------------------
iWindowClient* cPopupWindow::GetWindowClient(void)const
{
	return fWindowClient;
}
//---------------------------------------------------------------------------
iUIWindow* cPopupWindow::GetUIWindow(void)const
{
	return iCast<iUIWindow>(fWindowClient);
}
//---------------------------------------------------------------------------
iUIView* cPopupWindow::GetClientView(void)const
{
	auto Wnd=GetUIWindow();
	return Wnd->GetClient();
}
//---------------------------------------------------------------------------
bool cPopupWindow::SetClientView(iUIView *View)
{
	auto Wnd=GetUIWindow();
	return Wnd->SetClient(View);
}
//---------------------------------------------------------------------------
void cPopupWindow::SetupOwnerWindow(HWND OwnerWindow)
{
	if(fWindow==nullptr){
		fWindow=cnWindows::CreateHWND(OwnerWindow,WindowTitle,WS_POPUP|WindowStyle,WindowExStyle);
		fWindow->InsertMessageHandler(this);
		fWindowClient->SetWindow(fWindow);
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
void cPopupWindow::SetupOwner(iUIView *Owner)
{
	HWND OwnerWindowHandle=GetWindowHandleFromUIView(Owner);
	SetupOwnerWindow(OwnerWindowHandle);
}
//---------------------------------------------------------------------------
void cPopupWindow::MovePopup(iUIArea *Relative,const cUIPoint &WindowSize,const cUIPoint &AlignPos,const cUIPoint &AlignSize,eAroundAlignment Align)
{
	HWND WindowHandle=GetWindowHandle();
	if(WindowHandle==nullptr){
		return;
	}
	return MoveWindowAlign(WindowHandle,Relative,WindowSize,AlignPos,AlignSize,Align);
}
//---------------------------------------------------------------------------
void cPopupWindow::MovePopup(iUIArea *Relative,const cUIPoint &Pos,const cUIPoint &Size)
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
void cPopupWindow::ShowPopup(void)
{
	HWND WindowHandle=GetWindowHandle();
	if(WindowHandle==nullptr){
		return;
	}
	// show window and move to the top
	::SetWindowPos(WindowHandle,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);
}
//---------------------------------------------------------------------------
void cPopupWindow::HidePopup(void)
{
	HWND WindowHandle=GetWindowHandle();
	if(WindowHandle==nullptr){
		return;
	}
	::ShowWindow(WindowHandle,SW_HIDE);
}
//---------------------------------------------------------------------------
bool cPopupWindow::IsPopuped(void)
{
	HWND WindowHandle=GetWindowHandle();
	if(WindowHandle==nullptr){
		return false;
	}
	return FALSE!=::IsWindowVisible(WindowHandle);
}
//---------------------------------------------------------------------------
bool cPopupWindow::WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)
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
ModalDialog::ModalDialog()
{
	fDialogProvider=cnWindows::CreateWindowProvider();

	auto DialogWindow=iCast<iWindow>(fDialogProvider);
	DialogWindow->InsertMessageHandler(this);
}
//---------------------------------------------------------------------------
ModalDialog::~ModalDialog()
{
	auto DialogWindow=iCast<iWindow>(fDialogProvider);
	DialogWindow->RemoveMessageHandler(this);
}
//---------------------------------------------------------------------------
INT_PTR ModalDialog::Modal(HWND Parent,const wchar_t *WindowText,LONG X,LONG Y,LONG Width,LONG Height,DWORD Style,DWORD ExStyle)
{
	return fDialogProvider->ModalDialog(Parent,WindowText,X,Y,Width,Height,Style,ExStyle);
}
//---------------------------------------------------------------------------
INT_PTR ModalDialog::Modal(HWND Parent,HINSTANCE hInstance,LPCWSTR TemplateName)
{
	return fDialogProvider->ModalDialogTemplate(Parent,hInstance,TemplateName);
}
//---------------------------------------------------------------------------
INT_PTR ModalDialog::Modal(HWND Parent,HINSTANCE hInstance,LPCDLGTEMPLATEW DialogTemplate)
{
	return fDialogProvider->ModalDialogTemplateIndirect(Parent,hInstance,DialogTemplate);
}
//---------------------------------------------------------------------------
void ModalDialog::WindowAttached(void)
{
	auto DialogWindow=iCast<iWindow>(fDialogProvider);
	fWindowHandle=DialogWindow->GetWindowHandle();
	RECT crc;
	::GetClientRect(fWindowHandle,&crc);
	ModalDialogCreateClient(fWindowHandle,crc.left,crc.top,crc.right,crc.bottom);
}
//---------------------------------------------------------------------------
void ModalDialog::WindowDetached(void)
{
	fWindowHandle=nullptr;
}
//---------------------------------------------------------------------------
void ModalDialog::WindowMessageProcessed(LRESULT,const cWindowMessageParam &MsgParam)
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
bool ModalDialog::WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)
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
static INT_PTR CALLBACK DialogProcedure_ShowModalWindow(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam)
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
				ShowData->ClientProvider->Create(hDlg,nullptr,WS_CHILD|WS_VISIBLE,0,crc.left,crc.top,crc.right-crc.left,crc.bottom-crc.top,0);
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
INT_PTR cnUI::ShowModalWindow(iWindowProvider *ClientProvider,HWND Parent,const wchar_t *Text,int x,int y,int w,int h,DWORD Style,DWORD ExStyle)
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
