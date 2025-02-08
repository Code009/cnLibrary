//---------------------------------------------------------------------------
#include "ThreadAsync.h"

using namespace cnLibrary;
using namespace cnRTL;

#pragma message (cnLib_FILE_LINE " : TODO - rename to ThreadScheduling")
#pragma message (cnLib_FILE_LINE " : TODO - cAsyncExecutionThreadSwitch change to template")

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcExclusiveProcedure::bcExclusiveProcedure()noexcept(true)
{
}
//---------------------------------------------------------------------------
bcExclusiveProcedure::~bcExclusiveProcedure()noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcExclusiveProcedure::ProcedureThreadRunLoop(void)noexcept(true)
{
	do{
		do{
			fExclusiveFlag.Continue();
		}while(Procedure());
	}while(!fExclusiveFlag.Release());
}
//---------------------------------------------------------------------------
void bcExclusiveProcedure::Run(void)noexcept(true)
{
	if(!fExclusiveFlag.Acquire()){
		return;
	}

	return ProcedureThreadRunLoop();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcAsyncExclusiveProcedure::bcAsyncExclusiveProcedure()noexcept(true)
{
}
//---------------------------------------------------------------------------
bcAsyncExclusiveProcedure::~bcAsyncExclusiveProcedure()noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcAsyncExclusiveProcedure::cProcedure::Execute(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcAsyncExclusiveProcedure::fProcedure);
	return Host->ProcedureThreadRunLoop();
}
//---------------------------------------------------------------------------
bcAsyncExclusiveProcedure::operator iProcedure*()noexcept(true)
{
	return &fProcedure;
}
//---------------------------------------------------------------------------
void bcAsyncExclusiveProcedure::RunAsync(iAsyncProcedure *AsyncProc)noexcept(true)
{
	cnLib_ASSERT(AsyncProc!=nullptr);
	if(fExclusiveFlag.Acquire()){
		AsyncProc->Start();
	}
}
//---------------------------------------------------------------------------
void bcAsyncExclusiveProcedure::RunAsyncAfter(iAsyncProcedure *AsyncProc,uIntn Count)noexcept(true)
{
	cnLib_ASSERT(AsyncProc!=nullptr);

	if(!fExclusiveFlag.Acquire()){
		return;
	}
	while(Count!=0){
		fExclusiveFlag.Continue();
		if(Procedure()==false){
			if(fExclusiveFlag.Release()){
				return;
			}
		}
		Count--;
	}
	// run out
	AsyncProc->Start();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cCancellationFlag::Reset(void)noexcept(true)
{
	fFlag=cfNormal;
}
//---------------------------------------------------------------------------
bool cCancellationFlag::Complete(void)noexcept(true)
{
	return fFlag.Release.CmpStore(cfNormal,cfCompleted);
}
//---------------------------------------------------------------------------
bool cCancellationFlag::Cancel(void)noexcept(true)
{
	return fFlag.Release.CmpStore(cfNormal,cfCancelled);
}
//---------------------------------------------------------------------------
bool cCancellationFlag::IsCancelled(void)const noexcept(true)
{
	return fFlag==cfCancelled;
}
//---------------------------------------------------------------------------
