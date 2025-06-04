/*- cnSystem - Fundamental Interface --------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2008-09-21                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnSystem_cnFundamental_H__
#define	__cnLibrary_cnSystem_cnFundamental_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Interface.hpp>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iModuleReferrer
{
public:
	virtual rPtr<iStringReference> cnLib_FUNC CreateDescription(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iModuleHandle
{
public:
	virtual void cnLib_FUNC Close(void)noexcept(true)=0;
	virtual bool cnLib_FUNC CloseAndWaitUnload(iProcedure *NotifyProcedure)noexcept(true)=0;
	virtual void cnLib_FUNC UpdateDescription(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
iModuleHandle* cnLib_FUNC SystemQueryModule(iModuleReferrer *Referrer)noexcept(true);
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	/* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
