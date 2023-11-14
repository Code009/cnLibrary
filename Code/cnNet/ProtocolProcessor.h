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
	cEndpointFromProtocol(void);

	bool Connect(iProtocolProvider *Host);
	
private:
	rPtr<iProtocolProvider> fHost;

	virtual void ProtocolStarted(void)override;
	virtual void ProtocolStopped(void)override;
	virtual uIntn ProtocolInputPush(const void *Buffer,uIntn BufferSize,uIntn &LeastSizeNeeded)override;
	virtual uIntn ProtocolOutputPull(void *Buffer,uIntn BufferSize,uIntn &LeastSizeNeeded)override;
	virtual void ProtocolInputClosed(bool GracefulClose)=0;
	virtual void ProtocolOutputClosed(bool GracefulClose)=0;

	iReadQueueCallback *fReadCallback=nullptr;
	iWriteQueueCallback *fWriteCallback=nullptr;

	virtual bool cnLib_FUNC StartWrite(iReference *Reference,iWriteQueueCallback *Callback)override;
	virtual void cnLib_FUNC StopWrite(bool Terminate)override;
	virtual void cnLib_FUNC NotifyWrite(uIntn SizeToNotify)override;
	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn QuerySize)override;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)override;
	virtual bool cnLib_FUNC IsWriteClosed(bool &GracefulClose)override;

	virtual bool cnLib_FUNC StartRead(iReference *Reference,iReadQueueCallback *Callback)override;
	virtual void cnLib_FUNC StopRead(void)override;
	virtual void cnLib_FUNC NotifyRead(uIntn SizeToNotify)override;
	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn QuerySize)override;
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)override;
	virtual bool cnLib_FUNC IsReadClosed(bool &GracefulClose)override;

	virtual void cnLib_FUNC Close(void)override;
	virtual iReadQueue *cnLib_FUNC GetReadQueue(void)override;
	virtual iWriteQueue *cnLib_FUNC GetWriteQueue(void)override;
	virtual void cnLib_FUNC SetWriteEndMode(eEndpointWriteEndMode EndMode)override;

};
//---------------------------------------------------------------------------
}	// namespace cnNet
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
