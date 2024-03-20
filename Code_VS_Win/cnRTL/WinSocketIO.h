/*- cnRTL - Windows - WinSock IO ------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-07-15                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinSDKHeader.h>

#include <cnRTL\ThreadSynchronization.h>
#include <cnRTL\AsyncTask.h>
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
	bcSocketIOHandle();
	~bcSocketIOHandle();

	SOCKET Handle;

	virtual void Close(void)=0;
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

		virtual void Completed(void)=0;
	};

	virtual void StartIO(void)=0;
	virtual void CancelIO(void)=0;
	virtual void CancelOperation(bcIOObject *Object)=0;

};
//---------------------------------------------------------------------------
class cNTSocketOverlappedIOHandleSyncIOObject : public bcNTSocketOverlappedIOHandle::bcIOObject
{
public:
	cNTSocketOverlappedIOHandleSyncIOObject();
	~cNTSocketOverlappedIOHandleSyncIOObject();

	bool Recv(bcNTSocketOverlappedIOHandle *SocketIO,void *Buffer,uIntn Size);
	bool Send(bcNTSocketOverlappedIOHandle *SocketIO,const void *Buffer,uIntn Size);

	bool RecvFrom(bcNTSocketOverlappedIOHandle *SocketIO,cWin32SocketAddressBuffer &AddressBuffer,void *Buffer,uIntn Size);
	bool SendTo(bcNTSocketOverlappedIOHandle *SocketIO,const sockaddr *addr,socklen_t addrlen,const void *Buffer,uIntn Size);

protected:
	cnRTL::cThreadOneTimeNotifier fNotifier;
	virtual void Completed(void)override;

};
//---------------------------------------------------------------------------
class cNTSocketOverlappedIOHandleStreamAsyncIOTask : public iStreamTask, public iStreamErrorReport, public bcNTSocketOverlappedIOHandle::bcIOObject
{
public:
	cNTSocketOverlappedIOHandleStreamAsyncIOTask();
	~cNTSocketOverlappedIOHandleStreamAsyncIOTask();

	bool Recv(bcNTSocketOverlappedIOHandle *SocketIO,void *Buffer,uIntn Size);
	bool Send(bcNTSocketOverlappedIOHandle *SocketIO,const void *Buffer,uIntn Size);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

// iAsyncTask

	virtual bool cnLib_FUNC IsDone(void)override;
	virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;

// iSteamTask

	virtual void cnLib_FUNC Cancel(void)override;
	virtual bool cnLib_FUNC GetResult(uIntn &SizeCompleted)override;

// iStreamErrorReport
	virtual eStreamError cnLib_FUNC GetStreamError(void)override;
protected:
	rPtr<bcNTSocketOverlappedIOHandle> fSocketIO;

	virtual void Completed(void)override;

	void NotifyCompletion(void);
private:
	cnRTL::cAsyncTaskState fTaskState;
};
//---------------------------------------------------------------------------
class cNTSocketOverlappedIOHandleStream : public iStream, public iStreamErrorReport
{
public:
	// Conn		this should be connect-oriented socket
	cNTSocketOverlappedIOHandleStream(rPtr<bcNTSocketOverlappedIOHandle> SocketIO);
	~cNTSocketOverlappedIOHandleStream();
	
	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

	uIntn GetBufferSizeMax(void);

// iStream

	virtual void cnLib_FUNC Close(void)override;
	virtual bool cnLib_FUNC IsEndOfReading(void)override;
	virtual bool cnLib_FUNC SetEndOfStream(void)override;
	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)override;
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)override;
	virtual iPtr<iStreamTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size)override;
	virtual iPtr<iStreamTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size)override;

// iStreamErrorReport

	virtual eStreamError cnLib_FUNC GetStreamError(void)override;

protected:
	rPtr<bcNTSocketOverlappedIOHandle> fSocketIO;
	bool fEOS;

	bool ShutdownSend(void);

	class cReadTask : public cNTSocketOverlappedIOHandleStreamAsyncIOTask
	{
	public:
		iPtr<cNTSocketOverlappedIOHandleStream> Host;
		virtual void Completed(void)override;
	};
};
//---------------------------------------------------------------------------
class cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask : public iMultipointTask, public iStreamErrorReport,public bcNTSocketOverlappedIOHandle::bcIOObject
{
public:
	cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask();
	~cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask();

	bool SendTo(bcNTSocketOverlappedIOHandle *SocketIO,iAddress *RemoteAddress,const void *Buffer,uIntn Size);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

// iAsyncTask

	virtual bool cnLib_FUNC IsDone(void)override;
	virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;

// iSteamTask

	virtual void cnLib_FUNC Cancel(void)override;
	virtual bool cnLib_FUNC GetResult(uIntn &SizeCompleted)override;

// iMultipointTask

	virtual iAddress*	cnLib_FUNC GetRemoteAddress(void)override;

// iStreamErrorReport

	virtual eStreamError cnLib_FUNC GetStreamError(void)override;
protected:
	rPtr<bcNTSocketOverlappedIOHandle> fSocketIO;
	iPtr<iAddress> fRemoteAddress;
	cWin32SocketAddressBuffer fAddressBuffer;
	socklen_t fAddrLen;
	cnRTL::cAsyncTaskState fTaskState;

	void NotifyCompletion(void);
	virtual void Completed(void)override;

};
//---------------------------------------------------------------------------
class cNTSocketOverlappedIOHandleDatagramAsyncIORecvTask : public cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask
{
public:
	bool RecvFrom(bcNTSocketOverlappedIOHandle *SocketIO,iSocketAddress *LocalAddress,void *Buffer,uIntn Size);
	virtual void Completed(void)override;
};
//---------------------------------------------------------------------------
class cNTSocketOverlappedIOHandleDatagramStream : public iMultipointStream, public iStreamErrorReport
{
public:
	// Conn_ref		this should be message socket
	cNTSocketOverlappedIOHandleDatagramStream(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iSocketAddress *LocalAddress);
	~cNTSocketOverlappedIOHandleDatagramStream();

	uIntn GetMaxPacketSize(void);
	iSocketAddress* GetMessagePortAddress(void);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;
	
	// iMultipointStream

	virtual void cnLib_FUNC Close(void)override;
	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)override;

	virtual bool cnLib_FUNC Read(iPtr<iAddress> &Address,void *Buffer,uIntn Size,uIntn &SizeCompleted)override;
	virtual bool cnLib_FUNC Write(iAddress *Address,const void *Buffer,uIntn Size,uIntn &SizeCompleted)override;
	virtual iPtr<iMultipointTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size)override;
	virtual iPtr<iMultipointTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size,iAddress *Address)override;

	// iStreamErrorReport

	virtual eStreamError cnLib_FUNC GetStreamError(void)override;

protected:
	rPtr<bcNTSocketOverlappedIOHandle> fSocketIO;
	iPtr<iSocketAddress> fLocalAddress;

	class cIPv4Multicast
	{
	private:
		cNTSocketOverlappedIOHandleDatagramStream *fHost;
	public:
		cIPv4Multicast(cNTSocketOverlappedIOHandleDatagramStream *Host);
		~cIPv4Multicast();
		bool cnLib_FUNC Enter(iAddress *MulticastAddress);
		bool cnLib_FUNC Leave(iAddress *MulticastAddress);
	};
};
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
struct cNTSocketOverlappedIOHandleConnectorParameter
{
	ADDRESS_FAMILY AddressFamily;
	iPtr<iSocketAddress> BindAddress;

	bool Setup(iAddress *LocalAddress,iAddress *RemoteAddress);
};
//---------------------------------------------------------------------------
struct cNTSocketOverlappedIOHandleListenerParameter
{
	iPtr<iSocketAddress> BoundAddress;
	LPFN_ACCEPTEX AcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockaddrs;

	SOCKET Setup(iAddress *Address,int Type,int Protocol,int Flag);
};
//---------------------------------------------------------------------------
class bcNTSocketOverlappedIOHandleConnectionConnector : public iConnectionConnector, public cnRTL::cDualReference
{
public:
	bcNTSocketOverlappedIOHandleConnectionConnector(iPtr<iSocketAddress> BindAddress);
	~bcNTSocketOverlappedIOHandleConnectionConnector();

	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)override;
	virtual iPtr<iConnection>			cnLib_FUNC Connect(iAddress *RemoteAddress)override;
	virtual iPtr<iConnectionTask>		cnLib_FUNC ConnectAsync(iAddress *RemoteAddress)override;

	class cSyncConnectObject : public bcNTSocketOverlappedIOHandle::bcIOObject
	{
	public:
		cSyncConnectObject();
		~cSyncConnectObject();

		bool Connect(LPFN_CONNECTEX fpConnectEx,bcNTSocketOverlappedIOHandle *SocketIO,iSocketAddress *RemoteAddress);

	protected:
		cnRTL::cThreadOneTimeNotifier fNotifier;

		virtual void Completed(void)override;

	};

	class cAsyncConnectionTask : public iConnectionTask, public bcNTSocketOverlappedIOHandle::bcIOObject
	{
	public:
		cAsyncConnectionTask(bcNTSocketOverlappedIOHandleConnectionConnector *Owner);
		~cAsyncConnectionTask();

		virtual bool cnLib_FUNC IsDone(void)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;

		virtual void cnLib_FUNC Cancel(void)override;

		virtual iConnection* cnLib_FUNC GetConnection(void)override;

		bool Connect(LPFN_CONNECTEX fpConnectEx,rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iPtr<iSocketAddress> RemoteAddress);

	protected:
		cnRTL::rInnerPtr<bcNTSocketOverlappedIOHandleConnectionConnector> fOwner;
		rPtr<bcNTSocketOverlappedIOHandle> fSocketIO;
		virtual void Completed(void)override;
		void NotifyCompletion(void);

		cnRTL::cAsyncTaskState fTaskState;

		iPtr<iSocketAddress> fRemoteAddress;
		iPtr<iConnection> fConnection;
	};
protected:

	virtual rPtr<bcNTSocketOverlappedIOHandle> ConnectorQueryIOHandle(LPFN_CONNECTEX &fpConnectEx)=0;
	virtual iPtr<iConnection> ConnectorMakeConnection(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iPtr<iSocketAddress> RemoteAddress)=0;

	iPtr<iSocketAddress> fBindAddress;
private:
	iPtr<iConnection> MakeConnection(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iPtr<iSocketAddress> RemoteAddress);
};
//---------------------------------------------------------------------------
class bcNTSocketOverlappedIOHandleConnectionListener : public iConnectionListener, public cnRTL::cDualReference
{
public:
	bcNTSocketOverlappedIOHandleConnectionListener(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,cNTSocketOverlappedIOHandleListenerParameter &Parameter);
	~bcNTSocketOverlappedIOHandleConnectionListener();

	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)override;
	virtual void	cnLib_FUNC Close(void)override;
	virtual iPtr<iConnection> cnLib_FUNC Accept(void)override;
	virtual iPtr<iConnectionTask> cnLib_FUNC AcceptAsync(void)override;

	class cSyncAcceptObject : public bcNTSocketOverlappedIOHandle::bcIOObject
	{
	public:
		cSyncAcceptObject();
		~cSyncAcceptObject();

		bool Accept(bcNTSocketOverlappedIOHandleConnectionListener *Owner,bcNTSocketOverlappedIOHandle *AcceptSocketIO,void *AddressBuffer,DWORD LocalAddressLength,DWORD RemoteAddressLength);

	protected:
		cnRTL::cThreadOneTimeNotifier fNotifier;

		virtual void Completed(void)override;

	};


	class cAsyncAcceptTask : public iConnectionTask, public bcNTSocketOverlappedIOHandle::bcIOObject
	{
	public:
		cAsyncAcceptTask(bcNTSocketOverlappedIOHandleConnectionListener *Owner);
		~cAsyncAcceptTask();

		virtual bool cnLib_FUNC IsDone(void)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;

		virtual void cnLib_FUNC Cancel(void)override;

		virtual iConnection* cnLib_FUNC GetConnection(void)override;

		bool Accept(rPtr<bcNTSocketOverlappedIOHandle> AcceptSocketIO,int LocalAddressLength);

	protected:
		cnRTL::rInnerPtr<bcNTSocketOverlappedIOHandleConnectionListener> fOwner;
		rPtr<bcNTSocketOverlappedIOHandle> fAcceptSocketIO;
		virtual void Completed(void)override;
		void NotifyCompletion(void);

		cnRTL::cAsyncTaskState fTaskState;

		cnRTL::cMemoryBlock fAddressBuffer;
		DWORD fLocalBufferLength;
		DWORD fRemoteBufferLength;

		SOCKET fAcceptSocket;
	
		iPtr<iConnection> fConnection;

	};

protected:

	void VirtualStopped(void);

	rPtr<bcNTSocketOverlappedIOHandle> fSocketIO;
	LPFN_ACCEPTEX fAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS fGetAcceptExSockaddrs;
	iPtr<iSocketAddress> fLocalAddress;

	virtual rPtr<bcNTSocketOverlappedIOHandle> ListenerQueryIOHandle(void)=0;
	virtual iPtr<iConnection> ListenerMakeConnection(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress)=0;

private:
	iPtr<iConnection> MakeAcceptedConnection(rPtr<bcNTSocketOverlappedIOHandle> AcceptSocketIO,void *AddressBuffer,int LocalAddressLength,int RemoteAddressLength);

};
//---------------------------------------------------------------------------
class bcNTSocketOverlappedIORecyclableHandleManager : public iReference, public cnRTL::cDualReference
{
public:

	bcNTSocketOverlappedIORecyclableHandleManager();
	~bcNTSocketOverlappedIORecyclableHandleManager();

protected:
	void VirtualStopped(void);

	void DisconnectSocket(bcNTSocketOverlappedIOHandle *SocketIO,LPFN_DISCONNECTEX DisconnectEx);
	virtual void SocketDisconnected(bcNTSocketOverlappedIOHandle *SocketIO)=0;
	virtual void SocketDisconnectError(bcNTSocketOverlappedIOHandle *SocketIO)=0;


private:

	cExclusiveFlag fCheckQueueFlag;
	bool fClosed;
	
	void QueueProcedure(void);

	void QueueProcessDisconnect(void);
	void QueueProcessCancel(void);
	void QueueProcessDone(void);

	class cDisconnectTask : public bcNTSocketOverlappedIOHandle::bcIOObject
	{
	public:
		cDisconnectTask *Next;
		cDisconnectTask(bcNTSocketOverlappedIORecyclableHandleManager *Owner);
		~cDisconnectTask();

		bcNTSocketOverlappedIOHandle *SocketIO;
		LPFN_DISCONNECTEX fpDisconnectEx;
		bool Disconnect(void);

	protected:
		rInnerPtr<bcNTSocketOverlappedIORecyclableHandleManager> fOwner;
		virtual void Completed(void)override;
	};

	void DisconnectTaskDone(cDisconnectTask *Task);
	cnRTL::cAtomicQueueSO<cDisconnectTask> fDisconnectQueue;
	cnRTL::cAtomicQueueSO<cDisconnectTask> fDisconnectDoneQueue;
	cnRTL::cSeqSet<cDisconnectTask*> fDisconnectSet;
};
//---------------------------------------------------------------------------
class cNT6ThreadPoolSocketIOHandle : public bcNTSocketOverlappedIOHandle
{
public:
	cNT6ThreadPoolSocketIOHandle();
	~cNT6ThreadPoolSocketIOHandle();

	bool Open(SOCKET SocketHandle);

	virtual void Close(void)override;
	virtual void StartIO(void)override;
	virtual void CancelIO(void)override;
	virtual void CancelOperation(bcIOObject *Object)override;

protected:
	PTP_IO fIOThreadPool;

	static VOID CALLBACK IoCompletionCallback(
	  _Inout_     PTP_CALLBACK_INSTANCE Instance,
	  _Inout_opt_ PVOID                 Context,
	  _Inout_opt_ PVOID                 Overlapped,
	  _In_        ULONG                 IoResult,
	  _In_        ULONG_PTR             NumberOfBytesTransferred,
	  _Inout_     PTP_IO                Io
	);
};
//---------------------------------------------------------------------------
typedef iPtr<iConnection> NT6ThreadPoolSocketMakeConnectionFuction(rPtr<cNT6ThreadPoolSocketIOHandle> SocketIO,iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress);
iPtr<iConnection> NT6ThreadPoolSocketMakeStreamConnection(rPtr<cNT6ThreadPoolSocketIOHandle> SocketIO,iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress);
iPtr<iConnection> NT6ThreadPoolSocketMakeEndpointConnection(rPtr<cNT6ThreadPoolSocketIOHandle> SocketIO,iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress);
//---------------------------------------------------------------------------
class cNT6ThreadPoolRecyclableSocketIOHandle : public cNT6ThreadPoolSocketIOHandle
{
public:
	bool Open(SOCKET SocketHandle);

	bool Connected;
	LPFN_DISCONNECTEX fpDisconnectEx;

};
//---------------------------------------------------------------------------
class cNT6ThreadPoolClientSocketIOHandle : public cNT6ThreadPoolRecyclableSocketIOHandle
{
public:
	bool Open(SOCKET SocketHandle);

	iPtr<iSocketAddress> BoundAddress;
	LPFN_CONNECTEX fpConnectEx;

};
//---------------------------------------------------------------------------
template<class TSocketIOHandle>
class cNT6ThreadPoolRecyclableIOHandleManager
	: public bcNTSocketOverlappedIORecyclableHandleManager
	, public bcCPPLifeCycleRecyclableManager< impReferenceLifeCycleObject<TSocketIOHandle,cCPPLifeCycleRecyclableInstance> >
{
public:
	typedef impReferenceLifeCycleObject<TSocketIOHandle,cCPPLifeCycleRecyclableInstance> tLifeCycleObject;
	typedef typename tLifeCycleObject::template tActivation<tLifeCycleObject> tLifeCycleActivation;

	cNT6ThreadPoolRecyclableIOHandleManager(){}
	~cNT6ThreadPoolRecyclableIOHandleManager(){
		this->DeleteObjects();
	}


	virtual void Restore(tLifeCycleObject*)noexcept(true) override{
		rIncReference(this,'lcle');
	}
	virtual void Dispose(cCPPLifeCycleRecyclableInstance *Object)noexcept(true) override{
		DisconnectAndRecycleSocket(static_cast<tLifeCycleObject*>(Object));
	}
	void DisconnectAndRecycleSocket(tLifeCycleObject *SocketIO){
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
			bcCPPLifeCycleRecyclableManager<tLifeCycleObject>::Dispose(SocketIO);
			rDecReference(this,'lcle');
		}
	}

	virtual void SocketDisconnected(bcNTSocketOverlappedIOHandle *Object)override{
		auto LCObject=static_cast<tLifeCycleObject*>(Object);
		LCObject->Connected=false;
		bcCPPLifeCycleRecyclableManager<tLifeCycleObject>::Dispose(LCObject);
		rDecReference(this,'lcle');
	}
	virtual void SocketDisconnectError(bcNTSocketOverlappedIOHandle *SocketIO)override{
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
	cNT6ThreadPoolSocketIOHandleRecycler() : fManager(rCreate<tManager>()){}
	~cNT6ThreadPoolSocketIOHandleRecycler(){}

	rPtr<THandle> operator ()(void){	Query();	}
	rPtr<THandle> Query(void){
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
	cNT6ThreadPoolSocketConnectionConnector(iPtr<iSocketAddress> BindAddress,NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,int SocketAddressFamily,int SocketType,int SocketProtocol);
	~cNT6ThreadPoolSocketConnectionConnector();

protected:
	virtual rPtr<bcNTSocketOverlappedIOHandle> ConnectorQueryIOHandle(LPFN_CONNECTEX &fpConnectEx)override;
	virtual iPtr<iConnection> ConnectorMakeConnection(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iPtr<iSocketAddress> RemoteAddress)override;

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
		NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,int SocketType,int SocketProtocol);
	~cNT6ThreadPoolSocketConnectionListener();

protected:

	NT6ThreadPoolSocketMakeConnectionFuction *fMakeConnectionFunction;
	int fSocketType;
	int fSocketProtocol;
	
	virtual rPtr<bcNTSocketOverlappedIOHandle> ListenerQueryIOHandle(void)override;
	virtual iPtr<iConnection> ListenerMakeConnection(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress)override;

	cNT6ThreadPoolSocketIOHandleRecycler<cNT6ThreadPoolRecyclableSocketIOHandle> fSocketHandleRecyclePool;
};
//---------------------------------------------------------------------------
class cNT6ThreadPoolSocketStreamProtocol : public iConnectionProtocol
{
public:
	cNT6ThreadPoolSocketStreamProtocol(int Type,int Protocol);
	~cNT6ThreadPoolSocketStreamProtocol();

	virtual iPtr<iConnectionConnector>	cnLib_FUNC CreateStreamConnector(iAddress *LocalAddress,iAddress *RemoteAddress)override;
	virtual iPtr<iConnectionListener>	cnLib_FUNC CreateStreamListener(iAddress *LocalAddress)override;
	virtual iPtr<iConnectionQueue>		cnLib_FUNC CreateStreamConnectionQueue(iAddress *LocalAddress)override;

	virtual iPtr<iConnectionConnector>	cnLib_FUNC CreateEndpointConnector(iAddress *LocalAddress,iAddress *RemoteAddress)override;
	virtual iPtr<iConnectionListener>	cnLib_FUNC CreateEndpointListener(iAddress *LocalAddress)override;
	virtual iPtr<iConnectionQueue>		cnLib_FUNC CreateEndpointConnectionQueue(iAddress *LocalAddress)override;

	iPtr<iConnectionConnector> MakeConnector(NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,iAddress *LocalAddress,iAddress *RemoteAddress);
	iPtr<iConnectionListener> MakeListener(NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,iAddress *LocalAddress);

protected:
	int fType;
	int fProtocol;
};
//---------------------------------------------------------------------------
class cNT6ThreadPoolSocketDatagramProtocol : public iDatagramProtocol
{
public:
	cNT6ThreadPoolSocketDatagramProtocol(int Type,int Protocol);
	~cNT6ThreadPoolSocketDatagramProtocol();

	virtual iPtr<iConnection>		cnLib_FUNC CreateStreamPair(iAddress *LocalAddress,iAddress *RemoteAddress)override;
	virtual iPtr<iConnection>		cnLib_FUNC CreateEndpointPair(iAddress *LocalAddress,iAddress *RemoteAddress)override;
	virtual iPtr<iMultipointStream>	cnLib_FUNC OpenStream(iAddress *LocalAddress)override;
	virtual iPtr<iMultipointQueue>	cnLib_FUNC OpenQueue(iAddress *LocalAddress)override;
protected:
	int fType;
	int fProtocol;

	iPtr<iConnection> MakePair(NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,iAddress *LocalAddress,iAddress *RemoteAddress);

	bool SetupAddress(SOCKET s,iSocketAddress *Address);
	iPtr<iMultipointStream> MakeDatagramStream(iAddress *LocalAddress);
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
