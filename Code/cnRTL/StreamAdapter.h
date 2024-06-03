/*- cnRTL - Stream Adapter ------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-10-15                                          */
/*-------------------------------------------------------------------------*/
#ifndef	__cnLibrary_cnRTL_StreamAdapter_H__
#define	__cnLibrary_cnRTL_StreamAdapter_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/StreamBuffer.h>
#include <cnRTL/Stream.h>
#include <cnRTL/ObjectReference.h>
#include <cnRTL/NetConnection.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnRTL_CONSTVAR	constexpr
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnRTL_CONSTVAR	const
#endif // cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
eStreamError GetStreamError(iInterface *Stream)noexcept(true);
//---------------------------------------------------------------------------
class cStreamFromEndpoint : public bcStream, public cDualReference, private iReadQueueCallback, private iWriteQueueCallback
{
public:
	cStreamFromEndpoint()noexcept(true);
	~cStreamFromEndpoint()noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

	bool Connect(iEndpoint *Endpoint)noexcept(true);
protected:
	void VirtualStopped(void)noexcept(true);

	iPtr<iEndpoint> fEndpoint;

	// read task process

	virtual void StreamProcessReadTask(void)noexcept(true)override;
	virtual void StreamProcessReadEnd(void)noexcept(true)override;
	iPtr<cReadTask> fReadTask;

	// write task process

	virtual void StreamProcessWriteTask(void)noexcept(true)override;
	virtual void StreamProcessWriteSetEnd(void)noexcept(true)override;
	virtual void StreamProcessWriteEnd(void)noexcept(true)override;
	rPtr<cWriteTask> fWriteTask;


	// Write Queue

	iWriteQueue *fWriteQueue=nullptr;
	
	virtual void cnLib_FUNC WriteStarted(void)noexcept(true)override;
	virtual void cnLib_FUNC WriteStopped(void)noexcept(true)override;
	virtual void cnLib_FUNC WriteNotify(void)noexcept(true)override;

	// Read Queue

	iReadQueue *fReadQueue=nullptr;

	virtual void cnLib_FUNC ReadStarted(void)noexcept(true)override;
	virtual void cnLib_FUNC ReadStopped(void)noexcept(true)override;
	virtual void cnLib_FUNC ReadNotify(void)noexcept(true)override;

	bool fReadActive=false;
	bool fWriteActive=false;
};
//---------------------------------------------------------------------------
class cPacketStreamFromEndpoint : public bcStream, public cDualReference, private iReadQueueCallback, private iWriteQueueCallback
{
public:
	cPacketStreamFromEndpoint()noexcept(true);
	~cPacketStreamFromEndpoint()noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

	bool Connect(iEndpoint *Endpoint)noexcept(true);
protected:
	void VirtualStopped(void)noexcept(true);
	
	iPtr<iEndpoint> fEndpoint;

	// read task process

	virtual void StreamProcessReadTask(void)noexcept(true)override;
	virtual void StreamProcessReadEnd(void)noexcept(true)override;

	// write task process

	virtual void StreamProcessWriteTask(void)noexcept(true)override;
	virtual void StreamProcessWriteSetEnd(void)noexcept(true)override;
	virtual void StreamProcessWriteEnd(void)noexcept(true)override;

	// Write Queue

	iWriteQueue *fWriteQueue=nullptr;
	
	virtual void cnLib_FUNC WriteStarted(void)noexcept(true)override;
	virtual void cnLib_FUNC WriteStopped(void)noexcept(true)override;
	virtual void cnLib_FUNC WriteNotify(void)noexcept(true)override;

	// Read Queue

	iReadQueue *fReadQueue=nullptr;

	virtual void cnLib_FUNC ReadStarted(void)noexcept(true)override;
	virtual void cnLib_FUNC ReadStopped(void)noexcept(true)override;
	virtual void cnLib_FUNC ReadNotify(void)noexcept(true)override;

	bool fReadActive=false;
	bool fWriteActive=false;
};
//---------------------------------------------------------------------------
iPtr<iStream> CreateStreamFromEndpoint(iEndpoint *Endpoint,bool KeepBoundary)noexcept(true);
//---------------------------------------------------------------------------
class bcReadQueueFromStream : public bcReadQueue
{
public:
	bcReadQueueFromStream(iStream *Stream,uIntn PacketBufferSize)noexcept(true);
	~bcReadQueueFromStream()noexcept(true);

	virtual uIntn cnLib_FUNC GetMaxReadBufferSize(void)noexcept(true)override;
	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)noexcept(true)override;
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)noexcept(true)override;

protected:
	virtual void NotificationStarted(void)noexcept(true)override;
	virtual void NotificationClosed(void)noexcept(true)override;

private:
	iStream *fStream;

	cRingIndex<ufInt8,3,1> fReadItemRing;
	cExclusiveFlag fProcessTaskExclusiveFlag;

	struct{
		iPtr<iStreamTask> Task;
		void *Buffer;
		uIntn ValidSize;
		uIntn ValidOffset;
	}fReadItem[2];
	uIntn fReadBufferSize;

	void OnStartRead(void)noexcept(true);
	class cTaskProcedure : public iProcedure
	{
	public:
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	public:
	}fTaskProcedure;
	void ProcessTask(void)noexcept(true);

};
//---------------------------------------------------------------------------
class bcWriteQueueFromStream : public bcWriteQueue
{
public:
	bcWriteQueueFromStream(iStream *Stream)noexcept(true);
	~bcWriteQueueFromStream()noexcept(true);

	uIntn WriteBufferSizeLimit=65536;

	virtual uIntn cnLib_FUNC GetMaxWriteBufferSize(void)noexcept(true)override;
	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn QuerySize)noexcept(true)override;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)noexcept(true)override;

protected:
	virtual void NotificationClosed(void)noexcept(true)override;
private:
	iStream *fStream;

	cRingIndex<uInt8,2,1> fWriteItemRing;

	cExclusiveFlag fProcessTaskExclusiveFlag;

	struct cWriteItem{
		iPtr<iStreamTask> Task;
		cMemoryBlock SourceBuffer;
		uIntn SourceBufferSize=0;
	}fWriteItem[2];

	void SendWriteBuffer(cWriteItem &Item)noexcept(true);

	class cTaskProcedure : public iProcedure
	{
	public:
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	public:
	}fTaskProcedure;
	void ProcessTask(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cEndpointFromStream : public iEndpoint, public iReference, public cDualReference
{
public:
	cEndpointFromStream(rPtr<iReference> StreamReference,iStream *Stream,uIntn ReadBufferSize=0x10000,iThreadPool *ThreadPool=nullptr)noexcept(true);
	~cEndpointFromStream()noexcept(true);

	virtual void cnLib_FUNC Close(void)noexcept(true)override;
	virtual iReadQueue *cnLib_FUNC GetReadQueue(void)noexcept(true)override;
	virtual iWriteQueue *cnLib_FUNC GetWriteQueue(void)noexcept(true)override;
protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	rPtr<iReference> fStreamReference;

	class cReadQueue : public bcReadQueueFromStream
	{
		cEndpointFromStream* GetHost(void)noexcept(true);
	public:
		cReadQueue(iStream *Stream,uIntn PacketBufferSize,iThreadPool *ThreadPool)noexcept(true);
		virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
		virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;

		using bcReadQueueFromStream::CloseQueue;

		using bcReadQueueFromStream::ReadQueueReportBufferAvailable;
		using bcReadQueueFromStream::ReadQueueReportEnded;
		using bcReadQueueFromStream::ReadQueueReportTerminated;
	protected:
		virtual iReference* NotificationInnerReference(void)noexcept(true)override;
	}fReadQueue;

	class cWriteQueue : public bcWriteQueueFromStream
	{
		cEndpointFromStream* GetHost(void)noexcept(true);
	public:
		cWriteQueue(iStream *Stream,iThreadPool *ThreadPool)noexcept(true);
		virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
		virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;

		using bcWriteQueueFromStream::CloseQueue;
		
		using bcWriteQueueFromStream::WriteQueueReportBufferAvailable;
		using bcWriteQueueFromStream::WriteQueueReportTerminated;
		
	protected:
		virtual iReference* NotificationInnerReference(void)noexcept(true)override;
		virtual void NotificationClosed(void)noexcept(true)override;
	}fWriteQueue;

	void WriteQueueTerminated(void)noexcept(true);
};
//---------------------------------------------------------------------------
iPtr<iEndpoint> CreateEndpointFromSteam(iPtr<iStream> Stream,uIntn ReadBufferSize=0x10000,iThreadPool *ThreadPool=nullptr)noexcept(true);
//iPtr<iEndpoint> CreateEndpointFromSteam(bcAggregable *StreamAggregable,iStream *Stream,uIntn ReadBufferSize,iThreadPool *ThreadPool)noexcept(true);
//---------------------------------------------------------------------------
class cConnectionListenerFromAsyncListener : public bcConnectionListener, public cDualReference
{
public:

	class cnLib_INTERFACE iAsyncListener : public iReference
	{
	public:
		virtual iAddress* GetLocalAddress(void)noexcept(true)=0;
		virtual iPtr<iConnectionTask> AcceptAsync(void)noexcept(true)=0;
	};
	cConnectionListenerFromAsyncListener(iAsyncListener *Listener)noexcept(true);
	~cConnectionListenerFromAsyncListener()noexcept(true);

	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)noexcept(true)override;
	virtual iPtr<iConnection> cnLib_FUNC FetchConnection(void)noexcept(true)override;
protected:

	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	virtual iReference* NotificationInnerReference(void)noexcept(true)override;
	virtual void NotificationStarted(void)noexcept(true)override;
	virtual void NotificationStopped(void)noexcept(true)override;
	virtual void NotificationClosed(void)noexcept(true)override;
	virtual CycleState NotificationCheckState(void)noexcept(true)override;

private:
	rPtr<iAsyncListener> fListener;
	static constexpr uIntn TaskRingSizeBit=2;
	static constexpr uIntn TaskRingSize=1<<TaskRingSizeBit;

	cExclusiveFlag fTaskQueueExclusiveFlag;
	cExclusiveFlag fTaskCompletionExclusiveFlag;
	cRingIndex<ufInt8,3,TaskRingSizeBit> fTaskRingIndex;

	bool fQueueTerminated;

	class cTaskItem : public iProcedure
	{
	public:
		iPtr<iConnectionTask> AcceptTask;
		uIntn TaskIndex;
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fAcceptTasks[TaskRingSize];

	void QueueTask(void)noexcept(true);
	void TaskCompletionProcess(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cMultipointQueueFromStream : public bcMultipointQueue, public cDualReference
{
public:
	cMultipointQueueFromStream(iMultipointStream *Stream)noexcept(true);
	~cMultipointQueueFromStream()noexcept(true);

	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)noexcept(true)override;

	virtual cConstMemory cnLib_FUNC GatherReadBuffer(iPtr<iAddress> &Address)noexcept(true)override;
	virtual void cnLib_FUNC DismissReadBuffer(bool Dismiss)noexcept(true)override;
	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn Length)noexcept(true)override;
	virtual bool cnLib_FUNC CommitWriteBuffer(iAddress *Address,uIntn Length)noexcept(true)override;
protected:

	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	virtual iReference* MultipointQueueInnerReference(void)noexcept(true)override;

	virtual void MultipointQueueStarted(void)noexcept(true)override;
	//virtual void MultipointQueueStopped(void)noexcept(true)override;
	//virtual void MultipointQueueClosed(void)noexcept(true)override;
	//virtual void MultipointQueueReadNotify(void)noexcept(true)override;
	//virtual void MultipointQueueWriteNotify(void)noexcept(true)override;

private:
// datagram stream
	iPtr<iMultipointStream> fStream;
	uIntn fPacketSizeMax;


	//uInt8 fCloseState;
	//void CloseQueue(void);


	//bool fQueueCallbackStopped;

	//bool fReadIdleNotify;
	//bool fWriteIdleNotify;

	static constexpr uIntn SendBufferSize=65535;

// datagram IO

	class cIOProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)noexcept(true)override;
	}fIOProcedure;
	rPtr<iAsyncProcedure> fIOWork;
	bool IOProcedure(void)noexcept(true);

// datagram send

	static cnRTL_CONSTVAR uIntn fSendItemCount=2;
	struct cSendItem : public iProcedure
	{
		cMultipointQueueFromStream *Owner;
		iPtr<iMultipointTask> Task;
		void *WriteBuffer;

		enum{
			sIdle,sSend,sCompleted
		}State;

		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fSendItem[fSendItemCount];

	uIntn fCurSendItemIndex;
	void SendItemSetNotify(cSendItem &Item)noexcept(true);
	void SendItemCompletionNotify(cSendItem *Item)noexcept(true);

// datagram receive

	static cnRTL_CONSTVAR uIntn fReceiveItemCount=2;

	struct cReceiveItem : public iProcedure
	{
		cMultipointQueueFromStream *Owner;
		iPtr<iMultipointTask> Task;
		void *ReadBuffer;

		enum{
			sIdle,sRecv,sPush
		}State;

		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fReceiveItem[fReceiveItemCount];
	uIntn fCurReceiveItemIndex;
	void ReceiveItemSetNotify(cReceiveItem &Item)noexcept(true);
	void ReceiveItemCompletionNotify(cReceiveItem *Item)noexcept(true);
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#undef	cnRTL_CONSTVAR
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
