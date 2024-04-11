//---------------------------------------------------------------------------
#include "ThreadAsync.h"

using namespace cnLibrary;
using namespace cnRTL;

#pragma message (cnLib_FILE_LINE " : TODO - rename to ThreadScheduling")
#pragma message (cnLib_FILE_LINE " : TODO - cAsyncExecutionThreadSwitch change to template")

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcExclusiveProcedure::bcExclusiveProcedure()noexcept
{
}
//---------------------------------------------------------------------------
bcExclusiveProcedure::~bcExclusiveProcedure()noexcept
{
}
//---------------------------------------------------------------------------
void bcExclusiveProcedure::ProcedureThreadRunLoop(void)noexcept
{
	do{
		do{
			fExclusiveFlag.Continue();
		}while(Procedure());
	}while(!fExclusiveFlag.Release());
}
//---------------------------------------------------------------------------
void bcExclusiveProcedure::Run(void)noexcept
{
	if(!fExclusiveFlag.Acquire()){
		return;
	}

	return ProcedureThreadRunLoop();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcAsyncExclusiveProcedure::bcAsyncExclusiveProcedure()noexcept
{
}
//---------------------------------------------------------------------------
bcAsyncExclusiveProcedure::~bcAsyncExclusiveProcedure()noexcept
{
}
//---------------------------------------------------------------------------
void bcAsyncExclusiveProcedure::cProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcAsyncExclusiveProcedure::fProcedure);
	return Host->ProcedureThreadRunLoop();
}
//---------------------------------------------------------------------------
bcAsyncExclusiveProcedure::operator iProcedure*()noexcept
{
	return &fProcedure;
}
//---------------------------------------------------------------------------
void bcAsyncExclusiveProcedure::RunAsync(iAsyncProcedure *AsyncProc)noexcept
{
	cnLib_ASSERT(AsyncProc!=nullptr);
	if(fExclusiveFlag.Acquire()){
		AsyncProc->Start();
	}
}
//---------------------------------------------------------------------------
void bcAsyncExclusiveProcedure::RunAsyncAfter(iAsyncProcedure *AsyncProc,uIntn Count)noexcept
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
void cCancellationFlag::Reset(void)noexcept
{
	fFlag=cfNormal;
}
//---------------------------------------------------------------------------
bool cCancellationFlag::Complete(void)noexcept
{
	return fFlag.Release.CmpStore(cfNormal,cfCompleted);
}
//---------------------------------------------------------------------------
bool cCancellationFlag::Cancel(void)noexcept
{
	return fFlag.Release.CmpStore(cfNormal,cfCancelled);
}
//---------------------------------------------------------------------------
bool cCancellationFlag::IsCancelled(void)const noexcept
{
	return fFlag==cfCancelled;
}
//---------------------------------------------------------------------------
