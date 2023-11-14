/*- cnLibCRT - Fundamental ------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-04-01                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_CRTFundamental_H__
#define __cnLibrary_cnRTL_CRTFundamental_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/CRTCoreCommon.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
	
//- Class Allocation --------------------------------------------------------
struct cRTLAllocator
{
	void* operator new(tSize Size){		return malloc(Size);	}
	void operator delete(void*p){		free(p);	}

	void* operator new[](tSize Size){	return malloc(Size);	}
	void operator delete[](void*p){		free(p);	}
};

//---------------------------------------------------------------------------
struct cAllocationOperator
{
	static uIntn RoundUpCapacity(uIntn Length){
		return Length;
	}
	static void* Allocate(uIntn Length,uIntn){
		return malloc(Length);
	}
	static void Deallocate(void *Pointer,uIntn,uIntn){
		free(Pointer);
	}

	static void* Relocate(void *Pointer,uIntn,uIntn,uIntn NewLength,uIntn,bool &ManualCopy){
		ManualCopy=false;
		return realloc(Pointer,NewLength);
	}

	static bool Resize(void*,uIntn,uIntn){
		return false;
	}
};

//---------------------------------------------------------------------------
}   // namespace cnRTL
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif

