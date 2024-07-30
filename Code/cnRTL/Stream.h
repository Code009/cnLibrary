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
	bcReadQueue()noexcept(true);
	~bcReadQueue()noexcept(true);

	virtual bool cnLib_FUNC StartRead(iReference *Reference,iReadQueueCallback *Callback)noexcept(true)override;
	virtual void cnLib_FUNC StopRead(void)noexcept(true)override;
	virtual void cnLib_FUNC NotifyRead(uIntn SizeToNotify)noexcept(true)override;
	virtual void cnLib_FUNC CloseRead(bool Terminate)noexcept(true)override;
	virtual bool cnLib_FUNC IsReadClosed(bool &GracefulClose)noexcept(true)override;
protected:
	iReadQueueCallback *fReadCallback;
	rPtr<iReference> fReadCallbackReference;

	virtual void NotificationStarted(void)noexcept(true)override;
	virtual void NotificationStopped(void)noexcept(true)override;
	virtual CycleState NotificationCheckState(void)noexcept(true)override;
	virtual void AsyncQueueNotify(void)noexcept(true)override;

	void ReadQueueReportBufferAvailable(bool AsyncNotify)noexcept(true);
	void ReadQueueReportBufferEmpty(void)noexcept(true);
	void ReadQueueReportEnded(void)noexcept(true);
	void ReadQueueReportTerminated(void)noexcept(true);
private:
	bool fReadQueueBufferAvailable;
	bool fReadQueueEnded;
	bool fReadQueueCloseWhenIdle;
	bool fReadQueueTerminated;
};
//---------------------------------------------------------------------------
class bcWriteQueue : public iWriteQueue, protected bcAsyncQueue
{
public:
	bcWriteQueue()noexcept(true);
	~bcWriteQueue()noexcept(true);

	virtual bool cnLib_FUNC StartWrite(iReference *Reference,iWriteQueueCallback *Callback)noexcept(true)override;
	virtual void cnLib_FUNC StopWrite(void)noexcept(true)override;
	virtual void cnLib_FUNC NotifyWrite(uIntn SizeToNotify)noexcept(true)override;
	virtual void cnLib_FUNC CloseWrite(bool Terminate)noexcept(true)override;
	virtual bool cnLib_FUNC IsWriteClosed(bool &GracefulClose)noexcept(true)override;
protected:
	iWriteQueueCallback *fWriteCallback;
	rPtr<iReference> fWriteCallbackReference;

	virtual void NotificationStarted(void)noexcept(true)override;
	virtual void NotificationStopped(void)noexcept(true)override;
	virtual CycleState NotificationCheckState(void)noexcept(true)override;
	virtual void AsyncQueueNotify(void)noexcept(true)override;

	void WriteQueueReportBufferAvailable(bool AsyncNotify)noexcept(true);
	void WriteQueueReportTerminated(void)noexcept(true);

private:

	bool fWriteQueueTerminated;
	bool fWriteQueueCloseWhenIdle;
};
//---------------------------------------------------------------------------
class bcRWQueue : public iReadQueue, public iWriteQueue
{
public:
	typedef iReadQueue tReferenceInterface;
	typedef bcAsyncQueue::CycleState CycleState;

	bcRWQueue()noexcept(true);
	~bcRWQueue()noexcept(true);

	void CloseReadQueue(void)noexcept(true);
	void CloseWriteQueue(void)noexcept(true);

	bool ReadQueueIsStarted(void)const noexcept(true);
	bool ReadQueueIsClosed(void)const noexcept(true);
	bool ReadQueueIsEnded(void)const noexcept(true);

	bool WriteQueueIsStarted(void)const noexcept(true);
	bool WriteQueueIsClosed(void)const noexcept(true);
	bool WriteQueueIsEnded(void)const noexcept(true);

	virtual bool cnLib_FUNC StartRead(iReference *Reference,iReadQueueCallback *Callback)noexcept(true)override;
	virtual void cnLib_FUNC StopRead(void)noexcept(true)override;
	virtual void cnLib_FUNC NotifyRead(uIntn SizeToNotify)noexcept(true)override;
	virtual void cnLib_FUNC CloseRead(bool Terminate)noexcept(true)override;
	virtual bool cnLib_FUNC IsReadClosed(bool &GracefulClose)noexcept(true)override;


	virtual bool cnLib_FUNC StartWrite(iReference *Reference,iWriteQueueCallback *Callback)noexcept(true)override;
	virtual void cnLib_FUNC StopWrite(void)noexcept(true)override;
	virtual void cnLib_FUNC NotifyWrite(uIntn SizeToNotify)noexcept(true)override;
	virtual void cnLib_FUNC CloseWrite(bool Terminate)noexcept(true)override;
	virtual bool cnLib_FUNC IsWriteClosed(bool &GracefulClose)noexcept(true)override;
	
protected:

	iReadQueueCallback *fReadCallback;
	iWriteQueueCallback *fWriteCallback;
	rPtr<iReference> fReadCallbackReference;
	rPtr<iReference> fWriteCallbackReference;

	void CreateReadQueueWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop)noexcept(true);
	void UpdateReadQueueState(bool AsyncNotify)noexcept(true);

	void CreateWriteQueueWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop)noexcept(true);
	void UpdateWriteQueueState(bool AsyncNotify)noexcept(true);

	virtual iReference* RWQueueInnerReference(void)noexcept(true)=0;

	virtual void ReadQueueStarted(void)noexcept(true);
	virtual void ReadQueueStopped(void)noexcept(true);
	virtual void ReadQueueClosed(void)noexcept(true);
	virtual void ReadQueueNotify(void)noexcept(true);

	virtual void WriteQueueStarted(void)noexcept(true);
	virtual void WriteQueueStopped(void)noexcept(true);
	virtual void WriteQueueClosed(void)noexcept(true);
	virtual void WriteQueueNotify(void)noexcept(true);

	void ReadQueueReportBufferAvailable(bool AsyncNotify)noexcept(true);
	void ReadQueueReportBufferEmpty(void)noexcept(true);
	void ReadQueueReportEnded(void)noexcept(true);
	void ReadQueueReportTerminated(void)noexcept(true);

	void WriteQueueReportBufferAvailable(bool AsyncNotify)noexcept(true);
	void WriteQueueReportTerminated(void)noexcept(true);

private:
	bool fReadQueueBufferAvailable;
	bool fReadQueueEnded;
	bool fReadQueueCloseWhenIdle;
	bool fReadQueueTerminated;

	bool fWriteQueueTerminated;
	bool fWriteQueueCloseWhenIdle;

	CycleState ReadQueueCheckState(void)noexcept(true);
	CycleState WriteQueueCheckState(void)noexcept(true);
	class cReadQueue : public bcAsyncQueue
	{
	public:
		bcRWQueue* GetHost(void)noexcept(true);
		virtual iReference* NotificationInnerReference(void)noexcept(true)override;
		virtual void NotificationStarted(void)noexcept(true)override;
		virtual void NotificationStopped(void)noexcept(true)override;
		virtual void NotificationClosed(void)noexcept(true)override;
		virtual CycleState NotificationCheckState(void)noexcept(true)override;
		virtual void AsyncQueueNotify(void)noexcept(true)override;

		using bcAsyncQueue::CreateQueueWork;
		using bcAsyncQueue::UpdateQueueState;
		using bcAsyncQueue::AsyncQueueSetAvailable;
	}fReadAsyncQueue;
	class cWriteQueue : public bcAsyncQueue
	{
	public:
		bcRWQueue* GetHost(void)noexcept(true);
		virtual iReference* NotificationInnerReference(void)noexcept(true)override;
		virtual void NotificationStarted(void)noexcept(true)override;
		virtual void NotificationStopped(void)noexcept(true)override;
		virtual void NotificationClosed(void)noexcept(true)override;
		virtual CycleState NotificationCheckState(void)noexcept(true)override;
		virtual void AsyncQueueNotify(void)noexcept(true)override;

		using bcAsyncQueue::CreateQueueWork;
		using bcAsyncQueue::UpdateQueueState;
		using bcAsyncQueue::AsyncQueueSetAvailable;
	}fWriteAsyncQueue;
};
//---------------------------------------------------------------------------
class bcStream : public iStream, public iStreamErrorReport
{
public:
	bcStream()noexcept(true);
	~bcStream()noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

	virtual void cnLib_FUNC Close(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsEndOfReading(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetEndOfStream(void)noexcept(true)override;
	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)override;
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)override;
	virtual iPtr<iStreamTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size)noexcept(true)override;
	virtual iPtr<iStreamTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size)noexcept(true)override;

	virtual eStreamError cnLib_FUNC GetStreamError(void)noexcept(true)override;

protected:
	
protected:
	void StreamClose(void)noexcept(true);

	virtual void StreamProcessReadTask(void)noexcept(true)=0;
	virtual void StreamProcessReadEnd(void)noexcept(true)=0;
	virtual void StreamProcessWriteTask(void)noexcept(true)=0;
	virtual void StreamProcessWriteSetEnd(void)noexcept(true)=0;
	virtual void StreamProcessWriteEnd(void)noexcept(true)=0;

	void UpdateReadTaskQueue(void)noexcept(true);
	void UpdateWriteTaskQueue(void)noexcept(true);

	void SetReadEnd(bool GracefulClose)noexcept(true);
	void SetWriteEnd(bool GracefulClose)noexcept(true);

	// Stream Task

	class cAccessTask : public iStreamTask, public iStreamErrorReport , public cTaskQueue::cTask
	{
	public:
		cAccessTask()=default;
		~cAccessTask()=default;
		iPtr<bcStream> Owner;

		virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;
		virtual bool cnLib_FUNC IsDone(void)noexcept(true)override;
		virtual bool cnLib_FUNC Await(iProcedure *NotifyProcedure)noexcept(true)override;
		virtual bool cnLib_FUNC GetResult(uIntn &SizeCompleted)noexcept(true)override;

		eStreamError cnLib_FUNC GetStreamError(void)noexcept(true)override;
		StreamError AccessErrorCode;

		// TotalSizeCompleted
		//	size of buffer completed
		uIntn TotalSizeCompleted;
	};

	class cReadTask : public cAccessTask, public iWriteBuffer<void>
	{
	public:
		void Setup(void *Buffer,uIntn Size)noexcept(true);
		virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn QuerySize)noexcept(true)override;
		virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)noexcept(true)override;
	
		virtual void cnLib_FUNC Cancel(void)noexcept(true)override;

		bool IsAccessBufferCompleted(void)const noexcept(true);

	protected:
		// BufferList
		//	buffer list
		void *fAccessBuffer;
		uIntn fAccessBufferSize;
	};

	class cWriteTask : public cAccessTask, public iReadBuffer<void>
	{
	public:
		void Setup(const void *Buffer,uIntn Size)noexcept(true);
		virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)noexcept(true)override;
		virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)noexcept(true)override;
	
		virtual void cnLib_FUNC Cancel(void)noexcept(true)override;
		
		bool IsAccessBufferCompleted(void)const noexcept(true);

	protected:
		// BufferList
		//	buffer list
		const void *fAccessBuffer;
		uIntn fAccessBufferSize;
	};

	rPtr<cReadTask> QueryReadTask(void)noexcept(true);
	void CompleteReadTask(rPtr<cReadTask> Task)noexcept(true);
	rPtr<cWriteTask> QueryWriteTask(void)noexcept(true);
	void CompleteWriteTask(rPtr<cWriteTask> Task)noexcept(true);
private:
	// Read task queue

	cTaskQueue fReadTaskQueue;

	class cProcessReadTaskQueueProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)noexcept(true)override;
	}fProcessReadTaskQueueProcedure;
	rPtr<iAsyncProcedure> fProcessReadTaskWork;
	bool ProcessReadTaskQueueProc(void)noexcept(true);

	// Write task queue

	cTaskQueue fWriteTaskQueue;

	class cProcessWriteTaskQueueProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)noexcept(true)override;
	}fProcessWriteTaskQueueProcedure;
	rPtr<iAsyncProcedure> fProcessWriteTaskWork;
	bool ProcessWriteTaskQueueProc(void)noexcept(true);

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
	bcFileStream()noexcept(true);
	~bcFileStream()noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

	virtual void cnLib_FUNC Close(void)noexcept(true)override;
	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)noexcept(true)override;
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)noexcept(true)override;
	virtual iPtr<iStreamTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size,uInt64 Offset)noexcept(true)override;
	virtual iPtr<iStreamTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size,uInt64 Offset)noexcept(true)override;
	//virtual bool cnLib_FUNC GetSize(uInt64 &Size)noexcept(true)override;
	//virtual bool cnLib_FUNC SetSize(uInt64 NewSize)noexcept(true)override;
	//virtual bool cnLib_FUNC FlushBuffer(void)noexcept(true)override;

	virtual eStreamError cnLib_FUNC GetStreamError(void)noexcept(true)override;

protected:
	void StreamClose(void)noexcept(true);

	virtual void StreamProcessReadTask(void)noexcept(true)=0;
	virtual void StreamProcessReadEnd(void)noexcept(true)=0;
	virtual void StreamProcessWriteTask(void)noexcept(true)=0;
	virtual void StreamProcessWriteEnd(void)noexcept(true)=0;

	void UpdateReadTaskQueue(void)noexcept(true);
	void UpdateWriteTaskQueue(void)noexcept(true);

	// Stream Task

	class cAccessTask : public iStreamTask, public iStreamErrorReport , public cTaskQueue::cTask
	{
	public:
		cAccessTask()=default;
		~cAccessTask()=default;
		iPtr<bcFileStream> Owner;

		virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;
		virtual bool cnLib_FUNC IsDone(void)noexcept(true)override;
		virtual bool cnLib_FUNC Await(iProcedure *NotifyProcedure)noexcept(true)override;
		virtual bool cnLib_FUNC GetResult(uIntn &SizeCompleted)noexcept(true)override;

		eStreamError cnLib_FUNC GetStreamError(void)noexcept(true)override;
		StreamError AccessErrorCode;

		// TotalSizeCompleted
		//	size of buffer completed
		uIntn TotalSizeCompleted;
	};

	class cReadTask : public cAccessTask, public iWriteBuffer<void>
	{
	public:
		void PrepareBuffer(void *Buffer,uIntn BufferSize,uInt64 AccessOffset)noexcept(true);
		virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn QuerySize)noexcept(true)override;
		virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)noexcept(true)override;
	
		virtual void cnLib_FUNC Cancel(void)noexcept(true)override;

		bool IsAccessBufferCompleted(void)const noexcept(true);
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
		void PrepareBuffer(const void *Buffer,uIntn BufferSize,uInt64 AccessOffset)noexcept(true);
		virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)noexcept(true)override;
		virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)noexcept(true)override;
	
		virtual void cnLib_FUNC Cancel(void)noexcept(true)override;

		bool IsAccessBufferCompleted(void)const noexcept(true);
		uInt64 AccessOffset;
	protected:
		// BufferList
		//	buffer list
		cConstMemory fAccessBuffer;
		uIntn fAccessBufferOffset;
	};

	rPtr<cReadTask> QueryReadTask(void)noexcept(true);
	void CompleteReadTask(rPtr<cReadTask> Task)noexcept(true);
	rPtr<cWriteTask> QueryWriteTask(void)noexcept(true);
	void CompleteWriteTask(rPtr<cWriteTask> Task)noexcept(true);
private:
	// Read task queue

	cTaskQueue fReadTaskQueue;

	class cProcessReadTaskQueueProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)noexcept(true)override;
	}fProcessReadTaskQueueProcedure;
	rPtr<iAsyncProcedure> fProcessReadTaskWork;
	bool ProcessReadTaskQueueProc(void)noexcept(true);

	// Write task queue

	cTaskQueue fWriteTaskQueue;

	class cProcessWriteTaskQueueProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)noexcept(true)override;
	}fProcessWriteTaskQueueProcedure;
	rPtr<iAsyncProcedure> fProcessWriteTaskWork;
	bool ProcessWriteTaskQueueProc(void)noexcept(true);

	bool fStreamClosed;

	StreamError fLastErrorCode;

};
//---------------------------------------------------------------------------
class bcWriteToReadQueue : public bcRWQueue
{
public:
	bcWriteToReadQueue(uIntn BufferSize=65536)noexcept(true);
	~bcWriteToReadQueue()noexcept(true);

	cnLib_DEPRECATED void Close(void)noexcept(true);
	uIntn InitialBuffer(const cConstMemory *Buffers,uIntn BufferCount)noexcept(true);

	// Write Queue

	virtual uIntn cnLib_FUNC GetMaxWriteBufferSize(void)noexcept(true)override;
	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn QuerySize)noexcept(true)override;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)noexcept(true)override;

	// Read Queue

	virtual uIntn cnLib_FUNC GetMaxReadBufferSize(void)noexcept(true)override;
	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)noexcept(true)override;
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)noexcept(true)override;

protected:
	
	virtual void WriteQueueClosed(void)noexcept(true)override;


	virtual void ReadQueueClosed(void)noexcept(true)override;

private:
	cAsyncLoopbackStreamBuffer fBufferQueue;
};
//---------------------------------------------------------------------------
class cChannelQueuePair : public iReference
{
public:
	cChannelQueuePair()noexcept(true);
	~cChannelQueuePair()noexcept(true);

	void SetupAsyncExecution(iAsyncExecution *AsyncExecution)noexcept(true);
	iReadQueue* GetReadQueue(bool Side)noexcept(true);
	iWriteQueue* GetWriteQueue(bool Side)noexcept(true);
	void Close(bool Side)noexcept(true);

protected:

//	EndpontF				EndpointT
//	Write->		-QueueA-	->Read
//	Read<-		-QueueB-	<-Write

	class cQueueA : public bcWriteToReadQueue
	{
		cChannelQueuePair* GetHost(void)noexcept(true);
	protected:
	
		virtual iReference* RWQueueInnerReference(void)noexcept(true)override;

		virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
		virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;
	public:
		using bcWriteToReadQueue::CreateReadQueueWork;
		using bcWriteToReadQueue::CreateWriteQueueWork;
	}fQueueA;

	class cQueueB : public bcWriteToReadQueue
	{
		cChannelQueuePair* GetHost(void)noexcept(true);
	protected:
		virtual iReference* RWQueueInnerReference(void)noexcept(true)override;
		
		virtual void cnLib_FUNC IncreaseReference(void)noexcept(true) override;
		virtual void cnLib_FUNC DecreaseReference(void)noexcept(true) override;
	public:
		using bcWriteToReadQueue::CreateReadQueueWork;
		using bcWriteToReadQueue::CreateWriteQueueWork;
	}fQueueB;
};
//---------------------------------------------------------------------------
class cChannelEndpointPair : public cChannelQueuePair
{
public:
	cChannelEndpointPair()noexcept(true);
	~cChannelEndpointPair()noexcept(true);

	iEndpoint* GetEndpoint(bool Side)noexcept(true);

protected:

	class cEndpointF : public iEndpoint
	{
		cChannelEndpointPair* GetHost(void)noexcept(true);
	public:
		virtual void* cnLib_FUNC CastInterface(iTypeID TypeID)noexcept(true) override;

		virtual void cnLib_FUNC Close(void)noexcept(true)override;
		virtual iReadQueue *cnLib_FUNC GetReadQueue(void)noexcept(true)override;
		virtual iWriteQueue *cnLib_FUNC GetWriteQueue(void)noexcept(true)override;
	}fEndpointF;

	class cEndpointT : public iEndpoint
	{
		cChannelEndpointPair* GetHost(void)noexcept(true);
	public:
		virtual void* cnLib_FUNC CastInterface(iTypeID TypeID)noexcept(true) override;

		virtual void cnLib_FUNC Close(void)noexcept(true)override;
		virtual iReadQueue *cnLib_FUNC GetReadQueue(void)noexcept(true)override;
		virtual iWriteQueue *cnLib_FUNC GetWriteQueue(void)noexcept(true)override;
	}fEndpointT;
};
//---------------------------------------------------------------------------
class bcMultipointQueue : public iMultipointQueue
{
public:
	bcMultipointQueue()noexcept(true);
	~bcMultipointQueue()noexcept(true);

	void CloseMultipointQueue(void)noexcept(true);

	bool MultipointQueueIsStarted(void)const noexcept(true);
	bool MultipointQueueIsClosed(void)const noexcept(true);

	virtual bool cnLib_FUNC StartAccess(iReference *Reference,iMultipointQueueCallback *Callback)noexcept(true)override;
	virtual void cnLib_FUNC StopAccess(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsClosed(void)noexcept(true)override;
	virtual void cnLib_FUNC NotifyRead(bool IdleNotify)noexcept(true)override;
	virtual void cnLib_FUNC NotifyWrite(bool IdleNotify)noexcept(true)override;
	
protected:

	iMultipointQueueCallback *fMultipointCallback;
	rPtr<iReference> fMultipointCallbackReference;

	void CreateMultipointQueueReadWork(iAsyncExecution *AsyncExecution)noexcept(true);
	void CreateMultipointQueueStateWriteWork(iAsyncExecution *AsyncExecution,bool DisableUpdateThreadLoop)noexcept(true);
	void UpdateMultipointQueueState(bool AsyncNotify)noexcept(true);

	virtual iReference* MultipointQueueInnerReference(void)noexcept(true)=0;

	virtual void MultipointQueueStarted(void)noexcept(true);
	virtual void MultipointQueueStopped(void)noexcept(true);
	virtual void MultipointQueueClosed(void)noexcept(true);
	virtual void MultipointQueueReadNotify(void)noexcept(true);
	virtual void MultipointQueueWriteNotify(void)noexcept(true);

	void MultipointQueueSetTerminated(void)noexcept(true);
	void MultipointQueueSetReadBufferAvailable(bool AsyncNotify)noexcept(true);
	void MultipointQueueSetWriteBufferAvailable(bool AsyncNotify)noexcept(true);

private:
	bool fMultipointQueueTerminated;
	bool fMultipointQueueReadBufferAvailable;
	bool fMultipointQueueWriteBufferAvailable;
	ufInt8 fMultipointQueueReadNotifyType;
	ufInt8 fMultipointQueueWriteNotifyType;

	bcNotificationCycle::CycleState MultipointQueueCheckState(void)noexcept(true);
	void MultipointQueueNotify(void)noexcept(true);
	void MultipointQueueNotifyRead(void)noexcept(true);
	class cRWQueue : public bcNotificationCycle
	{
	public:
		bcMultipointQueue* GetHost(void)noexcept(true);
		virtual iReference* NotificationInnerReference(void)noexcept(true)override;
		virtual void NotificationStarted(void)noexcept(true)override;
		virtual void NotificationStopped(void)noexcept(true)override;
		virtual void NotificationClosed(void)noexcept(true)override;
		virtual CycleState NotificationCheckState(void)noexcept(true)override;
		virtual void NotificationProcedure(void)noexcept(true)override;
	}fRWQueue;

	class cStateWriteProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)noexcept(true)override;
	}fStateWriteProcedure;
	rPtr<iAsyncProcedure> fStateWriteProcedureWork;
	void (bcMultipointQueue::*fMultipointQueueUpdateStateProc)(bool AsyncNotify)noexcept(true);
	void MultipointQueueUpdateStateProc_Run(bool AsyncNotify)noexcept(true);
	void MultipointQueueUpdateStateProc_Async(bool AsyncNotify)noexcept(true);
	void MultipointQueueUpdateStateProc_AsyncAfter(bool AsyncNotify)noexcept(true);

	class cNotifyReadProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)noexcept(true)override;
	}fNotifyReadProcedure;
	rPtr<iAsyncProcedure> fNotifyReadProcedureWork;
	void (bcMultipointQueue::*fMultipointQueueNotifyReadProc)(void)noexcept(true);
	void MultipointQueueNotifyReadProc_Run(void)noexcept(true);
	void MultipointQueueNotifyReadProc_Async(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cBufferedExtEndpoint : public iEndpoint, protected bcRWQueue
	// move to StreamAdapter
{
public:
	cBufferedExtEndpoint()noexcept(true);
	~cBufferedExtEndpoint()noexcept(true);

	virtual void cnLib_FUNC Close(void)noexcept(true)override;
	virtual iReadQueue* cnLib_FUNC GetReadQueue(void)noexcept(true)override;
	virtual iWriteQueue* cnLib_FUNC GetWriteQueue(void)noexcept(true)override;

	uIntn GetTotalBufferedReadData(void)const noexcept(true);
	void PutReadData(const void *Data,uIntn Size)noexcept(true);
	cMemory QueryReadDataBuffer(uIntn QuerySize)noexcept(true);
	void AdvanceReadDataBuffer(uIntn Size)noexcept(true);
	//void SetReadDataEnd(void)noexcept(true);

	cConstMemory QueryWriteData(void)noexcept(true);
	void AdvanceWriteData(uIntn Size)noexcept(true);
	bool IsWriteDataEnded(void)noexcept(true);

protected:

	virtual uIntn cnLib_FUNC GetMaxReadBufferSize(void)noexcept(true)override;
	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)noexcept(true)override;
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)noexcept(true)override;
	virtual void ReadQueueStarted(void)noexcept(true)override;
	virtual void ReadQueueStopped(void)noexcept(true)override;
	virtual void ReadQueueClosed(void)noexcept(true)override;

	virtual uIntn cnLib_FUNC GetMaxWriteBufferSize(void)noexcept(true)override;
	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn QuerySize)noexcept(true)override;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)noexcept(true)override;
	virtual void WriteQueueStarted(void)noexcept(true)override;
	virtual void WriteQueueStopped(void)noexcept(true)override;
	virtual void WriteQueueClosed(void)noexcept(true)override;

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
