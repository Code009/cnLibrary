/*- cnRTL - Net Connection Base Classes -----------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-09-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef	__cnLibrary_cnRTL_NetConnection_H__
#define	__cnLibrary_cnRTL_NetConnection_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/ThreadAsync.h>
#include <cnRTL/AsyncTask.h>
#include <cnRTL/Stream.h>
#include <cnRTL/ThreadSynchronization.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
class cConnection : public iConnection
{
public:
	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)noexcept(true)override;
	virtual iAddress*	cnLib_FUNC GetRemoteAddress(void)noexcept(true)override;

	iPtr<iAddress> LocalAddress;
	iPtr<iAddress> RemoteAddress;
};
//---------------------------------------------------------------------------
class bcConnectionListener : public iConnectionListener, protected bcAsyncQueue
{
public:
	bcConnectionListener()noexcept(true);
	~bcConnectionListener()noexcept(true);

	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept(true)override;
	virtual void cnLib_FUNC StopNotify(void)noexcept(true)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)noexcept(true)override;
	virtual bool cnLib_FUNC IsClosed(void)noexcept(true)override;

	virtual void cnLib_FUNC Close(void)noexcept(true)override;

protected:
	iAsyncNotificationCallback *fConnectionQueueCallback;
	rPtr<iReference> fConnectionQueueReference;

	virtual void NotificationStarted(void)noexcept(true)override;
	virtual void NotificationStopped(void)noexcept(true)override;
	virtual void AsyncQueueNotify(void)noexcept(true)override;
};
//---------------------------------------------------------------------------

class bcBufferedRWQueue : public bcRWQueue
{
public:
	bcBufferedRWQueue(uIntn BufferSize=4096)noexcept(true);
	~bcBufferedRWQueue()noexcept(true);

	bool PutReadData(const void *Data,uIntn Size)noexcept(true);
	uIntn PutReadDataPart(const void *Data,uIntn Size)noexcept(true);
	cMemory QueryReadDataBuffer(uIntn QuerySize)noexcept(true);
	void AdvanceReadDataBuffer(uIntn Size)noexcept(true);

	uIntn GetWriteData(void *Data,uIntn Size)noexcept(true);
	cConstMemory QueryWriteData(void)noexcept(true);
	void AdvanceWriteData(uIntn Size)noexcept(true);
	bool IsWriteDataEnded(void)noexcept(true);

protected:

	virtual uIntn cnLib_FUNC GetMaxReadBufferSize(void)noexcept(true)override;
	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)noexcept(true)override;
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)noexcept(true)override;
	virtual void ReadQueueClosed(void)noexcept(true)override;

	virtual uIntn cnLib_FUNC GetMaxWriteBufferSize(void)noexcept(true)override;
	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn QuerySize)noexcept(true)override;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)noexcept(true)override;
	virtual void WriteQueueClosed(void)noexcept(true)override;

	virtual void ReadBufferNotify(void)noexcept(true)=0;
	virtual void WriteDataNotify(void)noexcept(true)=0;

private:
	cAsyncLoopbackStreamBuffer fReadDataQueue;
	cAsyncLoopbackStreamBuffer fWriteDataQueue;
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
