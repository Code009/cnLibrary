/*- cnSystem - Network ----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2010-03-28                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnSystem_cnNetwork_H__
#define	__cnLibrary_cnSystem_cnNetwork_H__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnFundamental.h>
#include <cnSystem/cnData.h>
#include <cnSystem/cnThread.h>
#include <cnSystem/cnStream.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#define	override
#endif
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAddress : public iInterface
{
public:
	virtual eiOrdering cnLib_FUNC Compare(iAddress *Dest)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAddressBinding : public iInterface
{
public:
	virtual iAddress*	cnLib_FUNC GetLocalAddress(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iMultipointQueueCallback
{
public:
	// AccessStarted
	//	called before MultipointQueue using the interface
	virtual void cnLib_FUNC AccessStarted(void)noexcept(true)=0;
	// AccessStopped
	//	called after MultipointQueue done using the interface
	virtual void cnLib_FUNC AccessStopped(void)noexcept(true)=0;

	// ReadNotify
	//	called when MultipointQueue has buffer available for reading , or when user called NotifyRead
	virtual void cnLib_FUNC ReadNotify(void)noexcept(true)=0;
	// WriteNotify
	//	called when MultipointQueue has buffer available for writing, or when user called NotifyWrite
	virtual void cnLib_FUNC WriteNotify(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iMultipointQueue : public iAddressBinding
{
public:
	virtual bool cnLib_FUNC StartAccess(iReference *Reference,iMultipointQueueCallback *Callback)noexcept(true)=0;
	virtual void cnLib_FUNC StopAccess(void)noexcept(true)=0;
	
	virtual bool cnLib_FUNC IsClosed(void)noexcept(true)=0;

	// NotifyRead
	//	notify callback at least once after the call, if there is data available in read buffer
	//	IdleNotify		notify even when pending buffer is empty
	virtual void cnLib_FUNC NotifyRead(bool IdleNotify)noexcept(true)=0;

	// NotifyWrite
	//	notify callback at least once after the call, if there is space available in write buffer
	//	IdleNotify		notify even when write buffer is full
	virtual void cnLib_FUNC NotifyWrite(bool IdleNotify)noexcept(true)=0;

	// QueryReadBuffer
	//	query read buffer
	virtual cConstMemory cnLib_FUNC GatherReadBuffer(iPtr<iAddress> &Address)noexcept(true)=0;
	// CompleteReadBuffer
	//	mark current buffer as completed. Caller should not access the read buffer after the call
	virtual void cnLib_FUNC DismissReadBuffer(bool Dismiss)noexcept(true)=0;


	// QueryWriteBuffer
	//	query buffer to write to
	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn Length)noexcept(true)=0;
	// CommitWriteBuffer
	//	commit data in write buffer
	// Size		size of data in buffer
	virtual bool cnLib_FUNC CommitWriteBuffer(iAddress *Address,uIntn Length)noexcept(true)=0;
};
//- Datagram port Operation -------------------------------------------------
class cnLib_INTERFACE iMultipointTask : public iStreamTask
{
public:
	virtual iAddress*	cnLib_FUNC GetRemoteAddress(void)noexcept(true)=0;
};
//- Datagram Stream ---------------------------------------------------------
class cnLib_INTERFACE iMultipointStream : public iAddressBinding
{
public:
	virtual void cnLib_FUNC Close(void)noexcept(true)=0;

	// Read
	//	receive a datagram
	virtual bool	cnLib_FUNC Read(iPtr<iAddress> &Address,void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)=0;
	// Write
	//	send a datagram
	virtual bool	cnLib_FUNC Write(iAddress *Address,const void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)=0;

	// ReadAsync
	//	receive a datagram
	// return true if task started
	virtual iPtr<iMultipointTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size)noexcept(true)=0;
	// WriteAsync
	//	send a datagram
	// return true if task started
	virtual iPtr<iMultipointTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size,iAddress *Address)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iConnection : public iAddressBinding
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual iAddress*	cnLib_FUNC GetRemoteAddress(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,ConnectError)
{
	Success,
	Error,
	Cancelled,
}cnLib_ENUM_END(ConnectError);
//---------------------------------------------------------------------------
class iConnectErrorReport : public iInterface
{
public:
	virtual eConnectError cnLib_FUNC GetConnectError(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iConnectionTask : public iAsyncTask
{
public:
	// Cancel
	//	cancel connecting
	virtual void cnLib_FUNC Cancel(void)noexcept(true)=0;

	virtual iConnection* cnLib_FUNC GetConnection(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iConnectionConnector : public iAddressBinding
{
public:
	//  Connect
	//	connect to RemoteAddress
	// [in]RemoteAddress	specify RemoteAddress to connect.
	virtual iPtr<iConnection>			cnLib_FUNC Connect(iAddress *RemoteAddress)noexcept(true)=0;
	//  ConnectAsync
	//	connect to RemoteAddress
	// [in]RemoteAddress	specify RemoteAddress to connect.
	virtual iPtr<iConnectionTask>		cnLib_FUNC ConnectAsync(iAddress *RemoteAddress)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iConnectionListener : public iAddressBinding, public iAsyncNotification
{
public:
	using iAddressBinding::CastInterface;
	virtual void	cnLib_FUNC Close(void)noexcept(true)=0;

	virtual iPtr<iConnection> cnLib_FUNC FetchConnection(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iConnectionProtocol : public iInterface
{
public:
	virtual iPtr<iConnectionConnector>	cnLib_FUNC CreateStreamConnector(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true)=0;
	virtual iPtr<iConnectionListener>	cnLib_FUNC CreateStreamListener(iAddress *LocalAddress)noexcept(true)=0;

	virtual iPtr<iConnectionConnector>	cnLib_FUNC CreateEndpointConnector(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true)=0;
	virtual iPtr<iConnectionListener>	cnLib_FUNC CreateEndpointListener(iAddress *LocalAddress)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDatagramProtocol : public iInterface
{
public:
	// Pair
	//	link up address
	// [in]ConnectionIID	iid of connection interface
	// [in]RemoteAddress	specify remote address.
	// [in]LocalAddress		specify local address. may be null if the protocol can automaticly select local address
	virtual iPtr<iConnection>		cnLib_FUNC CreateStreamPair(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true)=0;
	virtual iPtr<iConnection>		cnLib_FUNC CreateEndpointPair(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept(true)=0;
	// OpenStream
	//	open datagram stream at LocalAddress
	virtual iPtr<iMultipointStream>	cnLib_FUNC OpenStream(iAddress *LocalAddress)noexcept(true)=0;
	// OpenQueue
	//	create datagram queue at LocalAddress
	// [in]LocalAddress		local port address, cannot be null
	virtual iPtr<iMultipointQueue>	cnLib_FUNC OpenQueue(iAddress *LocalAddress)noexcept(true)=0;
};
//---------------------------------------------------------------------------
}	// namespace cnLibrary
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#undef	override
#endif
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
