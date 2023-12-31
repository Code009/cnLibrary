/*--- Mac - Thread --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2012-05-28                                          */
/*-------------------------------------------------------------------------*/
#pragma once

//---------------------------------------------------------------------------
#include <cnMac/Mac_Header.h>
/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnMac{
//---------------------------------------------------------------------------
class cClockService
{
public:
	cClockService();
	~cClockService();

	sInt64 GetSystemTimeNow(void)const;

private:
	clock_serv_t fClockService;
};
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
