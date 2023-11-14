//---------------------------------------------------------------------------
#include "ThreadAsync.h"

using namespace cnLibrary;
using namespace cnRTL;

#pragma message (cnLib_FILE_LINE " : TODO - rename to ThreadScheduling")
#pragma message (cnLib_FILE_LINE " : TODO - cAsyncExecutionThreadSwitch change to template")

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcExclusiveProcedure::bcExclusiveProcedure()
{
}
//---------------------------------------------------------------------------
bcExclusiveProcedure::~bcExclusiveProcedure()
{
}
//---------------------------------------------------------------------------
void bcExclusiveProcedure::ProcedureThreadRunLoop(void)
{
	do{
		do{
			fExclusiveFlag.Continue();
		}while(Procedure());
	}while(!fExclusiveFlag.Release());
}
//---------------------------------------------------------------------------
void bcExclusiveProcedure::Run(void)
{
	if(!fExclusiveFlag.Acquire()){
		return;
	}

	return ProcedureThreadRunLoop();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcAsyncExclusiveProcedure::bcAsyncExclusiveProcedure()
{
}
//---------------------------------------------------------------------------
bcAsyncExclusiveProcedure::~bcAsyncExclusiveProcedure()
{
}
//---------------------------------------------------------------------------
void bcAsyncExclusiveProcedure::cProcedure::Execute(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcAsyncExclusiveProcedure::fProcedure);
	return Host->ProcedureThreadRunLoop();
}
//---------------------------------------------------------------------------
bcAsyncExclusiveProcedure::operator iProcedure*()
{
	return &fProcedure;
}
//---------------------------------------------------------------------------
void bcAsyncExclusiveProcedure::RunAsync(iAsyncProcedure *AsyncProc)
{
	cnLib_ASSERT(AsyncProc!=nullptr);
	if(fExclusiveFlag.Acquire()){
		AsyncProc->Start();
	}
}
//---------------------------------------------------------------------------
void bcAsyncExclusiveProcedure::RunAsyncAfter(iAsyncProcedure *AsyncProc,uIntn Count)
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
void cCancellationFlag::Reset(void)
{
	fFlag=cfNormal;
}
//---------------------------------------------------------------------------
bool cCancellationFlag::Complete(void)
{
	return fFlag.Release.CmpStore(cfNormal,cfCompleted);
}
//---------------------------------------------------------------------------
bool cCancellationFlag::Cancel(void)
{
	return fFlag.Release.CmpStore(cfNormal,cfCancelled);
}
//---------------------------------------------------------------------------
bool cCancellationFlag::IsCancelled(void)const
{
	return fFlag==cfCancelled;
}
//---------------------------------------------------------------------------
