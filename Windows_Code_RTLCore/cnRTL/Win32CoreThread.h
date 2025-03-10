/*- CLib - CRT Atomic -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-06-04                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#include <cnRTL\Win32Interlocked.h>
#include <cnRTL\Win32Thread.h>

/*-------------------------------------------------------------------------*/
#ifdef __cplusplus


//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------

struct TKRuntime::SystemTime
{
	struct tTimeOperator
	{
		typedef LARGE_INTEGER tValue;
		typedef sfInt64 tOperand;
		static tValue Add(const tValue &Value,tOperand Operand)noexcept(true){
			auto Freq=cnRTL::cnWinRTL::QueryPerformanceFrequencyCached();
			LARGE_INTEGER RetTime;
			RetTime.QuadPart=Value.QuadPart+Operand*Freq/1000000000;
			return RetTime;
		}
		static tValue Sub(const tValue &Value,tOperand Operand)noexcept(true){
			auto Freq=cnRTL::cnWinRTL::QueryPerformanceFrequencyCached();
			LARGE_INTEGER RetTime;
			RetTime.QuadPart=Value.QuadPart-Operand*Freq/1000000000;
			return RetTime;
		}
		static tOperand Mod(const tValue &Value,tOperand Operand)noexcept(true){
			auto Freq=cnRTL::cnWinRTL::QueryPerformanceFrequencyCached();
			return (Value.QuadPart*1000000000/Freq)%Operand;
		}

		static tOperand Relative(const tValue &Minuend,const tValue &Subtrahend)noexcept(true){
			sInt64 s=Minuend.QuadPart-Subtrahend.QuadPart;
			auto Freq=cnRTL::cnWinRTL::QueryPerformanceFrequencyCached();
			return s*1000000000/Freq;
		}

		static bool Equal(const tValue &Value1,const tValue &Value2)noexcept(true){
			return Value1.QuadPart==Value2.QuadPart;
		}
		static sfInt8 Compare(const tValue &Value1,const tValue &Value2)noexcept(true){
			return cnVar::DefaultCompare(Value1.QuadPart,Value2.QuadPart);
		}
	};
	static tTimeOperator::tValue GetTimeNow(void)noexcept(true){
		LARGE_INTEGER CounterNow;
		::QueryPerformanceCounter(&CounterNow);
		return CounterNow;
	}

	struct tRealTimeOperator
	{
		typedef FILETIME tValue;
		typedef sfInt64 tOperand;
		static tValue Add(tValue &Value,tOperand Operand)noexcept(true){
			union{
				FILETIME RetTime;
				uInt64 RetValue;
			};
			RetValue=reinterpret_cast<const uInt64&>(Value)+Operand/100;
			return RetTime;
		}
		static tValue Sub(tValue &Value,tOperand Operand)noexcept(true){
			union{
				FILETIME RetTime;
				uInt64 RetValue;
			};
			RetValue=reinterpret_cast<const uInt64&>(Value)-Operand/100;
			return RetTime;
		}
		static tOperand Mod(tValue &Value,tOperand Operand)noexcept(true){
			return reinterpret_cast<const sInt64&>(Value)*100%Operand;
		}

		static tOperand Relative(const tValue &Minuend,const tValue &Subtrahend)noexcept(true){
			sInt64 s=reinterpret_cast<const uInt64&>(Minuend)-reinterpret_cast<const uInt64&>(Subtrahend);
			return s*100;
		}

		static bool Equal(const tValue &Value1,const tValue &Value2)noexcept(true){
			return reinterpret_cast<const uInt64&>(Value1)==reinterpret_cast<const uInt64&>(Value2);
		}
		static sfInt8 Compare(const tValue &Value1,const tValue &Value2)noexcept(true){
			return cnVar::DefaultCompare(reinterpret_cast<const uInt64&>(Value1),reinterpret_cast<const uInt64&>(Value2));
		}
	};

	static tRealTimeOperator::tValue GetRealTimeNow(void)noexcept(true){
		FILETIME ft;
		::GetSystemTimeAsFileTime(&ft);
		return ft;
	}
};


struct TKRuntime::SystemThread
{
	typedef DWORD tThreadID;
	//typedef tThreadReference;

	static void Yield(void)noexcept(true){	::SwitchToThread();	}
	static void SpinWait(uIntn)noexcept(true){	::SwitchToThread();	}
	static void Suspend(uInt64 Duration)noexcept(true){	::Sleep(static_cast<DWORD>(Duration/1000000));	}

	static tThreadID GetID(void)noexcept(true){	return ::GetCurrentThreadId();	}

	typedef cnRTL::Win32RTLCore::cSingleNotification tSingleNotification;

};

//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
