#include "POSIX_Socket.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace siPOSIX;

//---------------------------------------------------------------------------
void siPOSIX::SetupSocket(int s)
{
	int r;
	//int nonblocking=1;
	//ioctl(s,FIONBIO,&nonblocking);
	int FileFlags=fcntl(s,F_GETFL,0);
	fcntl(s,F_SETFL,FileFlags|O_NONBLOCK);

	struct linger lo;
	socklen_t lolen=sizeof(lo);
	r=getsockopt(s, SOL_SOCKET, SO_LINGER, &lo, &lolen);
	if(r==0){
		lo.l_onoff=1;
		lo.l_linger=0;
		setsockopt(s,SOL_SOCKET,SO_LINGER,&lo,sizeof(lo));
	}
}
//---------------------------------------------------------------------------
eOrdering siPOSIX::SocketAddressCompare(const sockaddr *addr1,const sockaddr *addr2)
{
	if(addr1->sa_family!=addr2->sa_family)
		return Ordering::Different;

	switch(addr1->sa_family){
	default:
		return Ordering::Different;
	case AF_INET:
		{
			auto *V4Addr1=reinterpret_cast<const sockaddr_in*>(addr1);
			auto *V4Addr2=reinterpret_cast<const sockaddr_in*>(addr2);
			uInt32 ip1=cnMemory::SwapBigEndian(V4Addr1->sin_addr.s_addr);
			uInt32 ip2=cnMemory::SwapBigEndian(V4Addr2->sin_addr.s_addr);
			if(ip1<ip2)
				return Ordering::Less;
			if(ip1>ip2)
				return Ordering::Greater;
			uInt16 port1=cnMemory::SwapBigEndian(V4Addr1->sin_port);
			uInt16 port2=cnMemory::SwapBigEndian(V4Addr2->sin_port);
			if(port1<port2)
				return Ordering::Less;
			if(port1>port2)
				return Ordering::Greater;
			return Ordering::Equal;
		}
	case AF_INET6:
		{
			auto *V6Addr1=reinterpret_cast<const sockaddr_in6*>(addr1);
			auto *V6Addr2=reinterpret_cast<const sockaddr_in6*>(addr2);
			auto *ip1=reinterpret_cast<const uIntn*>(&V6Addr1->sin6_addr);
			auto *ip2=reinterpret_cast<const uIntn*>(&V6Addr2->sin6_addr);
			for(uIntn i=sizeof(V6Addr1->sin6_addr)/sizeof(uIntn);i>0;i--){
				uIntn v1=cnMemory::SwapBigEndian(ip1[i-1]);
				uIntn v2=cnMemory::SwapBigEndian(ip2[i-1]);
				if(v1<v2)
					return Ordering::Less;
				if(v1>v2)
					return Ordering::Greater;
			}
			uInt16 port1=cnMemory::SwapBigEndian(V6Addr1->sin6_port);
			uInt16 port2=cnMemory::SwapBigEndian(V6Addr2->sin6_port);
			if(port1<port2)
				return Ordering::Less;
			if(port1>port2)
				return Ordering::Greater;
			return Ordering::Equal;
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSocketAddressBuffer::cSocketAddressBuffer()
{
	fBuffer.SetSize(sizeof(sockaddr));
}
//---------------------------------------------------------------------------
cSocketAddressBuffer::~cSocketAddressBuffer()
{
}
//---------------------------------------------------------------------------
void cSocketAddressBuffer::Clear(void)
{
	fBuffer.Clear();
}
//---------------------------------------------------------------------------
sockaddr* cSocketAddressBuffer::SockAddr(void)const
{
	return static_cast<sockaddr*>(fBuffer->Pointer);
}
//---------------------------------------------------------------------------
socklen_t cSocketAddressBuffer::SockAddrLen(void)const
{
	return static_cast<socklen_t>(fBuffer->Length);
}
//---------------------------------------------------------------------------
void cSocketAddressBuffer::SetSockAddrLen(socklen_t len)
{
	if(len<sizeof(sockaddr))
		len=sizeof(sockaddr);
	return fBuffer.SetSize(len);
}
//---------------------------------------------------------------------------
socklen_t cSocketAddressBuffer::GetCapacity(void)const
{
	return static_cast<socklen_t>(fBuffer.GetCapacity());
}
//---------------------------------------------------------------------------
void cSocketAddressBuffer::SetCapacity(socklen_t len)
{
	if(len<sizeof(sockaddr))
		len=sizeof(sockaddr);
	return fBuffer.SetCapacity(len);
}
//---------------------------------------------------------------------------
bool cSocketAddressBuffer::SetAddressFamily(sa_family_t sa_family)
{
	auto *saddr=SockAddr();
	switch(sa_family){
	default:
		return false;
	case AF_INET:
		fBuffer.SetSize(sizeof(sockaddr_in));
		break;
	case AF_INET6:
		fBuffer.SetSize(sizeof(sockaddr_in6));
		break;
	}
	saddr->sa_family=sa_family;
	return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const uIntn bcStreamSocketEndpoint::ReadBufferMinSize=128;
const uIntn bcStreamSocketEndpoint::WriteBufferMinSize=128;
//---------------------------------------------------------------------------
bcStreamSocketEndpoint::bcStreamSocketEndpoint(int Socket)
{
	fSocket=Socket;
}
//---------------------------------------------------------------------------
bcStreamSocketEndpoint::~bcStreamSocketEndpoint()
{
	close(fSocket);
}
//---------------------------------------------------------------------------
const void* bcStreamSocketEndpoint::CastInterface(iTypeID IID)const
{
	return ImpCastInterface<iConnection,iEndpoint>(this,IID);
}
//---------------------------------------------------------------------------
void bcStreamSocketEndpoint::Close(void)
{
	CloseReadQueue();
	CloseWriteQueue();
}
//---------------------------------------------------------------------------
iAddress* bcStreamSocketEndpoint::GetLocalAddress(void)
{
	return fLocalAddress;
}
//---------------------------------------------------------------------------
iAddress* bcStreamSocketEndpoint::GetRemoteAddress(void)
{
	return fRemoteAddress;
}
//---------------------------------------------------------------------------
iReadQueue* bcStreamSocketEndpoint::GetReadQueue(void)
{
	return this;
}
//---------------------------------------------------------------------------
iWriteQueue* bcStreamSocketEndpoint::GetWriteQueue(void)
{
	return this;
}
//---------------------------------------------------------------------------
void bcStreamSocketEndpoint::SetWriteEndMode(eEndpointWriteEndMode EndMode)
{
	WriteQueueSetEndMode(EndMode);
}
//---------------------------------------------------------------------------
void bcStreamSocketEndpoint::ReadContinue(void)
{
	ReadQueueSetBufferAvailable();
	UpdateReadQueueState();
}
//---------------------------------------------------------------------------
void bcStreamSocketEndpoint::WriteContinue(void)
{
	WriteQueueSetBufferAvailable();
	SendBufferWriteToSocket();
	UpdateWriteQueueState();
}
//---------------------------------------------------------------------------
cMemory bcStreamSocketEndpoint::ReserveWriteBuffer(uIntn Size)
{
	if(Size<WriteBufferMinSize)
		Size=WriteBufferMinSize;
	return fWriteQueueBuffer.ReserveWriteBuffer(Size);
}
//---------------------------------------------------------------------------
uIntn bcStreamSocketEndpoint::CommitWriteBuffer(uIntn Size)
{
	uIntn SizeCommited=fWriteQueueBuffer.CommitWriteBuffer(Size);

	// continue write to socket
	SendBufferWriteToSocket();

	return SizeCommited;
}
//---------------------------------------------------------------------------
void bcStreamSocketEndpoint::WriteQueueClosed(void)
{
	if(WriteQueueIsEnded()){
		shutdown(fSocket,SHUT_WR);
	}
}
//---------------------------------------------------------------------------
bool bcStreamSocketEndpoint::SendBufferWriteToSocketLoop(void)
{
	auto WriteBuffer=fWriteQueueBuffer.GatherReadBuffer(0);
	if(WriteBuffer.Length!=0){
		// send data
		auto BytesSent=send(fSocket,WriteBuffer.Pointer,WriteBuffer.Length,MSG_DONTWAIT);
		if(BytesSent<0){
			auto en=errno;
			if(en==EWOULDBLOCK || en==EAGAIN){
				// no send buffer
				OnWriteWait();
				return false;
			}
			if(en==EMSGSIZE){
				// size too big, discard
				fWriteQueueBuffer.DismissReadBuffer(WriteBuffer.Length);
				// continue send
				return true;
			}
			// other error
			CloseWriteQueue();
			return false;
		}
		else{
			// send completed
			fWriteQueueBuffer.DismissReadBuffer(BytesSent);
			// continue send
			return true;
		}
	}
	// nothing to send
	return false;
}
//---------------------------------------------------------------------------
void bcStreamSocketEndpoint::SendBufferWriteToSocket(void)
{
	if(fSendBufferToSocketExclusiveFlag.Acquire()==false)
		return;


	do{
		do{
			fSendBufferToSocketExclusiveFlag.Continue();

		}while(SendBufferWriteToSocketLoop());
	}while(fSendBufferToSocketExclusiveFlag.Release()==false);
}
//---------------------------------------------------------------------------
cConstMemory bcStreamSocketEndpoint::GatherReadBuffer(uIntn QuerySize)
{
	if(QuerySize<ReadBufferMinSize)
		QuerySize=ReadBufferMinSize;
	auto AllReadBuffer=fReadQueueBuffer.GetReadBuffer();
	if(AllReadBuffer.Length>QuerySize){
		return AllReadBuffer;
	}

	auto WriteBuffer=fReadQueueBuffer.ReserveWriteBuffer(QuerySize-AllReadBuffer.Length);

	auto SizeReceived=recv(fSocket,WriteBuffer.Pointer,WriteBuffer.Length,0);
	if(SizeReceived<0){
		auto en=errno;
		if(en==EWOULDBLOCK || en==EAGAIN){
			// no dat available, wait for socket
			OnReadWait();
		}
		else{
			// error
			CloseReadQueue();
			CloseWriteQueue();
			return NullArray;
		}
	}
	else if(SizeReceived==0){
		// stream ended
		ReadQueueSetEnded();
		CloseReadQueue();
	}
	else{	// SizeReceived>0
		fReadQueueBuffer.CommitWriteBuffer(SizeReceived);
	}

	return fReadQueueBuffer.GatherReadBuffer(QuerySize);
}
//---------------------------------------------------------------------------
uIntn bcStreamSocketEndpoint::DismissReadBuffer(uIntn Size)
{
	return fReadQueueBuffer.DismissReadBuffer(Size);
}
//---------------------------------------------------------------------------
void bcStreamSocketEndpoint::ReadQueueClosed(void)
{
	if(ReadQueueIsEnded()){
		shutdown(fSocket,SHUT_RD);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const uIntn bcDatagramSocketEndpoint::ReadBufferMinSize=128;
const uIntn bcDatagramSocketEndpoint::WriteBufferMinSize=128;
//---------------------------------------------------------------------------
bcDatagramSocketEndpoint::bcDatagramSocketEndpoint(int Socket)
{
	fSocket=Socket;

	fWriteDataSize=IndexNotFound;
	fReadDataSize=IndexNotFound;
}
//---------------------------------------------------------------------------
bcDatagramSocketEndpoint::~bcDatagramSocketEndpoint()
{
	close(fSocket);
}
//---------------------------------------------------------------------------
const void* bcDatagramSocketEndpoint::CastInterface(iTypeID IID)const
{
	return ImpCastInterface<iConnection,iEndpoint>(this,IID);
}
//---------------------------------------------------------------------------
void bcDatagramSocketEndpoint::Close(void)
{
	CloseReadQueue();
	CloseWriteQueue();
}
//---------------------------------------------------------------------------
iReadQueue* bcDatagramSocketEndpoint::GetReadQueue(void)
{
	return this;
}
//---------------------------------------------------------------------------
iWriteQueue* bcDatagramSocketEndpoint::GetWriteQueue(void)
{
	return this;
}
//---------------------------------------------------------------------------
void bcDatagramSocketEndpoint::SetWriteEndMode(eEndpointWriteEndMode EndMode)
{
	WriteQueueSetEndMode(EndMode);
}
//---------------------------------------------------------------------------
iAddress* bcDatagramSocketEndpoint::GetLocalAddress(void)
{
	return fLocalAddress;
}
//---------------------------------------------------------------------------
iAddress* bcDatagramSocketEndpoint::GetRemoteAddress(void)
{
	return fRemoteAddress;
}
//---------------------------------------------------------------------------
cMemory bcDatagramSocketEndpoint::ReserveWriteBuffer(uIntn Size)
{
	if(fWriteDataSize!=IndexNotFound)
		return NullArray;	// has pending buffer

	if(fWriteBuffer.Length<Size){
		fWriteBuffer.Clear();
		fWriteBuffer.SetLength(Size);
	}
	return fWriteBuffer;
}
//---------------------------------------------------------------------------
uIntn bcDatagramSocketEndpoint::CommitWriteBuffer(uIntn Size)
{
	if(fWriteDataSize!=IndexNotFound){
		fWriteDataSize=Size;
		// continue write to socket
		SendBufferWriteToSocket();
		return Size;
	}
	return 0;
}
//---------------------------------------------------------------------------
void bcDatagramSocketEndpoint::SendBufferWriteToSocket(void)
{
	if(fSendBufferToSocketExclusiveFlag.Acquire()==false)
		return;


	do{
		fSendBufferToSocketExclusiveFlag.Continue();

		if(fWriteDataSize!=IndexNotFound){
			// send data
			auto BytesSent=send(fSocket,fWriteBuffer.Pointer,fWriteDataSize,MSG_DONTWAIT);
			if(BytesSent<0){
				auto en=errno;
				if(en==EWOULDBLOCK || en==EAGAIN){
					// no send buffer
					OnWriteWait();
				}
				else if(en==EMSGSIZE){
					// size too big, discard
					fWriteDataSize=IndexNotFound;
					// continue send
				}
				else{
					// other error
					CloseWriteQueue();
				}
			}
			else{
				// send completed
				fWriteDataSize=IndexNotFound;
			}
		}
	}while(fSendBufferToSocketExclusiveFlag.Release()==false);
}
//---------------------------------------------------------------------------
cConstMemory bcDatagramSocketEndpoint::GatherReadBuffer(uIntn QuerySize)
{
	if(QuerySize==0){
		QuerySize=65536;
	}
	else if(QuerySize<ReadBufferMinSize){
		QuerySize=ReadBufferMinSize;
	}

	if(fReadDataSize==IndexNotFound){
		if(fReadBuffer.Length<QuerySize){
			fReadBuffer.Clear();
			fReadBuffer.SetLength(QuerySize);
		}
		ssize_t SizeReceived=recv(fSocket,fReadBuffer.Pointer,fReadBuffer.Length,0);
		if(SizeReceived<0){
			auto en=errno;
			if(en==EWOULDBLOCK || en==EAGAIN){
				// no dat available, wait for socket
				OnReadWait();
			}
			else{
				// error
				CloseReadQueue();
				CloseWriteQueue();
				return NullArray;
			}

			return NullArray;
		}
		else{	// SizeReceived>=0
			fReadDataSize=SizeReceived;
		}
	}
	cConstMemory Buffer;
	Buffer.Pointer=fReadBuffer.Pointer;
	Buffer.Length=fReadDataSize;
	return Buffer;
}
//---------------------------------------------------------------------------
uIntn bcDatagramSocketEndpoint::DismissReadBuffer(uIntn Size)
{
	uIntn Dismissed=fReadDataSize;
	fReadDataSize=IndexNotFound;
	return Dismissed;
}
//---------------------------------------------------------------------------
void bcDatagramSocketEndpoint::WriteQueueClosed(void)
{
	//if(WriteQueueIsEnded()){
	//	shutdown(fSocket,SHUT_WR);
	//}
}
//---------------------------------------------------------------------------
void bcDatagramSocketEndpoint::ReadQueueClosed(void)
{
}
//---------------------------------------------------------------------------
void bcDatagramSocketEndpoint::ReadContinue(void)
{
	ReadQueueSetBufferAvailable();
	UpdateReadQueueState();
}
//---------------------------------------------------------------------------
void bcDatagramSocketEndpoint::WriteContinue(void)
{
	WriteQueueSetBufferAvailable();
	SendBufferWriteToSocket();
	UpdateWriteQueueState();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcSocketConnectionQueue::bcSocketConnectionQueue(int Socket)
{
	fSocket=Socket;
	fAcceptAvailable=false;
}
//---------------------------------------------------------------------------
bcSocketConnectionQueue::~bcSocketConnectionQueue()
{
	close(fSocket);
}
//---------------------------------------------------------------------------
iPtr<iConnection> bcSocketConnectionQueue::FetchConnection(void)
{
	union{
		sockaddr saddr;
		uIntn AddrStorage[16];
	};
	socklen_t saddrlen=sizeof(saddr);
	int NewSocket=accept(fSocket,&saddr,&saddrlen);
	if(NewSocket==SocketInvalid){
		auto en=errno;
		if(en==EWOULDBLOCK || en==EAGAIN){
			// continue notify when connection available
			OnAcceptWait();
		}
		else{
			// error
			CloseSignal();
		}
		return nullptr;
	}
	auto Connection=OnAcceptCreateConnection(NewSocket,&saddr,saddrlen);
	return Connection;
}
//---------------------------------------------------------------------------
bcSocketConnectionQueue::Availability bcSocketConnectionQueue::AsyncSignalAvailable(void)
{
	if(fAcceptAvailable){
		return Availability::Available;
	}
	return Availability::Empty;
}
//---------------------------------------------------------------------------
void bcSocketConnectionQueue::AcceptContinue(void)
{
	fAcceptAvailable=true;
	UpdateSignalState();
}
//---------------------------------------------------------------------------
#if 0
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
cSocketDatagramPort::cSocketDatagramPort(int Socket,int SocketType,sa_family_t sa_family)
{
	fSocket=Socket;

	fReadAddressBuffer.SetAddressFamily(sa_family);

	fReadBuffer=RTLHeap::Alloc(65536);
	fReadBufferSize=65536;
	fReadDataSize=0;
	fWriteBuffer=RTLHeap::Alloc(65536);
	fWriteBufferSize=65536;
	fWriteSize=0;

	fCloseState=0;
}
//--------------------------------------------------------------------------
cSocketDatagramPort::~cSocketDatagramPort()
{
	RTLHeap::Free(fReadBuffer);
	RTLHeap::Free(fWriteBuffer);
}
//--------------------------------------------------------------------------
void cnLib_FUNC cSocketDatagramPort::Close(void)
{
	if(fCloseState==0){
		fCloseState=1;
		fPortProc.Run();
	}
}
//--------------------------------------------------------------------------
bool cnLib_FUNC cSocketDatagramPort::StartManage(iDatagramPortManager *Manager,bool MultiThreadPull)
{
	if(fCloseState)
		return false;

	if(fManager!=nullptr)
		return false;

	fManager=Manager;
	fPushPaused=true;
	fManagerStopped=false;
	fPushIdleNotify=false;
	fPullIdleNotify=false;


	fManager->PortStarted();
			
	return true;
}
//--------------------------------------------------------------------------
void cnLib_FUNC cSocketDatagramPort::StopManage(void)
{
	fManagerStopped=true;
	fPortProc.Run();
}
//--------------------------------------------------------------------------
bool cSocketDatagramPort::PullWrite(void)
{
	if(fWriteSize==0){
		// pull
		iPtr<iAddress> Address;
		if(fManager==nullptr)
			return false;
		fPullIdleNotify=false;
		fWriteSize=0;
		if(fManager->PullPacket(Address,fWriteBuffer,fWriteBufferSize,fWriteSize)==false){
			if(fWriteSize!=0){
				// buffer need too big
				Close();
				return true;
			}
			return false;
		}
		iCast(fWriteAddress,Address);
	}
	else{
		if(fPullIdleNotify){
			fPullIdleNotify=false;
			// idle notify
			iPtr<iAddress> addr;
			uintn SizeRet=0;
			fManager->PullPacket(addr,nullptr,0,SizeRet);
		}
	}

	auto saddr=fWriteAddress->SockAddr();
	auto saddrlen=fWriteAddress->SockAddrLen();
	auto BytesSent=::sendto(fSocket,fWriteBuffer,fWriteSize,MSG_DONTWAIT,saddr,saddrlen);
	if(BytesSent<0){
		auto en=errno;
		if(en==EWOULDBLOCK){
			// no send buffer
			OnWriteWait();
			return false;
		}
		if(en==EMSGSIZE){
			// size too big
		}
	}
	fWriteSize=0;
	fWriteAddress=nullptr;

	// continue send
	return true;
}
//---------------------------------------------------------------------------
bool cSocketDatagramPort::ReadPush(void)
{
	if(fPushPaused){
		// cannot push
		return false;
	}

	if(fReadDataSize==0){
		sockaddr *saddr=fReadAddressBuffer.SockAddr();
		socklen_t addrlen=fReadAddressBuffer.GetCapacity();
		auto SizeRecv=::recvfrom(fSocket,fReadBuffer,fReadBufferSize,MSG_DONTWAIT,saddr,&addrlen);
		if(SizeRecv<0){
			if(fPushIdleNotify){
				fPushIdleNotify=false;
				// idle notify
				bool Pushed=fManager->PushPacket(nullptr,nullptr,0);
				if(Pushed==false){
					fPushPaused=true;
				}
			}
			auto en=errno;
			if(en==EWOULDBLOCK){
				// continue notify when data available
				OnReadWait();
			}
			return false;
		}
		fReadDataSize=SizeRecv;
		fReadAddress=cnSystem::CreateSocketAddress(saddr,addrlen);
	}

	// push
	fPushIdleNotify=false;
	bool Pushed=fManager->PushPacket(fReadAddress,fReadBuffer,fReadDataSize);
	if(Pushed==false){
		fPushPaused=true;
		return false;
	}
	// continue
	fReadAddress=nullptr;
	fReadDataSize=0;
	return true;
}
//---------------------------------------------------------------------------
bool cSocketDatagramPort::cPortProcedure::HostProcedure(void)
{
	auto Host=GetObjectFromMemberPointer(this,&cSocketDatagramPort::fPortProc);
	return Host->PortProc();
}
//--------------------------------------------------------------------------
bool cSocketDatagramPort::PortProc(void)
{
	if(fManagerStopped){
		if(fManager!=nullptr){
			fManager->PortStopped();
			fManager=nullptr;
		}
	}
	if(fCloseState!=0){
		if(fCloseState==1){
			fCloseState=2;
			if(fManager!=nullptr){
				fManager->PortTerminate();
			}

			OnReadEnded();
			OnWriteEnded();
		}
		return false;
	}

	if(ReadPush())
		return true;

	if(PullWrite())
		return true;

	return false;
}
//--------------------------------------------------------------------------
void cSocketDatagramPort::ReadContinue(void)
{
	fPortProc.Run();
}
//--------------------------------------------------------------------------
void cSocketDatagramPort::WriteContinue(void)
{
	fPortProc.Run();
}
//--------------------------------------------------------------------------
void cnLib_FUNC cSocketDatagramPort::NotifyPull(bool IdleNotify)
{
	if(IdleNotify)
		fPullIdleNotify=true;
	fPortProc.Run();
}
//--------------------------------------------------------------------------
void cnLib_FUNC cSocketDatagramPort::ResumePush(bool IdleNotify)
{
	if(IdleNotify)
		fPushIdleNotify=true;
	fPushPaused=false;
	fPortProc.Run();
}
//--------------------------------------------------------------------------
void cnLib_FUNC cSocketDatagramPort::PausePush(void)
{
	fPushPaused=true;
}
//--------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif // 0
