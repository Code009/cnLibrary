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
cThread::cThread()noexcept
{
}
//---------------------------------------------------------------------------
cThread::~cThread()noexcept
{
	::CloseHandle(fThreadHandle);
}
//---------------------------------------------------------------------------
void cThread::TLSSetExitNotify(void)noexcept
{
	gTLS->SetThreadExitNotify(cnVar::StaticInitializedSinglton<cThreadExitNotifyProc>());
}
//---------------------------------------------------------------------------
void cThread::cThreadExitNotifyProc::Execute(rPtr<iReference> Reference,void *Thread)noexcept
{
	static_cast<cThread*>(Thread)->fThreadRunning=false;
}
//---------------------------------------------------------------------------
iPtr<cThread> cThread::StartThread(iProcedure *ThreadProcedure)noexcept
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
DWORD WINAPI cThread::ThreadEntry(LPVOID Parameter)noexcept
{
	iPtr<cThread> Thread;
	iProcedure *Procedure;
	{	// initialization for thread object
		auto ThreadParam=static_cast<cThreadCreateParameter*>(Parameter);
		auto CurThread=gTLS->Get();
		// make sure no other object attached
		if(CurThread==nullptr){
			gTLS->Set(ThreadParam->Thread.Reference(),ThreadParam->Thread);
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
cThread* cThread::QueryCurrent(void)noexcept
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

	gTLS->Set(NewThread.Reference(),NewThread);
	TLSSetExitNotify();
	CurrentThreadObject=NewThread;
	return NewThread;
}
//---------------------------------------------------------------------------
bool cThread::IsRunning(void)noexcept
{
	return fThreadRunning;
}
//---------------------------------------------------------------------------
void cThread::DependentShutdownNotification(void)noexcept
{
	cThreadHandle::DependentShutdownNotification();
	auto CurrentThreadObject=gTLS->Get();
	if(CurrentThreadObject!=nullptr){
		gTLS->Clear();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cErrorReportRecord::cErrorReportRecord()noexcept
{
}
//---------------------------------------------------------------------------
cErrorReportRecord::~cErrorReportRecord()noexcept
{
}
//---------------------------------------------------------------------------
iErrorReport* cErrorReportRecord::ParentReport(void)noexcept
{
	return Parent;
}
//---------------------------------------------------------------------------
cArray<const uChar16> cErrorReportRecord::FunctionName(void)noexcept
{
	return FuncName.GetArray();
}
//---------------------------------------------------------------------------
cArray<const uChar16> cErrorReportRecord::ErrorMessage(void)noexcept
{
	return ErrorMsg.GetArray();
}
//---------------------------------------------------------------------------
cArray<const cErrorInfo> cErrorReportRecord::Errors(void)noexcept
{
	return ErrorList.Storage();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cnSystem::ErrorReportMaker::Submit(void)noexcept
{
	auto Record=static_cast<cErrorReportRecord*>(fReport.Pointer());

	auto CurReport=cErrorReportRecord::gTLSRecord->Get();
	if(CurReport!=nullptr){
		Record->Parent=static_cast<cErrorReportRecord*>(CurReport);
	}
	cErrorReportRecord::gTLSRecord->Set(cnVar::MoveCast(fReport),Record);
}
//---------------------------------------------------------------------------
void cnSystem::ErrorReportMaker::SetMessage(const uChar16 *Name,uIntn Length)noexcept
{
	auto Record=static_cast<cErrorReportRecord*>(fReport.Pointer());
	
	Record->ErrorMsg.SetString(Name,Length);
}
//---------------------------------------------------------------------------
void cnSystem::ErrorReportMaker::Append(const cErrorInfo *ErrorInfo)noexcept
{
	auto Record=static_cast<cErrorReportRecord*>(fReport.Pointer());
	
	Record->ErrorList.AppendMake(*ErrorInfo);
	if(ErrorInfo->DataLength!=0){
		Record->ErrorData.Append(ErrorInfo->Data,ErrorInfo->DataLength);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cnSystem::ErrorReportMaker cnSystem::ErrorReportManager::MakeReport(const uChar16 *Name,uIntn Length)noexcept
{
	auto Record=rQuerySharedObject<cErrorReportRecord>();
	Record->FuncName.SetString(Name,Length);
	return cnSystem::ErrorReportMaker(Record);
}
//---------------------------------------------------------------------------
rPtr<iErrorReport> cnSystem::ErrorReportManager::Fetch(void)noexcept
{
	auto *TLSObject=cErrorReportRecord::gTLSRecord->Get();
	if(TLSObject==nullptr)
		return nullptr;
	rPtr<iErrorReport> RetReport=static_cast<iErrorReport*>(TLSObject);
	cErrorReportRecord::gTLSRecord->Clear();
	return RetReport;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//- Thread pool -------------------------------------------------------------
//- Thread pool -------------------------------------------------------------
cDefaultThreadPool::cDefaultThreadPool()noexcept
	: bcNT6ThreadPoolEnvironment(nullptr)
{
}
//- Thread pool -------------------------------------------------------------
cDefaultThreadPool::~cDefaultThreadPool()noexcept
{
}
//---------------------------------------------------------------------------
rPtr<iThreadPoolHandleWaiter> cDefaultThreadPool::CreateHandleWaiter(iReference *Reference,iFunction<void (DWORD)noexcept> *Callback)noexcept
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
