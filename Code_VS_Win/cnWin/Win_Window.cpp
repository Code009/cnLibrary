#include "Win_Window.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWindowClass::cWindowClass(const WNDCLASSEXW *ClassInfo)
{
	fAtom=RegisterClassExW(ClassInfo);
}
//---------------------------------------------------------------------------
cWindowClass::cWindowClass(const wchar_t *ClassName,WNDPROC WndClassProc,UINT style)
{
	// register window class for UIObject
	WNDCLASSEXW WindowClassInfo;
	cnMemory::ZeroFill(&WindowClassInfo.cbClsExtra,sizeof(WNDCLASSEXW)-cnMemory::MemberOffset(&WNDCLASSEXW::cbClsExtra));
	WindowClassInfo.cbSize=sizeof(WNDCLASSEXW);
	WindowClassInfo.style=style;
	WindowClassInfo.lpfnWndProc=WndClassProc;
	WindowClassInfo.hInstance=gModuleHandle;
	WindowClassInfo.lpszClassName=ClassName;
		
	fAtom=RegisterClassExW(&WindowClassInfo);
}
//---------------------------------------------------------------------------
cWindowClass::~cWindowClass()
{
	if(fAtom!=0)
		UnregisterClassW(reinterpret_cast<LPCWSTR>(fAtom),gModuleHandle);	
}
//---------------------------------------------------------------------------
cWindowClass::operator LPCWSTR (){
	return reinterpret_cast<LPCWSTR>(fAtom);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
INT_PTR CALLBACK cnWin::ModalDialogSubclassEmptyDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{wParam;lParam;
	return FALSE;
}
//---------------------------------------------------------------------------
static INT_PTR CALLBACK TemplateModalDialogInitProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{wParam;
	switch(Message){
	case WM_INITDIALOG:
		{
			// replace dlg proc with empty proc
			::SetWindowLongPtrW(hWnd,DWLP_DLGPROC,reinterpret_cast<LONG_PTR>(ModalDialogSubclassEmptyDlgProc));
			// subclass window proc
			auto WindowSubclass=reinterpret_cast<bcWindowSubclass*>(lParam);
			WindowsInterface::WindowLocalSubclass(hWnd,WindowSubclass);
		}
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------
INT_PTR cnWin::ModalDialog(bcWindowSubclass *WindowSubclass,HWND Parent,HINSTANCE hInstance,LPCWSTR TemplateName)
{
	return ::DialogBoxParamW(hInstance,TemplateName,Parent,TemplateModalDialogInitProc,reinterpret_cast<LONG_PTR>(WindowSubclass));
}
//---------------------------------------------------------------------------
INT_PTR cnWin::ModalDialog(bcWindowSubclass *WindowSubclass,HWND Parent,HINSTANCE hInstance,LPCDLGTEMPLATEW DialogTemplate)
{
	return ::DialogBoxIndirectParamW(hInstance,DialogTemplate,Parent,TemplateModalDialogInitProc,reinterpret_cast<LONG_PTR>(WindowSubclass));
}
//---------------------------------------------------------------------------
static INT_PTR CALLBACK ModalDialogSubclassInitDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{wParam;lParam;
	switch(Message){
	case WM_INITDIALOG:
		{
			// replace dlg proc with empty proc
			::SetWindowLongPtrW(hWnd,DWLP_DLGPROC,reinterpret_cast<LONG_PTR>(ModalDialogSubclassEmptyDlgProc));
			// initialize parameter
			auto Parameter=reinterpret_cast<cModalDialogInitParameter*>(lParam);
			// set window title
			::SetWindowTextW(hWnd,Parameter->Title);
			// move window
			::MoveWindow(hWnd,Parameter->X,Parameter->Y,Parameter->W,Parameter->H,FALSE);
			// subclass window proc
			WindowsInterface::WindowLocalSubclass(hWnd,Parameter->WindowSubclass);
		}
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------
INT_PTR cnWin::ModalDialog(const cModalDialogInitParameter &InitParameter,HWND Parent,DWORD Style,DWORD ExStyle)
{
	cDialogEmptyTemplateData DialogTemplateData;
	DialogTemplateData.style=Style;
	DialogTemplateData.exStyle=ExStyle;
	DialogTemplateData.cDlgItems=0;
	DialogTemplateData.x=0;
	DialogTemplateData.y=0;
	DialogTemplateData.cx=0;
	DialogTemplateData.cy=0;
	DialogTemplateData.zero_menu=0;
	DialogTemplateData.zero_windowClass=0;
	DialogTemplateData.zero_title=0;
	return ::DialogBoxIndirectParamW(gModuleHandle,reinterpret_cast<LPCDLGTEMPLATE>(&DialogTemplateData),Parent,ModalDialogSubclassInitDlgProc,reinterpret_cast<LONG_PTR>(&InitParameter));
}
//---------------------------------------------------------------------------
