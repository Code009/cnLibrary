#include "Mac_Socket.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;
#define	INVALID_SOCKET	-1

//---------------------------------------------------------------------------
iPtr<iSocketAddress> cnMac::CreateSocketAddress(const sockaddr *addr,socklen_t addrlen)
{
	switch(addr->sa_family){
	default:
		return nullptr;
	case AF_INET:
		if(addrlen>sizeof(sockaddr_in))
			return nullptr;
		return iCreate<cSocketAddress_IPv4>(reinterpret_cast<const sockaddr_in*>(addr));
/*	case AF_INET6:
		if(addrlen>sizeof(sockaddr_in6))
			return nullptr;
		return iCreate< cWinSocketAddress<SOCKADDR_IN6> >(reinterpret_cast<const SOCKADDR_IN6*>(addr));
	case AF_BTH:
		if(addrlen>sizeof(SOCKADDR_BTH))
			return nullptr;
		return iCreate< cWinSocketAddress<SOCKADDR_BTH> >(reinterpret_cast<const SOCKADDR_BTH*>(addr));
*/
	}
}
//---------------------------------------------------------------------------
iPtr<iSocketAddress> cnMac::CreateSocketAddress(const siPOSIX::cSocketAddressBuffer &Buffer)
{
	return CreateSocketAddress(Buffer.SockAddr(),Buffer.SockAddrLen());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSocketAddress_IPv4::cSocketAddress_IPv4(uInt32 IP,uInt16 Port)
{
	fAddr.sin_family=AF_INET;
	fAddr.sin_addr.s_addr=htonl(IP);
	fAddr.sin_port=htons(Port);
	cnMemory::ZeroFill(fAddr.sin_zero);
}
//---------------------------------------------------------------------------
void* cSocketAddress_IPv4::CastInterface(iTypeID ID)
{
	return ImpCastInterface<iSocketAddress,iIPv4Address>(this,ID);
}
//---------------------------------------------------------------------------
eiOrdering cSocketAddress_IPv4::Compare(iAddress *Dest)const
{
	return cSocketAddress::Compare(Dest);
}
//---------------------------------------------------------------------------
uInt32 cSocketAddress_IPv4::IPv4Address(void)
{
	return ntohl(fAddr.sin_addr.s_addr);
}
uInt16 cSocketAddress_IPv4::IPv4Port(void)
{
	return ntohs(fAddr.sin_port);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iIPv4Address> cnMac::CreateSocketAddress_IPv4(uInt32 IP,uInt16 Port)
{
	return iCreate<cSocketAddress_IPv4>(IP,Port);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cStreamSocketEndpoint::cStreamSocketEndpoint(int Socket)
	: bcStreamSocketEndpoint(Socket)
{
}
//---------------------------------------------------------------------------
cStreamSocketEndpoint::~cStreamSocketEndpoint()
{
}
//---------------------------------------------------------------------------
void cStreamSocketEndpoint::VirtualStarted(void)
{
	InnerIncReference('self');
	auto Queue=GCDDefaultQueue();

	fDispatchSourceRead=dispatch_source_create(DISPATCH_SOURCE_TYPE_READ,fSocket,0,Queue);
	dispatch_set_context(fDispatchSourceRead,this);
	dispatch_set_finalizer_f(fDispatchSourceRead,ReadSourceDeletor);
	dispatch_source_set_event_handler_f(fDispatchSourceRead,ReadSourceProcedure);
	InnerIncReference('read');

	fDispatchSourceWrite=dispatch_source_create(DISPATCH_SOURCE_TYPE_WRITE,fSocket,0,Queue);
	dispatch_set_context(fDispatchSourceWrite,this);
	dispatch_set_finalizer_f(fDispatchSourceWrite,WriteSourceDeletor);
	dispatch_source_set_event_handler_f(fDispatchSourceWrite,WriteSourceProcedure);
	InnerIncReference('writ');
}
//---------------------------------------------------------------------------
void cStreamSocketEndpoint::VirtualStopped(void)
{
	Close();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
iReference* cStreamSocketEndpoint::RWQueueInnerReference(void)
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cStreamSocketEndpoint::ReadSourceProcedure(void *Parameter)
{
	auto This=static_cast<cStreamSocketEndpoint*>(Parameter);
	This->ReadNotify();
}
//---------------------------------------------------------------------------
void cStreamSocketEndpoint::ReadSourceDeletor(void *Parameter)
{
	auto This=static_cast<cStreamSocketEndpoint*>(Parameter);
	This->InnerDecReference('read');
}
//---------------------------------------------------------------------------
void cStreamSocketEndpoint::WriteSourceProcedure(void *Parameter)
{
	auto This=static_cast<cStreamSocketEndpoint*>(Parameter);
	This->WriteNotify();
}
//---------------------------------------------------------------------------
void cStreamSocketEndpoint::WriteSourceDeletor(void *Parameter)
{
	auto This=static_cast<cStreamSocketEndpoint*>(Parameter);
	This->InnerDecReference('writ');
}
//---------------------------------------------------------------------------
void cStreamSocketEndpoint::ReadNotify(void)
{
	fReadWaiting=false;
	dispatch_suspend(fDispatchSourceRead);
	ReadContinue();
}
//---------------------------------------------------------------------------
void cStreamSocketEndpoint::WriteNotify(void)
{
	fWriteWaiting=false;
	dispatch_suspend(fDispatchSourceWrite);
	WriteContinue();
}
//---------------------------------------------------------------------------
void cStreamSocketEndpoint::WriteQueueClosed(void)
{
	bcStreamSocketEndpoint::WriteQueueClosed();
	if(fDispatchSourceWrite!=nullptr){
		dispatch_source_cancel(fDispatchSourceWrite);
		if(fWriteWaiting==false){
			fWriteWaiting=true;
			dispatch_resume(fDispatchSourceWrite);
		}
		dispatch_release(fDispatchSourceWrite);
		fDispatchSourceWrite=nullptr;
	}
}
//---------------------------------------------------------------------------
void cStreamSocketEndpoint::OnWriteWait(void)
{
	if(fWriteWaiting==false){
		fWriteWaiting=true;
		dispatch_resume(fDispatchSourceWrite);
	}
}
//---------------------------------------------------------------------------
void cStreamSocketEndpoint::ReadQueueClosed(void)
{
	bcStreamSocketEndpoint::ReadQueueClosed();

	if(fDispatchSourceRead!=nullptr){
		dispatch_source_cancel(fDispatchSourceRead);
		if(fReadWaiting==false){
			fReadWaiting=true;
			dispatch_resume(fDispatchSourceRead);
		}
		dispatch_release(fDispatchSourceRead);
		fDispatchSourceRead=nullptr;
	}
}
//---------------------------------------------------------------------------
void cStreamSocketEndpoint::OnReadWait(void)
{
	if(fReadWaiting==false){
		fReadWaiting=true;
		dispatch_resume(fDispatchSourceRead);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDatagramSocketEndpoint::cDatagramSocketEndpoint(int Socket)
	: bcDatagramSocketEndpoint(Socket)
{
}
//---------------------------------------------------------------------------
cDatagramSocketEndpoint::~cDatagramSocketEndpoint()
{
}
//---------------------------------------------------------------------------
void cDatagramSocketEndpoint::VirtualStarted(void)
{
	InnerIncReference('self');

	auto Queue=GCDDefaultQueue();

	fDispatchSourceRead=dispatch_source_create(DISPATCH_SOURCE_TYPE_READ,fSocket,0,Queue);
	dispatch_set_context(fDispatchSourceRead,this);
	dispatch_set_finalizer_f(fDispatchSourceRead,ReadSourceDeletor);
	dispatch_source_set_event_handler_f(fDispatchSourceRead,ReadSourceProcedure);
	InnerIncReference('read');

	fDispatchSourceWrite=dispatch_source_create(DISPATCH_SOURCE_TYPE_WRITE,fSocket,0,Queue);
	dispatch_set_context(fDispatchSourceWrite,this);
	dispatch_set_finalizer_f(fDispatchSourceWrite,WriteSourceDeletor);
	dispatch_source_set_event_handler_f(fDispatchSourceWrite,WriteSourceProcedure);
	InnerIncReference('writ');
}
//---------------------------------------------------------------------------
void cDatagramSocketEndpoint::VirtualStopped(void)
{
	Close();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
iReference* cDatagramSocketEndpoint::RWQueueInnerReference(void)
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cDatagramSocketEndpoint::ReadSourceProcedure(void *Parameter)
{
	auto This=static_cast<cDatagramSocketEndpoint*>(Parameter);
	This->ReadNotify();
}
//---------------------------------------------------------------------------
void cDatagramSocketEndpoint::ReadSourceDeletor(void *Parameter)
{
	auto This=static_cast<cDatagramSocketEndpoint*>(Parameter);
	This->InnerDecReference('read');
}
//---------------------------------------------------------------------------
void cDatagramSocketEndpoint::WriteSourceProcedure(void *Parameter)
{
	auto This=static_cast<cDatagramSocketEndpoint*>(Parameter);
	This->WriteNotify();
}
//---------------------------------------------------------------------------
void cDatagramSocketEndpoint::WriteSourceDeletor(void *Parameter)
{
	auto This=static_cast<cDatagramSocketEndpoint*>(Parameter);
	This->InnerDecReference('writ');
}
//---------------------------------------------------------------------------
void cDatagramSocketEndpoint::ReadNotify(void)
{
	fReadWaiting=false;
	dispatch_suspend(fDispatchSourceRead);
	ReadContinue();
}
//---------------------------------------------------------------------------
void cDatagramSocketEndpoint::WriteNotify(void)
{
	fWriteWaiting=false;
	dispatch_suspend(fDispatchSourceWrite);
	WriteContinue();
}
//---------------------------------------------------------------------------
void cDatagramSocketEndpoint::WriteQueueClosed(void)
{
	bcDatagramSocketEndpoint::WriteQueueClosed();
	if(fDispatchSourceWrite!=nullptr){
		dispatch_source_cancel(fDispatchSourceWrite);
		if(fWriteWaiting==false){
			fWriteWaiting=true;
			dispatch_resume(fDispatchSourceWrite);
		}
		dispatch_release(fDispatchSourceWrite);
		fDispatchSourceWrite=nullptr;
	}
}
//---------------------------------------------------------------------------
void cDatagramSocketEndpoint::OnWriteWait(void)
{
	if(fWriteWaiting==false){
		fWriteWaiting=true;
		dispatch_resume(fDispatchSourceWrite);
	}
}
//---------------------------------------------------------------------------
void cDatagramSocketEndpoint::ReadQueueClosed(void)
{
	bcDatagramSocketEndpoint::ReadQueueClosed();
	if(fDispatchSourceRead!=nullptr){
		dispatch_source_cancel(fDispatchSourceRead);
		if(fReadWaiting==false){
			fReadWaiting=true;
			dispatch_resume(fDispatchSourceRead);
		}
		dispatch_release(fDispatchSourceRead);
		fDispatchSourceRead=nullptr;
	}
}
//---------------------------------------------------------------------------
void cDatagramSocketEndpoint::OnReadWait(void)
{
	if(fReadWaiting==false){
		fReadWaiting=true;
		dispatch_resume(fDispatchSourceRead);
	}
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
cSocketConnectionQueue::cSocketConnectionQueue(iTypeID ConnectionTypeID,int Socket,int SocketType)
	: siPOSIX::bcSocketConnectionQueue(Socket)
{
	fConnectionTypeID=ConnectionTypeID;
	fSocketType=SocketType;

	fAcceptWaiting=false;
}
//--------------------------------------------------------------------------
cSocketConnectionQueue::~cSocketConnectionQueue()
{
}
//---------------------------------------------------------------------------
void cSocketConnectionQueue::VirtualStarted(void)
{
	InnerIncReference('self');

	auto Queue=GCDDefaultQueue();
	fDispatchSourceAccept=dispatch_source_create(DISPATCH_SOURCE_TYPE_READ,fSocket,0,Queue);
	dispatch_set_context(fDispatchSourceAccept,this);
	dispatch_set_finalizer_f(fDispatchSourceAccept,AcceptSourceDeletor);
	dispatch_source_set_event_handler_f(fDispatchSourceAccept,AcceptSourceProcedure);
	InnerIncReference('acpt');

}
//---------------------------------------------------------------------------
void cSocketConnectionQueue::VirtualStopped(void)
{
	CloseSignal();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
void cSocketConnectionQueue::AcceptSourceProcedure(void *Parameter)
{
	auto This=static_cast<cSocketConnectionQueue*>(Parameter);
	This->AcceptProcedure();
}
//---------------------------------------------------------------------------
void cSocketConnectionQueue::AcceptSourceDeletor(void *Parameter)
{
	auto This=static_cast<cSocketConnectionQueue*>(Parameter);
	This->InnerDecReference('acpt');
}
//---------------------------------------------------------------------------
void cSocketConnectionQueue::AcceptProcedure(void)
{
	fAcceptWaiting=false;
	dispatch_suspend(fDispatchSourceAccept);

	AcceptContinue();
}
//--------------------------------------------------------------------------
iPtr<iConnection> cSocketConnectionQueue::OnAcceptCreateConnection(int Socket,const sockaddr *addr,socklen_t addrlen)
{
	if(fConnectionTypeID==iTypeOf<iEndpoint>()){
		iPtr<iConnection> Connection;
		siPOSIX::SetupSocket(Socket);
		switch(fSocketType){
		case SOCK_STREAM:
			Connection=iCreate<cStreamSocketEndpoint>(Socket);
			break;
		case SOCK_DGRAM:
			Connection=iCreate<cDatagramSocketEndpoint>(Socket);
			break;
		}
		return Connection;
	}
	//if(fConnectionTypeID==iTypeOf<iStreamConnection>()){
	//}
	return nullptr;
}
//--------------------------------------------------------------------------
void cSocketConnectionQueue::OnAcceptWait(void)
{
	if(fAcceptWaiting==false){
		fAcceptWaiting=true;
		dispatch_resume(fDispatchSourceAccept);
	}
}
//--------------------------------------------------------------------------
void cSocketConnectionQueue::AsyncSignalClosed(void)
{
	bcSocketConnectionQueue::AsyncSignalClosed();

	if(fDispatchSourceAccept!=nullptr){
		dispatch_source_cancel(fDispatchSourceAccept);
		if(fAcceptWaiting==false){
			fAcceptWaiting=true;
			dispatch_resume(fDispatchSourceAccept);
		}
		dispatch_release(fDispatchSourceAccept);
		fDispatchSourceAccept=nullptr;
	}
}
//---------------------------------------------------------------------------
#if	0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSocketConnectionTask::cSocketConnectionTask()
{
	fSocket=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
cSocketConnectionTask::~cSocketConnectionTask()
{
}
//---------------------------------------------------------------------------
void cnLib_FUNC cSocketConnectionTask::Cancel(void)
{
}
//---------------------------------------------------------------------------
void cnLib_FUNC cSocketConnectionTask::cConnectProcedure::Execute(void)
{
	auto This=GetObjectFromMemberPointer(this,&cSocketConnectionTask::fConnectProcedure);
	This->ConnectProcedure();
}
//---------------------------------------------------------------------------
void cSocketConnectionTask::ConnectProcedure(void)
{
	auto *saddr=fRemoteAddress->SockAddr();
	auto saddrlen=fRemoteAddress->SockAddrLen();
	if(connect(fSocket,saddr,saddrlen)!=0){
		close(fSocket);
		fSocket=INVALID_SOCKET;
		SetError(ConnectError::Error);
		NotifyCompletion();
		return;
	}

	int nonblocking=1;
	ioctl(fSocket,FIONBIO,&nonblocking);

	struct linger lo;
	socklen_t lolen=sizeof(lo);
	getsockopt(fSocket, SOL_SOCKET, SO_LINGER, &lo, &lolen);
	lo.l_onoff=1;
	lo.l_linger=0;
	setsockopt(fSocket,SOL_SOCKET,SO_LINGER,&lo,sizeof(lo));

	auto SocketEndpoint=iCreate<cSocketStreamEndpoint>(fSocket,SOCK_STREAM);

	auto ResultConnection=CreateConnectionFromEndpoint(fConnectionIID,SocketEndpoint,nullptr, fRemoteAddress,false);
	SetResult(ResultConnection);
	NotifyCompletion();
}
//---------------------------------------------------------------------------
bool cSocketConnectionTask::Connect(int SocketType,int SocketProtocol,iTypeID ConnectionIID,iSocketAddress *RemoteAddress,iSocketAddress *LocalAddress)
{
	fConnectionIID=ConnectionIID;
	fRemoteAddress=RemoteAddress;
	auto *saddr=fRemoteAddress->SockAddr();
	//auto saddrlen=fRemoteAddress->SockAddrLen();

	fSocket=socket(saddr->sa_family,SocketType,SocketProtocol);
	if(fSocket==INVALID_SOCKET)
		return false;

	auto Queue=GCDDefaultQueue();
	GCDPost(Queue,this,&fConnectProcedure);
	return true;
}
//---------------------------------------------------------------------------
void cnLib_FUNC cSocketConnectionTask::cAcceptProcedure::Execute(void)
{
	auto This=GetObjectFromMemberPointer(this,&cSocketConnectionTask::fAcceptProcedure);
	This->AcceptProcedure();
}
//---------------------------------------------------------------------------
void cSocketConnectionTask::AcceptProcedure(void)
{
	siPOSIX::cSocketAddressBuffer AddressBuffer;
	AddressBuffer.SetAddressFamily(fFamily);
	auto saddr=AddressBuffer.SockAddr();
	socklen_t saddrlen=AddressBuffer.SockAddrLen();
	int s=accept(fSocket,saddr, &saddrlen);
	if(s==INVALID_SOCKET){
		SetError(ConnectError::Error);
		NotifyCompletion();
		return;
	}
	saddr->sa_len=saddrlen;
	auto Address=CreateSocketAddress(AddressBuffer);
	auto SocketEndpoint=iCreate<cSocketStreamEndpoint>(s,SOCK_STREAM);
	auto Connection=CreateConnectionFromEndpoint(fConnectionIID,SocketEndpoint,nullptr,Address,true);
	SetResult(Connection);

	NotifyCompletion();
}
//---------------------------------------------------------------------------
bool cSocketConnectionTask::Accept(int AcceptSocket,iTypeID ConnectionIID,sa_family_t sa_family)
{
	fConnectionIID=ConnectionIID;
	fSocket=AcceptSocket;
	fFamily=sa_family;
	auto Queue=GCDDefaultQueue();
	GCDPost(Queue,this,&fConnectProcedure);
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSocketListener::cSocketListener(iTypeID ConnectionIID,int Socket,int SocketType,sa_family_t sa_family)
{
	fConnectionIID=ConnectionIID;
	fSocket=Socket;
	fSocketType=SocketType;
	fFamily=sa_family;
}
//---------------------------------------------------------------------------
cSocketListener::~cSocketListener()
{
}
//---------------------------------------------------------------------------
void	cnLib_FUNC cSocketListener::Close(void)
{
	close(fSocket);
	fSocket=0;
}
//---------------------------------------------------------------------------
iPtr<iConnection>			cnLib_FUNC cSocketListener::Accept(void)
{
	siPOSIX::cSocketAddressBuffer AddressBuffer;
	AddressBuffer.SetAddressFamily(fFamily);
	auto saddr=AddressBuffer.SockAddr();
	socklen_t saddrlen=AddressBuffer.SockAddrLen();
	int s=accept(fSocket, saddr, &saddrlen);
	if(s==INVALID_SOCKET){
		return nullptr;
	}
	saddr->sa_len=saddrlen;
	auto Address=CreateSocketAddress(AddressBuffer);
	auto SocketEndpoint=iCreate<cSocketStreamEndpoint>(s,fSocketType);
	auto Connection=CreateConnectionFromEndpoint(fConnectionIID,SocketEndpoint,nullptr,Address,true);
	return Connection;
}
//---------------------------------------------------------------------------
iPtr<iConnectionTask>			cnLib_FUNC cSocketListener::AcceptAsync(void)
{
	auto Task=iCreate<cSocketConnectionTask>();
	Task->Accept(fSocket,fConnectionIID,fFamily);
	return Task;
}
//--------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSocketConnectionProtocol::cSocketConnectionProtocol(int SocketType,int SocketProtocol)
{
	fSocketType=SocketType;
	fSocketProtocol=SocketProtocol;
}
//---------------------------------------------------------------------------
cSocketConnectionProtocol::~cSocketConnectionProtocol()
{
}
//---------------------------------------------------------------------------
bool cSocketConnectionProtocol::SocketListen(int &Socket,iSocketAddress *LocalAddress)
{
	auto SocketAddress=iCast<iSocketAddress>(LocalAddress);
	if(SocketAddress==nullptr)
		return false;

	auto *saddr=SocketAddress->SockAddr();
	socklen_t saddrlen=SocketAddress->SockAddrLen();
	Socket=socket(saddr->sa_family,fSocketType,fSocketProtocol);
	if(bind(Socket,saddr,saddrlen)!=0){
		close(Socket);
		return false;
	}
	if(listen(Socket, SOMAXCONN)!=0){
		close(Socket);
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
iPtr<iConnectionListener>	cnLib_FUNC cSocketConnectionProtocol::Listen(iTypeID ConnectionIID,iAddress *LocalAddress)
{
	auto SocketAddress=iCast<iSocketAddress>(LocalAddress);
	if(SocketAddress==nullptr)
		return nullptr;
	int Socket;
	if(SocketListen(Socket,SocketAddress)==false)
		return nullptr;
	return iCreate<cSocketListener>(ConnectionIID,Socket,fSocketType,SocketAddress->SockAddr()->sa_family);
}
//---------------------------------------------------------------------------
iPtr<iConnectionPushPort>	cnLib_FUNC cSocketConnectionProtocol::ListenPush(iTypeID ConnectionIID,iAddress *LocalAddress)
{
	auto SocketAddress=iCast<iSocketAddress>(LocalAddress);
	if(SocketAddress==nullptr)
		return nullptr;
	int Socket;
	if(SocketListen(Socket,SocketAddress)==false)
		return nullptr;

	return iCreate<cSocketConnectionPushPort>(ConnectionIID,Socket,fSocketType);
}
//---------------------------------------------------------------------------
iPtr<iConnection>		cnLib_FUNC cSocketConnectionProtocol::Connect(iTypeID ConnectionIID,iAddress *RemoteAddress,iAddress *LocalAddress)
{
	if(LocalAddress!=nullptr){
		return nullptr;
	}
	auto RemoteSocketAddress=iCast<iSocketAddress>(RemoteAddress);
	if(RemoteSocketAddress==nullptr)
		return nullptr;

	cSyncNotifier Notifier;
	auto Task=iCreate<cSocketConnectionTask>();
	if(Task->Connect(fSocketType,fSocketProtocol,ConnectionIID,RemoteSocketAddress,nullptr)==false){
		return nullptr;
	}

	cFunctor<void (void) > NotifyEvent=[&]{	Notifier.Notify();	};
	Task->SetNotify(NotifyEvent);
	Notifier.Wait();
	return Task->GetConnection();
}
//---------------------------------------------------------------------------
iPtr<iConnectionTask>		cnLib_FUNC cSocketConnectionProtocol::ConnectAsync(iTypeID ConnectionIID,iAddress *RemoteAddress,iAddress *LocalAddress)
{
	if(LocalAddress!=nullptr){
		return nullptr;
	}
	auto RemoteSocketAddress=iCast<iSocketAddress>(RemoteAddress);
	if(RemoteSocketAddress==nullptr)
		return nullptr;

	auto Task=iCreate<cSocketConnectionTask>();
	if(Task->Connect(fSocketType,fSocketProtocol,ConnectionIID,RemoteSocketAddress,nullptr)){
		return Task;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSocketDatagramPort::cSocketDatagramPort(int Socket,int SocketType,sa_family_t sa_family)
	: siPOSIX::cSocketDatagramPort(Socket,SocketType,sa_family)
{
	auto Queue=GCDDefaultQueue();
	fDispatchRead=rCreate<cGCDSource>(DISPATCH_SOURCE_TYPE_READ,fSocket,0,Queue);
	fDispatchRead->ProcedureReferene=this;
	fDispatchRead->Procedure=&fReadNotify;

	fDispatchWrite=rCreate<cGCDSource>(DISPATCH_SOURCE_TYPE_WRITE,fSocket,0,Queue);
	fDispatchWrite->ProcedureReferene=this;
	fDispatchWrite->Procedure=&fWriteNotify;
}
//---------------------------------------------------------------------------
cSocketDatagramPort::~cSocketDatagramPort()
{
}
//---------------------------------------------------------------------------
void cSocketDatagramPort::InterfaceDelete(void)
{
	Close();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
void cnLib_FUNC cSocketDatagramPort::cReadNotify::Execute(void)
{
	auto This=GetObjectFromMemberPointer(this,&cSocketDatagramPort::fReadNotify);
	This->ReadNotify();
}
//---------------------------------------------------------------------------
void cSocketDatagramPort::ReadNotify(void)
{
	fReadWaiting=false;
	fDispatchRead->Suspend();
	ReadContinue();
}
//---------------------------------------------------------------------------
void cnLib_FUNC cSocketDatagramPort::cWriteNotify::Execute(void)
{
	auto This=GetObjectFromMemberPointer(this,&cSocketDatagramPort::fWriteNotify);
	This->WriteNotify();
}
//---------------------------------------------------------------------------
void cSocketDatagramPort::WriteNotify(void)
{
	fWriteWaiting=false;
	fDispatchWrite->Suspend();
	WriteContinue();
}
//---------------------------------------------------------------------------
void cSocketDatagramPort::OnWriteEnded(void)
{
	if(fDispatchWrite!=nullptr){
		fDispatchWrite->Cancel();
		if(fWriteWaiting==false){
			fWriteWaiting=true;
			fDispatchWrite->Resume();
		}
		fDispatchWrite=nullptr;
	}
}
//---------------------------------------------------------------------------
void cSocketDatagramPort::OnWriteWait(void)
{
	if(fWriteWaiting==false){
		fWriteWaiting=true;
		fDispatchWrite->Resume();
	}
}
//---------------------------------------------------------------------------
void cSocketDatagramPort::OnReadEnded(void)
{
	if(fDispatchRead!=nullptr){
		fDispatchRead->Cancel();
		if(fReadWaiting==false){
			fReadWaiting=true;
			fDispatchRead->Resume();
		}
		fDispatchRead=nullptr;
	}
}
//---------------------------------------------------------------------------
void cSocketDatagramPort::OnReadWait(void)
{
	if(fReadWaiting==false){
		fReadWaiting=true;
		fDispatchRead->Resume();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSocketDatagramProtocol::cSocketDatagramProtocol(int SocketType,int SocketProtocol)
{
	fSocketType=SocketType;
	fSocketProtocol=SocketProtocol;
}
cSocketDatagramProtocol::~cSocketDatagramProtocol()
{
}
//---------------------------------------------------------------------------
bool cSocketDatagramProtocol::SocketBind(int &Socket,iSocketAddress *LocalAddress)
{
	auto SocketAddress=iCast<iSocketAddress>(LocalAddress);
	if(SocketAddress==nullptr)
		return false;

	auto *saddr=SocketAddress->SockAddr();
	socklen_t saddrlen=SocketAddress->SockAddrLen();
	Socket=socket(saddr->sa_family,fSocketType,fSocketProtocol);
	if(bind(Socket,saddr,saddrlen)!=0){
		close(Socket);
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
iPtr<iConnection>	cnLib_FUNC cSocketDatagramProtocol::Pair(iTypeID ConnectionIID,iAddress *RemoteAddress,iAddress *LocalAddress)
{
	if(LocalAddress!=nullptr)
		return nullptr;

	auto RSocketAddress=iCast<iSocketAddress>(RemoteAddress);
	if(RSocketAddress==nullptr)
		return nullptr;

	auto *rsaddr=RSocketAddress->SockAddr();
	socklen_t rsaddrlen=RSocketAddress->SockAddrLen();


	int s=socket(rsaddr->sa_family,fSocketType,fSocketProtocol);
	if(connect(s,rsaddr,rsaddrlen)!=0){
		close(s);
		return nullptr;
	}

	siPOSIX::SetupSocket(s);

	auto SocketEndpoint=iCreate<cSocketStreamEndpoint>(s,fSocketType);

	
	auto ResultLink=CreateConnectionFromEndpoint(ConnectionIID,SocketEndpoint,nullptr, RemoteAddress,false);
	return ResultLink;
}
//---------------------------------------------------------------------------
iPtr<iDatagramStream>	cnLib_FUNC cSocketDatagramProtocol::OpenStream(iAddress *LocalAddress)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iDatagramPort>	cnLib_FUNC cSocketDatagramProtocol::OpenPort(iAddress *LocalAddress)
{
	auto SocketAddress=iCast<iSocketAddress>(LocalAddress);
	if(SocketAddress==nullptr)
		return nullptr;

	int Socket;
	if(SocketBind(Socket,SocketAddress)==false)
		return nullptr;
	auto Family=SocketAddress->SockAddr()->sa_family;
	return iCreate<cSocketDatagramPort>(Socket,fSocketType,Family);
}
#endif
//---------------------------------------------------------------------------
