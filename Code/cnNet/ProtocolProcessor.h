/*- cnNet - Protocol Processor --------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-03-15                                          */
/*-------------------------------------------------------------------------*/
#ifndef	__cnLibrary_cnNet_ProtocolProcessor_H__
#define	__cnLibrary_cnNet_ProtocolProcessor_H__
/*-------------------------------------------------------------------------*/
#include <cnNet/cnProtocol.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnNet{
//---------------------------------------------------------------------------
class cEndpointFromProtocol : public iEndpoint,/*public cnRTL::cInnerReference,*/ public iReadQueue, public iWriteQueue, public iProtocolProcessor
{
public:
	cEndpointFromProtocol(void)noexcept(true);

	bool Connect(iProtocolProvider *Host)noexcept(true);
	
private:
	rPtr<iProtocolProvider> fHost;

	virtual void ProtocolStarted(void)noexcept(true)override;
	virtual void ProtocolStopped(void)noexcept(true)override;
	virtual uIntn ProtocolInputPush(const void *Buffer,uIntn BufferSize,uIntn &LeastSizeNeeded)noexcept(true)override;
	virtual uIntn ProtocolOutputPull(void *Buffer,uIntn BufferSize,uIntn &LeastSizeNeeded)noexcept(true)override;
	virtual void ProtocolInputClosed(bool GracefulClose)noexcept(true)=0;
	virtual void ProtocolOutputClosed(bool GracefulClose)noexcept(true)=0;

	iReadQueueCallback *fReadCallback=nullptr;
	iWriteQueueCallback *fWriteCallback=nullptr;

	virtual bool cnLib_FUNC StartWrite(iReference *Reference,iWriteQueueCallback *Callback)noexcept(true)override;
	virtual void cnLib_FUNC StopWrite(void)noexcept(true)override;
	virtual void cnLib_FUNC NotifyWrite(uIntn SizeToNotify)noexcept(true)override;
	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn QuerySize)noexcept(true)override;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)noexcept(true)override;
	virtual bool cnLib_FUNC IsWriteClosed(bool &GracefulClose)noexcept(true)override;

	virtual bool cnLib_FUNC StartRead(iReference *Reference,iReadQueueCallback *Callback)noexcept(true)override;
	virtual void cnLib_FUNC StopRead(void)noexcept(true)override;
	virtual void cnLib_FUNC NotifyRead(uIntn SizeToNotify)noexcept(true)override;
	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn QuerySize)noexcept(true)override;
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)noexcept(true)override;
	virtual bool cnLib_FUNC IsReadClosed(bool &GracefulClose)noexcept(true)override;

	virtual void cnLib_FUNC Close(void)noexcept(true)override;
	virtual iReadQueue *cnLib_FUNC GetReadQueue(void)noexcept(true)override;
	virtual iWriteQueue *cnLib_FUNC GetWriteQueue(void)noexcept(true)override;

};
//---------------------------------------------------------------------------
}	// namespace cnNet
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
