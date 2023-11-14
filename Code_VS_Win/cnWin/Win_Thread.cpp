#include "Win_Thread.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace cnWin;
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
using namespace cnWinNT6;
#endif

static constexpr DWORD CurrentThreadExitCode=0;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cThread::cThread()
{
}
//---------------------------------------------------------------------------
cThread::~cThread()
{
	::CloseHandle(fThreadHandle);
}
//---------------------------------------------------------------------------
void cThread::TLSSetExitNotify(void)
{
	gTLS->SetThreadExitNotify(cnVar::StaticInitializedSinglton<cThreadExitNotifyProc>());
}
//---------------------------------------------------------------------------
void cThread::cThreadExitNotifyProc::Execute(rPtr<iReference> Reference,void *Thread)
{
	static_cast<cThread*>(Thread)->fThreadRunning=false;
}
//---------------------------------------------------------------------------
iPtr<cThread> cThread::StartThread(iProcedure *ThreadProcedure)
{
	auto NewThread=iCreate<cThread>();
	auto Param=new cThreadCreateParameter;
	Param->Procedure=ThreadProcedure;
	Param->Thread=NewThread;

	NewThread->fThreadHandle=::CreateThread(nullptr,0,ThreadEntry,Param,0,&NewThread->fThreadID);
	if(NewThread->fThreadHandle==nullptr){
		// failed to create thread
		return nullptr;
	}

	return NewThread;
}
//---------------------------------------------------------------------------
DWORD WINAPI cThread::ThreadEntry(LPVOID Parameter)
{
	iPtr<cThread> Thread;
	iProcedure *Procedure;
	{	// initialization for thread object
		auto ThreadParam=static_cast<cThreadCreateParameter*>(Parameter);
		auto CurThread=gTLS->Get();
		// make sure no other object attached
		if(CurThread==nullptr){
			gTLS->Set(iGetReference(ThreadParam->Thread),ThreadParam->Thread);
			TLSSetExitNotify();
			Thread=cnVar::MoveCast(ThreadParam->Thread);
		}
		else{
			Thread=static_cast<cThread*>(CurThread);
		}
		Procedure=ThreadParam->Procedure;
		delete ThreadParam;
	}

	// thread procedure
	Procedure->Execute();

	Thread->fThreadRunning=false;
	// detach thread object
	gTLS->Set(nullptr,nullptr);
	gTLS->SetThreadExitNotify(nullptr);
	return 0;
}
//---------------------------------------------------------------------------
cThread* cThread::QueryCurrent(void)
{
	auto CurrentThreadObject=gTLS->Get();
	if(CurrentThreadObject!=nullptr){
		return static_cast<cThread*>(CurrentThreadObject);
	}

	HANDLE ThreadHandle;
	if(DuplicateCurrentThreadHandleSameAccess(ThreadHandle)==FALSE){
		return nullptr;
	}

	auto NewThread=iCreate<cThread>();
	NewThread->fThreadHandle=ThreadHandle;
	NewThread->fThreadID=::GetCurrentThreadId();

	gTLS->Set(iGetReference(NewThread),NewThread);
	TLSSetExitNotify();
	CurrentThreadObject=NewThread;
	return NewThread;
}
//---------------------------------------------------------------------------
bool cThread::IsRunning(void)
{
	return fThreadRunning;
}
//---------------------------------------------------------------------------
void cThread::DependentShutdownNotification(void)
{
	cThreadHandle::DependentShutdownNotification();
	auto CurrentThreadObject=gTLS->Get();
	if(CurrentThreadObject!=nullptr){
		gTLS->Clear();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//- Thread pool -------------------------------------------------------------
//- Thread pool -------------------------------------------------------------
cDefaultThreadPool::cDefaultThreadPool()
	: bcNT6ThreadPoolEnvironment(nullptr)
{
}
//- Thread pool -------------------------------------------------------------
cDefaultThreadPool::~cDefaultThreadPool()
{
}
//---------------------------------------------------------------------------
rPtr<iThreadPoolHandleWaiter> cDefaultThreadPool::CreateHandleWaiter(iReference *Reference,iFunction<void (DWORD)> *Callback)
{
	return rCreate<cHandleWaiter>(nullptr,Reference,Callback);
}
//---------------------------------------------------------------------------
//rPtr<iThreadPoolHandleRepeatWaiter> cDefaultThreadPool::CreateHandleRepeatWaiter(iReference *Reference,iProcedure *Procedure)
//{
//	return rCreate<cHandleRepeatWaiter>(nullptr,Reference,Procedure);
//}
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
