/*--- Objective-C Type ----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2013-04-29                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#ifdef	__OBJC__

#include <objc/objc.h>
#include <objc/runtime.h>
#include <objc/NSObject.h>

#ifdef __cplusplus

#include <cnRTL/cnRTL.h>
#include <cnSystem/cnMac_Sys.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifdef	cnLib_DEBUG
template<class T>
union ocStaticIVar{
	ocStaticIVar(){}
	~ocStaticIVar(){}
	typename cnVar::TIntegerOfSize<alignof(T),false>::Type Storage[cnMath::IntRoundUpDiv(sizeof(T),alignof(T))];
	T Object;
};
#else
template<class T>
using ocStaticIVar=typename cnVar::TIntegerOfSize<alignof(T),false>::Type[cnMath::IntRoundUpDiv(sizeof(T),alignof(T))];
#endif
//---------------------------------------------------------------------------
void ocPtrAcquire(id __unsafe_unretained Object);
void ocPtrRelease(id __unsafe_unretained Object);
//---------------------------------------------------------------------------
template<class T>
struct cOCRefToken : cnVar::bcPointerRefTokenOperator<T *__unsafe_unretained>
{
	static void Acquire(T *__unsafe_unretained Token){
		ocPtrAcquire(Token);
	}
	static void Release(T *__unsafe_unretained Token){
		ocPtrRelease(Token);
	}
};
//---------------------------------------------------------------------------
template<class T>
using ocPtr = cnVar::cPtrReference< cOCRefToken<T> >;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template<class T>
ocPtr<T> ocTake(T *Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_DEC(static_cast<T*>(Src),Tag);
	return ocPtr<T>::TakeFromManual(Src);
}
//---------------------------------------------------------------------------
template<class T>
T* ocExtract(ocPtr<T> &Src,uInt32 Tag)
{
	cnRTL_DEBUG_LOG_REFERENCE_INC(static_cast<T*>(Src),Tag);
	return Src.ExtractToManual();
}
//---------------------------------------------------------------------------
// ocSafeCast
//	cast objective-c pointer, and check if convertable
//	return safely casted pointer , or nil if unconvertable
template<class TCast>
inline TCast* ocSafeCast(id<NSObject> object)
{
	if([object isKindOfClass:[TCast class]])
		return static_cast<TCast*>(object);
	return nil;
}
//---------------------------------------------------------------------------
inline id ociOCObject(iInterface *Interface)
{
	auto OCObject=iCast<iOCObject>(Interface);
	if(OCObject==nullptr)
		return nullptr;
	return OCObject->GetOCObject();
}
//---------------------------------------------------------------------------
template<class TCast>
inline TCast* ociSafeCast(iInterface *Interface)
{
	auto OCObject=iCast<iOCObject>(Interface);
	if(OCObject==nullptr)
		return nullptr;
	id object=OCObject->GetOCObject();
	return ocSafeCast<TCast>(object);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ocTypeEncode_IsSame(const char *TypeName)
{
	return cnString::Equal(TypeName,@encode(T),sizeof(@encode(T)));
}
//---------------------------------------------------------------------------
//	NSString
//---------------------------------------------------------------------------
NSString* NSStringFromCStr(const uChar16 *String,uIntn Length=cnString::StringMaxLength);
//---------------------------------------------------------------------------
uIntn NSStringAddTo(cStringBuffer<uChar16> &Buffer,NSString *str);
cString<uChar16> NSStringToCStr(NSString *str);
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------

#endif	/* __cplusplus */
#endif	// __OBJC__
