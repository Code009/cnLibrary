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
class cnLib_INTERFACE iDependentInfo
{
private:
	uIntn _Internal_[4];	// use by system
public:
	virtual rPtr< iArrayReference<const uChar16> > cnLib_FUNC DependentCreateDescription(void)noexcept(true)=0;
	virtual void cnLib_FUNC DependentShutdownNotification(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDependentRegistration
{
public:
	virtual void cnLib_FUNC Register(iDependentInfo *Dependent)noexcept(true)=0;
	virtual void cnLib_FUNC Unregister(iDependentInfo *Dependent)noexcept(true)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
iDependentRegistration* cnLib_FUNC GetSystemDependentRegistration(void)noexcept(true);
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	/* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
