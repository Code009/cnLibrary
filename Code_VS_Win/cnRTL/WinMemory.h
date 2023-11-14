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
	cCRTDebugMemoryHeap();
	~cCRTDebugMemoryHeap();

	virtual void* cnLib_FUNC Alloc(uIntn Size,uIntn Alignment)override;
	virtual bool cnLib_FUNC Free(void *Pointer,uIntn Size,uIntn Alignment)override;
	virtual bool cnLib_FUNC Resize(void *Pointer,uIntn Size,uIntn NewSize)override;
	virtual void* cnLib_FUNC Relocate(void *Pointer,uIntn Size,uIntn Alignment,uIntn NewSize,uIntn NewAlignment,bool &ManualCopy)override;
	virtual uIntn cnLib_FUNC SizeOf(void *Pointer)override;
};
#endif // cnLib_DEBUG
//---------------------------------------------------------------------------
// Thread safe
class bcWinMemoryHeap : public iMemoryHeap
{
public:
	bcWinMemoryHeap(HANDLE Heap);

	virtual void* cnLib_FUNC Alloc(uIntn Size,uIntn Alignment)override;
	virtual bool cnLib_FUNC Free(void *Pointer,uIntn Size,uIntn Alignment)override;
	virtual bool cnLib_FUNC Resize(void *Pointer,uIntn Size,uIntn NewSize)override;
	virtual void* cnLib_FUNC Relocate(void *Pointer,uIntn Size,uIntn Alignment,uIntn NewSize,uIntn NewAlignment,bool &ManualCopy)override;
	virtual uIntn cnLib_FUNC SizeOf(void *Pointer)override;

protected:
	HANDLE fHeap;
};
//---------------------------------------------------------------------------
// Thread safe
class cWinMemoryHeap : public bcWinMemoryHeap
{
public:
	cWinMemoryHeap(HANDLE Heap);
	cWinMemoryHeap(DWORD Options,SIZE_T InitialSize,SIZE_T MaximumSize);
	~cWinMemoryHeap();
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
