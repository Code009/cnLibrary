/*- cnRTL - Windows - COM IO ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-06-28                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinCommon.h>
#include <cnRTL\WinCOM.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
class cCOMSyncStream : public iFileSyncStream
{
public:
	cCOMSyncStream(COMPtr<IStream> Stream);
	~cCOMSyncStream();


	virtual void cnLib_FUNC Close(void)override;
	virtual bool cnLib_FUNC IsEndOfReading(void)override;
	virtual bool cnLib_FUNC SetEndOfStream(void)override;
	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)override;
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)override;

	virtual uInt64 cnLib_FUNC GetPointer(void)override;
	virtual bool cnLib_FUNC MovePointerBegin(uInt64 Offset)override;
	virtual bool cnLib_FUNC MovePointerCurrent(sInt64 Offset)override;
	virtual bool cnLib_FUNC MovePointerEnd(sInt64 Offset)override;

	//virtual eStreamError cnLib_FUNC GetStreamError(void)override;
protected:
	COMPtr<IStream> fStream;
	STATSTG fCachedState;
	bool fStreamStateCached;
	bool fIsEndOfReading;

	void SetupState(void);
};
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
