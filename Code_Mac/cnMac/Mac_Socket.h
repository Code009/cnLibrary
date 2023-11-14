/*--- Mac - Socket --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2012-11-20                                          */
/*-------------------------------------------------------------------------*/

#pragma once

//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>

//---------------------------------------------------------------------------
#include <siOS/POSIX/POSIX_Socket.h>

#include <cnMac/Mac_Header.h>
#include <cnMac/Mac_GCD.h>

/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnMac{
//---------------------------------------------------------------------------
static constexpr int SocketInvalid=siPOSIX::SocketInvalid;
//---------------------------------------------------------------------------
template<class TSockAddr>
class cSocketAddress : public iSocketAddress
{
protected:
	union{
		sockaddr saddr;
		TSockAddr fAddr;
	};
	cSocketAddress()=default;
	~cSocketAddress()=default;
public:
	cSocketAddress(const TSockAddr *addr){
		cnMemory::Copy(&fAddr,addr,sizeof(TSockAddr));
	}

	virtual eOrdering cnLib_FUNC Compare(const iAddress *Dest)const override{
		if(Dest==nullptr)
			return Ordering::Different;
		auto DestSocketAddress=iCast<iSocketAddress>(Dest);
		if(DestSocketAddress==nullptr)
			return Ordering::Different;
		return siPOSIX::SocketAddressCompare(&saddr,DestSocketAddress->SockAddr());
	}


	virtual sockaddr* cnLib_FUNC SockAddr(void)override{
		return &saddr;
	}
	virtual socklen_t cnLib_FUNC SockAddrLen(void)override{
		return sizeof(TSockAddr);
	}
};
//---------------------------------------------------------------------------
iPtr<iSocketAddress> CreateSocketAddress(const sockaddr *addr,socklen_t len);
iPtr<iSocketAddress> CreateSocketAddress(const siPOSIX::cSocketAddressBuffer &Buffer);
//---------------------------------------------------------------------------
class cSocketAddress_IPv4 : public cSocketAddress<sockaddr_in> , public iIPv4Address
{
protected:
public:

	using cSocketAddress::cSocketAddress;
	cSocketAddress_IPv4(uInt32 IP,uInt16 Port);

	virtual const void * cnLib_FUNC CastInterface(iTypeID InterfaceID)const override;

// iAddress

	virtual eOrdering cnLib_FUNC Compare(const iAddress *Dest)const override;
// iIPv4Address

	virtual uInt32 cnLib_FUNC IPv4Address(void)override;
	virtual uInt16 cnLib_FUNC IPv4Port(void)override;
};
//---------------------------------------------------------------------------
iPtr<iIPv4Address> CreateSocketAddress_IPv4(uInt32 IP,uInt16 Port);
//---------------------------------------------------------------------------
/*
class cSocketAddressBuffer
{
protected:
	cnRTL::cMemoryBuffer<> fBuffer;
public:
	cSocketAddressBuffer();
	~cSocketAddressBuffer();

	void Clear(void);

	sockaddr* SockAddr(void)const;
	socklen_t SockAddrLen(void)const;
	void SetSockAddrLen(socklen_t len);

	socklen_t GetCapacity(void)const;
	void SetCapacity(socklen_t len);

	bool SetAddressFamily(sa_family_t sa_family);

	iPtr<iSocketAddress> CreateSocketAddress(void);
};
*/
//---------------------------------------------------------------------------
class cStreamSocketEndpoint : public siPOSIX::bcStreamSocketEndpoint, public cnRTL::cDualReference
{
public:

	cStreamSocketEndpoint(int Socket);
	~cStreamSocketEndpoint();
protected:
	void VirtualStarted(void);
	void VirtualStopped(void);

	virtual iReference* RWQueueInnerReference(void)override;
	virtual void WriteQueueClosed(void)override;
	virtual void ReadQueueClosed(void)override;

	virtual void OnReadWait(void)override;
	virtual void OnWriteWait(void)override;


private:
	// GCD notification

	dispatch_source_t fDispatchSourceRead;
	dispatch_source_t fDispatchSourceWrite;


	static void ReadSourceProcedure(void *Parameter);
	static void ReadSourceDeletor(void *Parameter);

	static void WriteSourceProcedure(void *Parameter);
	static void WriteSourceDeletor(void *Parameter);

	bool fReadWaiting=false;
	bool fWriteWaiting=false;
	void ReadNotify(void);
	void WriteNotify(void);
};
//---------------------------------------------------------------------------
class cDatagramSocketEndpoint : public siPOSIX::bcDatagramSocketEndpoint, public cnRTL::cDualReference
{
public:
	cDatagramSocketEndpoint(int Socket);
	~cDatagramSocketEndpoint();
protected:
	void VirtualStarted(void);
	void VirtualStopped(void);

	virtual iReference* RWQueueInnerReference(void)override;
	virtual void WriteQueueClosed(void)override;
	virtual void ReadQueueClosed(void)override;

	virtual void OnReadWait(void)override;
	virtual void OnWriteWait(void)override;

private:
	// GCD notification

	dispatch_source_t fDispatchSourceRead;
	dispatch_source_t fDispatchSourceWrite;


	static void ReadSourceProcedure(void *Parameter);
	static void ReadSourceDeletor(void *Parameter);

	static void WriteSourceProcedure(void *Parameter);
	static void WriteSourceDeletor(void *Parameter);

	bool fReadWaiting=false;
	bool fWriteWaiting=false;
	void ReadNotify(void);
	void WriteNotify(void);
};
//---------------------------------------------------------------------------
class cSocketConnectionQueue : public siPOSIX::bcSocketConnectionQueue, public cnRTL::cDualReference
{
public:
	cSocketConnectionQueue(iTypeID ConnectionTypeID,int Socket,int SocketType);
	~cSocketConnectionQueue();
protected:
	void VirtualStarted(void);
	void VirtualStopped(void);

	virtual iReference* AsyncSignalInnerReference(void)override;
	virtual void AsyncSignalClosed(void)override;
	virtual iPtr<iConnection> OnAcceptCreateConnection(int Socket,const sockaddr *addr,socklen_t addrlen)override;
	virtual void OnAcceptWait(void)override;
private:
	dispatch_source_t fDispatchSourceAccept;
	iTypeID fConnectionTypeID;
	int fSocketType;

	bool fAcceptWaiting;
	static void AcceptSourceProcedure(void *Parameter);
	static void AcceptSourceDeletor(void *Parameter);

	void AcceptProcedure(void);


};
#if	0
//---------------------------------------------------------------------------
class cSocketConnectionTask : public cnRTL::cConnectionTask, public iReference
{
private:
	int fSocket;
	iPtr<iSocketAddress> fRemoteAddress;
	iTypeID fConnectionIID;
	sa_family_t fFamily;

	class cConnectProcedure : public iProcedure
	{
	protected:
		virtual void cnLib_FUNC Execute(void)override;
	}fConnectProcedure;
	void ConnectProcedure(void);

	class cAcceptProcedure : public iProcedure
	{
	protected:
		virtual void cnLib_FUNC Execute(void)override;
	}fAcceptProcedure;
	void AcceptProcedure(void);
public:
	cSocketConnectionTask();
	~cSocketConnectionTask();

	virtual void cnLib_FUNC Cancel(void)override;

	bool Connect(int SocketType,int SocketProtocol,iTypeID ConnectionIID,iSocketAddress *RemoteAddress,iSocketAddress *LocalAddress);
	bool Accept(int AcceptSocket,iTypeID ConnectionIID,sa_family_t sa_family);
};
//---------------------------------------------------------------------------
class cSocketListener : public iConnectionListener
{
private:
	int fSocket;
	iTypeID fConnectionIID;
	int fSocketType;
	sa_family_t fFamily;
public:
	cSocketListener(iTypeID ConnectionIID,int Socket,int SocketType,sa_family_t sa_family);
	~cSocketListener();

	virtual void	cnLib_FUNC Close(void)override;
	virtual iRet<iConnection>		cnLib_FUNC rpAccept(void)override;
	virtual iRet<iConnectionTask>	cnLib_FUNC rpAcceptAsync(void)override;
};
//---------------------------------------------------------------------------
class cSocketConnectionProtocol : public iConnectionProtocol
{
private:
	int fSocketType;
	int fSocketProtocol;

	bool SocketListen(int &Socket,iSocketAddress *LocalAddress);
public:
	cSocketConnectionProtocol(int SocketType,int SocketProtocol);
	~cSocketConnectionProtocol();

	virtual iPtr<iConnectionListener>	cnLib_FUNC Listen(iTypeID ConnectionIID,iAddress *LocalAddress)override;
	virtual iPtr<iConnectionPushPort>	cnLib_FUNC ListenPush(iTypeID ConnectionIID,iAddress *LocalAddress)override;
	virtual iPtr<iConnection>			cnLib_FUNC Connect(iTypeID ConnectionIID,iAddress *RemoteAddress,iAddress *LocalAddress)override;
	virtual iPtr<iConnectionTask>		cnLib_FUNC ConnectAsync(iTypeID ConnectionIID,iAddress *RemoteAddress,iAddress *LocalAddress)override;
};
//---------------------------------------------------------------------------
class cSocketDatagramPort : public siPOSIX::cSocketDatagramPort, protected cnRTL::cInnerReference
{
private:
	// GCD notification

	rPtr<cGCDSource> fDispatchRead;
	rPtr<cGCDSource> fDispatchWrite;

	class cReadNotify : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)override;
	}fReadNotify;
	class cWriteNotify : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)override;
	}fWriteNotify;

	bool fReadWaiting=false;
	bool fWriteWaiting=false;
	void ReadNotify(void);
	void WriteNotify(void);
protected:
	virtual void OnReadWait(void)override;
	virtual void OnReadEnded(void)override;
	virtual void OnWriteWait(void)override;
	virtual void OnWriteEnded(void)override;

	void InterfaceDelete(void);

	~cSocketDatagramPort();
public:
	cSocketDatagramPort(int Socket,int SocketType,sa_family_t sa_family);
};
//---------------------------------------------------------------------------
class cSocketDatagramProtocol : public iDatagramProtocol
{
private:
	int fSocketType;
	int fSocketProtocol;

	bool SocketBind(int &Socket,iSocketAddress *LocalAddress);
public:
	cSocketDatagramProtocol(int SocketType,int SocketProtocol);
	~cSocketDatagramProtocol();

	virtual iPtr<iConnection>	cnLib_FUNC Pair(iTypeID ConnectionIID,iAddress *RemoteAddress,iAddress *LocalAddress)override;
	virtual iPtr<iDatagramStream>	cnLib_FUNC OpenStream(iAddress *LocalAddress)override;
	virtual iPtr<iDatagramPort>	cnLib_FUNC OpenPort(iAddress *LocalAddress)override;
};
#endif
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
