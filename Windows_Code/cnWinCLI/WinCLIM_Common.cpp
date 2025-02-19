#include "WinCLIM_Common.h"

using namespace cnLibrary;
using namespace cnWin;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cGCRef::mIsAllocated(void)const noexcept(true)
{
	return const_cast<cGCRef*>(this)->fHandle.IsAllocated;
}
//---------------------------------------------------------------------------
void cGCRef::mClear(void)noexcept(true)
{
	return Clear();
}
//---------------------------------------------------------------------------
void cGCRef::mFree(void)noexcept(true)
{
	return Free();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rcNativeCaller_CLICommon::rcNativeCaller_CLICommon(void *CPP)
	: CPP(CPP)
{
}
//---------------------------------------------------------------------------
System::Action^ mbcActionDelegate::GetAction(void)const noexcept(true)
{
	return fAction.Get();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcNativeProcedureDelegate::mSetup(iProcedure *Procedure)noexcept(true)
{
	auto Caller=gcnew rcNativeCaller_CLICommon(Procedure);
	auto CallerAction=gcnew System::Action(Caller,&rcNativeCaller_CLICommon::iProcedure_Execute);
	fAction.Alloc(CallerAction);
}
//---------------------------------------------------------------------------
void mcNativeProcedureDelegate::mClear(void)noexcept(true)
{
	fAction.Free();
}
//---------------------------------------------------------------------------
void rcNativeCaller_CLICommon::iProcedure_Execute(void)
{
	return static_cast<iProcedure*>(CPP)->Execute();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mbcCLICallback::mSetup(void)noexcept(true)
{
	auto Caller=gcnew rcNativeCaller_CLICommon(this);
	auto CallerAction=gcnew System::Action(Caller,&rcNativeCaller_CLICommon::mbcCLICallback_Run);
	fAction.Alloc(CallerAction);
}
//---------------------------------------------------------------------------
void mbcCLICallback::mClear(void)noexcept(true)
{
	fAction.Free();
}
//---------------------------------------------------------------------------
void rcNativeCaller_CLICommon::mbcCLICallback_Run(void)
{
	return static_cast<mbcCLICallback*>(CPP)->CLIExecute();
}
//---------------------------------------------------------------------------
