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
typedef iFunction<void (void)noexcept(true)> iProcedure;
//---------------------------------------------------------------------------
class cnLib_INTERFACE iLibraryReferrer
{
public:
	virtual rPtr<iStringReference> cnLib_FUNC CreateDescription(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iLibraryReference : public iObservedReference
{
public:
	virtual void cnLib_FUNC UpdateDescription(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
rPtr<iLibraryReference> cnLib_FUNC SystemQueryReference(iLibraryReferrer *Referrer)noexcept(true);
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	/* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
