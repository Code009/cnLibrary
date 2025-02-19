/*- cnRTL - Windows - WinSock Address -------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-06-13                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinSDKHeader.h>

#include <cnRTL\WinCommon.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
eiOrdering Win32SocketAddressCompare(const SOCKADDR *addr1,const SOCKADDR *addr2)noexcept(true);
//---------------------------------------------------------------------------
template<class TSOCKADDR>
class cWin32SocketAddress : public iSocketAddress
{
protected:
	union{
		SOCKADDR fSAddr;
		TSOCKADDR fAddr;
	};
	cWin32SocketAddress()=default;
public:
	~cWin32SocketAddress()=default;
	cWin32SocketAddress(const TSOCKADDR *addr)noexcept(true){
		cnMemory::Copy(&fAddr,addr,sizeof(TSOCKADDR));
	}

// iAddress

	virtual eiOrdering cnLib_FUNC Compare(iAddress *Dest)noexcept(true)override{
		if(Dest==nullptr)
			return iOrdering::Different;
		iSocketAddress *DestSocketAddress;
		if(iCast(DestSocketAddress,Dest)==false)
			return iOrdering::Different;
		
		return Win32SocketAddressCompare(&fSAddr,DestSocketAddress->SockAddr());
	}

// iSocketAddress

	virtual const SOCKADDR*	cnLib_FUNC SockAddr(void)noexcept(true)override{
		return &fSAddr;
	}
	virtual socklen_t cnLib_FUNC SockAddrLen(void)noexcept(true)override{
		return sizeof(TSOCKADDR);
	}
};
//---------------------------------------------------------------------------
iPtr<iSocketAddress> Win32CreateCopySocketAddress(const sockaddr *addr,socklen_t addrlen)noexcept(true);
iPtr<iSocketAddress> Win32CreateCopySocketAddress(iAddress *Address)noexcept(true);
uIntn Win32CopySocketAddress(void *Buffer,uIntn BufferSize,iAddress *Address)noexcept(true);
//---------------------------------------------------------------------------
class cWin32SocketAddress_IPv4 : public cWin32SocketAddress<SOCKADDR_IN> , public iIPv4Address
{
protected:
public:
	using cWin32SocketAddress::cWin32SocketAddress;
	cWin32SocketAddress_IPv4(uInt32 IP,uInt16 Port)noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

// iAddress

	virtual eiOrdering cnLib_FUNC Compare(iAddress *Dest)noexcept(true)override;

// iIPv4Address

	virtual uInt32 cnLib_FUNC IPv4Address(void)noexcept(true)override;
	virtual uInt16 cnLib_FUNC IPv4Port(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
iPtr<iIPv4Address> Win32CreateAddressIPv4(uInt32 IP,uInt16 Port)noexcept(true);
//---------------------------------------------------------------------------
uInt16 Win32QueryAddressFamily(iAddress *Address)noexcept(true);
ufInt16 Win32QueryAddressSize(iAddress *Address)noexcept(true);
//---------------------------------------------------------------------------
class cWin32SocketAddressBuffer
{
public:
	cWin32SocketAddressBuffer()noexcept(true);
	~cWin32SocketAddressBuffer()noexcept(true);

	void Clear(void)noexcept(true);

	SOCKADDR* SockAddr(void)const noexcept(true);
	socklen_t SockAddrLen(void)const noexcept(true);
	void SetSockAddrLen(socklen_t len)noexcept(true);

	socklen_t GetCapacity(void)const noexcept(true);
	void SetCapacity(socklen_t len)noexcept(true);

	bool SetAddressAny(ADDRESS_FAMILY sa_family)noexcept(true);
	void SetupBuffer(ADDRESS_FAMILY sa_family,socklen_t socklen)noexcept(true);

	bool SetupForAddressFamily(ADDRESS_FAMILY sa_family)noexcept(true);
	bool SetupRecvAddress(const SOCKADDR *addr)noexcept(true);
	bool SetupRecvAddress(iSocketAddress *Address)noexcept(true);
	bool SetupRecvAddress(iAddress *Address)noexcept(true);
	const sockaddr* SetupSendAddress(iAddress *Address,socklen_t &addrlen)noexcept(true);

	iPtr<iSocketAddress> CreateSocketAddress(void)noexcept(true);

	//bool BindGetBoundAddress(SOCKET s,iPtr<iSocketAddress> &Address);
	bool GetBoundAddress(SOCKET s,ADDRESS_FAMILY sa_family=AF_UNSPEC)noexcept(true);

	eiOrdering Compare(const SOCKADDR *addr)const noexcept(true);

	bool operator == (const SOCKADDR *addr)const noexcept(true);
	bool operator != (const SOCKADDR *addr)const noexcept(true);

	cnLib_DEFINE_CLASS_THREE_WAY_COMPARISON(const SOCKADDR *,Compare)

protected:
	cnRTL::cMemoryBuffer fBuffer;
};
//---------------------------------------------------------------------------
class cWin32SocketMulticastAddress_IPv4 : public iWinSocketMulticastAddress , public iIPv4Address
{
public:
	cWin32SocketMulticastAddress_IPv4(const SOCKADDR_IN *addr,const struct ip_mreq *MulticastAddrs,uIntn MulticastAddrCount)noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

// iAddress

	virtual eiOrdering cnLib_FUNC Compare(iAddress *Dest)noexcept(true)override;

// iSocketAddress

	virtual const SOCKADDR* cnLib_FUNC SockAddr(void)noexcept(true)override;
	virtual socklen_t cnLib_FUNC SockAddrLen(void)noexcept(true)override;

// iSocketMulticastAddress

	virtual uIntn cnLib_FUNC AddMember(SOCKET Socket)noexcept(true)override;
	virtual uIntn cnLib_FUNC DropMember(SOCKET Socket)noexcept(true)override;
	
// iIPv4Address

	virtual uInt32 cnLib_FUNC IPv4Address(void)noexcept(true)override;
	virtual uInt16 cnLib_FUNC IPv4Port(void)noexcept(true)override;


protected:
	union{
		SOCKADDR fSAddr;
		SOCKADDR_IN fAddr;
	};
	cnRTL::cSeqList<struct ip_mreq> fMulticastAddressList;

};
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
