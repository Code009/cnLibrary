/*- cnTK - Common Template ------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-08-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Common_HPP__
#define	__cnLibrary_cnTK_Common_HPP__
/*-------------------------------------------------------------------------*/
#include <cnTK/cnPlatform.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------


#if defined(cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT)\
	|| defined(cnLibrary_CPPEXCLUDE_NULLPTR)\
	|| ( defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) && defined(cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION) )

#define	cnLibrary_CPPFEATURELEVEL	1

#elif cnLibrary_CPPFEATURE_DECLTYPE < 200707L\
	|| cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L\
	|| cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L\
	|| cnLibrary_CPPFEATURE_STATIC_ASSERT < 200410L\

#define	cnLibrary_CPPFEATURELEVEL	2

#else

#define	cnLibrary_CPPFEATURELEVEL	99

#endif


//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------

#ifndef cnLibrary_CPPEXCLUDE_ENUMCLASS

#define cnLib_INTENUM_BEGIN(_type_,_name_)		typedef _type_ e##_name_;	struct _name_{enum Enum_##_name_ : _type_
#define cnLib_INTENUM_END(_name_)				;}

#define cnLib_ENUM_BEGIN(_type_,_name_)			enum class _name_ : _type_
#define cnLib_ENUM_END(_name_)					;	typedef _name_ e##_name_

// !cnLibrary_CPPEXCLUDE_ENUMCLASS
#else
// cnLibrary_CPPEXCLUDE_ENUMCLASS

#define cnLib_INTENUM_BEGIN(_type_,_name_)		typedef _type_ e##_name_;	struct _name_{enum Enum_##_name_
#define cnLib_INTENUM_END(_name_)				;}

#define cnLib_ENUM_BEGIN(_type_,_name_)			typedef _type_ e##_name_;	struct _name_{enum Enum_##_name_
#define cnLib_ENUM_END(_name_)					;}

#endif // cnLibrary_CPPEXCLUDE_ENUMCLASS



#ifdef cnLibrary_CPPEXCLUDE_NOEXCEPT

#define	noexcept(...)

#endif	// cnLibrary_CPPEXCLUDE_NOEXCEPT

#if cnLibrary_CPPFEATURE_THREE_WAY_COMPARISION >= 201907L

#define	cnLib_DEFINE_CLASS_THREE_WAY_COMPARISON(_arg_type_,_comp_func_)\
	auto operator <=> (_arg_type_ CompareValue)const noexcept(noexcept(_comp_func_(CompareValue))) \
		-> decltype(_comp_func_(CompareValue)){	return _comp_func_(CompareValue);	}

// cnLibrary_CPPFEATURE_THREE_WAY_COMPARISION >= 201907L
#else
// cnLibrary_CPPFEATURE_THREE_WAY_COMPARISION < 201907L

#define	cnLib_DEFINE_CLASS_THREE_WAY_COMPARISON(_arg_type_,_comp_func_)\
	bool operator < (_arg_type_ CompareValue)const noexcept(noexcept(_comp_func_(CompareValue))){		return _comp_func_(CompareValue)<0;	}\
	bool operator > (_arg_type_ CompareValue)const noexcept(noexcept(_comp_func_(CompareValue))){		return _comp_func_(CompareValue)>0;	}\
	bool operator <= (_arg_type_ CompareValue)const noexcept(noexcept(_comp_func_(CompareValue))){	return _comp_func_(CompareValue)<=0;	}\
	bool operator >= (_arg_type_ CompareValue)const noexcept(noexcept(_comp_func_(CompareValue))){	return _comp_func_(CompareValue)>=0;	}\

#endif	// cnLibrary_CPPFEATURE_THREE_WAY_COMPARISION < 201907L
//---------------------------------------------------------------------------
namespace cnLib_THelper{
	using namespace cnLibrary;

//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L
struct TypeMissing;

#define	cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT\
	class _vt0_=cnLib_THelper::TypeMissing,\
	class _vt1_=cnLib_THelper::TypeMissing,\
	class _vt2_=cnLib_THelper::TypeMissing,\
	class _vt3_=cnLib_THelper::TypeMissing,\
	class _vt4_=cnLib_THelper::TypeMissing,\
	class _vt5_=cnLib_THelper::TypeMissing,\
	class _vt6_=cnLib_THelper::TypeMissing,\
	class _vt7_=cnLib_THelper::TypeMissing
#define	cnLib_VARIADIC_TEMPLATE_PARAMETER\
	class _vt0_,\
	class _vt1_,\
	class _vt2_,\
	class _vt3_,\
	class _vt4_,\
	class _vt5_,\
	class _vt6_,\
	class _vt7_
#define	cnLib_VARIADIC_TEMPLATE_ARGUMENT_SPECIALIZATION\
	cnLib_THelper::TypeMissing,\
	cnLib_THelper::TypeMissing,\
	cnLib_THelper::TypeMissing,\
	cnLib_THelper::TypeMissing,\
	cnLib_THelper::TypeMissing,\
	cnLib_THelper::TypeMissing,\
	cnLib_THelper::TypeMissing,\
	cnLib_THelper::TypeMissing
#define	cnLib_VARIADIC_TEMPLATE_EXPAND\
	_vt0_,\
	_vt1_,\
	_vt2_,\
	_vt3_,\
	_vt4_,\
	_vt5_,\
	_vt6_,\
	_vt7_
#define	cnLib_VARIADIC_TEMPLATE_EXPAND1	cnLib_VARIADIC_TEMPLATE_EXPAND,\
	cnLib_THelper::TypeMissing
#define	cnLib_VARIADIC_TEMPLATE_PARAMETER_COUNT	cnLibrary::cnVar::TIndexOf<cnLib_THelper::TypeMissing, cnLib_VARIADIC_TEMPLATE_EXPAND>::Value

#define	cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT_P1	cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT,\
	class _vt8_=cnLib_THelper::TypeMissing
#define	cnLib_VARIADIC_TEMPLATE_PARAMETER_P1	cnLib_VARIADIC_TEMPLATE_PARAMETER,\
	class _vt8_
#define	cnLib_VARIADIC_TEMPLATE_ARGUMENT_SPECIALIZATION_P1	cnLib_VARIADIC_TEMPLATE_ARGUMENT_SPECIALIZATION,\
	cnLib_THelper::TypeMissing
#define	cnLib_VARIADIC_TEMPLATE_EXPAND_P1	cnLib_VARIADIC_TEMPLATE_EXPAND,\
	_vt8_
#define	cnLib_VARIADIC_TEMPLATE_EXPAND1_P1	cnLib_VARIADIC_TEMPLATE_EXPAND_P1,\
	cnLib_THelper::TypeMissing

#define	cnLib_VARIADIC_TEMPLATE_PARAMETER_COUNT_P1	cnLibrary::cnVar::TIndexOf<cnLib_THelper::TypeMissing,cnLib_VARIADIC_TEMPLATE_EXPAND_P1>::Value


#define	cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT_P2	cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT_P1,\
	class _vt9_=cnLib_THelper::TypeMissing
#define	cnLib_VARIADIC_TEMPLATE_PARAMETER_P2	cnLib_VARIADIC_TEMPLATE_PARAMETER_P1,\
	class _vt9_
#define	cnLib_VARIADIC_TEMPLATE_ARGUMENT_SPECIALIZATION_P2	cnLib_VARIADIC_TEMPLATE_ARGUMENT_SPECIALIZATION_P1,\
	cnLib_THelper::TypeMissing
#define	cnLib_VARIADIC_TEMPLATE_EXPAND_P2	cnLib_VARIADIC_TEMPLATE_EXPAND_P1,\
	_vt9_
#define	cnLib_VARIADIC_TEMPLATE_EXPAND1_P2	cnLib_VARIADIC_TEMPLATE_EXPAND_P2,\
	cnLib_THelper::TypeMissing

#define	cnLib_VARIADIC_TEMPLATE_PARAMETER_COUNT_P2	cnLibrary::cnVar::TIndexOf<cnLib_THelper::TypeMissing,cnLib_VARIADIC_TEMPLATE_EXPAND_P2>::Value

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnMemory{
cnLib_ENUM_BEGIN(ufInt8,ByteOrder){
	LittleEndian,
	BigEndian,
}cnLib_ENUM_END(ByteOrder);
//---------------------------------------------------------------------------
}	// namespace cnMemory
//---------------------------------------------------------------------------
namespace TKRuntime{
//---------------------------------------------------------------------------

struct NativeByteOrder;
//{
//	static constexpr eByteOrder Value;
//};

struct Debug
{
	static void AssertionMessage(const char *Message);
};

//---------------------------------------------------------------------------
}	// namespace TKRuntime
//---------------------------------------------------------------------------
#ifdef	cnLib_DEBUG
#define	cnLib_ASSERT(_e_)		(void)( static_cast<bool>(_e_) || (cnLibrary::TKRuntime::Debug::AssertionMessage("Assertion failed at : \r\n " cnLib_FILE_LINE "\r\n"  #_e_ "\r\n"),true) )
#define	cnLib_UNEXPECTED_BRANCH	cnLibrary::TKRuntime::Debug::AssertionMessage("Assertion failed at : \r\n " cnLib_FILE_LINE "\r\nUnexpected code branch\r\n")
#else
#define	cnLib_ASSERT(_e_)		cnLib_ASSUME(_e_)
#define	cnLib_UNEXPECTED_BRANCH
#endif

static cnLib_CONSTVAR uIntn IndexNotFound=~static_cast<uIntn>(0);

//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

// cast return value as it is

template<class TRet,class T>
inline TRet ReturnCast(T&& Var)noexcept(true)
{
	return reinterpret_cast<TRet&&>(Var);
}

// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
#else
// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L

template<class TRet,class T>
inline const TRet& ReturnCast(const T& Var)noexcept(true)
{
	return reinterpret_cast<const TRet&>(Var);
}

#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L

template<bool v>
struct TConstantValueBool
{
	static cnLib_CONSTVAR bool Value=v;
};

typedef TConstantValueBool<true>	TConstantValueTrue;
typedef TConstantValueBool<false>	TConstantValueFalse;

template<uIntn v>
struct TConstantValueUIntn
{
	static cnLib_CONSTVAR uIntn Value=v;
};

//---------------------------------------------------------------------------

#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L

template<class T,T v>
struct TConstantValue
{
	static constexpr T Value=v;
};
template<class T,T v>
constexpr T TConstantValue<T,v>::Value;

// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
template<class T,T v>
struct TConstantValue
{
	static const T Value;
};
template<class T,T v>
const T TConstantValue<T,v>::Value=v;

#endif	// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class T,T...Elements>
struct TConstantArray
{
	static const T Value[sizeof...(Elements)];
};

template<class T,T...Elements>
const T TConstantArray<T,Elements...>::Value[sizeof...(Elements)]={Elements...};


template<class T,T...Elements>
struct TConstantString
{
	static const T Value[];
};
template<class T,T...Elements>
const T TConstantString<T,Elements...>::Value[]={
	Elements...,0
};


#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L


template<class T>	struct TTypeDef{	typedef T Type;	};


template<class T,int Condition>	struct TTypeConditional			: TTypeDef<T>{};
template<class T>				struct TTypeConditional<T,false>{};


template<class T,class TCheckDefined=T>
struct TTypeRequireDefined
	: cnVar::TTypeConditional<T,sizeof(TCheckDefined)>{};

template<class T> struct TTypeRequireDefined<T,void>				{	typedef T Type;	};
template<class T> struct TTypeRequireDefined<T,const void>			{	typedef T Type;	};
template<class T> struct TTypeRequireDefined<T,volatile void>		{	typedef T Type;	};
template<class T> struct TTypeRequireDefined<T,const volatile void>	{	typedef T Type;	};


#if cnLibrary_CPPFEATURE_ALIAS_TEMPLATES >= 200704L

// TypeDef
//	type that follows ths type of T
template<class T>
using TypeDef=typename TTypeDef<T>::Type;

#endif // cnLibrary_CPPFEATURE_ALIAS_TEMPLATES >= 200704L

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<uIntn Index,class...T>				struct TSelect{};
template<class T,class...TR>				struct TSelect<0,T,TR...>{    typedef T Type;	};
template<uIntn Index,class T,class...TR>	struct TSelect<Index,T,TR...> : TSelect<Index-1,TR...>{};

#if cnLibrary_CPPFEATURE_ALIAS_TEMPLATES >= 200704L

// Select
//	select type from variadic list
template<uIntn Index,class...T>
using Select=typename TSelect<Index,T...>::Type;

#endif // cnLibrary_CPPFEATURE_ALIAS_TEMPLATES >= 200704L


// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<uIntn Index,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT_P1>
struct TSelect;

template<>
struct TSelect<0,cnLib_VARIADIC_TEMPLATE_ARGUMENT_SPECIALIZATION_P1>{};

template<class T,cnLib_VARIADIC_TEMPLATE_PARAMETER>
struct TSelect<0,T,cnLib_VARIADIC_TEMPLATE_EXPAND>{    typedef T Type;	};

template<uIntn Index,class T,cnLib_VARIADIC_TEMPLATE_PARAMETER>
struct TSelect : TSelect<Index-1,cnLib_VARIADIC_TEMPLATE_EXPAND1>{};

#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L



//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------
template<class TEnable,class T>	struct HasTypeDef : cnVar::TConstantValueFalse{};
template<class T>
struct HasTypeDef<typename cnVar::TSelect<0,void,typename T::Type>::Type,T>
	: cnVar::TConstantValueTrue{};
//---------------------------------------------------------------------------
template<class TEnable,class T>	struct HasValueDef : cnVar::TConstantValueFalse{};
template<class T>
struct HasValueDef<typename cnVar::TTypeConditional<void,sizeof(T::Value)>::Type,T>
	: cnVar::TConstantValueTrue{};
//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

template<class T>	struct THasTypeDef		: TConstantValueBool<cnLib_THelper::Var_TH::HasTypeDef<void,T>::Value>{};
template<class T>	struct THasValueDef		: TConstantValueBool<cnLib_THelper::Var_TH::HasValueDef<void,T>::Value>{};

//---------------------------------------------------------------------------
template<class T>	struct TStoreSizeOf							: TConstantValueUIntn<sizeof(T)>{};
template<>			struct TStoreSizeOf<void>					: TConstantValueUIntn<0>{};
template<>			struct TStoreSizeOf<const void>				: TConstantValueUIntn<0>{};
template<>			struct TStoreSizeOf<volatile void>			: TConstantValueUIntn<0>{};
template<>			struct TStoreSizeOf<const volatile void>	: TConstantValueUIntn<0>{};
template<class T>	struct TStoreSizeOf<T []>					: TConstantValueUIntn<0>{};
template<class T>	struct TStoreSizeOf<T&>						: TConstantValueUIntn<sizeof(void*)>{};
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
template<class T>	struct TStoreSizeOf<T&&>					: TConstantValueUIntn<sizeof(void*)>{};
#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


//---------------------------------------------------------------------------

template<class T>	struct TIsCPPType : cnVar::TConstantValueFalse{};

template<>	struct TIsCPPType<long double>			: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<double>				: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<float>				: cnVar::TConstantValueTrue{};

template<>	struct TIsCPPType<unsigned long long>	: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<signed long long>		: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<unsigned long>		: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<signed long>			: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<unsigned int>			: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<signed int>			: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<unsigned short>		: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<signed short>			: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<unsigned char>		: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<signed char>			: cnVar::TConstantValueTrue{};

template<>	struct TIsCPPType<wchar_t>	: cnVar::TConstantValueTrue{};

#if cnLibrary_CPPFEATURE_UCHARS >= 200704L

template<>	struct TIsCPPType<char16_t>	: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<char32_t>	: cnVar::TConstantValueTrue{};

#endif // cnLibrary_CPPFEATURE_UCHARS >= 200704L

#if cnLibrary_CPPFEATURE_UCHAR8 >= 201811L

template<>	struct TIsCPPType<char8_t>	: cnVar::TConstantValueTrue{};

#endif // cnLibrary_CPPFEATURE_UCHAR8 >= 201811L


template<class T>
struct TIsSigned
	: TConstantValueBool< (static_cast<T>(-1)<0) >{};
template<>
struct TIsSigned<bool>
	: TConstantValueBool< false >{};


//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_STATIC_ASSERT >= 200410L

static_assert(sizeof(uIntn)==sizeof(void*),	"incorrect size of uIntn");
static_assert(sizeof(sIntn)==sizeof(void*),	"incorrect size of sIntn");

static_assert(sizeof(uInt64)*ByteBitCount/8==8,"incorrect size of uInt64");
static_assert(sizeof(sInt64)*ByteBitCount/8==8,"incorrect size of sInt64");
static_assert(sizeof(uInt32)*ByteBitCount/8==4,"incorrect size of uInt32");
static_assert(sizeof(sInt32)*ByteBitCount/8==4,"incorrect size of sInt32");
static_assert(sizeof(uInt16)*ByteBitCount/8==2,"incorrect size of uInt16");
static_assert(sizeof(sInt16)*ByteBitCount/8==2,"incorrect size of sInt16");
static_assert(sizeof(uInt8)*ByteBitCount/8==1,	"incorrect size of uInt8");
static_assert(sizeof(sInt8)*ByteBitCount/8==1,	"incorrect size of sInt8");

#endif // cnLibrary_CPPFEATURE_STATIC_ASSERT >= 200410L
//---------------------------------------------------------------------------

template<uIntn Size,bool Signed>	struct TIntegerOfSize{};

template<bool Signed>	struct TIntegerOfSize<1,Signed>		: TTypeDef<sInt8>{};
template<bool Signed>	struct TIntegerOfSize<2,Signed>		: TTypeDef<sInt16>{};
template<bool Signed>	struct TIntegerOfSize<4,Signed>		: TTypeDef<sInt32>{};
template<bool Signed>	struct TIntegerOfSize<8,Signed>		: TTypeDef<sInt64>{};
template<bool Signed>	struct TIntegerOfSize<16,Signed>	: TTypeDef<sInt128>{};

template<>	struct TIntegerOfSize<1,false>					: TTypeDef<uInt8>{};
template<>	struct TIntegerOfSize<2,false>					: TTypeDef<uInt16>{};
template<>	struct TIntegerOfSize<4,false>					: TTypeDef<uInt32>{};
template<>	struct TIntegerOfSize<8,false>					: TTypeDef<uInt64>{};
template<>	struct TIntegerOfSize<16,false>					: TTypeDef<uInt128>{};


template<uIntn Size,bool Signed>	struct TFastIntegerOfSize{};

template<bool Signed>	struct TFastIntegerOfSize<1,Signed>		: TTypeDef<sfInt8>{};
template<bool Signed>	struct TFastIntegerOfSize<2,Signed>		: TTypeDef<sfInt16>{};
template<bool Signed>	struct TFastIntegerOfSize<4,Signed>		: TTypeDef<sfInt32>{};
template<bool Signed>	struct TFastIntegerOfSize<8,Signed>		: TTypeDef<sfInt64>{};
template<bool Signed>	struct TFastIntegerOfSize<16,Signed>	: TTypeDef<sfInt128>{};

template<>	struct TFastIntegerOfSize<1,false>					: TTypeDef<ufInt8>{};
template<>	struct TFastIntegerOfSize<2,false>					: TTypeDef<ufInt16>{};
template<>	struct TFastIntegerOfSize<4,false>					: TTypeDef<ufInt32>{};
template<>	struct TFastIntegerOfSize<8,false>					: TTypeDef<ufInt64>{};
template<>	struct TFastIntegerOfSize<16,false>					: TTypeDef<ufInt128>{};

// TODO : correct value
static cnLib_CONSTVAR uIntn MaxIntegerSize=8;


#if cnLibrary_CPPFEATURE_ALIAS_TEMPLATES >= 200704L

// IntegerOfSize
//	integer type by size
template<uIntn Size,bool Sign>
using IntegerOfSize=typename TIntegerOfSize<Size,Sign>::Type;

// UIntegerOfSize
//	unsigned integer type by size
template<uIntn Size>
using UIntegerOfSize=typename TIntegerOfSize<Size,false>::Type;
// SIntegerOfSize
//	signed integer type by size
template<uIntn Size>
using SIntegerOfSize=typename TIntegerOfSize<Size,true>::Type;


// OfSize
//	fast integer type by size
template<uIntn Size,bool Sign>
using FastIntegerOfSize=typename TFastIntegerOfSize<Size,Sign>::Type;

// UFastOfSize
//	fast unsigned integer type by size
template<uIntn Size>
using UFastIntegerOfSize=typename TFastIntegerOfSize<Size,false>::Type;
// SFastOfSize
//	fast signed integer type by size
template<uIntn Size>
using SFastIntegerOfSize=typename TFastIntegerOfSize<Size,true>::Type;

#endif // cnLibrary_CPPFEATURE_ALIAS_TEMPLATES >= 200704L

template<uIntn Size>	struct TFloatOfSize{};

template<>	struct TFloatOfSize<4>					: TTypeDef<Float32>{};
template<>	struct TFloatOfSize<8>					: TTypeDef<Float64>{};
template<>	struct TFloatOfSize<16>					: TTypeDef<Float128>{};

#if	cnLibrary_CPPFEATURE_ALIAS_TEMPLATES >= 200704L

// OfSize
//	float type by size
template<uIntn Size>
using FloatOfSize=typename TFloatOfSize<Size>::Type;

#endif // cnLibrary_CPPFEATURE_ALIAS_TEMPLATES >= 200704L

template<uIntn Size>	struct TUCharOfSize{};

template<>	struct TUCharOfSize<1>					: TTypeDef<uChar8>{};
template<>	struct TUCharOfSize<2>					: TTypeDef<uChar16>{};
template<>	struct TUCharOfSize<4>					: TTypeDef<uChar32>{};

#if	cnLibrary_CPPFEATURE_ALIAS_TEMPLATES >= 200704L

// UCharOfSize
//	character type by size
template<uIntn Size>
using UCharOfSize=typename TUCharOfSize<Size>::Type;

#endif // cnLibrary_CPPFEATURE_ALIAS_TEMPLATES >= 200704L

template<uIntn Size,bool Sign>
struct TIntegerTypeExists
	: THasTypeDef< TFastIntegerOfSize<Size,Sign> >
{};

template<uIntn Size>
struct TFloatTypeExists
	: THasTypeDef< TFloatOfSize<Size> >
{};

template<uIntn Size>
struct TUCharExists
	: THasTypeDef< TUCharOfSize<Size> >
{};

#if cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L


template<class T>
static cnLib_CONSTVAR bool IsSigned=TIsSigned<T>::Value;

template<uIntn Size,bool Signed>
static cnLib_CONSTVAR bool IntegerTypeExists=TIntegerTypeExists<Size,Signed>::Value;

template<uIntn Size>
static cnLib_CONSTVAR bool FloatTypeExists=TFloatTypeExists<Size>::Value;

template<uIntn Size>
static cnLib_CONSTVAR bool UCharExists=TUCharExists<Size>::Value;

#endif // cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L


//---------------------------------------------------------------------------
template<class T> static inline T DeclVal()noexcept(true);
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
namespace cnClass{
//---------------------------------------------------------------------------
struct cNoInitialization{};
static cnLib_CONSTVAR cNoInitialization *NoInitialization=0;
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
inline void* operator new (cnLibrary::tSize,cnLibrary::cnClass::cNoInitialization *Pointer)noexcept(true){	return Pointer;}
inline void operator delete(void *,cnLibrary::cnClass::cNoInitialization*)noexcept(true){}
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------
template<class TValueDecl,class TNextDecl,bool NextZero>
struct IntegerValueMSBCalc;

template<class T,T v,T vn>
struct IntegerValueMSBCalc<cnVar::TConstantValue<T,v>,cnVar::TConstantValue<T,vn>,false>
	: IntegerValueMSBCalc<cnVar::TConstantValue<T,vn>,cnVar::TConstantValue<T,vn&(vn-1)>,(vn&(vn-1))==0>
{};

template<class T,T v,T vn,bool NextZero>
struct IntegerValueMSBCalc< cnVar::TConstantValue<T,v>,cnVar::TConstantValue<T,vn>,NextZero >
	: cnVar::TConstantValue<T,v>
{
};

//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{

template<class T,T v>
struct TIntegerValueLSB
	: cnVar::TConstantValue<T,v&~(v-1)>
{
};

template<class T,T v>
struct TIntegerValueMSB
	: cnLib_THelper::Var_TH::IntegerValueMSBCalc<cnVar::TConstantValue<T,v>,cnVar::TConstantValue<T,v&(v-1)>,(v&(v-1))==0>
{
};

//---------------------------------------------------------------------------

#if cnLibrary_CPPFEATURE_THREE_WAY_COMPARISION >= 201907L && cnLibrary_CPPFEATURE_DECLTYPE >=200707L

template<class TCompare1,class TCompare2=TCompare1>
struct TDefaultCompareResult
	: TTypeDef<decltype(cnVar::DeclVal<TCompare1>()<=>cnVar::DeclVal<TCompare2>())>
{
};

template<class TCompare1,class TCompare2>
inline auto DefaultCompare(const TCompare1 &Value1,const TCompare2 &Value2)
noexcept(noexcept(Value1<=>Value2))
-> decltype(Value1<=>Value2)
{
	return Value1<=>Value2;
}

#else	// cnLibrary_CPPFEATURE_THREE_WAY_COMPARISION < 201907L || cnLibrary_CPPFEATURE_DECLTYPE < 200707L

template<class TCompare1,class TCompare2=TCompare1>
struct TDefaultCompareResult
	: TTypeDef<sfInt8>
{
};

template<class TCompare1,class TCompare2>
inline sfInt8 DefaultCompare(const TCompare1 &Value1,const TCompare2 &Value2)
noexcept(noexcept(Value1==Value2) && noexcept(Value1<Value2))
{
	sfInt8 ne=static_cast<sfInt8>(!(Value1==Value2));
	sfInt8 lt=-static_cast<sfInt8>(Value1<Value2);
	return ne|lt;
}
#endif	// cnLibrary_CPPFEATURE_THREE_WAY_COMPARISION , cnLibrary_CPPFEATURE_DECLTYPE

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
namespace TKRuntime{
//---------------------------------------------------------------------------

template<uIntn IntegerSize>
struct TInteger;
//{
//	static T ReverseBytes(T Value)noexcept;
//	static sfInt8 Compare(T Src1,T Src2)noexcept;
//	static tUInt ShiftLeftInto(tUInt ValueHigh,tUInt ValueLow,ufInt8 Count)noexcept;
//	static tUInt ShiftRightInto(tUInt ValueLow,tUInt ValueHigh,ufInt8 Count)noexcept;
//	static bool BitScanL(ufInt8 &BitIndex,tUInt Src)noexcept;
//	static bool BitScanH(ufInt8 &BitIndex,tUInt Src)noexcept;
//	static bool BitTestAt(tUInt Src,ufInt8 BitIndex)noexcept;
//	static bool BitSetAt(tUInt &Src,ufInt8 BitIndex)noexcept;
//	static bool BitResetAt(tUInt &Src,ufInt8 BitIndex)noexcept;
//	static bool BitComplementAt(tUInt &Src,ufInt8 BitIndex)noexcept;
//	static tUInt RotateLeft(tUInt Src,ufInt8 Count)noexcept;
//	static tUInt RotateRight(tUInt Src,ufInt8 Count)noexcept;
//};

//---------------------------------------------------------------------------
}	// namespace TKRuntime
//---------------------------------------------------------------------------
namespace CPPRuntime{
//---------------------------------------------------------------------------

template<uIntn IntegerSize>
struct TInteger_BitScan
{
	typedef typename cnVar::TIntegerOfSize<IntegerSize,false>::Type tUInt;
	typedef typename cnVar::TIntegerOfSize<IntegerSize/2,false>::Type tUIntH;

	static void BitScanL(ufInt8 &BitIndex,tUInt Src)noexcept(true){
		if((Src&((static_cast<tUInt>(1)<<IntegerSize*ByteBitCount/2)-1))==0){
			BitIndex+=IntegerSize*ByteBitCount/2;
			Src>>=(IntegerSize*ByteBitCount/2);
		}
		return TInteger_BitScan<IntegerSize/2>::BitScanL(BitIndex,static_cast<tUIntH>(Src));
	}
	static void BitScanH(ufInt8 &BitIndex,tUInt Src)noexcept(true){
		if(Src&(static_cast<tUInt>(~static_cast<tUInt>(0))<<IntegerSize*ByteBitCount/2)){
			BitIndex+=IntegerSize*ByteBitCount/2;
			Src>>=(IntegerSize*ByteBitCount/2);
		}
		return TInteger_BitScan<IntegerSize/2>::BitScanH(BitIndex,static_cast<tUIntH>(Src));
	}
};
template<>
struct TInteger_BitScan<1>
{
	static void BitScanL(ufInt8 &BitIndex,uInt8 Src)noexcept(true){
		if((Src&0xF)==0){
			BitIndex+=4;
			Src>>=4;
		}
		if(Src&1){
			return;
		}
		if(Src&0x2){
			BitIndex+=1;
		}
		else if(Src&0x4){
			BitIndex+=2;
		}
		else if(Src&0x8){
			BitIndex+=3;
		}
	}
	static void BitScanH(ufInt8 &BitIndex,uInt8 Src)noexcept(true){
		if(Src&0xF0){
			BitIndex+=4;
			Src>>=4;
		}
		if(Src&0x8){
			BitIndex+=3;
		}
		else if(Src&0x4){
			BitIndex+=2;
		}
		else if(Src&0x2){
			BitIndex+=1;
		}
	}
};

template<uIntn IntegerSize>
struct TInteger
{
	typedef typename cnVar::TIntegerOfSize<IntegerSize,false>::Type tUInt;
	typedef typename cnVar::TIntegerOfSize<IntegerSize,true>::Type tSInt;

	//	static T ReverseBytes(T Value)noexcept;

	static sfInt8 Compare(tUInt Src1,tUInt Src2)noexcept(true){
		return cnVar::DefaultCompare(Src1,Src2);
	}

	static tUInt ShiftLeftInto(tUInt ValueHigh,tUInt ValueLow,ufInt8 Count)noexcept(true)
	{
		tUInt h=ValueHigh<<Count;
		tUInt l=ValueLow>>(IntegerSize*ByteBitCount-Count);
		return h|l;
	}
	static tUInt ShiftRightInto(tUInt ValueLow,tUInt ValueHigh,ufInt8 Count)noexcept(true)
	{
		tUInt l=ValueLow>>Count;
		tUInt h=ValueHigh<<(IntegerSize*ByteBitCount-Count);
		return h|l;
	}

	static bool BitScanL(ufInt8 &BitIndex,tUInt Src)noexcept(true){
		if(Src==0){
			return false;
		}
		BitIndex=0;
		TInteger_BitScan<IntegerSize>::BitScanL(BitIndex,Src);
		return true;
	}
	static bool BitScanH(ufInt8 &BitIndex,tUInt Src)noexcept(true){
		if(Src==0){
			return false;
		}
		BitIndex=0;
		TInteger_BitScan<IntegerSize>::BitScanH(BitIndex,Src);
		return true;
	}


	static bool BitTestAt(tUInt Src,ufInt8 BitIndex)noexcept(true){
		return (Src&(static_cast<tUInt>(1)<<BitIndex))!=0;
	}
	static bool BitSetAt(tUInt &Dest,ufInt8 BitIndex)noexcept(true){
		tUInt Mask=static_cast<tUInt>(1)<<BitIndex;
		bool r=(Dest&Mask)!=0;
		if(!r){
			Dest|=Mask;
		}
		return r;
	}
	static bool BitResetAt(tUInt &Dest,ufInt8 BitIndex)noexcept(true){
		tUInt Mask=static_cast<tUInt>(1)<<BitIndex;
		bool r=(Dest&Mask)!=0;
		if(r){
			Dest&=~Mask;
		}
		return r;
	}
	static bool BitComplementAt(tUInt &Dest,ufInt8 BitIndex)noexcept(true){
		tUInt Mask=static_cast<tUInt>(1)<<BitIndex;
		bool r=(Dest&Mask)!=0;
		Dest^=Mask;
		return r;
	}

	static tUInt RotateLeft(tUInt Src,ufInt8 Count)noexcept(true){
		tUInt h=Src<<Count;
		tUInt l=Src>>(IntegerSize*ByteBitCount-Count);
		return h|l;
	}
	static tUInt RotateRight(tUInt Src,ufInt8 Count)noexcept(true){
		tUInt l=Src>>Count;
		tUInt h=Src<(IntegerSize*ByteBitCount-Count);
		return h|l;
	}

};

//---------------------------------------------------------------------------
}	// namespace CPPRuntime
//---------------------------------------------------------------------------
namespace cnMemory{
//---------------------------------------------------------------------------


template<eByteOrder TargetOrder,eByteOrder ValueOrder>
struct TByteOrderConvert;

template<cnMemory::eByteOrder Order>
struct TByteOrderConvert<Order,Order>
{
	template<class T>
	static T Convert(const T &Data)noexcept(true){
		return Data;
	}
};

template<>
struct TByteOrderConvert<cnMemory::ByteOrder::LittleEndian,cnMemory::ByteOrder::BigEndian>
{	// BigEndian -> LittleEndian
	template<class T>
	static T Convert(const T &Data)noexcept(true){
		typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tInt;
		return cnVar::ReturnCast<T>(
			TKRuntime::TInteger<sizeof(T)>::ReverseBytes(reinterpret_cast<const tInt&>(Data))
		);
	}
};

template<>
struct TByteOrderConvert<cnMemory::ByteOrder::BigEndian,cnMemory::ByteOrder::LittleEndian>
{	// LittleEndian -> BigEndian
	template<class T>
	static T Convert(const T &Data)noexcept(true){
		typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tInt;
		return cnVar::ReturnCast<T>(
			TKRuntime::TInteger<sizeof(T)>::ReverseBytes(reinterpret_cast<const tInt&>(Data))
		);
	}
};


// SwapByteOrder
//	swap byte order between native and specified
// [in]Value		bytes of data to swap
// return converted data
template<eByteOrder Order,class T>
inline T SwapByteOrder(T Value)noexcept(true)
{
	return TByteOrderConvert<cnVar::TSelect<1,T,TKRuntime::NativeByteOrder>::Type::Value,Order>::Convert(Value);
}

// SwapLittleEndian
//	swap byte order between native and little endian
// [in]Value		little endian data
// return converted data
template<class T>
inline T SwapLittleEndian(T Value)noexcept(true)
{
	return TByteOrderConvert<cnVar::TSelect<1,T,TKRuntime::NativeByteOrder>::Type::Value,ByteOrder::LittleEndian>::Convert(Value);
}

// SwapBigEndian
//	swap byte order between native and big endian
// [in]Value		big endian data
// return converted data
template<class T>
inline T SwapBigEndian(T Value)noexcept(true)
{
	return TByteOrderConvert<cnVar::TSelect<1,T,TKRuntime::NativeByteOrder>::Type::Value,ByteOrder::BigEndian>::Convert(Value);
}


// UnalignedRead
//	read from unaligned memory
// Pointer	[in]	pointer to memory to read
// return: content of pointer
template<class T>
inline T UnalignedRead(const T *Pointer)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tInt;
	return cnVar::ReturnCast<T>(
		TKRuntime::TInteger<sizeof(T)>::UnalignedRead(reinterpret_cast<const tInt*>(Pointer))
	);
}

// UnalignedWrite
//	write to unaligned memory
// Pointer	[out]	pointer to memory to write
// Value			value to write
template<class T>
inline void UnalignedWrite(T *Pointer,const typename cnVar::TTypeDef<T>::Type &Value)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tInt;
	return TKRuntime::TInteger<sizeof(T)>::UnalignedWrite(reinterpret_cast<tInt*>(Pointer),reinterpret_cast<tInt&>(Value));
}



// ShiftLeftInto
//	Shift High while moving bits from Low
// [in]High		uIntn to shift
// [in]Low		bits to shift into Dest
// [in]Count	counts to shift, must smaller than bit count of uIntn
// return: High<<Count<<Low
template<class T>
inline T ShiftLeftInto(typename cnVar::TTypeDef<T>::Type High,T Low,ufInt8 Count)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;
	return static_cast<T>(TKRuntime::TInteger<sizeof(T)>::ShiftLeftInto(
		static_cast<tUInt>(High),
		static_cast<tUInt>(Low),
		static_cast<uInt8>(Count)
		));
}
// ShiftRightInto
//	Shift Low while moving bits from High
// [in]Low		uIntn to shift
// [in]High		bits to shift into Dest
// [in]Count	counts to shift, must smaller than bit count of uIntn
// return: Low>>Count>>High
template<class T>
inline T ShiftRightInto(T Low,typename cnVar::TTypeDef<T>::Type High,uIntn Count)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;
	return static_cast<T>(TKRuntime::TInteger<sizeof(T)>::ShiftRightInto(
		static_cast<tUInt>(Low),
		static_cast<tUInt>(High),
		static_cast<uInt8>(Count)
		));
}


// BitScanL
//	find least significant set bit
// BitIndex	[out]	bit index of least significant set bit
// Src				integer to scan
// return: false if Src is 0
template<class T>
inline bool BitScanL(ufInt8 &BitIndex,const T &Src)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;
	return static_cast<T>(TKRuntime::TInteger<sizeof(T)>::BitScanL(BitIndex,static_cast<tUInt>(Src)));
}

// BitScanH
//	find most significant set bit
// BitIndex	[out]	bit index of most significant set bit
// Src				uIntn to scan
// return: false if Src is 0
template<class T>
inline bool BitScanH(ufInt8 &BitIndex,const T &Src)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;
	return static_cast<T>(TKRuntime::TInteger<sizeof(T)>::BitScanH(BitIndex,static_cast<tUInt>(Src)));
}


// BitTest
//	test if bit is set at Index
// Src		bits to test
// Index	index of bit to test
// return bit
template<class T>
inline bool BitTestAt(const T &Src,ufInt8 BitIndex)noexcept(true)
{
	return TKRuntime::TInteger<sizeof(T)>::BitTestAt(static_cast<typename cnVar::TIntegerOfSize<sizeof(T),false>::Type>(Src),BitIndex);
}

// BitSetAt
//	set bit at Index
// Dest	[in]	bits to test
// Dest	[out]	bit at Index is set
// Index		index of bit to set
// return previous bit value
template<class T>
inline bool BitSetAt(T &Dest,ufInt8 BitIndex)noexcept(true)
{
	return TKRuntime::TInteger<sizeof(T)>::BitSetAt(reinterpret_cast<typename cnVar::TIntegerOfSize<sizeof(T),false>::Type&>(Dest),BitIndex);
}

// BitResetAt
//	reset bit at Index
// Dest	[in]	bits to test
// Dest	[out]	bit at Index is reset
// Index		index of bit to reset
// return previous bit value
template<class T>
inline bool BitResetAt(T &Dest,ufInt8 BitIndex)noexcept(true)
{
	return TKRuntime::TInteger<sizeof(T)>::BitResetAt(reinterpret_cast<typename cnVar::TIntegerOfSize<sizeof(T),false>::Type&>(Dest),BitIndex);
}

// BitComplementAt
//	toggle bit at Index
// Dest	[in]	bits to test
// Dest	[out]	bit at Index is reset
// Index		index of bit to reset
// return previous bit value
template<class T>
inline bool BitComplementAt(T &Dest,ufInt8 BitIndex)noexcept(true)
{
	return TKRuntime::TInteger<sizeof(T)>::BitComplementAt(reinterpret_cast<typename cnVar::TIntegerOfSize<sizeof(T),false>::Type&>(Dest),BitIndex);
}


// BitRotateLeft
//	Rotate bits of Dest to left
// Dest		uIntn to shift
// Count	counts to shift, must smaller than bit count of UInt
// return rotated result
template<class T>
inline T RotateLeft(const T &Dest,ufInt8 Count)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;
	return static_cast<T>(TKRuntime::TInteger<sizeof(T)>::RotateLeft(static_cast<tUInt>(Dest),Count));
}

// RotateRight
//	Rotate bits of Dest to right
// Dest		uIntn to shift
// Count	counts to shift, must smaller than bit count of UInt
// return rotated result
template<class T>
inline T RotateRight(const T &Dest,uIntn Count)noexcept(true)
{
	typedef typename cnVar::TIntegerOfSize<sizeof(T),false>::Type tUInt;
	return static_cast<T>(TKRuntime::TInteger<sizeof(T)>::RotateRight(static_cast<tUInt>(Dest),Count));
}

//---------------------------------------------------------------------------
}	// namespace cnMemory
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Memory_TH{
//---------------------------------------------------------------------------


template<tSize BlockSize,tSize MSB>
struct TPlainDataOrderOpeatorSmallTail
{
	static bool Equal(const void *p1,const void *p2)noexcept(true){
		typedef typename cnVar::TIntegerOfSize<MSB,false>::Type tInt;
		tInt v1=*static_cast<const tInt*>(p1);
		tInt v2=*static_cast<const tInt*>(p2);
		if(v1!=v2)
			return false;
		return TPlainDataOrderOpeatorSmallTail<BlockSize-MSB,cnVar::TIntegerValueMSB<tSize,BlockSize-MSB>::Value>::Equal(
			static_cast<const tInt*>(p1)+1,static_cast<const tInt*>(p2)+1);
	}
	static sfInt8 Compare(const void *p1,const void *p2)noexcept(true){
		typedef typename cnVar::TIntegerOfSize<MSB,false>::Type tInt;
		tInt v1=*static_cast<const tInt*>(p1);
		tInt v2=*static_cast<const tInt*>(p2);
		if(v1!=v2){
			v1=SwapBigEndian(v1);
			v2=SwapBigEndian(v2);
			sfInt8 lt=-static_cast<sfInt8>(v1<v2);
			return lt|1;
		}
		return TPlainDataOrderOpeatorSmallTail<BlockSize-MSB,cnVar::TIntegerValueMSB<tSize,BlockSize-MSB>::Value>::Compare(
			static_cast<const tInt*>(p1)+1,static_cast<const tInt*>(p2)+1);
	}
};

template<tSize IntSize>
struct TPlainDataOrderOpeatorSmallTail<IntSize,IntSize>
{
	static bool Equal(const void *p1,const void *p2)noexcept(true){
		typedef typename cnVar::TIntegerOfSize<IntSize,false>::Type tInt;
		tInt v1=*static_cast<const tInt*>(p1);
		tInt v2=*static_cast<const tInt*>(p2);
		return v1==v2;
	}
	static sfInt8 Compare(const void *p1,const void *p2)noexcept(true){
		typedef typename cnVar::TIntegerOfSize<IntSize,false>::Type tInt;
		tInt v1=*static_cast<const tInt*>(p1);
		tInt v2=*static_cast<const tInt*>(p2);
		if(v1==v2){
			return 0;
		}
		v1=SwapBigEndian(v1);
		v2=SwapBigEndian(v2);
		sfInt8 lt=-static_cast<sfInt8>(v1<v2);
		return lt|1;
	}
};
template<>
struct TPlainDataOrderOpeatorSmallTail<0,0>
{
	static bool Equal(const void*,const void*)noexcept(true){
		return true;
	}
	static sfInt8 Compare(const void*,const void*)noexcept(true){
		return 0;
	}
};

//---------------------------------------------------------------------------
}	// namespace Memory_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnMemory{
//---------------------------------------------------------------------------


template<tSize BlockSize>
struct TPlainDataOrderOpeator
{
	static bool Equal(const void *p1,const void *p2)noexcept(true){
		const uIntn IntLength=BlockSize/sizeof(uIntn);
		const uIntn *a1=static_cast<const uIntn*>(p1);
		const uIntn *a2=static_cast<const uIntn*>(p2);
		for(uIntn i=0;i<IntLength;i++){
			if(*a1++!=*a2++){
				return false;
			}
		}
		return cnLib_THelper::Memory_TH::TPlainDataOrderOpeatorSmallTail<BlockSize%sizeof(uIntn),cnVar::TIntegerValueMSB<tSize,BlockSize%sizeof(uIntn)>::Value>::Equal(a1,a2);
	}
	static sfInt8 Compare(const void *p1,const void *p2)noexcept(true){
		const uIntn IntLength=BlockSize/sizeof(uIntn);
		const uIntn *a1=static_cast<const uIntn*>(p1);
		const uIntn *a2=static_cast<const uIntn*>(p2);
		for(uIntn i=0;i<IntLength;i++){
			uIntn v1=*a1++;
			uIntn v2=*a2++;
			if(v1!=v2){
				v1=SwapBigEndian(v1);
				v2=SwapBigEndian(v2);
				sfInt8 lt=-static_cast<sfInt8>(v1<v2);
				return lt|1;
			}
		}
		return cnLib_THelper::Memory_TH::TPlainDataOrderOpeatorSmallTail<BlockSize%sizeof(uIntn),cnVar::TIntegerValueMSB<tSize,BlockSize%sizeof(uIntn)>::Value>::Compare(a1,a2);
	}
};


template<uIntn Size>
struct cPlainData
{
	uInt8 RAW[Size];

	sfInt8 Compare(const cPlainData &Dest)const noexcept(true){	return TPlainDataOrderOpeator<Size>::Compare(RAW,Dest.RAW);	}

	bool operator ==(const cPlainData &Dest)const noexcept(true){	return TPlainDataOrderOpeator<Size>::Equal(RAW,Dest.RAW);	}
	bool operator !=(const cPlainData &Dest)const noexcept(true){	return !TPlainDataOrderOpeator<Size>::Equal(RAW,Dest.RAW);	}

	cnLib_DEFINE_CLASS_THREE_WAY_COMPARISON(const cPlainData &,Compare)
};

template<>
struct cPlainData<0>;

// ArrayLength
//	get length of auto storage array
template<class T,uIntn Length>
cnLib_CONSTEXPR_FUNC uIntn ArrayLength(T (&)[Length])noexcept(true){
	return Length;
}

template<tSize Size,class TDest,class TSrc>
inline void TrivialCopy(TDest *Dest,const TSrc *Src)noexcept(true)
{
	*reinterpret_cast<cPlainData<Size>*>(Dest)=*reinterpret_cast<const cPlainData<Size>*>(Src);
}
template<tSize Size,class TDest>
inline void TrivialZeroFill(TDest *Dest)noexcept(true)
{
	*reinterpret_cast<cPlainData<Size>*>(Dest)=cPlainData<Size>();
}
//---------------------------------------------------------------------------
}	// namespace cnMemory
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class T,class TCompare,class...VT>	struct TIsSame					: TConstantValueFalse{};
template<class T>							struct TIsSame<T,T>				: TConstantValueTrue{};
template<class T,class...TCompare>			struct TIsSame<T,T,TCompare...>	: TIsSame<T,TCompare...>{};

template<class T,class TCompare,class...VT>	struct TIsDifferent						: TConstantValueTrue{};
template<class T>							struct TIsDifferent<T,T>				: TConstantValueFalse{};
template<class T,class...TCompare>			struct TIsDifferent<T,T,TCompare...>	: TIsDifferent<T,TCompare...>{};

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class T,class TCompare,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TIsSame
	: TConstantValueFalse{};
template<class T>
struct TIsSame<T,T,cnLib_VARIADIC_TEMPLATE_ARGUMENT_SPECIALIZATION>
	: TConstantValueTrue{};
template<class T,cnLib_VARIADIC_TEMPLATE_PARAMETER>
struct TIsSame<T,T,cnLib_VARIADIC_TEMPLATE_EXPAND>
	: TIsSame<T,cnLib_VARIADIC_TEMPLATE_EXPAND>{};

template<class T,class TCompare,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TIsDifferent
	: TConstantValueTrue{};
template<class T>
struct TIsDifferent<T,T,cnLib_VARIADIC_TEMPLATE_ARGUMENT_SPECIALIZATION>
	: TConstantValueFalse{};
template<class T,cnLib_VARIADIC_TEMPLATE_PARAMETER>
struct TIsDifferent<T,T,cnLib_VARIADIC_TEMPLATE_EXPAND>
	: TIsDifferent<T,cnLib_VARIADIC_TEMPLATE_EXPAND>{};

#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L


#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TFind,class...VT>	struct TIndexOf;

template<class TFind>							struct TIndexOf<TFind>					: TConstantValueUIntn<0>{};
template<class TFind,class...VT>				struct TIndexOf<TFind,TFind,VT...>		: TConstantValueUIntn<0>{};
template<class TFind,class TNoMatch,class...VT>	struct TIndexOf<TFind,TNoMatch,VT...>	: TConstantValueUIntn<1+TIndexOf<TFind,VT...>::Value>{};

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class TFind,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT_P2>
struct TIndexOf;

template<class TFind>
struct TIndexOf<TFind,cnLib_VARIADIC_TEMPLATE_ARGUMENT_SPECIALIZATION_P2>
	: TConstantValueUIntn<0>{};

template<class TFind,cnLib_VARIADIC_TEMPLATE_PARAMETER_P1>
struct TIndexOf<TFind,TFind,cnLib_VARIADIC_TEMPLATE_EXPAND_P1>
	: TConstantValueUIntn<0>{};

template<class TFind,class TNoMatch,cnLib_VARIADIC_TEMPLATE_PARAMETER_P1>
struct TIndexOf
	: TConstantValueUIntn<
		1+TIndexOf<TFind,cnLib_VARIADIC_TEMPLATE_EXPAND_P1>::Value
	>
{};

#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L && cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L
// IsSame
//	test if all types in variadic list are the same
template<class T1,class T2,class...VTCompare>
static cnLib_CONSTVAR bool IsSame=TIsSame<T1,T2,VTCompare...>::Value;

// IndexOf
//	find index of given type in type list
template<class TFind>
struct IndexOf
{
	template<class...T>
	static cnLib_CONSTVAR uIntn In=TIndexOf<TFind,T...>::Value;
};

#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L && cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

//---------------------------------------------------------------------------

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class...VT>
struct TTypePack
{
	template<uIntn TypeIndex>	struct tTypeByIndex : TSelect<TypeIndex,VT...>{};
	static cnLib_CONSTVAR uIntn Count=sizeof...(VT);

	template<class TExtend>	struct tExtend : TTypeDef< TTypePack<VT...,TExtend> >{};
	template<class TForType>	struct Declarator;
};

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TTypePack
{
	template<uIntn TypeIndex>	struct tTypeByIndex : TSelect<TypeIndex,cnLib_VARIADIC_TEMPLATE_EXPAND>{};
	static cnLib_CONSTVAR uIntn Count=cnLib_VARIADIC_TEMPLATE_PARAMETER_COUNT;

	template<class TForType>	struct Declarator;
};

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L


//---------------------------------------------------------------------------

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L


template<uIntn Index,class T,T...Values>			struct TSelectValue;

template<uIntn Index,class T>						struct TSelectValue<Index,T>{};
template<uIntn Index,class T,T Value,T...Values>	struct TSelectValue<Index,T,Value,Values...>	: TSelectValue<Index-1,T,Values...>{};
template<class T,T Value,T...Values>				struct TSelectValue<0,T,Value,Values...>		: TConstantValue<T,Value>{};


#if cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

// Select
//	select type from variadic list
template<uIntn Index,class T,T...Values>
static cnLib_CONSTVAR T SelectValue=TSelectValue<Index,T,Values...>::Value;

#endif	// cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

//---------------------------------------------------------------------------

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L


template<bool...Values>				struct TBooleanValuesAnd				: cnVar::TConstantValueFalse{};
template<bool v>					struct TBooleanValuesAnd<v>				: cnVar::TConstantValueBool<v>{};
template<bool v0,bool v1,bool...vv>	struct TBooleanValuesAnd<v0,v1,vv...>	: cnVar::TBooleanValuesAnd<v1,vv...>{};
template<bool v1,bool...vv>			struct TBooleanValuesAnd<false,v1,vv...>: cnVar::TConstantValueFalse{};

template<bool...Values>					struct TBooleanValuesOr					: cnVar::TConstantValueFalse{};
template<bool v>						struct TBooleanValuesOr<v>				: cnVar::TConstantValueBool<v>{};
template<bool v0,bool v1,bool...vv>		struct TBooleanValuesOr<v0,v1,vv...>	: cnVar::TConstantValueTrue{};
template<bool v1,bool...vv>				struct TBooleanValuesOr<false,v1,vv...>	: cnVar::TBooleanValuesOr<v1,vv...>{};


template<class TTypePack,uIntn Count,class TAppend>
struct TMakeTypePackRepeat
	: TMakeTypePackRepeat<typename TTypePack::template tExtend<TAppend>::Type,Count-1,TAppend>{};

template<class TValueSequence,class TAppend>
struct TMakeTypePackRepeat<TValueSequence,0,TAppend>
	: TTypeDef<TValueSequence>{};


template<class T,T...Values>
struct TValueSequence
{
	typedef T tValue;
	template<uIntn Index>	struct tAt : TSelectValue<Index,T,Values...>{};
	template<T Value>	struct tExtend : TTypeDef< TValueSequence<T,Values...,Value> >{};

	template<class TForType>	struct Declarator;
};


template<class TValueSequence,uIntn Count,typename TValueSequence::tValue Index=0,typename TValueSequence::tValue Increment=1>
struct TMakeIndexSequence
	: TMakeIndexSequence<typename TValueSequence::template tExtend<Index>::Type,Count-1,Index+Increment,Increment>{};

template<class TValueSequence,typename TValueSequence::tValue Index,typename TValueSequence::tValue Increment>
struct TMakeIndexSequence<TValueSequence,0,Index,Increment>
	: TTypeDef<TValueSequence>{};


template<class TValueSequence,typename TValueSequence::tValue...AddValues>
struct TMakeAccumulateSequence;

template<class TValueSequence,typename TValueSequence::tValue Value,typename TValueSequence::tValue...AddValues>
struct TMakeAccumulateSequence<TValueSequence,Value,AddValues...>
	: TMakeAccumulateSequence<typename TValueSequence::template tExtend<Value>::Type,AddValues...>{};

template<class TValueSequence>
struct TMakeAccumulateSequence<TValueSequence>
	: TTypeDef<TValueSequence>{};


template<class...TConstantArraies>
struct TMergeConstantArrayDef;

template<template<class T,T...> class TConstantClass,class T,T...Elements1,T...Elements2,class...TConstantArraies>
struct TMergeConstantArrayDef<TConstantClass<T,Elements1...>,TConstantClass<T,Elements2...>,TConstantArraies...>
	: TMergeConstantArrayDef<TConstantClass<T,Elements1...,Elements2...>,TConstantArraies...>
{
};

template<template<class T,T...> class TConstantClass,class T,T...Elements>
struct TMergeConstantArrayDef< TConstantClass<T,Elements...> >
	: TTypeDef< TConstantClass<T,Elements...> >{};

#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L


//---------------------------------------------------------------------------
}	// namespace cnVar

//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
template<class...T>	inline void UnusedParameter(T cnLib_UREF...)noexcept(true){}
#else // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L
template<class T>	inline void UnusedParameter(T cnLib_UREF)noexcept(true){}
template<class T0,class T1>
inline void UnusedParameter(T0 cnLib_UREF,T1 cnLib_UREF)noexcept(true){}
template<class T0,class T1,class T2>
inline void UnusedParameter(T0 cnLib_UREF,T1 cnLib_UREF,T2 cnLib_UREF)noexcept(true){}
template<class T0,class T1,class T2,class T3>
inline void UnusedParameter(T0 cnLib_UREF,T1 cnLib_UREF,T2 cnLib_UREF,T3 cnLib_UREF)noexcept(true){}

#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
