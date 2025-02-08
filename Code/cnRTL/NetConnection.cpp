#include "NetConnection.h"
//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iAddress* cConnection::GetLocalAddress(void)noexcept(true)
{
	return LocalAddress;
}
//---------------------------------------------------------------------------
iAddress* cConnection::GetRemoteAddress(void)noexcept(true)
{
	return RemoteAddress;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcConnectionListener::bcConnectionListener()noexcept(true)
{
	fConnectionQueueCallback=nullptr;
}
//---------------------------------------------------------------------------
bcConnectionListener::~bcConnectionListener()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool bcConnectionListener::StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept(true)
{
	if(PrepareStartNotify()==false)
		return false;

	fConnectionQueueCallback=Callback;
	fConnectionQueueReference=Reference;

	ConfirmStartNotify();
	UpdateQueueState(false);
	return true;
}
//---------------------------------------------------------------------------
void bcConnectionListener::StopNotify(void)noexcept(true)
{
	if(bcAsyncQueue::StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcConnectionListener::NotifyCallback(bool IdleNotify)noexcept(true)
{
	return NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool bcConnectionListener::IsClosed(void)noexcept(true)
{
	return IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcConnectionListener::Close(void)noexcept(true)
{
	return CloseQueue();
}
//---------------------------------------------------------------------------
void bcConnectionListener::NotificationStarted(void)noexcept(true)
{
	bcAsyncQueue::NotificationStarted();

	rIncReference(this,'cnqu');
	fConnectionQueueCallback->AsyncStarted();
}
//---------------------------------------------------------------------------
void bcConnectionListener::NotificationStopped(void)noexcept(true)
{
	bcAsyncQueue::NotificationStopped();

	auto Reference=cnVar::MoveCast(fConnectionQueueReference);
	auto Callback=fConnectionQueueCallback;
	fConnectionQueueCallback=nullptr;
	Callback->AsyncStopped();
	rDecReference(this,'cnqu');
}
//---------------------------------------------------------------------------
void bcConnectionListener::AsyncQueueNotify(void)noexcept(true)
{
	fConnectionQueueCallback->AsyncNotify();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcBufferedRWQueue::bcBufferedRWQueue(uIntn BufferSize)noexcept(true)
	: fReadDataQueue(BufferSize)
	, fWriteDataQueue(BufferSize)
{
}
//---------------------------------------------------------------------------
bcBufferedRWQueue::~bcBufferedRWQueue()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool bcBufferedRWQueue::PutReadData(const void *Data,uIntn Size)noexcept(true)
{
	if(Size==0)
		return true;
	auto WriteMemory=fReadDataQueue.ReserveWriteBuffer(Size);
	if(Size>WriteMemory.Length){
		fReadDataQueue.CommitWriteBuffer(0);
		return false;
	}
	cnMemory::Copy(WriteMemory.Pointer,Data,Size);
	fReadDataQueue.CommitWriteBuffer(Size);

	ReadQueueReportBufferAvailable(false);
	return true;
}
//---------------------------------------------------------------------------
uIntn bcBufferedRWQueue::PutReadDataPart(const void *Data,uIntn Size)noexcept(true)
{
	if(Size==0)
		return 0;
	auto WriteMemory=fReadDataQueue.ReserveWriteBuffer(Size);
	if(Size>WriteMemory.Length){
		Size=WriteMemory.Length;
	}
	cnMemory::Copy(WriteMemory.Pointer,Data,Size);
	fReadDataQueue.CommitWriteBuffer(Size);

	ReadQueueReportBufferAvailable(false);
	return Size;
}
//---------------------------------------------------------------------------
cMemory bcBufferedRWQueue::QueryReadDataBuffer(uIntn QuerySize)noexcept(true)
{
	return fReadDataQueue.ReserveWriteBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::AdvanceReadDataBuffer(uIntn Size)noexcept(true)
{
	fReadDataQueue.CommitWriteBuffer(Size);
	if(Size!=0){
		ReadQueueReportBufferAvailable(false);
	}
}
//---------------------------------------------------------------------------
uIntn bcBufferedRWQueue::GetWriteData(void *Data,uIntn Size)noexcept(true)
{
	auto ReadMemory=fWriteDataQueue.GatherReadBuffer(Size);
	if(ReadMemory.Length==0){
		return 0;
	}
	if(ReadMemory.Length>Size){
		ReadMemory.Length=Size;
	}
	cnMemory::Copy(Data,ReadMemory.Pointer,ReadMemory.Length);
	fWriteDataQueue.DismissReadBuffer(ReadMemory.Length);

	WriteQueueReportBufferAvailable(false);

	return ReadMemory.Length;
}
//---------------------------------------------------------------------------
cConstMemory bcBufferedRWQueue::QueryWriteData(void)noexcept(true)
{
	return fWriteDataQueue.GatherReadBuffer(0);
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::AdvanceWriteData(uIntn Size)noexcept(true)
{
	fWriteDataQueue.DismissReadBuffer(Size);
	if(Size!=0){
		WriteQueueReportBufferAvailable(false);
	}
}
//---------------------------------------------------------------------------
bool bcBufferedRWQueue::IsWriteDataEnded(void)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
uIntn bcBufferedRWQueue::GetMaxReadBufferSize(void)noexcept(true)
{
	return fReadDataQueue.GetBufferSizeLimit();
}
//---------------------------------------------------------------------------
cConstMemory bcBufferedRWQueue::GatherReadBuffer(uIntn QuerySize)noexcept(true)
{
	return fReadDataQueue.GatherReadBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::DismissReadBuffer(uIntn Size)noexcept(true)
{
	fReadDataQueue.DismissReadBuffer(Size);
	if(Size!=0){
		ReadBufferNotify();
	}
}
//---------------------------------------------------------------------------
uIntn bcBufferedRWQueue::GetMaxWriteBufferSize(void)noexcept(true)
{
	return fWriteDataQueue.GetBufferSizeLimit();
}
//---------------------------------------------------------------------------
cMemory bcBufferedRWQueue::ReserveWriteBuffer(uIntn QuerySize)noexcept(true)
{
	if(ReadQueueIsClosed()){
		return NullArray;
	}
	return fWriteDataQueue.ReserveWriteBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::CommitWriteBuffer(uIntn Size)noexcept(true)
{
	fWriteDataQueue.CommitWriteBuffer(Size);
	if(Size==0){
		WriteDataNotify();
	}
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::ReadQueueClosed(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::WriteQueueClosed(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
