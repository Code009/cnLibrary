/*- cnTK - Fundamental ----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2025-02-23                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Fundamental_HPP__
#define	__cnLibrary_cnTK_Fundamental_HPP__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/Memory.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace TKRuntime{
//---------------------------------------------------------------------------

struct SystemAllocationOperator;
//	: TAllocationOperator
//{
//};

struct SystemTime;
//{
//	typedef tTimeOperator;
//	static tTimeOperator::tValue GetTimeNow(void)noexcept;
//
//	typedef tRealTimeOperator;
//	static tRealTimeOperator::tValue GetRealTimeNow(void)noexcept;
//};

//---------------------------------------------------------------------------
}	// namespace TKRuntime
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
