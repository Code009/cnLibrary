#include "Stream.h"
//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcReadQueue::bcReadQueue()
	: fReadCallback(nullptr)
	, fReadQueueEnded(false)
	, fReadQueueTerminated(false)
	, fReadQueueBufferAvailable(false)
{
}
//---------------------------------------------------------------------------
bcReadQueue::~bcReadQueue()
{
}
//---------------------------------------------------------------------------
void bcReadQueue::NotificationStarted(void)
{
	bcAsyncQueue::NotificationStarted();

	rIncReference(this,'rdqu');
	fReadCallback->ReadStarted();
}
//---------------------------------------------------------------------------
void bcReadQueue::NotificationStopped(void)
{
	bcAsyncQueue::NotificationStopped();

	auto Reference=cnVar::MoveCast(fReadCallbackReference);
	auto Callback=fReadCallback;
	fReadCallback=nullptr;
	Callback->ReadStopped();

	rDecReference(this,'rdqu');
}
//---------------------------------------------------------------------------
bcReadQueue::CycleState bcReadQueue::NotificationCheckState(void)
{
	if(fReadQueueTerminated){
		return CycleState::Terminated;
	}
	if(fReadQueueBufferAvailable){
		return CycleState::Normal;
	}
	if(fReadQueueEnded){
		return CycleState::Ended;
	}
	return CycleState::Normal;
}
//---------------------------------------------------------------------------
void bcReadQueue::AsyncQueueNotify(void)
{
	fReadCallback->ReadNotify();
}
//---------------------------------------------------------------------------
bool bcReadQueue::StartRead(iReference *Reference,iReadQueueCallback *Callback)
{
	if(fReadQueueTerminated)
		return false;

	if(PrepareStartNotify()==false){
		return false;
	}

	fReadCallback=Callback;
	fReadCallbackReference=Reference;

	ConfirmStartNotify();
	UpdateQueueState(false);
	return true;
}
//---------------------------------------------------------------------------
void bcReadQueue::StopRead(void)
{
	if(StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcReadQueue::NotifyRead(uIntn SizeToNotify)
{
	bool IdleNotify=SizeToNotify==0;
	if(fReadQueueEnded)
		IdleNotify=true;
	return NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool bcReadQueue::IsReadClosed(bool &GracefulClose)
{
	GracefulClose=IsNotificationEnded();
	return IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcReadQueue::ReadQueueReportBufferAvailable(bool AsyncNotify)
{
	fReadQueueBufferAvailable=true;
	AsyncQueueSetAvailable(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcReadQueue::ReadQueueReportBufferEmpty(void)
{
	if(fReadQueueBufferAvailable){
		fReadQueueBufferAvailable=false;
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcReadQueue::ReadQueueReportEnded(void)
{
	if(fReadQueueEnded==false){
		fReadQueueEnded=true;
		AsyncQueueSetAvailable(false);
	}
}
//---------------------------------------------------------------------------
void bcReadQueue::ReadQueueReportTerminated(void)
{
	if(fReadQueueTerminated==false){
		fReadQueueTerminated=true;
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcWriteQueue::bcWriteQueue()
	: fWriteCallback(nullptr)
	, fWriteQueueTerminated(false)
	, fWriteQueueEndMode(EndpointWriteEndMode::NextSession)
{
}
//---------------------------------------------------------------------------
bcWriteQueue::~bcWriteQueue()
{
}
//---------------------------------------------------------------------------
void bcWriteQueue::NotificationStarted(void)
{
	bcAsyncQueue::NotificationStarted();

	rIncReference(this,'wrqu');
	if(fWriteQueueEndMode==EndpointWriteEndMode::NextSession){
		fWriteQueueEndMode=EndpointWriteEndMode::Idle;
	}
	fWriteCallback->WriteStarted();
}
//---------------------------------------------------------------------------
void bcWriteQueue::NotificationStopped(void)
{
	bcAsyncQueue::NotificationStopped();

	auto Reference=cnVar::MoveCast(fWriteCallbackReference);
	auto Callback=fWriteCallback;
	fWriteCallback=nullptr;
	Callback->WriteStopped();

	rDecReference(this,'wrqu');
}
//---------------------------------------------------------------------------
bcWriteQueue::CycleState bcWriteQueue::NotificationCheckState(void)
{
	if(fWriteQueueTerminated){
		return CycleState::Terminated;
	}
	if(fWriteCallback==nullptr && fWriteQueueEndMode==EndpointWriteEndMode::Idle){
		return CycleState::Ended;
	}
	return CycleState::Normal;
}
//---------------------------------------------------------------------------
void bcWriteQueue::AsyncQueueNotify(void)
{
	fWriteCallback->WriteNotify();
}
//---------------------------------------------------------------------------
bool bcWriteQueue::StartWrite(iReference *Reference,iWriteQueueCallback *Callback)
{
	if(fWriteQueueTerminated)
		return false;
	if(PrepareStartNotify()==false){
		return false;
	}

	fWriteCallback=Callback;
	fWriteCallbackReference=Reference;

	ConfirmStartNotify();
	UpdateQueueState(false);
	return true;
}
//---------------------------------------------------------------------------
void bcWriteQueue::StopWrite(bool Terminate)
{
	if(Terminate)
		fWriteQueueTerminated=true;

	if(StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcWriteQueue::NotifyWrite(uIntn SizeToNotify)
{
	bool IdleNotify=SizeToNotify==0;
	return NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool bcWriteQueue::IsWriteClosed(bool &GracefulClose)
{
	GracefulClose=IsNotificationEnded();
	return IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcWriteQueue::WriteQueueReportBufferAvailable(bool AsyncNotify)
{
	AsyncQueueSetAvailable(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcWriteQueue::WriteQueueReportEnded(void)
{
	return WriteQueueSetEndMode(EndpointWriteEndMode::Idle);
}
//---------------------------------------------------------------------------
void bcWriteQueue::WriteQueueReportTerminated(void)
{
	if(fWriteQueueTerminated==false){
		fWriteQueueTerminated=true;
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcWriteQueue::WriteQueueSetEndMode(eEndpointWriteEndMode EndMode)
{
	if(fWriteCallback!=nullptr && EndMode==EndpointWriteEndMode::NextSession){
		EndMode=EndpointWriteEndMode::Idle;
	}
	if(fWriteQueueEndMode==EndMode)
		return;

	fWriteQueueEndMode=EndMode;
	UpdateQueueState(false);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcRWQueue::bcRWQueue()
	: fReadCallback(nullptr)
	, fWriteCallback(nullptr)
	, fReadQueueBufferAvailable(false)
	, fReadQueueEnded(false)
	, fReadQueueTerminated(false)
	, fWriteQueueTerminated(false)
	, fWriteQueueEndMode(EndpointWriteEndMode::NextSession)
{
}
//---------------------------------------------------------------------------
bcRWQueue::~bcRWQueue()
{
}
//---------------------------------------------------------------------------
void bcRWQueue::CreateReadQueueWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop)
{
	return fReadAsyncQueue.CreateQueueWork(AsyncExecution,DisableUpdateThreadLoop);
}
//---------------------------------------------------------------------------
void bcRWQueue::UpdateReadQueueState(bool AsyncNotify)
{
	return fReadAsyncQueue.UpdateQueueState(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcRWQueue::CreateWriteQueueWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop)
{
	return fWriteAsyncQueue.CreateQueueWork(AsyncExecution,DisableUpdateThreadLoop);
}
//---------------------------------------------------------------------------
void bcRWQueue::UpdateWriteQueueState(bool AsyncNotify)
{
	return fWriteAsyncQueue.UpdateQueueState(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcRWQueue::CloseReadQueue(void)
{
	return fReadAsyncQueue.CloseQueue();
}
//---------------------------------------------------------------------------
void bcRWQueue::CloseWriteQueue(void)
{
	return fWriteAsyncQueue.CloseQueue();
}
//---------------------------------------------------------------------------
bool bcRWQueue::ReadQueueIsStarted(void)const
{
	return fReadAsyncQueue.IsNotificationStarted();
}
//---------------------------------------------------------------------------
bool bcRWQueue::ReadQueueIsClosed(void)const
{
	return fReadAsyncQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
bool bcRWQueue::ReadQueueIsEnded(void)const
{
	return fReadAsyncQueue.IsNotificationEnded();
}
//---------------------------------------------------------------------------
bool bcRWQueue::WriteQueueIsStarted(void)const
{
	return fWriteAsyncQueue.IsNotificationStarted();
}
//---------------------------------------------------------------------------
bool bcRWQueue::WriteQueueIsClosed(void)const
{
	return fWriteAsyncQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
bool bcRWQueue::WriteQueueIsEnded(void)const
{
	return fWriteAsyncQueue.IsNotificationEnded();
}
//---------------------------------------------------------------------------
bcRWQueue* bcRWQueue::cReadQueue::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&bcRWQueue::fReadAsyncQueue);
}
//---------------------------------------------------------------------------
iReference* bcRWQueue::cReadQueue::NotificationInnerReference(void)
{
	return GetHost()->RWQueueInnerReference();
}
//---------------------------------------------------------------------------
void bcRWQueue::cReadQueue::NotificationStarted(void)
{
	bcAsyncQueue::NotificationStarted();

	auto Host=GetHost();
	rIncReference(static_cast<iReadQueue*>(Host),'rdqu');
	Host->fReadCallback->ReadStarted();
	Host->ReadQueueStarted();
}
//---------------------------------------------------------------------------
void bcRWQueue::cReadQueue::NotificationStopped(void)
{
	bcAsyncQueue::NotificationStopped();

	auto Host=GetHost();

	auto Reference=cnVar::MoveCast(Host->fReadCallbackReference);
	auto Callback=Host->fReadCallback;
	Host->fReadCallback=nullptr;
	Callback->ReadStopped();

	Host->ReadQueueStopped();
	rDecReference(static_cast<iReadQueue*>(Host),'rdqu');
}
//---------------------------------------------------------------------------
void bcRWQueue::cReadQueue::NotificationClosed(void)
{
	bcAsyncQueue::NotificationClosed();

	auto Host=GetHost();
	Host->ReadQueueClosed();
}
//---------------------------------------------------------------------------
void bcRWQueue::cReadQueue::AsyncQueueNotify(void)
{
	auto Host=GetHost();
	return Host->ReadQueueNotify();
}
//---------------------------------------------------------------------------
bcRWQueue::CycleState bcRWQueue::cReadQueue::NotificationCheckState(void)
{
	auto Host=GetHost();
	return Host->ReadQueueCheckState();
}
//---------------------------------------------------------------------------
bcRWQueue::CycleState bcRWQueue::ReadQueueCheckState(void)
{
	if(fReadQueueTerminated){
		return CycleState::Terminated;
	}
	if(fReadQueueBufferAvailable){
		return CycleState::Normal;
	}
	if(fReadQueueEnded){
		return CycleState::Ended;
	}
	return CycleState::Normal;
}
//---------------------------------------------------------------------------
bool bcRWQueue::StartRead(iReference *Reference,iReadQueueCallback *Callback)
{
	if(fReadQueueTerminated)
		return false;

	if(fReadAsyncQueue.PrepareStartNotify()==false)
		return false;

	fReadCallback=Callback;
	fReadCallbackReference=Reference;

	fReadAsyncQueue.ConfirmStartNotify();
	fReadAsyncQueue.UpdateQueueState(false);
	return true;
}
//---------------------------------------------------------------------------
void bcRWQueue::StopRead(void)
{
	if(fReadAsyncQueue.StopNotify()){
		fReadAsyncQueue.UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::NotifyRead(uIntn SizeToNotify)
{
	bool IdleNotify=SizeToNotify==0;
	if(fReadQueueEnded)
		IdleNotify=true;
	return fReadAsyncQueue.NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool bcRWQueue::IsReadClosed(bool &GracefulClose)
{
	GracefulClose=fReadAsyncQueue.IsNotificationEnded();
	return fReadAsyncQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueReportBufferAvailable(bool AsyncNotify)
{
	fReadQueueBufferAvailable=true;
	fReadAsyncQueue.AsyncQueueSetAvailable(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueReportBufferEmpty(void)
{
	if(fReadQueueBufferAvailable){
		fReadQueueBufferAvailable=false;
		fReadAsyncQueue.UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueReportEnded(void)
{
	if(fReadQueueEnded==false){
		fReadQueueEnded=true;
		fReadAsyncQueue.AsyncQueueSetAvailable(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueReportTerminated(void)
{
	if(fReadQueueTerminated==false){
		fReadQueueTerminated=true;
		fReadAsyncQueue.UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueStarted(void)
{
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueStopped(void)
{
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueClosed(void)
{
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueNotify(void)
{
	fReadCallback->ReadNotify();
}
//---------------------------------------------------------------------------
bcRWQueue* bcRWQueue::cWriteQueue::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&bcRWQueue::fWriteAsyncQueue);
}
//---------------------------------------------------------------------------
iReference* bcRWQueue::cWriteQueue::NotificationInnerReference(void)
{
	return GetHost()->RWQueueInnerReference();
}
//---------------------------------------------------------------------------
void bcRWQueue::cWriteQueue::NotificationStarted(void)
{
	bcAsyncQueue::NotificationStarted();

	auto Host=GetHost();
	rIncReference(static_cast<iWriteQueue*>(Host),'wrqu');
	if(Host->fWriteQueueEndMode==EndpointWriteEndMode::NextSession){
		Host->fWriteQueueEndMode=EndpointWriteEndMode::Idle;
	}
	Host->fWriteCallback->WriteStarted();
	Host->WriteQueueStarted();
}
//---------------------------------------------------------------------------
void bcRWQueue::cWriteQueue::NotificationStopped(void)
{
	bcAsyncQueue::NotificationStopped();

	auto Host=GetHost();

	auto Reference=cnVar::MoveCast(Host->fWriteCallbackReference);
	auto Callback=Host->fWriteCallback;
	Host->fWriteCallback=nullptr;
	Callback->WriteStopped();

	Host->WriteQueueStopped();
	rDecReference(static_cast<iWriteQueue*>(Host),'wrqu');
}
//---------------------------------------------------------------------------
void bcRWQueue::cWriteQueue::NotificationClosed(void)
{
	bcAsyncQueue::NotificationClosed();

	auto Host=GetHost();
	return Host->WriteQueueClosed();
}
//---------------------------------------------------------------------------
bcRWQueue::CycleState bcRWQueue::cWriteQueue::NotificationCheckState(void)
{
	auto Host=GetHost();
	return Host->WriteQueueCheckState();
}
//---------------------------------------------------------------------------
void bcRWQueue::cWriteQueue::AsyncQueueNotify(void)
{
	auto Host=GetHost();
	Host->WriteQueueNotify();
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueStarted(void)
{
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueStopped(void)
{
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueNotify(void)
{
	fWriteCallback->WriteNotify();
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueClosed(void)
{
}
//---------------------------------------------------------------------------
bcRWQueue::CycleState bcRWQueue::WriteQueueCheckState(void)
{
	if(fWriteQueueTerminated){
		return CycleState::Terminated;
	}
	if(fWriteCallback==nullptr && fWriteQueueEndMode==EndpointWriteEndMode::Idle){
		return CycleState::Ended;
	}
	return CycleState::Normal;
}
//---------------------------------------------------------------------------
bool bcRWQueue::StartWrite(iReference *Reference,iWriteQueueCallback *Callback)
{
	if(fWriteQueueTerminated)
		return false;

	if(fWriteAsyncQueue.PrepareStartNotify()==false)
		return false;

	fWriteCallback=Callback;
	fWriteCallbackReference=Reference;

	fWriteAsyncQueue.ConfirmStartNotify();
	fWriteAsyncQueue.UpdateQueueState(false);
	return true;
}
//---------------------------------------------------------------------------
void bcRWQueue::StopWrite(bool Terminate)
{
	if(Terminate)
		fWriteQueueTerminated=true;

	if(fWriteAsyncQueue.StopNotify()){
		fWriteAsyncQueue.UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::NotifyWrite(uIntn SizeToNotify)
{
	bool IdleNotify=SizeToNotify==0;
	if(fReadQueueEnded)
		IdleNotify=true;
	return fWriteAsyncQueue.NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool bcRWQueue::IsWriteClosed(bool &GracefulClose)
{
	GracefulClose=fWriteAsyncQueue.IsNotificationEnded();
	return fWriteAsyncQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueReportBufferAvailable(bool AsyncNotify)
{
	fWriteAsyncQueue.AsyncQueueSetAvailable(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueReportEnded(void)
{
	return WriteQueueSetEndMode(EndpointWriteEndMode::Idle);
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueReportTerminated(void)
{
	if(fWriteQueueTerminated==false){
		fWriteQueueTerminated=true;
		fWriteAsyncQueue.UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueSetEndMode(eEndpointWriteEndMode EndMode)
{
	if(fWriteCallback!=nullptr && EndMode==EndpointWriteEndMode::NextSession){
		EndMode=EndpointWriteEndMode::Idle;
	}
	if(fWriteQueueEndMode==EndMode)
		return;

	fWriteQueueEndMode=EndMode;
	fWriteAsyncQueue.UpdateQueueState(false);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void* bcStream::cAccessTask::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	return ImpCastInterface<iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
bool bcStream::cAccessTask::IsDone(void)
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool bcStream::cAccessTask::SetNotify(iProcedure *NotifyProcedure)
{
	return fTaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
bool bcStream::cAccessTask::GetResult(uIntn &SizeCompleted)
{
	if(fTaskState.IsDone()==false){
		SizeCompleted=0;
		return false;
	}
	SizeCompleted=TotalSizeCompleted;
	return AccessErrorCode==StreamError::Success;
}
//---------------------------------------------------------------------------
eStreamError bcStream::cAccessTask::GetStreamError(void)
{
	return AccessErrorCode;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcStream::cReadTask::Cancel(void)
{
	Owner->fReadTaskQueue.CancelTask(this);
}
//---------------------------------------------------------------------------
void bcStream::cReadTask::Setup(void *Buffer,uIntn Size)
{
	cnLib_ASSERT(Buffer!=nullptr);

	fAccessBuffer=Buffer;
	fAccessBufferSize=Size;

	TotalSizeCompleted=0;
}
//---------------------------------------------------------------------------
cMemory bcStream::cReadTask::ReserveWriteBuffer(uIntn)
{
	if(TotalSizeCompleted>=fAccessBufferSize){
		// no more buffer
		return NullArray;
	}
	cMemory Buffer;
	Buffer.Pointer=cnMemory::PointerAddByteOffset(fAccessBuffer,TotalSizeCompleted);
	Buffer.Length=fAccessBufferSize-TotalSizeCompleted;
	return Buffer;
}
//---------------------------------------------------------------------------
void bcStream::cReadTask::CommitWriteBuffer(uIntn Size)
{
	if(TotalSizeCompleted>=fAccessBufferSize){
		// no more buffer
		return;
	}
	if(TotalSizeCompleted+Size>=fAccessBufferSize){
		Size=fAccessBufferSize-TotalSizeCompleted;
		TotalSizeCompleted=fAccessBufferSize;
	}
	else{
		TotalSizeCompleted+=Size;
	}
}
//---------------------------------------------------------------------------
bool bcStream::cReadTask::IsAccessBufferCompleted(void)const
{
	return TotalSizeCompleted>=fAccessBufferSize;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcStream::cWriteTask::Cancel(void)
{
	Owner->fWriteTaskQueue.CancelTask(this);
}
//---------------------------------------------------------------------------
void bcStream::cWriteTask::Setup(const void *Buffer,uIntn Size)
{
	cnLib_ASSERT(Buffer!=nullptr);

	fAccessBuffer=Buffer;
	fAccessBufferSize=Size;

	TotalSizeCompleted=0;
}
//---------------------------------------------------------------------------
cConstMemory bcStream::cWriteTask::GatherReadBuffer(uIntn)
{
	if(TotalSizeCompleted>=fAccessBufferSize){
		// no more buffer
		return NullArray;
	}
	cConstMemory Buffer;
	Buffer.Pointer=cnMemory::PointerAddByteOffset(fAccessBuffer,TotalSizeCompleted);
	Buffer.Length=fAccessBufferSize-TotalSizeCompleted;
	return Buffer;
}
//---------------------------------------------------------------------------
void bcStream::cWriteTask::DismissReadBuffer(uIntn Size)
{
	if(TotalSizeCompleted>=fAccessBufferSize){
		// no more buffer
		return;
	}
	if(TotalSizeCompleted+Size>=fAccessBufferSize){
		Size=fAccessBufferSize-TotalSizeCompleted;
		TotalSizeCompleted=fAccessBufferSize;
	}
	else{
		TotalSizeCompleted+=Size;
	}
}
//---------------------------------------------------------------------------
bool bcStream::cWriteTask::IsAccessBufferCompleted(void)const
{
	return TotalSizeCompleted>=fAccessBufferSize;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcStream::bcStream()
{
	fStreamClosed=false;
	fStreamReadEnded=false;
	fStreamWriteSetEnd=false;
	fStreamWriteEnded=false;
}
//---------------------------------------------------------------------------
bcStream::~bcStream()
{
}
//---------------------------------------------------------------------------
void* bcStream::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	return ImpCastInterface<iStream,iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
void bcStream::StreamClose(void)
{
	fStreamClosed=true;
	UpdateWriteTaskQueue();
	UpdateReadTaskQueue();
}
//---------------------------------------------------------------------------
void bcStream::Close(void)
{
	StreamClose();
}
//---------------------------------------------------------------------------
bool bcStream::IsEndOfReading(void)
{
	return fStreamReadEnded;
}
//---------------------------------------------------------------------------
bool bcStream::SetEndOfStream(void)
{
	if(fStreamClosed)
		return fStreamWriteEnded;

	fStreamWriteSetEnd=true;
	UpdateWriteTaskQueue();
	return true;
}
//---------------------------------------------------------------------------
eStreamError bcStream::GetStreamError(void)
{
	return fLastErrorCode;
}
//---------------------------------------------------------------------------
bool bcStream::Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)
{
	if(fStreamClosed){
		fLastErrorCode=StreamError::Closed;
		return false;
	}
	if(fStreamReadEnded){
		// stream read ended, directly return
		fLastErrorCode=StreamError::Success;
		SizeCompleted=0;
		return true;
	}
	auto Task=iCreate<cReadTask>();
	Task->Owner=this;
	Task->Setup(Buffer,Size);
	fReadTaskQueue.EnqueueTask(Task);

	UpdateReadTaskQueue();
	// sync access

	WaitForTask(Task);

	SizeCompleted=Task->TotalSizeCompleted;
	fLastErrorCode=Task->AccessErrorCode;
	return fLastErrorCode==StreamError::Success;
}
//---------------------------------------------------------------------------
bool bcStream::Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)
{
	if(fStreamClosed || fStreamWriteEnded){
		fLastErrorCode=StreamError::Closed;
		return false;
	}
	auto Task=iCreate<cWriteTask>();
	Task->Owner=this;
	Task->Setup(Buffer,Size);
	fWriteTaskQueue.EnqueueTask(Task);

	UpdateWriteTaskQueue();
	// sync access

	WaitForTask(Task);

	SizeCompleted=Task->TotalSizeCompleted;
	fLastErrorCode=Task->AccessErrorCode;
	return fLastErrorCode==StreamError::Success;
}
//---------------------------------------------------------------------------
iPtr<iStreamTask> bcStream::ReadAsync(void *Buffer,uIntn Size)
{
	if(fStreamClosed){
		fLastErrorCode=StreamError::Closed;
		return nullptr;
	}
	auto Task=iCreate<cReadTask>();
	Task->Owner=this;
	if(fStreamReadEnded){
		// stream read ended, directly return
		Task->AccessErrorCode=StreamError::Success;
		Task->TotalSizeCompleted=0;
		return Task;
	}
	Task->Setup(Buffer,Size);
	fReadTaskQueue.EnqueueTask(Task);

	UpdateReadTaskQueue();
	return Task;
}
//---------------------------------------------------------------------------
iPtr<iStreamTask> bcStream::WriteAsync(const void *Buffer,uIntn Size)
{
	if(fStreamClosed || fStreamWriteEnded){
		fLastErrorCode=StreamError::Closed;
		return nullptr;
	}
	auto Task=iCreate<cWriteTask>();
	Task->Owner=this;
	Task->Setup(Buffer,Size);
	fWriteTaskQueue.EnqueueTask(Task);
	UpdateWriteTaskQueue();

	return Task;
}
//---------------------------------------------------------------------------
rPtr<bcStream::cReadTask> bcStream::QueryReadTask(void)
{
	auto Task=fReadTaskQueue.DequeueTask();
	if(Task==nullptr)
		return nullptr;

	auto pTask=Task.ExtractToManual();
	return rPtr<cReadTask>::TakeFromManual(static_cast<cReadTask*>(pTask));
}
//---------------------------------------------------------------------------
void bcStream::CompleteReadTask(rPtr<cReadTask> Task)
{
	fReadTaskQueue.CompleteTask(cnVar::MoveCast(Task));
}
//---------------------------------------------------------------------------
rPtr<bcStream::cWriteTask> bcStream::QueryWriteTask(void)
{
	auto Task=fWriteTaskQueue.DequeueTask();
	if(Task==nullptr){
		if(fStreamWriteSetEnd){
			StreamProcessWriteSetEnd();
			fStreamWriteEnded=true;
		}
		return nullptr;
	}

	auto pTask=Task.ExtractToManual();
	return rPtr<cWriteTask>::TakeFromManual(static_cast<cWriteTask*>(pTask));
}
//---------------------------------------------------------------------------
void bcStream::CompleteWriteTask(rPtr<cWriteTask> Task)
{
	fWriteTaskQueue.CompleteTask(cnVar::MoveCast(Task));
}
//---------------------------------------------------------------------------
void bcStream::SetReadEnd(bool GracefulClose)
{
	if(GracefulClose){
		fStreamReadEnded=true;
		UpdateReadTaskQueue();
	}
	else{
		Close();
	}
}
//---------------------------------------------------------------------------
void bcStream::SetWriteEnd(bool GracefulClose)
{
	if(GracefulClose){
		if(fStreamWriteSetEnd){
			if(fWriteTaskQueue.IsEmpty()){
				// stream gracefully shut down
				fStreamWriteEnded=true;
				UpdateWriteTaskQueue();
				return;
			}
		}
	}
	Close();
}
//---------------------------------------------------------------------------
void bcStream::UpdateReadTaskQueue(void)
{
	fProcessReadTaskQueueProcedure.Run();
}
//---------------------------------------------------------------------------
bool bcStream::cProcessReadTaskQueueProcedure::Procedure(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcStream::fProcessReadTaskQueueProcedure);
	return Host->ProcessReadTaskQueueProc();
}
//---------------------------------------------------------------------------
bool bcStream::ProcessReadTaskQueueProc(void)
{
	if(fStreamClosed){
		// close all read task
		auto Task=fReadTaskQueue.DequeueTask();
		while(Task!=nullptr){

			auto *ReadTask=static_cast<cReadTask*>(Task.Pointer());
			ReadTask->AccessErrorCode=StreamError::Closed;
			fReadTaskQueue.CompleteTask(Task);

			Task=fReadTaskQueue.DequeueTask();
		}

		// process when read end
		StreamProcessReadEnd();
		return false;
	}
	if(fStreamReadEnded){
		// complete all read task
		auto Task=fReadTaskQueue.DequeueTask();
		while(Task!=nullptr){

			auto *ReadTask=static_cast<cReadTask*>(Task.Pointer());
			ReadTask->AccessErrorCode=StreamError::Success;
			fReadTaskQueue.CompleteTask(Task);

			Task=fReadTaskQueue.DequeueTask();
		}
		return false;
	}

	StreamProcessReadTask();
	return false;
}
//---------------------------------------------------------------------------
void bcStream::UpdateWriteTaskQueue(void)
{
	fProcessWriteTaskQueueProcedure.Run();
}
//---------------------------------------------------------------------------
bool bcStream::cProcessWriteTaskQueueProcedure::Procedure(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcStream::fProcessWriteTaskQueueProcedure);
	return Host->ProcessWriteTaskQueueProc();
}
//---------------------------------------------------------------------------
bool bcStream::ProcessWriteTaskQueueProc(void)
{
	if(fStreamClosed){
		// close all write task
		auto Task=fWriteTaskQueue.DequeueTask();
		while(Task!=nullptr){

			auto *WriteTask=static_cast<cWriteTask*>(Task.Pointer());
			WriteTask->AccessErrorCode=StreamError::Closed;
			fWriteTaskQueue.CompleteTask(Task);

			Task=fWriteTaskQueue.DequeueTask();
		}

		StreamProcessWriteEnd();
		return false;
	}
	if(fStreamWriteEnded){
		// cancel all write task
		auto Task=fWriteTaskQueue.DequeueTask();
		while(Task!=nullptr){

			auto *WriteTask=static_cast<cWriteTask*>(Task.Pointer());
			WriteTask->AccessErrorCode=StreamError::Cancelled;
			fWriteTaskQueue.CompleteTask(Task);

			Task=fWriteTaskQueue.DequeueTask();
		}
	}

	StreamProcessWriteTask();
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void* bcFileStream::cAccessTask::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	return ImpCastInterface<iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
bool bcFileStream::cAccessTask::IsDone(void)
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool bcFileStream::cAccessTask::SetNotify(iProcedure *NotifyProcedure)
{
	return fTaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
bool bcFileStream::cAccessTask::GetResult(uIntn &SizeCompleted)
{
	if(fTaskState.IsDone()==false){
		SizeCompleted=0;
		return false;
	}
	SizeCompleted=TotalSizeCompleted;
	return AccessErrorCode==StreamError::Success;
}
//---------------------------------------------------------------------------
eStreamError bcFileStream::cAccessTask::GetStreamError(void)
{
	return AccessErrorCode;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcFileStream::cReadTask::Cancel(void)
{
	Owner->fReadTaskQueue.CancelTask(this);
}
//---------------------------------------------------------------------------
void bcFileStream::cReadTask::PrepareBuffer(void *Buffer,uIntn BufferSize,uInt64 pAccessOffset)
{
	fAccessBuffer.Pointer=Buffer;
	fAccessBuffer.Length=BufferSize;
	AccessOffset=pAccessOffset;
	TotalSizeCompleted=0;
}
//---------------------------------------------------------------------------
cMemory bcFileStream::cReadTask::ReserveWriteBuffer(uIntn)
{
	if(fAccessBufferOffset>=fAccessBuffer.Length){
		// no more buffer
		return NullArray;
	}
	cMemory Buffer;
	Buffer.Pointer=cnMemory::PointerAddByteOffset(fAccessBuffer.Pointer,fAccessBufferOffset);
	Buffer.Length=fAccessBuffer.Length-fAccessBufferOffset;
	return Buffer;
}
//---------------------------------------------------------------------------
void bcFileStream::cReadTask::CommitWriteBuffer(uIntn Size)
{
	if(fAccessBufferOffset+Size>=fAccessBuffer.Length){
		Size=fAccessBuffer.Length-fAccessBufferOffset;
		TotalSizeCompleted+=Size;
		// next buffer
		fAccessBufferOffset=0;
	}
	else{
		fAccessBufferOffset+=Size;
		TotalSizeCompleted+=Size;
	}
}
//---------------------------------------------------------------------------
bool bcFileStream::cReadTask::IsAccessBufferCompleted(void)const
{
	return fAccessBufferOffset>=fAccessBuffer.Length;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcFileStream::cWriteTask::Cancel(void)
{
	Owner->fWriteTaskQueue.CancelTask(this);
}
//---------------------------------------------------------------------------
void bcFileStream::cWriteTask::PrepareBuffer(const void *Buffer,uIntn BufferSize,uInt64 pAccessOffset)
{
	fAccessBuffer.Pointer=Buffer;
	fAccessBuffer.Length=BufferSize;
	AccessOffset=pAccessOffset;
	TotalSizeCompleted=0;
}
//---------------------------------------------------------------------------
cConstMemory bcFileStream::cWriteTask::GatherReadBuffer(uIntn)
{
	if(fAccessBufferOffset>=fAccessBuffer.Length){
		// no more buffer
		return NullArray;
	}
	cConstMemory Buffer;
	Buffer.Pointer=cnMemory::PointerAddByteOffset(fAccessBuffer.Pointer,fAccessBufferOffset);
	Buffer.Length=fAccessBuffer.Length-fAccessBufferOffset;
	return Buffer;
}
//---------------------------------------------------------------------------
void bcFileStream::cWriteTask::DismissReadBuffer(uIntn Size)
{
	if(fAccessBufferOffset+Size>=fAccessBuffer.Length){
		Size=fAccessBuffer.Length-fAccessBufferOffset;
		TotalSizeCompleted+=Size;
		// next buffer
		fAccessBufferOffset=0;
	}
	else{
		fAccessBufferOffset+=Size;
		TotalSizeCompleted+=Size;
	}
}
//---------------------------------------------------------------------------
bool bcFileStream::cWriteTask::IsAccessBufferCompleted(void)const
{
	return fAccessBufferOffset>=fAccessBuffer.Length;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcFileStream::bcFileStream()
{
	fStreamClosed=false;
}
//---------------------------------------------------------------------------
bcFileStream::~bcFileStream()
{
}
//---------------------------------------------------------------------------
void* bcFileStream::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	return ImpCastInterface<iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
void bcFileStream::StreamClose(void)
{
	fStreamClosed=true;
	UpdateWriteTaskQueue();
	UpdateReadTaskQueue();
}
//---------------------------------------------------------------------------
void bcFileStream::Close(void)
{
	StreamClose();
}
//---------------------------------------------------------------------------
eStreamError bcFileStream::GetStreamError(void)
{
	return fLastErrorCode;
}
//---------------------------------------------------------------------------
bool bcFileStream::Read(void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)
{
	if(fStreamClosed){
		fLastErrorCode=StreamError::Closed;
		return false;
	}
	auto Task=iCreate<cReadTask>();
	Task->Owner=this;
	Task->PrepareBuffer(Buffer,Size,Offset);
	fReadTaskQueue.EnqueueTask(Task);

	UpdateReadTaskQueue();
	// sync access

	WaitForTask(Task);

	SizeCompleted=Task->TotalSizeCompleted;
	fLastErrorCode=Task->AccessErrorCode;
	return fLastErrorCode==StreamError::Success;
}
//---------------------------------------------------------------------------
bool bcFileStream::Write(const void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)
{
	if(fStreamClosed){
		fLastErrorCode=StreamError::Closed;
		return false;
	}
	auto Task=iCreate<cWriteTask>();
	Task->Owner=this;
	Task->PrepareBuffer(Buffer,Size,Offset);
	fWriteTaskQueue.EnqueueTask(Task);

	UpdateWriteTaskQueue();
	// sync access

	WaitForTask(Task);

	SizeCompleted=Task->TotalSizeCompleted;
	fLastErrorCode=Task->AccessErrorCode;
	return fLastErrorCode==StreamError::Success;
}
//---------------------------------------------------------------------------
iPtr<iStreamTask> bcFileStream::ReadAsync(void *Buffer,uIntn Size,uInt64 Offset)
{
	if(fStreamClosed){
		fLastErrorCode=StreamError::Closed;
		return nullptr;
	}
	auto Task=iCreate<cReadTask>();
	Task->Owner=this;
	Task->PrepareBuffer(Buffer,Size,Offset);
	fReadTaskQueue.EnqueueTask(Task);

	UpdateReadTaskQueue();
	return Task;
}
//---------------------------------------------------------------------------
iPtr<iStreamTask> bcFileStream::WriteAsync(const void *Buffer,uIntn Size,uInt64 Offset)
{
	if(fStreamClosed){
		fLastErrorCode=StreamError::Closed;
		return nullptr;
	}
	auto Task=iCreate<cWriteTask>();
	Task->Owner=this;
	Task->PrepareBuffer(Buffer,Size,Offset);
	fWriteTaskQueue.EnqueueTask(Task);
	UpdateWriteTaskQueue();

	return Task;
}
//---------------------------------------------------------------------------
rPtr<bcFileStream::cReadTask> bcFileStream::QueryReadTask(void)
{
	auto Task=fReadTaskQueue.DequeueTask();
	if(Task==nullptr)
		return nullptr;

	auto pTask=Task.ExtractToManual();
	return rPtr<cReadTask>::TakeFromManual(static_cast<cReadTask*>(pTask));
}
//---------------------------------------------------------------------------
void bcFileStream::CompleteReadTask(rPtr<cReadTask> Task)
{
	fReadTaskQueue.CompleteTask(cnVar::MoveCast(Task));
}
//---------------------------------------------------------------------------
rPtr<bcFileStream::cWriteTask> bcFileStream::QueryWriteTask(void)
{
	auto Task=fWriteTaskQueue.DequeueTask();
	if(Task==nullptr){
		return nullptr;
	}

	auto pTask=Task.ExtractToManual();
	return rPtr<cWriteTask>::TakeFromManual(static_cast<cWriteTask*>(pTask));
}
//---------------------------------------------------------------------------
void bcFileStream::CompleteWriteTask(rPtr<cWriteTask> Task)
{
	fWriteTaskQueue.CompleteTask(cnVar::MoveCast(Task));
}
//---------------------------------------------------------------------------
void bcFileStream::UpdateReadTaskQueue(void)
{
	fProcessReadTaskQueueProcedure.Run();
}
//---------------------------------------------------------------------------
bool bcFileStream::cProcessReadTaskQueueProcedure::Procedure(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcFileStream::fProcessReadTaskQueueProcedure);
	return Host->ProcessReadTaskQueueProc();
}
//---------------------------------------------------------------------------
bool bcFileStream::ProcessReadTaskQueueProc(void)
{
	if(fStreamClosed){
		// close all read task
		auto Task=fReadTaskQueue.DequeueTask();
		while(Task!=nullptr){

			auto *ReadTask=static_cast<cReadTask*>(Task.Pointer());
			ReadTask->AccessErrorCode=StreamError::Closed;
			fReadTaskQueue.CompleteTask(Task);

			Task=fReadTaskQueue.DequeueTask();
		}

		// process when read end
		StreamProcessReadEnd();
		return false;
	}

	StreamProcessReadTask();
	return false;
}
//---------------------------------------------------------------------------
void bcFileStream::UpdateWriteTaskQueue(void)
{
	fProcessWriteTaskQueueProcedure.Run();
}
//---------------------------------------------------------------------------
bool bcFileStream::cProcessWriteTaskQueueProcedure::Procedure(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcFileStream::fProcessWriteTaskQueueProcedure);
	return Host->ProcessWriteTaskQueueProc();
}
//---------------------------------------------------------------------------
bool bcFileStream::ProcessWriteTaskQueueProc(void)
{
	if(fStreamClosed){
		// close all write task
		auto Task=fWriteTaskQueue.DequeueTask();
		while(Task!=nullptr){

			auto *WriteTask=static_cast<cWriteTask*>(Task.Pointer());
			WriteTask->AccessErrorCode=StreamError::Closed;
			fWriteTaskQueue.CompleteTask(Task);

			Task=fWriteTaskQueue.DequeueTask();
		}

		StreamProcessWriteEnd();
		return false;
	}

	StreamProcessWriteTask();
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcWriteToReadQueue::bcWriteToReadQueue()
{
	fBufferQueue.BufferSizeLimit=65536;
}
//---------------------------------------------------------------------------
bcWriteToReadQueue::~bcWriteToReadQueue()
{
}
//---------------------------------------------------------------------------
void bcWriteToReadQueue::SetBufferSizeLimit(uIntn Value)
{
	fBufferQueue.BufferSizeLimit=Value;
}
//---------------------------------------------------------------------------
void bcWriteToReadQueue::Close(void)
{
	CloseReadQueue();
	CloseWriteQueue();
}
//---------------------------------------------------------------------------
uIntn bcWriteToReadQueue::InitialBuffer(const cConstMemory *Buffers,uIntn BufferCount)
{
	if(BufferCount==0)
		return 0;

	// calculate total length
	uIntn TotalSize=Buffers[0].Length;
	for(uIntn i=1;i<BufferCount;i++){
		TotalSize+=Buffers[i].Length;
	}

	// query write buffer
	auto WriteBuffer=fBufferQueue.ReserveWriteBuffer(TotalSize);
	uIntn WriteOffset=0;
	for(uIntn i=0;i<BufferCount;i++){
		auto &CurBuffer=Buffers[i];
		// copy each buffer

		if(WriteOffset+CurBuffer.Length>WriteBuffer.Length){
			// not enough buffer in buffer queue
			uIntn CopySize=WriteBuffer.Length-WriteOffset;
			cnMemory::Copy(cnMemory::PointerAddByteOffset(WriteBuffer.Pointer,WriteOffset),CurBuffer.Pointer,CopySize);
			fBufferQueue.CommitWriteBuffer(WriteBuffer.Length);
			return WriteBuffer.Length;
		}
		cnMemory::Copy(cnMemory::PointerAddByteOffset(WriteBuffer.Pointer,WriteOffset),CurBuffer.Pointer,CurBuffer.Length);
		WriteOffset+=CurBuffer.Length;
	}
	fBufferQueue.CommitWriteBuffer(WriteOffset);
	return WriteOffset;
}
//---------------------------------------------------------------------------
void bcWriteToReadQueue::WriteQueueClosed(void)
{
	bcRWQueue::WriteQueueClosed();

	if(WriteQueueIsEnded()==false){
		ReadQueueReportTerminated();
	}
	else{
		ReadQueueReportEnded();
	}
}
//---------------------------------------------------------------------------
void bcWriteToReadQueue::ReadQueueClosed(void)
{
	bcRWQueue::ReadQueueClosed();

	WriteQueueReportTerminated();
}
//---------------------------------------------------------------------------
cMemory bcWriteToReadQueue::ReserveWriteBuffer(uIntn QuerySize)
{
	auto WriteBuffer=fBufferQueue.ReserveWriteBuffer(QuerySize);
	if(WriteBuffer.Length==0){
		return NullArray;
	}

	return WriteBuffer;
}
//---------------------------------------------------------------------------
void bcWriteToReadQueue::CommitWriteBuffer(uIntn Size)
{
	if(Size==0)
		return;
	fBufferQueue.CommitWriteBuffer(Size);

	// notify data available for reading
	ReadQueueReportBufferAvailable(false);
}
//---------------------------------------------------------------------------
cConstMemory bcWriteToReadQueue::GatherReadBuffer(uIntn QuerySize)
{
	return fBufferQueue.GatherReadBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void bcWriteToReadQueue::DismissReadBuffer(uIntn Size)
{
	if(Size==0)
		return;
	fBufferQueue.DismissReadBuffer(Size);

	// notify space available for writing
	WriteQueueReportBufferAvailable(false);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cChannelQueuePair::cChannelQueuePair()
{
}
//---------------------------------------------------------------------------
cChannelQueuePair::~cChannelQueuePair()
{
}
//---------------------------------------------------------------------------
void cChannelQueuePair::SetupAsyncExecution(iAsyncExecution *AsyncExecution)
{
	fQueueA.CreateReadQueueWork(AsyncExecution,false);
	fQueueB.CreateReadQueueWork(AsyncExecution,false);
	fQueueA.CreateWriteQueueWork(AsyncExecution,false);
	fQueueB.CreateWriteQueueWork(AsyncExecution,false);
}
//---------------------------------------------------------------------------
iReadQueue* cChannelQueuePair::GetReadQueue(bool Side)
{
	if(Side){
		// T side
		return &fQueueA;
	}
	else{
		// F side
		return &fQueueB;
	}
}
//---------------------------------------------------------------------------
iWriteQueue* cChannelQueuePair::GetWriteQueue(bool Side)
{
	if(Side){
		// T side
		return &fQueueB;
	}
	else{
		// F side
		return &fQueueA;
	}
}
//---------------------------------------------------------------------------
void cChannelQueuePair::Close(bool Side)
{
	if(Side){
		// T side
		fQueueA.CloseReadQueue();
		fQueueB.CloseWriteQueue();
	}
	else{
		// F side
		fQueueA.CloseWriteQueue();
		fQueueB.CloseReadQueue();
	}
}
//---------------------------------------------------------------------------
void cChannelQueuePair::SetWriteEndMode(bool Side,eEndpointWriteEndMode EndMode)
{
	if(Side){
		// T side
		fQueueB.WriteQueueSetEndMode(EndMode);
	}
	else{
		// F side
		fQueueA.WriteQueueSetEndMode(EndMode);
	}
}
//---------------------------------------------------------------------------
cChannelQueuePair* cChannelQueuePair::cQueueA::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cChannelQueuePair::fQueueA);
}
//---------------------------------------------------------------------------
iReference* cChannelQueuePair::cQueueA::RWQueueInnerReference(void)
{
	return GetHost();
}
//---------------------------------------------------------------------------
void cChannelQueuePair::cQueueA::IncreaseReference(void)noexcept(true)
{
	rIncReference(GetHost(),'queA');
}
//---------------------------------------------------------------------------
void cChannelQueuePair::cQueueA::DecreaseReference(void)noexcept(true)
{
	rDecReference(GetHost(),'queA');
}
//---------------------------------------------------------------------------
cChannelQueuePair* cChannelQueuePair::cQueueB::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cChannelQueuePair::fQueueB);
}
//---------------------------------------------------------------------------
iReference* cChannelQueuePair::cQueueB::RWQueueInnerReference(void)
{
	return GetHost();
}
//---------------------------------------------------------------------------
void cChannelQueuePair::cQueueB::IncreaseReference(void)noexcept(true)
{
	rIncReference(GetHost(),'queB');
}
//---------------------------------------------------------------------------
void cChannelQueuePair::cQueueB::DecreaseReference(void)noexcept(true)
{
	rDecReference(GetHost(),'queB');
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cChannelEndpointPair::cChannelEndpointPair()
{
}
//---------------------------------------------------------------------------
cChannelEndpointPair::~cChannelEndpointPair()
{
}
//---------------------------------------------------------------------------
iEndpoint* cChannelEndpointPair::GetEndpoint(bool Side)
{
	if(Side){
		return &fEndpointT;
	}
	else{
		return &fEndpointF;
	}
}
//---------------------------------------------------------------------------
cChannelEndpointPair* cChannelEndpointPair::cEndpointF::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cChannelEndpointPair::fEndpointF);
}
//---------------------------------------------------------------------------
void* cChannelEndpointPair::cEndpointF::CastInterface(iTypeID TypeID)noexcept(true)
{
	if(TypeID==TInterfaceID<iReference>::Value){
		auto Host=cnMemory::GetObjectFromMemberPointer(this,&cChannelEndpointPair::fEndpointF);
		return static_cast<iReference*>(Host);
	}
	return nullptr;
}
//---------------------------------------------------------------------------
void cChannelEndpointPair::cEndpointF::Close(void)
{
	auto Host=GetHost();
	Host->Close(false);
}
//---------------------------------------------------------------------------
iReadQueue* cChannelEndpointPair::cEndpointF::GetReadQueue(void)
{
	auto Host=GetHost();
	return Host->GetReadQueue(false);
}
//---------------------------------------------------------------------------
iWriteQueue* cChannelEndpointPair::cEndpointF::GetWriteQueue(void)
{
	auto Host=GetHost();
	return Host->GetWriteQueue(false);
}
//---------------------------------------------------------------------------
void cChannelEndpointPair::cEndpointF::SetWriteEndMode(eEndpointWriteEndMode EndMode)
{
	auto Host=GetHost();
	Host->SetWriteEndMode(false,EndMode);
}
//---------------------------------------------------------------------------
cChannelEndpointPair* cChannelEndpointPair::cEndpointT::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cChannelEndpointPair::fEndpointT);
}
//---------------------------------------------------------------------------
void* cChannelEndpointPair::cEndpointT::CastInterface(iTypeID TypeID)noexcept(true)
{
	if(TypeID==TInterfaceID<iReference>::Value){
		auto Host=cnMemory::GetObjectFromMemberPointer(this,&cChannelEndpointPair::fEndpointT);
		return static_cast<iReference*>(Host);
	}
	return nullptr;
}
//---------------------------------------------------------------------------
void cChannelEndpointPair::cEndpointT::Close(void)
{
	auto Host=GetHost();
	Host->Close(true);
}
//---------------------------------------------------------------------------
iReadQueue* cChannelEndpointPair::cEndpointT::GetReadQueue(void)
{
	auto Host=GetHost();
	return Host->GetReadQueue(true);
}
//---------------------------------------------------------------------------
iWriteQueue* cChannelEndpointPair::cEndpointT::GetWriteQueue(void)
{
	auto Host=GetHost();
	return Host->GetWriteQueue(true);
}
//---------------------------------------------------------------------------
void cChannelEndpointPair::cEndpointT::SetWriteEndMode(eEndpointWriteEndMode EndMode)
{
	auto Host=GetHost();
	Host->SetWriteEndMode(true,EndMode);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcMultipointQueue::bcMultipointQueue()
	: fMultipointCallback(nullptr)
	, fMultipointQueueTerminated(false)
	, fMultipointQueueReadBufferAvailable(false)
	, fMultipointQueueWriteBufferAvailable(false)
	, fMultipointQueueReadNotifyType(0)
	, fMultipointQueueWriteNotifyType(0)
{
	fMultipointQueueUpdateStateProc=&bcMultipointQueue::MultipointQueueUpdateStateProc_Run;
	fMultipointQueueNotifyReadProc=&bcMultipointQueue::MultipointQueueNotifyReadProc_Run;
}
//---------------------------------------------------------------------------
bcMultipointQueue::~bcMultipointQueue()
{
}
//---------------------------------------------------------------------------
void bcMultipointQueue::CreateMultipointQueueReadWork(iAsyncExecution *AsyncExecution)
{
	if(fNotifyReadProcedureWork!=nullptr)
		return;

	fNotifyReadProcedureWork=AsyncExecution->CreateWork(MultipointQueueInnerReference(),fNotifyReadProcedure);
	fMultipointQueueNotifyReadProc=&bcMultipointQueue::MultipointQueueNotifyReadProc_Async;
}
//---------------------------------------------------------------------------
void bcMultipointQueue::CreateMultipointQueueStateWriteWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop)
{
	if(fStateWriteProcedureWork!=nullptr)
		return;

	fStateWriteProcedureWork=AsyncExecution->CreateWork(MultipointQueueInnerReference(),fStateWriteProcedure);
	if(DisableUpdateThreadLoop){
		fMultipointQueueUpdateStateProc=&bcMultipointQueue::MultipointQueueUpdateStateProc_Async;
	}
	else{
		fMultipointQueueUpdateStateProc=&bcMultipointQueue::MultipointQueueUpdateStateProc_AsyncAfter;
	}
}
//---------------------------------------------------------------------------
void bcMultipointQueue::UpdateMultipointQueueState(bool AsyncNotify)
{
	return (this->*fMultipointQueueUpdateStateProc)(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueUpdateStateProc_Run(bool)
{
	fStateWriteProcedure.Run();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueUpdateStateProc_Async(bool)
{
	fStateWriteProcedure.RunAsync(fStateWriteProcedureWork);
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueUpdateStateProc_AsyncAfter(bool DisableUpdateThreadLoop)
{
	if(DisableUpdateThreadLoop){
		fStateWriteProcedure.RunAsync(fStateWriteProcedureWork);
	}
	else{
		fStateWriteProcedure.RunAsyncAfter(fStateWriteProcedureWork,1);
	}
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueNotifyReadProc_Run(void)
{
	fNotifyReadProcedure.Run();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueNotifyReadProc_Async(void)
{
	fNotifyReadProcedure.RunAsync(fNotifyReadProcedureWork);
}
//---------------------------------------------------------------------------
void bcMultipointQueue::CloseMultipointQueue(void)
{
	if(fRWQueue.CloseNotify()){
		UpdateMultipointQueueState(false);
	}
}
//---------------------------------------------------------------------------
bool bcMultipointQueue::MultipointQueueIsStarted(void)const
{
	return fRWQueue.IsNotificationStarted();
}
//---------------------------------------------------------------------------
bool bcMultipointQueue::MultipointQueueIsClosed(void)const
{
	return fRWQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
bcMultipointQueue* bcMultipointQueue::cRWQueue::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&bcMultipointQueue::fRWQueue);
}
//---------------------------------------------------------------------------
iReference* bcMultipointQueue::cRWQueue::NotificationInnerReference(void)
{
	return GetHost()->MultipointQueueInnerReference();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::cRWQueue::NotificationStarted(void)
{
	auto Host=GetHost();
	iIncReference(Host,'queu');
	Host->fMultipointCallback->AccessStarted();
	Host->MultipointQueueStarted();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::cRWQueue::NotificationStopped(void)
{
	auto Host=GetHost();

	auto Reference=cnVar::MoveCast(Host->fMultipointCallbackReference);
	auto Callback=Host->fMultipointCallback;
	Host->fMultipointCallback=nullptr;
	Callback->AccessStopped();

	Host->MultipointQueueStopped();
	iDecReference(Host,'queu');
}
//---------------------------------------------------------------------------
void bcMultipointQueue::cRWQueue::NotificationClosed(void)
{
	auto Host=GetHost();
	Host->MultipointQueueClosed();
}
//---------------------------------------------------------------------------
bcMultipointQueue::cRWQueue::CycleState bcMultipointQueue::cRWQueue::NotificationCheckState(void)
{
	auto Host=GetHost();
	return Host->MultipointQueueCheckState();
}
//---------------------------------------------------------------------------
bool bcMultipointQueue::cStateWriteProcedure::Procedure(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcMultipointQueue::fStateWriteProcedure);
	return Host->fRWQueue.RunNotifyCycle();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::cRWQueue::NotificationProcedure(void)
{
	auto Host=GetHost();
	Host->MultipointQueueNotify();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueSetTerminated(void)
{
	if(fMultipointQueueTerminated==false){
		fMultipointQueueTerminated=true;
		UpdateMultipointQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueSetReadBufferAvailable(bool AsyncNotify)
{
	if(fMultipointQueueReadBufferAvailable==false){
		fMultipointQueueReadBufferAvailable=true;
		UpdateMultipointQueueState(AsyncNotify);
	}
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueSetWriteBufferAvailable(bool AsyncNotify)
{
	if(fMultipointQueueWriteBufferAvailable==false){
		fMultipointQueueWriteBufferAvailable=true;
		UpdateMultipointQueueState(AsyncNotify);
	}
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueStarted(void)
{
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueStopped(void)
{
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueClosed(void)
{
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueReadNotify(void)
{
	fMultipointCallback->ReadNotify();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueWriteNotify(void)
{
	fMultipointCallback->WriteNotify();
}
//---------------------------------------------------------------------------
bcNotificationCycle::CycleState bcMultipointQueue::MultipointQueueCheckState(void)
{
	if(fMultipointQueueTerminated){
		return bcNotificationCycle::CycleState::Terminated;
	}
	return bcNotificationCycle::CycleState::Normal;
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueNotify(void)
{
	switch(fMultipointQueueWriteNotifyType){
	case 0:
		break;
	case 1:
		if(fMultipointQueueWriteBufferAvailable==false){
			break;
		}
		// notify
		cnLib_SWITCH_FALLTHROUGH;
	case 2:	// always notify
		fMultipointQueueWriteNotifyType=0;
		fMultipointQueueWriteBufferAvailable=false;
		MultipointQueueWriteNotify();
		break;
	}

	(this->*fMultipointQueueNotifyReadProc)();
}
//---------------------------------------------------------------------------
bool bcMultipointQueue::cNotifyReadProcedure::Procedure(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcMultipointQueue::fNotifyReadProcedure);
	Host->MultipointQueueNotifyRead();
	return false;
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueNotifyRead(void)
{
	switch(fMultipointQueueReadNotifyType){
	case 0:
		break;
	case 1:
		if(fMultipointQueueReadBufferAvailable==false){
			break;
		}
		// notify
		cnLib_SWITCH_FALLTHROUGH;
	case 2:	// always notify
		fMultipointQueueReadNotifyType=0;
		fMultipointQueueReadBufferAvailable=false;
		MultipointQueueReadNotify();
		break;
	}
}
//---------------------------------------------------------------------------
bool bcMultipointQueue::StartAccess(iReference *Reference,iMultipointQueueCallback *Callback)
{
	if(fRWQueue.PrepareStartNotify()==false)
		return false;

	fMultipointCallback=Callback;
	fMultipointCallbackReference=Reference;

	fRWQueue.ConfirmStartNotify();
	UpdateMultipointQueueState(false);
	return true;
}
//---------------------------------------------------------------------------
void bcMultipointQueue::StopAccess(void)
{
	fRWQueue.StopNotify();
}
//---------------------------------------------------------------------------
bool bcMultipointQueue::IsClosed(void)
{
	return fRWQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::NotifyRead(bool IdleNotify)
{
	if(IdleNotify){
		if(fMultipointQueueReadNotifyType<2){
			fMultipointQueueReadNotifyType=2;
			UpdateMultipointQueueState(true);
		}
	}
	else{
		if(fMultipointQueueReadNotifyType<=0){
			fMultipointQueueReadNotifyType=1;
			UpdateMultipointQueueState(false);
		}
	}
}
//---------------------------------------------------------------------------
void bcMultipointQueue::NotifyWrite(bool IdleNotify)
{
	if(IdleNotify){
		if(fMultipointQueueReadNotifyType<2){
			fMultipointQueueReadNotifyType=2;
			UpdateMultipointQueueState(true);
		}
	}
	else{
		if(fMultipointQueueReadNotifyType<=0){
			fMultipointQueueReadNotifyType=1;
			UpdateMultipointQueueState(false);
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#pragma message (cnLib_FILE_LINE " : TODO - ExtEndpoint")
cBufferedExtEndpoint::cBufferedExtEndpoint()
{
	fReadQueueDataSize=0;
}
//---------------------------------------------------------------------------
cBufferedExtEndpoint::~cBufferedExtEndpoint()
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::Close(void)
{
	CloseReadQueue();
	CloseWriteQueue();
}
//---------------------------------------------------------------------------
iReadQueue* cBufferedExtEndpoint::GetReadQueue(void)
{
	return this;
}
//---------------------------------------------------------------------------
iWriteQueue* cBufferedExtEndpoint::GetWriteQueue(void)
{
	return this;
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::SetWriteEndMode(eEndpointWriteEndMode EndMode)
{
	//SetWriteQueueEnd();
}
//---------------------------------------------------------------------------
uIntn cBufferedExtEndpoint::GetTotalBufferedReadData(void)const
{
	return fReadQueueDataSize;
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::PutReadData(const void *Data,uIntn Size)
{
	auto WriteMemory=fReadDataQueue.ReserveWriteBuffer(Size);
	cnMemory::Copy(WriteMemory.Pointer,Data,Size);
	fReadDataQueue.CommitWriteBuffer(Size);
	fReadQueueDataSize.Free+=Size;

	UpdateReadQueueState(false);
}
//---------------------------------------------------------------------------
cMemory cBufferedExtEndpoint::QueryReadDataBuffer(uIntn QuerySize)
{
	return fReadDataQueue.ReserveWriteBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::AdvanceReadDataBuffer(uIntn Size)
{
	return fReadDataQueue.CommitWriteBuffer(Size);
}
//---------------------------------------------------------------------------
cConstMemory cBufferedExtEndpoint::QueryWriteData(void)
{
	return fWriteDataQueue.GatherReadBuffer(0);
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::AdvanceWriteData(uIntn Size)
{
	fWriteDataQueue.DismissReadBuffer(Size);
	fReadQueueDataSize.Free-=Size;
}
//---------------------------------------------------------------------------
bool cBufferedExtEndpoint::IsWriteDataEnded(void)
{
	return false;
}
//---------------------------------------------------------------------------
cConstMemory cBufferedExtEndpoint::GatherReadBuffer(uIntn QuerySize)
{
	return fReadDataQueue.GatherReadBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::DismissReadBuffer(uIntn Size)
{
	return fReadDataQueue.DismissReadBuffer(Size);
}
//---------------------------------------------------------------------------
cMemory cBufferedExtEndpoint::ReserveWriteBuffer(uIntn QuerySize)
{
	return fWriteDataQueue.ReserveWriteBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::CommitWriteBuffer(uIntn Size)
{
	return fWriteDataQueue.CommitWriteBuffer(Size);
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::ReadQueueStarted(void)
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::ReadQueueStopped(void)
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::ReadQueueClosed(void)
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::WriteQueueStarted(void)
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::WriteQueueStopped(void)
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::WriteQueueClosed(void)
{
}
//---------------------------------------------------------------------------
