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
	bcBufferedRWQueue()noexcept(true);
	~bcBufferedRWQueue()noexcept(true);

	void PutReadData(const void *Data,uIntn Size)noexcept(true);
	cMemory QueryReadDataBuffer(uIntn QuerySize)noexcept(true);
	void AdvanceReadDataBuffer(uIntn Size)noexcept(true);

	cConstMemory QueryWriteData(void)noexcept(true);
	void AdvanceWriteData(uIntn Size)noexcept(true);
	bool IsWriteDataEnded(void)noexcept(true);

protected:

	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)noexcept(true)override;
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)noexcept(true)override;
	virtual void ReadQueueClosed(void)noexcept(true)override;

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
	cGATTTunnelConectionDevice(rPtr<iGATTService> Service,rPtr<iGATTCharacteristic> ReadChar,rPtr<iGATTCharacteristic> WriteChar)noexcept(true);
	~cGATTTunnelConectionDevice()noexcept(true);

	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}
	// iAddress

	virtual eiOrdering cnLib_FUNC Compare(iAddress *Dest)noexcept(true)override;

	cGATTTunnelConnectionDeviceID GetDeviceID()const noexcept(true);
	iGATTService* GetService(void)const noexcept(true);
	iGATTCharacteristic* GetReadCharacteristic(void)const noexcept(true);
	iGATTCharacteristic* GetWriteCharacteristic(void)const noexcept(true);

	class cRWQueue : public bcBufferedRWQueue, public cDualReference
	{
	public:
		cRWQueue(rPtr<cGATTTunnelConectionDevice> Device)noexcept(true);
		~cRWQueue()noexcept(true);

		cGATTTunnelConectionDevice* GetDevice(void)const noexcept(true);

		using bcBufferedRWQueue::WriteQueueSetEndMode;

	protected:
		void VirtualStarted(void)noexcept(true);
		void VirtualStopped(void)noexcept(true);


		rPtr<cGATTTunnelConectionDevice> fDevice;
		//void BLEWriteProcedure(void);

		virtual iReference* RWQueueInnerReference(void)noexcept(true)override;
		virtual void ReadBufferNotify(void)noexcept(true)override;
		virtual void WriteDataNotify(void)noexcept(true)override;
	};
	class iConnectCallback
	{
	public:
		virtual void TunnelDeviceOnConnectDone(rPtr<cRWQueue> RWQueue)noexcept(true)=0;
		//virtual void TunnelDeviceOnConnectFailed(void)noexcept(true)=0;
	};
	bool Connect(iConnectCallback *Callback)noexcept(true);
protected:
	rPtr<iGATTService> fService;
	rPtr<iGATTCharacteristic> fReadChar;
	rPtr<iGATTCharacteristic> fWriteChar;
	rPtr<iGATTDescriptor> fReadNotifyDescriptor;

	rPtr<iMutex> fConnectProcedureMutex;
	class cConnectProcedure
	{
	public:
		operator bool ()const noexcept(true);

		void Start(iConnectCallback *Callback)noexcept(true);

	protected:
	}fConnectProcedure;

	virtual void cnLib_FUNC GATTServiceStateChanged(void)noexcept(true)override;

	rInnerPtr<cRWQueue> fRWQueue;
	void RWQueueNotifyWrite(void)noexcept(true);

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

	void UpdateFunctionState(void)noexcept(true);
	void ConnectProcSucceed(void)noexcept(true);
	void ConnectProcFailed(void)noexcept(true);

	class cReadCharHandler : public iGATTCharacteristicHandler
	{
	public:
		virtual void cnLib_FUNC GATTCharacteristStateChanged(void)noexcept(true)override;
		virtual void cnLib_FUNC GATTCharacteristValueNotify(const void *Data,uIntn DataSize)noexcept(true)override;
	}fReadCharHandler;
	class cWriteCharHandler : public iGATTCharacteristicHandler
	{
	public:
		virtual void cnLib_FUNC GATTCharacteristStateChanged(void)noexcept(true)override;
		virtual void cnLib_FUNC GATTCharacteristValueNotify(const void *Data,uIntn DataSize)noexcept(true)override;
	}fWriteCharHandler;

	void ReadCharValueNotify(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cGATTTunnelConectionConnector : public iConnectionConnector
{
public:
	cGATTTunnelConectionConnector()noexcept(true);
	~cGATTTunnelConectionConnector()noexcept(true);

	virtual iAddress*		cnLib_FUNC GetLocalAddress(void)noexcept(true)override;
	virtual iPtr<iConnection>		cnLib_FUNC Connect(iAddress *RemoteAddress)noexcept(true)override;
	virtual iPtr<iConnectionTask>	cnLib_FUNC ConnectAsync(iAddress *RemoteAddress)noexcept(true)override;

protected:


	class cEndpoint : public iConnection, public iEndpoint
	{
	public:
		cEndpoint(rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue)noexcept(true);
		~cEndpoint()noexcept(true);

		virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

		// iConnection

		virtual iAddress* cnLib_FUNC GetLocalAddress(void)noexcept(true)override;
		virtual iAddress* cnLib_FUNC GetRemoteAddress(void)noexcept(true)override;

		// iEndpoint

		virtual void cnLib_FUNC Close(void)noexcept(true)override;
		virtual iReadQueue* cnLib_FUNC GetReadQueue(void)noexcept(true)override;
		virtual iWriteQueue* cnLib_FUNC GetWriteQueue(void)noexcept(true)override;
		virtual void cnLib_FUNC SetWriteEndMode(eEndpointWriteEndMode EndMode)noexcept(true)override;

	protected:
		rPtr<cGATTTunnelConectionDevice::cRWQueue> fRWQueue;
	};

	static iPtr<iConnection> CreateConnection(iTypeID ConnectionIID,rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue)noexcept(true);
	class cConnectSyncObject : public cThreadOneTimeNotifier, public cGATTTunnelConectionDevice::iConnectCallback
	{
	public:
		//iTypeID ConnectionIID;
		iPtr<iConnection> Connection;
	protected:
		virtual void TunnelDeviceOnConnectDone(rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue)noexcept(true)override;

	};
	class cConnectAsyncTask : public iReference,public iConnectionTask, public cGATTTunnelConectionDevice::iConnectCallback
	{
	public:
		using iConnectionTask::CastInterface;

		virtual bool cnLib_FUNC IsDone(void)noexcept(true)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)noexcept(true)override;
		
		virtual void cnLib_FUNC Cancel(void)noexcept(true)override;
		virtual iConnection* cnLib_FUNC GetConnection(void)noexcept(true)override;

		//eStreamError cnLib_FUNC GetStreamError(void)override;
		//StreamError AcceptErrorCode;

		//iTypeID ConnectionIID;
		cAsyncTaskState TaskState;

		void ConnectStart(void)noexcept(true);
		void ConnectCancel(void)noexcept(true);
	protected:
		iPtr<iConnection> fConnection;

		virtual void TunnelDeviceOnConnectDone(rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue)noexcept(true)override;

	};
};
//---------------------------------------------------------------------------
class cGATTTunnelConectionObserver : public bcAsyncNotification, public cDualReference, protected iAsyncNotificationCallback
{
public:
	cGATTTunnelConectionObserver(iGATTAdvertisementObserver *Observer)noexcept(true);
	~cGATTTunnelConectionObserver()noexcept(true);

	cSeqList<cGATTTunnelConnectionDeviceID> DeviceIDs;

	rPtr< iArrayReference< rPtr<cGATTTunnelConectionDevice> > > cnLib_FUNC QueryAllDevices(void)noexcept(true);
	rPtr< iArrayReference< rPtr<cGATTTunnelConectionDevice> > > cnLib_FUNC FetchDeviceChanges(void)noexcept(true);
	void cnLib_FUNC DiscardChanges(void)noexcept(true);

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	virtual iReference* NotificationInnerReference(void)noexcept(true)override;
	virtual void NotificationStarted(void)noexcept(true)override;
	virtual void NotificationStopped(void)noexcept(true)override;
	//virtual void NotificationClosed(void)noexcept(true)override;
	virtual CycleState NotificationCheckState(void)noexcept(true)override;

	rPtr<iGATTAdvertisementObserver> fObserver;

	bool fPeripheralObserverError;

	// iAsyncNotificationCallback from iGATTPeripheralObserver

	virtual void cnLib_FUNC AsyncStarted(void)noexcept(true)override;
	virtual void cnLib_FUNC AsyncStopped(void)noexcept(true)override;
	virtual void cnLib_FUNC AsyncNotify(void)noexcept(true)override;

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
	cGATTTunnelConectionProtocol(rPtr<iGATTPeripheralCentral> Central)noexcept(true);
	~cGATTTunnelConectionProtocol()noexcept(true);

	// iConnectionProtocol

	virtual iPtr<iConnectionConnector>	cnLib_FUNC CreateStreamConnector(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true)override;
	virtual iPtr<iConnectionListener>	cnLib_FUNC CreateStreamListener(iAddress *LocalAddress)noexcept(true)override;

	virtual iPtr<iConnectionConnector>	cnLib_FUNC CreateEndpointConnector(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true)override;
	virtual iPtr<iConnectionListener>	cnLib_FUNC CreateEndpointListener(iAddress *LocalAddress)noexcept(true)override;

	// iGATTTunnelConnectionProtocol

	rPtr<cGATTTunnelConectionObserver> CreateObserver(const cGATTTunnelConnectionDeviceID *ServiceIDs,uIntn ServiceCount)noexcept(true);

protected:

	rPtr<iGATTPeripheralCentral> fCentral;

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
