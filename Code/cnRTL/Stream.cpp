#include "Stream.h"
//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcReadQueue::bcReadQueue()noexcept
	: fReadCallback(nullptr)
	, fReadQueueEnded(false)
	, fReadQueueTerminated(false)
	, fReadQueueBufferAvailable(false)
{
}
//---------------------------------------------------------------------------
bcReadQueue::~bcReadQueue()noexcept
{
}
//---------------------------------------------------------------------------
void bcReadQueue::NotificationStarted(void)noexcept
{
	bcAsyncQueue::NotificationStarted();

	rIncReference(this,'rdqu');
	fReadCallback->ReadStarted();
}
//---------------------------------------------------------------------------
void bcReadQueue::NotificationStopped(void)noexcept
{
	bcAsyncQueue::NotificationStopped();

	auto Reference=cnVar::MoveCast(fReadCallbackReference);
	auto Callback=fReadCallback;
	fReadCallback=nullptr;
	Callback->ReadStopped();

	rDecReference(this,'rdqu');
}
//---------------------------------------------------------------------------
bcReadQueue::CycleState bcReadQueue::NotificationCheckState(void)noexcept
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
void bcReadQueue::AsyncQueueNotify(void)noexcept
{
	fReadCallback->ReadNotify();
}
//---------------------------------------------------------------------------
bool bcReadQueue::StartRead(iReference *Reference,iReadQueueCallback *Callback)noexcept
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
void bcReadQueue::StopRead(void)noexcept
{
	if(StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcReadQueue::NotifyRead(uIntn SizeToNotify)noexcept
{
	bool IdleNotify=SizeToNotify==0;
	if(fReadQueueEnded)
		IdleNotify=true;
	return NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool bcReadQueue::IsReadClosed(bool &GracefulClose)noexcept
{
	GracefulClose=IsNotificationEnded();
	return IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcReadQueue::ReadQueueReportBufferAvailable(bool AsyncNotify)noexcept
{
	fReadQueueBufferAvailable=true;
	AsyncQueueSetAvailable(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcReadQueue::ReadQueueReportBufferEmpty(void)noexcept
{
	if(fReadQueueBufferAvailable){
		fReadQueueBufferAvailable=false;
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcReadQueue::ReadQueueReportEnded(void)noexcept
{
	if(fReadQueueEnded==false){
		fReadQueueEnded=true;
		AsyncQueueSetAvailable(false);
	}
}
//---------------------------------------------------------------------------
void bcReadQueue::ReadQueueReportTerminated(void)noexcept
{
	if(fReadQueueTerminated==false){
		fReadQueueTerminated=true;
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcWriteQueue::bcWriteQueue()noexcept
	: fWriteCallback(nullptr)
	, fWriteQueueTerminated(false)
	, fWriteQueueEndMode(EndpointWriteEndMode::NextSession)
{
}
//---------------------------------------------------------------------------
bcWriteQueue::~bcWriteQueue()noexcept
{
}
//---------------------------------------------------------------------------
void bcWriteQueue::NotificationStarted(void)noexcept
{
	bcAsyncQueue::NotificationStarted();

	rIncReference(this,'wrqu');
	if(fWriteQueueEndMode==EndpointWriteEndMode::NextSession){
		fWriteQueueEndMode=EndpointWriteEndMode::Idle;
	}
	fWriteCallback->WriteStarted();
}
//---------------------------------------------------------------------------
void bcWriteQueue::NotificationStopped(void)noexcept
{
	bcAsyncQueue::NotificationStopped();

	auto Reference=cnVar::MoveCast(fWriteCallbackReference);
	auto Callback=fWriteCallback;
	fWriteCallback=nullptr;
	Callback->WriteStopped();

	rDecReference(this,'wrqu');
}
//---------------------------------------------------------------------------
bcWriteQueue::CycleState bcWriteQueue::NotificationCheckState(void)noexcept
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
void bcWriteQueue::AsyncQueueNotify(void)noexcept
{
	fWriteCallback->WriteNotify();
}
//---------------------------------------------------------------------------
bool bcWriteQueue::StartWrite(iReference *Reference,iWriteQueueCallback *Callback)noexcept
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
void bcWriteQueue::StopWrite(bool Terminate)noexcept
{
	if(Terminate)
		fWriteQueueTerminated=true;

	if(StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcWriteQueue::NotifyWrite(uIntn SizeToNotify)noexcept
{
	bool IdleNotify=SizeToNotify==0;
	return NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool bcWriteQueue::IsWriteClosed(bool &GracefulClose)noexcept
{
	GracefulClose=IsNotificationEnded();
	return IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcWriteQueue::WriteQueueReportBufferAvailable(bool AsyncNotify)noexcept
{
	AsyncQueueSetAvailable(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcWriteQueue::WriteQueueReportEnded(void)noexcept
{
	return WriteQueueSetEndMode(EndpointWriteEndMode::Idle);
}
//---------------------------------------------------------------------------
void bcWriteQueue::WriteQueueReportTerminated(void)noexcept
{
	if(fWriteQueueTerminated==false){
		fWriteQueueTerminated=true;
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcWriteQueue::WriteQueueSetEndMode(eEndpointWriteEndMode EndMode)noexcept
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
bcRWQueue::bcRWQueue()noexcept
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
bcRWQueue::~bcRWQueue()noexcept
{
}
//---------------------------------------------------------------------------
void bcRWQueue::CreateReadQueueWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop)noexcept
{
	return fReadAsyncQueue.CreateQueueWork(AsyncExecution,DisableUpdateThreadLoop);
}
//---------------------------------------------------------------------------
void bcRWQueue::UpdateReadQueueState(bool AsyncNotify)noexcept
{
	return fReadAsyncQueue.UpdateQueueState(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcRWQueue::CreateWriteQueueWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop)noexcept
{
	return fWriteAsyncQueue.CreateQueueWork(AsyncExecution,DisableUpdateThreadLoop);
}
//---------------------------------------------------------------------------
void bcRWQueue::UpdateWriteQueueState(bool AsyncNotify)noexcept
{
	return fWriteAsyncQueue.UpdateQueueState(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcRWQueue::CloseReadQueue(void)noexcept
{
	return fReadAsyncQueue.CloseQueue();
}
//---------------------------------------------------------------------------
void bcRWQueue::CloseWriteQueue(void)noexcept
{
	return fWriteAsyncQueue.CloseQueue();
}
//---------------------------------------------------------------------------
bool bcRWQueue::ReadQueueIsStarted(void)const noexcept
{
	return fReadAsyncQueue.IsNotificationStarted();
}
//---------------------------------------------------------------------------
bool bcRWQueue::ReadQueueIsClosed(void)const noexcept
{
	return fReadAsyncQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
bool bcRWQueue::ReadQueueIsEnded(void)const noexcept
{
	return fReadAsyncQueue.IsNotificationEnded();
}
//---------------------------------------------------------------------------
bool bcRWQueue::WriteQueueIsStarted(void)const noexcept
{
	return fWriteAsyncQueue.IsNotificationStarted();
}
//---------------------------------------------------------------------------
bool bcRWQueue::WriteQueueIsClosed(void)const noexcept
{
	return fWriteAsyncQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
bool bcRWQueue::WriteQueueIsEnded(void)const noexcept
{
	return fWriteAsyncQueue.IsNotificationEnded();
}
//---------------------------------------------------------------------------
bcRWQueue* bcRWQueue::cReadQueue::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&bcRWQueue::fReadAsyncQueue);
}
//---------------------------------------------------------------------------
iReference* bcRWQueue::cReadQueue::NotificationInnerReference(void)noexcept
{
	return GetHost()->RWQueueInnerReference();
}
//---------------------------------------------------------------------------
void bcRWQueue::cReadQueue::NotificationStarted(void)noexcept
{
	bcAsyncQueue::NotificationStarted();

	auto Host=GetHost();
	rIncReference(static_cast<iReadQueue*>(Host),'rdqu');
	Host->fReadCallback->ReadStarted();
	Host->ReadQueueStarted();
}
//---------------------------------------------------------------------------
void bcRWQueue::cReadQueue::NotificationStopped(void)noexcept
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
void bcRWQueue::cReadQueue::NotificationClosed(void)noexcept
{
	bcAsyncQueue::NotificationClosed();

	auto Host=GetHost();
	Host->ReadQueueClosed();
}
//---------------------------------------------------------------------------
void bcRWQueue::cReadQueue::AsyncQueueNotify(void)noexcept
{
	auto Host=GetHost();
	return Host->ReadQueueNotify();
}
//---------------------------------------------------------------------------
bcRWQueue::CycleState bcRWQueue::cReadQueue::NotificationCheckState(void)noexcept
{
	auto Host=GetHost();
	return Host->ReadQueueCheckState();
}
//---------------------------------------------------------------------------
bcRWQueue::CycleState bcRWQueue::ReadQueueCheckState(void)noexcept
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
bool bcRWQueue::StartRead(iReference *Reference,iReadQueueCallback *Callback)noexcept
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
void bcRWQueue::StopRead(void)noexcept
{
	if(fReadAsyncQueue.StopNotify()){
		fReadAsyncQueue.UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::NotifyRead(uIntn SizeToNotify)noexcept
{
	bool IdleNotify=SizeToNotify==0;
	if(fReadQueueEnded)
		IdleNotify=true;
	return fReadAsyncQueue.NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool bcRWQueue::IsReadClosed(bool &GracefulClose)noexcept
{
	GracefulClose=fReadAsyncQueue.IsNotificationEnded();
	return fReadAsyncQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueReportBufferAvailable(bool AsyncNotify)noexcept
{
	fReadQueueBufferAvailable=true;
	fReadAsyncQueue.AsyncQueueSetAvailable(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueReportBufferEmpty(void)noexcept
{
	if(fReadQueueBufferAvailable){
		fReadQueueBufferAvailable=false;
		fReadAsyncQueue.UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueReportEnded(void)noexcept
{
	if(fReadQueueEnded==false){
		fReadQueueEnded=true;
		fReadAsyncQueue.AsyncQueueSetAvailable(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueReportTerminated(void)noexcept
{
	if(fReadQueueTerminated==false){
		fReadQueueTerminated=true;
		fReadAsyncQueue.UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueStarted(void)noexcept
{
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueStopped(void)noexcept
{
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueClosed(void)noexcept
{
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueNotify(void)noexcept
{
	fReadCallback->ReadNotify();
}
//---------------------------------------------------------------------------
bcRWQueue* bcRWQueue::cWriteQueue::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&bcRWQueue::fWriteAsyncQueue);
}
//---------------------------------------------------------------------------
iReference* bcRWQueue::cWriteQueue::NotificationInnerReference(void)noexcept
{
	return GetHost()->RWQueueInnerReference();
}
//---------------------------------------------------------------------------
void bcRWQueue::cWriteQueue::NotificationStarted(void)noexcept
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
void bcRWQueue::cWriteQueue::NotificationStopped(void)noexcept
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
void bcRWQueue::cWriteQueue::NotificationClosed(void)noexcept
{
	bcAsyncQueue::NotificationClosed();

	auto Host=GetHost();
	return Host->WriteQueueClosed();
}
//---------------------------------------------------------------------------
bcRWQueue::CycleState bcRWQueue::cWriteQueue::NotificationCheckState(void)noexcept
{
	auto Host=GetHost();
	return Host->WriteQueueCheckState();
}
//---------------------------------------------------------------------------
void bcRWQueue::cWriteQueue::AsyncQueueNotify(void)noexcept
{
	auto Host=GetHost();
	Host->WriteQueueNotify();
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueStarted(void)noexcept
{
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueStopped(void)noexcept
{
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueNotify(void)noexcept
{
	fWriteCallback->WriteNotify();
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueClosed(void)noexcept
{
}
//---------------------------------------------------------------------------
bcRWQueue::CycleState bcRWQueue::WriteQueueCheckState(void)noexcept
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
bool bcRWQueue::StartWrite(iReference *Reference,iWriteQueueCallback *Callback)noexcept
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
void bcRWQueue::StopWrite(bool Terminate)noexcept
{
	if(Terminate)
		fWriteQueueTerminated=true;

	if(fWriteAsyncQueue.StopNotify()){
		fWriteAsyncQueue.UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::NotifyWrite(uIntn SizeToNotify)noexcept
{
	bool IdleNotify=SizeToNotify==0;
	if(fReadQueueEnded)
		IdleNotify=true;
	return fWriteAsyncQueue.NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool bcRWQueue::IsWriteClosed(bool &GracefulClose)noexcept
{
	GracefulClose=fWriteAsyncQueue.IsNotificationEnded();
	return fWriteAsyncQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueReportBufferAvailable(bool AsyncNotify)noexcept
{
	fWriteAsyncQueue.AsyncQueueSetAvailable(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueReportEnded(void)noexcept
{
	return WriteQueueSetEndMode(EndpointWriteEndMode::Idle);
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueReportTerminated(void)noexcept
{
	if(fWriteQueueTerminated==false){
		fWriteQueueTerminated=true;
		fWriteAsyncQueue.UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueSetEndMode(eEndpointWriteEndMode EndMode)noexcept
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
void* bcStream::cAccessTask::CastInterface(iTypeID InterfaceID)noexcept
{
	return ImpCastInterface<iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
bool bcStream::cAccessTask::IsDone(void)noexcept
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool bcStream::cAccessTask::SetNotify(iProcedure *NotifyProcedure)noexcept
{
	return fTaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
bool bcStream::cAccessTask::GetResult(uIntn &SizeCompleted)noexcept
{
	if(fTaskState.IsDone()==false){
		SizeCompleted=0;
		return false;
	}
	SizeCompleted=TotalSizeCompleted;
	return AccessErrorCode==StreamError::Success;
}
//---------------------------------------------------------------------------
eStreamError bcStream::cAccessTask::GetStreamError(void)noexcept
{
	return AccessErrorCode;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcStream::cReadTask::Cancel(void)noexcept
{
	Owner->fReadTaskQueue.CancelTask(this);
}
//---------------------------------------------------------------------------
void bcStream::cReadTask::Setup(void *Buffer,uIntn Size)noexcept
{
	cnLib_ASSERT(Buffer!=nullptr);

	fAccessBuffer=Buffer;
	fAccessBufferSize=Size;

	TotalSizeCompleted=0;
}
//---------------------------------------------------------------------------
cMemory bcStream::cReadTask::ReserveWriteBuffer(uIntn)noexcept
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
void bcStream::cReadTask::CommitWriteBuffer(uIntn Size)noexcept
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
bool bcStream::cReadTask::IsAccessBufferCompleted(void)const noexcept
{
	return TotalSizeCompleted>=fAccessBufferSize;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcStream::cWriteTask::Cancel(void)noexcept
{
	Owner->fWriteTaskQueue.CancelTask(this);
}
//---------------------------------------------------------------------------
void bcStream::cWriteTask::Setup(const void *Buffer,uIntn Size)noexcept
{
	cnLib_ASSERT(Buffer!=nullptr);

	fAccessBuffer=Buffer;
	fAccessBufferSize=Size;

	TotalSizeCompleted=0;
}
//---------------------------------------------------------------------------
cConstMemory bcStream::cWriteTask::GatherReadBuffer(uIntn)noexcept
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
void bcStream::cWriteTask::DismissReadBuffer(uIntn Size)noexcept
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
bool bcStream::cWriteTask::IsAccessBufferCompleted(void)const noexcept
{
	return TotalSizeCompleted>=fAccessBufferSize;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcStream::bcStream()noexcept
{
	fStreamClosed=false;
	fStreamReadEnded=false;
	fStreamWriteSetEnd=false;
	fStreamWriteEnded=false;
}
//---------------------------------------------------------------------------
bcStream::~bcStream()noexcept
{
}
//---------------------------------------------------------------------------
void* bcStream::CastInterface(iTypeID InterfaceID)noexcept
{
	return ImpCastInterface<iStream,iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
void bcStream::StreamClose(void)noexcept
{
	fStreamClosed=true;
	UpdateWriteTaskQueue();
	UpdateReadTaskQueue();
}
//---------------------------------------------------------------------------
void bcStream::Close(void)noexcept
{
	StreamClose();
}
//---------------------------------------------------------------------------
bool bcStream::IsEndOfReading(void)noexcept
{
	return fStreamReadEnded;
}
//---------------------------------------------------------------------------
bool bcStream::SetEndOfStream(void)noexcept
{
	if(fStreamClosed)
		return fStreamWriteEnded;

	fStreamWriteSetEnd=true;
	UpdateWriteTaskQueue();
	return true;
}
//---------------------------------------------------------------------------
eStreamError bcStream::GetStreamError(void)noexcept
{
	return fLastErrorCode;
}
//---------------------------------------------------------------------------
bool bcStream::Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept
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
bool bcStream::Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept
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
iPtr<iStreamTask> bcStream::ReadAsync(void *Buffer,uIntn Size)noexcept
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
iPtr<iStreamTask> bcStream::WriteAsync(const void *Buffer,uIntn Size)noexcept
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
rPtr<bcStream::cReadTask> bcStream::QueryReadTask(void)noexcept
{
	auto Task=fReadTaskQueue.DequeueTask();
	if(Task==nullptr)
		return nullptr;

	auto pTask=Task.ExtractToManual();
	return rPtr<cReadTask>::TakeFromManual(static_cast<cReadTask*>(pTask));
}
//---------------------------------------------------------------------------
void bcStream::CompleteReadTask(rPtr<cReadTask> Task)noexcept
{
	fReadTaskQueue.CompleteTask(cnVar::MoveCast(Task));
}
//---------------------------------------------------------------------------
rPtr<bcStream::cWriteTask> bcStream::QueryWriteTask(void)noexcept
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
void bcStream::CompleteWriteTask(rPtr<cWriteTask> Task)noexcept
{
	fWriteTaskQueue.CompleteTask(cnVar::MoveCast(Task));
}
//---------------------------------------------------------------------------
void bcStream::SetReadEnd(bool GracefulClose)noexcept
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
void bcStream::SetWriteEnd(bool GracefulClose)noexcept
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
void bcStream::UpdateReadTaskQueue(void)noexcept
{
	fProcessReadTaskQueueProcedure.Run();
}
//---------------------------------------------------------------------------
bool bcStream::cProcessReadTaskQueueProcedure::Procedure(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcStream::fProcessReadTaskQueueProcedure);
	return Host->ProcessReadTaskQueueProc();
}
//---------------------------------------------------------------------------
bool bcStream::ProcessReadTaskQueueProc(void)noexcept
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
void bcStream::UpdateWriteTaskQueue(void)noexcept
{
	fProcessWriteTaskQueueProcedure.Run();
}
//---------------------------------------------------------------------------
bool bcStream::cProcessWriteTaskQueueProcedure::Procedure(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcStream::fProcessWriteTaskQueueProcedure);
	return Host->ProcessWriteTaskQueueProc();
}
//---------------------------------------------------------------------------
bool bcStream::ProcessWriteTaskQueueProc(void)noexcept
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
void* bcFileStream::cAccessTask::CastInterface(iTypeID InterfaceID)noexcept
{
	return ImpCastInterface<iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
bool bcFileStream::cAccessTask::IsDone(void)noexcept
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool bcFileStream::cAccessTask::SetNotify(iProcedure *NotifyProcedure)noexcept
{
	return fTaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
bool bcFileStream::cAccessTask::GetResult(uIntn &SizeCompleted)noexcept
{
	if(fTaskState.IsDone()==false){
		SizeCompleted=0;
		return false;
	}
	SizeCompleted=TotalSizeCompleted;
	return AccessErrorCode==StreamError::Success;
}
//---------------------------------------------------------------------------
eStreamError bcFileStream::cAccessTask::GetStreamError(void)noexcept
{
	return AccessErrorCode;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcFileStream::cReadTask::Cancel(void)noexcept
{
	Owner->fReadTaskQueue.CancelTask(this);
}
//---------------------------------------------------------------------------
void bcFileStream::cReadTask::PrepareBuffer(void *Buffer,uIntn BufferSize,uInt64 pAccessOffset)noexcept
{
	fAccessBuffer.Pointer=Buffer;
	fAccessBuffer.Length=BufferSize;
	AccessOffset=pAccessOffset;
	TotalSizeCompleted=0;
}
//---------------------------------------------------------------------------
cMemory bcFileStream::cReadTask::ReserveWriteBuffer(uIntn)noexcept
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
void bcFileStream::cReadTask::CommitWriteBuffer(uIntn Size)noexcept
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
bool bcFileStream::cReadTask::IsAccessBufferCompleted(void)const noexcept
{
	return fAccessBufferOffset>=fAccessBuffer.Length;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcFileStream::cWriteTask::Cancel(void)noexcept
{
	Owner->fWriteTaskQueue.CancelTask(this);
}
//---------------------------------------------------------------------------
void bcFileStream::cWriteTask::PrepareBuffer(const void *Buffer,uIntn BufferSize,uInt64 pAccessOffset)noexcept
{
	fAccessBuffer.Pointer=Buffer;
	fAccessBuffer.Length=BufferSize;
	AccessOffset=pAccessOffset;
	TotalSizeCompleted=0;
}
//---------------------------------------------------------------------------
cConstMemory bcFileStream::cWriteTask::GatherReadBuffer(uIntn)noexcept
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
void bcFileStream::cWriteTask::DismissReadBuffer(uIntn Size)noexcept
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
bool bcFileStream::cWriteTask::IsAccessBufferCompleted(void)const noexcept
{
	return fAccessBufferOffset>=fAccessBuffer.Length;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcFileStream::bcFileStream()noexcept
{
	fStreamClosed=false;
}
//---------------------------------------------------------------------------
bcFileStream::~bcFileStream()noexcept
{
}
//---------------------------------------------------------------------------
void* bcFileStream::CastInterface(iTypeID InterfaceID)noexcept
{
	return ImpCastInterface<iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
void bcFileStream::StreamClose(void)noexcept
{
	fStreamClosed=true;
	UpdateWriteTaskQueue();
	UpdateReadTaskQueue();
}
//---------------------------------------------------------------------------
void bcFileStream::Close(void)noexcept
{
	StreamClose();
}
//---------------------------------------------------------------------------
eStreamError bcFileStream::GetStreamError(void)noexcept
{
	return fLastErrorCode;
}
//---------------------------------------------------------------------------
bool bcFileStream::Read(void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)noexcept
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
bool bcFileStream::Write(const void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)noexcept
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
iPtr<iStreamTask> bcFileStream::ReadAsync(void *Buffer,uIntn Size,uInt64 Offset)noexcept
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
iPtr<iStreamTask> bcFileStream::WriteAsync(const void *Buffer,uIntn Size,uInt64 Offset)noexcept
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
rPtr<bcFileStream::cReadTask> bcFileStream::QueryReadTask(void)noexcept
{
	auto Task=fReadTaskQueue.DequeueTask();
	if(Task==nullptr)
		return nullptr;

	auto pTask=Task.ExtractToManual();
	return rPtr<cReadTask>::TakeFromManual(static_cast<cReadTask*>(pTask));
}
//---------------------------------------------------------------------------
void bcFileStream::CompleteReadTask(rPtr<cReadTask> Task)noexcept
{
	fReadTaskQueue.CompleteTask(cnVar::MoveCast(Task));
}
//---------------------------------------------------------------------------
rPtr<bcFileStream::cWriteTask> bcFileStream::QueryWriteTask(void)noexcept
{
	auto Task=fWriteTaskQueue.DequeueTask();
	if(Task==nullptr){
		return nullptr;
	}

	auto pTask=Task.ExtractToManual();
	return rPtr<cWriteTask>::TakeFromManual(static_cast<cWriteTask*>(pTask));
}
//---------------------------------------------------------------------------
void bcFileStream::CompleteWriteTask(rPtr<cWriteTask> Task)noexcept
{
	fWriteTaskQueue.CompleteTask(cnVar::MoveCast(Task));
}
//---------------------------------------------------------------------------
void bcFileStream::UpdateReadTaskQueue(void)noexcept
{
	fProcessReadTaskQueueProcedure.Run();
}
//---------------------------------------------------------------------------
bool bcFileStream::cProcessReadTaskQueueProcedure::Procedure(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcFileStream::fProcessReadTaskQueueProcedure);
	return Host->ProcessReadTaskQueueProc();
}
//---------------------------------------------------------------------------
bool bcFileStream::ProcessReadTaskQueueProc(void)noexcept
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
void bcFileStream::UpdateWriteTaskQueue(void)noexcept
{
	fProcessWriteTaskQueueProcedure.Run();
}
//---------------------------------------------------------------------------
bool bcFileStream::cProcessWriteTaskQueueProcedure::Procedure(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcFileStream::fProcessWriteTaskQueueProcedure);
	return Host->ProcessWriteTaskQueueProc();
}
//---------------------------------------------------------------------------
bool bcFileStream::ProcessWriteTaskQueueProc(void)noexcept
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
bcWriteToReadQueue::bcWriteToReadQueue()noexcept
{
	fBufferQueue.BufferSizeLimit=65536;
}
//---------------------------------------------------------------------------
bcWriteToReadQueue::~bcWriteToReadQueue()noexcept
{
}
//---------------------------------------------------------------------------
void bcWriteToReadQueue::SetBufferSizeLimit(uIntn Value)noexcept
{
	fBufferQueue.BufferSizeLimit=Value;
}
//---------------------------------------------------------------------------
void bcWriteToReadQueue::Close(void)noexcept
{
	CloseReadQueue();
	CloseWriteQueue();
}
//---------------------------------------------------------------------------
uIntn bcWriteToReadQueue::InitialBuffer(const cConstMemory *Buffers,uIntn BufferCount)noexcept
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
void bcWriteToReadQueue::WriteQueueClosed(void)noexcept
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
void bcWriteToReadQueue::ReadQueueClosed(void)noexcept
{
	bcRWQueue::ReadQueueClosed();

	WriteQueueReportTerminated();
}
//---------------------------------------------------------------------------
uIntn bcWriteToReadQueue::GetMaxWriteBufferSize(void)noexcept
{
	return fBufferQueue.BufferSizeLimit;
}
//---------------------------------------------------------------------------
cMemory bcWriteToReadQueue::ReserveWriteBuffer(uIntn QuerySize)noexcept
{
	auto WriteBuffer=fBufferQueue.ReserveWriteBuffer(QuerySize);
	if(WriteBuffer.Length==0){
		return NullArray;
	}

	return WriteBuffer;
}
//---------------------------------------------------------------------------
void bcWriteToReadQueue::CommitWriteBuffer(uIntn Size)noexcept
{
	if(Size==0)
		return;
	fBufferQueue.CommitWriteBuffer(Size);

	// notify data available for reading
	ReadQueueReportBufferAvailable(false);
}
//---------------------------------------------------------------------------
uIntn bcWriteToReadQueue::GetMaxReadBufferSize(void)noexcept
{
	return fBufferQueue.BufferSizeLimit;
}
//---------------------------------------------------------------------------
cConstMemory bcWriteToReadQueue::GatherReadBuffer(uIntn QuerySize)noexcept
{
	return fBufferQueue.GatherReadBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void bcWriteToReadQueue::DismissReadBuffer(uIntn Size)noexcept
{
	if(Size==0)
		return;
	fBufferQueue.DismissReadBuffer(Size);

	// notify space available for writing
	WriteQueueReportBufferAvailable(false);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cChannelQueuePair::cChannelQueuePair()noexcept
{
}
//---------------------------------------------------------------------------
cChannelQueuePair::~cChannelQueuePair()noexcept
{
}
//---------------------------------------------------------------------------
void cChannelQueuePair::SetupAsyncExecution(iAsyncExecution *AsyncExecution)noexcept
{
	fQueueA.CreateReadQueueWork(AsyncExecution,false);
	fQueueB.CreateReadQueueWork(AsyncExecution,false);
	fQueueA.CreateWriteQueueWork(AsyncExecution,false);
	fQueueB.CreateWriteQueueWork(AsyncExecution,false);
}
//---------------------------------------------------------------------------
iReadQueue* cChannelQueuePair::GetReadQueue(bool Side)noexcept
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
iWriteQueue* cChannelQueuePair::GetWriteQueue(bool Side)noexcept
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
void cChannelQueuePair::Close(bool Side)noexcept
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
void cChannelQueuePair::SetWriteEndMode(bool Side,eEndpointWriteEndMode EndMode)noexcept
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
cChannelQueuePair* cChannelQueuePair::cQueueA::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cChannelQueuePair::fQueueA);
}
//---------------------------------------------------------------------------
iReference* cChannelQueuePair::cQueueA::RWQueueInnerReference(void)noexcept
{
	return GetHost();
}
//---------------------------------------------------------------------------
void cChannelQueuePair::cQueueA::IncreaseReference(void)noexcept
{
	rIncReference(GetHost(),'queA');
}
//---------------------------------------------------------------------------
void cChannelQueuePair::cQueueA::DecreaseReference(void)noexcept
{
	rDecReference(GetHost(),'queA');
}
//---------------------------------------------------------------------------
cChannelQueuePair* cChannelQueuePair::cQueueB::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cChannelQueuePair::fQueueB);
}
//---------------------------------------------------------------------------
iReference* cChannelQueuePair::cQueueB::RWQueueInnerReference(void)noexcept
{
	return GetHost();
}
//---------------------------------------------------------------------------
void cChannelQueuePair::cQueueB::IncreaseReference(void)noexcept
{
	rIncReference(GetHost(),'queB');
}
//---------------------------------------------------------------------------
void cChannelQueuePair::cQueueB::DecreaseReference(void)noexcept
{
	rDecReference(GetHost(),'queB');
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cChannelEndpointPair::cChannelEndpointPair()noexcept
{
}
//---------------------------------------------------------------------------
cChannelEndpointPair::~cChannelEndpointPair()noexcept
{
}
//---------------------------------------------------------------------------
iEndpoint* cChannelEndpointPair::GetEndpoint(bool Side)noexcept
{
	if(Side){
		return &fEndpointT;
	}
	else{
		return &fEndpointF;
	}
}
//---------------------------------------------------------------------------
cChannelEndpointPair* cChannelEndpointPair::cEndpointF::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cChannelEndpointPair::fEndpointF);
}
//---------------------------------------------------------------------------
void* cChannelEndpointPair::cEndpointF::CastInterface(iTypeID TypeID)noexcept
{
	if(TypeID==TInterfaceID<iReference>::Value){
		auto Host=cnMemory::GetObjectFromMemberPointer(this,&cChannelEndpointPair::fEndpointF);
		return static_cast<iReference*>(Host);
	}
	return nullptr;
}
//---------------------------------------------------------------------------
void cChannelEndpointPair::cEndpointF::Close(void)noexcept
{
	auto Host=GetHost();
	Host->Close(false);
}
//---------------------------------------------------------------------------
iReadQueue* cChannelEndpointPair::cEndpointF::GetReadQueue(void)noexcept
{
	auto Host=GetHost();
	return Host->GetReadQueue(false);
}
//---------------------------------------------------------------------------
iWriteQueue* cChannelEndpointPair::cEndpointF::GetWriteQueue(void)noexcept
{
	auto Host=GetHost();
	return Host->GetWriteQueue(false);
}
//---------------------------------------------------------------------------
void cChannelEndpointPair::cEndpointF::SetWriteEndMode(eEndpointWriteEndMode EndMode)noexcept
{
	auto Host=GetHost();
	Host->SetWriteEndMode(false,EndMode);
}
//---------------------------------------------------------------------------
cChannelEndpointPair* cChannelEndpointPair::cEndpointT::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cChannelEndpointPair::fEndpointT);
}
//---------------------------------------------------------------------------
void* cChannelEndpointPair::cEndpointT::CastInterface(iTypeID TypeID)noexcept
{
	if(TypeID==TInterfaceID<iReference>::Value){
		auto Host=cnMemory::GetObjectFromMemberPointer(this,&cChannelEndpointPair::fEndpointT);
		return static_cast<iReference*>(Host);
	}
	return nullptr;
}
//---------------------------------------------------------------------------
void cChannelEndpointPair::cEndpointT::Close(void)noexcept
{
	auto Host=GetHost();
	Host->Close(true);
}
//---------------------------------------------------------------------------
iReadQueue* cChannelEndpointPair::cEndpointT::GetReadQueue(void)noexcept
{
	auto Host=GetHost();
	return Host->GetReadQueue(true);
}
//---------------------------------------------------------------------------
iWriteQueue* cChannelEndpointPair::cEndpointT::GetWriteQueue(void)noexcept
{
	auto Host=GetHost();
	return Host->GetWriteQueue(true);
}
//---------------------------------------------------------------------------
void cChannelEndpointPair::cEndpointT::SetWriteEndMode(eEndpointWriteEndMode EndMode)noexcept
{
	auto Host=GetHost();
	Host->SetWriteEndMode(true,EndMode);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcMultipointQueue::bcMultipointQueue()noexcept
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
bcMultipointQueue::~bcMultipointQueue()noexcept
{
}
//---------------------------------------------------------------------------
void bcMultipointQueue::CreateMultipointQueueReadWork(iAsyncExecution *AsyncExecution)noexcept
{
	if(fNotifyReadProcedureWork!=nullptr)
		return;

	fNotifyReadProcedureWork=AsyncExecution->CreateWork(MultipointQueueInnerReference(),fNotifyReadProcedure);
	fMultipointQueueNotifyReadProc=&bcMultipointQueue::MultipointQueueNotifyReadProc_Async;
}
//---------------------------------------------------------------------------
void bcMultipointQueue::CreateMultipointQueueStateWriteWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop)noexcept
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
void bcMultipointQueue::UpdateMultipointQueueState(bool AsyncNotify)noexcept
{
	return (this->*fMultipointQueueUpdateStateProc)(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueUpdateStateProc_Run(bool)noexcept
{
	fStateWriteProcedure.Run();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueUpdateStateProc_Async(bool)noexcept
{
	fStateWriteProcedure.RunAsync(fStateWriteProcedureWork);
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueUpdateStateProc_AsyncAfter(bool DisableUpdateThreadLoop)noexcept
{
	if(DisableUpdateThreadLoop){
		fStateWriteProcedure.RunAsync(fStateWriteProcedureWork);
	}
	else{
		fStateWriteProcedure.RunAsyncAfter(fStateWriteProcedureWork,1);
	}
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueNotifyReadProc_Run(void)noexcept
{
	fNotifyReadProcedure.Run();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueNotifyReadProc_Async(void)noexcept
{
	fNotifyReadProcedure.RunAsync(fNotifyReadProcedureWork);
}
//---------------------------------------------------------------------------
void bcMultipointQueue::CloseMultipointQueue(void)noexcept
{
	if(fRWQueue.CloseNotify()){
		UpdateMultipointQueueState(false);
	}
}
//---------------------------------------------------------------------------
bool bcMultipointQueue::MultipointQueueIsStarted(void)const noexcept
{
	return fRWQueue.IsNotificationStarted();
}
//---------------------------------------------------------------------------
bool bcMultipointQueue::MultipointQueueIsClosed(void)const noexcept
{
	return fRWQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
bcMultipointQueue* bcMultipointQueue::cRWQueue::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&bcMultipointQueue::fRWQueue);
}
//---------------------------------------------------------------------------
iReference* bcMultipointQueue::cRWQueue::NotificationInnerReference(void)noexcept
{
	return GetHost()->MultipointQueueInnerReference();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::cRWQueue::NotificationStarted(void)noexcept
{
	auto Host=GetHost();
	iIncReference(Host,'queu');
	Host->fMultipointCallback->AccessStarted();
	Host->MultipointQueueStarted();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::cRWQueue::NotificationStopped(void)noexcept
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
void bcMultipointQueue::cRWQueue::NotificationClosed(void)noexcept
{
	auto Host=GetHost();
	Host->MultipointQueueClosed();
}
//---------------------------------------------------------------------------
bcMultipointQueue::cRWQueue::CycleState bcMultipointQueue::cRWQueue::NotificationCheckState(void)noexcept
{
	auto Host=GetHost();
	return Host->MultipointQueueCheckState();
}
//---------------------------------------------------------------------------
bool bcMultipointQueue::cStateWriteProcedure::Procedure(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcMultipointQueue::fStateWriteProcedure);
	return Host->fRWQueue.RunNotifyCycle();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::cRWQueue::NotificationProcedure(void)noexcept
{
	auto Host=GetHost();
	Host->MultipointQueueNotify();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueSetTerminated(void)noexcept
{
	if(fMultipointQueueTerminated==false){
		fMultipointQueueTerminated=true;
		UpdateMultipointQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueSetReadBufferAvailable(bool AsyncNotify)noexcept
{
	if(fMultipointQueueReadBufferAvailable==false){
		fMultipointQueueReadBufferAvailable=true;
		UpdateMultipointQueueState(AsyncNotify);
	}
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueSetWriteBufferAvailable(bool AsyncNotify)noexcept
{
	if(fMultipointQueueWriteBufferAvailable==false){
		fMultipointQueueWriteBufferAvailable=true;
		UpdateMultipointQueueState(AsyncNotify);
	}
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueStarted(void)noexcept
{
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueStopped(void)noexcept
{
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueClosed(void)noexcept
{
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueReadNotify(void)noexcept
{
	fMultipointCallback->ReadNotify();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueWriteNotify(void)noexcept
{
	fMultipointCallback->WriteNotify();
}
//---------------------------------------------------------------------------
bcNotificationCycle::CycleState bcMultipointQueue::MultipointQueueCheckState(void)noexcept
{
	if(fMultipointQueueTerminated){
		return bcNotificationCycle::CycleState::Terminated;
	}
	return bcNotificationCycle::CycleState::Normal;
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueNotify(void)noexcept
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
bool bcMultipointQueue::cNotifyReadProcedure::Procedure(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcMultipointQueue::fNotifyReadProcedure);
	Host->MultipointQueueNotifyRead();
	return false;
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueNotifyRead(void)noexcept
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
bool bcMultipointQueue::StartAccess(iReference *Reference,iMultipointQueueCallback *Callback)noexcept
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
void bcMultipointQueue::StopAccess(void)noexcept
{
	fRWQueue.StopNotify();
}
//---------------------------------------------------------------------------
bool bcMultipointQueue::IsClosed(void)noexcept
{
	return fRWQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::NotifyRead(bool IdleNotify)noexcept
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
void bcMultipointQueue::NotifyWrite(bool IdleNotify)noexcept
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
cBufferedExtEndpoint::cBufferedExtEndpoint()noexcept
{
	fReadQueueDataSize=0;
}
//---------------------------------------------------------------------------
cBufferedExtEndpoint::~cBufferedExtEndpoint()noexcept
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::Close(void)noexcept
{
	CloseReadQueue();
	CloseWriteQueue();
}
//---------------------------------------------------------------------------
iReadQueue* cBufferedExtEndpoint::GetReadQueue(void)noexcept
{
	return this;
}
//---------------------------------------------------------------------------
iWriteQueue* cBufferedExtEndpoint::GetWriteQueue(void)noexcept
{
	return this;
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::SetWriteEndMode(eEndpointWriteEndMode EndMode)noexcept
{
	//SetWriteQueueEnd();
}
//---------------------------------------------------------------------------
uIntn cBufferedExtEndpoint::GetTotalBufferedReadData(void)const noexcept
{
	return fReadQueueDataSize;
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::PutReadData(const void *Data,uIntn Size)noexcept
{
	auto WriteMemory=fReadDataQueue.ReserveWriteBuffer(Size);
	cnMemory::Copy(WriteMemory.Pointer,Data,Size);
	fReadDataQueue.CommitWriteBuffer(Size);
	fReadQueueDataSize.Free+=Size;

	UpdateReadQueueState(false);
}
//---------------------------------------------------------------------------
cMemory cBufferedExtEndpoint::QueryReadDataBuffer(uIntn QuerySize)noexcept
{
	return fReadDataQueue.ReserveWriteBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::AdvanceReadDataBuffer(uIntn Size)noexcept
{
	return fReadDataQueue.CommitWriteBuffer(Size);
}
//---------------------------------------------------------------------------
cConstMemory cBufferedExtEndpoint::QueryWriteData(void)noexcept
{
	return fWriteDataQueue.GatherReadBuffer(0);
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::AdvanceWriteData(uIntn Size)noexcept
{
	fWriteDataQueue.DismissReadBuffer(Size);
	fReadQueueDataSize.Free-=Size;
}
//---------------------------------------------------------------------------
bool cBufferedExtEndpoint::IsWriteDataEnded(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
uIntn cBufferedExtEndpoint::GetMaxReadBufferSize(void)noexcept
{
	return fReadDataQueue.BufferSizeLimit;
}
//---------------------------------------------------------------------------
cConstMemory cBufferedExtEndpoint::GatherReadBuffer(uIntn QuerySize)noexcept
{
	return fReadDataQueue.GatherReadBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::DismissReadBuffer(uIntn Size)noexcept
{
	return fReadDataQueue.DismissReadBuffer(Size);
}
//---------------------------------------------------------------------------
uIntn cBufferedExtEndpoint::GetMaxWriteBufferSize(void)noexcept
{
	return fWriteDataQueue.BufferSizeLimit;
}
//---------------------------------------------------------------------------
cMemory cBufferedExtEndpoint::ReserveWriteBuffer(uIntn QuerySize)noexcept
{
	return fWriteDataQueue.ReserveWriteBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::CommitWriteBuffer(uIntn Size)noexcept
{
	return fWriteDataQueue.CommitWriteBuffer(Size);
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::ReadQueueStarted(void)noexcept
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::ReadQueueStopped(void)noexcept
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::ReadQueueClosed(void)noexcept
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::WriteQueueStarted(void)noexcept
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::WriteQueueStopped(void)noexcept
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::WriteQueueClosed(void)noexcept
{
}
//---------------------------------------------------------------------------
