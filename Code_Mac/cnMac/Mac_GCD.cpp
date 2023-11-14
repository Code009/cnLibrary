#include "Mac_GCD.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cnMac::iTimeToDispatchTimeNS(dispatch_time_t &dt,const iTimepoint *Time,sInt64 Delay)
{
	if(Time==nullptr){
		dt=DISPATCH_TIME_FOREVER;
	}
	//else if(Time==cnSystem::TimeNow){
	//	if(Delay<0){
	//		// dur now
	//		dt=DISPATCH_TIME_NOW;
	//	}
	//	else{
	//		// delay by now
	//		dt=dispatch_time(DISPATCH_TIME_NOW,Delay);
	//	}
	//}
	else{
		// absolute time
		auto st=Time->SystemTime();
		timespec when;
		when.tv_sec=static_cast<sInt32>(cnSystem::SystemTimeEpochSecondsSinceUnixEpoch);
		when.tv_nsec=0;
		uint64_t delta=st;
		delta+=Delay;
		dt=dispatch_walltime(&when,delta);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void GCD_Call(void *p)
{
	return static_cast<iProcedure*>(p)->Execute();
}
//---------------------------------------------------------------------------
void cnMac::GCDSync(dispatch_queue_t Queue,iProcedure *Procedure)
{
	dispatch_sync_f(Queue,Procedure,GCD_Call);
}
//---------------------------------------------------------------------------
void cnMac::GCDAsync(dispatch_queue_t Queue,iProcedure *Procedure)
{
	dispatch_async_f(Queue,Procedure,GCD_Call);
}
//---------------------------------------------------------------------------
void cnMac::GCDAsync(dispatch_queue_t Queue,rPtr<iReference> Reference,iProcedure *Procedure)
{
	struct Caller : cRTLAllocator{
		
		rPtr<iReference> ProcedureReference;
		iProcedure *Procedure;
		static void Call(void *p){
			auto This=static_cast<Caller*>(p);
			This->Procedure->Execute();
			delete This;
		}
	};
	auto param=new Caller;
	param->ProcedureReference=MoveCast(Reference);
	param->Procedure=Procedure;
	dispatch_async_f(Queue,param,Caller::Call);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGCDAsyncProc::cGCDAsyncProc(dispatch_queue_t Queue,iReference *ThreadReference,iProcedure *ThreadProcedure)
{
	fQueue=Queue;
	dispatch_retain(fQueue);
	fProcedure=ThreadProcedure;
	fReference=ThreadReference;
}
//---------------------------------------------------------------------------
cGCDAsyncProc::~cGCDAsyncProc()
{
	dispatch_release(fQueue);
}
//---------------------------------------------------------------------------
void cGCDAsyncProc::AsyncProcedure(void *Parameter)
{
	auto This=static_cast<cGCDAsyncProc*>(Parameter);
	This->fProcedure->Execute();

	if(This->fReference!=nullptr){
		rDecReference(This->fReference,'proc');
	}
	rDecReference(This,'work');
}
//---------------------------------------------------------------------------
void cGCDAsyncProc::Start(void)
{
	rIncReference(this,'work');
	if(fReference!=nullptr){
		rIncReference(fReference,'proc');
	}
	dispatch_async_f(fQueue,this,AsyncProcedure);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGCDTimer::cGCDTimer(dispatch_queue_t Queue,iReference *ThreadReference,iProcedure *ThreadProcedure)
{
	fQueue=Queue;
	dispatch_retain(fQueue);
	fTimerSource=nullptr;
	fReference=ThreadReference;
	fProcedure=ThreadProcedure;
	fLocking=false;
}
//---------------------------------------------------------------------------
cGCDTimer::~cGCDTimer()
{
	ClearSource();
	dispatch_release(fQueue);
}
//---------------------------------------------------------------------------
void cGCDTimer::TimerSourceProcedure(void *Parameter)
{
	auto This=static_cast<cGCDTimer*>(Parameter);
	This->fProcedure->Execute();
}
//---------------------------------------------------------------------------
void cGCDTimer::TimerSourceDeletor(void *Parameter)
{
	auto This=static_cast<cGCDTimer*>(Parameter);
	if(This->fReference!=nullptr){
		rDecReference(This->fReference,'time');
	}
	rDecReference(This,'tmsr');
}
//---------------------------------------------------------------------------
void cGCDTimer::Start(const iTimepoint *DueTime,sInt64 DueTimeDelay,uInt64 Period)
{
	if(fLocking.Acquire.Xchg(true)){
		return;
	}

	ClearSource();
	if(DueTime==nullptr){
		fLocking.Release.Store(false);
		return;
	}

	rIncReference(this,'tmsr');

	if(fReference!=nullptr){
		rIncReference(fReference,'time');
	}


	fTimerSource=dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER,0,0,fQueue);
	dispatch_set_context(fTimerSource,this);
	dispatch_set_finalizer_f(fTimerSource,TimerSourceDeletor);
	dispatch_source_set_event_handler_f(fTimerSource,TimerSourceProcedure);



	dispatch_time_t dt;
	iTimeToDispatchTimeNS(dt,DueTime,DueTimeDelay);

	uint64_t interval;
	if(Period<=0){
		interval=DISPATCH_TIME_FOREVER;
	}
	else{
		interval=Period;
	}
	dispatch_source_set_timer(fTimerSource,dt,interval,0);

	dispatch_resume(fTimerSource);

	fLocking.Release.Store(false);
}
//---------------------------------------------------------------------------
void cGCDTimer::ClearSource(void)
{
	if(fTimerSource!=nullptr){
		dispatch_source_cancel(fTimerSource);
		dispatch_release(fTimerSource);
		fTimerSource=nullptr;
	}
}
//---------------------------------------------------------------------------
void cGCDTimer::Stop(void)
{
	if(fLocking.Acquire.Xchg(true)){
		return;
	}
	ClearSource();

	fLocking.Release.Store(false);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGCDThreadPool::cGCDThreadPool(dispatch_queue_t Queue)
	: fQueue(Queue)
{
//	fQueue=dispatch_queue_create(DISPATCH_QUEUE_PRIORITY_DEFAULT,DISPATCH_QUEUE_CONCURRENT);
	dispatch_retain(fQueue);
}
//---------------------------------------------------------------------------
cGCDThreadPool::~cGCDThreadPool()
{
	dispatch_release(fQueue);
}
//---------------------------------------------------------------------------
const void* cGCDThreadPool::CastInterface(iTypeID InterfaceID)const
{
	return ImpCastInterface<iThreadPool,iGCDDispatch>(this,InterfaceID);
}
//---------------------------------------------------------------------------
dispatch_queue_t cGCDThreadPool::GetGCDDispatch(void)
{
	return fQueue;
}
//---------------------------------------------------------------------------
void cGCDThreadPool::StartWork(iReference *Reference,iProcedure *Procedure)
{
	if(Reference!=nullptr){
		GCDAsync(fQueue,Reference,Procedure);
	}
	else{
		GCDAsync(fQueue,Procedure);
	}
}
//---------------------------------------------------------------------------
rPtr<iAsyncProcedure> cGCDThreadPool::CreateWork(iReference *Reference,iProcedure *ThreadProcedure)
{
	return rCreate<cGCDAsyncProc>(fQueue,Reference,ThreadProcedure);
}
//---------------------------------------------------------------------------
rPtr<iAsyncTimer> cGCDThreadPool::CreateTimer(iReference *Reference,iProcedure *ThreadProcedure)
{
	return rCreate<cGCDTimer>(fQueue,Reference,ThreadProcedure);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGCDDispatch::cGCDDispatch(dispatch_queue_t Queue)
	: fQueue(Queue)
{
	dispatch_retain(fQueue);
}
//---------------------------------------------------------------------------
cGCDDispatch::~cGCDDispatch()
{
	dispatch_release(fQueue);
}
//---------------------------------------------------------------------------
const void* cGCDDispatch::CastInterface(iTypeID InterfaceID)const
{
	return ImpCastInterface<iDispatch,iGCDDispatch>(this,InterfaceID);
}
//---------------------------------------------------------------------------
dispatch_queue_t cGCDDispatch::GetGCDDispatch(void)
{
	return fQueue;
}
//---------------------------------------------------------------------------
bool cGCDDispatch::IsCurrentThread(void)
{
#pragma message ("TODO - Identify Queue Thread")
	auto Label=dispatch_queue_get_label(DISPATCH_CURRENT_QUEUE_LABEL);
	return cnString::Compare(Label,dispatch_queue_get_label(fQueue))==0;
}
//---------------------------------------------------------------------------
rPtr<iAsyncProcedure> cGCDDispatch::CreateWork(iReference *Reference,iProcedure *Procedure)
{
	if(Procedure==nullptr)
		return nullptr;
	return rCreate<cGCDAsyncProc>(fQueue,Reference,Procedure);
}
//---------------------------------------------------------------------------
rPtr<iAsyncTimer> cGCDDispatch::CreateTimer(iReference *Reference,iProcedure *Procedure)
{
	if(Procedure==nullptr)
		return nullptr;
	return rCreate<cGCDTimer>(fQueue,Reference,Procedure);
}
//---------------------------------------------------------------------------
void cGCDDispatch::Execute(iReference *Reference,iProcedure *Procedure)
{
	if(Reference!=nullptr){
		GCDAsync(fQueue,Reference,Procedure);
	}
	else{
		GCDAsync(fQueue,Procedure);
	}
}
//---------------------------------------------------------------------------
void cGCDDispatch::ExecuteSync(iProcedure *Procedure)
{
	GCDSync(fQueue,Procedure);
}
//---------------------------------------------------------------------------
iPtr<iAsyncTask> cGCDDispatch::ExecuteAsync(iReference *Reference,iProcedure *Procedure)
{
	return nullptr;
}
//---------------------------------------------------------------------------
