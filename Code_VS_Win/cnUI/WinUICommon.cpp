#include "WinUICommon.h"

using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
HWND cnUI::GetWindowHandleFromUIWindow(iUIArea *Area)
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
HWND cnUI::GetWindowHandleFromUIView(iUIView *View)
{
	if(View==nullptr)
		return nullptr;
	return GetWindowHandleFromUIWindow(View->GetWindow());
}
//---------------------------------------------------------------------------
//HWND cnUI::GetComponentWindowHandle(iWindowComponent *WindowComponent)
//{
//	if(WindowComponent==nullptr){
//		return nullptr;
//	}
//	auto Window=WindowComponent->GetWindow();
//	if(Window==nullptr)
//		return nullptr;
//	return Window->GetWindowHandle();
//}
//---------------------------------------------------------------------------
void cnUI::HWNDSetOwner(HWND WindowHandle,HWND OwnerHandle)
{
	if(WindowHandle==nullptr){
		return;
	}
	auto CurOwnerWindow=reinterpret_cast<HWND>(::GetWindowLongPtrW(WindowHandle,GWLP_HWNDPARENT));
	if(CurOwnerWindow!=OwnerHandle){
		::SetWindowLongPtrW(WindowHandle,GWLP_HWNDPARENT,reinterpret_cast<LONG_PTR>(OwnerHandle));
	}
}
//---------------------------------------------------------------------------
cUIPoint cnUI::AlignWindowRect(iUIArea *Relative,const cUIPoint &WindowSize,const cUIPoint &AlignPos,const cUIPoint &AlignSize,eAroundAlignment Align)
{
	cUIPoint ScreenPos;
	if(Relative!=nullptr && Relative->GetPosition(nullptr,ScreenPos)){
		ScreenPos.x+=AlignPos.x;
		ScreenPos.y+=AlignPos.y;
	}
	else{
		ScreenPos=AlignPos;
	}

	RECT WorkRect;
	::SystemParametersInfoW(SPI_GETWORKAREA,0,&WorkRect,0);

	ScreenPos.x-=WorkRect.left;
	ScreenPos.y-=WorkRect.top;
	
	Float32 ScreenSizeX=WorkRect.right-WorkRect.left;
	Float32 ScreenSizeY=WorkRect.bottom-WorkRect.top;

	return cnUI::RectAlignAroundRectLimited(Align,{ScreenSizeX,ScreenSizeY},ScreenPos,AlignSize,WindowSize);
}
//---------------------------------------------------------------------------
void cnUI::MoveWindowAlign(HWND WindowHandle,iUIArea *Relative,const cUIPoint &WindowSize,const cUIPoint &AlignPos,const cUIPoint &AlignSize,eAroundAlignment Align)
{
	auto WindowPos=AlignWindowRect(Relative,WindowSize,AlignPos,AlignSize,Align);
	::MoveWindow(WindowHandle,WindowPos.x,WindowPos.y,WindowSize.x,WindowSize.y,FALSE);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace{
class cSysMouseCursor : public iMouseCursorSetter
{
public:
	HCURSOR Cursor;
	cSysMouseCursor(HCURSOR Cursor):Cursor(Cursor){}

	virtual void cnLib_FUNC SetCursor(void)override{
		::SetCursor(Cursor);
	}
};

class cNullMouseCursor : public iMouseCursorSetter
{
public:
	virtual void cnLib_FUNC SetCursor(void)override{}
};
static cNullMouseCursor gNullMouseCursor;
}
//---------------------------------------------------------------------------
iInterface* cnUI::QueryNullMouseCursor(void)
{
	return &gNullMouseCursor;
}
//---------------------------------------------------------------------------
iPtr<iInterface> cnUI::QuerySystemMouseCursor(LPCWSTR ID)
{
	auto Cursor=::LoadCursorW(nullptr,ID);
	return iCreate<cSysMouseCursor>(Cursor);
}
//---------------------------------------------------------------------------
rPtr<iGDIFontHandle> cnUI::QueryGDIFont(const cUITextStyle &TextStyle)
{
	auto Device=cnWindows::QueryDCPaintDevice();
	return Device->QueryFont(TextStyle);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void WindowMessageHandler::WindowAttached(void){}
void WindowMessageHandler::WindowDetached(void){}
bool WindowMessageHandler::WindowMessage(LRESULT &,const cWindowMessageParam &){	return false;	}
void WindowMessageHandler::WindowMessageProcessed(LRESULT,const cWindowMessageParam&){}
//---------------------------------------------------------------------------
