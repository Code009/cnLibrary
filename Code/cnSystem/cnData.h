/*- cnSystem - data -------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-22                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnSystem_cnData_H__
#define	__cnLibrary_cnSystem_cnData_H__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnFundamental.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//- Timepoint ---------------------------------------------------------------
class cnLib_INTERFACE iTimepoint : public iInterface
{
public:
	//	nanoseconds since system epoch
	virtual sInt64 cnLib_FUNC SystemTime(void)noexcept(true)=0;
	virtual sInt64 cnLib_FUNC SinceTime(iTimepoint *Time)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iTextEncodingConverter : public iReference
{
public:
	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)noexcept(true)=0;
};
//---------------------------------------------------------------------------
template<class T>
class cnLib_INTERFACE iReadBuffer
{
public:
	typedef T tElement;

	virtual cArray<const T> cnLib_FUNC GatherReadBuffer(uIntn Length)noexcept(true)=0;
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Length)noexcept(true)=0;

};
//---------------------------------------------------------------------------
template<class T>
class cnLib_INTERFACE iWriteBuffer
{
public:
	typedef T tElement;

	virtual cArray<T> cnLib_FUNC ReserveWriteBuffer(uIntn Length)noexcept(true)=0;
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Length)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iVariable : public iReference
{
public:
	virtual void cnLib_FUNC Clear(void)noexcept(true)=0;
	virtual void* cnLib_FUNC Get(void)noexcept(true)=0;
	virtual void cnLib_FUNC Set(iReference *Reference,void *Value)noexcept(true)=0;
};
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
