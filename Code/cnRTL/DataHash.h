/*- cnRTL - Data Hash Functions -------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2010-12-23                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_DataHash_H__
#define __cnLibrary_cnRTL_DataHash_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnRTL_CONSTEXPR_FUNC	constexpr
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnRTL_CONSTEXPR_FUNC
#endif // cnLibrary_CPPFEATURE_CONSTEXPR < 200704L

#if	cnLibrary_CPPFEATURE_IF_CONSTEXPR >= 201606L
#define	cnRTL_IFCONSTEXPR	constexpr
#else	// cnLibrary_CPPFEATURE_IF_CONSTEXPR < 201606L
#define	cnRTL_IFCONSTEXPR
#endif	// cnLibrary_CPPFEATURE_IF_CONSTEXPR
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
template<class T>
cnRTL_CONSTEXPR_FUNC T CRC_ByteTable(uInt8 Value,T Polynomial)
{
	T crc;
	crc = static_cast<T>(Value) << (sizeof(T)*ByteBitCount - ByteBitCount);
	for(uInt8 bi=0;bi<ByteBitCount;bi++){
		bool s=(crc&cnVar::IntegerMSBValue<T>)!=0;
		crc<<=1;
		if(s)
			crc^=Polynomial;
	}
    return crc;
}
//---------------------------------------------------------------------------
template<class T>
cnRTL_CONSTEXPR_FUNC T CRCReverse_ByteTable(uInt8 Value,T Polynomial)
{
    uInt8 Temp=Value;
	for(uInt8 i=0;i<ByteBitCount;i++){
		Value<<=1;
		Value|=Temp&1;
		Temp>>=1;
	}

	T crc=CRC_ByteTable(Value,Polynomial);
    T ct=crc;
	for(uInt8 i=0;i<cnVar::TIntegerValue<T>::BitCount;i++){
		crc<<=1;
		crc|=ct&1;
		ct>>=1;
	}
	return crc;
}
//---------------------------------------------------------------------------
uInt8 CRC8_LT(const void *Buffer,uIntn Size,const uInt8 *Table,uInt8 Accum);
//---------------------------------------------------------------------------
template<class T>
T CRC_LT(const void *Buffer,uIntn Size,const T *Table,typename cnVar::TTypeDef<T>::Type Accum)
{
	if cnRTL_IFCONSTEXPR(sizeof(T)==1){
		return CRC8_LT(Buffer,Size,reinterpret_cast<const uInt8*>(Table),Accum);
	}
	else{
		if(Size==0)
			return 0;

		cnLib_ASSERT(Buffer!=nullptr);
		auto Data=static_cast<const uInt8*>(Buffer);
		const uInt8 &AccumHighByte=reinterpret_cast<const uInt8*>(&Accum)[sizeof(T)-1];
		for(uIntn i=0;i<Size;i++){
			T TableValue=Table[AccumHighByte^Data[i] ];
			Accum<<=ByteBitCount;
			Accum^=TableValue;
		}
		return Accum; 
	}
}
//---------------------------------------------------------------------------
template<class T>
T CRCReverse_LT(const void *Buffer,uIntn Size,const T *Table,typename cnVar::TTypeDef<T>::Type Accum)
{
	if cnRTL_IFCONSTEXPR(sizeof(T)==1){
		return CRC8_LT(Buffer,Size,reinterpret_cast<const uInt8*>(Table),Accum);
	}
	else{
		if(Size==0)
			return 0;

		cnLib_ASSERT(Buffer!=nullptr);
		auto Data=static_cast<const uInt8*>(Buffer);
		const uInt8 &AccumLowByte=*reinterpret_cast<const uInt8*>(&Accum);
		for(uIntn i=0;i<Size;i++){
			T TableValue=Table[AccumLowByte^Data[i] ];
			Accum>>=ByteBitCount;
			Accum^=TableValue;
		}
		return Accum; 
	}
}
//---------------------------------------------------------------------------
uInt8 CRC8_07(const void *Buffer,uIntn Size);
uInt8 CRC8_EBU(const void *Buffer,uIntn Size);
uInt8 CRC8_ICODE(const void *Buffer,uIntn Size);
//---------------------------------------------------------------------------
uInt16 CRC16_ARC(const void *Buffer,uIntn Size);
uInt16 CRC16_BUYPASS(const void *Buffer,uIntn Size);
//---------------------------------------------------------------------------
uInt32 CRC32_04C11DB7(const void *Buffer,uIntn Size);
uInt32 CRC32_POSIX(const void *Buffer,uIntn Size);
//---------------------------------------------------------------------------
}   // namespace cnRTL
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#undef	cnRTL_CONSTEXPR_FUNC
#undef	cnRTL_IFCONSTEXPR
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif

