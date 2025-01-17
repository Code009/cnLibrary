/*- cnSystem - IO Device --------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2008-08-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnSystem_cnStream_H__
#define	__cnLibrary_cnSystem_cnStream_H__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnFundamental.h>
#include <cnSystem/cnData.h>
#include <cnSystem/cnThread.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#define	override
#endif
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAsyncNotificationCallback
{
public:
	// AsyncStarted
	//	called before iAsyncNotification start using this interface
	virtual void cnLib_FUNC AsyncStarted(void)noexcept(true)=0;
	// AsyncStopped
	//	called after iAsyncNotification finished using the interface
	virtual void cnLib_FUNC AsyncStopped(void)noexcept(true)=0;

	// AsyncNotify
	//	called when user requested an notification.
	//	when the function is called, notification signal will be cleared, and user need to call NotifyCallback again for another notification
	virtual void cnLib_FUNC AsyncNotify(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAsyncNotification : public iReference
{
public:
	// StartNotify
	//	start notify and calls callback AsyncStarted
	// Callback		callback
	// return true if successful registered callback
	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept(true)=0;
	// StopNotify
	//	stop notify, call AsyncStopped after all running notification is finished
	virtual void cnLib_FUNC StopNotify(void)noexcept(true)=0;
	// NotifyCallback
	//	request a notify to callback. the iAsyncNotification will call callback asynchronously if notification signal arrived or user requested an idle notification
	//	IdleNotify		if true, the callback will be called even when no pending notification signals. if false, the callback will be called on when there is notification signal
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)noexcept(true)=0;

	// IsClosed
	//	check if notification is closed
	virtual bool cnLib_FUNC IsClosed(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAsyncQueueCallback
{
public:
	virtual void cnLib_FUNC QueueStarted(void)noexcept(true)=0;
	virtual void cnLib_FUNC QueueStopped(void)noexcept(true)=0;
	virtual void cnLib_FUNC QueueNotify(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAsyncQueue : public iInterface
{
public:
	virtual bool cnLib_FUNC StartNotify(iAsyncQueueCallback *Callback)noexcept(true)=0;
	virtual void cnLib_FUNC StopNotify(void)noexcept(true)=0;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)noexcept(true)=0;
	virtual bool cnLib_FUNC IsClosed(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iReadQueueCallback
{
public:
	// ReadStarted
	//	called before iReadQueue using the interface
	virtual void cnLib_FUNC ReadStarted(void)noexcept(true)=0;
	// ReadStopped
	//	called after iReadQueue done using the interface
	virtual void cnLib_FUNC ReadStopped(void)noexcept(true)=0;

	// ReadNotify
	//	called when iReadQueue has buffer available for reading , or when user called NotifyRead
	virtual void cnLib_FUNC ReadNotify(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iReadQueue : public iReference
{
public:
	typedef void tElement;

	// StartRead
	//	start reading from the data queue
	// Callback		callback
	// return true if it start reading
	virtual bool cnLib_FUNC StartRead(iReference *Reference,iReadQueueCallback *Callback)noexcept(true)=0;
	// StopRead
	//	stop reading
	virtual void cnLib_FUNC StopRead(void)noexcept(true)=0;
	// NotifyRead
	//	notify callback at least once after the call, if there are more data available in read buffer since last notification
	//	IdleNotify		notify even when pending buffer is empty
	virtual void cnLib_FUNC NotifyRead(uIntn SizeToNotify)noexcept(true)=0;

	// CloseRead
	//	terminate read
	virtual void cnLib_FUNC CloseRead(bool Terminate)noexcept(true)=0;

	// IsReadClosed
	//	test if read queue is closed
	// GracefulClose	wehter the read stream is correctly received from source
	virtual bool cnLib_FUNC IsReadClosed(bool &GracefulClose)noexcept(true)=0;
	
	virtual uIntn cnLib_FUNC GetMaxReadBufferSize(void)noexcept(true)=0;

	// GatherReadBuffer
	//	gather read data to buffer
	// Length			number of elements to gather for reading.
	// return array of data buffer for reading
	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Length)noexcept(true)=0;
	// DismissReadBuffer
	//	dismiss length of read data . Caller should not access returned read buffer after the call
	// Length		length of data to dismiss, in elements
	// return length of data dismissed
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Length)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWriteQueueCallback
{
public:
	// WriteStarted
	//	called before iWriteQueue using the interface and before StartPull returns
	virtual void cnLib_FUNC WriteStarted(void)noexcept(true)=0;
	// WriteStopped
	//	called after iWriteQueue done using the interface
	virtual void cnLib_FUNC WriteStopped(void)noexcept(true)=0;

	// WriteNotify
	//	called when iWriteQueue has buffer available for writing, or when user called NotifyWrite
	virtual void cnLib_FUNC WriteNotify(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWriteQueue : public iReference
{
public:
	typedef void tElement;

	// StartWrite
	//	start writing to the data queue
	// Callback		callback
	// return true if it start writing
	virtual bool cnLib_FUNC StartWrite(iReference *Reference,iWriteQueueCallback *Callback)noexcept(true)=0;
	// StopWrite
	//	stop writing
	virtual void cnLib_FUNC StopWrite(void)noexcept(true)=0;
	// NotifyWrite
	//	notify callback at least once after the call, if there are more space available in write buffer since last notification
	//	IdleNotify		notify even when write buffer is full
	virtual void cnLib_FUNC NotifyWrite(uIntn SizeToNotify)noexcept(true)=0;

	// CloseWrite
	// Terminate		wether to discard pending buffer
	virtual void cnLib_FUNC CloseWrite(bool Terminate)noexcept(true)=0;

	// IsWriteClosed
	//	test if write queue is closed
	// GracefulClose	wehter the write stream is correctly sent to the destination
	virtual bool cnLib_FUNC IsWriteClosed(bool &GracefulClose)noexcept(true)=0;
	
	virtual uIntn cnLib_FUNC GetMaxWriteBufferSize(void)noexcept(true)=0;

	// ReserveWriteBuffer
	//	reserve write buffer. uncommited buffer content will not be preserved
	// Length		number of elements to query for writing.
	// return array of data buffer for writing
	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn Length)noexcept(true)=0;
	// CommitWriteBuffer
	//	commit data in write buffer. Caller should not access the write buffer after the call
	// Length		length of data to commit, in elements
	// return length of data commited
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Length)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iEndpoint : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnClass::ImplementCastInterface(this,ID);	}

	virtual void cnLib_FUNC Close(void)noexcept(true)=0;
	virtual iReadQueue* cnLib_FUNC GetReadQueue(void)noexcept(true)=0;
	virtual iWriteQueue* cnLib_FUNC GetWriteQueue(void)noexcept(true)=0;
};
//- Stream Error Code -----------------------------------------------------
cnLib_ENUM_BEGIN(uInt8,StreamError)
{
	Success,
	Unknown,
	AccessError,
	Cancelled,
	Closed,
	Timeout,
	MoreData,
}cnLib_ENUM_END(StreamError);
//- Stream Error Report -----------------------------------------------------
class iStreamErrorReport : public iInterface
{
public:

	virtual eStreamError cnLib_FUNC GetStreamError(void)=0;
};
//- Sync Stream -------------------------------------------------------------
class cnLib_INTERFACE iSyncStream : public iInterface
{
public:
	// Close
	virtual void cnLib_FUNC Close(void)noexcept(true)=0;

	// IsEndOfReading
	//	check if read reached the end
	virtual bool cnLib_FUNC IsEndOfReading(void)noexcept(true)=0;
	// SetEndOfStream
	//	set end of write at current ending
	virtual bool cnLib_FUNC SetEndOfStream(void)noexcept(true)=0;

	// Read
	//	read from stream
	// Buffer			[out]		buffer to read into
	// Size							size of buffer
	// SizeCompleted	[out]		total bytes read
	// return true if read successed
	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)=0;
	// Write
	//	write to stream
	// Buffer			[in]		buffer to write
	// Size							size of buffer
	// SizeCompleted	[out]		total bytes written
	// return true if write successed
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)=0;

};
//- Stream Task -------------------------------------------------------------
class cnLib_INTERFACE iStreamTask : public iAsyncTask
{
public:
	// Cancel
	//	cancel the task if it is not done
	virtual void cnLib_FUNC Cancel(void)noexcept(true)=0;

	// GetResult
	//	get operation result
	// SizeCompleted	[out]	size completed
	// return true if successed
	virtual bool cnLib_FUNC GetResult(uIntn &SizeCompleted)noexcept(true)=0;
};
//- Stream ------------------------------------------------------------------
class cnLib_INTERFACE iStream : public iSyncStream
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC  CastInterface(iTypeID ID)noexcept(true) override{		return cnClass::ImplementCastInterface(this,ID);	}
	
		// ReadAsync
	//	read from stream
	// Buffer		[out]	Buffer to read, the buffer must remain valid for the duration of the read operation
	// Size					size of buffer, in bytes
	virtual iPtr<iStreamTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size)noexcept(true)=0;

	// WriteAsync
	//	write to stream
	// Buffer		[in]	Buffer to write, the buffer must remain valid for the duration of the write operation
	// Size					size of buffer, in bytes
	virtual iPtr<iStreamTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size)noexcept(true)=0;

};
//---------------------------------------------------------------------------
}	// namespace cnLibrary
	//---------------------------------------------------------------------------
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#undef	override
#endif
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
