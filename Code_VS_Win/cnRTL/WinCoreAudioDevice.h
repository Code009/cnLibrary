/*- cnRTL - Windows - Core Audio Device -----------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-10-22                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinSDKHeader.h>

#include <cnRTL\Stream.h>
#include <cnRTL\WinCommon.h>
#include <cnRTL\WinCOM.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
class cCoreAudioRenderEndpoint : public iAudioOutputEndpoint, public cnRTL::bcWriteQueue, public cnRTL::cDualReference
{
public:
	cCoreAudioRenderEndpoint(COMPtr<IAudioClient> AudioClient,COMPtr<IAudioRenderClient> RenderClient,const WAVEFORMATEX &wf)noexcept(true);
	~cCoreAudioRenderEndpoint()noexcept(true);

	static iPtr<cCoreAudioRenderEndpoint> CreateEndpoint(IMMDevice *Device,const WAVEFORMATEX *WaveFormat,ufInt32 BufferDurationNS)noexcept(true);

	void RenderStart(void)noexcept(true);
	void RenderStop(void)noexcept(true);
	void RenderReset(void)noexcept(true);

	virtual void cnLib_FUNC Close(void)noexcept(true)override;
	virtual iWriteQueue* cnLib_FUNC GetWriteQueue(void)noexcept(true)override;

	virtual void cnLib_FUNC Start(void)noexcept(true)override;
	virtual void cnLib_FUNC Stop(void)noexcept(true)override;
	virtual void cnLib_FUNC Reset(void)noexcept(true)override;

protected:
	void VirtualStopped(void)noexcept(true);

	virtual uIntn cnLib_FUNC GetMaxWriteBufferSize(void)noexcept(true)override;
	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn Size)noexcept(true)override;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)noexcept(true)override;

	virtual iReference* NotificationInnerReference(void)noexcept(true)override;

private:
	COMPtr<IAudioClient> fAudioClient;
	COMPtr<IAudioRenderClient> fRenderClient;
	rPtr<iAsyncTimer> fAsyncTimer;
	
	sInt64 fBufferHalfTimeNS;
	UINT32 fBufferFrameCount;
	uInt32 fFrameSize;

	class cQueueWaveTimerProcedure : public iProcedure
	{
	protected:
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fQueueWaveTimerProcedure;
	void QueueWaveTimer(void)noexcept(true);

};
//---------------------------------------------------------------------------
class cCoreAudioCaptureEndpoint : public iAudioInputEndpoint, public cnRTL::bcReadQueue, public cnRTL::cDualReference
{
public:
	cCoreAudioCaptureEndpoint(COMPtr<IAudioClient> AudioClient,COMPtr<IAudioCaptureClient> CaptureClient,const WAVEFORMATEX &wf)noexcept(true);
	~cCoreAudioCaptureEndpoint()noexcept(true);

	static iPtr<cCoreAudioCaptureEndpoint> CreateEndpoint(IMMDevice *Device,const WAVEFORMATEX *WaveFormat,ufInt32 BufferDurationNS)noexcept(true);

	void CaptureStart(void)noexcept(true);
	void CaptureStop(void)noexcept(true);
	void CaptureReset(void)noexcept(true);

	virtual void cnLib_FUNC Close(void)noexcept(true)override;
	virtual iReadQueue* cnLib_FUNC GetReadQueue(void)noexcept(true)override;

	virtual void cnLib_FUNC Start(void)noexcept(true)override;
	virtual void cnLib_FUNC Stop(void)noexcept(true)override;
	virtual void cnLib_FUNC Reset(void)noexcept(true)override;

protected:
	void VirtualStopped(void)noexcept(true);

	virtual uIntn cnLib_FUNC GetMaxReadBufferSize(void)noexcept(true)override;
	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)noexcept(true)override;
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)noexcept(true)override;

	virtual iReference* NotificationInnerReference(void)noexcept(true)override;


private:
	COMPtr<IAudioClient> fAudioClient;
	COMPtr<IAudioCaptureClient> fCaptureClient;
	rPtr<iAsyncTimer> fAsyncTimer;

	sInt64 fBufferHalfTimeNS;
	UINT32 fBufferFrameCount;
	uInt32 fFrameSize;

	class cQueueWaveTimerProcedure : public iProcedure
	{
	protected:
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fQueueWaveTimerProcedure;

	void QueueWaveTimer(void)noexcept(true);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class cCoreAudioDevice : public iAudioDevice
{
public:
	cCoreAudioDevice(COMPtr<IMMDevice> Device)noexcept(true);
	~cCoreAudioDevice()noexcept(true);

	virtual iPtr<iAudioOutputEndpoint>	cnLib_FUNC OpenOutput(iInterface *Format,uInt32 BufferDurationNS)noexcept(true)override;
	virtual iPtr<iAudioInputEndpoint>	cnLib_FUNC OpenInput(iInterface *Format,uInt32 BufferDurationNS)noexcept(true)override;
private:
	COMPtr<IMMDevice> fDevice;
};
//---------------------------------------------------------------------------
class cCoreAudioDefaultDeviceWatcher : public iAudioDeviceWatcher,cnRTL::bcAsyncQueue, public cnRTL::cDualReference//,private IMMNotificationClient
{
public:
	cCoreAudioDefaultDeviceWatcher(COMPtr<IMMDeviceEnumerator> DeviceEnum,EDataFlow DataFlow)noexcept(true);
	~cCoreAudioDefaultDeviceWatcher()noexcept(true);

	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept(true)override;
	virtual void cnLib_FUNC StopNotify(void)noexcept(true)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)noexcept(true)override;
	virtual bool cnLib_FUNC IsClosed(void)noexcept(true)override;
	virtual iPtr<iAudioDevice> cnLib_FUNC GetDevice(void)noexcept(true)override;

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	iAsyncNotificationCallback *fCallback;
	rPtr<iReference> fCallbackReference;

	virtual iReference* NotificationInnerReference(void)noexcept(true)override;
	virtual void NotificationStarted(void)noexcept(true)override;
	virtual void NotificationStopped(void)noexcept(true)override;
	virtual void NotificationClosed(void)noexcept(true)override;
	virtual CycleState NotificationCheckState(void)noexcept(true)override;
	virtual void AsyncQueueNotify(void)noexcept(true)override;

	COMPtr<IMMDeviceEnumerator> fDeviceEnum;
	EDataFlow fDataFlow;

private:

#if 0
	
	// IMMNotificationClient

	virtual HRESULT STDMETHODCALLTYPE OnDeviceStateChanged( 
		/* [annotation][in] */ 
		_In_  LPCWSTR pwstrDeviceId,
		/* [annotation][in] */ 
		_In_  DWORD dwNewState)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnDeviceAdded( 
		/* [annotation][in] */ 
		_In_  LPCWSTR pwstrDeviceId)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnDeviceRemoved( 
		/* [annotation][in] */ 
		_In_  LPCWSTR pwstrDeviceId)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged( 
		/* [annotation][in] */ 
		_In_  EDataFlow flow,
		/* [annotation][in] */ 
		_In_  ERole role,
		/* [annotation][in] */ 
		_In_  LPCWSTR pwstrDefaultDeviceId)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnPropertyValueChanged( 
		/* [annotation][in] */ 
		_In_  LPCWSTR pwstrDeviceId,
		/* [annotation][in] */ 
		_In_  const PROPERTYKEY key)override;

#endif // 0

};
//---------------------------------------------------------------------------
class cCoreAudioMappingDevice : public iAudioDevice
{
public:
	cCoreAudioMappingDevice(COMPtr<IMMDevice> OutputDevice,COMPtr<IMMDevice> InputDevice)noexcept(true);
	~cCoreAudioMappingDevice()noexcept(true);

	virtual iPtr<iAudioOutputEndpoint>	cnLib_FUNC OpenOutput(iInterface *Format,uInt32 BufferDurationNS)noexcept(true)override;
	virtual iPtr<iAudioInputEndpoint>	cnLib_FUNC OpenInput(iInterface *Format,uInt32 BufferDurationNS)noexcept(true)override;
private:
	COMPtr<IMMDevice> fOutputDevice;
	COMPtr<IMMDevice> fInputDevice;
};
//---------------------------------------------------------------------------
rPtr<iAudioDeviceWatcher>	CoreAudioQueryDefaultDeviceWatcher(EDataFlow DataFlow)noexcept(true);
iPtr<iAudioDevice>			CoreAudioQueryMainDevice(void)noexcept(true);
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
