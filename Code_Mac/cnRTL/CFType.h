/*--- CoreFoundation Type -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2013-04-29                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#ifdef __cplusplus
//---------------------------------------------------------------------------
#include <CoreFoundation/CoreFoundation.h>
//---------------------------------------------------------------------------
#include <cnRTL/cnRTL.h>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
template<class TCFRef>
struct cCFRefToken : cnVar::bcPointerRefTokenOperator<TCFRef>
{
	static void Acquire(TCFRef Token){
		if(Token!=nullptr)
			CFRetain(Token);
	}
	static void Release(TCFRef Token){
		if(Token!=nullptr)
			CFRelease(Token);
	}
};
//---------------------------------------------------------------------------

template<class T>
using cfPtr = cnVar::cPtrReference< cCFRefToken<T> >;
//---------------------------------------------------------------------------
template<class T>
cfPtr<T> cfTakeFromManual(T Src)
{
	return cfPtr< cnVar::RemoveConst<T> >::TakeFromManual(Src);
}
//---------------------------------------------------------------------------
template<class T>
cfPtr<T> cfTake(T Src,uInt32 Tag)
{
#ifdef	cnLib_DEBUG
	cnSystem::DebugLogReferenceDec(Src,Tag);
#endif
	return cfPtr< cnVar::RemoveConst<T> >::TakeFromManual(Src);
}
//---------------------------------------------------------------------------
template<class T>
T* cfExtract(cfPtr<T> &Src,uInt32 Tag)
{
#ifdef	cnLib_DEBUG
	cnSystem::DebugLogReferenceInc(Src,Tag);
#endif
	return Src.ExtractToManual();
}
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	/* __cplusplus */

