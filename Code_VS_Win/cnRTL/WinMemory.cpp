#include <cnTK\cnPlatform.h>

#ifdef	cnLib_DEBUG
#include <crtdbg.h>
#endif

#include "WinMemory.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#ifdef	cnLib_DEBUG
//#define	cnLib_DEBUG_REPORTLEAK

#ifdef cnLib_DEBUG_REPORTLEAK
static _CrtMemState DebugMemStat;
#endif // cnLib_DEBUG_REPORTLEAK

cCRTDebugMemoryHeap::cCRTDebugMemoryHeap()noexcept(true)
{
#ifdef cnLib_DEBUG_REPORTLEAK
	_CrtMemCheckpoint(&DebugMemStat);
#endif // cnLib_DEBUG_REPORTLEAK
}
//---------------------------------------------------------------------------
cCRTDebugMemoryHeap::~cCRTDebugMemoryHeap()noexcept(true)
{
#ifdef cnLib_DEBUG_REPORTLEAK
	_CrtMemState CurDebugMemStat;
	_CrtMemCheckpoint(&CurDebugMemStat);
	_CrtMemState DiffMemStat;
	if(_CrtMemDifference(&DiffMemStat,&DebugMemStat,&CurDebugMemStat)){
		// leaks detected
		_CrtMemDumpAllObjectsSince(&DebugMemStat);
		DebugBreak();
	}
#endif // cnLib_DEBUG_REPORTLEAK
}
//---------------------------------------------------------------------------
void* cCRTDebugMemoryHeap::Alloc(uIntn Size,uIntn Alignment)noexcept(true)
{Alignment;
	return _malloc_dbg(Size,_NORMAL_BLOCK, __FILE__, __LINE__);
}
//---------------------------------------------------------------------------
bool cCRTDebugMemoryHeap::Free(void *Pointer,uIntn Size,uIntn Alignment)noexcept(true)
{Size,Alignment;
	_free_dbg(Pointer,_NORMAL_BLOCK);
	return true;
}
//---------------------------------------------------------------------------
bool cCRTDebugMemoryHeap::Resize(void *Pointer,uIntn Size,uIntn NewSize)noexcept(true)
{Size;
	return nullptr!=_expand_dbg(Pointer,NewSize,_NORMAL_BLOCK, __FILE__, __LINE__);
}
//---------------------------------------------------------------------------
void* cCRTDebugMemoryHeap::Relocate(void *Pointer,uIntn Size,uIntn Alignment,uIntn NewSize,uIntn NewAlignment,bool &ManualCopy)noexcept(true)
{Alignment;NewAlignment;Size;
	ManualCopy=false;
	return _realloc_dbg(Pointer,NewSize,_NORMAL_BLOCK, __FILE__, __LINE__);
}
//---------------------------------------------------------------------------
uIntn cCRTDebugMemoryHeap::SizeOf(void *Pointer)noexcept(true)
{
	return _msize_dbg(Pointer,_NORMAL_BLOCK);
} 
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcWinMemoryHeap::bcWinMemoryHeap(HANDLE Heap)noexcept(true)
	: fHeap(Heap)
{
}
//---------------------------------------------------------------------------
void* bcWinMemoryHeap::Alloc(uIntn Size,uIntn Alignment)noexcept(true)
{
	DWORD Flags=0;
	if(Alignment>8 && Size>8){
		Flags=HEAP_CREATE_ALIGN_16;
	}
	return ::HeapAlloc(fHeap,Flags|HEAP_GENERATE_EXCEPTIONS,Size);
}
//---------------------------------------------------------------------------
bool bcWinMemoryHeap::Free(void *Pointer,uIntn Size,uIntn Alignment)noexcept(true)
{Size,Alignment;
	return ::HeapFree(fHeap,0,Pointer)!=FALSE;
}
//---------------------------------------------------------------------------
bool bcWinMemoryHeap::Resize(void *Pointer,uIntn Size,uIntn NewSize)noexcept(true)
{Size;
	return nullptr!=::HeapReAlloc(fHeap,HEAP_REALLOC_IN_PLACE_ONLY,Pointer,NewSize);
}
//---------------------------------------------------------------------------
void* bcWinMemoryHeap::Relocate(void *Pointer,uIntn Size,uIntn Alignment,uIntn NewSize,uIntn NewAlignment,bool &ManualCopy)noexcept(true)
{Size,Alignment;
	DWORD Flags=0;
	if(NewAlignment>8 && NewSize>8){
		Flags=HEAP_CREATE_ALIGN_16;
	}
	ManualCopy=false;
	if(Pointer==nullptr){
		return ::HeapAlloc(fHeap,Flags|HEAP_GENERATE_EXCEPTIONS,NewSize);
	}
	return ::HeapReAlloc(fHeap,Flags|HEAP_GENERATE_EXCEPTIONS,Pointer,NewSize);
}
//---------------------------------------------------------------------------
uIntn bcWinMemoryHeap::SizeOf(void *Pointer)noexcept(true)
{
	return ::HeapSize(fHeap,0,Pointer);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinMemoryHeap::cWinMemoryHeap(HANDLE Heap)noexcept(true)
	: bcWinMemoryHeap(Heap)
{
}
//---------------------------------------------------------------------------
cWinMemoryHeap::cWinMemoryHeap(DWORD Options,SIZE_T InitialSize,SIZE_T MaximumSize)noexcept(true)
	: bcWinMemoryHeap(::HeapCreate(Options,InitialSize,MaximumSize))
{
}
//---------------------------------------------------------------------------
cWinMemoryHeap::~cWinMemoryHeap()noexcept(true)
{
	::HeapDestroy(fHeap);
}
//---------------------------------------------------------------------------
