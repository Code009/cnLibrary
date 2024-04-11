/*- cnSystem - Media ------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2008-09-21                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnSystem_cnMedia_H__
#define	__cnLibrary_cnSystem_cnMedia_H__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnFundamental.h>
#include <cnSystem/cnStream.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAudioOutputEndpoint : public iInterface
{
public:
	virtual void cnLib_FUNC Close(void)noexcept(true)=0;
	virtual iWriteQueue* cnLib_FUNC GetWriteQueue(void)noexcept(true)=0;

	virtual void cnLib_FUNC Start(void)noexcept(true)=0;
	virtual void cnLib_FUNC Stop(void)noexcept(true)=0;
	virtual void cnLib_FUNC Reset(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAudioInputEndpoint : public iInterface
{
public:
	virtual void cnLib_FUNC Close(void)noexcept(true)=0;
	virtual iReadQueue *cnLib_FUNC GetReadQueue(void)noexcept(true)=0;

	virtual void cnLib_FUNC Start(void)noexcept(true)=0;
	virtual void cnLib_FUNC Stop(void)noexcept(true)=0;
	virtual void cnLib_FUNC Reset(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAudioDevice : public iInterface
{
public:
	virtual iPtr<iAudioOutputEndpoint>	cnLib_FUNC OpenOutput(iInterface *Format,ufInt32 BufferDurationNS)noexcept(true)=0;
	virtual iPtr<iAudioInputEndpoint>	cnLib_FUNC OpenInput(iInterface *Format,ufInt32 BufferDurationNS)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iAudioDeviceWatcher : public iAsyncNotification
{
public:
	virtual iPtr<iAudioDevice> cnLib_FUNC GetDevice(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
iPtr<iInterface>			cnLib_FUNC CreateAudioFormat_PCM(uInt32 Freq,uInt8 bits,uInt8 Channels)noexcept(true);
rPtr<iAudioDeviceWatcher>	cnLib_FUNC QueryAudioDefaultInputDeviceWatcher(void)noexcept(true);
rPtr<iAudioDeviceWatcher>	cnLib_FUNC QueryAudioDefaultOutputDeviceWatcher(void)noexcept(true);
iPtr<iAudioDevice>			cnLib_FUNC QueryAudioMainDevice(void)noexcept(true);
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
