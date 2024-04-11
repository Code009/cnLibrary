#define INITGUID

#include "WinSocketAddress.h"
#include <Mswsock.h>
#include <ws2bth.h>
#include <WS2tcpip.h>
// For guids
#include <svcguid.h>

#include <cnRTL\InterfaceImplementation.h>

#pragma comment(lib,"ws2_32.lib")

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

//---------------------------------------------------------------------------
eiOrdering cnWinRTL::Win32SocketAddressCompare(const SOCKADDR *addr1,const SOCKADDR *addr2)noexcept
{
	if(addr1->sa_family!=addr2->sa_family)
		return iOrdering::Different;
	switch(addr1->sa_family){
	default:
		break;
	case AF_INET:
		{
			auto *V4Addr1=reinterpret_cast<const SOCKADDR_IN*>(addr1);
			auto *V4Addr2=reinterpret_cast<const SOCKADDR_IN*>(addr2);
			uInt32 ip1=cnMemory::SwapBigEndian(V4Addr1->sin_addr.S_un.S_addr);
			uInt32 ip2=cnMemory::SwapBigEndian(V4Addr2->sin_addr.S_un.S_addr);
			if(ip1<ip2)
				return iOrdering::Less;
			if(ip1>ip2)
				return iOrdering::Greater;
			uInt16 port1=cnMemory::SwapBigEndian(V4Addr1->sin_port);
			uInt16 port2=cnMemory::SwapBigEndian(V4Addr2->sin_port);
			if(port1<port2)
				return iOrdering::Less;
			if(port1>port2)
				return iOrdering::Greater;
			return iOrdering::Equal;
		}
	case AF_INET6:
		{
			auto *V6Addr1=reinterpret_cast<const SOCKADDR_IN6*>(addr1);
			auto *V6Addr2=reinterpret_cast<const SOCKADDR_IN6*>(addr2);
			auto *ip1=reinterpret_cast<const uIntn*>(&V6Addr1->sin6_addr);
			auto *ip2=reinterpret_cast<const uIntn*>(&V6Addr2->sin6_addr);
			for(uIntn i=sizeof(V6Addr1->sin6_addr)/sizeof(uIntn);i>0;i--){
				uIntn v1=cnMemory::SwapBigEndian(ip1[i-1]);
				uIntn v2=cnMemory::SwapBigEndian(ip2[i-1]);
				if(v1<v2)
					return iOrdering::Less;
				if(v1>v2)
					return iOrdering::Greater;
			}
			uInt16 port1=cnMemory::SwapBigEndian(V6Addr1->sin6_port);
			uInt16 port2=cnMemory::SwapBigEndian(V6Addr2->sin6_port);
			if(port1<port2)
				return iOrdering::Less;
			if(port1>port2)
				return iOrdering::Greater;
			return iOrdering::Equal;
		}
	}
	return iOrdering::Different;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWin32SocketAddress_IPv4::cWin32SocketAddress_IPv4(uInt32 IP,uInt16 Port)noexcept
{
	fAddr.sin_family=AF_INET;
	fAddr.sin_addr.s_addr=htonl(IP);
	fAddr.sin_port=htons(Port);
	cnMemory::ZeroFill(fAddr.sin_zero);
}
//---------------------------------------------------------------------------
void * cWin32SocketAddress_IPv4::CastInterface(iTypeID InterfaceID)noexcept
{
	return ImpCastInterface<iSocketAddress,iIPv4Address>(this,InterfaceID);
}
//---------------------------------------------------------------------------
eiOrdering cWin32SocketAddress_IPv4::Compare(iAddress *Dest)noexcept
{
	return cWin32SocketAddress::Compare(Dest);
}
//---------------------------------------------------------------------------
uInt32 cWin32SocketAddress_IPv4::IPv4Address(void)noexcept
{
	return ::ntohl(fAddr.sin_addr.s_addr);
}
uInt16 cWin32SocketAddress_IPv4::IPv4Port(void)noexcept
{
	return ::ntohs(fAddr.sin_port);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iSocketAddress> cnWinRTL::Win32CreateCopySocketAddress(const sockaddr *addr,socklen_t addrlen)noexcept
{
	switch(addr->sa_family){
	default:
		return nullptr;
	case AF_INET:
		if(addrlen>sizeof(SOCKADDR_IN))
			return nullptr;
		return iCreate<cWin32SocketAddress_IPv4>(reinterpret_cast<const SOCKADDR_IN*>(addr));
	case AF_INET6:
		if(addrlen>sizeof(SOCKADDR_IN6))
			return nullptr;
		return iCreate<cWin32SocketAddress<SOCKADDR_IN6> >(reinterpret_cast<const SOCKADDR_IN6*>(addr));
	case AF_BTH:
		if(addrlen>sizeof(SOCKADDR_BTH))
			return nullptr;
		return iCreate<cWin32SocketAddress<SOCKADDR_BTH> >(reinterpret_cast<const SOCKADDR_BTH*>(addr));
	}
}
//---------------------------------------------------------------------------
iPtr<iSocketAddress> cnWinRTL::Win32CreateCopySocketAddress(iAddress *Address)noexcept
{
	auto SocketAddress=iCast<iSocketAddress>(Address);
	if(SocketAddress!=nullptr){
		return Win32CreateCopySocketAddress(SocketAddress->SockAddr(),SocketAddress->SockAddrLen());
	}

	auto IPv4Address=iCast<iIPv4Address>(Address);
	if(IPv4Address!=nullptr){
		return iCreate<cWin32SocketAddress_IPv4>(IPv4Address->IPv4Address(),IPv4Address->IPv4Port());
	}

	return nullptr;
}
//---------------------------------------------------------------------------
uIntn cnWinRTL::Win32CopySocketAddress(void *Buffer,uIntn BufferSize,iAddress *Address)noexcept
{
	auto SocketAddress=iCast<iSocketAddress>(Address);
	if(SocketAddress!=nullptr){
		uIntn AddressSize=SocketAddress->SockAddrLen();
		if(BufferSize<AddressSize)
			return 0;
		cnMemory::Copy(Buffer,SocketAddress->SockAddr(),AddressSize);
		return AddressSize;
	}

	auto IPv4Address=iCast<iIPv4Address>(Address);
	if(IPv4Address!=nullptr){
		if(BufferSize<sizeof(SOCKADDR_IN))
			return 0;
			
		auto *Addr=static_cast<SOCKADDR_IN*>(Buffer);
		Addr->sin_family=AF_INET;
		Addr->sin_addr.s_addr=htonl(IPv4Address->IPv4Address());
		Addr->sin_port=htons(IPv4Address->IPv4Port());
		cnMemory::ZeroFill(Addr->sin_zero);
		return sizeof(SOCKADDR_IN);
	}

	return 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iIPv4Address>	cnWinRTL::Win32CreateAddressIPv4(uInt32 IP,uInt16 Port)noexcept
{
	return iCreate<cWin32SocketAddress_IPv4>(IP,Port);
}
//---------------------------------------------------------------------------
uInt16 cnWinRTL::Win32QueryAddressFamily(iAddress *Address)noexcept
{
	auto SocketAddress=iCast<iSocketAddress>(Address);
	if(SocketAddress!=nullptr){
		return SocketAddress->SockAddr()->sa_family;
	}
	auto IPv4Address=iCast<iIPv4Address>(Address);
	if(IPv4Address!=nullptr){
		return AF_INET;
	}
	return AF_UNSPEC;
}
//---------------------------------------------------------------------------
ufInt16 cnWinRTL::Win32QueryAddressSize(iAddress *Address)noexcept
{
	auto SocketAddress=iCast<iSocketAddress>(Address);
	if(SocketAddress!=nullptr){
		return static_cast<ufInt16>(SocketAddress->SockAddrLen());
	}
	auto IPv4Address=iCast<iIPv4Address>(Address);
	if(IPv4Address!=nullptr){
		return sizeof(SOCKADDR_IN);
	}
	return 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWin32SocketAddressBuffer::cWin32SocketAddressBuffer()noexcept
{
	fBuffer.SetSize(sizeof(SOCKADDR));
}
//---------------------------------------------------------------------------
cWin32SocketAddressBuffer::~cWin32SocketAddressBuffer()noexcept
{
}
//---------------------------------------------------------------------------
void cWin32SocketAddressBuffer::Clear(void)noexcept
{
	fBuffer.Clear();
}
//---------------------------------------------------------------------------
SOCKADDR* cWin32SocketAddressBuffer::SockAddr(void)const noexcept
{
	return static_cast<SOCKADDR*>(fBuffer->Pointer);
}
//---------------------------------------------------------------------------
socklen_t cWin32SocketAddressBuffer::SockAddrLen(void)const noexcept
{
	return static_cast<socklen_t>(fBuffer->Length);
}
//---------------------------------------------------------------------------
void cWin32SocketAddressBuffer::SetSockAddrLen(socklen_t len)noexcept
{
	if(len<sizeof(SOCKADDR))
		len=sizeof(SOCKADDR);
	return fBuffer.SetSize(len);
}
//---------------------------------------------------------------------------
socklen_t cWin32SocketAddressBuffer::GetCapacity(void)const noexcept
{
	return static_cast<socklen_t>(fBuffer.GetCapacity());
}
//---------------------------------------------------------------------------
void cWin32SocketAddressBuffer::SetCapacity(socklen_t len)noexcept
{
	if(len<sizeof(SOCKADDR))
		len=sizeof(SOCKADDR);
	return fBuffer.SetCapacity(len);
}
//---------------------------------------------------------------------------
void cWin32SocketAddressBuffer::SetupBuffer(ADDRESS_FAMILY sa_family,socklen_t socklen)noexcept
{
	auto *saddr=SockAddr();
	socklen_t SockMinLength;
	switch(sa_family){
	default:
		SockMinLength=128;
		break;
	case AF_INET:
		SockMinLength=sizeof(SOCKADDR_IN);
		break;
	case AF_INET6:
		SockMinLength=sizeof(SOCKADDR_IN6);
		break;
	case AF_BTH:
		SockMinLength=sizeof(SOCKADDR_BTH);
		break;
	}
	if(socklen<SockMinLength){
		socklen=SockMinLength;
	}
	fBuffer.SetSize(socklen);
	cnMemory::ZeroFill(saddr,socklen);
}
//---------------------------------------------------------------------------
bool cWin32SocketAddressBuffer::SetAddressAny(ADDRESS_FAMILY sa_family)noexcept
{
	auto *saddr=SockAddr();
	switch(sa_family){
	default:
		return false;
	case AF_INET:
		fBuffer.SetSize(sizeof(SOCKADDR_IN));
		break;
	case AF_INET6:
		fBuffer.SetSize(sizeof(SOCKADDR_IN6));
		break;
	case AF_BTH:
		fBuffer.SetSize(sizeof(SOCKADDR_BTH));
		break;
	}
	cnMemory::ZeroFill(saddr,fBuffer.GetSize());
	saddr->sa_family=sa_family;
	return true;
}
//---------------------------------------------------------------------------
iPtr<iSocketAddress> cWin32SocketAddressBuffer::CreateSocketAddress(void)noexcept
{
	if(fBuffer.GetSize()==0)
		return nullptr;
	return Win32CreateCopySocketAddress(SockAddr(),SockAddrLen());
}
//---------------------------------------------------------------------------
bool cWin32SocketAddressBuffer::SetupForAddressFamily(ADDRESS_FAMILY sa_family)noexcept
{
	auto *saddr=SockAddr();
	switch(sa_family){
	default:
		return false;
	case AF_INET:
		fBuffer.SetSize(sizeof(SOCKADDR_IN));
		break;
	case AF_INET6:
		fBuffer.SetSize(sizeof(SOCKADDR_IN6));
		break;
	case AF_BTH:
		fBuffer.SetSize(sizeof(SOCKADDR_BTH));
		break;
	}
	cnMemory::ZeroFill(saddr,fBuffer.GetSize());
	return true;
}
//---------------------------------------------------------------------------
bool cWin32SocketAddressBuffer::SetupRecvAddress(const SOCKADDR *addr)noexcept
{
	return SetupForAddressFamily(addr->sa_family);
}
//---------------------------------------------------------------------------
bool cWin32SocketAddressBuffer::SetupRecvAddress(iSocketAddress *Address)noexcept
{
	return SetupForAddressFamily(Address->SockAddr()->sa_family);
}
//---------------------------------------------------------------------------
bool cWin32SocketAddressBuffer::SetupRecvAddress(iAddress *Address)noexcept
{
	auto SocketAddress=iCast<iSocketAddress>(Address);
	if(SocketAddress!=nullptr){
		return SetupForAddressFamily(SocketAddress->SockAddr()->sa_family);
	}

	auto IPv4Address=iCast<iIPv4Address>(Address);
	if(IPv4Address!=nullptr){
		fBuffer.SetSize(sizeof(SOCKADDR_IN));
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------
const sockaddr* cWin32SocketAddressBuffer::SetupSendAddress(iAddress *Address,socklen_t &addrlen)noexcept
{
	auto SocketAddress=iCast<iSocketAddress>(Address);
	if(SocketAddress!=nullptr){
		addrlen=SocketAddress->SockAddrLen();
		return SocketAddress->SockAddr();
	}

	auto IPv4Address=iCast<iIPv4Address>(Address);
	if(IPv4Address!=nullptr){
		fBuffer.SetSize(sizeof(SOCKADDR_IN));
		auto addr=static_cast<SOCKADDR_IN*>(fBuffer.GetPointer());
		addr->sin_family=AF_INET;
		addr->sin_addr.s_addr=cnMemory::SwapBigEndian(IPv4Address->IPv4Address());
		addr->sin_port=cnMemory::SwapBigEndian(IPv4Address->IPv4Port());
		cnMemory::ZeroFill(addr->sin_zero);
		addrlen=sizeof(SOCKADDR_IN);
		return reinterpret_cast<const sockaddr*>(addr);
	}

	return nullptr;
}
//---------------------------------------------------------------------------
bool cWin32SocketAddressBuffer::GetBoundAddress(SOCKET s,ADDRESS_FAMILY sa_family)noexcept
{
	SetupBuffer(sa_family,0);
	auto BoundAddrLen=static_cast<socklen_t>(fBuffer.GetCapacity());
	auto BoundSockAddr=static_cast<sockaddr*>(fBuffer.GetPointer());
	if(::getpeername(s,BoundSockAddr,&BoundAddrLen)!=0){
		DWORD ErrorNo=::WSAGetLastError();
		return false;
	}
	fBuffer.SetSize(BoundAddrLen);
	return true;
}
//---------------------------------------------------------------------------
//bool cWin32SocketAddressBuffer::BindGetBoundAddress(SOCKET s,iPtr<iSocketAddress> &Address)
//{
//	auto BindSockAddr=Address->SockAddr();
//	if(::bind(s,BindSockAddr,Address->SockAddrLen())!=0){
//		return false;
//	}
//
//	if(GetBoundAddress(s,BindSockAddr->sa_family)==false)
//		return false;
//
//	uIntn BoundAddrLen=fBuffer.GetSize();
//	auto BoundSockAddr=static_cast<sockaddr*>(fBuffer.GetPointer());
//	if(Win32SocketAddressCompare(BindSockAddr,BoundSockAddr)==iOrdering::Equal){
//		return true;
//	}
//	Address=Win32CreateCopySocketAddress(BoundSockAddr,static_cast<socklen_t>(BoundAddrLen));	
//	return true;
//}
//---------------------------------------------------------------------------
eiOrdering cWin32SocketAddressBuffer::Compare(const SOCKADDR *addr)const noexcept
{
	return Win32SocketAddressCompare(static_cast<const sockaddr*>(fBuffer.GetPointer()),addr);
}
//---------------------------------------------------------------------------
bool cWin32SocketAddressBuffer::operator == (const SOCKADDR *addr)const noexcept
{
	return Compare(addr)==iOrdering::Equal;
}
//---------------------------------------------------------------------------
bool cWin32SocketAddressBuffer::operator != (const SOCKADDR *addr)const noexcept
{
	return Compare(addr)!=iOrdering::Equal;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWin32SocketMulticastAddress_IPv4::cWin32SocketMulticastAddress_IPv4(const SOCKADDR_IN *addr,const struct ip_mreq *MulticastAddrs,uIntn MulticastAddrCount)noexcept
{
	fAddr=*addr;
	cnMemory::ZeroFill(fAddr.sin_zero);
	fMulticastAddressList.Append(MulticastAddrs,MulticastAddrCount);
}
//---------------------------------------------------------------------------
void * cWin32SocketMulticastAddress_IPv4::CastInterface(iTypeID InterfaceID)noexcept
{
	return ImpCastInterface<iWinSocketMulticastAddress,iIPv4Address>(this,InterfaceID);
}
//---------------------------------------------------------------------------
eiOrdering cWin32SocketMulticastAddress_IPv4::Compare(iAddress *Dest)noexcept
{
	if(Dest==nullptr)
		return iOrdering::Different;
	auto *DestSocketAddress=iCast<iWinSocketMulticastAddress>(Dest);
	if(DestSocketAddress!=this){
		return iOrdering::Different;
	}

	return Win32SocketAddressCompare(&fSAddr,DestSocketAddress->SockAddr());
}
//---------------------------------------------------------------------------
const sockaddr* cWin32SocketMulticastAddress_IPv4::SockAddr(void)noexcept
{
	return &fSAddr;
}
//---------------------------------------------------------------------------
socklen_t cWin32SocketMulticastAddress_IPv4::SockAddrLen(void)noexcept
{
	return sizeof(fAddr);
}
//---------------------------------------------------------------------------
uIntn cWin32SocketMulticastAddress_IPv4::AddMember(SOCKET s)noexcept
{
	uIntn SuccessCount=0;
	for(uIntn i=0,c=fMulticastAddressList.GetCount();i<c;i++){
		auto &imr=fMulticastAddressList[i];
		if(setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &imr, sizeof(imr))!=SOCKET_ERROR){
			SuccessCount++;
		}
	}
	return SuccessCount;
}
//---------------------------------------------------------------------------
uIntn cWin32SocketMulticastAddress_IPv4::DropMember(SOCKET s)noexcept
{
	uIntn SuccessCount=0;
	for(uIntn i=0,c=fMulticastAddressList.GetCount();i<c;i++){
		auto &imr=fMulticastAddressList[i];
		if(setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *) &imr, sizeof(imr))!=SOCKET_ERROR){
			SuccessCount++;
		}
	}
	return SuccessCount;
}
//---------------------------------------------------------------------------
uInt32 cWin32SocketMulticastAddress_IPv4::IPv4Address(void)noexcept
{
	return ntohl(fAddr.sin_addr.s_addr);
}
uInt16 cWin32SocketMulticastAddress_IPv4::IPv4Port(void)noexcept
{
	return ntohs(fAddr.sin_port);
}
//---------------------------------------------------------------------------
