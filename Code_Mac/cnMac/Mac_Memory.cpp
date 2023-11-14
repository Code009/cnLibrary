#include "Mac_Memory.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cMAllocHeap::cMAllocHeap(malloc_zone_t *zone)
	: fMAllocZone(zone)
{
}
//---------------------------------------------------------------------------
void* cMAllocHeap::Alloc(uIntn Size,uIntn Alignment)
{
	void *Pointer;
	if(Alignment<sizeof(void*)){
		Pointer=malloc_zone_malloc(fMAllocZone,Size);
	}
	else{
		Pointer=malloc_zone_memalign(fMAllocZone,Alignment,Size);
	}
	return Pointer;
}
//---------------------------------------------------------------------------
bool cMAllocHeap::Free(void *Pointer,uIntn,uIntn)
{
	malloc_zone_free(fMAllocZone,Pointer);
	return true;
}
//---------------------------------------------------------------------------
bool cMAllocHeap::Resize(void *Pointer,uIntn Size,uIntn NewSize)
{
	return false;
}
//---------------------------------------------------------------------------
void* cMAllocHeap::Relocate(void *Pointer,uIntn Size,uIntn Alignment,uIntn NewSize,uIntn NewAlignment,bool &ManualCopy)
{
	if(NewAlignment>sizeof(void*)){
		void *Pointer=malloc_zone_memalign(fMAllocZone,NewAlignment,NewSize);
		ManualCopy=true;
		return Pointer;
	}
	ManualCopy=false;
	return malloc_zone_realloc(fMAllocZone,Pointer,NewSize);
}
//---------------------------------------------------------------------------
uIntn cMAllocHeap::SizeOf(void *Pointer)
{
	return malloc_size(Pointer);
}
//---------------------------------------------------------------------------
