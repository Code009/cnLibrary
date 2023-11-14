/*- cnSystem - Fundamental Interface --------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2008-09-21                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnSystem_cnFundamental_H__
#define	__cnLibrary_cnSystem_cnFundamental_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Interface.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
typedef iFunction<void (void)> iProcedure;
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDependentInfo
{
private:
	uIntn _Internal_[4];	// use by system
public:
	virtual rPtr< iArrayReference<const uChar16> > cnLib_FUNC DependentCreateDescription(void)=0;
	virtual void cnLib_FUNC DependentShutdownNotification(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDependentRegistration
{
public:
	virtual void cnLib_FUNC Register(iDependentInfo *Dependent)=0;
	virtual void cnLib_FUNC Unregister(iDependentInfo *Dependent)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
iDependentRegistration* cnLib_FUNC GetSystemDependentRegistration(void);
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	/* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
