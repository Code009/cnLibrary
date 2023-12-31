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
namespace cnLibrary{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAsyncNotificationCallback
{
public:
	// AsyncStarted
	//	called before iAsyncNotification start using this interface
	virtual void cnLib_FUNC AsyncStarted(void)=0;
	// AsyncStopped
	//	called after iAsyncNotification finished using the interface
	virtual void cnLib_FUNC AsyncStopped(void)=0;

	// AsyncNotify
	//	called when user requested an notification.
	//	when the function is called, notification signal will be cleared, and user need to call NotifyCallback again for another notification
	virtual void cnLib_FUNC AsyncNotify(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAsyncNotification : public iReference
{
public:
	// StartNotify
	//	start notify and calls callback AsyncStarted
	// Callback		callback
	// return true if successful registered callback
	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)=0;
	// StopNotify
	//	stop notify, call AsyncStopped after all running notification is finished
	virtual void cnLib_FUNC StopNotify(void)=0;
	// NotifyCallback
	//	request a notify to callback. the iAsyncNotification will call callback asynchronously if notification signal arrived or user requested an idle notification
	//	IdleNotify		if true, the callback will be called even when no pending notification signals. if false, the callback will be called on when there is notification signal
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)=0;

	// IsClosed
	//	check if notification is closed
	virtual bool cnLib_FUNC IsClosed(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAsyncQueueCallback
{
public:
	virtual void cnLib_FUNC QueueStarted(void)=0;
	virtual void cnLib_FUNC QueueStopped(void)=0;
	virtual void cnLib_FUNC QueueNotify(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAsyncQueue : public iInterface
{
public:
	virtual bool cnLib_FUNC StartNotify(iAsyncQueueCallback *Callback)=0;
	virtual void cnLib_FUNC StopNotify(void)=0;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)=0;
	virtual bool cnLib_FUNC IsClosed(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iReadQueueCallback
{
public:
	// ReadStarted
	//	called before iReadQueue using the interface
	virtual void cnLib_FUNC ReadStarted(void)=0;
	// ReadStopped
	//	called after iReadQueue done using the interface
	virtual void cnLib_FUNC ReadStopped(void)=0;

	// ReadNotify
	//	called when iReadQueue has buffer available for reading , or when user called NotifyRead
	virtual void cnLib_FUNC ReadNotify(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iReadQueue : public iReference
{
public:
	// StartRead
	//	start reading from the data queue
	// Callback		callback
	// return true if it start reading
	virtual bool cnLib_FUNC StartRead(iReference *Reference,iReadQueueCallback *Callback)=0;
	// StopRead
	//	stop reading
	virtual void cnLib_FUNC StopRead(void)=0;
	// NotifyRead
	//	notify callback at least once after the call, if there are more data available in read buffer since last notification
	//	IdleNotify		notify even when pending buffer is empty
	virtual void cnLib_FUNC NotifyRead(uIntn SizeToNotify)=0;

	virtual bool cnLib_FUNC IsReadClosed(bool &GracefulClose)=0;
	
	//virtual bool cnLib_FUNC IsReadBufferFull(void)=0;

	// GatherReadBuffer
	//	gather read data to buffer
	// Length			number of elements to gather for reading.
	// return array of data buffer for reading
	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Length)=0;
	// DismissReadBuffer
	//	dismiss length of read data . Caller should not access returned read buffer after the call
	// Length		length of data to dismiss, in elements
	// return length of data dismissed
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Length)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWriteQueueCallback
{
public:
	// WriteStarted
	//	called before iWriteQueue using the interface and before StartPull returns
	virtual void cnLib_FUNC WriteStarted(void)=0;
	// WriteStopped
	//	called after iWriteQueue done using the interface
	virtual void cnLib_FUNC WriteStopped(void)=0;

	// WriteNotify
	//	called when iWriteQueue has buffer available for writing, or when user called NotifyWrite
	virtual void cnLib_FUNC WriteNotify(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWriteQueue : public iReference
{
public:
	// StartWrite
	//	start writing to the data queue
	// Callback		callback
	// return true if it start writing
	virtual bool cnLib_FUNC StartWrite(iReference *Reference,iWriteQueueCallback *Callback)=0;
	// StopWrite
	//	stop writing
	// Terminate		wether to discard pending buffer
	virtual void cnLib_FUNC StopWrite(bool Terminate)=0;
	// NotifyWrite
	//	notify callback at least once after the call, if there are more space available in write buffer since last notification
	//	IdleNotify		notify even when write buffer is full
	virtual void cnLib_FUNC NotifyWrite(uIntn SizeToNotify)=0;

	virtual bool cnLib_FUNC IsWriteClosed(bool &GracefulClose)=0;
	
	//virtual bool cnLib_FUNC IsWriteBufferFull(void)=0;

	// ReserveWriteBuffer
	//	reserve write buffer. uncommited buffer content will not be preserved
	// Length		number of elements to query for writing.
	//				the function is not necessarily return exactly same length of buffer, this length is a hint for the buffer object to optimatialze buffer allocation
	// return array of data buffer for writing
	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn Length)=0;
	// CommitWriteBuffer
	//	commit data in write buffer. Caller should not access the write buffer after the call
	// Length		length of data to commit, in elements
	// return length of data commited
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Length)=0;
};
//---------------------------------------------------------------------------
// EndpointWriteEndMode
//	default mode is NextSession
cnLib_ENUM_BEGIN(ufInt8,EndpointWriteEndMode)
{
	Never,			// never mark stream end automaticly, stream will be ended when the WriteQueue released without any termination
	Idle,			// mark stream end when no session active
	NextSession,	// mark stream end after next session stopped, this is the default mode
}cnLib_ENUM_END(EndpointWriteEndMode);
//---------------------------------------------------------------------------
class cnLib_INTERFACE iEndpoint : public iInterface
{
public:
	cnLib_INTERFACE_DEFINE(iEndpoint,iInterface)

	virtual void cnLib_FUNC Close(void)=0;
	virtual iReadQueue* cnLib_FUNC GetReadQueue(void)=0;
	virtual iWriteQueue* cnLib_FUNC GetWriteQueue(void)=0;
	// SetWriteEndMode
	//	mark the write stream as ended when conditions are met
	// EndMode			mode that when to ends the write stream
	virtual void cnLib_FUNC SetWriteEndMode(eEndpointWriteEndMode EndMode)=0;
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
	cnLib_INTERFACE_DEFINE(iStreamErrorReport,iInterface)

	virtual eStreamError cnLib_FUNC GetStreamError(void)=0;
};
//- Sync Stream -------------------------------------------------------------
class cnLib_INTERFACE iSyncStream : public iInterface
{
public:
	// Close
	virtual void cnLib_FUNC Close(void)=0;

	// IsEndOfReading
	//	check if read reached the end
	virtual bool cnLib_FUNC IsEndOfReading(void)=0;
	// SetEndOfStream
	//	set end of write at current ending
	virtual bool cnLib_FUNC SetEndOfStream(void)=0;

	// Read
	//	read from stream
	// Buffer			[out]		buffer to read into
	// Size							size of buffer
	// SizeCompleted	[out]		total bytes read
	// return true if read successed
	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)=0;
	// Write
	//	write to stream
	// Buffer			[in]		buffer to write
	// Size							size of buffer
	// SizeCompleted	[out]		total bytes written
	// return true if write successed
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)=0;

};
//- Stream Task -------------------------------------------------------------
class cnLib_INTERFACE iStreamTask : public iAsyncTask
{
public:
	// Cancel
	//	cancel the task if it is not done
	virtual void cnLib_FUNC Cancel(void)=0;

	// GetResult
	//	get operation result
	// SizeCompleted	[out]	size completed
	// return true if successed
	virtual bool cnLib_FUNC GetResult(uIntn &SizeCompleted)=0;
};
//- Stream ------------------------------------------------------------------
class cnLib_INTERFACE iStream : public iSyncStream
{
public:
	cnLib_INTERFACE_DEFINE(iStream,iSyncStream)
	
		// ReadAsync
	//	read from stream
	// Buffer		[out]	Buffer to read, the buffer must remain valid for the duration of the read operation
	// Size					size of buffer, in bytes
	virtual iPtr<iStreamTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size)=0;

	// WriteAsync
	//	write to stream
	// Buffer		[in]	Buffer to write, the buffer must remain valid for the duration of the write operation
	// Size					size of buffer, in bytes
	virtual iPtr<iStreamTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size)=0;

};
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
