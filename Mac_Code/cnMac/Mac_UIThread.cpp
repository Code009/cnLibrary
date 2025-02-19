#include <mach/mach.h>
#include "Mac_UIThread.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;

//---------------------------------------------------------------------------
sInt64 cnMac::CFTimeTocnSystemTime(CFAbsoluteTime Time)
{
	Time-=CFTime_RefTime_Unix;
	Time-=cnSystem::SystemTimeEpochSecondsSinceUnixEpoch;
	Time*=Time_1s;
	return static_cast<sInt64>(Time);
}
//---------------------------------------------------------------------------
CFAbsoluteTime cnMac::cnSystemTimeToCFTime(sInt64 st)
{
	Float64 TimeSinceUnix=st;
	TimeSinceUnix/=Time_1s;
	TimeSinceUnix+=cnSystem::SystemTimeEpochSecondsSinceUnixEpoch;
	TimeSinceUnix+=CFTime_RefTime_Unix;
	return TimeSinceUnix;
}
//---------------------------------------------------------------------------
CFAbsoluteTime cnMac::TimeToCFTime(const iTimepoint *RefTime)
{
	return cnSystemTimeToCFTime(RefTime->SystemTime());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cRunLoopAsyncProc::cRunLoopAsyncProc(CFRunLoopRef RunLoop,CFRunLoopMode RunLoopMode,iReference *Reference,iProcedure *Procedure)
	: fRunLoop(RunLoop)
	, fRunLoopMode(RunLoopMode)
{
	fRunCount=0;

	fProcedure=Procedure;
	fReference=Reference;
	CFRunLoopSourceContext rlctx={
		0, 			//	CFIndex	version;
		this,		//	void *	info;
		nullptr,	//const void *(*retain)(const void *info);
		nullptr,	//void	(*release)(const void *info);
		nullptr,	//CFStringRef	(*copyDescription)(const void *info);
		nullptr,	//Boolean	(*equal)(const void *info1, const void *info2);
		nullptr,	//CFHashCode	(*hash)(const void *info);
		nullptr,	//void	(*schedule)(void *info, CFRunLoopRef rl, CFStringRef mode);
		nullptr,	//void	(*cancel)(void *info, CFRunLoopRef rl, CFStringRef mode);
		RunLoopSourcePerformRoutine,//void	(*perform)(void *info);
	};
	fSource=cfTakeFromManual(CFRunLoopSourceCreate(kCFAllocatorDefault,0, &rlctx));
	CFRunLoopAddSource(fRunLoop,fSource,fRunLoopMode);
}
//---------------------------------------------------------------------------
cRunLoopAsyncProc::~cRunLoopAsyncProc()
{
	CFRunLoopRemoveSource(fRunLoop,fSource,fRunLoopMode);
}
//---------------------------------------------------------------------------
void cRunLoopAsyncProc::CallProc(void)
{
	if(fRunCount!=0){
		fProcedure->Execute();
		if(fRunCount.Acquire.XchgAdd(-1)!=1){
			CFRunLoopSourceSignal(fSource);
		}
		else{
			rDecReference(this,'work');
		}
	}
}
//---------------------------------------------------------------------------
void cRunLoopAsyncProc::RunLoopSourcePerformRoutine(void *info)
{
	auto This=static_cast<cRunLoopAsyncProc*>(info);
	This->CallProc();
}
//---------------------------------------------------------------------------
void cRunLoopAsyncProc::Start(void)
{
	if(fRunCount.Acquire.XchgAdd(1)==0){
		rIncReference(this,'work');
		CFRunLoopSourceSignal(fSource);
		CFRunLoopWakeUp(fRunLoop);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cRunLoopAsyncTimer::cRunLoopAsyncTimer(CFRunLoopRef RunLoop,CFRunLoopMode RunLoopMode,iReference *Reference,iProcedure *Procedure)
	: fRunLoop(RunLoop)
	, fRunLoopMode(RunLoopMode)
{
	fProcedure=Procedure;
	fReference=Reference;
	fLocking=false;
}
//---------------------------------------------------------------------------
cRunLoopAsyncTimer::~cRunLoopAsyncTimer()
{
	ClearTimer();
}
//---------------------------------------------------------------------------
void cRunLoopAsyncTimer::StartTimer(const iTimepoint *DueTime,sInt64 DueTimeDelay,uInt64 Period)
{
	cnLib_ASSERT(fTimer==nullptr);
	cnLib_ASSERT(DueTime!=nullptr);

	CFAbsoluteTime FireDate;
	FireDate=TimeToCFTime(DueTime);
	Float64 DueTimeDelaySeconds=DueTimeDelay;
	DueTimeDelaySeconds/=Time_1s;
	FireDate+=DueTimeDelaySeconds;

	Float64 PeriodSecond=Period;
	PeriodSecond/=Time_1s;

	CFRunLoopTimerContext Context={
    	0,			// CFIndex	version;
    	this,		// void *	info;
		nullptr,	// const void *(*retain)(const void *info);
	    nullptr,	// void	(*release)(const void *info);
	    nullptr,	// CFStringRef	(*copyDescription)(const void *info);
	};
	fTimer=CFRunLoopTimerCreate(nullptr,FireDate,PeriodSecond,0,0,TimerCallback,&Context);
	CFRunLoopAddTimer(fRunLoop,fTimer,fRunLoopMode);
}
//---------------------------------------------------------------------------
void cRunLoopAsyncTimer::ClearTimer(void)
{
	if(fTimer!=nil){
		CFRunLoopTimerInvalidate(fTimer);
		CFRelease(fTimer);
		fTimer=nullptr;
	}
}
//---------------------------------------------------------------------------
void cRunLoopAsyncTimer::TimerCallback(CFRunLoopTimerRef timer,void *info)
{
	static_cast<cRunLoopAsyncTimer*>(info)->TimerProcedure();
}
//---------------------------------------------------------------------------
void cRunLoopAsyncTimer::Start(const iTimepoint *DueTime,sInt64 DueTimeDelay,uInt64 Period)
{
	if(fLocking.Acquire.Xchg(true))
		return;

	ClearTimer();

	if(DueTime==nullptr){
		fLocking.Release.Store(false);
		return;
	}
	StartTimer(DueTime,DueTimeDelay,Period);

	fLocking.Release.Store(false);
}
//---------------------------------------------------------------------------
void cRunLoopAsyncTimer::Stop(void)
{
	if(fLocking.Acquire.Xchg(true)==true)
		return;

	ClearTimer();

	fLocking.Release.Store(false);
}
//---------------------------------------------------------------------------
void cRunLoopAsyncTimer::TimerProcedure(void)
{
	fProcedure->Execute();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cRunLoopDispatch::cAsyncExecuteTask::cAsyncExecuteTask()
{
	CFRunLoopSourceContext rlctx={
		0, 			//	CFIndex	version;
		this,		//	void *	info;
		nullptr,	//const void *(*retain)(const void *info);
		nullptr,	//void	(*release)(const void *info);
		nullptr,	//CFStringRef	(*copyDescription)(const void *info);
		nullptr,	//Boolean	(*equal)(const void *info1, const void *info2);
		nullptr,	//CFHashCode	(*hash)(const void *info);
		nullptr,	//void	(*schedule)(void *info, CFRunLoopRef rl, CFStringRef mode);
		nullptr,	//void	(*cancel)(void *info, CFRunLoopRef rl, CFStringRef mode);
		RunLoopSourcePerformRoutine,//void	(*perform)(void *info);
	};
	fSource=cfTakeFromManual(CFRunLoopSourceCreate(kCFAllocatorDefault,0, &rlctx));
}
//---------------------------------------------------------------------------
cRunLoopDispatch::cAsyncExecuteTask::~cAsyncExecuteTask()
{
	CFRunLoopSourceInvalidate(fSource);
}
//---------------------------------------------------------------------------
void cRunLoopDispatch::cAsyncExecuteTask::Setup(cRunLoopDispatch *Host,iReference *Reference,iProcedure *Procedure)
{
	iIncReference(this,'work');
	fHost=Host;
	CFRunLoopAddSource(fHost->fRunLoop,fSource,fHost->fRunLoopMode);
	fProcedure=Procedure;
	fReference=Reference;
	CFRunLoopSourceSignal(fSource);
	CFRunLoopWakeUp(fHost->fRunLoop);
}
//---------------------------------------------------------------------------
bool cRunLoopDispatch::cAsyncExecuteTask::IsDone(void)
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool cRunLoopDispatch::cAsyncExecuteTask::SetNotify(iProcedure *NotifyProcedure)
{
	return fTaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
void cRunLoopDispatch::cAsyncExecuteTask::RunLoopSourcePerformRoutine(void *info)
{
	auto This=static_cast<cAsyncExecuteTask*>(info);
	This->CallProc();
}
//---------------------------------------------------------------------------
void cRunLoopDispatch::cAsyncExecuteTask::CallProc(void)
{
	fProcedure->Execute();

	CFRunLoopRemoveSource(fHost->fRunLoop,fSource,fHost->fRunLoopMode);
	fHost=nullptr;

	fTaskState.SetDone();

	fReference=nullptr;
	iDecReference(this,'work');
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cRunLoopDispatch::cRunLoopDispatch(CFRunLoopRef RunLoop,CFRunLoopMode RunLoopMode)
	: fRunLoop(RunLoop)
	, fRunLoopMode(RunLoopMode)
{
}
//---------------------------------------------------------------------------
cRunLoopDispatch::~cRunLoopDispatch()
{
}
//---------------------------------------------------------------------------
bool cRunLoopDispatch::IsCurrentThread(void)
{
#pragma message ("TODO - Test - IsCurrentThread")
	return CFRunLoopGetCurrent()==fRunLoop;
}
//---------------------------------------------------------------------------
rPtr<iAsyncProcedure> cRunLoopDispatch::CreateWork(iReference *Reference,iProcedure *Procedure)
{
	return rCreate<cRunLoopAsyncProc>(fRunLoop,fRunLoopMode,Reference,Procedure);
}
//---------------------------------------------------------------------------
rPtr<iAsyncTimer> cRunLoopDispatch::CreateTimer(iReference *Reference,iProcedure *Procedure)
{
	return rCreate<cRunLoopAsyncTimer>(fRunLoop,fRunLoopMode,Reference,Procedure);
}
//---------------------------------------------------------------------------
void cRunLoopDispatch::Execute(iReference *Reference,iProcedure *Procedure)
{
	if(Reference!=nullptr){
		rIncReference(Reference,'post');
		CFRunLoopPerformBlock(fRunLoop,fRunLoopMode, ^{
			Procedure->Execute();
			rDecReference(Reference,'post');
		});
		CFRunLoopWakeUp(fRunLoop);
	}
	else{
		CFRunLoopPerformBlock(fRunLoop,fRunLoopMode, ^{
			Procedure->Execute();
		});
		CFRunLoopWakeUp(fRunLoop);
	}
}
//---------------------------------------------------------------------------
void cRunLoopDispatch::ExecuteSync(iProcedure *Procedure)
{
	cThreadOneTimeNotifier Notifier;
	auto &block_Notifier=Notifier;
	CFRunLoopPerformBlock(fRunLoop,fRunLoopMode, ^{
		Procedure->Execute();

		block_Notifier.Notify();
	});

	Notifier.Wait();
}
//---------------------------------------------------------------------------
iPtr<iAsyncTask> cRunLoopDispatch::ExecuteAsync(iReference *Reference,iProcedure *Procedure)
{
	auto Task=fAsyncExecuteTaskRecycler.Query();
	Task->Setup(this,Reference,Procedure);
	return Task;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUIThread::cUIThread()
{
	fMacThread=mach_thread_self();

	fRunLoop=CFRunLoopGetMain();
	CFRetain(fRunLoop);

	fDispatchCommon=iCreate<cRunLoopDispatch>(fRunLoop,kCFRunLoopCommonModes);
	fDispatchDefault=iCreate<cRunLoopDispatch>(fRunLoop,kCFRunLoopDefaultMode);
}
//---------------------------------------------------------------------------
cUIThread::~cUIThread()
{
	CFRelease(fRunLoop);
}
//---------------------------------------------------------------------------
bool cUIThread::IsCurrentThread(void)
{
	auto CurThreadID=mach_thread_self();
	return CurThreadID==fMacThread;
}
//---------------------------------------------------------------------------
iDispatch* cUIThread::GetDispatch(bool HighPriority)
{
	if(HighPriority){
		return fDispatchCommon;
	}
	else{
		return fDispatchDefault;
	}
}
//---------------------------------------------------------------------------
iUIKeyboard* cUIThread::GetDefaultKeyboard(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iUIMouse* cUIThread::GetDefaultMouse(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
