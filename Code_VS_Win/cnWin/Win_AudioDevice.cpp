﻿#include "Win_AudioDevice.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace cnWin;
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
using namespace cnWinNT6;
#endif


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
rPtr<iAudioDeviceWatcher>	AudioDevice::QueryAudioDefaultInputDeviceWatcher(void)
{
	return cnWinRTL::CoreAudioQueryDefaultDeviceWatcher(EDataFlow::eCapture);
}
//---------------------------------------------------------------------------
rPtr<iAudioDeviceWatcher>	AudioDevice::QueryAudioDefaultOutputDeviceWatcher(void)
{
	return cnWinRTL::CoreAudioQueryDefaultDeviceWatcher(EDataFlow::eRender);
}
//---------------------------------------------------------------------------
iPtr<iAudioDevice>			AudioDevice::QueryAudioMainDevice(void)
{
	return cnWinRTL::CoreAudioQueryMainDevice();
}
//---------------------------------------------------------------------------
#endif	//_WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
