/*- cnRTL - String Classes ------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2006-10-02                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_InterfaceWrapper_H__
#define __cnLibrary_cnRTL_InterfaceWrapper_H__
//---------------------------------------------------------------------------
#include <cnRTL/cnRTLHeader.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
template<class TImplementation>
class impMutex : public iMutex, public TImplementation
{
public:
	using TImplementation::TImplementation;

	virtual void cnLib_FUNC Acquire(void)override{		return TImplementation::Acquire();	}
	virtual bool cnLib_FUNC TryAcquire(void)override{	return TImplementation::TryAcquire();	}
	virtual void cnLib_FUNC Release(void)override{		return TImplementation::Release();	}
};
//---------------------------------------------------------------------------
template<class TImplementation>
class impSharedMutex : public iSharedMutex, public TImplementation
{
public:
	using TImplementation::TImplementation;

	virtual void cnLib_FUNC Acquire(void){			return TImplementation::Acquire();	}
	virtual bool cnLib_FUNC TryAcquire(void){		return TImplementation::TryAcquire();	}
	virtual void cnLib_FUNC Release(void){			return TImplementation::Release();	}
	virtual void cnLib_FUNC AcquireShared(void){	return TImplementation::AcquireShared();	}
	virtual bool cnLib_FUNC TryAcquireShared(void){	return TImplementation::TryAcquireShared();	}
	virtual void cnLib_FUNC ReleaseShared(void){	return TImplementation::ReleaseShared();	}
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
