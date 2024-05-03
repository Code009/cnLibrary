/*--- Mac - Socket --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-07-27                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_siOS_POSIX_POSIX_Socket_H__
#define __cnLib_siOS_POSIX_POSIX_Socket_H__


//#include <sys/types.h>

#include <siOS/POSIX/POSIXHeader.h>
#include <cnSystem/Socket.h>

/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace siPOSIX{
//---------------------------------------------------------------------------
static constexpr int SocketInvalid=-1;
//---------------------------------------------------------------------------
void SetupSocket(int s)noexcept(true);
//---------------------------------------------------------------------------
eiOrdering SocketAddressCompare(const sockaddr *addr1,const sockaddr *addr2)noexcept(true);
//---------------------------------------------------------------------------
class cSocketAddressBuffer
{
public:
	cSocketAddressBuffer()noexcept(true);
	~cSocketAddressBuffer()noexcept(true);

	void Clear(void)noexcept(true);

	sockaddr* SockAddr(void)const noexcept(true);
	socklen_t SockAddrLen(void)const noexcept(true);
	void SetSockAddrLen(socklen_t len)noexcept(true);

	socklen_t GetCapacity(void)const noexcept(true);
	void SetCapacity(socklen_t len)noexcept(true);

	bool SetAddressFamily(sa_family_t sa_family)noexcept(true);

protected:
	cnRTL::cMemoryBuffer fBuffer;
};
//---------------------------------------------------------------------------
class bcStreamSocketEndpoint : public iConnection, public iEndpoint, public cnRTL::bcRWQueue
{
public:
	bcStreamSocketEndpoint(int Socket)noexcept(true);
	~bcStreamSocketEndpoint()noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

	virtual void cnLib_FUNC Close(void)noexcept(true)override;
	virtual iReadQueue *cnLib_FUNC GetReadQueue(void)noexcept(true)override;
	virtual iWriteQueue *cnLib_FUNC GetWriteQueue(void)noexcept(true)override;
	virtual void cnLib_FUNC SetWriteEndMode(eEndpointWriteEndMode EndMode)noexcept(true)override;

	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)noexcept(true)override;
	virtual iAddress*	cnLib_FUNC GetRemoteAddress(void)noexcept(true)override;

protected:
	int fSocket;

	void ReadContinue(void)noexcept(true);
	void WriteContinue(void)noexcept(true);

	virtual void OnReadWait(void)noexcept(true)=0;
	virtual void OnWriteWait(void)noexcept(true)=0;


	// iWriteQueue

	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn Size)noexcept(true)override;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)noexcept(true)override;

	// iReadQueue

	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn QuerySize)noexcept(true)override;
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)noexcept(true)override;

// bcRWQueue

	virtual void WriteQueueClosed(void)noexcept(true)override;
	virtual void ReadQueueClosed(void)noexcept(true)override;

	iPtr<iAddress> fLocalAddress;
	iPtr<iAddress> fRemoteAddress;

private:
	static const uIntn ReadBufferMinSize;
	static const uIntn WriteBufferMinSize;

	// socket send

	cnRTL::cAsyncLoopbackStreamBuffer fWriteQueueBuffer;
	bool SendBufferWriteToSocketLoop(void)noexcept(true);
	void SendBufferWriteToSocket(void)noexcept(true);

	// socket recv

	cnRTL::cLoopbackStreamBuffer fReadQueueBuffer;

	cnRTL::cExclusiveFlag fSendBufferToSocketExclusiveFlag;

};
//---------------------------------------------------------------------------
class bcDatagramSocketEndpoint : public iConnection, public iEndpoint, public cnRTL::bcRWQueue
{
public:
	bcDatagramSocketEndpoint(int Socket)noexcept(true);
	~bcDatagramSocketEndpoint()noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true)override;

	virtual void cnLib_FUNC Close(void)noexcept(true)override;
	virtual iReadQueue *cnLib_FUNC GetReadQueue(void)noexcept(true)override;
	virtual iWriteQueue *cnLib_FUNC GetWriteQueue(void)noexcept(true)override;
	virtual void cnLib_FUNC SetWriteEndMode(eEndpointWriteEndMode EndMode)noexcept(true)override;

	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)noexcept(true)override;
	virtual iAddress*	cnLib_FUNC GetRemoteAddress(void)noexcept(true)override;

protected:
	int fSocket;

	void ReadContinue(void)noexcept(true);
	void WriteContinue(void)noexcept(true);

	virtual void OnReadWait(void)noexcept(true)=0;
	virtual void OnWriteWait(void)noexcept(true)=0;

	// iWriteQueue

	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn Size)noexcept(true)override;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)noexcept(true)override;

	// iReadQueue

	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)noexcept(true)override;
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)noexcept(true)override;

	// bcRWQueue
	virtual void WriteQueueClosed(void)noexcept(true)override;
	virtual void ReadQueueClosed(void)noexcept(true)override;

	iPtr<iAddress> fLocalAddress;
	iPtr<iAddress> fRemoteAddress;
private:
	static const uIntn ReadBufferMinSize;
	static const uIntn WriteBufferMinSize;

	// socket send

	cnRTL::cMemoryBlock fWriteBuffer;
	uIntn fWriteDataSize;
	bool SendBufferWriteToSocketLoop(void)noexcept(true);
	void SendBufferWriteToSocket(void)noexcept(true);

	// socket recv

	cnRTL::cMemoryBlock fReadBuffer;
	uIntn fReadDataSize;

	cnRTL::cExclusiveFlag fSendBufferToSocketExclusiveFlag;

};
//---------------------------------------------------------------------------
class bcSocketConnectionListener : public cnRTL::bcConnectionListener
{
public:
	bcSocketConnectionListener(int Socket)noexcept(true);
	~bcSocketConnectionListener()noexcept(true);

	virtual iPtr<iConnection> cnLib_FUNC FetchConnection(void)noexcept(true)override;
protected:
	int fSocket;

	void AcceptContinue(void)noexcept(true);

	virtual iPtr<iConnection> OnAcceptCreateConnection(int Socket,const sockaddr *addr,socklen_t addrlen)noexcept(true)=0;
	virtual void OnAcceptWait(void)noexcept(true)=0;

	virtual void AsyncQueueNotify(void)noexcept(true)override;

private:
	bool fAcceptAvailable;
};
#if	0
//---------------------------------------------------------------------------
class cSocketDatagramPort : public iMultipointQueue
{
protected:
	~cSocketDatagramPort();
public:
	cSocketDatagramPort(int Socket,int SocketType,sa_family_t sa_family);

//	int GetHandle();

	virtual bool cnLib_FUNC StartAccess(iMultipointQueueCallback *Callback)override;
	virtual void cnLib_FUNC StopAccess(void)override;
	virtual bool cnLib_FUNC IsClosed(void)override;
	virtual void cnLib_FUNC NotifyRead(bool IdleNotify)override;
	virtual void cnLib_FUNC NotifyWrite(bool IdleNotify)override;
	virtual bool cnLib_FUNC QueryReadBuffer(iPtr<iAddress> &Address,cConstMemory &Buffer)override;
	virtual void cnLib_FUNC CompleteReadBuffer(void)override;
	virtual bool cnLib_FUNC QueryWriteBuffer(cMemory &Buffer)override;
	virtual void cnLib_FUNC CommitWriteBuffer(iAddress *Address,uintn Size)override;
protected:
	iMultipointQueueCallback *fCallback=nullptr;
	int fSocket;

	void ReadContinue(void);
	void WriteContinue(void);

	virtual void OnReadWait(void)=0;
	virtual void OnReadEnded(void)=0;
	virtual void OnWriteWait(void)=0;
	virtual void OnWriteEnded(void)=0;


private:


// pull - write(send)

	bool PullWrite(void);
	iPtr<iSocketAddress> fWriteAddress;
	void *fWriteBuffer;
	uIntn fWriteBufferSize;
	uIntn fWriteSize;

// read(recv) - push

	bool ReadPush(void);
	iPtr<iSocketAddress> fReadAddress;
	cSocketAddressBuffer fReadAddressBuffer;
	void *fReadBuffer;
	uIntn fReadBufferSize;
	uIntn fReadDataSize;


	bool fManagerStopped;
	bool fPushPaused;
	bool fPushIdleNotify;
	bool fPullIdleNotify;

	uInt8 fCloseState;


	class cPortProcedure : public cnRTL::bcExclusiveProcedure
	{
	protected:
		virtual bool HostProcedure(void)override;
	}fPortProc;
	bool PortProc(void);

};
#endif
//---------------------------------------------------------------------------
}	// namespace siPOSIX
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
#endif
