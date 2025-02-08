#include "Win_BasicStorage.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDefaultMemoryHeap::cDefaultMemoryHeap()noexcept(true)
	: bcWinMemoryHeap(HeapCreate(0,0,0))
{
}
//---------------------------------------------------------------------------
cDefaultMemoryHeap::~cDefaultMemoryHeap()noexcept(true)
{
	::HeapDestroy(fHeap);
}
//---------------------------------------------------------------------------
