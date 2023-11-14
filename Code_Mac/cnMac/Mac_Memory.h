/*--- Mac - Memory --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2012-05-26                                          */
/*-------------------------------------------------------------------------*/

#pragma once

#include <malloc/malloc.h>
//---------------------------------------------------------------------------
#include <cnMac/Mac_Header.h>
/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnMac{
//---------------------------------------------------------------------------
class cMAllocHeap : public iMemoryHeap
{
private:
	malloc_zone_t *fMAllocZone;

protected:
	cMAllocHeap()=default;
public:
	cMAllocHeap(malloc_zone_t *zone);
	~cMAllocHeap()=default;

	virtual void* cnLib_FUNC Alloc(uIntn Size,uIntn Alignment)override;
	virtual bool cnLib_FUNC Free(void *Pointer,uIntn Size,uIntn Alignment)override;
	virtual bool cnLib_FUNC Resize(void *Pointer,uIntn Size,uIntn NewSize)override;
	virtual void* cnLib_FUNC Relocate(void *Pointer,uIntn Size,uIntn Alignment,uIntn NewSize,uIntn NewAlignment,bool &ManualCopy)override;
	virtual uIntn cnLib_FUNC SizeOf(void *Pointer)override;
};
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
