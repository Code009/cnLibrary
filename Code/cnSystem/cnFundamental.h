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
	uIntn SystemReserved[4];	// used by iDependentRegistration
public:
	virtual rPtr<iStringReference> cnLib_FUNC DependentCreateDescription(void)noexcept(true){
		UnusedParameter(SystemReserved);	// compiler may complain SystemReserved will never be used
		return nullptr;
	}
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
extern iDependentRegistration*const SystemDependentRegistration;
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	/* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
