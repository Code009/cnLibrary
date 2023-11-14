/*- cnSystem - socket -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-08-20                                          */
/*-------------------------------------------------------------------------*/
#ifdef __cnLibrary_cnSystem_Socket_H__
#ifndef __cnLibrary_cnSystem_osSocket_H__
#define	__cnLibrary_cnSystem_osSocket_H__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnFundamental.h>
#include <cnSystem/cnNetwork.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iSocketAddress : public iAddress
{
public:
	cnLib_INTERFACE_DEFINE(iSocketAddress,iInterface)

	virtual const sockaddr* cnLib_FUNC SockAddr(void)=0;
	virtual socklen_t cnLib_FUNC SockAddrLen(void)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
iPtr<iSocketAddress> cnLib_FUNC CreateSocketAddress(const sockaddr *addr,socklen_t addrlen);
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
#endif /* __cnLibrary_cnSystem_Socket_H__ */
