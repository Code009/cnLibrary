/*- cnWin - Audio Device --------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-10-22                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnWin\Win_Common.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
namespace cnWinNT6{
//---------------------------------------------------------------------------
namespace AudioDevice{
//---------------------------------------------------------------------------
rPtr<iAudioDeviceWatcher>	QueryAudioDefaultInputDeviceWatcher(void)noexcept(true);
rPtr<iAudioDeviceWatcher>	QueryAudioDefaultOutputDeviceWatcher(void)noexcept(true);
iPtr<iAudioDevice>			QueryAudioMainDevice(void)noexcept(true);
//---------------------------------------------------------------------------
}	//	namespace AudioDevice
//---------------------------------------------------------------------------
}	// namespace cnWinNT6
//---------------------------------------------------------------------------
#if cnWin_AUDIODEVICE_VERSION < _WIN32_WINNT_VISTA
#undef	cnWin_AUDIODEVICE_VERSION
#undef	cnWin_AUDIODEVICE_NAMESPACE
#define	cnWin_AUDIODEVICE_VERSION		_WIN32_WINNT_VISTA
#define	cnWin_AUDIODEVICE_NAMESPACE		cnWinNT6::AudioDevice
#endif // cnWin_AUDIODEVICE_VERSION < _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
