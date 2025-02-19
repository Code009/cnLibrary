/*- cnWin - basic memory and storage --------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-15                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnWin\Win_Common.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//- Win32 Heap Memory Manager -----------------------------------------------
class cDefaultMemoryHeap : public cnRTL::cnWinRTL::bcWinMemoryHeap
{
protected:
	cDefaultMemoryHeap()noexcept(true);
	~cDefaultMemoryHeap()noexcept(true);
};



#ifdef cnLib_DEBUG
typedef cnRTL::cnWinRTL::cCRTDebugMemoryHeap tDefaultMemoryHeap;
#else
typedef cDefaultMemoryHeap tDefaultMemoryHeap;
#endif

extern cnRTL::cReferenceStaticImplementT<tDefaultMemoryHeap> gDefaultMemoryHeap;
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
