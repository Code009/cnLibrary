/*- CLib - CRT Time -------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2025-02-25                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_CLib_CRT_CRTTime_H__
#define __cnLibrary_CLib_CRT_CRTTime_H__

#include <cnSystem/cnDebug.h>

#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace CRT{
//---------------------------------------------------------------------------
template<class TClock>
struct cTimeOperator
{
	typedef std::chrono::time_point<TClock> tValue;
	typedef sfInt64 tOperand;
	static tValue Add(tValue &Value,tOperand Operand)noexcept(true){
		return Value+std::chrono::round<tValue::duration>(std::chrono::nanoseconds(Operand));
	}
	static tValue Sub(tValue &Value,tOperand Operand)noexcept(true){
		return Value-std::chrono::round<tValue::duration>(std::chrono::nanoseconds(Operand));
	}
	static tOperand Mod(tValue &Value,tOperand Operand)noexcept(true){
		std::chrono::nanoseconds ValueNanoseconds(Value.time_since_epoch());
		return static_cast<sfInt64>((ValueNanoseconds%std::chrono::nanoseconds(Operand)).count());
	}

	static tOperand Relative(const tValue &Minuend,const tValue &Subtrahend)noexcept(true){
		return static_cast<sfInt64>(std::chrono::nanoseconds(Minuend-Subtrahend).count());
	}

	static bool Equal(const tValue &Value1,const tValue &Value2)noexcept(true){
		return Value1==Value2;
	}
	static sfInt8 Compare(const tValue &Value1,const tValue &Value2)noexcept(true){
		return cnVar::DefaultCompare(Value1.time_since_epoch().count(),Value2.time_since_epoch().count());
	}
};
//---------------------------------------------------------------------------
}	// namespace CRT
//---------------------------------------------------------------------------

struct TKRuntime::SystemTime
{

	struct tTimeOperator : CRT::cTimeOperator<std::chrono::steady_clock>{};

	static tTimeOperator::tValue GetTimeNow(void)noexcept(true){
		return std::chrono::steady_clock::now();
	}


	struct tRealTimeOperator : CRT::cTimeOperator<std::chrono::system_clock>{};

	static tRealTimeOperator::tValue GetRealTimeNow(void)noexcept(true){
		return std::chrono::system_clock::now();
	}
};
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif


