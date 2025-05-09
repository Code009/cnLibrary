﻿#include "Win_Window.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
INT_PTR CALLBACK cnWin::ModalDialogSubclassEmptyDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)noexcept(true)
{hWnd,Message,wParam;lParam;
	return FALSE;
}
//---------------------------------------------------------------------------
static INT_PTR CALLBACK TemplateModalDialogInitProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)noexcept(true)
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
INT_PTR cnWin::ModalDialog(bcWindowSubclass *WindowSubclass,HWND Parent,HINSTANCE hInstance,LPCWSTR TemplateName)noexcept(true)
{
	return ::DialogBoxParamW(hInstance,TemplateName,Parent,TemplateModalDialogInitProc,reinterpret_cast<LONG_PTR>(WindowSubclass));
}
//---------------------------------------------------------------------------
INT_PTR cnWin::ModalDialog(bcWindowSubclass *WindowSubclass,HWND Parent,HINSTANCE hInstance,LPCDLGTEMPLATEW DialogTemplate)noexcept(true)
{
	return ::DialogBoxIndirectParamW(hInstance,DialogTemplate,Parent,TemplateModalDialogInitProc,reinterpret_cast<LONG_PTR>(WindowSubclass));
}
//---------------------------------------------------------------------------
static INT_PTR CALLBACK ModalDialogSubclassInitDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)noexcept(true)
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
INT_PTR cnWin::ModalDialog(const cModalDialogInitParameter &InitParameter,HWND Parent,DWORD Style,DWORD ExStyle)noexcept(true)
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
cMessageThreadWindowClass::cMessageThreadWindowClass(const wchar_t *ClassName)noexcept(true)
	: cWindowClass(gModuleHandle,ClassName,MessageWindowProc,0)
{
}
//---------------------------------------------------------------------------
LRESULT CALLBACK cMessageThreadWindowClass::MessageWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)noexcept(true)
{
	if(cnWinRTL::cWindowMessageThread::cWindowMessageThread::MessageWindowProc(hWnd,uMsg,wParam,lParam)){
		return 0;
	}

	return ::DefWindowProcW(hWnd,uMsg,wParam,lParam);
}
//---------------------------------------------------------------------------
DWORD WINAPI cMessageThreadWindowClass::MessageThreadProcedure(LPVOID Parameter)noexcept(true)
{
	aClsRef<cnRTL::cnWinRTL::cWindowMessageThread> MessageThread;
	{
		auto ThreadParam=static_cast<cWindowMessageThreadParam*>(Parameter);
		ThreadParam->MessageThread=aClsCreate<cnWinRTL::cWindowMessageThread>();
		MessageThread=ThreadParam->MessageThread;

		HWND MessageWindow=::CreateWindowExW(0,gMessageThreadWindowClass,nullptr,0,0,0,0,0,HWND_MESSAGE,nullptr,gModuleHandle,nullptr);
	
		MessageThread->Setup(MessageWindow,::GetCurrentThreadId());
		
		ThreadParam->CallerNotify.Notify();
	}

	MessageThread->MessageLoop();
	return 0;
}
//---------------------------------------------------------------------------
aClsRef<cnRTL::cnWinRTL::cWindowMessageThread> cMessageThreadWindowClass::StartMessageThread(void)noexcept(true)
{
	cWindowMessageThreadParam ThreadParam;

	//ThreadParam.CallerNotify.Reset();

	HANDLE ThreadHandle=::CreateThread(nullptr,0,MessageThreadProcedure,&ThreadParam,0,nullptr);


	ThreadParam.CallerNotify.Wait();

	::CloseHandle(ThreadHandle);

	return cnVar::MoveCast(ThreadParam.MessageThread);
}
//---------------------------------------------------------------------------
iPtr<cnWinRTL::cWindowMessageQueueDispatch> cnWin::CreateWindowMessageDispathThread(void)noexcept(true)
{
	auto MessageThread=cMessageThreadWindowClass::StartMessageThread();
	if(MessageThread==nullptr)
		return nullptr;

	return iCreate<cnWinRTL::cWindowMessageQueueDispatch>(cnVar::MoveCast(MessageThread));
}
//---------------------------------------------------------------------------
