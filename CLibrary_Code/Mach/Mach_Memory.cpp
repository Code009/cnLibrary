
#include <mach/mach.h>

#include "Mach_Memory.h"

using namespace cnLibrary;
//using namespace cnRTL;
using namespace siMach;

#if	0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static vm_prot_t ProtectLib2Sys(VMProtect Protect)
{
	static constexpr vm_prot_t ProtectFlag[]={
		VM_PROT_NONE,				//NoAccess
		VM_PROT_READ,				//Read
		VM_PROT_WRITE,				//Write
		VM_PROT_READ|VM_PROT_WRITE,	//ReadWrite
		VM_PROT_EXECUTE,			//Execute
		VM_PROT_EXECUTE|PROT_READ,	//ExecuteRead
		VM_PROT_EXECUTE|VM_PROT_WRITE,	//ExecuteWrite
		VM_PROT_EXECUTE|PROT_READ|VM_PROT_WRITE,	//ExecuteWriteCopy
	};
	auto pi=static_cast<uint8>(Protect);
	if(pi>=ArrayLength(ProtectFlag))
		return VM_PROT_NONE;
	return ProtectFlag[pi];
}
//---------------------------------------------------------------------------
void* VM::Reserve(void *Address,uintn Size)
{
	vm_address_t addr;
	int flag;
	if(Address==nullptr){
		flag=TRUE;
	}
	else{
		addr=reinterpret_cast<vm_address_t>(Address);
		flag=FALSE;
	}
	auto r=vm_allocate(mach_task_self(),&addr,Size,flag);
	if(r!=KERN_SUCCESS){
		return nullptr;
	}
	vm_protect(mach_task_self(),addr,Size, false,VM_PROT_NONE);
	return reinterpret_cast<void*>(addr);
}
//---------------------------------------------------------------------------
void* VM::Commit(void *Address,uintn Size,VMProtect ProtectFlag)
{
	vm_address_t addr;
	int flag;
	if(Address==nullptr){
		flag=TRUE;
	}
	else{
		addr=reinterpret_cast<vm_address_t>(Address);
		flag=FALSE;
	}
	auto r=vm_allocate(mach_task_self(),&addr,Size,flag);
	if(r!=KERN_SUCCESS){
		return nullptr;
	}
	vm_protect(mach_task_self(),addr,Size, false,VM_PROT_NONE);
	return reinterpret_cast<void*>(addr);
}
//---------------------------------------------------------------------------
bool VM::Decommit(void *Address,uintn Size)
{
	if(mprotect(Address,Size,PROT_NONE)!=0)
		return false;
	// mark as discard
	madvise(Address,Size,MADV_DONTNEED);
	return true;
}
//---------------------------------------------------------------------------
bool VM::Free(void *Address,uintn Size)
{
	auto r=vm_deallocate(mach_task_self(),reinterpret_cast<vm_address_t>(Address),Size);
	return r==KERN_SUCCESS;
}

//---------------------------------------------------------------------------
bool cnLib_FUNC cVirtualMemoryManager::Protect(void *Address,uintn Size,VMProtect ProtectFlag)
{
	return mprotect(Address,Size,ProtectLib2Sys(ProtectFlag))==0;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cVirtualMemoryManager::Discard(void *Address,uintn Size)
{
	return true;
//	return madvise(Address,Size,MADV_REMOVE)==0;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cVirtualMemoryManager::Lock(void *Address,uintn Size)
{
	return mlock(Address,Size)==0;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cVirtualMemoryManager::Unlock(void *Address,uintn Size)
{
	return munlock(Address,Size)==0;
}
//---------------------------------------------------------------------------
uint8 cnLib_FUNC cVirtualMemoryManager::GetPageSizeB(void)
{
	return 12;
}
//---------------------------------------------------------------------------
uint8 cnLib_FUNC cVirtualMemoryManager::GetAllocationGranularityB(void)
{
	return 12;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cMMapFile::cMMapFile(int FileHandle,int ProtectFlag)
{
	fFileHandle=FileHandle;
	ProtectFlag=ProtectFlag;
}
//---------------------------------------------------------------------------
uInt64 cMMapFile::GetSize(void)
{
	return fFileSize;
}
//---------------------------------------------------------------------------
void* cMMapFile::Map(uIntn Offset,uIntn Size)
{
	return mmap(nullptr,Size,ProtectFlag,MAP_SHARED,fFileHandle,Address);
}
//---------------------------------------------------------------------------
bool cMMapFile::Unmap(void *Pointer,uIntn Size)
{
	return munmap(Pointer,Size)==0;
}
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

