/*- cnRTL - Windows - Core Audio Device -----------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-10-22                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinSDKHeader.h>

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
	cCoreAudioRenderEndpoint(COMPtr<IAudioClient> AudioClient,COMPtr<IAudioRenderClient> RenderClient,const WAVEFORMATEX &wf);
	~cCoreAudioRenderEndpoint();

	static iPtr<cCoreAudioRenderEndpoint> CreateEndpoint(IMMDevice *Device,const WAVEFORMATEX *WaveFormat,ufInt32 BufferDurationNS);

	void RenderStart(void);
	void RenderStop(void);
	void RenderReset(void);

	virtual void cnLib_FUNC Close(void)override;
	virtual iWriteQueue* cnLib_FUNC GetWriteQueue(void)override;

	virtual void cnLib_FUNC Start(void)override;
	virtual void cnLib_FUNC Stop(void)override;
	virtual void cnLib_FUNC Reset(void)override;

protected:
	void VirtualStopped(void);

	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn Size)override;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)override;

	virtual iReference* NotificationInnerReference(void)override;

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
		virtual void cnLib_FUNC Execute(void)override;
	}fQueueWaveTimerProcedure;
	void QueueWaveTimer(void);

};
//---------------------------------------------------------------------------
class cCoreAudioCaptureEndpoint : public iAudioInputEndpoint, public cnRTL::bcReadQueue, public cnRTL::cDualReference
{
public:
	cCoreAudioCaptureEndpoint(COMPtr<IAudioClient> AudioClient,COMPtr<IAudioCaptureClient> CaptureClient,const WAVEFORMATEX &wf);
	~cCoreAudioCaptureEndpoint();

	static iPtr<cCoreAudioCaptureEndpoint> CreateEndpoint(IMMDevice *Device,const WAVEFORMATEX *WaveFormat,ufInt32 BufferDurationNS);

	void CaptureStart(void);
	void CaptureStop(void);
	void CaptureReset(void);

	virtual void cnLib_FUNC Close(void)override;
	virtual iReadQueue* cnLib_FUNC GetReadQueue(void)override;

	virtual void cnLib_FUNC Start(void)override;
	virtual void cnLib_FUNC Stop(void)override;
	virtual void cnLib_FUNC Reset(void)override;

protected:
	void VirtualStopped(void);

	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)override;
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)override;

	virtual iReference* NotificationInnerReference(void)override;


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
		virtual void cnLib_FUNC Execute(void)override;
	}fQueueWaveTimerProcedure;

	void QueueWaveTimer(void);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class cCoreAudioDevice : public iAudioDevice
{
public:
	cCoreAudioDevice(COMPtr<IMMDevice> Device);
	~cCoreAudioDevice();

	virtual iPtr<iAudioOutputEndpoint>	cnLib_FUNC OpenOutput(iInterface *Format,uInt32 BufferDurationNS)override;
	virtual iPtr<iAudioInputEndpoint>	cnLib_FUNC OpenInput(iInterface *Format,uInt32 BufferDurationNS)override;
private:
	COMPtr<IMMDevice> fDevice;
};
//---------------------------------------------------------------------------
class cCoreAudioDefaultDeviceWatcher : public iAudioDeviceWatcher,cnRTL::bcAsyncQueue, public cnRTL::cDualReference//,private IMMNotificationClient
{
public:
	cCoreAudioDefaultDeviceWatcher(COMPtr<IMMDeviceEnumerator> DeviceEnum,EDataFlow DataFlow);
	~cCoreAudioDefaultDeviceWatcher();

	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)override;
	virtual void cnLib_FUNC StopNotify(void)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)override;
	virtual bool cnLib_FUNC IsClosed(void)override;
	virtual iPtr<iAudioDevice> cnLib_FUNC GetDevice(void)override;

protected:
	void VirtualStarted(void);
	void VirtualStopped(void);

	iAsyncNotificationCallback *fCallback;
	rPtr<iReference> fCallbackReference;

	virtual iReference* NotificationInnerReference(void)override;
	virtual void NotificationStarted(void)override;
	virtual void NotificationStopped(void)override;
	virtual void NotificationClosed(void)override;
	virtual CycleState NotificationCheckState(void)override;
	virtual void AsyncQueueNotify(void)override;

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
	cCoreAudioMappingDevice(COMPtr<IMMDevice> OutputDevice,COMPtr<IMMDevice> InputDevice);
	~cCoreAudioMappingDevice();

	virtual iPtr<iAudioOutputEndpoint>	cnLib_FUNC OpenOutput(iInterface *Format,uInt32 BufferDurationNS)override;
	virtual iPtr<iAudioInputEndpoint>	cnLib_FUNC OpenInput(iInterface *Format,uInt32 BufferDurationNS)override;
private:
	COMPtr<IMMDevice> fOutputDevice;
	COMPtr<IMMDevice> fInputDevice;
};
//---------------------------------------------------------------------------
rPtr<iAudioDeviceWatcher>	CoreAudioQueryDefaultDeviceWatcher(EDataFlow DataFlow);
iPtr<iAudioDevice>			CoreAudioQueryMainDevice(void);
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
