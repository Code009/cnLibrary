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
	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)override;
	virtual iAddress*	cnLib_FUNC GetRemoteAddress(void)override;

	iPtr<iAddress> LocalAddress;
	iPtr<iAddress> RemoteAddress;
};
//---------------------------------------------------------------------------
class bcConnectionQueueProcessor :  protected iAsyncNotificationCallback
{
public:
	bcConnectionQueueProcessor(iConnectionQueue *Queue=nullptr);
	~bcConnectionQueueProcessor();

	iConnectionQueue* GetConnectionQueue(void)const;
	bool SetConnectionQueue(iConnectionQueue *Queue);

	bool IsActive(void)const;

	bool Start(iReference *Reference=nullptr);
	void Stop(void);
protected:

	iPtr<iConnectionQueue> fConnectionQueue;
	bool fQueueActive;
	cAtomicVar<bool> fActiveMutex;
	virtual void cnLib_FUNC AsyncStarted(void)override;
	virtual void cnLib_FUNC AsyncStopped(void)override;
};
//---------------------------------------------------------------------------
class bcConnectionQueue : public iConnectionQueue, protected bcAsyncQueue
{
public:
	bcConnectionQueue();
	~bcConnectionQueue();

	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)override;
	virtual void cnLib_FUNC StopNotify(void)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)override;
	virtual bool cnLib_FUNC IsClosed(void)override;

protected:
	iAsyncNotificationCallback *fConnectionQueueCallback;
	rPtr<iReference> fConnectionQueueReference;

	virtual void NotificationStarted(void)override;
	virtual void NotificationStopped(void)override;
	virtual void AsyncQueueNotify(void)override;
};
//---------------------------------------------------------------------------
class bcConnectionListener : public iConnectionListener
{
public:
	bcConnectionListener();
	~bcConnectionListener();

	virtual void	cnLib_FUNC Close(void)override;
	virtual iPtr<iConnection>		cnLib_FUNC Accept(void)override;
	virtual iPtr<iConnectionTask>	cnLib_FUNC AcceptAsync(void)override;

protected:
	void CloseListener(void);

	virtual void ConnectionProcessTask(void)=0;
	virtual void ConnectionProcessEnd(void)=0;

	void UpdateAcceptTaskQueue(void);

	// accept Task

	class cAcceptTask : public iConnectionTask , public cTaskQueue::cTask
	{
	public:
		
		cAcceptTask()=default;
		~cAcceptTask()=default;
		iPtr<bcConnectionListener> Owner;

		using iConnectionTask::CastInterface;
		//virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)override;
		virtual bool cnLib_FUNC IsDone(void)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;
		virtual void cnLib_FUNC Cancel(void)override;
		virtual iConnection* cnLib_FUNC GetConnection(void)override;

		//eStreamError cnLib_FUNC GetStreamError(void)override;
		StreamError AcceptErrorCode;

		iPtr<iConnection> Connection;
		void PrepareAccept(void);
	protected:
	};


	rPtr<cAcceptTask> QueryAcceptTask(void);
	void CompleteAcceptTask(rPtr<cAcceptTask> Task);
private:
	// task queue

	cTaskQueue fAcceptTaskQueue;

	class cProcessAcceptTaskQueueProcedure : public bcAsyncExclusiveProcedure
	{
		virtual bool Procedure(void)override;
	}fProcessAcceptTaskQueueProcedure;
	rPtr<iAsyncProcedure> fProcessAcceptTaskWork;
	bool ProcessAcceptTaskQueueProc(void);

	bool fListenerClosed;

};
//---------------------------------------------------------------------------

class bcBufferedRWQueue : public bcRWQueue
{
public:
	bcBufferedRWQueue();
	~bcBufferedRWQueue();

	void PutReadData(const void *Data,uIntn Size);
	cMemory QueryReadDataBuffer(uIntn QuerySize);
	void AdvanceReadDataBuffer(uIntn Size);

	cConstMemory QueryWriteData(void);
	void AdvanceWriteData(uIntn Size);
	bool IsWriteDataEnded(void);

protected:

	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)override;
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)override;
	virtual void ReadQueueClosed(void)override;

	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn QuerySize)override;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)override;
	virtual void WriteQueueClosed(void)override;

	virtual void ReadBufferNotify(void)=0;
	virtual void WriteDataNotify(void)=0;

private:
	cAsyncLoopbackStreamBuffer fReadDataQueue;
	cAsyncLoopbackStreamBuffer fWriteDataQueue;
};
//---------------------------------------------------------------------------
struct cGATTTunnelConnectionDeviceID
{
	cGATTUUID ServiceUUID;
	cGATTUUID ReadCharacteristicUUID;
	cGATTUUID WriteCharacteristicUUID;
};
//---------------------------------------------------------------------------
class cGATTTunnelConectionDevice : public iReference, public iAddress, protected iGATTServiceHandler
{
public:
	cGATTTunnelConectionDevice(rPtr<iGATTService> Service,iPtr<iGATTCharacteristic> ReadChar,iPtr<iGATTCharacteristic> WriteChar);
	~cGATTTunnelConectionDevice();

	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}
	// iAddress

	virtual eiOrdering cnLib_FUNC Compare(iAddress *Dest)override;

	cGATTTunnelConnectionDeviceID GetDeviceID()const;
	iGATTService* GetService(void)const;
	iGATTCharacteristic* GetReadCharacteristic(void)const;
	iGATTCharacteristic* GetWriteCharacteristic(void)const;

	class cRWQueue : public bcBufferedRWQueue, public cDualReference
	{
	public:
		cRWQueue(rPtr<cGATTTunnelConectionDevice> Device);
		~cRWQueue();

		cGATTTunnelConectionDevice* GetDevice(void)const;

		using bcBufferedRWQueue::WriteQueueSetEndMode;

	protected:
		void VirtualStarted(void);
		void VirtualStopped(void);


		rPtr<cGATTTunnelConectionDevice> fDevice;
		//void BLEWriteProcedure(void);

		virtual iReference* RWQueueInnerReference(void)override;
		virtual void ReadBufferNotify(void)override;
		virtual void WriteDataNotify(void)override;
	};
	class iConnectCallback
	{
	public:
		virtual void TunnelDeviceOnConnectDone(rPtr<cRWQueue> RWQueue)=0;
		//virtual void TunnelDeviceOnConnectFailed(void)=0;
	};
	bool Connect(iConnectCallback *Callback);
protected:
	rPtr<iGATTService> fService;
	rPtr<iGATTCharacteristic> fReadChar;
	rPtr<iGATTCharacteristic> fWriteChar;
	rPtr<iGATTDescriptor> fReadNotifyDescriptor;

	rPtr<iMutex> fConnectProcedureMutex;
	class cConnectProcedure
	{
	public:
		operator bool ()const;

		void Start(iConnectCallback *Callback);

	protected:
	}fConnectProcedure;

	virtual void cnLib_FUNC GATTServiceStateChanged(void)override;
	virtual void cnLib_FUNC GATTServiceCharacteristListChanged(void)override;

	rInnerPtr<cRWQueue> fRWQueue;
	void RWQueueNotifyWrite(void);

	// connect procedure
	enum{
		csIdle,
		csWaitConnectPerpheral,
		csCheckService,
		csWaitNotifyDescriptor,
		csWaitRegisterNotify,
		csConnected,
	}fConnectState;
	iConnectCallback* fConnectCallback=nullptr;

	void UpdateFunctionState(void);
	void ConnectProcSucceed(void);
	void ConnectProcFailed(void);

	class cReadCharHandler : public iGATTCharacteristHandler
	{
	public:
		virtual void cnLib_FUNC GATTCharacteristStateChanged(void)override;
		virtual void cnLib_FUNC GATTCharacteristDescriptorListChanged(void)override;
		virtual void cnLib_FUNC GATTCharacteristValueNotify(void)override;
	}fReadCharHandler;
	class cWriteCharHandler : public iGATTCharacteristHandler
	{
	public:
		virtual void cnLib_FUNC GATTCharacteristStateChanged(void)override;
		virtual void cnLib_FUNC GATTCharacteristDescriptorListChanged(void)override;
		virtual void cnLib_FUNC GATTCharacteristValueNotify(void)override;
	}fWriteCharHandler;

	void ReadCharValueNotify(void);
};
//---------------------------------------------------------------------------
class cGATTTunnelConectionConnector : public iConnectionConnector
{
public:
	cGATTTunnelConectionConnector();
	~cGATTTunnelConectionConnector();

	virtual iAddress*		cnLib_FUNC GetLocalAddress(void)override;
	virtual iPtr<iConnection>		cnLib_FUNC Connect(iAddress *RemoteAddress)override;
	virtual iPtr<iConnectionTask>	cnLib_FUNC ConnectAsync(iAddress *RemoteAddress)override;

protected:


	class cEndpoint : public iConnection, public iEndpoint
	{
	public:
		cEndpoint(rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue);
		~cEndpoint();

		virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

		// iConnection

		virtual iAddress* cnLib_FUNC GetLocalAddress(void)override;
		virtual iAddress* cnLib_FUNC GetRemoteAddress(void)override;

		// iEndpoint

		virtual void cnLib_FUNC Close(void)override;
		virtual iReadQueue* cnLib_FUNC GetReadQueue(void)override;
		virtual iWriteQueue* cnLib_FUNC GetWriteQueue(void)override;
		virtual void cnLib_FUNC SetWriteEndMode(eEndpointWriteEndMode EndMode)override;

	protected:
		rPtr<cGATTTunnelConectionDevice::cRWQueue> fRWQueue;
	};

	static iPtr<iConnection> CreateConnection(iTypeID ConnectionIID,rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue);
	class cConnectSyncObject : public cThreadOneTimeNotifier, public cGATTTunnelConectionDevice::iConnectCallback
	{
	public:
		//iTypeID ConnectionIID;
		iPtr<iConnection> Connection;
	protected:
		virtual void TunnelDeviceOnConnectDone(rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue)override;

	};
	class cConnectAsyncTask : public iReference,public iConnectionTask, public cGATTTunnelConectionDevice::iConnectCallback
	{
	public:
		using iConnectionTask::CastInterface;

		virtual bool cnLib_FUNC IsDone(void)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;
		
		virtual void cnLib_FUNC Cancel(void)override;
		virtual iConnection* cnLib_FUNC GetConnection(void)override;

		//eStreamError cnLib_FUNC GetStreamError(void)override;
		//StreamError AcceptErrorCode;

		//iTypeID ConnectionIID;
		cAsyncTaskState TaskState;

		void ConnectStart(void);
		void ConnectCancel(void);
	protected:
		iPtr<iConnection> fConnection;

		virtual void TunnelDeviceOnConnectDone(rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue)override;

	};
};
//---------------------------------------------------------------------------
class cGATTTunnelConectionObserver : public bcAsyncNotification, public cDualReference, protected iAsyncNotificationCallback
{
public:
	cGATTTunnelConectionObserver(iGATTPeripheralObserver *Observer);
	~cGATTTunnelConectionObserver();

	cSeqList<cGATTTunnelConnectionDeviceID> DeviceIDs;

	rPtr< iArrayReference< rPtr<cGATTTunnelConectionDevice> > > cnLib_FUNC QueryAllDevices(void);
	rPtr< iArrayReference< rPtr<cGATTTunnelConectionDevice> > > cnLib_FUNC FetchDeviceChanges(void);
	void cnLib_FUNC DiscardChanges(void);

protected:
	void VirtualStarted(void);
	void VirtualStopped(void);

	virtual iReference* NotificationInnerReference(void)override;
	virtual void NotificationStarted(void)override;
	virtual void NotificationStopped(void)override;
	//virtual void NotificationClosed(void)override;
	virtual CycleState NotificationCheckState(void)override;

	rPtr<iGATTPeripheralObserver> fObserver;

	bool fPeripheralObserverError;

	// iAsyncNotificationCallback from iGATTPeripheralObserver

	virtual void cnLib_FUNC AsyncStarted(void)override;
	virtual void cnLib_FUNC AsyncStopped(void)override;
	virtual void cnLib_FUNC AsyncNotify(void)override;

	struct cDeviceItem : cRTLAllocator
	{
		cDeviceItem *Next;
		iPtr<cGATTTunnelConectionDevice> Device;
	};
	cSharedLinkItemRecycler<cDeviceItem> fDeviceItemRecycler;
	cAtomicQueueSO<cDeviceItem> fDeviceQueue;
};
//---------------------------------------------------------------------------
class cGATTTunnelConectionProtocol : public iConnectionProtocol
{
public:
	cGATTTunnelConectionProtocol(rPtr<iGATTPeripheralCentral> Central,rPtr<iGATTPeripheralDevice> Device);
	~cGATTTunnelConectionProtocol();

	// iConnectionProtocol

	virtual iPtr<iConnectionConnector>	cnLib_FUNC CreateStreamConnector(iAddress *LocalAddress,iAddress *RemoteAddress)override;
	virtual iPtr<iConnectionListener>	cnLib_FUNC CreateStreamListener(iAddress *LocalAddress)override;
	virtual iPtr<iConnectionQueue>		cnLib_FUNC CreateStreamConnectionQueue(iAddress *LocalAddress)override;

	virtual iPtr<iConnectionConnector>	cnLib_FUNC CreateEndpointConnector(iAddress *LocalAddress,iAddress *RemoteAddress)override;
	virtual iPtr<iConnectionListener>	cnLib_FUNC CreateEndpointListener(iAddress *LocalAddress)override;
	virtual iPtr<iConnectionQueue>		cnLib_FUNC CreateEndpointConnectionQueue(iAddress *LocalAddress)override;

	// iGATTTunnelConnectionProtocol

	rPtr<cGATTTunnelConectionObserver> CreateObserver(const cGATTTunnelConnectionDeviceID *ServiceIDs,uIntn ServiceCount);

protected:

	rPtr<iGATTPeripheralCentral> fCentral;
	rPtr<iGATTPeripheralDevice> fDevice;

	iPtr<cGATTTunnelConectionConnector> fEndpointConnector;
	iPtr<cGATTTunnelConectionConnector> fStreamConnector;
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
