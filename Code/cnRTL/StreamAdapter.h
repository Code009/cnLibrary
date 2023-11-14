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
eStreamError GetStreamError(iInterface *Stream);
//---------------------------------------------------------------------------
class cStreamFromEndpoint : public bcStream, public cDualReference, private iReadQueueCallback, private iWriteQueueCallback
{
public:
	cStreamFromEndpoint();
	~cStreamFromEndpoint();

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

	bool Connect(iEndpoint *Endpoint);
protected:
	void VirtualStopped(void);

	iPtr<iEndpoint> fEndpoint;

	// read task process

	virtual void StreamProcessReadTask(void)override;
	virtual void StreamProcessReadEnd(void)override;
	iPtr<cReadTask> fReadTask;

	// write task process

	virtual void StreamProcessWriteTask(void)override;
	virtual void StreamProcessWriteSetEnd(void)override;
	virtual void StreamProcessWriteEnd(void)override;
	rPtr<cWriteTask> fWriteTask;


	// Write Queue

	iWriteQueue *fWriteQueue=nullptr;
	
	virtual void cnLib_FUNC WriteStarted(void)override;
	virtual void cnLib_FUNC WriteStopped(void)override;
	virtual void cnLib_FUNC WriteNotify(void)override;

	// Read Queue

	iReadQueue *fReadQueue=nullptr;

	virtual void cnLib_FUNC ReadStarted(void)override;
	virtual void cnLib_FUNC ReadStopped(void)override;
	virtual void cnLib_FUNC ReadNotify(void)override;

	bool fReadActive=false;
	bool fWriteActive=false;
};
//---------------------------------------------------------------------------
class cPacketStreamFromEndpoint : public bcStream, public cDualReference, private iReadQueueCallback, private iWriteQueueCallback
{
public:
	cPacketStreamFromEndpoint();
	~cPacketStreamFromEndpoint();

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

	bool Connect(iEndpoint *Endpoint);
protected:
	void VirtualStopped(void);
	
	iPtr<iEndpoint> fEndpoint;

	// read task process

	virtual void StreamProcessReadTask(void)override;
	virtual void StreamProcessReadEnd(void)override;

	// write task process

	virtual void StreamProcessWriteTask(void)override;
	virtual void StreamProcessWriteSetEnd(void)override;
	virtual void StreamProcessWriteEnd(void)override;

	// Write Queue

	iWriteQueue *fWriteQueue=nullptr;
	
	virtual void cnLib_FUNC WriteStarted(void)override;
	virtual void cnLib_FUNC WriteStopped(void)override;
	virtual void cnLib_FUNC WriteNotify(void)override;

	// Read Queue

	iReadQueue *fReadQueue=nullptr;

	virtual void cnLib_FUNC ReadStarted(void)override;
	virtual void cnLib_FUNC ReadStopped(void)override;
	virtual void cnLib_FUNC ReadNotify(void)override;

	bool fReadActive=false;
	bool fWriteActive=false;
};
//---------------------------------------------------------------------------
iPtr<iStream> CreateStreamFromEndpoint(iEndpoint *Endpoint,bool KeepBoundary);
//---------------------------------------------------------------------------
class bcReadQueueFromStream : public bcReadQueue
{
public:
	bcReadQueueFromStream(iStream *Stream,uIntn PacketBufferSize);
	~bcReadQueueFromStream();

	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)override;
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)override;

protected:
	virtual void NotificationStarted(void)override;
	virtual void NotificationClosed(void)override;

private:
	iStream *fStream;

	cRingIndex<uInt8,3,1> fReadItemRing;
	cExclusiveFlag fProcessTaskExclusiveFlag;

	struct{
		iPtr<iStreamTask> Task;
		void *Buffer;
		uIntn SizeCompleted;
		bool Result;
	}fReadItem[2];
	uIntn fReadBufferSize;
	uIntn fCurReadOffset;

	void StartRead(void);
	class cTaskProcedure : public iProcedure
	{
	public:
		virtual void cnLib_FUNC Execute(void)override;
	public:
	}fTaskProcedure;
	void ProcessTask(void);

};
//---------------------------------------------------------------------------
class bcWriteQueueFromStream : public bcWriteQueue
{
public:
	bcWriteQueueFromStream(iStream *Stream);
	~bcWriteQueueFromStream();

	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn QuerySize)override;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)override;

protected:
	virtual void NotificationClosed(void)override;
private:
	iStream *fStream;

	cRingIndex<uInt8,2,1> fWriteItemRing;

	cExclusiveFlag fProcessTaskExclusiveFlag;

	struct cWriteItem{
		iPtr<iStreamTask> Task;
		void *SourceBuffer;
		uIntn SourceBufferSize;
	}fWriteItem[2];
	static constexpr uIntn SourceBufferTotalSize=65536;

	void SendWriteBuffer(cWriteItem &Item);

	class cTaskProcedure : public iProcedure
	{
	public:
		virtual void cnLib_FUNC Execute(void)override;
	public:
	}fTaskProcedure;
	void ProcessTask(void);
};
//---------------------------------------------------------------------------
class cEndpointFromStream : public iEndpoint, public iReference, public cDualReference
{
public:
	cEndpointFromStream(rPtr<iReference> StreamReference,iStream *Stream,uIntn ReadBufferSize=0x10000,iThreadPool *ThreadPool=nullptr);
	~cEndpointFromStream();

	virtual void cnLib_FUNC Close(void)override;
	virtual iReadQueue *cnLib_FUNC GetReadQueue(void)override;
	virtual iWriteQueue *cnLib_FUNC GetWriteQueue(void)override;
	virtual void cnLib_FUNC SetWriteEndMode(eEndpointWriteEndMode EndMode)override;
protected:
	void VirtualStopped(void);

	rPtr<iReference> fStreamReference;

	class cReadQueue : public bcReadQueueFromStream
	{
		cEndpointFromStream* GetHost(void);
	public:
		cReadQueue(iStream *Stream,uIntn PacketBufferSize,iThreadPool *ThreadPool);
		virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
		virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;

		using bcReadQueueFromStream::CloseQueue;

		using bcReadQueueFromStream::ReadQueueReportBufferAvailable;
		using bcReadQueueFromStream::ReadQueueReportEnded;
		using bcReadQueueFromStream::ReadQueueReportTerminated;
	protected:
		virtual iReference* NotificationInnerReference(void)override;
	}fReadQueue;

	class cWriteQueue : public bcWriteQueueFromStream
	{
		cEndpointFromStream* GetHost(void);
	public:
		cWriteQueue(iStream *Stream,iThreadPool *ThreadPool);
		virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
		virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;

		using bcWriteQueueFromStream::CloseQueue;
		
		using bcWriteQueueFromStream::WriteQueueReportBufferAvailable;
		using bcWriteQueueFromStream::WriteQueueReportTerminated;
		using bcWriteQueueFromStream::WriteQueueReportEnded;
		using bcWriteQueueFromStream::WriteQueueSetEndMode;
		
	protected:
		virtual iReference* NotificationInnerReference(void)override;
		virtual void NotificationClosed(void)override;
	}fWriteQueue;

	void WriteQueueTerminated(void);
};
//---------------------------------------------------------------------------
iPtr<iEndpoint> CreateEndpointFromSteam(iPtr<iStream> Stream,uIntn ReadBufferSize=0x10000,iThreadPool *ThreadPool=nullptr);
//iPtr<iEndpoint> CreateEndpointFromSteam(bcAggregable *StreamAggregable,iStream *Stream,uIntn ReadBufferSize,iThreadPool *ThreadPool);
//---------------------------------------------------------------------------
class cConnectionQueueFromListener : public bcConnectionQueue, public cDualReference
{
public:
	cConnectionQueueFromListener(iConnectionListener *Listener);
	~cConnectionQueueFromListener();

	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)override;
	virtual iPtr<iConnection> cnLib_FUNC FetchConnection(void)override;
protected:

	void VirtualStopped(void);

	virtual iReference* NotificationInnerReference(void)override;
	virtual void NotificationStarted(void)override;
	virtual void NotificationStopped(void)override;
	virtual void NotificationClosed(void)override;
	virtual CycleState NotificationCheckState(void)override;

private:
	iPtr<iConnectionListener> fListener;
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
		virtual void cnLib_FUNC Execute(void)override;
	}fAcceptTasks[TaskRingSize];

	void QueueTask(void);
	void TaskCompletionProcess(void);
};
//---------------------------------------------------------------------------
class cConnectionListenerFromQueue : public bcConnectionListener, protected bcConnectionQueueProcessor, public cDualReference
{
public:
	cConnectionListenerFromQueue(iConnectionQueue *ConnectionQueue);
	~cConnectionListenerFromQueue();

	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)override;
protected:
	void VirtualStarted(void);
	void VirtualStopped(void);

	virtual void ConnectionProcessTask(void)override;
	virtual void ConnectionProcessEnd(void)override;


	virtual void cnLib_FUNC AsyncNotify(void)override;

private:
	iPtr<iConnection> fCurrentConnection;
};
//---------------------------------------------------------------------------
class cMultipointQueueFromStream : public bcMultipointQueue, public cDualReference
{
public:
	cMultipointQueueFromStream(iMultipointStream *Stream);
	~cMultipointQueueFromStream();

	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)override;

	virtual cConstMemory cnLib_FUNC GatherReadBuffer(iPtr<iAddress> &Address)override;
	virtual void cnLib_FUNC DismissReadBuffer(bool Dismiss)override;
	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn Length)override;
	virtual bool cnLib_FUNC CommitWriteBuffer(iAddress *Address,uIntn Length)override;
protected:

	void VirtualStarted(void);
	void VirtualStopped(void);

	virtual iReference* MultipointQueueInnerReference(void)override;

	virtual void MultipointQueueStarted(void)override;
	//virtual void MultipointQueueStopped(void)override;
	//virtual void MultipointQueueClosed(void)override;
	//virtual void MultipointQueueReadNotify(void)override;
	//virtual void MultipointQueueWriteNotify(void)override;

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
		virtual bool Procedure(void)override;
	}fIOProcedure;
	rPtr<iAsyncProcedure> fIOWork;
	bool IOProcedure(void);

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

		virtual void cnLib_FUNC Execute(void)override;
	}fSendItem[fSendItemCount];

	uIntn fCurSendItemIndex;
	void SendItemSetNotify(cSendItem &Item);
	void SendItemCompletionNotify(cSendItem *Item);

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

		virtual void cnLib_FUNC Execute(void)override;
	}fReceiveItem[fReceiveItemCount];
	uIntn fCurReceiveItemIndex;
	void ReceiveItemSetNotify(cReceiveItem &Item);
	void ReceiveItemCompletionNotify(cReceiveItem *Item);
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
