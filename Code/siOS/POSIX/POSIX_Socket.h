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
void SetupSocket(int s);
//---------------------------------------------------------------------------
eOrdering SocketAddressCompare(const sockaddr *addr1,const sockaddr *addr2);
//---------------------------------------------------------------------------
class cSocketAddressBuffer
{
protected:
	cnRTL::cMemoryBuffer fBuffer;
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

};
//---------------------------------------------------------------------------
class bcStreamSocketEndpoint : public iConnection, public iEndpoint, public cnRTL::bcRWQueue
{
public:
	bcStreamSocketEndpoint(int Socket);
	~bcStreamSocketEndpoint();

	virtual const void* cnLib_FUNC CastInterface(iTypeID IID)const override;

	virtual void cnLib_FUNC Close(void)override;
	virtual iReadQueue *cnLib_FUNC GetReadQueue(void)override;
	virtual iWriteQueue *cnLib_FUNC GetWriteQueue(void)override;
	virtual void cnLib_FUNC SetWriteEndMode(eEndpointWriteEndMode EndMode)override;

	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)override;
	virtual iAddress*	cnLib_FUNC GetRemoteAddress(void)override;

protected:
	int fSocket;

	void ReadContinue(void);
	void WriteContinue(void);

	virtual void OnReadWait(void)=0;
	virtual void OnWriteWait(void)=0;


	// iWriteQueue

	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn Size)override;
	virtual uIntn cnLib_FUNC CommitWriteBuffer(uIntn Size)override;

	// iReadQueue

	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn QuerySize)override;
	virtual uIntn cnLib_FUNC DismissReadBuffer(uIntn Size)override;

// bcRWQueue

	virtual void WriteQueueClosed(void)override;
	virtual void ReadQueueClosed(void)override;

	iPtr<iAddress> fLocalAddress;
	iPtr<iAddress> fRemoteAddress;

private:
	static const uIntn ReadBufferMinSize;
	static const uIntn WriteBufferMinSize;

	// socket send

	cnRTL::cAsyncLoopbackStreamBuffer fWriteQueueBuffer;
	bool SendBufferWriteToSocketLoop(void);
	void SendBufferWriteToSocket(void);

	// socket recv

	cnRTL::cLoopbackStreamBuffer fReadQueueBuffer;

	cnRTL::cExclusiveFlag fSendBufferToSocketExclusiveFlag;

};
//---------------------------------------------------------------------------
class bcDatagramSocketEndpoint : public iConnection, public iEndpoint, public cnRTL::bcRWQueue
{
public:
	bcDatagramSocketEndpoint(int Socket);
	~bcDatagramSocketEndpoint();

	virtual const void* cnLib_FUNC CastInterface(iTypeID IID)const override;

	virtual void cnLib_FUNC Close(void)override;
	virtual iReadQueue *cnLib_FUNC GetReadQueue(void)override;
	virtual iWriteQueue *cnLib_FUNC GetWriteQueue(void)override;
	virtual void cnLib_FUNC SetWriteEndMode(eEndpointWriteEndMode EndMode)override;

	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)override;
	virtual iAddress*	cnLib_FUNC GetRemoteAddress(void)override;

protected:
	int fSocket;

	void ReadContinue(void);
	void WriteContinue(void);

	virtual void OnReadWait(void)=0;
	virtual void OnWriteWait(void)=0;

	// iWriteQueue

	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn Size)override;
	virtual uIntn cnLib_FUNC CommitWriteBuffer(uIntn Size)override;

	// iReadQueue

	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)override;
	virtual uIntn cnLib_FUNC DismissReadBuffer(uIntn Size)override;

	// bcRWQueue
	virtual void WriteQueueClosed(void)override;
	virtual void ReadQueueClosed(void)override;

	iPtr<iAddress> fLocalAddress;
	iPtr<iAddress> fRemoteAddress;
private:
	static const uIntn ReadBufferMinSize;
	static const uIntn WriteBufferMinSize;

	// socket send

	cnRTL::cMemoryBlock fWriteBuffer;
	uIntn fWriteDataSize;
	bool SendBufferWriteToSocketLoop(void);
	void SendBufferWriteToSocket(void);

	// socket recv

	cnRTL::cMemoryBlock fReadBuffer;
	uIntn fReadDataSize;

	cnRTL::cExclusiveFlag fSendBufferToSocketExclusiveFlag;

};
//---------------------------------------------------------------------------
class bcSocketConnectionQueue : public cnRTL::bcConnectionQueue
{
public:
	bcSocketConnectionQueue(int Socket);
	~bcSocketConnectionQueue();

	virtual iPtr<iConnection> cnLib_FUNC FetchConnection(void)override;
protected:
	int fSocket;

	void AcceptContinue(void);

	virtual iPtr<iConnection> OnAcceptCreateConnection(int Socket,const sockaddr *addr,socklen_t addrlen)=0;
	virtual void OnAcceptWait(void)=0;

	virtual Availability AsyncSignalAvailable(void)override;

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
