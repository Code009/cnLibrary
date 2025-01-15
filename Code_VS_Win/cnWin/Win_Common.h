﻿/*- cnWin - common --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-02-10                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/

#include <cnWin\cnWinInterface.h>


#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
// module handle for witch executable links the static library
extern const HMODULE gModuleHandle;
//---------------------------------------------------------------------------
// Current Process Value
extern HANDLE gCurrentProcessHandle;
// Current Thread Value
extern HANDLE gCurrentThreadHandle;
//---------------------------------------------------------------------------
struct cSysInfo : SYSTEM_INFO
{
	cSysInfo();
};
//---------------------------------------------------------------------------
extern const cSysInfo gSysInfo;
//---------------------------------------------------------------------------
//struct cStartupInfo : STARTUPINFOW
//{
//	cStartupInfo();
//};
////---------------------------------------------------------------------------
//extern const cStartupInfo gStartupInfo;
//---------------------------------------------------------------------------
DWORD GetMemPageSize(void);
DWORD GetMemAllocationGranularity(void);
//---------------------------------------------------------------------------
BOOL DuplicateCurrentThreadHandle(HANDLE &OutHandle,DWORD Access,BOOL InheritHandle=FALSE);
BOOL DuplicateCurrentThreadHandleSameAccess(HANDLE &OutHandle,BOOL InheritHandle=FALSE);

//---------------------------------------------------------------------------
class cWindowsSystemReference
{
public:
	rPtr<iLibraryReference> QueryReference(iLibraryReferrer *Referrer,bool NoLoad)noexcept(true);
	void WaitShutdown(rPtr<iLibraryReference> &&Reference)noexcept(true);

protected:

private:
	class cContext : public cnRTL::bcRegisteredReference
	{
	public:
		cContext(void)noexcept(true);

		cnRTL::cnWinRTL::cCriticalSection CS;

		cWindowsSystemReference* GetHost(void)noexcept(true);
		
		virtual void ReferenceUpdate(void)noexcept(true)override;
		virtual void ReferenceShutdown(void)noexcept(true)override;
	};
	cnVar::cStaticVariable<cContext> fContext;
	static constexpr ufInt8 sIdle=0;
	static constexpr ufInt8 sLoading=1;
	static constexpr ufInt8 sActive=2;
	static constexpr ufInt8 sNotifyWait=3;
	static constexpr ufInt8 sUnloading=4;
	cnRTL::cAtomicVar<ufInt8> fState=0;
	cnRTL::cExclusiveFlag fReferenceProcessFlag;
	cnRTL::cAtomicVar<bool> fShutdownWaiting=0;
	bool fModuleShutdown;
	HANDLE fShutdownWaitThreadHandle;

	void ReferenceProcess(void)noexcept(true);

	static VOID NTAPI NotifyShutdownAPC(_In_ ULONG_PTR Parameter);
};
extern cnVar::cStaticVariable<cWindowsSystemReference> gWindowsSystemReference;
//---------------------------------------------------------------------------

#if 0

//- Sequence Stack ----------------------------------------------------------

template<class T,class TVoidMemoryOperator>
class cSeqStack
{
public:
	static_assert(cnVar::TIsSame<void,typename TVoidMemoryOperator::tElement>::Value,"should use memory operator use void type element");

	cSeqStack(){	fTop=nullptr;	}
	~cSeqStack(){	Clear();	}

	cSeqStack(const cSeqStack &Src)=delete;
	cSeqStack(cSeqStack &&Src)=delete;

	// = copy
	cSeqStack& operator =(const cSeqStack &Src)=delete;
	// = move
	cSeqStack& operator =(cSeqStack &&Src)=delete;

	// GetCount
	// return number items in the stack
	uIntn GetCount(void)const{
		if(fTop==nullptr)
			return 0;

		uIntn Count=fIndex;
		auto CurBlock=fTop->Next;
		while(CurBlock!=nullptr){
			Count+=CurBlock->Length;
			CurBlock=CurBlock->Next;
		}
		return Count;
	}

	// Clear
	//	remove all items
	void Clear(void){
		if(fTop!=nullptr){
			auto DeleteBlock=fTop;
			fTop=fTop->Next;

			for(uIntn i=0;i<fIndex;i++){
				cnVar::ManualDestruct(fTop->Data[i]);
			}
			TVoidMemoryOperator::Deallocate(DeleteBlock,sizeof(cBlock),1);
			while(fTop!=nullptr){
				DeleteBlock=fTop;
				fTop=fTop->Next;

				for(uIntn i=0;i<DeleteBlock->Length;i++){
					cnVar::ManualDestruct(fTop->Data[i]);
				}
				TVoidMemoryOperator::Deallocate(DeleteBlock,sizeof(cBlock),1);
			}
		}
	}

	// Push
	//	push a default constructed item
	template<class...TArgs>
	T& Push(TArgs&&...Args){
		if(fTop==nullptr){
			IncBlock();
		}
		else{
			fIndex++;
			if(fIndex>=fTop->Length){
				IncBlock();
			}
		}
		T &CurTopItem=fTop->Data[fIndex];
		cnVar::ManualConstruct(CurTopItem,static_cast<TArgs&&>(Args)...);
		return CurTopItem;
	}
	// Pop
	//	pop an item
	bool Pop(void){
		if(fTop==nullptr)
			return false;
		cnVar::ManualDestruct(fTop->Data[fIndex]);
		if(fIndex==0){
			DecBlock();
		}
		else{
			fIndex--;
		}
		return true;
	}
	// Pop
	//	pop an item
	bool Pop(T &PopedItem){
		if(fTop==nullptr)
			return false;
		PopedItem=cnVar::MoveCast(fTop->Data[fIndex]);
		cnVar::ManualDestruct(fTop->Data[fIndex]);
		if(fIndex==0){
			DecBlock();
		}
		else{
			fIndex--;
		}
		return true;
	}
	// Peek
	T& Peek(void){
		if(fTop==nullptr)
			return *reinterpret_cast<T*>(nullptr);
		return fTop->Data[fIndex];
	}
protected:
	struct cBlock{
		cBlock *Next;
		uIntn Length;
		T Data[1];
	};
	cBlock *fTop;
	uIntn fIndex;

	void IncBlock(void){
		uIntn ItemCount=16;
		auto NewBlock=static_cast<cBlock*>(TVoidMemoryOperator::Allocate(sizeof(cBlock)-sizeof(T)+sizeof(T)*ItemCount,0));
		NewBlock->Length=ItemCount;
		NewBlock->Next=fTop;
		fTop=NewBlock;
		fIndex=0;
	}
	void DecBlock(void){
		auto DeleteBlock=fTop;
		fTop=fTop->Next;
		if(fTop!=nullptr){
			fIndex=fTop->Length-1;
		}
		TVoidMemoryOperator::Deallocate(DeleteBlock,sizeof(cBlock),1);
	}
};
#endif // 0
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
