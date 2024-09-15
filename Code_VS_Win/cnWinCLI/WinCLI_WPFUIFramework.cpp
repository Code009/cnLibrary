#include "WinCLI_WPFUIFramework.h"


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

/*
//---------------------------------------------------------------------------
iPtr<iUIView> cnWin::MakeUIViewFromViewTarget(cWPFUIViewTargetElementReference &Reference)noexcept
{
	if(TargetObject->CheckAccess()==false)
		return nullptr;

	auto WPFUIThread=mcWPFUIThread::mCurrentUIThread();
	if(WPFUIThread==nullptr)
		return nullptr;

	mcGCHandle<IWPFUIViewTargetElement,eGCHandleType::Normal> TargetHandle;
	TargetHandle.Alloc(Target);
	return DNetCreateUIView(WPFUIThread,TargetHandle);


	auto UIThread=cWPFUIThread::CurrentUIThread();
	if(UIThread==nullptr)
		return nullptr;

	cGCRef WPFViewHandle;
	MakeWPFView(WPFViewHandle);
	return iCreate<cWPFUIView>(UIThread,WPFViewHandle);

}
//---------------------------------------------------------------------------
*/