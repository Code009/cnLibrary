/*- cnRTL - Windows - Win32 Interlocked -----------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-06-04                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#include <cnRTL\WinSDKHeader.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
namespace Interlocked{
//---------------------------------------------------------------------------
template<uIntn Size>
struct _Func;
//---------------------------------------------------------------------------
template<>
struct _Func<1>
{
	typedef CHAR Type;
	static CHAR Inc(volatile CHAR &Dest)noexcept(true);
	static CHAR Dec(volatile CHAR &Dest)noexcept(true);
	static CHAR Xchg(volatile CHAR &Dest,CHAR Src)noexcept(true);
	static CHAR CmpXchg(volatile CHAR &Dest,CHAR Src,CHAR Compare)noexcept(true);
	static CHAR XchgAdd(volatile CHAR &Dest,CHAR Val)noexcept(true);
};
//---------------------------------------------------------------------------
template<>
struct _Func<2>
{
	typedef SHORT Type;
	static SHORT Inc(volatile SHORT &Dest)noexcept(true);
	static SHORT Dec(volatile SHORT &Dest)noexcept(true);
	static SHORT Xchg(volatile SHORT &Dest,SHORT Src)noexcept(true);
	static SHORT CmpXchg(volatile SHORT &Dest,SHORT Src,SHORT Compare)noexcept(true);
	static SHORT XchgAdd(volatile SHORT &Dest,SHORT Val)noexcept(true);
};
//---------------------------------------------------------------------------
template<>
struct _Func<4>
{
	typedef LONG Type;
	static LONG Inc(volatile LONG &Dest)noexcept(true);
	static LONG Dec(volatile LONG &Dest)noexcept(true);
	static LONG Xchg(volatile LONG &Dest,LONG Src)noexcept(true);
	static LONG CmpXchg(volatile LONG &Dest,LONG Src,LONG Compare)noexcept(true);
	static LONG XchgAdd(volatile LONG &Dest,LONG Val)noexcept(true);
};
//---------------------------------------------------------------------------
template<>
struct _Func<8>
{
	typedef LONG64 Type;
	static LONG64 Inc(volatile LONG64 &Dest)noexcept(true);
	static LONG64 Dec(volatile LONG64 &Dest)noexcept(true);
	static LONG64 Xchg(volatile LONG64 &Dest,LONG64 Src)noexcept(true);
	static LONG64 CmpXchg(volatile LONG64 &Dest,LONG64 Src,LONG64 Compare)noexcept(true);
	static LONG64 XchgAdd(volatile LONG64 &Dest,LONG64 Val)noexcept(true);
};
//---------------------------------------------------------------------------
// Inc
template<class TInt>
inline TInt Inc(volatile TInt &Dest)noexcept(true){
	typedef _Func<sizeof(TInt)> FuncType;
	return static_cast<TInt>( FuncType::Inc(reinterpret_cast<typename FuncType::Type volatile&>(Dest)) );
}
// Dec
template<class TInt>
inline TInt Dec(volatile TInt &Dest)noexcept(true){
	typedef _Func<sizeof(TInt)> FuncType;
	return static_cast<TInt>( FuncType::Dec(reinterpret_cast<typename FuncType::Type volatile&>(Dest)) );
}

// Xchg
template<class TInt>
inline TInt Xchg(volatile TInt &Dest,typename cnVar::TTypeDef<TInt>::Type Src)noexcept(true){
	typedef _Func<sizeof(TInt)> FuncType;
	return static_cast<TInt>( FuncType::Xchg(reinterpret_cast<typename FuncType::Type volatile&>(Dest),reinterpret_cast<typename FuncType::Type const&>(Src)) );
}

// CmpXchg
template<class TInt>
inline TInt CmpXchg(volatile TInt &Dest,typename cnVar::TTypeDef<TInt>::Type Src,typename cnVar::TTypeDef<TInt>::Type Compare)noexcept(true){
	typedef _Func<sizeof(TInt)> FuncType;
	return static_cast<TInt>( FuncType::CmpXchg(reinterpret_cast<typename FuncType::Type volatile&>(Dest),reinterpret_cast<typename FuncType::Type const&>(Src),reinterpret_cast<typename FuncType::Type const&>(Compare)) );
}

// XchgAdd
template<class TInt>
inline TInt XchgAdd(volatile TInt &Dest,typename cnVar::TTypeDef<TInt>::Type Value)noexcept(true){
	typedef _Func<sizeof(TInt)> FuncType;
	return static_cast<TInt>( FuncType::XchgAdd(reinterpret_cast<typename FuncType::Type volatile&>(Dest),reinterpret_cast<typename FuncType::Type const&>(Value)) );
}
//---------------------------------------------------------------------------
}	// namespace Interlocked
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
