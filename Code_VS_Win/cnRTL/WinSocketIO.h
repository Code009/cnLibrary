/*- cnRTL - Windows - WinSock IO ------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-07-15                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinSDKHeader.h>

#include <cnRTL\ThreadSynchronization.h>
#include <cnRTL\AsyncTask.h>
#include <cnRTL\StreamAdapter.h>
#include <cnRTL\WinCommon.h>
#include <cnRTL\WinSocketAddress.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
class bcSocketIOHandle : public iReference
{
public:
	bcSocketIOHandle()noexcept(true);
	~bcSocketIOHandle()noexcept(true);

	SOCKET Handle;

	virtual void Close(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class bcNTSocketOverlappedIOHandle : public bcSocketIOHandle
{
public:
	class bcIOObject
	{
	public:
		WSAOVERLAPPED Overlapped;
		DWORD Flags;
		DWORD BytesCompleted;
		DWORD ErrorCode;

		virtual void Completed(void)noexcept(true)=0;
	};

	virtual void StartIO(void)noexcept(true)=0;
	virtual void CancelIO(void)noexcept(true)=0;
	virtual void CancelOperation(bcIOObject *Object)noexcept(true)=0;

};
//---------------------------------------------------------------------------
class cNTSocketOverlappedIOHandleSyncIOObject : public bcNTSocketOverlappedIOHandle::bcIOObject
{
public:
	cNTSocketOverlappedIOHandleSyncIOObject()noexcept(true);
	~cNTSocketOverlappedIOHandleSyncIOObject()noexcept(true);

	bool Recv(bcNTSocketOverlappedIOHandle *SocketIO,void *Buffer,uIntn Size)noexcept(true);
	bool Send(bcNTSocketOverlappedIOHandle *SocketIO,const void *Buffer,uIntn Size)noexcept(true);

	bool RecvFrom(bcNTSocketOverlappedIOHandle *SocketIO,cWin32SocketAddressBuffer &AddressBuffer,void *Buffer,uIntn Size)noexcept(true);
	bool SendTo(bcNTSocketOverlappedIOHandle *SocketIO,const sockaddr *addr,socklen_t addrlen,const void *Buffer,uIntn Size)noexcept(true);

protected:
	cnRTL::cLocalSingleThreadNotification fNotifier;
	virtual void Completed(void)noexcept(true)override;

};
//---------------------------------------------------------------------------
class cNTSocketOverlappedIOHandleStreamAsyncIOTask : public iStreamTask, public iStreamErrorReport, public bcNTSocketOverlappedIOHandle::bcIOObject
{
public:
	cNTSocketOverlappedIOHandleStreamAsyncIOTask()noexcept(true);
	~cNTSocketOverlappedIOHandleStreamAsyncIOTask()noexcept(true);

	bool Recv(bcNTSocketOverlappedIOHandle *SocketIO,void *Buffer,uIntn Size)noexcept(true);
	bool Send(bcNTSocketOverlappedIOHandle *SocketIO,const void *Buffer,uIntn Size)noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

// iAsyncTask

	virtual bool cnLib_FUNC IsDone(void)noexcept(true)override;
	virtual bool cnLib_FUNC Await(iProcedure *NotifyProcedure)noexcept(true)override;

// iSteamTask

	virtual void cnLib_FUNC Cancel(void)noexcept(true)override;
	virtual bool cnLib_FUNC GetResult(uIntn &SizeCompleted)noexcept(true)override;

// iStreamErrorReport
	virtual eStreamError cnLib_FUNC GetStreamError(void)noexcept(true)override;
protected:
	rPtr<bcNTSocketOverlappedIOHandle> fSocketIO;

	virtual void Completed(void)noexcept(true)override;

	void NotifyCompletion(void)noexcept(true);
private:
	cnRTL::cAsyncTaskState fTaskState;
};
//---------------------------------------------------------------------------
class cNTSocketOverlappedIOHandleStream : public iStream, public iStreamErrorReport
{
public:
	// Conn		this should be connect-oriented socket
	cNTSocketOverlappedIOHandleStream(rPtr<bcNTSocketOverlappedIOHandle> SocketIO)noexcept(true);
	~cNTSocketOverlappedIOHandleStream()noexcept(true);
	
	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

	uIntn GetBufferSizeMax(void)noexcept(true);

// iStream

	virtual void cnLib_FUNC Close(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsEndOfReading(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetEndOfStream(void)noexcept(true)override;
	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)override;
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)override;
	virtual iPtr<iStreamTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size)noexcept(true)override;
	virtual iPtr<iStreamTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size)noexcept(true)override;

// iStreamErrorReport

	virtual eStreamError cnLib_FUNC GetStreamError(void)noexcept(true)override;

protected:
	rPtr<bcNTSocketOverlappedIOHandle> fSocketIO;
	bool fEOS;

	bool ShutdownSend(void)noexcept(true);

	class cReadTask : public cNTSocketOverlappedIOHandleStreamAsyncIOTask
	{
	public:
		iPtr<cNTSocketOverlappedIOHandleStream> Host;
		virtual void Completed(void)noexcept(true)override;
	};
};
//---------------------------------------------------------------------------
class cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask : public iMultipointTask, public iStreamErrorReport,public bcNTSocketOverlappedIOHandle::bcIOObject
{
public:
	cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask()noexcept(true);
	~cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask()noexcept(true);

	bool SendTo(bcNTSocketOverlappedIOHandle *SocketIO,iAddress *RemoteAddress,const void *Buffer,uIntn Size)noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

// iAsyncTask

	virtual bool cnLib_FUNC IsDone(void)noexcept(true)override;
	virtual bool cnLib_FUNC Await(iProcedure *NotifyProcedure)noexcept(true)override;

// iSteamTask

	virtual void cnLib_FUNC Cancel(void)noexcept(true)override;
	virtual bool cnLib_FUNC GetResult(uIntn &SizeCompleted)noexcept(true)override;

// iMultipointTask

	virtual iAddress*	cnLib_FUNC GetRemoteAddress(void)noexcept(true)override;

// iStreamErrorReport

	virtual eStreamError cnLib_FUNC GetStreamError(void)noexcept(true)override;
protected:
	rPtr<bcNTSocketOverlappedIOHandle> fSocketIO;
	iPtr<iAddress> fRemoteAddress;
	cWin32SocketAddressBuffer fAddressBuffer;
	socklen_t fAddrLen;
	cnRTL::cAsyncTaskState fTaskState;

	void NotifyCompletion(void)noexcept(true);
	virtual void Completed(void)noexcept(true)override;

};
//---------------------------------------------------------------------------
class cNTSocketOverlappedIOHandleDatagramAsyncIORecvTask : public cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask
{
public:
	bool RecvFrom(bcNTSocketOverlappedIOHandle *SocketIO,iSocketAddress *LocalAddress,void *Buffer,uIntn Size)noexcept(true);
	virtual void Completed(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cNTSocketOverlappedIOHandleDatagramStream : public iMultipointStream, public iStreamErrorReport
{
public:
	// Conn_ref		this should be message socket
	cNTSocketOverlappedIOHandleDatagramStream(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iSocketAddress *LocalAddress)noexcept(true);
	~cNTSocketOverlappedIOHandleDatagramStream()noexcept(true);

	uIntn GetMaxPacketSize(void)noexcept(true);
	iSocketAddress* GetMessagePortAddress(void)noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;
	
	// iMultipointStream

	virtual void cnLib_FUNC Close(void)noexcept(true)override;
	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)noexcept(true)override;

	virtual bool cnLib_FUNC Read(iPtr<iAddress> &Address,void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)override;
	virtual bool cnLib_FUNC Write(iAddress *Address,const void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)override;
	virtual iPtr<iMultipointTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size)noexcept(true)override;
	virtual iPtr<iMultipointTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size,iAddress *Address)noexcept(true)override;

	// iStreamErrorReport

	virtual eStreamError cnLib_FUNC GetStreamError(void)noexcept(true)override;

protected:
	rPtr<bcNTSocketOverlappedIOHandle> fSocketIO;
	iPtr<iSocketAddress> fLocalAddress;

	class cIPv4Multicast
	{
	private:
		cNTSocketOverlappedIOHandleDatagramStream *fHost;
	public:
		cIPv4Multicast(cNTSocketOverlappedIOHandleDatagramStream *Host)noexcept(true);
		~cIPv4Multicast()noexcept(true);
		bool cnLib_FUNC Enter(iAddress *MulticastAddress)noexcept(true);
		bool cnLib_FUNC Leave(iAddress *MulticastAddress)noexcept(true);
	};
};
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
struct cNTSocketOverlappedIOHandleConnectorParameter
{
	ADDRESS_FAMILY AddressFamily;
	iPtr<iSocketAddress> BindAddress;

	bool Setup(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true);
};
//---------------------------------------------------------------------------
struct cNTSocketOverlappedIOHandleListenerParameter
{
	iPtr<iSocketAddress> BoundAddress;
	LPFN_ACCEPTEX AcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockaddrs;

	SOCKET Setup(iAddress *Address,int Type,int Protocol,int Flag)noexcept(true);
};
//---------------------------------------------------------------------------
class bcNTSocketOverlappedIOHandleConnectionConnector : public iConnectionConnector, public cnRTL::cDualReference
{
public:
	bcNTSocketOverlappedIOHandleConnectionConnector(iPtr<iSocketAddress> BindAddress)noexcept(true);
	~bcNTSocketOverlappedIOHandleConnectionConnector()noexcept(true);

	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)noexcept(true)override;
	virtual iPtr<iConnection>			cnLib_FUNC Connect(iAddress *RemoteAddress)noexcept(true)override;
	virtual iPtr<iConnectionTask>		cnLib_FUNC ConnectAsync(iAddress *RemoteAddress)noexcept(true)override;

	class cSyncConnectObject : public bcNTSocketOverlappedIOHandle::bcIOObject
	{
	public:
		cSyncConnectObject()noexcept(true);
		~cSyncConnectObject()noexcept(true);

		bool Connect(LPFN_CONNECTEX fpConnectEx,bcNTSocketOverlappedIOHandle *SocketIO,iSocketAddress *RemoteAddress)noexcept(true);

	protected:
		cnRTL::cLocalSingleThreadNotification fNotifier;

		virtual void Completed(void)noexcept(true)override;

	};

	class cAsyncConnectionTask : public iConnectionTask, public bcNTSocketOverlappedIOHandle::bcIOObject
	{
	public:
		cAsyncConnectionTask(bcNTSocketOverlappedIOHandleConnectionConnector *Owner)noexcept(true);
		~cAsyncConnectionTask()noexcept(true);

		virtual bool cnLib_FUNC IsDone(void)noexcept(true)override;
		virtual bool cnLib_FUNC Await(iProcedure *NotifyProcedure)noexcept(true)override;

		virtual void cnLib_FUNC Cancel(void)noexcept(true)override;

		virtual iConnection* cnLib_FUNC GetConnection(void)noexcept(true)override;

		bool Connect(LPFN_CONNECTEX fpConnectEx,rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iPtr<iSocketAddress> RemoteAddress)noexcept(true);

	protected:
		cnRTL::rInnerPtr<bcNTSocketOverlappedIOHandleConnectionConnector> fOwner;
		rPtr<bcNTSocketOverlappedIOHandle> fSocketIO;
		virtual void Completed(void)noexcept(true)override;
		void NotifyCompletion(void)noexcept(true);

		cnRTL::cAsyncTaskState fTaskState;

		iPtr<iSocketAddress> fRemoteAddress;
		iPtr<iConnection> fConnection;
	};
protected:

	virtual rPtr<bcNTSocketOverlappedIOHandle> ConnectorQueryIOHandle(LPFN_CONNECTEX &fpConnectEx)noexcept(true)=0;
	virtual iPtr<iConnection> ConnectorMakeConnection(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iPtr<iSocketAddress> RemoteAddress)noexcept(true)=0;

	iPtr<iSocketAddress> fBindAddress;
private:
	iPtr<iConnection> MakeConnection(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iPtr<iSocketAddress> RemoteAddress)noexcept(true);
};
//---------------------------------------------------------------------------
class bcNTSocketOverlappedIOHandleConnectionListener : public cnRTL::cConnectionListenerFromAsyncListener::iAsyncListener, public cnRTL::cDualReference
{
public:
	bcNTSocketOverlappedIOHandleConnectionListener(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,cNTSocketOverlappedIOHandleListenerParameter &Parameter)noexcept(true);
	~bcNTSocketOverlappedIOHandleConnectionListener()noexcept(true);

	iPtr<iConnection> Accept(void)noexcept(true);
	virtual iAddress* GetLocalAddress(void)noexcept(true)override;
	virtual iPtr<iConnectionTask> AcceptAsync(void)noexcept(true)override;

	class cSyncAcceptObject : public bcNTSocketOverlappedIOHandle::bcIOObject
	{
	public:
		cSyncAcceptObject()noexcept(true);
		~cSyncAcceptObject()noexcept(true);

		bool Accept(bcNTSocketOverlappedIOHandleConnectionListener *Owner,bcNTSocketOverlappedIOHandle *AcceptSocketIO,void *AddressBuffer,DWORD LocalAddressLength,DWORD RemoteAddressLength)noexcept(true);

	protected:
		cnRTL::cLocalSingleThreadNotification fNotifier;

		virtual void Completed(void)noexcept(true)override;

	};


	class cAsyncAcceptTask : public iConnectionTask, public bcNTSocketOverlappedIOHandle::bcIOObject
	{
	public:
		cAsyncAcceptTask(bcNTSocketOverlappedIOHandleConnectionListener *Owner)noexcept(true);
		~cAsyncAcceptTask()noexcept(true);

		virtual bool cnLib_FUNC IsDone(void)noexcept(true)override;
		virtual bool cnLib_FUNC Await(iProcedure *NotifyProcedure)noexcept(true)override;

		virtual void cnLib_FUNC Cancel(void)noexcept(true)override;

		virtual iConnection* cnLib_FUNC GetConnection(void)noexcept(true)override;

		bool Accept(rPtr<bcNTSocketOverlappedIOHandle> AcceptSocketIO,int LocalAddressLength)noexcept(true);

	protected:
		cnRTL::rInnerPtr<bcNTSocketOverlappedIOHandleConnectionListener> fOwner;
		rPtr<bcNTSocketOverlappedIOHandle> fAcceptSocketIO;
		virtual void Completed(void)noexcept(true)override;
		void NotifyCompletion(void)noexcept(true);

		cnRTL::cAsyncTaskState fTaskState;

		cnRTL::cMemoryBlock fAddressBuffer;
		DWORD fLocalBufferLength;
		DWORD fRemoteBufferLength;

		SOCKET fAcceptSocket;
	
		iPtr<iConnection> fConnection;

	};

protected:

	void VirtualStopped(void)noexcept(true);

	rPtr<bcNTSocketOverlappedIOHandle> fSocketIO;
	LPFN_ACCEPTEX fAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS fGetAcceptExSockaddrs;
	iPtr<iSocketAddress> fLocalAddress;

	virtual rPtr<bcNTSocketOverlappedIOHandle> ListenerQueryIOHandle(void)noexcept(true)=0;
	virtual iPtr<iConnection> ListenerMakeConnection(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress)noexcept(true)=0;

private:
	iPtr<iConnection> MakeAcceptedConnection(rPtr<bcNTSocketOverlappedIOHandle> AcceptSocketIO,void *AddressBuffer,int LocalAddressLength,int RemoteAddressLength)noexcept(true);

};
//---------------------------------------------------------------------------
class bcNTSocketOverlappedIORecyclableHandleManager : public iReference, public cnRTL::cDualReference
{
public:

	bcNTSocketOverlappedIORecyclableHandleManager()noexcept(true);
	~bcNTSocketOverlappedIORecyclableHandleManager()noexcept(true);

protected:
	void VirtualStopped(void)noexcept(true);

	void DisconnectSocket(bcNTSocketOverlappedIOHandle *SocketIO,LPFN_DISCONNECTEX DisconnectEx)noexcept(true);
	virtual void SocketDisconnected(bcNTSocketOverlappedIOHandle *SocketIO)noexcept(true)=0;
	virtual void SocketDisconnectError(bcNTSocketOverlappedIOHandle *SocketIO)noexcept(true)=0;


private:

	cExclusiveFlag fCheckQueueFlag;
	bool fClosed;
	
	void QueueProcedure(void)noexcept(true);

	void QueueProcessDisconnect(void)noexcept(true);
	void QueueProcessCancel(void)noexcept(true);
	void QueueProcessDone(void)noexcept(true);

	class cDisconnectTask : public bcNTSocketOverlappedIOHandle::bcIOObject
	{
	public:
		cDisconnectTask *Next;
		cDisconnectTask(bcNTSocketOverlappedIORecyclableHandleManager *Owner)noexcept(true);
		~cDisconnectTask()noexcept(true);

		bcNTSocketOverlappedIOHandle *SocketIO;
		LPFN_DISCONNECTEX fpDisconnectEx;
		bool Disconnect(void)noexcept(true);

	protected:
		rInnerPtr<bcNTSocketOverlappedIORecyclableHandleManager> fOwner;
		virtual void Completed(void)noexcept(true)override;
	};

	void DisconnectTaskDone(cDisconnectTask *Task)noexcept(true);
	cnRTL::cAtomicQueueSO<cDisconnectTask> fDisconnectQueue;
	cnRTL::cAtomicQueueSO<cDisconnectTask> fDisconnectDoneQueue;
	cnRTL::cSeqSet<cDisconnectTask*> fDisconnectSet;
};
//---------------------------------------------------------------------------
class cNT6ThreadPoolSocketIOHandle : public bcNTSocketOverlappedIOHandle
{
public:
	cNT6ThreadPoolSocketIOHandle()noexcept(true);
	~cNT6ThreadPoolSocketIOHandle()noexcept(true);

	bool Open(SOCKET SocketHandle)noexcept(true);

	virtual void Close(void)noexcept(true)override;
	virtual void StartIO(void)noexcept(true)override;
	virtual void CancelIO(void)noexcept(true)override;
	virtual void CancelOperation(bcIOObject *Object)noexcept(true)override;

protected:
	PTP_IO fIOThreadPool;

	static VOID CALLBACK IoCompletionCallback(
	  _Inout_     PTP_CALLBACK_INSTANCE Instance,
	  _Inout_opt_ PVOID                 Context,
	  _Inout_opt_ PVOID                 Overlapped,
	  _In_        ULONG                 IoResult,
	  _In_        ULONG_PTR             NumberOfBytesTransferred,
	  _Inout_     PTP_IO                Io
	)noexcept(true);
};
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L
typedef iPtr<iConnection> NT6ThreadPoolSocketMakeConnectionFuction(rPtr<cNT6ThreadPoolSocketIOHandle> SocketIO,iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress)noexcept(true);
#else
typedef iPtr<iConnection> NT6ThreadPoolSocketMakeConnectionFuction(rPtr<cNT6ThreadPoolSocketIOHandle> SocketIO,iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress);
#endif
iPtr<iConnection> NT6ThreadPoolSocketMakeStreamConnection(rPtr<cNT6ThreadPoolSocketIOHandle> SocketIO,iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress)noexcept(true);
iPtr<iConnection> NT6ThreadPoolSocketMakeEndpointConnection(rPtr<cNT6ThreadPoolSocketIOHandle> SocketIO,iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress)noexcept(true);
//---------------------------------------------------------------------------
class cNT6ThreadPoolRecyclableSocketIOHandle : public cNT6ThreadPoolSocketIOHandle
{
public:
	bool Open(SOCKET SocketHandle)noexcept(true);

	bool Connected;
	LPFN_DISCONNECTEX fpDisconnectEx;

};
//---------------------------------------------------------------------------
class cNT6ThreadPoolClientSocketIOHandle : public cNT6ThreadPoolRecyclableSocketIOHandle
{
public:
	bool Open(SOCKET SocketHandle)noexcept(true);

	iPtr<iSocketAddress> BoundAddress;
	LPFN_CONNECTEX fpConnectEx;

};
//---------------------------------------------------------------------------
template<class TSocketIOHandle>
class cNT6ThreadPoolRecyclableIOHandleManager
	: public bcNTSocketOverlappedIORecyclableHandleManager
	, public cnClass::bcRecyclableLifeCycleManager< impReferenceRecyclableLifeCycleObject<TSocketIOHandle> >
{
public:
	typedef impReferenceRecyclableLifeCycleObject<TSocketIOHandle> tLifeCycleObject;
	typedef TReferenceObjectLifeCycleActivation<tLifeCycleObject> tLifeCycleActivation;

	cNT6ThreadPoolRecyclableIOHandleManager()noexcept(true){}
	~cNT6ThreadPoolRecyclableIOHandleManager()noexcept(true){
		this->DeleteObjects();
	}


	virtual void Restore(tLifeCycleObject*)noexcept(true) override{
		rIncReference(this,'lcle');
	}
	virtual void Dispose(bcDisposable *Object)noexcept(true) override{
		DisconnectAndRecycleSocket(static_cast<tLifeCycleObject*>(Object));
	}
	void DisconnectAndRecycleSocket(tLifeCycleObject *SocketIO)noexcept(true){
		if(SocketIO->Handle==INVALID_SOCKET){
			// cannot recycle
			delete SocketIO;
			rDecReference(this,'lcle');
			return;
		}
		if(SocketIO->Connected){
			// need disconnect first
			this->DisconnectSocket(SocketIO,SocketIO->fpDisconnectEx);
		}
		else{
			// recycle socket now
			cnClass::bcRecyclableLifeCycleManager<tLifeCycleObject>::Dispose(SocketIO);
			rDecReference(this,'lcle');
		}
	}

	virtual void SocketDisconnected(bcNTSocketOverlappedIOHandle *Object)noexcept(true)override{
		auto LCObject=static_cast<tLifeCycleObject*>(Object);
		LCObject->Connected=false;
		cnClass::bcRecyclableLifeCycleManager<tLifeCycleObject>::Dispose(LCObject);
		rDecReference(this,'lcle');
	}
	virtual void SocketDisconnectError(bcNTSocketOverlappedIOHandle *SocketIO)noexcept(true)override{
		delete static_cast<tLifeCycleObject*>(SocketIO);
		rDecReference(this,'lcle');
	}
};
//---------------------------------------------------------------------------
template<class THandle>
class cNT6ThreadPoolSocketIOHandleRecycler
{
public:
	typedef cNT6ThreadPoolRecyclableIOHandleManager<THandle> tManager;
	cNT6ThreadPoolSocketIOHandleRecycler()noexcept(true) : fManager(rCreate<tManager>()){}
	~cNT6ThreadPoolSocketIOHandleRecycler()noexcept(true){}

	rPtr<THandle> operator ()(void)noexcept(true){	Query();	}
	rPtr<THandle> Query(void)noexcept(true){
		auto Object=fManager->Query();
		tManager::tLifeCycleActivation::Start(Object);
		return rPtr<THandle>::TakeFromManual(Object);
	}
private:
	rPtr<tManager> fManager;
};
//---------------------------------------------------------------------------
class cNT6ThreadPoolSocketConnectionConnector : public bcNTSocketOverlappedIOHandleConnectionConnector
{
public:
	cNT6ThreadPoolSocketConnectionConnector(iPtr<iSocketAddress> BindAddress,NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,int SocketAddressFamily,int SocketType,int SocketProtocol)noexcept(true);
	~cNT6ThreadPoolSocketConnectionConnector()noexcept(true);

protected:
	virtual rPtr<bcNTSocketOverlappedIOHandle> ConnectorQueryIOHandle(LPFN_CONNECTEX &fpConnectEx)noexcept(true)override;
	virtual iPtr<iConnection> ConnectorMakeConnection(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iPtr<iSocketAddress> RemoteAddress)noexcept(true)override;

	NT6ThreadPoolSocketMakeConnectionFuction *fMakeConnectionFunction;
	int fSocketAddressFamily;
	int fSocketType;
	int fSocketProtocol;

	cNT6ThreadPoolSocketIOHandleRecycler<cNT6ThreadPoolClientSocketIOHandle> fSocketHandleRecyclePool;
};
//---------------------------------------------------------------------------
class cNT6ThreadPoolSocketConnectionListener : public bcNTSocketOverlappedIOHandleConnectionListener
{
public:
	cNT6ThreadPoolSocketConnectionListener(rPtr<cNT6ThreadPoolSocketIOHandle> SocketIO,cNTSocketOverlappedIOHandleListenerParameter &Parameter,
		NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,int SocketType,int SocketProtocol)noexcept(true);
	~cNT6ThreadPoolSocketConnectionListener()noexcept(true);

protected:

	NT6ThreadPoolSocketMakeConnectionFuction *fMakeConnectionFunction;
	int fSocketType;
	int fSocketProtocol;
	
	virtual rPtr<bcNTSocketOverlappedIOHandle> ListenerQueryIOHandle(void)noexcept(true)override;
	virtual iPtr<iConnection> ListenerMakeConnection(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress)noexcept(true)override;

	cNT6ThreadPoolSocketIOHandleRecycler<cNT6ThreadPoolRecyclableSocketIOHandle> fSocketHandleRecyclePool;
};
//---------------------------------------------------------------------------
class cNT6ThreadPoolSocketStreamProtocol : public iConnectionProtocol
{
public:
	cNT6ThreadPoolSocketStreamProtocol(int Type,int Protocol)noexcept(true);
	~cNT6ThreadPoolSocketStreamProtocol()noexcept(true);

	virtual iPtr<iConnectionConnector>	cnLib_FUNC CreateStreamConnector(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true)override;
	virtual iPtr<iConnectionListener>	cnLib_FUNC CreateStreamListener(iAddress *LocalAddress)noexcept(true)override;

	virtual iPtr<iConnectionConnector>	cnLib_FUNC CreateEndpointConnector(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true)override;
	virtual iPtr<iConnectionListener>	cnLib_FUNC CreateEndpointListener(iAddress *LocalAddress)noexcept(true)override;

	iPtr<iConnectionConnector> MakeConnector(NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true);
	iPtr<iConnectionListener> MakeListener(NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,iAddress *LocalAddress)noexcept(true);

protected:
	int fType;
	int fProtocol;
};
//---------------------------------------------------------------------------
class cNT6ThreadPoolSocketDatagramProtocol : public iDatagramProtocol
{
public:
	cNT6ThreadPoolSocketDatagramProtocol(int Type,int Protocol)noexcept(true);
	~cNT6ThreadPoolSocketDatagramProtocol()noexcept(true);

	virtual iPtr<iConnection>		cnLib_FUNC CreateStreamPair(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true)override;
	virtual iPtr<iConnection>		cnLib_FUNC CreateEndpointPair(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true)override;
	virtual iPtr<iMultipointStream>	cnLib_FUNC OpenStream(iAddress *LocalAddress)noexcept(true)override;
	virtual iPtr<iMultipointQueue>	cnLib_FUNC OpenQueue(iAddress *LocalAddress)noexcept(true)override;
protected:
	int fType;
	int fProtocol;

	iPtr<iConnection> MakePair(NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true);

	bool SetupAddress(SOCKET s,iSocketAddress *Address)noexcept(true);
	iPtr<iMultipointStream> MakeDatagramStream(iAddress *LocalAddress)noexcept(true);
};
//---------------------------------------------------------------------------
#endif	// >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
