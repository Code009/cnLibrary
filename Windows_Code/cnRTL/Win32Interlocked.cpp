#include <intrin.h>
#include "Win32Interlocked.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CHAR Interlocked::_Func<1>::Inc(volatile CHAR &Dest)noexcept(true){	return _InterlockedExchangeAdd8(&Dest,1);	}
CHAR Interlocked::_Func<1>::Dec(volatile CHAR &Dest)noexcept(true){	return _InterlockedExchangeAdd8(&Dest,-1);	}
CHAR Interlocked::_Func<1>::Xchg(volatile CHAR &Dest,CHAR Src)noexcept(true){	return InterlockedExchange8(&Dest,Src);	}
CHAR Interlocked::_Func<1>::CmpXchg(volatile CHAR &Dest,CHAR Src,CHAR Compare)noexcept(true){	return _InterlockedCompareExchange8(&Dest,Src,Compare);	}
CHAR Interlocked::_Func<1>::XchgAdd(volatile CHAR &Dest,CHAR Src)noexcept(true){	return InterlockedExchangeAdd8(&Dest,Src);	}
//---------------------------------------------------------------------------
SHORT Interlocked::_Func<2>::Inc(volatile SHORT &Dest)noexcept(true){	return InterlockedIncrement16(&Dest);	}
SHORT Interlocked::_Func<2>::Dec(volatile SHORT &Dest)noexcept(true){	return InterlockedDecrement16(&Dest);	}
SHORT Interlocked::_Func<2>::Xchg(volatile SHORT &Dest,SHORT Src)noexcept(true){	return InterlockedExchange16(&Dest,Src);	}
SHORT Interlocked::_Func<2>::CmpXchg(volatile SHORT &Dest,SHORT Src,SHORT Compare)noexcept(true){	return InterlockedCompareExchange16(&Dest,Src,Compare);	}
SHORT Interlocked::_Func<2>::XchgAdd(volatile SHORT &Dest,SHORT Src)noexcept(true){
	SHORT TargetValue=Dest;
	SHORT CurValue;
	do{
		CurValue=TargetValue;
		TargetValue=InterlockedCompareExchange16(&Dest,CurValue+Src,TargetValue);
	}while(CurValue==TargetValue);
	return TargetValue;
}
//---------------------------------------------------------------------------
LONG Interlocked::_Func<4>::Inc(volatile LONG &Dest)noexcept(true){	return InterlockedIncrement(&Dest);	}
LONG Interlocked::_Func<4>::Dec(volatile LONG &Dest)noexcept(true){	return InterlockedDecrement(&Dest);	}
LONG Interlocked::_Func<4>::Xchg(volatile LONG &Dest,LONG Src)noexcept(true){	return InterlockedExchange(&Dest,Src);	}
LONG Interlocked::_Func<4>::CmpXchg(volatile LONG &Dest,LONG Src,LONG Compare)noexcept(true){	return InterlockedCompareExchange(&Dest,Src,Compare);	}
LONG Interlocked::_Func<4>::XchgAdd(volatile LONG &Dest,LONG Src)noexcept(true){	return InterlockedExchangeAdd(&Dest,Src);	}
//---------------------------------------------------------------------------
LONG64 Interlocked::_Func<8>::Inc(volatile LONG64 &Dest)noexcept(true){	return InterlockedIncrement64(&Dest);	}
LONG64 Interlocked::_Func<8>::Dec(volatile LONG64 &Dest)noexcept(true){	return InterlockedDecrement64(&Dest);	}
LONG64 Interlocked::_Func<8>::Xchg(volatile LONG64 &Dest,LONG64 Src)noexcept(true){	return InterlockedExchange64(&Dest,Src);	}
LONG64 Interlocked::_Func<8>::CmpXchg(volatile LONG64 &Dest,LONG64 Src,LONG64 Compare)noexcept(true){	return InterlockedCompareExchange64(&Dest,Src,Compare);	}
LONG64 Interlocked::_Func<8>::XchgAdd(volatile LONG64 &Dest,LONG64 Src)noexcept(true){	return InterlockedExchangeAdd64(&Dest,Src);	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
