#include "Win_BasicStorage.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDefaultMemoryHeap::cDefaultMemoryHeap()
	: bcWinMemoryHeap(HeapCreate(0,0,0))
{
}
//---------------------------------------------------------------------------
cDefaultMemoryHeap::~cDefaultMemoryHeap()
{
	::HeapDestroy(fHeap);
}
//---------------------------------------------------------------------------
