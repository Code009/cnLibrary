/*- cnRTL - Win32 - Memory ------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-15                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinCommon.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
#ifdef cnLib_DEBUG
class cCRTDebugMemoryHeap : public iMemoryHeap
{
public:
	cCRTDebugMemoryHeap()noexcept(true);
	~cCRTDebugMemoryHeap()noexcept(true);

	virtual void* cnLib_FUNC Alloc(uIntn Size,uIntn Alignment)noexcept(true)override;
	virtual bool cnLib_FUNC Free(void *Pointer,uIntn Size,uIntn Alignment)noexcept(true)override;
	virtual bool cnLib_FUNC Resize(void *Pointer,uIntn Size,uIntn NewSize)noexcept(true)override;
	virtual void* cnLib_FUNC Relocate(void *Pointer,uIntn Size,uIntn Alignment,uIntn NewSize,uIntn NewAlignment,bool &ManualCopy)noexcept(true)override;
	virtual uIntn cnLib_FUNC SizeOf(void *Pointer)noexcept(true)override;
};
#endif // cnLib_DEBUG
//---------------------------------------------------------------------------
// Thread safe
class bcWinMemoryHeap : public iMemoryHeap
{
public:
	bcWinMemoryHeap(HANDLE Heap)noexcept(true);

	virtual void* cnLib_FUNC Alloc(uIntn Size,uIntn Alignment)noexcept(true)override;
	virtual bool cnLib_FUNC Free(void *Pointer,uIntn Size,uIntn Alignment)noexcept(true)override;
	virtual bool cnLib_FUNC Resize(void *Pointer,uIntn Size,uIntn NewSize)noexcept(true)override;
	virtual void* cnLib_FUNC Relocate(void *Pointer,uIntn Size,uIntn Alignment,uIntn NewSize,uIntn NewAlignment,bool &ManualCopy)noexcept(true)override;
	virtual uIntn cnLib_FUNC SizeOf(void *Pointer)noexcept(true)override;

protected:
	HANDLE fHeap;
};
//---------------------------------------------------------------------------
// Thread safe
class cWinMemoryHeap : public bcWinMemoryHeap
{
public:
	cWinMemoryHeap(HANDLE Heap)noexcept(true);
	cWinMemoryHeap(DWORD Options,SIZE_T InitialSize,SIZE_T MaximumSize)noexcept(true);
	~cWinMemoryHeap()noexcept(true);
};
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
