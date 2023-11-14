#include "WinCoreAudioDevice.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
static constexpr REFERENCE_TIME RefTime_Second=1'000'000'0;
static constexpr REFERENCE_TIME RefTime_ToNanoSecond=100;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCoreAudioRenderEndpoint::cCoreAudioRenderEndpoint(COMPtr<IAudioClient> AudioClient,COMPtr<IAudioRenderClient> RenderClient,const WAVEFORMATEX &wf)
	: fAudioClient(cnVar::MoveCast(AudioClient))
	, fRenderClient(cnVar::MoveCast(RenderClient))
{

	fFrameSize=wf.nBlockAlign;
    HRESULT hr;
    hr=fAudioClient->GetBufferSize(&fBufferFrameCount);
    if(FAILED(hr)){
		fBufferHalfTimeNS=Time_1s;
	}
	else{
		fBufferHalfTimeNS=Time_1ms*500;
	}
	fBufferHalfTimeNS=fBufferHalfTimeNS*fBufferFrameCount/wf.nSamplesPerSec;

	// wave buffer process thread
	CreateQueueWork(cnSystem::DefaultThreadPool,false);
	fAsyncTimer=cnSystem::DefaultThreadPool->CreateTimer(&fInnerReference,&fQueueWaveTimerProcedure);
}
//---------------------------------------------------------------------------
cCoreAudioRenderEndpoint::~cCoreAudioRenderEndpoint()
{
}
//---------------------------------------------------------------------------
iPtr<cCoreAudioRenderEndpoint> cCoreAudioRenderEndpoint::CreateEndpoint(IMMDevice *Device,const WAVEFORMATEX *WaveFormat,ufInt32 BufferDurationNS)
{
	HRESULT hr;

    COMPtr<IAudioClient> AudioClient;
    hr=Device->Activate(__uuidof(IAudioClient),CLSCTX_ALL,nullptr,COMRetPtr(AudioClient));
	if(FAILED(hr)){
		return nullptr;
	}
	
	auto BufTimeRef=static_cast<REFERENCE_TIME>(BufferDurationNS/RefTime_ToNanoSecond);
    hr = AudioClient->Initialize(
                         AUDCLNT_SHAREMODE_SHARED,	// ShareMode
                         0,							// StreamFlags
                         BufTimeRef,				// hnsBufferDuration
                         0,							// hnsPeriodicity
                         WaveFormat,				// pFormat
                         NULL);						// AudioSessionGuid
 	if(FAILED(hr)){
		return nullptr;
	}

	COMPtr<IAudioRenderClient> RenderClient;
	hr=AudioClient->GetService(__uuidof(IAudioRenderClient),COMRetPtr(RenderClient));
    if(FAILED(hr)){
		return nullptr;
	}
	return iCreate<cCoreAudioRenderEndpoint>(cnVar::MoveCast(AudioClient),cnVar::MoveCast(RenderClient),*WaveFormat);
}
//---------------------------------------------------------------------------
void cCoreAudioRenderEndpoint::VirtualStopped(void)
{
	RenderStop();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
void cCoreAudioRenderEndpoint::Close(void)
{
	CloseQueue();
}
//---------------------------------------------------------------------------
iWriteQueue* cCoreAudioRenderEndpoint::GetWriteQueue(void)
{
	return this;
}
//---------------------------------------------------------------------------
void cCoreAudioRenderEndpoint::RenderStart(void)
{
	fAudioClient->Start();
	fAsyncTimer->Start(nullptr,fBufferHalfTimeNS,fBufferHalfTimeNS);
}
//---------------------------------------------------------------------------
void cCoreAudioRenderEndpoint::RenderStop(void)
{
	fAsyncTimer->Stop();
	fAudioClient->Stop();
}
//---------------------------------------------------------------------------
void cCoreAudioRenderEndpoint::RenderReset(void)
{
	fAudioClient->Reset();
}
//---------------------------------------------------------------------------
void cCoreAudioRenderEndpoint::Start(void)
{
	RenderStart();
}
//---------------------------------------------------------------------------
void cCoreAudioRenderEndpoint::Stop(void)
{
	RenderStop();
}
//---------------------------------------------------------------------------
void cCoreAudioRenderEndpoint::Reset(void)
{
	RenderReset();
}
//---------------------------------------------------------------------------
void cCoreAudioRenderEndpoint::cQueueWaveTimerProcedure::Execute(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cCoreAudioRenderEndpoint::fQueueWaveTimerProcedure);
	Host->QueueWaveTimer();
}
//---------------------------------------------------------------------------
iReference* cCoreAudioRenderEndpoint::NotificationInnerReference(void)
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cCoreAudioRenderEndpoint::QueueWaveTimer(void)
{
    HRESULT hr;
    // See how much buffer space is available.
	UINT32 numFramesPadding;
    hr=fAudioClient->GetCurrentPadding(&numFramesPadding);
	if(FAILED(hr)){
		return;
	}
    UINT32 numFramesAvailable = fBufferFrameCount - numFramesPadding;
	if(numFramesAvailable<fBufferFrameCount/2){
		// not available
		return;
	}

	WriteQueueReportBufferAvailable(false);
}
//---------------------------------------------------------------------------
cMemory cCoreAudioRenderEndpoint::ReserveWriteBuffer(uIntn)
{
    HRESULT hr;
    // See how much buffer space is available.
	UINT32 numFramesPadding;
    hr = fAudioClient->GetCurrentPadding(&numFramesPadding);
	if(FAILED(hr)){
		return NullArray;
	}
    
    UINT32 numFramesAvailable = fBufferFrameCount - numFramesPadding;
	if(numFramesAvailable<fBufferFrameCount/2){
		return NullArray;
	}

	BYTE *pData;
    // Grab all the available space in the shared buffer.
    hr=fRenderClient->GetBuffer(numFramesAvailable, &pData);
	if(FAILED(hr)){
		switch(hr){
		case AUDCLNT_E_BUFFER_TOO_LARGE:
		case AUDCLNT_E_OUT_OF_ORDER:
		case AUDCLNT_E_BUFFER_OPERATION_PENDING:
			return NullArray;
		}
		// error
		CloseQueue();
		return NullArray;
	}
    // Load data into the shared buffer.
	cMemory Buffer;
	Buffer.Pointer=pData;
	Buffer.Length=numFramesAvailable*fFrameSize;
	return Buffer;
}
//---------------------------------------------------------------------------
void cCoreAudioRenderEndpoint::CommitWriteBuffer(uIntn Size)
{
	uInt32 FrameCopyied=static_cast<uInt32>(Size/fFrameSize);

    HRESULT hr;
    hr=fRenderClient->ReleaseBuffer(FrameCopyied,0);
	if(FAILED(hr)){
		switch(hr){
		case AUDCLNT_E_OUT_OF_ORDER:
			return;
		}
		// error
		CloseQueue();
		return;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCoreAudioCaptureEndpoint::cCoreAudioCaptureEndpoint(COMPtr<IAudioClient> AudioClient,COMPtr<IAudioCaptureClient> CaptureClient,const WAVEFORMATEX &wf)
	: fAudioClient(cnVar::MoveCast(AudioClient))
	, fCaptureClient(cnVar::MoveCast(CaptureClient))
{

	fFrameSize=wf.nBlockAlign;
    HRESULT hr;
    hr = fAudioClient->GetBufferSize(&fBufferFrameCount);
    if(FAILED(hr)){
		fBufferHalfTimeNS=Time_1s;
	}
	else{
		fBufferHalfTimeNS=500*Time_1ms;
	}
	fBufferHalfTimeNS=fBufferHalfTimeNS*fBufferFrameCount/wf.nSamplesPerSec;

	CreateQueueWork(cnSystem::DefaultThreadPool,false);
	fAsyncTimer=cnSystem::DefaultThreadPool->CreateTimer(&fInnerReference,&fQueueWaveTimerProcedure);
}
//---------------------------------------------------------------------------
cCoreAudioCaptureEndpoint::~cCoreAudioCaptureEndpoint()
{
}
//---------------------------------------------------------------------------
iPtr<cCoreAudioCaptureEndpoint> cCoreAudioCaptureEndpoint::CreateEndpoint(IMMDevice *Device,const WAVEFORMATEX *WaveFormat,ufInt32 BufferDurationNS)
{
	HRESULT hr;
    COMPtr<IAudioClient> AudioClient;
    hr=Device->Activate(__uuidof(IAudioClient),CLSCTX_ALL,nullptr,COMRetPtr(AudioClient));
	if(FAILED(hr)){
		return nullptr;
	}
	auto BufTimeRef=static_cast<REFERENCE_TIME>(BufferDurationNS/RefTime_ToNanoSecond);
    
    hr = AudioClient->Initialize(
                         AUDCLNT_SHAREMODE_SHARED,	// ShareMode
                         0,							// StreamFlags
                         BufTimeRef,				// hnsBufferDuration
                         0,							// hnsPeriodicity
                         WaveFormat,					// pFormat
                         NULL);						// AudioSessionGuid
 	if(FAILED(hr)){
		return nullptr;
	}

	COMPtr<IAudioCaptureClient> CaptureClient;
    hr=AudioClient->GetService(__uuidof(IAudioCaptureClient),COMRetPtr(CaptureClient));
    if(FAILED(hr)){
		return nullptr;
	}

	return iCreate<cCoreAudioCaptureEndpoint>(cnVar::MoveCast(AudioClient),cnVar::MoveCast(CaptureClient),*WaveFormat);
}
//---------------------------------------------------------------------------
void cCoreAudioCaptureEndpoint::VirtualStopped(void)
{
	CaptureStop();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
void cCoreAudioCaptureEndpoint::Close(void)
{
	return CloseQueue();
}
//---------------------------------------------------------------------------
iReadQueue* cCoreAudioCaptureEndpoint::GetReadQueue(void)
{
	return this;
}
//---------------------------------------------------------------------------
void cCoreAudioCaptureEndpoint::Start(void)
{
	return CaptureStart();
}
//---------------------------------------------------------------------------
void cCoreAudioCaptureEndpoint::Stop(void)
{
	return CaptureStop();
}
//---------------------------------------------------------------------------
void cCoreAudioCaptureEndpoint::Reset(void)
{
	return CaptureReset();
}
//---------------------------------------------------------------------------
void cCoreAudioCaptureEndpoint::CaptureStart(void)
{
	fAsyncTimer->Start(nullptr,fBufferHalfTimeNS,fBufferHalfTimeNS);
	fAudioClient->Start();
}
//---------------------------------------------------------------------------
void cCoreAudioCaptureEndpoint::CaptureStop(void)
{
	fAudioClient->Stop();
	fAsyncTimer->Stop();
}
//---------------------------------------------------------------------------
void cCoreAudioCaptureEndpoint::CaptureReset(void)
{
	fAudioClient->Reset();
}
//---------------------------------------------------------------------------
void cCoreAudioCaptureEndpoint::cQueueWaveTimerProcedure::Execute(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cCoreAudioCaptureEndpoint::fQueueWaveTimerProcedure);
	return Host->QueueWaveTimer();
}
//---------------------------------------------------------------------------
void cCoreAudioCaptureEndpoint::QueueWaveTimer(void)
{
	HRESULT hr;
	UINT32 PacketLength=0;
	hr=fCaptureClient->GetNextPacketSize(&PacketLength);
	if(FAILED(hr)){
		ReadQueueReportTerminated();
		return;
	}
	if(PacketLength!=0){
		ReadQueueReportBufferAvailable(false);
	}
	else{
		ReadQueueReportBufferEmpty();
	}
}
//---------------------------------------------------------------------------
iReference* cCoreAudioCaptureEndpoint::NotificationInnerReference(void)
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
cConstMemory cCoreAudioCaptureEndpoint::GatherReadBuffer(uIntn)
{
	HRESULT hr;
	UINT32 PacketLength;
	hr = fCaptureClient->GetNextPacketSize(&PacketLength);
	if(FAILED(hr)){
		return NullArray;
	}

	if(PacketLength==0){
		// no data
		return NullArray;
	}
	BYTE *pData;
	UINT32 numFramesAvailable;
	DWORD flags;

    // Get the available data in the shared buffer.
    hr = fCaptureClient->GetBuffer(
                            &pData,
                            &numFramesAvailable,
                            &flags, nullptr, nullptr);
	if(FAILED(hr)){
		switch(hr){
		case AUDCLNT_E_BUFFER_TOO_LARGE:
		case AUDCLNT_E_OUT_OF_ORDER:
		case AUDCLNT_E_BUFFER_OPERATION_PENDING:
			return NullArray;
		}
		// error
		CloseQueue();
		return NullArray;
	}
	uIntn BufferSize=numFramesAvailable*fFrameSize;

	cMemoryBuffer SilentData;
    if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
    {
		SilentData.SetSize(BufferSize);
		cnMemory::ZeroFill(SilentData->Pointer,BufferSize);
		pData=static_cast<BYTE*>(SilentData->Pointer);
    }

	cConstMemory Buffer;
	Buffer.Pointer=pData;
	Buffer.Length=BufferSize;
	return Buffer;
}
//---------------------------------------------------------------------------
void cCoreAudioCaptureEndpoint::DismissReadBuffer(uIntn Size)
{
	uIntn FramesRead=Size/fFrameSize;
	HRESULT hr;
	hr = fCaptureClient->ReleaseBuffer(static_cast<UINT32>(FramesRead));
	if(FAILED(hr)){
		switch(hr){
		case AUDCLNT_E_OUT_OF_ORDER:
			return;
		}
		// error
		CloseQueue();
		return;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCoreAudioDevice::cCoreAudioDevice(COMPtr<IMMDevice> Device)
	:fDevice(MoveCast(Device))
{
}
//---------------------------------------------------------------------------
cCoreAudioDevice::~cCoreAudioDevice()
{
}
//---------------------------------------------------------------------------
iPtr<iAudioOutputEndpoint> cCoreAudioDevice::OpenOutput(iInterface *Format,uInt32 BufferDurationNS)
{
	auto WaveFormat=iCast<iWaveFormat>(Format);
	if(WaveFormat==nullptr){
		return nullptr;
	}
	auto wf=WaveFormat->GetFormat();
	return cCoreAudioRenderEndpoint::CreateEndpoint(fDevice,wf,BufferDurationNS);
}
//---------------------------------------------------------------------------
iPtr<iAudioInputEndpoint> cCoreAudioDevice::OpenInput(iInterface *Format,uInt32 BufferDurationNS)
{
	auto WaveFormat=iCast<iWaveFormat>(Format);
	if(WaveFormat==nullptr){
		return nullptr;
	}
	auto wf=WaveFormat->GetFormat();
	return cCoreAudioCaptureEndpoint::CreateEndpoint(fDevice,wf,BufferDurationNS);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCoreAudioDefaultDeviceWatcher::cCoreAudioDefaultDeviceWatcher(COMPtr<IMMDeviceEnumerator> DeviceEnum,EDataFlow DataFlow)
	: fDeviceEnum(cnVar::MoveCast(DeviceEnum))
	, fDataFlow(DataFlow)
	, fCallback(nullptr)
{
}
//---------------------------------------------------------------------------
cCoreAudioDefaultDeviceWatcher::~cCoreAudioDefaultDeviceWatcher()
{
}
//---------------------------------------------------------------------------
void cCoreAudioDefaultDeviceWatcher::VirtualStarted(void)
{
	InnerIncReference('self');
	//fDeviceEnum->RegisterEndpointNotificationCallback(this);
}
//---------------------------------------------------------------------------
void cCoreAudioDefaultDeviceWatcher::VirtualStopped(void)
{
	CloseQueue();

	InnerDecReference('self');
}
//---------------------------------------------------------------------------
bool cCoreAudioDefaultDeviceWatcher::StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)
{
	if(PrepareStartNotify()==false)
		return false;

	fCallback=Callback;
	fCallbackReference=Reference;

	ConfirmStartNotify();
	UpdateQueueState(false);
	return true;
}
//---------------------------------------------------------------------------
void cCoreAudioDefaultDeviceWatcher::StopNotify(void)
{
	if(bcAsyncQueue::StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void cCoreAudioDefaultDeviceWatcher::NotifyCallback(bool IdleNotify)
{
	return NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool cCoreAudioDefaultDeviceWatcher::IsClosed(void)
{
	return IsNotificationClosed();
}
//---------------------------------------------------------------------------
iPtr<iAudioDevice> cCoreAudioDefaultDeviceWatcher::GetDevice(void)
{
	HRESULT hr;
	COMPtr<IMMDevice> Device;
	hr=fDeviceEnum->GetDefaultAudioEndpoint(fDataFlow,eMultimedia,COMRetPtrT(Device));
	if(FAILED(hr)){
		return nullptr;
	}

	return iCreate<cCoreAudioDevice>(cnVar::MoveCast(Device));
}
//---------------------------------------------------------------------------
iReference* cCoreAudioDefaultDeviceWatcher::NotificationInnerReference(void)
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cCoreAudioDefaultDeviceWatcher::NotificationStarted(void)
{
	bcAsyncQueue::NotificationStarted();
	
	rIncReference(this,'auqu');
	fCallback->AsyncStarted();
}
//---------------------------------------------------------------------------
void cCoreAudioDefaultDeviceWatcher::NotificationStopped(void)
{
	auto Reference=cnVar::MoveCast(fCallbackReference);
	auto Callback=fCallback;
	fCallback=nullptr;
	Callback->AsyncStopped();

	bcAsyncQueue::NotificationStopped();
	rDecReference(this,'auqu');
}
//---------------------------------------------------------------------------
void cCoreAudioDefaultDeviceWatcher::NotificationClosed(void)
{
	bcAsyncQueue::NotificationClosed();

	//fDeviceEnum->UnregisterEndpointNotificationCallback(this);
}
//---------------------------------------------------------------------------
void cCoreAudioDefaultDeviceWatcher::AsyncQueueNotify(void)
{
	fCallback->AsyncNotify();
}
//---------------------------------------------------------------------------
cCoreAudioDefaultDeviceWatcher::CycleState cCoreAudioDefaultDeviceWatcher::NotificationCheckState(void)
{
	return CycleState::Normal;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCoreAudioMappingDevice::cCoreAudioMappingDevice(COMPtr<IMMDevice> OutputDevice,COMPtr<IMMDevice> InputDevice)
	: fOutputDevice(cnVar::MoveCast(OutputDevice))
	, fInputDevice(cnVar::MoveCast(InputDevice))
{
}
//---------------------------------------------------------------------------
cCoreAudioMappingDevice::~cCoreAudioMappingDevice()
{
}
//---------------------------------------------------------------------------
iPtr<iAudioOutputEndpoint> cCoreAudioMappingDevice::OpenOutput(iInterface *Format,uInt32 BufferDurationNS)
{
	if(fOutputDevice==nullptr)
		return nullptr;
	auto WaveFormat=iCast<iWaveFormat>(Format);
	if(WaveFormat==nullptr){
		return nullptr;
	}
	auto wf=WaveFormat->GetFormat();

	return cCoreAudioRenderEndpoint::CreateEndpoint(fOutputDevice,wf,BufferDurationNS);
}
//---------------------------------------------------------------------------
iPtr<iAudioInputEndpoint> cCoreAudioMappingDevice::OpenInput(iInterface *Format,uInt32 BufferDurationNS)
{
	if(fInputDevice==nullptr)
		return nullptr;
	auto WaveFormat=iCast<iWaveFormat>(Format);
	if(WaveFormat==nullptr){
		return nullptr;
	}
	auto wf=WaveFormat->GetFormat();
	return cCoreAudioCaptureEndpoint::CreateEndpoint(fInputDevice,wf,BufferDurationNS);
}
//---------------------------------------------------------------------------
rPtr<iAudioDeviceWatcher> cnWinRTL::CoreAudioQueryDefaultDeviceWatcher(EDataFlow DataFlow)
{
	HRESULT hr;
	
	COMPtr<IMMDeviceEnumerator> DeviceEnum;
	hr=::CoCreateInstance(__uuidof(MMDeviceEnumerator),nullptr,CLSCTX_ALL,__uuidof(IMMDeviceEnumerator),COMRetPtr(DeviceEnum));
	if(FAILED(hr)){
		return nullptr;
	}

	return rCreate<cCoreAudioDefaultDeviceWatcher>(cnVar::MoveCast(DeviceEnum),DataFlow);
}
//---------------------------------------------------------------------------
iPtr<iAudioDevice> cnWinRTL::CoreAudioQueryMainDevice(void)
{
	HRESULT hr;
	
	COMPtr<IMMDeviceEnumerator> DeviceEnum;
	hr=::CoCreateInstance(__uuidof(MMDeviceEnumerator),nullptr,CLSCTX_ALL,__uuidof(IMMDeviceEnumerator),COMRetPtr(DeviceEnum));
	if(FAILED(hr)){
		return nullptr;
	}

	bool AllFailed=true;

	COMPtr<IMMDevice> OutputDevice;
	hr=DeviceEnum->GetDefaultAudioEndpoint(EDataFlow::eRender,ERole::eMultimedia,COMRetPtrT(OutputDevice));

	AllFailed&=FAILED(hr);
	
	COMPtr<IMMDevice> InputDevice;
	hr=DeviceEnum->GetDefaultAudioEndpoint(EDataFlow::eCapture,ERole::eMultimedia,COMRetPtrT(InputDevice));
	AllFailed&=FAILED(hr);

	if(AllFailed){
		return nullptr;
	}

	return iCreate<cCoreAudioMappingDevice>(cnVar::MoveCast(OutputDevice),cnVar::MoveCast(InputDevice));
}
//---------------------------------------------------------------------------
#endif	//_WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
