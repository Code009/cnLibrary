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
eiOrdering Win32SocketAddressCompare(const SOCKADDR *addr1,const SOCKADDR *addr2);
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
	cWin32SocketAddress(const TSOCKADDR *addr){
		cnMemory::Copy(&fAddr,addr,sizeof(TSOCKADDR));
	}

// iAddress

	virtual eiOrdering cnLib_FUNC Compare(iAddress *Dest)override{
		if(Dest==nullptr)
			return iOrdering::Different;
		iSocketAddress *DestSocketAddress;
		if(iCast(DestSocketAddress,Dest)==false)
			return iOrdering::Different;
		
		return Win32SocketAddressCompare(&fSAddr,DestSocketAddress->SockAddr());
	}

// iSocketAddress

	virtual const SOCKADDR*	cnLib_FUNC SockAddr(void)override{
		return &fSAddr;
	}
	virtual socklen_t cnLib_FUNC SockAddrLen(void)override{
		return sizeof(TSOCKADDR);
	}
};
//---------------------------------------------------------------------------
iPtr<iSocketAddress> Win32CreateCopySocketAddress(const sockaddr *addr,socklen_t addrlen);
iPtr<iSocketAddress> Win32CreateCopySocketAddress(iAddress *Address);
uIntn Win32CopySocketAddress(void *Buffer,uIntn BufferSize,iAddress *Address);
//---------------------------------------------------------------------------
class cWin32SocketAddress_IPv4 : public cWin32SocketAddress<SOCKADDR_IN> , public iIPv4Address
{
protected:
public:
	using cWin32SocketAddress::cWin32SocketAddress;
	cWin32SocketAddress_IPv4(uInt32 IP,uInt16 Port);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

// iAddress

	virtual eiOrdering cnLib_FUNC Compare(iAddress *Dest)override;

// iIPv4Address

	virtual uInt32 cnLib_FUNC IPv4Address(void)override;
	virtual uInt16 cnLib_FUNC IPv4Port(void)override;
};
//---------------------------------------------------------------------------
iPtr<iIPv4Address> Win32CreateAddressIPv4(uInt32 IP,uInt16 Port);
//---------------------------------------------------------------------------
uInt16 Win32QueryAddressFamily(iAddress *Address);
ufInt16 Win32QueryAddressSize(iAddress *Address);
//---------------------------------------------------------------------------
class cWin32SocketAddressBuffer
{
public:
	cWin32SocketAddressBuffer();
	~cWin32SocketAddressBuffer();

	void Clear(void);

	SOCKADDR* SockAddr(void)const;
	socklen_t SockAddrLen(void)const;
	void SetSockAddrLen(socklen_t len);

	socklen_t GetCapacity(void)const;
	void SetCapacity(socklen_t len);

	bool SetAddressAny(ADDRESS_FAMILY sa_family);
	void SetupBuffer(ADDRESS_FAMILY sa_family,socklen_t socklen);

	bool SetupForAddressFamily(ADDRESS_FAMILY sa_family);
	bool SetupRecvAddress(const SOCKADDR *addr);
	bool SetupRecvAddress(iSocketAddress *Address);
	bool SetupRecvAddress(iAddress *Address);
	const sockaddr* SetupSendAddress(iAddress *Address,socklen_t &addrlen);

	iPtr<iSocketAddress> CreateSocketAddress(void);

	//bool BindGetBoundAddress(SOCKET s,iPtr<iSocketAddress> &Address);
	bool GetBoundAddress(SOCKET s,ADDRESS_FAMILY sa_family=AF_UNSPEC);

	eiOrdering Compare(const SOCKADDR *addr)const;

	bool operator == (const SOCKADDR *addr)const;
	bool operator != (const SOCKADDR *addr)const;

	cnLib_DEFINE_CLASS_THREE_WAY_COMPARISON(const SOCKADDR *,Compare)

protected:
	cnRTL::cMemoryBuffer fBuffer;
};
//---------------------------------------------------------------------------
class cWin32SocketMulticastAddress_IPv4 : public iWinSocketMulticastAddress , public iIPv4Address
{
public:
	cWin32SocketMulticastAddress_IPv4(const SOCKADDR_IN *addr,const struct ip_mreq *MulticastAddrs,uIntn MulticastAddrCount);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

// iAddress

	virtual eiOrdering cnLib_FUNC Compare(iAddress *Dest)override;

// iSocketAddress

	virtual const SOCKADDR* cnLib_FUNC SockAddr(void)override;
	virtual socklen_t cnLib_FUNC SockAddrLen(void)override;

// iSocketMulticastAddress

	virtual uIntn cnLib_FUNC AddMember(SOCKET Socket)override;
	virtual uIntn cnLib_FUNC DropMember(SOCKET Socket)override;
	
// iIPv4Address

	virtual uInt32 cnLib_FUNC IPv4Address(void)override;
	virtual uInt16 cnLib_FUNC IPv4Port(void)override;


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
