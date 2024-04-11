/*- cnSystem - Memory Management ------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2008-09-06                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnSystem_cnMemory_H__
#define	__cnLibrary_cnSystem_cnMemory_H__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnFundamental.h>
#include <cnSystem/cnData.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//- Heap memory -------------------------------------------------------------
class cnLib_INTERFACE iMemoryHeap : public iReference
{
public:
	// Alloc
	//	Allocates a block of memory from this heap
	// Size				the size to be allocated, in bytes 
	// Alignment		alignment requirement of the memory
	// return pointer to allocated block. if fails, return null pointer
	virtual void* cnLib_FUNC Alloc(uIntn Size,uIntn Alignment)noexcept(true)=0;
	// Free
	//	Frees a memory block allocated from this heap
	// Pointer			A pointer to the memory block to be freed
	// Size				size close to the number of bytes of the memory, or 0 if size of the memory block is unknown
	// Alignment		alignment of the memory
	// return true if mem is valid and freed
	virtual bool cnLib_FUNC Free(void *Pointer,uIntn Size,uIntn Alignment)noexcept(true)=0;
	// Resize
	//	Resize a block of memory
	// Pointer			pointer to the memory to resizes
	// Size				size close to the size of the block, or 0 if size of the block is unknown
	// NewSize			The new size of the memory block
	// return true if block is resized, or false if the block cannot be resized
	virtual bool cnLib_FUNC Resize(void *Pointer,uIntn Size,uIntn NewSize)noexcept(true)=0;

	// Relocate
	//	Relocate a block of memory
	// Pointer				pointer to the memory to relocate, can be null
	// Size					size close to the size of the block, or 0 if size of the block is unknown
	// NewAlignment			The new alignment of the the memory
	// NewSize				The new size of the memory
	// ManualCopy	[in]	wether allow manual copy.
	//						when the block cannot be remapped, the function allocates new block of memory.
	//						if true , the function leave the new block empty and do not free the old block. otherwise the function copy content from old block to the new block.
	// ManualCopy	[out]	false if the new block is ready and old block is freed. true if new block is empty and old block is not freed
	// return pointer to relocated block. if fails, return null pointer
	virtual void* cnLib_FUNC Relocate(void *Pointer,uIntn Size,uIntn Alignment,uIntn NewSize,uIntn NewAlignment,bool &ManualCopy)noexcept(true)=0;

	// SizeOf
	//	get number of bytes of the memory block
	// Pointer				pointer to the block of memory that the function resizes
	// return size of memory block
	virtual uIntn cnLib_FUNC SizeOf(void *Pointer)noexcept(true)=0;

};
//- swap memory -------------------------------------------------------------
class cnLib_INTERFACE iSwapMemory : public iReference
{
public:
	// GetSize
	//	get total size of block
	virtual uInt64	cnLib_FUNC GetSize(void)noexcept(true)=0;
	// Map
	//	map content into memory
	// Offset		start address to map in the block, must matches memory allocation granularity
	// Size			size to map
	// ProtectFlag	protection of the view
	// return: pointer to content mapped into memory
	virtual void*	cnLib_FUNC Map(uInt64 Offset,uIntn Size)noexcept(true)=0;
	// Unmap
	//	cancel map in memory
	// Pointer	pointer previously returned from Map
	// Size		size to unmap, must be the size previously mapped
	// return: true if the content unmapped
	virtual bool	cnLib_FUNC Unmap(void *Pointer,uIntn Size)noexcept(true)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
extern iMemoryHeap*const DefaultMemoryHeap;
//---------------------------------------------------------------------------
struct DefaultHeap{
//---------------------------------------------------------------------------

static void* cnLib_FUNC Alloc(uIntn Size,uIntn Alignment)noexcept(true);
static bool cnLib_FUNC Free(void *Pointer,uIntn Size,uIntn Alignment)noexcept(true);
static bool cnLib_FUNC Resize(void *Pointer,uIntn Size,uIntn NewSize)noexcept(true);
static void* cnLib_FUNC Relocate(void *Pointer,uIntn Size,uIntn Alignment,uIntn NewSize,uIntn NewAlignment,bool &ManualCopy)noexcept(true);
static uIntn cnLib_FUNC SizeOf(void *Pointer)noexcept(true);

//---------------------------------------------------------------------------
};
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
