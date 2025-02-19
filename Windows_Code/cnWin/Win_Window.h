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
INT_PTR CALLBACK ModalDialogSubclassEmptyDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)noexcept(true);
//---------------------------------------------------------------------------
// ModalDialog
//	modal dialog box
INT_PTR ModalDialog(bcWindowSubclass *WindowSubclass,HWND Parent,HINSTANCE hInstance,LPCWSTR TemplateName)noexcept(true);
// ModalDialog
//	modal dialog box
INT_PTR ModalDialog(bcWindowSubclass *WindowSubclass,HWND Parent,HINSTANCE hInstance,LPCDLGTEMPLATEW DialogTemplate)noexcept(true);
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
INT_PTR ModalDialog(const cModalDialogInitParameter &InitParameter,HWND Parent,DWORD Style,DWORD ExStyle)noexcept(true);
//---------------------------------------------------------------------------
class cMessageThreadWindowClass : public cnRTL::cnWinRTL::cWindowClass
{
public:
	static cMessageThreadWindowClass gMessageThreadWindowClass;

	cMessageThreadWindowClass(const wchar_t *ClassName)noexcept(true);

	static LRESULT CALLBACK MessageWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)noexcept(true);

	struct cWindowMessageThreadParam
	{
		aClsRef<cnRTL::cnWinRTL::cWindowMessageThread> MessageThread;
		cnRTL::cLocalSingleThreadNotification CallerNotify;
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
