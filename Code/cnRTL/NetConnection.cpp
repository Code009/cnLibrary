#include "NetConnection.h"
//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iAddress* cConnection::GetLocalAddress(void)noexcept
{
	return LocalAddress;
}
//---------------------------------------------------------------------------
iAddress* cConnection::GetRemoteAddress(void)noexcept
{
	return RemoteAddress;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcConnectionListener::bcConnectionListener()noexcept
{
	fConnectionQueueCallback=nullptr;
}
//---------------------------------------------------------------------------
bcConnectionListener::~bcConnectionListener()noexcept
{
}
//---------------------------------------------------------------------------
bool bcConnectionListener::StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept
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
void bcConnectionListener::StopNotify(void)noexcept
{
	if(bcAsyncQueue::StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcConnectionListener::NotifyCallback(bool IdleNotify)noexcept
{
	return NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool bcConnectionListener::IsClosed(void)noexcept
{
	return IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcConnectionListener::Close(void)noexcept
{
	return CloseQueue();
}
//---------------------------------------------------------------------------
void bcConnectionListener::NotificationStarted(void)noexcept
{
	bcAsyncQueue::NotificationStarted();

	rIncReference(this,'cnqu');
	fConnectionQueueCallback->AsyncStarted();
}
//---------------------------------------------------------------------------
void bcConnectionListener::NotificationStopped(void)noexcept
{
	bcAsyncQueue::NotificationStopped();

	auto Reference=cnVar::MoveCast(fConnectionQueueReference);
	auto Callback=fConnectionQueueCallback;
	fConnectionQueueCallback=nullptr;
	Callback->AsyncStopped();
	rDecReference(this,'cnqu');
}
//---------------------------------------------------------------------------
void bcConnectionListener::AsyncQueueNotify(void)noexcept
{
	fConnectionQueueCallback->AsyncNotify();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcBufferedRWQueue::bcBufferedRWQueue()noexcept
{
}
//---------------------------------------------------------------------------
bcBufferedRWQueue::~bcBufferedRWQueue()noexcept
{
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::PutReadData(const void *Data,uIntn Size)noexcept
{
	auto WriteMemory=fReadDataQueue.ReserveWriteBuffer(Size);
	cnMemory::Copy(WriteMemory.Pointer,Data,Size);
	fReadDataQueue.CommitWriteBuffer(Size);

	ReadQueueReportBufferAvailable(false);
}
//---------------------------------------------------------------------------
cMemory bcBufferedRWQueue::QueryReadDataBuffer(uIntn QuerySize)noexcept
{
	return fReadDataQueue.ReserveWriteBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::AdvanceReadDataBuffer(uIntn Size)noexcept
{
	return fReadDataQueue.CommitWriteBuffer(Size);
}
//---------------------------------------------------------------------------
uIntn bcBufferedRWQueue::GetWriteData(void *Data,uIntn Size)noexcept
{
	auto ReadMemory=fWriteDataQueue.GatherReadBuffer(Size);
	if(ReadMemory.Length>Size){
		ReadMemory.Length=Size;
	}
	cnMemory::Copy(Data,ReadMemory.Pointer,ReadMemory.Length);
	fWriteDataQueue.DismissReadBuffer(ReadMemory.Length);

	WriteQueueReportBufferAvailable(false);

	return ReadMemory.Length;
}
//---------------------------------------------------------------------------
cConstMemory bcBufferedRWQueue::QueryWriteData(void)noexcept
{
	return fWriteDataQueue.GatherReadBuffer(0);
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::AdvanceWriteData(uIntn Size)noexcept
{
	return fWriteDataQueue.DismissReadBuffer(Size);
}
//---------------------------------------------------------------------------
bool bcBufferedRWQueue::IsWriteDataEnded(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
uIntn bcBufferedRWQueue::GetMaxReadBufferSize(void)noexcept
{
	return fReadDataQueue.BufferSizeLimit;
}
//---------------------------------------------------------------------------
cConstMemory bcBufferedRWQueue::GatherReadBuffer(uIntn QuerySize)noexcept
{
	return fReadDataQueue.GatherReadBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::DismissReadBuffer(uIntn Size)noexcept
{
	fReadDataQueue.DismissReadBuffer(Size);
	if(Size!=0){
		ReadBufferNotify();
	}
}
//---------------------------------------------------------------------------
uIntn bcBufferedRWQueue::GetMaxWriteBufferSize(void)noexcept
{
	return fWriteDataQueue.BufferSizeLimit;
}
//---------------------------------------------------------------------------
cMemory bcBufferedRWQueue::ReserveWriteBuffer(uIntn QuerySize)noexcept
{
	if(ReadQueueIsClosed()){
		return NullArray;
	}
	return fWriteDataQueue.ReserveWriteBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::CommitWriteBuffer(uIntn Size)noexcept
{
	fWriteDataQueue.CommitWriteBuffer(Size);
	if(Size!=0){
		WriteDataNotify();
	}
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::ReadQueueClosed(void)noexcept
{
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::WriteQueueClosed(void)noexcept
{
}
//---------------------------------------------------------------------------
