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

#define cnLib_TYPELESS_ENUM_BEGIN(_name_)		enum class _name_
#define cnLib_TYPELESS_ENUM_END(_name_)			;	typedef _name_ e##_name_

// !cnLibrary_CPPEXCLUDE_ENUMCLASS
#else
// cnLibrary_CPPEXCLUDE_ENUMCLASS

#define cnLib_INTENUM_BEGIN(_type_,_name_)		typedef _type_ e##_name_;	struct _name_{enum Enum_##_name_
#define cnLib_INTENUM_END(_name_)				;}

#define cnLib_ENUM_BEGIN(_type_,_name_)			typedef _type_ e##_name_;	struct _name_{enum Enum_##_name_
#define cnLib_ENUM_END(_name_)					;}

#define cnLib_TYPELESS_ENUM_BEGIN(_name_)		struct _name_{enum e##_name_
#define cnLib_TYPELESS_ENUM_END(_name_)			;};	typedef _name_::e##_name_ e##_name_

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
cnLib_TYPELESS_ENUM_BEGIN(ByteOrder){
	LittleEndian,
		BigEndian,
}cnLib_TYPELESS_ENUM_END(ByteOrder);
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

template<class T, T v>
struct TConstantValue
{
	static constexpr T Value=v;
};
template<class T,T v>
constexpr T TConstantValue<T,v>::Value;

// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
template<class T, T v>
struct TConstantValue
{
	static const T Value;
};
template<class T, T v>
const T TConstantValue<T, v>::Value=v;

#endif	// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L

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


template<uIntn Size,bool Signed>
struct TFastIntegerOfSizeMostNature
	: TFastIntegerOfSize<(Size<sizeof(uIntn)?Size:sizeof(uIntn)),Signed>
{};

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------

typedef cnVar::TFastIntegerOfSizeMostNature<2,true>::Type	sfInt16n;
typedef cnVar::TFastIntegerOfSizeMostNature<2,false>::Type	ufInt16n;
typedef cnVar::TFastIntegerOfSizeMostNature<4,true>::Type	sfInt32n;
typedef cnVar::TFastIntegerOfSizeMostNature<4,false>::Type	ufInt32n;
typedef cnVar::TFastIntegerOfSizeMostNature<8,true>::Type	sfInt64n;
typedef cnVar::TFastIntegerOfSizeMostNature<8,false>::Type	ufInt64n;
typedef cnVar::TFastIntegerOfSizeMostNature<16,true>::Type	sfInt128n;
typedef cnVar::TFastIntegerOfSizeMostNature<16,false>::Type	ufInt128n;

//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

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

struct cNoInitialization{};
static cnLib_CONSTVAR cNoInitialization *NoInitialization=0;
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
inline void* operator new (cnLibrary::tSize,cnLibrary::cnVar::cNoInitialization *Pointer)noexcept(true){	return Pointer;}
inline void operator delete(void *,cnLibrary::cnVar::cNoInitialization*)noexcept(true){}
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnMemory{
//---------------------------------------------------------------------------

template<uIntn Size>
struct cPlainData
{
	uInt8 _space_[Size];
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
	new(reinterpret_cast<cnVar::cNoInitialization*>(Dest)) cPlainData<Size>();
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
};

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TTypePack
{
	template<uIntn TypeIndex>	struct tTypeByIndex : TSelect<TypeIndex,cnLib_VARIADIC_TEMPLATE_EXPAND>{};
	static cnLib_CONSTVAR uIntn Count=cnLib_VARIADIC_TEMPLATE_PARAMETER_COUNT;
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
namespace cnVar{

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
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
