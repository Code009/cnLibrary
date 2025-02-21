#include "Stream.h"
//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcReadQueue::bcReadQueue()noexcept(true)
	: fReadCallback(nullptr)
	, fReadQueueBufferAvailable(false)
	, fReadQueueEnded(false)
	, fReadQueueCloseWhenIdle(false)
	, fReadQueueTerminated(false)
{
}
//---------------------------------------------------------------------------
bcReadQueue::~bcReadQueue()noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcReadQueue::NotificationStarted(void)noexcept(true)
{
	bcAsyncQueue::NotificationStarted();

	rIncReference(this,'rdqu');
	fReadCallback->ReadStarted();
}
//---------------------------------------------------------------------------
void bcReadQueue::NotificationStopped(void)noexcept(true)
{
	bcAsyncQueue::NotificationStopped();

	auto Reference=cnVar::MoveCast(fReadCallbackReference);
	auto Callback=fReadCallback;
	fReadCallback=nullptr;
	Callback->ReadStopped();

	rDecReference(this,'rdqu');
}
//---------------------------------------------------------------------------
bcReadQueue::CycleState bcReadQueue::NotificationCheckState(void)noexcept(true)
{
	if(fReadQueueTerminated){
		return CycleState::Terminated;
	}
	if(fReadQueueBufferAvailable){
		return CycleState::Normal;
	}
	if(fReadQueueEnded || (fReadCallback==nullptr && fReadQueueCloseWhenIdle)){
		return CycleState::Ended;
	}
	return CycleState::Normal;
}
//---------------------------------------------------------------------------
void bcReadQueue::AsyncQueueNotify(void)noexcept(true)
{
	fReadCallback->ReadNotify();
}
//---------------------------------------------------------------------------
bool bcReadQueue::StartRead(iReference *Reference,iReadQueueCallback *Callback)noexcept(true)
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
void bcReadQueue::StopRead(void)noexcept(true)
{
	if(StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcReadQueue::NotifyRead(uIntn SizeToNotify)noexcept(true)
{
	bool IdleNotify=SizeToNotify==0;
	if(fReadQueueEnded)
		IdleNotify=true;
	return NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
void bcReadQueue::CloseRead(bool Terminate)noexcept(true)
{
	if(Terminate)
		fReadQueueTerminated=true;
	else
		fReadQueueCloseWhenIdle=true;

	UpdateQueueState(true);
}
//---------------------------------------------------------------------------
bool bcReadQueue::IsReadClosed(bool &GracefulClose)noexcept(true)
{
	GracefulClose=IsNotificationEnded();
	return IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcReadQueue::ReadQueueReportBufferAvailable(bool AsyncNotify)noexcept(true)
{
	fReadQueueBufferAvailable=true;
	AsyncQueueSetAvailable(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcReadQueue::ReadQueueReportBufferEmpty(void)noexcept(true)
{
	if(fReadQueueBufferAvailable){
		fReadQueueBufferAvailable=false;
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcReadQueue::ReadQueueReportEnded(void)noexcept(true)
{
	if(fReadQueueEnded==false){
		fReadQueueEnded=true;
		AsyncQueueSetAvailable(false);
	}
}
//---------------------------------------------------------------------------
void bcReadQueue::ReadQueueReportTerminated(void)noexcept(true)
{
	if(fReadQueueTerminated==false){
		fReadQueueTerminated=true;
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcWriteQueue::bcWriteQueue()noexcept(true)
	: fWriteCallback(nullptr)
	, fWriteQueueTerminated(false)
	, fWriteQueueCloseWhenIdle(false)
{
}
//---------------------------------------------------------------------------
bcWriteQueue::~bcWriteQueue()noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcWriteQueue::NotificationStarted(void)noexcept(true)
{
	bcAsyncQueue::NotificationStarted();

	rIncReference(this,'wrqu');
	fWriteCallback->WriteStarted();
}
//---------------------------------------------------------------------------
void bcWriteQueue::NotificationStopped(void)noexcept(true)
{
	bcAsyncQueue::NotificationStopped();

	auto Reference=cnVar::MoveCast(fWriteCallbackReference);
	auto Callback=fWriteCallback;
	fWriteCallback=nullptr;
	Callback->WriteStopped();

	rDecReference(this,'wrqu');
}
//---------------------------------------------------------------------------
bcWriteQueue::CycleState bcWriteQueue::NotificationCheckState(void)noexcept(true)
{
	if(fWriteQueueTerminated){
		return CycleState::Terminated;
	}
	if(fWriteCallback==nullptr && fWriteQueueCloseWhenIdle){
		return CycleState::Ended;
	}
	return CycleState::Normal;
}
//---------------------------------------------------------------------------
void bcWriteQueue::AsyncQueueNotify(void)noexcept(true)
{
	fWriteCallback->WriteNotify();
}
//---------------------------------------------------------------------------
bool bcWriteQueue::StartWrite(iReference *Reference,iWriteQueueCallback *Callback)noexcept(true)
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
void bcWriteQueue::StopWrite(void)noexcept(true)
{
	if(StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcWriteQueue::NotifyWrite(uIntn SizeToNotify)noexcept(true)
{
	bool IdleNotify=SizeToNotify==0;
	return NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
void bcWriteQueue::CloseWrite(bool Terminate)noexcept(true)
{
	if(Terminate)
		fWriteQueueTerminated=true;
	else
		fWriteQueueCloseWhenIdle=true;
	UpdateQueueState(true);
}
//---------------------------------------------------------------------------
bool bcWriteQueue::IsWriteClosed(bool &GracefulClose)noexcept(true)
{
	GracefulClose=IsNotificationEnded();
	return IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcWriteQueue::WriteQueueReportBufferAvailable(bool AsyncNotify)noexcept(true)
{
	AsyncQueueSetAvailable(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcWriteQueue::WriteQueueReportTerminated(void)noexcept(true)
{
	if(fWriteQueueTerminated==false){
		fWriteQueueTerminated=true;
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcRWQueue::bcRWQueue()noexcept(true)
	: fReadCallback(nullptr)
	, fWriteCallback(nullptr)
	, fReadQueueBufferAvailable(false)
	, fReadQueueEnded(false)
	, fReadQueueTerminated(false)
	, fWriteQueueTerminated(false)
{
}
//---------------------------------------------------------------------------
bcRWQueue::~bcRWQueue()noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcRWQueue::CreateReadQueueWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop)noexcept(true)
{
	return fReadAsyncQueue.CreateQueueWork(AsyncExecution,DisableUpdateThreadLoop);
}
//---------------------------------------------------------------------------
void bcRWQueue::UpdateReadQueueState(bool AsyncNotify)noexcept(true)
{
	return fReadAsyncQueue.UpdateQueueState(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcRWQueue::CreateWriteQueueWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop)noexcept(true)
{
	return fWriteAsyncQueue.CreateQueueWork(AsyncExecution,DisableUpdateThreadLoop);
}
//---------------------------------------------------------------------------
void bcRWQueue::UpdateWriteQueueState(bool AsyncNotify)noexcept(true)
{
	return fWriteAsyncQueue.UpdateQueueState(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcRWQueue::CloseReadQueue(void)noexcept(true)
{
	return fReadAsyncQueue.CloseQueue();
}
//---------------------------------------------------------------------------
void bcRWQueue::CloseWriteQueue(void)noexcept(true)
{
	return fWriteAsyncQueue.CloseQueue();
}
//---------------------------------------------------------------------------
bool bcRWQueue::ReadQueueIsStarted(void)const noexcept(true)
{
	return fReadAsyncQueue.IsNotificationStarted();
}
//---------------------------------------------------------------------------
bool bcRWQueue::ReadQueueIsClosed(void)const noexcept(true)
{
	return fReadAsyncQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
bool bcRWQueue::ReadQueueIsEnded(void)const noexcept(true)
{
	return fReadAsyncQueue.IsNotificationEnded();
}
//---------------------------------------------------------------------------
bool bcRWQueue::WriteQueueIsStarted(void)const noexcept(true)
{
	return fWriteAsyncQueue.IsNotificationStarted();
}
//---------------------------------------------------------------------------
bool bcRWQueue::WriteQueueIsClosed(void)const noexcept(true)
{
	return fWriteAsyncQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
bool bcRWQueue::WriteQueueIsEnded(void)const noexcept(true)
{
	return fWriteAsyncQueue.IsNotificationEnded();
}
//---------------------------------------------------------------------------
bcRWQueue* bcRWQueue::cReadQueue::GetHost(void)noexcept(true)
{
	return cnMemory::GetObjectFromMemberPointer(this,&bcRWQueue::fReadAsyncQueue);
}
//---------------------------------------------------------------------------
iReference* bcRWQueue::cReadQueue::NotificationInnerReference(void)noexcept(true)
{
	return GetHost()->RWQueueInnerReference();
}
//---------------------------------------------------------------------------
void bcRWQueue::cReadQueue::NotificationStarted(void)noexcept(true)
{
	bcAsyncQueue::NotificationStarted();

	auto Host=GetHost();
	rIncReference(static_cast<iReadQueue*>(Host),'rdqu');
	Host->fReadCallback->ReadStarted();
	Host->ReadQueueStarted();
}
//---------------------------------------------------------------------------
void bcRWQueue::cReadQueue::NotificationStopped(void)noexcept(true)
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
void bcRWQueue::cReadQueue::NotificationClosed(void)noexcept(true)
{
	bcAsyncQueue::NotificationClosed();

	auto Host=GetHost();
	Host->ReadQueueClosed();
}
//---------------------------------------------------------------------------
void bcRWQueue::cReadQueue::AsyncQueueNotify(void)noexcept(true)
{
	auto Host=GetHost();
	return Host->ReadQueueNotify();
}
//---------------------------------------------------------------------------
bcRWQueue::CycleState bcRWQueue::cReadQueue::NotificationCheckState(void)noexcept(true)
{
	auto Host=GetHost();
	return Host->ReadQueueCheckState();
}
//---------------------------------------------------------------------------
bcRWQueue::CycleState bcRWQueue::ReadQueueCheckState(void)noexcept(true)
{
	if(fReadQueueTerminated){
		return CycleState::Terminated;
	}
	if(fReadQueueBufferAvailable){
		return CycleState::Normal;
	}
	if(fReadQueueEnded || (fReadCallback==nullptr && fReadQueueCloseWhenIdle)){
		return CycleState::Ended;
	}
	return CycleState::Normal;
}
//---------------------------------------------------------------------------
bool bcRWQueue::StartRead(iReference *Reference,iReadQueueCallback *Callback)noexcept(true)
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
void bcRWQueue::StopRead(void)noexcept(true)
{
	if(fReadAsyncQueue.StopNotify()){
		fReadAsyncQueue.UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::NotifyRead(uIntn SizeToNotify)noexcept(true)
{
	bool IdleNotify=SizeToNotify==0;
	if(fReadQueueEnded)
		IdleNotify=true;
	return fReadAsyncQueue.NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
void bcRWQueue::CloseRead(bool Terminate)noexcept(true)
{
	if(Terminate)
		fReadQueueTerminated=true;
	else
		fReadQueueCloseWhenIdle=true;

	fReadAsyncQueue.UpdateQueueState(true);
}
//---------------------------------------------------------------------------
bool bcRWQueue::IsReadClosed(bool &GracefulClose)noexcept(true)
{
	GracefulClose=fReadAsyncQueue.IsNotificationEnded();
	return fReadAsyncQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueReportBufferAvailable(bool AsyncNotify)noexcept(true)
{
	fReadQueueBufferAvailable=true;
	fReadAsyncQueue.AsyncQueueSetAvailable(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueReportBufferEmpty(void)noexcept(true)
{
	if(fReadQueueBufferAvailable){
		fReadQueueBufferAvailable=false;
		fReadAsyncQueue.UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueReportEnded(void)noexcept(true)
{
	if(fReadQueueEnded==false){
		fReadQueueEnded=true;
		fReadAsyncQueue.AsyncQueueSetAvailable(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueReportTerminated(void)noexcept(true)
{
	if(fReadQueueTerminated==false){
		fReadQueueTerminated=true;
		fReadAsyncQueue.UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueStarted(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueStopped(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueClosed(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcRWQueue::ReadQueueNotify(void)noexcept(true)
{
	fReadCallback->ReadNotify();
}
//---------------------------------------------------------------------------
bcRWQueue* bcRWQueue::cWriteQueue::GetHost(void)noexcept(true)
{
	return cnMemory::GetObjectFromMemberPointer(this,&bcRWQueue::fWriteAsyncQueue);
}
//---------------------------------------------------------------------------
iReference* bcRWQueue::cWriteQueue::NotificationInnerReference(void)noexcept(true)
{
	return GetHost()->RWQueueInnerReference();
}
//---------------------------------------------------------------------------
void bcRWQueue::cWriteQueue::NotificationStarted(void)noexcept(true)
{
	bcAsyncQueue::NotificationStarted();

	auto Host=GetHost();
	rIncReference(static_cast<iWriteQueue*>(Host),'wrqu');
	Host->fWriteCallback->WriteStarted();
	Host->WriteQueueStarted();
}
//---------------------------------------------------------------------------
void bcRWQueue::cWriteQueue::NotificationStopped(void)noexcept(true)
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
void bcRWQueue::cWriteQueue::NotificationClosed(void)noexcept(true)
{
	bcAsyncQueue::NotificationClosed();

	auto Host=GetHost();
	return Host->WriteQueueClosed();
}
//---------------------------------------------------------------------------
bcRWQueue::CycleState bcRWQueue::cWriteQueue::NotificationCheckState(void)noexcept(true)
{
	auto Host=GetHost();
	return Host->WriteQueueCheckState();
}
//---------------------------------------------------------------------------
void bcRWQueue::cWriteQueue::AsyncQueueNotify(void)noexcept(true)
{
	auto Host=GetHost();
	Host->WriteQueueNotify();
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueStarted(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueStopped(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueNotify(void)noexcept(true)
{
	fWriteCallback->WriteNotify();
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueClosed(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
bcRWQueue::CycleState bcRWQueue::WriteQueueCheckState(void)noexcept(true)
{
	if(fWriteQueueTerminated){
		return CycleState::Terminated;
	}
	if(fWriteCallback==nullptr && fWriteQueueCloseWhenIdle){
		return CycleState::Ended;
	}
	return CycleState::Normal;
}
//---------------------------------------------------------------------------
bool bcRWQueue::StartWrite(iReference *Reference,iWriteQueueCallback *Callback)noexcept(true)
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
void bcRWQueue::StopWrite(void)noexcept(true)
{
	if(fWriteAsyncQueue.StopNotify()){
		fWriteAsyncQueue.UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcRWQueue::NotifyWrite(uIntn SizeToNotify)noexcept(true)
{
	bool IdleNotify=SizeToNotify==0;
	if(fWriteQueueTerminated)
		IdleNotify=true;
	return fWriteAsyncQueue.NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
void bcRWQueue::CloseWrite(bool Terminate)noexcept(true)
{
	if(Terminate)
		fWriteQueueTerminated=true;
	else
		fWriteQueueCloseWhenIdle=true;
	fWriteAsyncQueue.UpdateQueueState(true);
}
//---------------------------------------------------------------------------
bool bcRWQueue::IsWriteClosed(bool &GracefulClose)noexcept(true)
{
	GracefulClose=fWriteAsyncQueue.IsNotificationEnded();
	return fWriteAsyncQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueReportBufferAvailable(bool AsyncNotify)noexcept(true)
{
	fWriteAsyncQueue.AsyncQueueSetAvailable(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcRWQueue::WriteQueueReportTerminated(void)noexcept(true)
{
	if(fWriteQueueTerminated==false){
		fWriteQueueTerminated=true;
		fWriteAsyncQueue.UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void* bcStream::cAccessTask::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	return ImpCastInterface<iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
bool bcStream::cAccessTask::IsDone(void)noexcept(true)
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool bcStream::cAccessTask::Await(iProcedure *NotifyProcedure)noexcept(true)
{
	return fTaskState.Await(NotifyProcedure);
}
//---------------------------------------------------------------------------
bool bcStream::cAccessTask::GetResult(uIntn &SizeCompleted)noexcept(true)
{
	if(fTaskState.IsDone()==false){
		SizeCompleted=0;
		return false;
	}
	SizeCompleted=TotalSizeCompleted;
	return AccessErrorCode==StreamError::Success;
}
//---------------------------------------------------------------------------
eStreamError bcStream::cAccessTask::GetStreamError(void)noexcept(true)
{
	return AccessErrorCode;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcStream::cReadTask::Cancel(void)noexcept(true)
{
	Owner->fReadTaskQueue.CancelTask(this);
}
//---------------------------------------------------------------------------
void bcStream::cReadTask::Setup(void *Buffer,uIntn Size)noexcept(true)
{
	cnLib_ASSERT(Buffer!=nullptr);

	fAccessBuffer=Buffer;
	fAccessBufferSize=Size;

	TotalSizeCompleted=0;
}
//---------------------------------------------------------------------------
cMemory bcStream::cReadTask::ReserveWriteBuffer(uIntn)noexcept(true)
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
void bcStream::cReadTask::CommitWriteBuffer(uIntn Size)noexcept(true)
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
bool bcStream::cReadTask::IsAccessBufferCompleted(void)const noexcept(true)
{
	return TotalSizeCompleted>=fAccessBufferSize;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcStream::cWriteTask::Cancel(void)noexcept(true)
{
	Owner->fWriteTaskQueue.CancelTask(this);
}
//---------------------------------------------------------------------------
void bcStream::cWriteTask::Setup(const void *Buffer,uIntn Size)noexcept(true)
{
	cnLib_ASSERT(Buffer!=nullptr);

	fAccessBuffer=Buffer;
	fAccessBufferSize=Size;

	TotalSizeCompleted=0;
}
//---------------------------------------------------------------------------
cConstMemory bcStream::cWriteTask::GatherReadBuffer(uIntn)noexcept(true)
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
void bcStream::cWriteTask::DismissReadBuffer(uIntn Size)noexcept(true)
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
bool bcStream::cWriteTask::IsAccessBufferCompleted(void)const noexcept(true)
{
	return TotalSizeCompleted>=fAccessBufferSize;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcStream::bcStream()noexcept(true)
{
	fStreamClosed=false;
	fStreamReadEnded=false;
	fStreamWriteSetEnd=false;
	fStreamWriteEnded=false;
}
//---------------------------------------------------------------------------
bcStream::~bcStream()noexcept(true)
{
}
//---------------------------------------------------------------------------
void* bcStream::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	return ImpCastInterface<iStream,iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
void bcStream::StreamClose(void)noexcept(true)
{
	fStreamClosed=true;
	UpdateWriteTaskQueue();
	UpdateReadTaskQueue();
}
//---------------------------------------------------------------------------
void bcStream::Close(void)noexcept(true)
{
	StreamClose();
}
//---------------------------------------------------------------------------
bool bcStream::IsEndOfReading(void)noexcept(true)
{
	return fStreamReadEnded;
}
//---------------------------------------------------------------------------
bool bcStream::SetEndOfStream(void)noexcept(true)
{
	if(fStreamClosed)
		return fStreamWriteEnded;

	fStreamWriteSetEnd=true;
	UpdateWriteTaskQueue();
	return true;
}
//---------------------------------------------------------------------------
eStreamError bcStream::GetStreamError(void)noexcept(true)
{
	return fLastErrorCode;
}
//---------------------------------------------------------------------------
bool bcStream::Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)
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

	WaitForTask(*Task);

	SizeCompleted=Task->TotalSizeCompleted;
	fLastErrorCode=Task->AccessErrorCode;
	return fLastErrorCode==StreamError::Success;
}
//---------------------------------------------------------------------------
bool bcStream::Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)
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

	WaitForTask(*Task);

	SizeCompleted=Task->TotalSizeCompleted;
	fLastErrorCode=Task->AccessErrorCode;
	return fLastErrorCode==StreamError::Success;
}
//---------------------------------------------------------------------------
iPtr<iStreamTask> bcStream::ReadAsync(void *Buffer,uIntn Size)noexcept(true)
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
iPtr<iStreamTask> bcStream::WriteAsync(const void *Buffer,uIntn Size)noexcept(true)
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
rPtr<bcStream::cReadTask> bcStream::QueryReadTask(void)noexcept(true)
{
	auto Task=fReadTaskQueue.DequeueTask();
	if(Task==nullptr)
		return nullptr;

	auto pTask=Task.ExtractToManual();
	return rPtr<cReadTask>::TakeFromManual(static_cast<cReadTask*>(pTask));
}
//---------------------------------------------------------------------------
void bcStream::CompleteReadTask(rPtr<cReadTask> Task)noexcept(true)
{
	fReadTaskQueue.CompleteTask(cnVar::MoveCast(Task));
}
//---------------------------------------------------------------------------
rPtr<bcStream::cWriteTask> bcStream::QueryWriteTask(void)noexcept(true)
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
void bcStream::CompleteWriteTask(rPtr<cWriteTask> Task)noexcept(true)
{
	fWriteTaskQueue.CompleteTask(cnVar::MoveCast(Task));
}
//---------------------------------------------------------------------------
void bcStream::SetReadEnd(bool GracefulClose)noexcept(true)
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
void bcStream::SetWriteEnd(bool GracefulClose)noexcept(true)
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
void bcStream::UpdateReadTaskQueue(void)noexcept(true)
{
	fProcessReadTaskQueueProcedure.Run();
}
//---------------------------------------------------------------------------
bool bcStream::cProcessReadTaskQueueProcedure::Procedure(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcStream::fProcessReadTaskQueueProcedure);
	return Host->ProcessReadTaskQueueProc();
}
//---------------------------------------------------------------------------
bool bcStream::ProcessReadTaskQueueProc(void)noexcept(true)
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
void bcStream::UpdateWriteTaskQueue(void)noexcept(true)
{
	fProcessWriteTaskQueueProcedure.Run();
}
//---------------------------------------------------------------------------
bool bcStream::cProcessWriteTaskQueueProcedure::Procedure(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcStream::fProcessWriteTaskQueueProcedure);
	return Host->ProcessWriteTaskQueueProc();
}
//---------------------------------------------------------------------------
bool bcStream::ProcessWriteTaskQueueProc(void)noexcept(true)
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
bool bcFileStream::cAccessTask::IsDone(void)noexcept(true)
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool bcFileStream::cAccessTask::Await(iProcedure *NotifyProcedure)noexcept(true)
{
	return fTaskState.Await(NotifyProcedure);
}
//---------------------------------------------------------------------------
bool bcFileStream::cAccessTask::GetResult(uIntn &SizeCompleted)noexcept(true)
{
	if(fTaskState.IsDone()==false){
		SizeCompleted=0;
		return false;
	}
	SizeCompleted=TotalSizeCompleted;
	return AccessErrorCode==StreamError::Success;
}
//---------------------------------------------------------------------------
eStreamError bcFileStream::cAccessTask::GetStreamError(void)noexcept(true)
{
	return AccessErrorCode;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcFileStream::cReadTask::Cancel(void)noexcept(true)
{
	Owner->fReadTaskQueue.CancelTask(this);
}
//---------------------------------------------------------------------------
void bcFileStream::cReadTask::PrepareBuffer(void *Buffer,uIntn BufferSize,uInt64 pAccessOffset)noexcept(true)
{
	fAccessBuffer.Pointer=Buffer;
	fAccessBuffer.Length=BufferSize;
	AccessOffset=pAccessOffset;
	TotalSizeCompleted=0;
}
//---------------------------------------------------------------------------
cMemory bcFileStream::cReadTask::ReserveWriteBuffer(uIntn)noexcept(true)
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
void bcFileStream::cReadTask::CommitWriteBuffer(uIntn Size)noexcept(true)
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
bool bcFileStream::cReadTask::IsAccessBufferCompleted(void)const noexcept(true)
{
	return fAccessBufferOffset>=fAccessBuffer.Length;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcFileStream::cWriteTask::Cancel(void)noexcept(true)
{
	Owner->fWriteTaskQueue.CancelTask(this);
}
//---------------------------------------------------------------------------
void bcFileStream::cWriteTask::PrepareBuffer(const void *Buffer,uIntn BufferSize,uInt64 pAccessOffset)noexcept(true)
{
	fAccessBuffer.Pointer=Buffer;
	fAccessBuffer.Length=BufferSize;
	AccessOffset=pAccessOffset;
	TotalSizeCompleted=0;
}
//---------------------------------------------------------------------------
cConstMemory bcFileStream::cWriteTask::GatherReadBuffer(uIntn)noexcept(true)
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
void bcFileStream::cWriteTask::DismissReadBuffer(uIntn Size)noexcept(true)
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
bool bcFileStream::cWriteTask::IsAccessBufferCompleted(void)const noexcept(true)
{
	return fAccessBufferOffset>=fAccessBuffer.Length;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcFileStream::bcFileStream()noexcept(true)
{
	fStreamClosed=false;
}
//---------------------------------------------------------------------------
bcFileStream::~bcFileStream()noexcept(true)
{
}
//---------------------------------------------------------------------------
void* bcFileStream::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	return ImpCastInterface<iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
void bcFileStream::StreamClose(void)noexcept(true)
{
	fStreamClosed=true;
	UpdateWriteTaskQueue();
	UpdateReadTaskQueue();
}
//---------------------------------------------------------------------------
void bcFileStream::Close(void)noexcept(true)
{
	StreamClose();
}
//---------------------------------------------------------------------------
eStreamError bcFileStream::GetStreamError(void)noexcept(true)
{
	return fLastErrorCode;
}
//---------------------------------------------------------------------------
bool bcFileStream::Read(void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)noexcept(true)
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

	WaitForTask(*Task);

	SizeCompleted=Task->TotalSizeCompleted;
	fLastErrorCode=Task->AccessErrorCode;
	return fLastErrorCode==StreamError::Success;
}
//---------------------------------------------------------------------------
bool bcFileStream::Write(const void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)noexcept(true)
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

	WaitForTask(*Task);

	SizeCompleted=Task->TotalSizeCompleted;
	fLastErrorCode=Task->AccessErrorCode;
	return fLastErrorCode==StreamError::Success;
}
//---------------------------------------------------------------------------
iPtr<iStreamTask> bcFileStream::ReadAsync(void *Buffer,uIntn Size,uInt64 Offset)noexcept(true)
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
iPtr<iStreamTask> bcFileStream::WriteAsync(const void *Buffer,uIntn Size,uInt64 Offset)noexcept(true)
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
rPtr<bcFileStream::cReadTask> bcFileStream::QueryReadTask(void)noexcept(true)
{
	auto Task=fReadTaskQueue.DequeueTask();
	if(Task==nullptr)
		return nullptr;

	auto pTask=Task.ExtractToManual();
	return rPtr<cReadTask>::TakeFromManual(static_cast<cReadTask*>(pTask));
}
//---------------------------------------------------------------------------
void bcFileStream::CompleteReadTask(rPtr<cReadTask> Task)noexcept(true)
{
	fReadTaskQueue.CompleteTask(cnVar::MoveCast(Task));
}
//---------------------------------------------------------------------------
rPtr<bcFileStream::cWriteTask> bcFileStream::QueryWriteTask(void)noexcept(true)
{
	auto Task=fWriteTaskQueue.DequeueTask();
	if(Task==nullptr){
		return nullptr;
	}

	auto pTask=Task.ExtractToManual();
	return rPtr<cWriteTask>::TakeFromManual(static_cast<cWriteTask*>(pTask));
}
//---------------------------------------------------------------------------
void bcFileStream::CompleteWriteTask(rPtr<cWriteTask> Task)noexcept(true)
{
	fWriteTaskQueue.CompleteTask(cnVar::MoveCast(Task));
}
//---------------------------------------------------------------------------
void bcFileStream::UpdateReadTaskQueue(void)noexcept(true)
{
	fProcessReadTaskQueueProcedure.Run();
}
//---------------------------------------------------------------------------
bool bcFileStream::cProcessReadTaskQueueProcedure::Procedure(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcFileStream::fProcessReadTaskQueueProcedure);
	return Host->ProcessReadTaskQueueProc();
}
//---------------------------------------------------------------------------
bool bcFileStream::ProcessReadTaskQueueProc(void)noexcept(true)
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
void bcFileStream::UpdateWriteTaskQueue(void)noexcept(true)
{
	fProcessWriteTaskQueueProcedure.Run();
}
//---------------------------------------------------------------------------
bool bcFileStream::cProcessWriteTaskQueueProcedure::Procedure(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcFileStream::fProcessWriteTaskQueueProcedure);
	return Host->ProcessWriteTaskQueueProc();
}
//---------------------------------------------------------------------------
bool bcFileStream::ProcessWriteTaskQueueProc(void)noexcept(true)
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
bcWriteToReadQueue::bcWriteToReadQueue(uIntn BufferSize)noexcept(true)
	: fBufferQueue(BufferSize)
{
}
//---------------------------------------------------------------------------
bcWriteToReadQueue::~bcWriteToReadQueue()noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcWriteToReadQueue::Close(void)noexcept(true)
{
	CloseReadQueue();
	CloseWriteQueue();
}
//---------------------------------------------------------------------------
uIntn bcWriteToReadQueue::InitialBuffer(const cConstMemory *Buffers,uIntn BufferCount)noexcept(true)
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
void bcWriteToReadQueue::WriteQueueClosed(void)noexcept(true)
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
void bcWriteToReadQueue::ReadQueueClosed(void)noexcept(true)
{
	bcRWQueue::ReadQueueClosed();

	WriteQueueReportTerminated();
}
//---------------------------------------------------------------------------
uIntn bcWriteToReadQueue::GetMaxWriteBufferSize(void)noexcept(true)
{
	return fBufferQueue.GetBufferSizeLimit();
}
//---------------------------------------------------------------------------
cMemory bcWriteToReadQueue::ReserveWriteBuffer(uIntn QuerySize)noexcept(true)
{
	auto WriteBuffer=fBufferQueue.ReserveWriteBuffer(QuerySize);
	if(WriteBuffer.Length==0){
		return NullArray;
	}

	return WriteBuffer;
}
//---------------------------------------------------------------------------
void bcWriteToReadQueue::CommitWriteBuffer(uIntn Size)noexcept(true)
{
	if(Size==0)
		return;
	fBufferQueue.CommitWriteBuffer(Size);

	// notify data available for reading
	ReadQueueReportBufferAvailable(false);
}
//---------------------------------------------------------------------------
uIntn bcWriteToReadQueue::GetMaxReadBufferSize(void)noexcept(true)
{
	return fBufferQueue.GetBufferSizeLimit();
}
//---------------------------------------------------------------------------
cConstMemory bcWriteToReadQueue::GatherReadBuffer(uIntn QuerySize)noexcept(true)
{
	return fBufferQueue.GatherReadBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void bcWriteToReadQueue::DismissReadBuffer(uIntn Size)noexcept(true)
{
	if(Size==0)
		return;
	fBufferQueue.DismissReadBuffer(Size);

	// notify space available for writing
	WriteQueueReportBufferAvailable(false);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cChannelQueuePair::cChannelQueuePair()noexcept(true)
{
}
//---------------------------------------------------------------------------
cChannelQueuePair::~cChannelQueuePair()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cChannelQueuePair::SetupAsyncExecution(iAsyncExecution *AsyncExecution)noexcept(true)
{
	fQueueA.CreateReadQueueWork(AsyncExecution,false);
	fQueueB.CreateReadQueueWork(AsyncExecution,false);
	fQueueA.CreateWriteQueueWork(AsyncExecution,false);
	fQueueB.CreateWriteQueueWork(AsyncExecution,false);
}
//---------------------------------------------------------------------------
iReadQueue* cChannelQueuePair::GetReadQueue(bool Side)noexcept(true)
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
iWriteQueue* cChannelQueuePair::GetWriteQueue(bool Side)noexcept(true)
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
void cChannelQueuePair::Close(bool Side)noexcept(true)
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
cChannelQueuePair* cChannelQueuePair::cQueueA::GetHost(void)noexcept(true)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cChannelQueuePair::fQueueA);
}
//---------------------------------------------------------------------------
iReference* cChannelQueuePair::cQueueA::RWQueueInnerReference(void)noexcept(true)
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
cChannelQueuePair* cChannelQueuePair::cQueueB::GetHost(void)noexcept(true)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cChannelQueuePair::fQueueB);
}
//---------------------------------------------------------------------------
iReference* cChannelQueuePair::cQueueB::RWQueueInnerReference(void)noexcept(true)
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
cChannelEndpointPair::cChannelEndpointPair()noexcept(true)
{
}
//---------------------------------------------------------------------------
cChannelEndpointPair::~cChannelEndpointPair()noexcept(true)
{
}
//---------------------------------------------------------------------------
iEndpoint* cChannelEndpointPair::GetEndpoint(bool Side)noexcept(true)
{
	if(Side){
		return &fEndpointT;
	}
	else{
		return &fEndpointF;
	}
}
//---------------------------------------------------------------------------
cChannelEndpointPair* cChannelEndpointPair::cEndpointF::GetHost(void)noexcept(true)
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
void cChannelEndpointPair::cEndpointF::Close(void)noexcept(true)
{
	auto Host=GetHost();
	Host->Close(false);
}
//---------------------------------------------------------------------------
iReadQueue* cChannelEndpointPair::cEndpointF::GetReadQueue(void)noexcept(true)
{
	auto Host=GetHost();
	return Host->GetReadQueue(false);
}
//---------------------------------------------------------------------------
iWriteQueue* cChannelEndpointPair::cEndpointF::GetWriteQueue(void)noexcept(true)
{
	auto Host=GetHost();
	return Host->GetWriteQueue(false);
}
//---------------------------------------------------------------------------
cChannelEndpointPair* cChannelEndpointPair::cEndpointT::GetHost(void)noexcept(true)
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
void cChannelEndpointPair::cEndpointT::Close(void)noexcept(true)
{
	auto Host=GetHost();
	Host->Close(true);
}
//---------------------------------------------------------------------------
iReadQueue* cChannelEndpointPair::cEndpointT::GetReadQueue(void)noexcept(true)
{
	auto Host=GetHost();
	return Host->GetReadQueue(true);
}
//---------------------------------------------------------------------------
iWriteQueue* cChannelEndpointPair::cEndpointT::GetWriteQueue(void)noexcept(true)
{
	auto Host=GetHost();
	return Host->GetWriteQueue(true);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcMultipointQueue::bcMultipointQueue()noexcept(true)
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
bcMultipointQueue::~bcMultipointQueue()noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcMultipointQueue::CreateMultipointQueueReadWork(iAsyncExecution *AsyncExecution)noexcept(true)
{
	if(fNotifyReadProcedureWork!=nullptr)
		return;

	fNotifyReadProcedureWork=AsyncExecution->CreateWork(MultipointQueueInnerReference(),fNotifyReadProcedure);
	fMultipointQueueNotifyReadProc=&bcMultipointQueue::MultipointQueueNotifyReadProc_Async;
}
//---------------------------------------------------------------------------
void bcMultipointQueue::CreateMultipointQueueStateWriteWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop)noexcept(true)
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
void bcMultipointQueue::UpdateMultipointQueueState(bool AsyncNotify)noexcept(true)
{
	return (this->*fMultipointQueueUpdateStateProc)(AsyncNotify);
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueUpdateStateProc_Run(bool)noexcept(true)
{
	fStateWriteProcedure.Run();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueUpdateStateProc_Async(bool)noexcept(true)
{
	fStateWriteProcedure.RunAsync(fStateWriteProcedureWork);
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueUpdateStateProc_AsyncAfter(bool DisableUpdateThreadLoop)noexcept(true)
{
	if(DisableUpdateThreadLoop){
		fStateWriteProcedure.RunAsync(fStateWriteProcedureWork);
	}
	else{
		fStateWriteProcedure.RunAsyncAfter(fStateWriteProcedureWork,1);
	}
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueNotifyReadProc_Run(void)noexcept(true)
{
	fNotifyReadProcedure.Run();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueNotifyReadProc_Async(void)noexcept(true)
{
	fNotifyReadProcedure.RunAsync(fNotifyReadProcedureWork);
}
//---------------------------------------------------------------------------
void bcMultipointQueue::CloseMultipointQueue(void)noexcept(true)
{
	if(fRWQueue.CloseNotify()){
		UpdateMultipointQueueState(false);
	}
}
//---------------------------------------------------------------------------
bool bcMultipointQueue::MultipointQueueIsStarted(void)const noexcept(true)
{
	return fRWQueue.IsNotificationStarted();
}
//---------------------------------------------------------------------------
bool bcMultipointQueue::MultipointQueueIsClosed(void)const noexcept(true)
{
	return fRWQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
bcMultipointQueue* bcMultipointQueue::cRWQueue::GetHost(void)noexcept(true)
{
	return cnMemory::GetObjectFromMemberPointer(this,&bcMultipointQueue::fRWQueue);
}
//---------------------------------------------------------------------------
iReference* bcMultipointQueue::cRWQueue::NotificationInnerReference(void)noexcept(true)
{
	return GetHost()->MultipointQueueInnerReference();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::cRWQueue::NotificationStarted(void)noexcept(true)
{
	auto Host=GetHost();
	iIncReference(Host,'queu');
	Host->fMultipointCallback->AccessStarted();
	Host->MultipointQueueStarted();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::cRWQueue::NotificationStopped(void)noexcept(true)
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
void bcMultipointQueue::cRWQueue::NotificationClosed(void)noexcept(true)
{
	auto Host=GetHost();
	Host->MultipointQueueClosed();
}
//---------------------------------------------------------------------------
bcMultipointQueue::cRWQueue::CycleState bcMultipointQueue::cRWQueue::NotificationCheckState(void)noexcept(true)
{
	auto Host=GetHost();
	return Host->MultipointQueueCheckState();
}
//---------------------------------------------------------------------------
bool bcMultipointQueue::cStateWriteProcedure::Procedure(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcMultipointQueue::fStateWriteProcedure);
	return Host->fRWQueue.RunNotifyCycle();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::cRWQueue::NotificationProcedure(void)noexcept(true)
{
	auto Host=GetHost();
	Host->MultipointQueueNotify();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueSetTerminated(void)noexcept(true)
{
	if(fMultipointQueueTerminated==false){
		fMultipointQueueTerminated=true;
		UpdateMultipointQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueSetReadBufferAvailable(bool AsyncNotify)noexcept(true)
{
	if(fMultipointQueueReadBufferAvailable==false){
		fMultipointQueueReadBufferAvailable=true;
		UpdateMultipointQueueState(AsyncNotify);
	}
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueSetWriteBufferAvailable(bool AsyncNotify)noexcept(true)
{
	if(fMultipointQueueWriteBufferAvailable==false){
		fMultipointQueueWriteBufferAvailable=true;
		UpdateMultipointQueueState(AsyncNotify);
	}
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueStarted(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueStopped(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueClosed(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueReadNotify(void)noexcept(true)
{
	fMultipointCallback->ReadNotify();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueWriteNotify(void)noexcept(true)
{
	fMultipointCallback->WriteNotify();
}
//---------------------------------------------------------------------------
bcNotificationCycle::CycleState bcMultipointQueue::MultipointQueueCheckState(void)noexcept(true)
{
	if(fMultipointQueueTerminated){
		return bcNotificationCycle::CycleState::Terminated;
	}
	return bcNotificationCycle::CycleState::Normal;
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueNotify(void)noexcept(true)
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
bool bcMultipointQueue::cNotifyReadProcedure::Procedure(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcMultipointQueue::fNotifyReadProcedure);
	Host->MultipointQueueNotifyRead();
	return false;
}
//---------------------------------------------------------------------------
void bcMultipointQueue::MultipointQueueNotifyRead(void)noexcept(true)
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
bool bcMultipointQueue::StartAccess(iReference *Reference,iMultipointQueueCallback *Callback)noexcept(true)
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
void bcMultipointQueue::StopAccess(void)noexcept(true)
{
	fRWQueue.StopNotify();
}
//---------------------------------------------------------------------------
bool bcMultipointQueue::IsClosed(void)noexcept(true)
{
	return fRWQueue.IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcMultipointQueue::NotifyRead(bool IdleNotify)noexcept(true)
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
void bcMultipointQueue::NotifyWrite(bool IdleNotify)noexcept(true)
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
cBufferedExtEndpoint::cBufferedExtEndpoint()noexcept(true)
{
	fReadQueueDataSize=0;
}
//---------------------------------------------------------------------------
cBufferedExtEndpoint::~cBufferedExtEndpoint()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::Close(void)noexcept(true)
{
	CloseReadQueue();
	CloseWriteQueue();
}
//---------------------------------------------------------------------------
iReadQueue* cBufferedExtEndpoint::GetReadQueue(void)noexcept(true)
{
	return this;
}
//---------------------------------------------------------------------------
iWriteQueue* cBufferedExtEndpoint::GetWriteQueue(void)noexcept(true)
{
	return this;
}
//---------------------------------------------------------------------------
uIntn cBufferedExtEndpoint::GetTotalBufferedReadData(void)const noexcept(true)
{
	return fReadQueueDataSize;
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::PutReadData(const void *Data,uIntn Size)noexcept(true)
{
	auto WriteMemory=fReadDataQueue.ReserveWriteBuffer(Size);
	cnMemory::Copy(WriteMemory.Pointer,Data,Size);
	fReadDataQueue.CommitWriteBuffer(Size);
	fReadQueueDataSize.Free+=Size;

	UpdateReadQueueState(false);
}
//---------------------------------------------------------------------------
cMemory cBufferedExtEndpoint::QueryReadDataBuffer(uIntn QuerySize)noexcept(true)
{
	return fReadDataQueue.ReserveWriteBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::AdvanceReadDataBuffer(uIntn Size)noexcept(true)
{
	return fReadDataQueue.CommitWriteBuffer(Size);
}
//---------------------------------------------------------------------------
cConstMemory cBufferedExtEndpoint::QueryWriteData(void)noexcept(true)
{
	return fWriteDataQueue.GatherReadBuffer(0);
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::AdvanceWriteData(uIntn Size)noexcept(true)
{
	fWriteDataQueue.DismissReadBuffer(Size);
	fReadQueueDataSize.Free-=Size;
}
//---------------------------------------------------------------------------
bool cBufferedExtEndpoint::IsWriteDataEnded(void)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
uIntn cBufferedExtEndpoint::GetMaxReadBufferSize(void)noexcept(true)
{
	return fReadDataQueue.GetBufferSizeLimit();
}
//---------------------------------------------------------------------------
cConstMemory cBufferedExtEndpoint::GatherReadBuffer(uIntn QuerySize)noexcept(true)
{
	return fReadDataQueue.GatherReadBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::DismissReadBuffer(uIntn Size)noexcept(true)
{
	return fReadDataQueue.DismissReadBuffer(Size);
}
//---------------------------------------------------------------------------
uIntn cBufferedExtEndpoint::GetMaxWriteBufferSize(void)noexcept(true)
{
	return fWriteDataQueue.GetBufferSizeLimit();
}
//---------------------------------------------------------------------------
cMemory cBufferedExtEndpoint::ReserveWriteBuffer(uIntn QuerySize)noexcept(true)
{
	return fWriteDataQueue.ReserveWriteBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::CommitWriteBuffer(uIntn Size)noexcept(true)
{
	return fWriteDataQueue.CommitWriteBuffer(Size);
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::ReadQueueStarted(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::ReadQueueStopped(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::ReadQueueClosed(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::WriteQueueStarted(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::WriteQueueStopped(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void cBufferedExtEndpoint::WriteQueueClosed(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
