﻿//#include <WS2tcpip.h>
// For guids
//#include <svcguid.h>

#include "WinSocketIO.h"
#include <cnRTL\InterfaceImplementation.h>
#include <cnRTL\StreamAdapter.h>
#include <cnRTL\NetConnection.h>

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;


// disable buffers
	//int opt=0;
	//::setsockopt(SocketIO->Handle,SOL_SOCKET,SO_SNDBUF,(char*)&opt,sizeof(opt));
	//::setsockopt(SocketIO->Handle,SOL_SOCKET,SO_RCVBUF,(char*)&opt,sizeof(opt));

//---------------------------------------------------------------------------
static eStreamError WinSocketErrorToStreamError(DWORD ErrorCode)noexcept(true)
{
	switch(ErrorCode){
	default:
		return StreamError::Unknown;
	case ERROR_SUCCESS:
		return StreamError::Success;
	case WSAECONNRESET:
	case WSAECONNREFUSED:
		return StreamError::Closed;
	case ERROR_CANCELLED:
		return StreamError::Cancelled;
	case ERROR_INVALID_HANDLE:
		return StreamError::Closed;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcSocketIOHandle::bcSocketIOHandle()noexcept(true)
	: Handle(INVALID_SOCKET)
{
}
//---------------------------------------------------------------------------
bcSocketIOHandle::~bcSocketIOHandle()noexcept(true)
{
	cnLib_ASSERT(Handle==INVALID_SOCKET);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNTSocketOverlappedIOHandleSyncIOObject::cNTSocketOverlappedIOHandleSyncIOObject()noexcept(true)
{
}
//---------------------------------------------------------------------------
cNTSocketOverlappedIOHandleSyncIOObject::~cNTSocketOverlappedIOHandleSyncIOObject()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cNTSocketOverlappedIOHandleSyncIOObject::Completed(void)noexcept(true)
{
	fNotifier.Notify();
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleSyncIOObject::Recv(bcNTSocketOverlappedIOHandle *SocketIO,void *Buffer,uIntn Size)noexcept(true)
{
	BytesCompleted=0;
	Flags=0;

	SocketIO->StartIO();

	WSABUF wb;
	wb.buf=static_cast<CHAR*>(Buffer);
	wb.len=static_cast<ULONG>(Size);
	int RecvRet=::WSARecv(SocketIO->Handle,&wb,1,&BytesCompleted,&Flags,&Overlapped,nullptr);
	DWORD RecvErrorCode=::WSAGetLastError();
	if(RecvRet==0 || (RecvErrorCode=::WSAGetLastError())==WSA_IO_PENDING){
		// completed or in progress , wait for notification
		fNotifier.Wait();
		return true;
	}
	
	// error
	SocketIO->CancelIO();
	ErrorCode=RecvErrorCode;
	return false;
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleSyncIOObject::Send(bcNTSocketOverlappedIOHandle *SocketIO,const void *Buffer,uIntn Size)noexcept(true)
{
	BytesCompleted=0;
	Flags=0;

	SocketIO->StartIO();

	WSABUF wb;
	wb.buf=static_cast<CHAR*>(const_cast<void*>(Buffer));
	wb.len=static_cast<ULONG>(Size);

	int SendRet=::WSASend(SocketIO->Handle,&wb,1,&BytesCompleted,Flags,&Overlapped,nullptr);
	DWORD SendErrorCode=::GetLastError();
	if(SendRet==0 || (SendErrorCode=::GetLastError())==WSA_IO_PENDING){
		// completed or in progress, wait for notification
		fNotifier.Wait();
		return true;
	}

	// error
	SocketIO->CancelIO();
	ErrorCode=SendErrorCode;
	return false;
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleSyncIOObject::RecvFrom(bcNTSocketOverlappedIOHandle *SocketIO,cWin32SocketAddressBuffer &AddressBuffer,void *Buffer,uIntn Size)noexcept(true)
{
	cnMemory::ZeroFill(Overlapped);
	Flags=0;
	BytesCompleted=0;
	
	socklen_t AddrLen=static_cast<socklen_t>(AddressBuffer.GetCapacity());
	SocketIO->StartIO();
	WSABUF wb;
	wb.buf=static_cast<CHAR*>(Buffer);
	wb.len=static_cast<ULONG>(Size);
	int RecvRet=::WSARecvFrom(SocketIO->Handle,&wb,1,&BytesCompleted,&Flags,AddressBuffer.SockAddr(),&AddrLen,&Overlapped,nullptr);
	DWORD RecvErrorCode;
	if(RecvRet==0 || (RecvErrorCode=::WSAGetLastError())==WSA_IO_PENDING){
		// wait for notification
		fNotifier.Wait();
		AddressBuffer.SetSockAddrLen(AddrLen);
		return true;
	}
	
	// error
	SocketIO->CancelIO();
	ErrorCode=RecvErrorCode;
	return false;
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleSyncIOObject::SendTo(bcNTSocketOverlappedIOHandle *SocketIO,const sockaddr *addr,socklen_t addrlen,const void *Buffer,uIntn Size)noexcept(true)
{
	cnMemory::ZeroFill(Overlapped);
	Flags=0;
	BytesCompleted=0;

	SocketIO->StartIO();
	WSABUF wb;
	wb.buf=static_cast<CHAR*>(const_cast<void*>(Buffer));
	wb.len=static_cast<ULONG>(Size);
	int SendRet=::WSASendTo(SocketIO->Handle,&wb,1,&BytesCompleted,Flags,addr,addrlen,&Overlapped,nullptr);
	DWORD SendErrorCode=::WSAGetLastError();
	if(SendRet==0 || (SendErrorCode=::WSAGetLastError())==WSA_IO_PENDING){
		// completed or in progress, wait for notification
		fNotifier.Wait();
		return true;
	}

	// error
	SocketIO->CancelIO();
	ErrorCode=SendErrorCode;
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNTSocketOverlappedIOHandleStreamAsyncIOTask::cNTSocketOverlappedIOHandleStreamAsyncIOTask()noexcept(true)
{
}
//---------------------------------------------------------------------------
cNTSocketOverlappedIOHandleStreamAsyncIOTask::~cNTSocketOverlappedIOHandleStreamAsyncIOTask()noexcept(true)
{
}
//---------------------------------------------------------------------------
void* cNTSocketOverlappedIOHandleStreamAsyncIOTask::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	return ImpCastInterface<iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleStreamAsyncIOTask::IsDone(void)noexcept(true)
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleStreamAsyncIOTask::Await(iProcedure *NotifyProcedure)noexcept(true)
{
	return fTaskState.Await(NotifyProcedure);
}
//---------------------------------------------------------------------------
void cNTSocketOverlappedIOHandleStreamAsyncIOTask::Cancel(void)noexcept(true)
{
	::CancelIoEx(reinterpret_cast<HANDLE>(fSocketIO->Handle),&Overlapped);
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleStreamAsyncIOTask::GetResult(uIntn &retSizeCompleted)noexcept(true)
{
	retSizeCompleted=BytesCompleted;
	return ErrorCode==ERROR_SUCCESS;
}
//---------------------------------------------------------------------------
eStreamError cNTSocketOverlappedIOHandleStreamAsyncIOTask::GetStreamError(void)noexcept(true)
{
	return WinSocketErrorToStreamError(ErrorCode);
}
//---------------------------------------------------------------------------
void cNTSocketOverlappedIOHandleStreamAsyncIOTask::Completed(void)noexcept(true)
{
	NotifyCompletion();
}
//---------------------------------------------------------------------------
void cNTSocketOverlappedIOHandleStreamAsyncIOTask::NotifyCompletion(void)noexcept(true)
{
	fTaskState.SetDone();
	iDecReference(this,'task');
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleStreamAsyncIOTask::Recv(bcNTSocketOverlappedIOHandle *SocketIO,void *Buffer,uIntn Size)noexcept(true)
{
	fSocketIO=SocketIO;
	BytesCompleted=0;
	Flags=0;

	iIncReference(this,'task');

	fSocketIO->StartIO();

	WSABUF wb;
	wb.buf=static_cast<CHAR*>(Buffer);
	wb.len=static_cast<ULONG>(Size);
	int RecvRet=::WSARecv(fSocketIO->Handle,&wb,1,&BytesCompleted,&Flags,&Overlapped,nullptr);
	DWORD RecvErrorCode;
	if(RecvRet==0 || (RecvErrorCode=::WSAGetLastError())==WSA_IO_PENDING){
		// completed or in progress, wait for notification
		return true;
	}
	
	// error
	SocketIO->CancelIO();
	ErrorCode=RecvErrorCode;
	NotifyCompletion();
	return false;
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleStreamAsyncIOTask::Send(bcNTSocketOverlappedIOHandle *SocketIO,const void *Buffer,uIntn Size)noexcept(true)
{
	fSocketIO=SocketIO;
	BytesCompleted=0;
	Flags=0;

	iIncReference(this,'task');

	fSocketIO->StartIO();

	WSABUF wb;
	wb.buf=static_cast<CHAR*>(const_cast<void*>(Buffer));
	wb.len=static_cast<ULONG>(Size);

	int SendRet=::WSASend(fSocketIO->Handle,&wb,1,&BytesCompleted,Flags,&Overlapped,nullptr);
	DWORD SendErrorCode;
	if(SendRet==0 || (SendErrorCode=::GetLastError())==WSA_IO_PENDING){
		// completed or in progress, wait for notification
		return true;
	}

	// error
	fSocketIO->CancelIO();
	ErrorCode=SendErrorCode;
	NotifyCompletion();
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNTSocketOverlappedIOHandleStream::cNTSocketOverlappedIOHandleStream(rPtr<bcNTSocketOverlappedIOHandle> SocketIO)noexcept(true)
	: fSocketIO(cnVar::MoveCast(SocketIO))
{
	fEOS=false;
}
//---------------------------------------------------------------------------
cNTSocketOverlappedIOHandleStream::~cNTSocketOverlappedIOHandleStream()noexcept(true)
{
}
//---------------------------------------------------------------------------
void* cNTSocketOverlappedIOHandleStream::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	return ImpCastInterface<iStream,iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
void cNTSocketOverlappedIOHandleStream::Close(void)noexcept(true)
{
	fSocketIO->Close();
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleStream::IsEndOfReading(void)noexcept(true)
{
	return fEOS;
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleStream::SetEndOfStream(void)noexcept(true)
{
	return ShutdownSend();
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleStream::ShutdownSend(void)noexcept(true)
{
	if(::shutdown(fSocketIO->Handle,SD_SEND)==0){
		return true;
	}
	//DWORD ErrorCode=::WSAGetLastError();
	return false;
}
//---------------------------------------------------------------------------
eStreamError cNTSocketOverlappedIOHandleStream::GetStreamError(void)noexcept(true)
{
	DWORD Error=::GetLastError();
	return WinSocketErrorToStreamError(Error);
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleStream::Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)
{
	cNTSocketOverlappedIOHandleSyncIOObject IOObject;
	cnMemory::ZeroFill(IOObject.Overlapped);

	if(IOObject.Recv(fSocketIO,Buffer,Size)==false)
		return false;

	SizeCompleted=IOObject.BytesCompleted;
	if(IOObject.ErrorCode==ERROR_SUCCESS && IOObject.BytesCompleted==0){
		fEOS=true;
	}
	if(IOObject.ErrorCode==ERROR_HANDLE_EOF){
		fEOS=true;
		return true;
	}

	return IOObject.ErrorCode==ERROR_SUCCESS;
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleStream::Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)
{
	cNTSocketOverlappedIOHandleSyncIOObject IOObject;
	cnMemory::ZeroFill(IOObject.Overlapped);

	if(IOObject.Send(fSocketIO,Buffer,Size)==false)
		return false;

	SizeCompleted=IOObject.BytesCompleted;
	return IOObject.ErrorCode==ERROR_SUCCESS;
}
//---------------------------------------------------------------------------
void cNTSocketOverlappedIOHandleStream::cReadTask::Completed(void)noexcept(true)
{
	if(ErrorCode==ERROR_SUCCESS && BytesCompleted==0){
		Host->fEOS=true;
	}
	if(ErrorCode==ERROR_HANDLE_EOF){
		Host->fEOS=true;
	}
	Host=nullptr;

	cNTSocketOverlappedIOHandleStreamAsyncIOTask::Completed();
}
//---------------------------------------------------------------------------
iPtr<iStreamTask> cNTSocketOverlappedIOHandleStream::ReadAsync(void *Buffer,uIntn Size)noexcept(true)
{
	auto Task=iCreate<cReadTask>();
	Task->Host=this;
	cnMemory::ZeroFill(Task->Overlapped);
	if(Task->Recv(fSocketIO,Buffer,Size)==false)
		return nullptr;

	return Task;
}
//---------------------------------------------------------------------------
iPtr<iStreamTask> cNTSocketOverlappedIOHandleStream::WriteAsync(const void *Buffer,uIntn Size)noexcept(true)
{
	auto Task=iCreate<cNTSocketOverlappedIOHandleStreamAsyncIOTask>();
	cnMemory::ZeroFill(Task->Overlapped);
	if(Task->Send(fSocketIO,Buffer,Size)==false)
		return nullptr;

	return Task;
}
//---------------------------------------------------------------------------
uIntn cNTSocketOverlappedIOHandleStream::GetBufferSizeMax(void)noexcept(true)
{
	union{
		char data;
		unsigned int maxmsgsize;
		uIntn Result;
	};
	Result=0;
	int OptLen=4;
	getsockopt(fSocketIO->Handle,SOL_SOCKET,SO_MAX_MSG_SIZE,&data,&OptLen);
	return Result;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask::cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask()noexcept(true)
{
}
//---------------------------------------------------------------------------
cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask::~cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask()noexcept(true)
{
}
//---------------------------------------------------------------------------
void* cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	return ImpCastInterface<iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask::IsDone(void)noexcept(true)
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask::Await(iProcedure *NotifyProcedure)noexcept(true)
{
	return fTaskState.Await(NotifyProcedure);
}
//---------------------------------------------------------------------------
void cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask::Cancel(void)noexcept(true)
{
	::CancelIoEx(reinterpret_cast<HANDLE>(fSocketIO->Handle),&Overlapped);
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask::GetResult(uIntn &retSizeCompleted)noexcept(true)
{
	retSizeCompleted=BytesCompleted;
	return ErrorCode==ERROR_SUCCESS;
}
//---------------------------------------------------------------------------
eStreamError cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask::GetStreamError(void)noexcept(true)
{
	return WinSocketErrorToStreamError(ErrorCode);
}
//---------------------------------------------------------------------------
void cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask::Completed(void)noexcept(true)
{
	NotifyCompletion();
}
//---------------------------------------------------------------------------
void cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask::NotifyCompletion(void)noexcept(true)
{
	fTaskState.SetDone();
	//fSocketIO=nullptr;
	iDecReference(this,'task');
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask::SendTo(bcNTSocketOverlappedIOHandle *SocketIO,iAddress *RemoteAddress,const void *Buffer,uIntn Size)noexcept(true)
{
	auto saddr=fAddressBuffer.SetupSendAddress(RemoteAddress,fAddrLen);
	if(saddr==nullptr)
		return false;

	fSocketIO=SocketIO;
	BytesCompleted=0;

	iIncReference(this,'task');

	fRemoteAddress=RemoteAddress;


	fSocketIO->StartIO();
	WSABUF wb;
	wb.buf=static_cast<CHAR*>(const_cast<void*>(Buffer));
	wb.len=static_cast<ULONG>(Size);
	int SentRet=::WSASendTo(fSocketIO->Handle,&wb,1,&BytesCompleted,Flags,saddr,fAddrLen,&Overlapped,nullptr);
	DWORD SendErrorCode;
	if(SentRet==0 || (SendErrorCode=::WSAGetLastError())==WSA_IO_PENDING){
		// completed or in progress, wait for notification
		return true;
	}

	// error
	fSocketIO->CancelIO();
	ErrorCode=SendErrorCode;
	NotifyCompletion();
	return false;
}
//---------------------------------------------------------------------------
iAddress* cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask::GetRemoteAddress(void)noexcept(true)
{
	return fRemoteAddress;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleDatagramAsyncIORecvTask::RecvFrom(bcNTSocketOverlappedIOHandle *SocketIO,iSocketAddress *LocalAddress,void *Buffer,uIntn Size)noexcept(true)
{
	if(fAddressBuffer.SetupRecvAddress(LocalAddress)==false)
		return false;

	fSocketIO=SocketIO;
	BytesCompleted=0;
	Flags=0;

	iIncReference(this,'task');
	fAddrLen=static_cast<socklen_t>(fAddressBuffer.GetCapacity());
	
	fSocketIO->StartIO();
	WSABUF wb;
	wb.buf=static_cast<CHAR*>(Buffer);
	wb.len=static_cast<ULONG>(Size);
	int RecvRet=::WSARecvFrom(fSocketIO->Handle,&wb,1,&BytesCompleted,&Flags,fAddressBuffer.SockAddr(),&fAddrLen,&Overlapped,nullptr);
	DWORD RecvErrorCode;
	if(RecvRet==0 || (RecvErrorCode=::WSAGetLastError())==WSA_IO_PENDING){
		// completed ot in progress, wait for notification
		return true;
	}

	// error
	fSocketIO->CancelIO();
	ErrorCode=RecvErrorCode;
	NotifyCompletion();
	return false;
}
//---------------------------------------------------------------------------
void cNTSocketOverlappedIOHandleDatagramAsyncIORecvTask::Completed(void)noexcept(true)
{
	if(ErrorCode==ERROR_SUCCESS){
		fAddressBuffer.SetSockAddrLen(fAddrLen);

		fRemoteAddress=fAddressBuffer.CreateSocketAddress();
	}

	NotifyCompletion();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNTSocketOverlappedIOHandleDatagramStream::cNTSocketOverlappedIOHandleDatagramStream(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iSocketAddress *LocalAddress)noexcept(true)
	: fSocketIO(cnVar::MoveCast(SocketIO))
	, fLocalAddress(LocalAddress)
{
}
//---------------------------------------------------------------------------
cNTSocketOverlappedIOHandleDatagramStream::~cNTSocketOverlappedIOHandleDatagramStream()noexcept(true)
{
}
//---------------------------------------------------------------------------
void* cNTSocketOverlappedIOHandleDatagramStream::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	return iStreamErrorReport::CastInterface(InterfaceID);
}
//---------------------------------------------------------------------------
iSocketAddress*	cNTSocketOverlappedIOHandleDatagramStream::GetMessagePortAddress(void)noexcept(true)
{
	return fLocalAddress;
}
//---------------------------------------------------------------------------
void cNTSocketOverlappedIOHandleDatagramStream::Close(void)noexcept(true)
{
	fSocketIO->Close();
}
//---------------------------------------------------------------------------
eStreamError cNTSocketOverlappedIOHandleDatagramStream::GetStreamError(void)noexcept(true)
{
	DWORD Error=::GetLastError();
	return WinSocketErrorToStreamError(Error);
}
//---------------------------------------------------------------------------
iAddress* cNTSocketOverlappedIOHandleDatagramStream::GetLocalAddress(void)noexcept(true)
{
	return GetMessagePortAddress();
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleDatagramStream::Read(iPtr<iAddress> &Address,void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)
{
	cNTSocketOverlappedIOHandleSyncIOObject IOObject;
	cnMemory::ZeroFill(IOObject.Overlapped);
	IOObject.BytesCompleted=0;
	
	cWin32SocketAddressBuffer AddressBuffer;
	AddressBuffer.SetupRecvAddress(fLocalAddress);
	if(IOObject.RecvFrom(fSocketIO,AddressBuffer,Buffer,Size)==false)
		return false;

	SizeCompleted=IOObject.BytesCompleted;

	Address=AddressBuffer.CreateSocketAddress();

	return IOObject.ErrorCode==ERROR_SUCCESS;
}
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleDatagramStream::Write(iAddress *Address,const void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)
{
	cNTSocketOverlappedIOHandleSyncIOObject IOObject;

	cnMemory::ZeroFill(IOObject.Overlapped);
	IOObject.BytesCompleted=0;

	cWin32SocketAddressBuffer AddressBuffer;
	socklen_t addrlen;
	auto saddr=AddressBuffer.SetupSendAddress(Address,addrlen);
	if(saddr==nullptr)
		return false;

	if(IOObject.SendTo(fSocketIO,saddr,addrlen,Buffer,Size)==false)
		return false;

	SizeCompleted=IOObject.BytesCompleted;
	return IOObject.ErrorCode==ERROR_SUCCESS;
}
//---------------------------------------------------------------------------
iPtr<iMultipointTask> cNTSocketOverlappedIOHandleDatagramStream::ReadAsync(void *Buffer,uIntn Size)noexcept(true)
{
	auto Task=iCreate<cNTSocketOverlappedIOHandleDatagramAsyncIORecvTask>();
	if(Task->RecvFrom(fSocketIO,fLocalAddress,Buffer,Size)==false)
		return nullptr;

	return Task;
}
//---------------------------------------------------------------------------
iPtr<iMultipointTask> cNTSocketOverlappedIOHandleDatagramStream::WriteAsync(const void *Buffer,uIntn Size,iAddress *Address)noexcept(true)
{
	auto Task=iCreate<cNTSocketOverlappedIOHandleDatagramAsyncIOSendTask>();
	if(Task->SendTo(fSocketIO,Address,Buffer,Size)==false)
		return nullptr;

	return Task;
}
//---------------------------------------------------------------------------
uIntn cNTSocketOverlappedIOHandleDatagramStream::GetMaxPacketSize(void)noexcept(true)
{
	union{
		char data;
		unsigned int maxmsgsize;
		uIntn Result;
	};
	Result=0;
	int OptLen=4;
	getsockopt(fSocketIO->Handle,SOL_SOCKET,SO_MAX_MSG_SIZE,&data,&OptLen);
	return Result;
}
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cNTSocketOverlappedIOHandleConnectorParameter::Setup(iAddress *ConnLocalAddress,iAddress *ConnRemoteAddress)noexcept(true)
{
	if(ConnLocalAddress==nullptr){
		if(ConnRemoteAddress==nullptr)
			return false;
		
		// create any local address
		AddressFamily=Win32QueryAddressFamily(ConnRemoteAddress);

		cWin32SocketAddressBuffer AddressBuffer;
		if(AddressBuffer.SetAddressAny(static_cast<ADDRESS_FAMILY>(AddressFamily))==false)
			return false;

		BindAddress=AddressBuffer.CreateSocketAddress();
		if(BindAddress==nullptr)
			return false;
	}
	else{
		BindAddress=Win32CreateCopySocketAddress(ConnLocalAddress);
		if(BindAddress==nullptr)
			return false;

		auto LocalSockaddr=BindAddress->SockAddr();
		AddressFamily=LocalSockaddr->sa_family;
	}
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
SOCKET cNTSocketOverlappedIOHandleListenerParameter::Setup(iAddress *Address,int Type,int Protocol,int Flag)noexcept(true)
{
	if(Address==nullptr)
		return INVALID_SOCKET;

	cWin32SocketAddressBuffer AddressBuffer;

	socklen_t addrlen;
	auto addr=AddressBuffer.SetupSendAddress(Address,addrlen);
	if(addr==nullptr)
		return INVALID_SOCKET;

	SOCKET SocketHandle=::WSASocketW(addr->sa_family,Type,Protocol,NULL,0,Flag);
	if(SocketHandle==INVALID_SOCKET){
		//create socket failed
		return INVALID_SOCKET;
	}

	// bind address
	if(::bind(SocketHandle,addr,addrlen)==SOCKET_ERROR){
		::closesocket(SocketHandle);
		return INVALID_SOCKET;
	}
	// start listen
	if(::listen(SocketHandle,SOMAXCONN)==SOCKET_ERROR){
		::closesocket(SocketHandle);
		return INVALID_SOCKET;
	}

	DWORD FuncPtrSize;
	GUID FunctionID;
	
	FunctionID=WSAID_ACCEPTEX;
	if(::WSAIoctl(SocketHandle,SIO_GET_EXTENSION_FUNCTION_POINTER,&FunctionID,sizeof(FunctionID),&AcceptEx,sizeof(AcceptEx),&FuncPtrSize,nullptr,nullptr)!=0){
		::closesocket(SocketHandle);
		return INVALID_SOCKET;
	}
	
	FunctionID=WSAID_GETACCEPTEXSOCKADDRS;
	if(::WSAIoctl(SocketHandle,SIO_GET_EXTENSION_FUNCTION_POINTER,&FunctionID,sizeof(FunctionID),&GetAcceptExSockaddrs,sizeof(GetAcceptExSockaddrs),&FuncPtrSize,nullptr,nullptr)!=0){
		::closesocket(SocketHandle);
		return INVALID_SOCKET;
	}

	BoundAddress=Win32CreateCopySocketAddress(addr,addrlen);
	if(BoundAddress==nullptr){
		::closesocket(SocketHandle);
		return INVALID_SOCKET;
	}
	return SocketHandle;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcNTSocketOverlappedIOHandleConnectionConnector::cSyncConnectObject::cSyncConnectObject()noexcept(true)
{
}
//---------------------------------------------------------------------------
bcNTSocketOverlappedIOHandleConnectionConnector::cSyncConnectObject::~cSyncConnectObject()noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIOHandleConnectionConnector::cSyncConnectObject::Completed(void)noexcept(true)
{
	fNotifier.Notify();
}
//---------------------------------------------------------------------------
bool bcNTSocketOverlappedIOHandleConnectionConnector::cSyncConnectObject::Connect(LPFN_CONNECTEX fpConnectEx,bcNTSocketOverlappedIOHandle *SocketIO,iSocketAddress *RemoteAddress)noexcept(true)
{
	SocketIO->StartIO();

	cnMemory::ZeroFill(Overlapped);
	BOOL ConnectRet=(*fpConnectEx)(SocketIO->Handle,RemoteAddress->SockAddr(),RemoteAddress->SockAddrLen(),nullptr,0,&BytesCompleted,&Overlapped);
	DWORD ConnectErrorCode;
	if(ConnectRet || (ConnectErrorCode=::WSAGetLastError())==ERROR_IO_PENDING){
		// completed or in progress, wait for notification
		fNotifier.Wait();
		return true;
	}

	ErrorCode=ConnectErrorCode;
	SocketIO->CancelIO();
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcNTSocketOverlappedIOHandleConnectionConnector::cAsyncConnectionTask::cAsyncConnectionTask(bcNTSocketOverlappedIOHandleConnectionConnector *Owner)noexcept(true)
	: fOwner(Owner)
{
}
//---------------------------------------------------------------------------
bcNTSocketOverlappedIOHandleConnectionConnector::cAsyncConnectionTask::~cAsyncConnectionTask()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool bcNTSocketOverlappedIOHandleConnectionConnector::cAsyncConnectionTask::IsDone(void)noexcept(true)
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool bcNTSocketOverlappedIOHandleConnectionConnector::cAsyncConnectionTask::Await(iProcedure *NotifyProcedure)noexcept(true)
{
	return fTaskState.Await(NotifyProcedure);
}
//---------------------------------------------------------------------------
iConnection* bcNTSocketOverlappedIOHandleConnectionConnector::cAsyncConnectionTask::GetConnection(void)noexcept(true)
{
	return fConnection;
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIOHandleConnectionConnector::cAsyncConnectionTask::Cancel(void)noexcept(true)
{
	if(fTaskState.IsDone())
		return;

	::CancelIoEx(reinterpret_cast<HANDLE>(fSocketIO->Handle),&Overlapped);
}
//---------------------------------------------------------------------------
bool bcNTSocketOverlappedIOHandleConnectionConnector::cAsyncConnectionTask::Connect(LPFN_CONNECTEX fpConnectEx,rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iPtr<iSocketAddress> RemoteAddress)noexcept(true)
{
	fSocketIO=cnVar::MoveCast(SocketIO);
	fRemoteAddress=cnVar::MoveCast(RemoteAddress);
	cnMemory::ZeroFill(Overlapped);

	iIncReference(this,'task');

	fSocketIO->StartIO();
	
	BOOL ConnectRet=(*fpConnectEx)(fSocketIO->Handle,fRemoteAddress->SockAddr(),fRemoteAddress->SockAddrLen(),nullptr,0,&BytesCompleted,&Overlapped);
	DWORD ConnectErrorCode;
	if(ConnectRet || (ConnectErrorCode=::WSAGetLastError())==WSA_IO_PENDING){
		// completed or in progress, wait for notification
		return true;
	}

	// error
	fSocketIO->CancelIO();
	ErrorCode=ConnectErrorCode;
	NotifyCompletion();
	return false;
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIOHandleConnectionConnector::cAsyncConnectionTask::NotifyCompletion(void)noexcept(true)
{
	fTaskState.SetDone();
	iDecReference(this,'task');
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIOHandleConnectionConnector::cAsyncConnectionTask::Completed(void)noexcept(true)
{
	if(ErrorCode==ERROR_SUCCESS){
		if(::setsockopt(fSocketIO->Handle,SOL_SOCKET,SO_UPDATE_CONNECT_CONTEXT,nullptr,0)!=0){
			ErrorCode=::WSAGetLastError();
			fSocketIO->Close();
		}
		else{
			fConnection=fOwner->ConnectorMakeConnection(cnVar::MoveCast(fSocketIO),cnVar::MoveCast(fRemoteAddress));
		}
	}
	NotifyCompletion();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcNTSocketOverlappedIOHandleConnectionConnector::bcNTSocketOverlappedIOHandleConnectionConnector(iPtr<iSocketAddress> BindAddress)noexcept(true)
	: fBindAddress(cnVar::MoveCast(BindAddress))
{
	
}
//---------------------------------------------------------------------------
bcNTSocketOverlappedIOHandleConnectionConnector::~bcNTSocketOverlappedIOHandleConnectionConnector()noexcept(true)
{
}
//---------------------------------------------------------------------------
iAddress* bcNTSocketOverlappedIOHandleConnectionConnector::GetLocalAddress(void)noexcept(true)
{
	return fBindAddress;
}
//---------------------------------------------------------------------------
iPtr<iConnection> bcNTSocketOverlappedIOHandleConnectionConnector::Connect(iAddress *RemoteAddress)noexcept(true)
{
	auto RemoteSocketAddress=Win32CreateCopySocketAddress(RemoteAddress);
	if(RemoteSocketAddress==nullptr)
		return nullptr;

	LPFN_CONNECTEX fpConnectEx;
	auto SocketIO=ConnectorQueryIOHandle(fpConnectEx);
	if(SocketIO==nullptr)
		return nullptr;

	cSyncConnectObject IOObject;

	if(IOObject.Connect(fpConnectEx,SocketIO,RemoteSocketAddress)==false){
		return nullptr;
	}

	if(::setsockopt(SocketIO->Handle,SOL_SOCKET,SO_UPDATE_CONNECT_CONTEXT,nullptr,0)!=0){
		//ErrorCode=::WSAGetLastError();
		SocketIO->Close();
		return nullptr;
	}
	return ConnectorMakeConnection(cnVar::MoveCast(SocketIO),cnVar::MoveCast(RemoteSocketAddress));
}
//---------------------------------------------------------------------------
iPtr<iConnectionTask> bcNTSocketOverlappedIOHandleConnectionConnector::ConnectAsync(iAddress *RemoteAddress)noexcept(true)
{
	auto RemoteSocketAddress=Win32CreateCopySocketAddress(RemoteAddress);
	if(RemoteSocketAddress==nullptr)
		return nullptr;

	LPFN_CONNECTEX fpConnectEx;
	auto SocketIO=ConnectorQueryIOHandle(fpConnectEx);
	if(SocketIO==nullptr)
		return nullptr;

	auto ConnectTask=iCreate<cAsyncConnectionTask>(this);
	if(ConnectTask->Connect(fpConnectEx,cnVar::MoveCast(SocketIO),cnVar::MoveCast(RemoteSocketAddress))==false){
		return nullptr;
	}

	return ConnectTask;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcNTSocketOverlappedIOHandleConnectionListener::cSyncAcceptObject::cSyncAcceptObject()noexcept(true)
{
}
//---------------------------------------------------------------------------
bcNTSocketOverlappedIOHandleConnectionListener::cSyncAcceptObject::~cSyncAcceptObject()noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIOHandleConnectionListener::cSyncAcceptObject::Completed(void)noexcept(true)
{
	fNotifier.Notify();
}
//---------------------------------------------------------------------------
bool bcNTSocketOverlappedIOHandleConnectionListener::cSyncAcceptObject::Accept(bcNTSocketOverlappedIOHandleConnectionListener *Owner,bcNTSocketOverlappedIOHandle *AcceptSocketIO,void *AddressBuffer,DWORD LocalAddressLength,DWORD RemoteAddressLength)noexcept(true)
{
	cnMemory::ZeroFill(Overlapped);

	Owner->fSocketIO->StartIO();

	BOOL AcceptRet=(*Owner->fAcceptEx)(Owner->fSocketIO->Handle,AcceptSocketIO->Handle,AddressBuffer,0,LocalAddressLength,RemoteAddressLength,&BytesCompleted,&Overlapped);
	DWORD AcceptErrorCode;
	if(AcceptRet || (AcceptErrorCode=::WSAGetLastError())==WSA_IO_PENDING){
		// complated or in progress, wait for notification
		fNotifier.Wait();
		return true;
	}
	Owner->fSocketIO->CancelIO();
	ErrorCode=AcceptErrorCode;
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcNTSocketOverlappedIOHandleConnectionListener::cAsyncAcceptTask::cAsyncAcceptTask(bcNTSocketOverlappedIOHandleConnectionListener *Owner)noexcept(true)
	: fOwner(Owner)
{
	fAcceptSocket=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
bcNTSocketOverlappedIOHandleConnectionListener::cAsyncAcceptTask::~cAsyncAcceptTask()noexcept(true)
{
	if(fAcceptSocket!=INVALID_SOCKET)
		::closesocket(fAcceptSocket);
}
//---------------------------------------------------------------------------
bool	bcNTSocketOverlappedIOHandleConnectionListener::cAsyncAcceptTask::IsDone(void)noexcept(true)
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool bcNTSocketOverlappedIOHandleConnectionListener::cAsyncAcceptTask::Await(iProcedure *NotifyProcedure)noexcept(true)
{
	return fTaskState.Await(NotifyProcedure);
}
//---------------------------------------------------------------------------
iConnection* bcNTSocketOverlappedIOHandleConnectionListener::cAsyncAcceptTask::GetConnection(void)noexcept(true)
{
	return fConnection;
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIOHandleConnectionListener::cAsyncAcceptTask::Cancel(void)noexcept(true)
{
	if(fTaskState.IsDone())
		return;

	::CancelIoEx(reinterpret_cast<HANDLE>(fOwner->fSocketIO->Handle),&Overlapped);
}
//---------------------------------------------------------------------------
bool bcNTSocketOverlappedIOHandleConnectionListener::cAsyncAcceptTask::Accept(rPtr<bcNTSocketOverlappedIOHandle> AcceptSocketIO,int LocalAddressLength)noexcept(true)
{
	fAcceptSocketIO=cnVar::MoveCast(AcceptSocketIO);

	cnMemory::ZeroFill(Overlapped);

	fLocalBufferLength=LocalAddressLength+16;
	fRemoteBufferLength=LocalAddressLength+16;
	fAddressBuffer.SetLength(fLocalBufferLength+fRemoteBufferLength);
	iIncReference(this,'task');

	fOwner->fSocketIO->StartIO();

	BOOL AcceptRet=(*fOwner->fAcceptEx)(fOwner->fSocketIO->Handle,fAcceptSocketIO->Handle,fAddressBuffer,0,fLocalBufferLength,fRemoteBufferLength,&BytesCompleted,&Overlapped);
	DWORD AcceptErrorCode;
	if(AcceptRet || (AcceptErrorCode=::WSAGetLastError())==WSA_IO_PENDING){
		// complated or in progress, wait for notification
		return true;
	}
	fOwner->fSocketIO->CancelIO();
	ErrorCode=AcceptErrorCode;
	NotifyCompletion();
	return false;
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIOHandleConnectionListener::cAsyncAcceptTask::NotifyCompletion(void)noexcept(true)
{
	fTaskState.SetDone();
	iDecReference(this,'task');
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIOHandleConnectionListener::cAsyncAcceptTask::Completed(void)noexcept(true)
{
	if(ErrorCode==ERROR_SUCCESS){
		fConnection=fOwner->MakeAcceptedConnection(cnVar::MoveCast(fAcceptSocketIO),fAddressBuffer,fLocalBufferLength,fRemoteBufferLength);
	}

	NotifyCompletion();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcNTSocketOverlappedIOHandleConnectionListener::bcNTSocketOverlappedIOHandleConnectionListener(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,cNTSocketOverlappedIOHandleListenerParameter &Parameter)noexcept(true)
	: fSocketIO(cnVar::MoveCast(SocketIO))
	, fLocalAddress(cnVar::MoveCast(Parameter.BoundAddress))
	, fAcceptEx(Parameter.AcceptEx)
	, fGetAcceptExSockaddrs(Parameter.GetAcceptExSockaddrs)
{
}
//---------------------------------------------------------------------------
bcNTSocketOverlappedIOHandleConnectionListener::~bcNTSocketOverlappedIOHandleConnectionListener()noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIOHandleConnectionListener::VirtualStopped(void)noexcept(true)
{
	fSocketIO->Close();

	InnerDecReference('self');
}
//---------------------------------------------------------------------------
iPtr<iConnection> bcNTSocketOverlappedIOHandleConnectionListener::MakeAcceptedConnection(rPtr<bcNTSocketOverlappedIOHandle> AcceptSocketIO,void *AddressBuffer,int LocalAddressLength,int RemoteAddressLength)noexcept(true)
{
	sockaddr *laddr;
	int laddrlen;
	sockaddr *raddr;
	int raddrlen;

	(*fGetAcceptExSockaddrs)(AddressBuffer,0,LocalAddressLength,RemoteAddressLength,&laddr,&laddrlen,&raddr,&raddrlen);

	auto LocalAddress=Win32CreateCopySocketAddress(laddr,laddrlen);
	auto RemoteAddress=Win32CreateCopySocketAddress(raddr,raddrlen);

	if(::setsockopt(AcceptSocketIO->Handle,SOL_SOCKET,SO_UPDATE_ACCEPT_CONTEXT,reinterpret_cast<char*>(&fSocketIO->Handle),sizeof(fSocketIO->Handle))!=0){
		AcceptSocketIO->Close();
		return nullptr;
	}

	return ListenerMakeConnection(cnVar::MoveCast(AcceptSocketIO),cnVar::MoveCast(LocalAddress),cnVar::MoveCast(RemoteAddress));
}
//---------------------------------------------------------------------------
iAddress* bcNTSocketOverlappedIOHandleConnectionListener::GetLocalAddress(void)noexcept(true)
{
	return fLocalAddress;
}
//---------------------------------------------------------------------------
iPtr<iConnection> bcNTSocketOverlappedIOHandleConnectionListener::Accept(void)noexcept(true)
{
	auto AcceptSocketIO=ListenerQueryIOHandle();
	if(AcceptSocketIO==nullptr)
		return nullptr;

	cSyncAcceptObject IOObject;

	cMemoryBlock AddressBuffer;
	DWORD addrlen=static_cast<DWORD>(fLocalAddress->SockAddrLen());
	addrlen+=16;

	AddressBuffer.SetLength(addrlen+addrlen);

	if(IOObject.Accept(this,AcceptSocketIO,AddressBuffer,addrlen,addrlen)==false){
		return nullptr;
	}

	if(IOObject.ErrorCode!=ERROR_SUCCESS)
		return nullptr;

	return MakeAcceptedConnection(cnVar::MoveCast(AcceptSocketIO),AddressBuffer,addrlen,addrlen);
}
//---------------------------------------------------------------------------
iPtr<iConnectionTask> bcNTSocketOverlappedIOHandleConnectionListener::AcceptAsync(void)noexcept(true)
{
	auto AcceptSocketIO=ListenerQueryIOHandle();
	if(AcceptSocketIO==nullptr)
		return nullptr;

	auto AcceptTask=iCreate<cAsyncAcceptTask>(this);

	if(AcceptTask->Accept(cnVar::MoveCast(AcceptSocketIO),fLocalAddress->SockAddrLen())==false){
		return nullptr;
	}

	return AcceptTask;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcNTSocketOverlappedIORecyclableHandleManager::cDisconnectTask::cDisconnectTask(bcNTSocketOverlappedIORecyclableHandleManager *Owner)noexcept(true)
	: fOwner(Owner)
{
}
//---------------------------------------------------------------------------
bcNTSocketOverlappedIORecyclableHandleManager::cDisconnectTask::~cDisconnectTask()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool bcNTSocketOverlappedIORecyclableHandleManager::cDisconnectTask::Disconnect(void)noexcept(true)
{
	cnMemory::ZeroFill(Overlapped);
	SocketIO->StartIO();

	BOOL DisconnectRet=(*fpDisconnectEx)(SocketIO->Handle,&Overlapped,TF_REUSE_SOCKET,0);
	DWORD DisconnectError;
	if(DisconnectRet || (DisconnectError=::WSAGetLastError())==WSA_IO_PENDING){
		// completed or in progress, wait for notification
		return true;
	}

	SocketIO->CancelIO();

	ErrorCode=DisconnectError;
	return false;
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIORecyclableHandleManager::cDisconnectTask::Completed(void)noexcept(true)
{
	fOwner->DisconnectTaskDone(this);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcNTSocketOverlappedIORecyclableHandleManager::bcNTSocketOverlappedIORecyclableHandleManager()noexcept(true)
	: fClosed(false)
{
}
//---------------------------------------------------------------------------
bcNTSocketOverlappedIORecyclableHandleManager::~bcNTSocketOverlappedIORecyclableHandleManager()noexcept(true)
{
	cnLib_ASSERT(fDisconnectQueue.IsEmpty());
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIORecyclableHandleManager::VirtualStopped(void)noexcept(true)
{
	fClosed=true;
	
	QueueProcedure();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIORecyclableHandleManager::QueueProcedure(void)noexcept(true)
{
	if(fCheckQueueFlag.Acquire()==false)
		return;

	InnerIncReference('chkq');

	do{
		fCheckQueueFlag.Continue();

		if(fClosed){
			QueueProcessCancel();
		}
		else{
			QueueProcessDisconnect();
		}

		QueueProcessDone();

	}while(fCheckQueueFlag.Release()==false);

	InnerDecReference('chkq');
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIORecyclableHandleManager::QueueProcessDisconnect(void)noexcept(true)
{
	auto DisconnectTask=fDisconnectQueue.DequeueAll();
	while(DisconnectTask!=nullptr){
		auto CurTask=DisconnectTask;
		DisconnectTask=DisconnectTask->Next;

		if(CurTask->Disconnect()==false){
			// error disconnecting
			SocketDisconnectError(CurTask->SocketIO);
			delete CurTask;
		}

		fDisconnectSet.Insert(CurTask);
	}
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIORecyclableHandleManager::QueueProcessCancel(void)noexcept(true)
{
	auto DisconnectTask=fDisconnectQueue.DequeueAll();
	while(DisconnectTask!=nullptr){
		auto CurTask=DisconnectTask;
		DisconnectTask=DisconnectTask->Next;

		SocketDisconnectError(CurTask->SocketIO);
		delete CurTask;
	}

	for(auto *DisconnectingTask : fDisconnectSet){
		DisconnectingTask->SocketIO->CancelOperation(DisconnectingTask);
	}
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIORecyclableHandleManager::QueueProcessDone(void)noexcept(true)
{
	auto DisconnectTask=fDisconnectDoneQueue.DequeueAll();
	while(DisconnectTask!=nullptr){
		auto CurTask=DisconnectTask;
		DisconnectTask=DisconnectTask->Next;

		fDisconnectSet.Remove(CurTask);

		if(CurTask->ErrorCode!=ERROR_SUCCESS){
			// error disconnecting
			SocketDisconnectError(CurTask->SocketIO);
		}
		else{
			SocketDisconnected(CurTask->SocketIO);
		}
		delete CurTask;
	}
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIORecyclableHandleManager::DisconnectSocket(bcNTSocketOverlappedIOHandle *SocketIO,LPFN_DISCONNECTEX DisconnectEx)noexcept(true)
{
	auto Task=new cDisconnectTask(this);
	Task->SocketIO=SocketIO;
	Task->fpDisconnectEx=DisconnectEx;
	fDisconnectQueue.Enqueue(Task);
	QueueProcedure();
}
//---------------------------------------------------------------------------
void bcNTSocketOverlappedIORecyclableHandleManager::DisconnectTaskDone(cDisconnectTask *Task)noexcept(true)
{
	fDisconnectDoneQueue.Enqueue(Task);
	QueueProcedure();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNT6ThreadPoolSocketIOHandle::cNT6ThreadPoolSocketIOHandle()noexcept(true)
	: fIOThreadPool(nullptr)
{
}
//---------------------------------------------------------------------------
cNT6ThreadPoolSocketIOHandle::~cNT6ThreadPoolSocketIOHandle()noexcept(true)
{
	if(Handle!=INVALID_SOCKET){
		::CloseThreadpoolIo(fIOThreadPool);
		::closesocket(Handle);
		Handle=INVALID_SOCKET;
	}
}
//---------------------------------------------------------------------------
bool cNT6ThreadPoolSocketIOHandle::Open(SOCKET SocketHandle)noexcept(true)
{
	fIOThreadPool=::CreateThreadpoolIo(reinterpret_cast<HANDLE>(SocketHandle),IoCompletionCallback,this,nullptr);
	if(fIOThreadPool==nullptr)
		return false;

	Handle=SocketHandle;
	return true;
}
//---------------------------------------------------------------------------
void cNT6ThreadPoolSocketIOHandle::Close(void)noexcept(true)
{
	cnLib_ASSERT(Handle!=INVALID_SOCKET);

	::CloseThreadpoolIo(fIOThreadPool);
	::closesocket(Handle);
	Handle=INVALID_SOCKET;
}
//---------------------------------------------------------------------------
VOID CALLBACK cNT6ThreadPoolSocketIOHandle::IoCompletionCallback(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_opt_ PVOID                 Overlapped,
	_In_        ULONG                 IoResult,
	_In_        ULONG_PTR             NumberOfBytesTransferred,
	_Inout_     PTP_IO                Io
)noexcept(true){
Instance,Context,Io;
	auto IOObject=cnMemory::GetObjectFromMemberPointer(static_cast<OVERLAPPED*>(Overlapped),&bcIOObject::Overlapped);
	IOObject->ErrorCode=IoResult;
	IOObject->BytesCompleted=NumberOfBytesTransferred;
	IOObject->Completed();
}
//---------------------------------------------------------------------------
void cNT6ThreadPoolSocketIOHandle::StartIO(void)noexcept(true)
{
	::StartThreadpoolIo(fIOThreadPool);
}
//---------------------------------------------------------------------------
void cNT6ThreadPoolSocketIOHandle::CancelIO(void)noexcept(true)
{
	::CancelThreadpoolIo(fIOThreadPool);
}
//---------------------------------------------------------------------------
void cNT6ThreadPoolSocketIOHandle::CancelOperation(bcIOObject *Object)noexcept(true)
{
	::CancelIoEx(reinterpret_cast<HANDLE>(Handle),&Object->Overlapped);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iConnection> cnWinRTL::NT6ThreadPoolSocketMakeStreamConnection(rPtr<cNT6ThreadPoolSocketIOHandle> SocketIO,iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress)noexcept(true)
{	
	auto AggregableStream=iAggregableCreate<cNTSocketOverlappedIOHandleStream>(cnVar::MoveCast(SocketIO));
	auto AggregableConnection=iAggregableCreate<cConnection>();
	AggregableConnection->LocalAddress=cnVar::MoveCast(LocalAddress);
	AggregableConnection->RemoteAddress=cnVar::MoveCast(RemoteAddress);
	auto ConnectionAgg=iAggregateCreate(cnVar::MoveCast(AggregableStream),cnVar::MoveCast(AggregableConnection));
	return iCast<iConnection>(ConnectionAgg);
}
//---------------------------------------------------------------------------
iPtr<iConnection> cnWinRTL::NT6ThreadPoolSocketMakeEndpointConnection(rPtr<cNT6ThreadPoolSocketIOHandle> SocketIO,iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress)noexcept(true)
{
	auto Stream=iCreate<cNTSocketOverlappedIOHandleStream>(cnVar::MoveCast(SocketIO));
	auto AggregableEndpoint=iAggregableCreate<cEndpointFromStream>(Stream.Reference(),Stream);
	auto AggregableConnection=iAggregableCreate<cConnection>();
	AggregableConnection->LocalAddress=cnVar::MoveCast(LocalAddress);
	AggregableConnection->RemoteAddress=cnVar::MoveCast(RemoteAddress);
	auto ConnectionAgg=iAggregateCreate(cnVar::MoveCast(AggregableEndpoint),cnVar::MoveCast(AggregableConnection));
	return iCast<iConnection>(ConnectionAgg);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cNT6ThreadPoolRecyclableSocketIOHandle::Open(SOCKET SocketHandle)noexcept(true)
{
	GUID FuncID;
	DWORD FuncPtrSize;

	FuncID=WSAID_DISCONNECTEX;
	if(::WSAIoctl(SocketHandle,SIO_GET_EXTENSION_FUNCTION_POINTER,&FuncID,sizeof(FuncID),&fpDisconnectEx,sizeof(fpDisconnectEx),&FuncPtrSize,nullptr,nullptr)!=0){
		return false;
	}
	if(FuncPtrSize!=sizeof(fpDisconnectEx))
		return false;
	return cNT6ThreadPoolSocketIOHandle::Open(SocketHandle);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cNT6ThreadPoolClientSocketIOHandle::Open(SOCKET SocketHandle)noexcept(true)
{
	GUID FuncID;
	DWORD FuncPtrSize;

	FuncID=WSAID_CONNECTEX;
	if(::WSAIoctl(SocketHandle,SIO_GET_EXTENSION_FUNCTION_POINTER,&FuncID,sizeof(FuncID),&fpConnectEx,sizeof(fpConnectEx),&FuncPtrSize,nullptr,nullptr)!=0){
		return false;
	}
	if(FuncPtrSize!=sizeof(fpConnectEx))
		return false;
	return cNT6ThreadPoolRecyclableSocketIOHandle::Open(SocketHandle);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNT6ThreadPoolSocketConnectionConnector::cNT6ThreadPoolSocketConnectionConnector(iPtr<iSocketAddress> BindAddress,NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,int SocketAddressFamily,int SocketType,int SocketProtocol)noexcept(true)
	: bcNTSocketOverlappedIOHandleConnectionConnector(cnVar::MoveCast(BindAddress))
	, fMakeConnectionFunction(MakeConnectionFunction)
	, fSocketAddressFamily(SocketAddressFamily)
	, fSocketType(SocketType)
	, fSocketProtocol(SocketProtocol)
{
}
//---------------------------------------------------------------------------
cNT6ThreadPoolSocketConnectionConnector::~cNT6ThreadPoolSocketConnectionConnector()noexcept(true)
{
}
//---------------------------------------------------------------------------
rPtr<bcNTSocketOverlappedIOHandle> cNT6ThreadPoolSocketConnectionConnector::ConnectorQueryIOHandle(LPFN_CONNECTEX &fpConnectEx)noexcept(true)
{
	// get cache
	auto SocketIO=fSocketHandleRecyclePool.Query();
	if(SocketIO->Handle!=INVALID_SOCKET){
		fpConnectEx=SocketIO->fpConnectEx;
		return SocketIO;
	}

	// no cache

	SOCKET NewSocket=::WSASocketW(fSocketAddressFamily,fSocketType,fSocketProtocol,NULL,0,WSA_FLAG_OVERLAPPED);
	if(NewSocket==INVALID_SOCKET)
		return nullptr;

	if(::bind(NewSocket,fBindAddress->SockAddr(),fBindAddress->SockAddrLen())!=0){
		// error binding address
		::closesocket(NewSocket);
		return nullptr;
	}

	if(SocketIO->Open(NewSocket)==false){
		::closesocket(NewSocket);
		return nullptr;
	}

	fpConnectEx=SocketIO->fpConnectEx;
	return SocketIO;
}
//---------------------------------------------------------------------------
iPtr<iConnection> cNT6ThreadPoolSocketConnectionConnector::ConnectorMakeConnection(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iPtr<iSocketAddress> RemoteAddress)noexcept(true)
{
	auto RecyclableIOHandle=static_cast<cNT6ThreadPoolClientSocketIOHandle*>(SocketIO.Pointer());
	RecyclableIOHandle->Connected=true;
	if(RecyclableIOHandle->BoundAddress==nullptr){
		cWin32SocketAddressBuffer AddressBuffer;
		if(AddressBuffer.GetBoundAddress(RecyclableIOHandle->Handle,fBindAddress->SockAddr()->sa_family)){
			RecyclableIOHandle->BoundAddress=AddressBuffer.CreateSocketAddress();
		}
	}
	return (*fMakeConnectionFunction)(RecyclableIOHandle,RecyclableIOHandle->BoundAddress,cnVar::MoveCast(RemoteAddress));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNT6ThreadPoolSocketConnectionListener::cNT6ThreadPoolSocketConnectionListener(rPtr<cNT6ThreadPoolSocketIOHandle> SocketIO,cNTSocketOverlappedIOHandleListenerParameter &Parameter,
	NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,int SocketType,int SocketProtocol
)noexcept(true)
	: bcNTSocketOverlappedIOHandleConnectionListener(cnVar::MoveCast(SocketIO),Parameter)
	, fMakeConnectionFunction(MakeConnectionFunction)
	, fSocketType(SocketType)
	, fSocketProtocol(SocketProtocol)
{
}
//---------------------------------------------------------------------------
cNT6ThreadPoolSocketConnectionListener::~cNT6ThreadPoolSocketConnectionListener()noexcept(true)
{
}
//---------------------------------------------------------------------------
rPtr<bcNTSocketOverlappedIOHandle> cNT6ThreadPoolSocketConnectionListener::ListenerQueryIOHandle(void)noexcept(true)
{
	// get cache
	auto SocketIO=fSocketHandleRecyclePool.Query();
	if(SocketIO->Handle!=INVALID_SOCKET){
		return SocketIO;
	}
	auto LocalAddr=fLocalAddress->SockAddr();

	// no cache

	SOCKET NewSocket=::WSASocketW(LocalAddr->sa_family,fSocketType,fSocketProtocol,NULL,0,WSA_FLAG_OVERLAPPED);
	if(NewSocket==INVALID_SOCKET)
		return nullptr;

	if(SocketIO->Open(NewSocket)==false){
		::closesocket(NewSocket);
		return nullptr;
	}

	return SocketIO;
}
//---------------------------------------------------------------------------
iPtr<iConnection> cNT6ThreadPoolSocketConnectionListener::ListenerMakeConnection(rPtr<bcNTSocketOverlappedIOHandle> SocketIO,iPtr<iSocketAddress> LocalAddress,iPtr<iSocketAddress> RemoteAddress)noexcept(true)
{
	auto RecyclableIOHandle=static_cast<cNT6ThreadPoolRecyclableSocketIOHandle*>(SocketIO.Pointer());
	RecyclableIOHandle->Connected=true;

	return (*fMakeConnectionFunction)(RecyclableIOHandle,cnVar::MoveCast(LocalAddress),cnVar::MoveCast(RemoteAddress));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNT6ThreadPoolSocketStreamProtocol::cNT6ThreadPoolSocketStreamProtocol(int Type,int Protocol)noexcept(true)
	: fType(Type)
	, fProtocol(Protocol)
{
}
//---------------------------------------------------------------------------
cNT6ThreadPoolSocketStreamProtocol::~cNT6ThreadPoolSocketStreamProtocol()noexcept(true)
{
}
//---------------------------------------------------------------------------
iPtr<iConnectionConnector> cNT6ThreadPoolSocketStreamProtocol::MakeConnector(NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true)
{
	iPtr<iSocketAddress> LocalSocketAddress;
	int AddressFamily;
	if(LocalAddress==nullptr){
		if(RemoteAddress==nullptr)
			return nullptr;
		
		// create any local address
		AddressFamily=Win32QueryAddressFamily(RemoteAddress);

		cWin32SocketAddressBuffer AddressBuffer;
		if(AddressBuffer.SetAddressAny(static_cast<ADDRESS_FAMILY>(AddressFamily))==false)
			return nullptr;

		LocalSocketAddress=AddressBuffer.CreateSocketAddress();
		if(LocalSocketAddress==nullptr)
			return nullptr;
	}
	else{
		LocalSocketAddress=Win32CreateCopySocketAddress(LocalAddress);
		if(LocalSocketAddress==nullptr)
			return nullptr;

		auto LocalSockaddr=LocalSocketAddress->SockAddr();
		AddressFamily=LocalSockaddr->sa_family;
	}

	
	return iCreate<cNT6ThreadPoolSocketConnectionConnector>(cnVar::MoveCast(LocalSocketAddress),MakeConnectionFunction,AddressFamily,fType,fProtocol);
}
//---------------------------------------------------------------------------
iPtr<iConnectionConnector>	cNT6ThreadPoolSocketStreamProtocol::CreateStreamConnector(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true)
{
	return MakeConnector(NT6ThreadPoolSocketMakeStreamConnection,LocalAddress,RemoteAddress);
}
//---------------------------------------------------------------------------
iPtr<iConnectionConnector>	cNT6ThreadPoolSocketStreamProtocol::CreateEndpointConnector(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true)
{
	return MakeConnector(NT6ThreadPoolSocketMakeEndpointConnection,LocalAddress,RemoteAddress);
}
//---------------------------------------------------------------------------
iPtr<iConnectionListener> cNT6ThreadPoolSocketStreamProtocol::MakeListener(NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,iAddress *LocalAddress)noexcept(true)
{
	cNTSocketOverlappedIOHandleListenerParameter ListenerParameter;

	SOCKET NewSocket=ListenerParameter.Setup(LocalAddress,fType,fProtocol,WSA_FLAG_OVERLAPPED);
	if(NewSocket==INVALID_SOCKET){
		//create socket failed
		return nullptr;
	}

	auto SocketIO=rCreate<cNT6ThreadPoolSocketIOHandle>();
	if(SocketIO->Open(NewSocket)==false){
		::closesocket(NewSocket);
		return nullptr;
	}

	auto AsyncListener=rCreate<cNT6ThreadPoolSocketConnectionListener>(cnVar::MoveCast(SocketIO),ListenerParameter,MakeConnectionFunction,fType,fProtocol);
	return iCreate<cConnectionListenerFromAsyncListener>(AsyncListener);
}
//---------------------------------------------------------------------------
iPtr<iConnectionListener>	cNT6ThreadPoolSocketStreamProtocol::CreateStreamListener(iAddress *LocalAddress)noexcept(true)
{
	return MakeListener(NT6ThreadPoolSocketMakeStreamConnection,LocalAddress);
}
//---------------------------------------------------------------------------
iPtr<iConnectionListener>	cNT6ThreadPoolSocketStreamProtocol::CreateEndpointListener(iAddress *LocalAddress)noexcept(true)
{
	return MakeListener(NT6ThreadPoolSocketMakeEndpointConnection,LocalAddress);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNT6ThreadPoolSocketDatagramProtocol::cNT6ThreadPoolSocketDatagramProtocol(int Type,int Protocol)noexcept(true)
	: fType(Type)
	, fProtocol(Protocol)
{
}
//---------------------------------------------------------------------------
cNT6ThreadPoolSocketDatagramProtocol::~cNT6ThreadPoolSocketDatagramProtocol()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool cNT6ThreadPoolSocketDatagramProtocol::SetupAddress(SOCKET s,iSocketAddress *SocketAddress)noexcept(true)
{
	auto AddrLen=SocketAddress->SockAddrLen();
	auto Addr=SocketAddress->SockAddr();


	
	// check if multicast
	auto MulticastAddress=iCast<iWinSocketMulticastAddress>(SocketAddress);
	if(MulticastAddress!=nullptr){
		// set reuse address
		int reuse = 1;
		if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(char*)&reuse,sizeof(reuse))<0){
			return false;
		}
	}


	// bind address
	if(::bind(s,Addr,AddrLen)==SOCKET_ERROR){
		return false;
	}

	if(MulticastAddress!=nullptr){
		if(MulticastAddress->AddMember(s)==0)
			return false;
	}
	return true;
}
//---------------------------------------------------------------------------
iPtr<iConnection> cNT6ThreadPoolSocketDatagramProtocol::MakePair(NT6ThreadPoolSocketMakeConnectionFuction *MakeConnectionFunction,iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true)
{
	cWin32SocketAddressBuffer RemoteAddressBuffer;

	socklen_t RemoteAddrLen;
	auto RemoteAddr=RemoteAddressBuffer.SetupSendAddress(RemoteAddress,RemoteAddrLen);
	if(RemoteAddr==nullptr)
		return nullptr;

	SOCKET NewSocket=::WSASocketW(RemoteAddr->sa_family,fType,fProtocol,NULL,0,WSA_FLAG_OVERLAPPED);
	if(NewSocket==INVALID_SOCKET){
		// create socket failed
		return nullptr;
	}

	iPtr<iSocketAddress> LocalSocketAddress;
	if(LocalAddress!=nullptr){
		cWin32SocketAddressBuffer LocalAddressBuffer;
		// bind address
		int addrlen;
		auto addr=LocalAddressBuffer.SetupSendAddress(LocalAddress,addrlen);
		if(addr==nullptr || ::bind(NewSocket,addr,addrlen)==SOCKET_ERROR){
			::closesocket(NewSocket);
			return nullptr;
		}
		LocalSocketAddress=Win32CreateCopySocketAddress(addr,addrlen);
	}

	if(::connect(NewSocket,RemoteAddr,RemoteAddrLen)==SOCKET_ERROR){
		::closesocket(NewSocket);
		return nullptr;
	}

	//BOOL Val=1;
	//::setsockopt(NewSocket,SOL_SOCKET,SO_DONTLINGER,(const char*)&Val,1);
	
	auto SocketIO=rCreate<cNT6ThreadPoolClientSocketIOHandle>();
	if(SocketIO->Open(NewSocket)==false){
		::closesocket(NewSocket);
		return nullptr;
	}

	return (*MakeConnectionFunction)(cnVar::MoveCast(SocketIO),cnVar::MoveCast(LocalSocketAddress),Win32CreateCopySocketAddress(RemoteAddr,RemoteAddrLen));
}
//---------------------------------------------------------------------------
iPtr<iConnection>	cNT6ThreadPoolSocketDatagramProtocol::CreateStreamPair(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true)
{
	return MakePair(NT6ThreadPoolSocketMakeStreamConnection,LocalAddress,RemoteAddress);
}
//---------------------------------------------------------------------------
iPtr<iConnection>	cNT6ThreadPoolSocketDatagramProtocol::CreateEndpointPair(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true)
{
	return MakePair(NT6ThreadPoolSocketMakeEndpointConnection,LocalAddress,RemoteAddress);
}
//---------------------------------------------------------------------------
iPtr<iMultipointStream> cNT6ThreadPoolSocketDatagramProtocol::MakeDatagramStream(iAddress *LocalAddress)noexcept(true)
{
	cWin32SocketAddressBuffer AddressBuffer;
	int addrlen;
	auto addr=AddressBuffer.SetupSendAddress(LocalAddress,addrlen);
	if(addr==nullptr){
		return nullptr;
	}

	SOCKET NewSocket=::WSASocketW(addr->sa_family,fType,fProtocol,NULL,0,WSA_FLAG_OVERLAPPED);
	if(NewSocket==INVALID_SOCKET){
		//create socket failed
		return nullptr;
	}

	// bind address
	if(::bind(NewSocket,addr,addrlen)==false){
		::closesocket(NewSocket);
		return nullptr;
	}
	auto SocketIO=rCreate<cNT6ThreadPoolSocketIOHandle>();
	if(SocketIO->Open(NewSocket)==false){
		::closesocket(NewSocket);
		return nullptr;
	}
	return iCreate<cNTSocketOverlappedIOHandleDatagramStream>(cnVar::MoveCast(SocketIO),Win32CreateCopySocketAddress(addr,addrlen));
}
//---------------------------------------------------------------------------
iPtr<iMultipointStream>	cNT6ThreadPoolSocketDatagramProtocol::OpenStream(iAddress *LocalAddress)noexcept(true)
{
	return MakeDatagramStream(LocalAddress);
}
//---------------------------------------------------------------------------
iPtr<iMultipointQueue>	cNT6ThreadPoolSocketDatagramProtocol::OpenQueue(iAddress *LocalAddress)noexcept(true)
{
	auto Stream=MakeDatagramStream(LocalAddress);
	if(Stream==nullptr)
		return nullptr;
	return iCreate<cMultipointQueueFromStream>(Stream);
}
//---------------------------------------------------------------------------
#endif	// >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
