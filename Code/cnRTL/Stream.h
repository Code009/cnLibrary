/*- cnRTL - Stream Base Classes -------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2010-03-24                                          */
/*-------------------------------------------------------------------------*/
#ifndef	__cnLibrary_cnRTL_Stream_H__
#define	__cnLibrary_cnRTL_Stream_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/ThreadAsync.h>
#include <cnRTL/AsyncTask.h>
#include <cnRTL/StreamBuffer.h>
#include <cnRTL/InterfaceImplementation.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
class bcReadQueue : public iReadQueue, protected bcAsyncQueue
{
public:
	bcReadQueue();
	~bcReadQueue();

	virtual bool cnLib_FUNC StartRead(iReference *Reference,iReadQueueCallback *Callback)override;
	virtual void cnLib_FUNC StopRead(void)override;
	virtual void cnLib_FUNC NotifyRead(uIntn SizeToNotify)override;
	virtual bool cnLib_FUNC IsReadClosed(bool &GracefulClose)override;
protected:
	iReadQueueCallback *fReadCallback;
	rPtr<iReference> fReadCallbackReference;

	virtual void NotificationStarted(void)override;
	virtual void NotificationStopped(void)override;
	virtual CycleState NotificationCheckState(void)override;
	virtual void AsyncQueueNotify(void)override;

	void ReadQueueReportBufferAvailable(bool AsyncNotify);
	void ReadQueueReportBufferEmpty(void);
	void ReadQueueReportEnded(void);
	void ReadQueueReportTerminated(void);
private:
	bool fReadQueueBufferAvailable;
	bool fReadQueueEnded;
	bool fReadQueueTerminated;
};
//---------------------------------------------------------------------------
class bcWriteQueue : public iWriteQueue, protected bcAsyncQueue
{
public:
	bcWriteQueue();
	~bcWriteQueue();

	virtual bool cnLib_FUNC StartWrite(iReference *Reference,iWriteQueueCallback *Callback)override;
	virtual void cnLib_FUNC StopWrite(bool Terminate)override;
	virtual void cnLib_FUNC NotifyWrite(uIntn SizeToNotify)override;
	virtual bool cnLib_FUNC IsWriteClosed(bool &GracefulClose)override;
protected:
	iWriteQueueCallback *fWriteCallback;
	rPtr<iReference> fWriteCallbackReference;

	virtual void NotificationStarted(void)override;
	virtual void NotificationStopped(void)override;
	virtual CycleState NotificationCheckState(void)override;
	virtual void AsyncQueueNotify(void)override;

	void WriteQueueReportBufferAvailable(bool AsyncNotify);
	void WriteQueueReportEnded(void);
	void WriteQueueReportTerminated(void);
	void WriteQueueSetEndMode(eEndpointWriteEndMode EndMode);

private:

	bool fWriteQueueTerminated;
	eEndpointWriteEndMode fWriteQueueEndMode;
};
//---------------------------------------------------------------------------
class bcRWQueue : public iReadQueue, public iWriteQueue
{
public:
	typedef iReadQueue tReferenceInterface;
	typedef bcAsyncQueue::CycleState CycleState;

	bcRWQueue();
	~bcRWQueue();

	void CloseReadQueue(void);
	void CloseWriteQueue(void);

	bool ReadQueueIsStarted(void)const;
	bool ReadQueueIsClosed(void)const;
	bool ReadQueueIsEnded(void)const;

	bool WriteQueueIsStarted(void)const;
	bool WriteQueueIsClosed(void)const;
	bool WriteQueueIsEnded(void)const;

	virtual bool cnLib_FUNC StartRead(iReference *Reference,iReadQueueCallback *Callback)override;
	virtual void cnLib_FUNC StopRead(void)override;
	virtual void cnLib_FUNC NotifyRead(uIntn SizeToNotify)override;
	virtual bool cnLib_FUNC IsReadClosed(bool &GracefulClose)override;


	virtual bool cnLib_FUNC StartWrite(iReference *Reference,iWriteQueueCallback *Callback)override;
	virtual void cnLib_FUNC StopWrite(bool Terminate)override;
	virtual void cnLib_FUNC NotifyWrite(uIntn SizeToNotify)override;
	virtual bool cnLib_FUNC IsWriteClosed(bool &GracefulClose)override;
	
protected:

	iReadQueueCallback *fReadCallback;
	iWriteQueueCallback *fWriteCallback;
	rPtr<iReference> fReadCallbackReference;
	rPtr<iReference> fWriteCallbackReference;

	void CreateReadQueueWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop);
	void UpdateReadQueueState(bool AsyncNotify);

	void CreateWriteQueueWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop);
	void UpdateWriteQueueState(bool AsyncNotify);

	virtual iReference* RWQueueInnerReference(void)=0;

	virtual void ReadQueueStarted(void);
	virtual void ReadQueueStopped(void);
	virtual void ReadQueueClosed(void);
	virtual void ReadQueueNotify(void);

	virtual void WriteQueueStarted(void);
	virtual void WriteQueueStopped(void);
	virtual void WriteQueueClosed(void);
	virtual void WriteQueueNotify(void);

	void ReadQueueReportBufferAvailable(bool AsyncNotify);
	void ReadQueueReportBufferEmpty(void);
	void ReadQueueReportEnded(void);
	void ReadQueueReportTerminated(void);

	void WriteQueueReportBufferAvailable(bool AsyncNotify);
	void WriteQueueReportEnded(void);
	void WriteQueueReportTerminated(void);
	void WriteQueueSetEndMode(eEndpointWriteEndMode EndMode);

private:
	bool fReadQueueBufferAvailable;
	bool fReadQueueEnded;
	bool fReadQueueTerminated;

	bool fWriteQueueTerminated;
	eEndpointWriteEndMode fWriteQueueEndMode;

	CycleState ReadQueueCheckState(void);
	CycleState WriteQueueCheckState(void);
	class cReadQueue : public bcAsyncQueue
	{
	public:
		bcRWQueue* GetHost(void);
		virtual iReference* NotificationInnerReference(void)override;
		virtual void NotificationStarted(void)override;
		virtual void NotificationStopped(void)override;
		virtual void NotificationClosed(void)override;
		virtual CycleState NotificationCheckState(void)override;
		virtual void AsyncQueueNotify(void)override;

		using bcAsyncQueue::CreateQueueWork;
		using bcAsyncQueue::UpdateQueueState;
		using bcAsyncQueue::AsyncQueueSetAvailable;
	}fReadAsyncQueue;
	class cWriteQueue : public bcAsyncQueue
	{
	public:
		bcRWQueue* GetHost(void);
		virtual iReference* NotificationInnerReference(void)override;
		virtual void NotificationStarted(void)override;
		virtual void NotificationStopped(void)override;
		virtual void NotificationClosed(void)override;
		virtual CycleState NotificationCheckState(void)override;
		virtual void AsyncQueueNotify(void)override;

		using bcAsyncQueue::CreateQueueWork;
		using bcAsyncQueue::UpdateQueueState;
		using bcAsyncQueue::AsyncQueueSetAvailable;
	}fWriteAsyncQueue;
};
//---------------------------------------------------------------------------
class bcStream : public iStream, public iStreamErrorReport
{
public:
	bcStream();
	~bcStream();

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

	virtual void cnLib_FUNC Close(void)override;
	virtual bool cnLib_FUNC IsEndOfReading(void)override;
	virtual bool cnLib_FUNC SetEndOfStream(void)override;
	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)override;
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)override;
	virtual iPtr<iStreamTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size)override;
	virtual iPtr<iStreamTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size)override;

	virtual eStreamError cnLib_FUNC GetStreamError(void)override;

protected:
	
protected:
	void StreamClose(void);

	virtual void StreamProcessReadTask(void)=0;
	virtual void StreamProcessReadEnd(void)=0;
	virtual void StreamProcessWriteTask(void)=0;
	virtual void StreamProcessWriteSetEnd(void)=0;
	virtual void StreamProcessWriteEnd(void)=0;

	void UpdateReadTaskQueue(void);
	void UpdateWriteTaskQueue(void);

	void SetReadEnd(bool GracefulClose);
	void SetWriteEnd(bool GracefulClose);

	// Stream Task

	class cAccessTask : public iStreamTask, public iStreamErrorReport , public cTaskQueue::cTask
	{
	public:
		cAccessTask()=default;
		~cAccessTask()=default;
		iPtr<bcStream> Owner;

		virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;
		virtual bool cnLib_FUNC IsDone(void)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;
		virtual bool cnLib_FUNC GetResult(uIntn &SizeCompleted)override;

		eStreamError cnLib_FUNC GetStreamError(void)override;
		StreamError AccessErrorCode;

		// TotalSizeCompleted
		//	size of buffer completed
		uIntn TotalSizeCompleted;
	};

	class cReadTask : public cAccessTask, public iWriteBuffer<void>
	{
	public:
		void Setup(void *Buffer,uIntn Size);
		virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn QuerySize)override;
		virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)override;
	
		virtual void cnLib_FUNC Cancel(void)override;

		bool IsAccessBufferCompleted(void)const;

	protected:
		// BufferList
		//	buffer list
		void *fAccessBuffer;
		uIntn fAccessBufferSize;
	};

	class cWriteTask : public cAccessTask, public iReadBuffer<void>
	{
	public:
		void Setup(const void *Buffer,uIntn Size);
		virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)override;
		virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)override;
	
		virtual void cnLib_FUNC Cancel(void)override;
		
		bool IsAccessBufferCompleted(void)const;

	protected:
		// BufferList
		//	buffer list
		const void *fAccessBuffer;
		uIntn fAccessBufferSize;
	};

	rPtr<cReadTask> QueryReadTask(void);
	void CompleteReadTask(rPtr<cReadTask> Task);
	rPtr<cWriteTask> QueryWriteTask(void);
	void CompleteWriteTask(rPtr<cWriteTask> Task);
private:
	// Read task queue

	cTaskQueue fReadTaskQueue;

	class cProcessReadTaskQueueProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)override;
	}fProcessReadTaskQueueProcedure;
	rPtr<iAsyncProcedure> fProcessReadTaskWork;
	bool ProcessReadTaskQueueProc(void);

	// Write task queue

	cTaskQueue fWriteTaskQueue;

	class cProcessWriteTaskQueueProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)override;
	}fProcessWriteTaskQueueProcedure;
	rPtr<iAsyncProcedure> fProcessWriteTaskWork;
	bool ProcessWriteTaskQueueProc(void);

	bool fStreamClosed;
	bool fStreamReadEnded;
	bool fStreamWriteEnded;
	bool fStreamWriteSetEnd;

	StreamError fLastErrorCode;

};
//---------------------------------------------------------------------------
class bcFileStream : public iFileStream, public iStreamErrorReport
{
public:
	bcFileStream();
	~bcFileStream();

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

	virtual void cnLib_FUNC Close(void)override;
	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)override;
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)override;
	virtual iPtr<iStreamTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size,uInt64 Offset)override;
	virtual iPtr<iStreamTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size,uInt64 Offset)override;
	//virtual bool cnLib_FUNC GetSize(uInt64 &Size)override;
	//virtual bool cnLib_FUNC SetSize(uInt64 NewSize)override;
	//virtual bool cnLib_FUNC FlushBuffer(void)override;

	virtual eStreamError cnLib_FUNC GetStreamError(void)override;

protected:
	void StreamClose(void);

	virtual void StreamProcessReadTask(void)=0;
	virtual void StreamProcessReadEnd(void)=0;
	virtual void StreamProcessWriteTask(void)=0;
	virtual void StreamProcessWriteEnd(void)=0;

	void UpdateReadTaskQueue(void);
	void UpdateWriteTaskQueue(void);

	// Stream Task

	class cAccessTask : public iStreamTask, public iStreamErrorReport , public cTaskQueue::cTask
	{
	public:
		cAccessTask()=default;
		~cAccessTask()=default;
		iPtr<bcFileStream> Owner;

		virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;
		virtual bool cnLib_FUNC IsDone(void)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;
		virtual bool cnLib_FUNC GetResult(uIntn &SizeCompleted)override;

		eStreamError cnLib_FUNC GetStreamError(void)override;
		StreamError AccessErrorCode;

		// TotalSizeCompleted
		//	size of buffer completed
		uIntn TotalSizeCompleted;
	};

	class cReadTask : public cAccessTask, public iWriteBuffer<void>
	{
	public:
		void PrepareBuffer(void *Buffer,uIntn BufferSize,uInt64 AccessOffset);
		virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn QuerySize)override;
		virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)override;
	
		virtual void cnLib_FUNC Cancel(void)override;

		bool IsAccessBufferCompleted(void)const;
		uInt64 AccessOffset;
	protected:
		// BufferList
		//	buffer list
		cMemory fAccessBuffer;
		uIntn fAccessBufferOffset;
	};

	class cWriteTask : public cAccessTask, public iReadBuffer<void>
	{
	public:
		void PrepareBuffer(const void *Buffer,uIntn BufferSize,uInt64 AccessOffset);
		virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)override;
		virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)override;
	
		virtual void cnLib_FUNC Cancel(void)override;

		bool IsAccessBufferCompleted(void)const;
		uInt64 AccessOffset;
	protected:
		// BufferList
		//	buffer list
		cConstMemory fAccessBuffer;
		uIntn fAccessBufferOffset;
	};

	rPtr<cReadTask> QueryReadTask(void);
	void CompleteReadTask(rPtr<cReadTask> Task);
	rPtr<cWriteTask> QueryWriteTask(void);
	void CompleteWriteTask(rPtr<cWriteTask> Task);
private:
	// Read task queue

	cTaskQueue fReadTaskQueue;

	class cProcessReadTaskQueueProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)override;
	}fProcessReadTaskQueueProcedure;
	rPtr<iAsyncProcedure> fProcessReadTaskWork;
	bool ProcessReadTaskQueueProc(void);

	// Write task queue

	cTaskQueue fWriteTaskQueue;

	class cProcessWriteTaskQueueProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)override;
	}fProcessWriteTaskQueueProcedure;
	rPtr<iAsyncProcedure> fProcessWriteTaskWork;
	bool ProcessWriteTaskQueueProc(void);

	bool fStreamClosed;

	StreamError fLastErrorCode;

};
//---------------------------------------------------------------------------
class bcWriteToReadQueue : public bcRWQueue
{
public:
	bcWriteToReadQueue();
	~bcWriteToReadQueue();

	cnLib_DEPRECATED void Close(void);
	uIntn InitialBuffer(const cConstMemory *Buffers,uIntn BufferCount);

	void SetBufferSizeLimit(uIntn Value);

	// Write Queue

	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn QuerySize)override;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)override;

	// Read Queue

	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)override;
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)override;

protected:
	
	virtual void WriteQueueClosed(void)override;


	virtual void ReadQueueClosed(void)override;

private:
	cAsyncLoopbackStreamBuffer fBufferQueue;
};
//---------------------------------------------------------------------------
class cChannelQueuePair : public iReference
{
public:
	cChannelQueuePair();
	~cChannelQueuePair();

	void SetupAsyncExecution(iAsyncExecution *AsyncExecution);
	iReadQueue* GetReadQueue(bool Side);
	iWriteQueue* GetWriteQueue(bool Side);
	void Close(bool Side);
	void SetWriteEndMode(bool Side,eEndpointWriteEndMode EndMode);

protected:

//	EndpontF				EndpointT
//	Write->		-QueueA-	->Read
//	Read<-		-QueueB-	<-Write

	class cQueueA : public bcWriteToReadQueue
	{
		cChannelQueuePair* GetHost(void);
	protected:
	
		virtual iReference* RWQueueInnerReference(void)override;

		virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
		virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;
	public:
		using bcWriteToReadQueue::CreateReadQueueWork;
		using bcWriteToReadQueue::CreateWriteQueueWork;
		using bcWriteToReadQueue::WriteQueueSetEndMode;
	}fQueueA;

	class cQueueB : public bcWriteToReadQueue
	{
		cChannelQueuePair* GetHost(void);
	protected:
		virtual iReference* RWQueueInnerReference(void)override;
		
		virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
		virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;
	public:
		using bcWriteToReadQueue::CreateReadQueueWork;
		using bcWriteToReadQueue::CreateWriteQueueWork;
		using bcWriteToReadQueue::WriteQueueSetEndMode;
	}fQueueB;
};
//---------------------------------------------------------------------------
class cChannelEndpointPair : public cChannelQueuePair
{
public:
	cChannelEndpointPair();
	~cChannelEndpointPair();

	iEndpoint* GetEndpoint(bool Side);

protected:

	class cEndpointF : public iEndpoint
	{
		cChannelEndpointPair* GetHost(void);
	public:
		virtual void* cnLib_FUNC CastInterface(iTypeID TypeID)noexcept(true) override;

		virtual void cnLib_FUNC Close(void)override;
		virtual iReadQueue *cnLib_FUNC GetReadQueue(void)override;
		virtual iWriteQueue *cnLib_FUNC GetWriteQueue(void)override;
		virtual void cnLib_FUNC SetWriteEndMode(eEndpointWriteEndMode EndMode)override;
	}fEndpointF;

	class cEndpointT : public iEndpoint
	{
		cChannelEndpointPair* GetHost(void);
	public:
		virtual void* cnLib_FUNC CastInterface(iTypeID TypeID)noexcept(true) override;

		virtual void cnLib_FUNC Close(void)override;
		virtual iReadQueue *cnLib_FUNC GetReadQueue(void)override;
		virtual iWriteQueue *cnLib_FUNC GetWriteQueue(void)override;
		virtual void cnLib_FUNC SetWriteEndMode(eEndpointWriteEndMode EndMode)override;
	}fEndpointT;
};
//---------------------------------------------------------------------------
class bcMultipointQueue : public iMultipointQueue
{
public:
	bcMultipointQueue();
	~bcMultipointQueue();

	void CloseMultipointQueue(void);

	bool MultipointQueueIsStarted(void)const;
	bool MultipointQueueIsClosed(void)const;

	virtual bool cnLib_FUNC StartAccess(iReference *Reference,iMultipointQueueCallback *Callback)override;
	virtual void cnLib_FUNC StopAccess(void)override;
	virtual bool cnLib_FUNC IsClosed(void)override;
	virtual void cnLib_FUNC NotifyRead(bool IdleNotify)override;
	virtual void cnLib_FUNC NotifyWrite(bool IdleNotify)override;
	
protected:

	iMultipointQueueCallback *fMultipointCallback;
	rPtr<iReference> fMultipointCallbackReference;

	void CreateMultipointQueueReadWork(iAsyncExecution *AsyncExecution);
	void CreateMultipointQueueStateWriteWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop);
	void UpdateMultipointQueueState(bool AsyncNotify);

	virtual iReference* MultipointQueueInnerReference(void)=0;

	virtual void MultipointQueueStarted(void);
	virtual void MultipointQueueStopped(void);
	virtual void MultipointQueueClosed(void);
	virtual void MultipointQueueReadNotify(void);
	virtual void MultipointQueueWriteNotify(void);

	void MultipointQueueSetTerminated(void);
	void MultipointQueueSetReadBufferAvailable(bool AsyncNotify);
	void MultipointQueueSetWriteBufferAvailable(bool AsyncNotify);

private:
	bool fMultipointQueueTerminated;
	bool fMultipointQueueReadBufferAvailable;
	bool fMultipointQueueWriteBufferAvailable;
	ufInt8 fMultipointQueueReadNotifyType;
	ufInt8 fMultipointQueueWriteNotifyType;

	bcNotificationCycle::CycleState MultipointQueueCheckState(void);
	void MultipointQueueNotify(void);
	void MultipointQueueNotifyRead(void);
	class cRWQueue : public bcNotificationCycle
	{
	public:
		bcMultipointQueue* GetHost(void);
		virtual iReference* NotificationInnerReference(void)override;
		virtual void NotificationStarted(void)override;
		virtual void NotificationStopped(void)override;
		virtual void NotificationClosed(void)override;
		virtual CycleState NotificationCheckState(void)override;
		virtual void NotificationProcedure(void)override;
	}fRWQueue;

	class cStateWriteProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)override;
	}fStateWriteProcedure;
	rPtr<iAsyncProcedure> fStateWriteProcedureWork;
	void (bcMultipointQueue::*fMultipointQueueUpdateStateProc)(bool AsyncNotify);
	void MultipointQueueUpdateStateProc_Run(bool AsyncNotify);
	void MultipointQueueUpdateStateProc_Async(bool AsyncNotify);
	void MultipointQueueUpdateStateProc_AsyncAfter(bool AsyncNotify);

	class cNotifyReadProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)override;
	}fNotifyReadProcedure;
	rPtr<iAsyncProcedure> fNotifyReadProcedureWork;
	void (bcMultipointQueue::*fMultipointQueueNotifyReadProc)(void);
	void MultipointQueueNotifyReadProc_Run(void);
	void MultipointQueueNotifyReadProc_Async(void);
};
//---------------------------------------------------------------------------
class cBufferedExtEndpoint : public iEndpoint, protected bcRWQueue
	// move to StreamAdapter
{
public:
	cBufferedExtEndpoint();
	~cBufferedExtEndpoint();

	virtual void cnLib_FUNC Close(void)override;
	virtual iReadQueue* cnLib_FUNC GetReadQueue(void)override;
	virtual iWriteQueue* cnLib_FUNC GetWriteQueue(void)override;
	virtual void cnLib_FUNC SetWriteEndMode(eEndpointWriteEndMode EndMode)override;

	uIntn GetTotalBufferedReadData(void)const;
	void PutReadData(const void *Data,uIntn Size);
	cMemory QueryReadDataBuffer(uIntn QuerySize);
	void AdvanceReadDataBuffer(uIntn Size);
	//void SetReadDataEnd(void);

	cConstMemory QueryWriteData(void);
	void AdvanceWriteData(uIntn Size);
	bool IsWriteDataEnded(void);

protected:

	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)override;
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)override;
	virtual void ReadQueueStarted(void)override;
	virtual void ReadQueueStopped(void)override;
	virtual void ReadQueueClosed(void)override;

	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn QuerySize)override;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)override;
	virtual void WriteQueueStarted(void)override;
	virtual void WriteQueueStopped(void)override;
	virtual void WriteQueueClosed(void)override;

	cnRTL::cAsyncLoopbackStreamBuffer fReadDataQueue;

	cnRTL::cAsyncLoopbackStreamBuffer fWriteDataQueue;
	
	cnRTL::cAtomicVar<uIntn> fReadQueueDataSize;
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
