/*- cnWin - Window --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-06-28                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnWin\Win_Common.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
class cWindowClass : public cnRTL::cnWinRTL::bcWindowClass
{
public:

	cWindowClass(const WNDCLASSEXW *ClassInfo);
	cWindowClass(const wchar_t *ClassName,WNDPROC WndClassProc,UINT style=0);
	~cWindowClass();
};
//---------------------------------------------------------------------------
struct cDialogEmptyTemplateData{
	DWORD		style;
	DWORD		exStyle;
	WORD		cDlgItems;
	short		x;
	short		y;
	short		cx;
	short		cy;
	WORD		zero_menu;
	WORD		zero_windowClass;
	WCHAR		zero_title;
};
//---------------------------------------------------------------------------
INT_PTR CALLBACK ModalDialogSubclassEmptyDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
//---------------------------------------------------------------------------
// ModalDialog
//	modal dialog box
INT_PTR ModalDialog(bcWindowSubclass *WindowSubclass,HWND Parent,HINSTANCE hInstance,LPCWSTR TemplateName);
// ModalDialog
//	modal dialog box
INT_PTR ModalDialog(bcWindowSubclass *WindowSubclass,HWND Parent,HINSTANCE hInstance,LPCDLGTEMPLATEW DialogTemplate);
//---------------------------------------------------------------------------
struct cModalDialogInitParameter
{
	bcWindowSubclass *WindowSubclass;
	LPCWSTR Title;
	LONG X;
	LONG Y;
	LONG W;
	LONG H;
};
// ModalDialog
//	modal dialog box
INT_PTR ModalDialog(const cModalDialogInitParameter &InitParameter,HWND Parent,DWORD Style,DWORD ExStyle);
//---------------------------------------------------------------------------
class cMessageThreadWindowClass : public cWindowClass
{
public:
	static cMessageThreadWindowClass gMessageThreadWindowClass;

	cMessageThreadWindowClass(const wchar_t *ClassName);

	static LRESULT CALLBACK MessageWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)noexcept(true);

	struct cWindowMessageThreadParam
	{
		aClsRef<cnRTL::cnWinRTL::cWindowMessageThread> MessageThread;
		cnRTL::cThreadOneTimeNotifier CallerNotify;
	};
	static DWORD WINAPI MessageThreadProcedure(LPVOID Parameter)noexcept(true);

	static aClsRef<cnRTL::cnWinRTL::cWindowMessageThread> StartMessageThread(void)noexcept(true);
};
iPtr<cnRTL::cnWinRTL::cWindowMessageQueueDispatch> CreateWindowMessageDispathThread(void)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
