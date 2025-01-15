#include "Win_Common.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

// surpress constructor and destructor
cnVar::cStaticVariable<cWindowsSystemReference> cnWin::gWindowsSystemReference;

//- System Info -------------------------------------------------------------
cSysInfo::cSysInfo()
{
	::GetSystemInfo(this);
}
//- System Info - Startup Info ----------------------------------------------
//cStartupInfo::cStartupInfo()
//{
//	::GetStartupInfoW(this);
//}
//---------------------------------------------------------------------------
DWORD cnWin::GetMemPageSize(void)
{
	return gSysInfo.dwPageSize;
}
//---------------------------------------------------------------------------
DWORD cnWin::GetMemAllocationGranularity(void)
{
	return gSysInfo.dwAllocationGranularity;
}
//---------------------------------------------------------------------------
BOOL cnWin::DuplicateCurrentThreadHandle(HANDLE &OutHandle,DWORD Access,BOOL InheritHandle)
{
	cnLib_ASSERT(&OutHandle!=nullptr);

	return DuplicateHandle(gCurrentProcessHandle,gCurrentThreadHandle,gCurrentProcessHandle,&OutHandle,Access,InheritHandle,0);
}
BOOL cnWin::DuplicateCurrentThreadHandleSameAccess(HANDLE &OutHandle,BOOL InheritHandle)
{
	cnLib_ASSERT(&OutHandle!=nullptr);

	return DuplicateHandle(gCurrentProcessHandle,gCurrentThreadHandle,gCurrentProcessHandle,&OutHandle,0,InheritHandle,DUPLICATE_SAME_ACCESS);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rPtr<iLibraryReference> cWindowsSystemReference::QueryReference(iLibraryReferrer *Referrer,bool NoLoad)noexcept(true)
{
	ufInt8 CurState=fState.Acquire.Load();
	if(CurState==sLoading){
		return nullptr;
	}
	if(CurState==sIdle){
		if(NoLoad)
			return nullptr;

		if(fState.Barrier.CmpXchg(CurState,sLoading)){
			fContext.Construct();

			auto AutoLock=TakeLock(&fContext->CS);

			fModuleShutdown=false;
			fState.Release.Store(sActive);

			WindowsInterface::SystemInitialize();

			return fContext->CreateReference(Referrer);
		}
		if(CurState==sLoading){
			return nullptr;
		}
	}
	cnLib_ASSERT(CurState==sActive);
	if(fContext->MakeStrongReference()){
		auto AutoLock=TakeLock(&fContext->CS);
		return fContext->CreateReference(Referrer);
	}
	else{
		return nullptr;
	}
}
//---------------------------------------------------------------------------
void cWindowsSystemReference::WaitShutdown(rPtr<iLibraryReference> &&Reference)noexcept(true)
{
	ufInt8 CurState=fState.Acquire.Load();
	if(CurState==sIdle)
		return;

	if(fShutdownWaiting.Barrier.Xchg(true)==true){
		return;
	}
	{
		auto AutoLock=TakeLock(&fContext->CS);

		HANDLE CurProc=::GetCurrentProcess();
		HANDLE CurThread=::GetCurrentThread();

		::DuplicateHandle(CurProc,CurThread,CurProc,&fShutdownWaitThreadHandle,0,false,DUPLICATE_SAME_ACCESS);
	}
	Reference=nullptr;

#if 1 && defined(cnLib_DEBUG)
	if(fShutdownWaitThreadHandle!=nullptr){
		do{
			if(::SleepEx(3000,TRUE)==0){
				// timeout, report remaining referrer
				auto AutoLock=TakeLock(&fContext->CS);

				cStringBuffer<uChar16> ReportText;
				fContext->ReportReferenceSet(ReportText);
				OutputDebugStringW(utow(ReportText.GetString()));
				break;
			}
		}while(fShutdownWaitThreadHandle!=nullptr);

	}
#endif
	while(fShutdownWaitThreadHandle!=nullptr){
		::SleepEx(INFINITE,TRUE);
	}
	fState.Barrier.CmpStore(sNotifyWait,sUnloading);
	ReferenceProcess();


	fShutdownWaiting.Release.Store(false);
}
//---------------------------------------------------------------------------
VOID cWindowsSystemReference::NotifyShutdownAPC(_In_ ULONG_PTR Parameter)
{
	auto This=reinterpret_cast<cWindowsSystemReference*>(Parameter);
	if(This->fShutdownWaitThreadHandle!=nullptr){
		::CloseHandle(This->fShutdownWaitThreadHandle);
		This->fShutdownWaitThreadHandle=nullptr;
	}
}
//---------------------------------------------------------------------------
cWindowsSystemReference::cContext::cContext(void)noexcept(true)
	: bcRegisteredReference(true)
{
}
//---------------------------------------------------------------------------
cWindowsSystemReference* cWindowsSystemReference::cContext::GetHost(void)noexcept(true)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cWindowsSystemReference::fContext);
}
//---------------------------------------------------------------------------
void cWindowsSystemReference::cContext::ReferenceUpdate(void)noexcept(true)
{
	return GetHost()->ReferenceProcess();
}
//---------------------------------------------------------------------------
void cWindowsSystemReference::cContext::ReferenceShutdown(void)noexcept(true)
{
	auto Host=GetHost();
	Host->fModuleShutdown=true;
	return Host->ReferenceProcess();
}
//---------------------------------------------------------------------------
void cWindowsSystemReference::ReferenceProcess(void)noexcept(true)
{
	if(fReferenceProcessFlag.Acquire()==false)
		return;

	bool Finalize=false;
	do{
		fReferenceProcessFlag.Continue();

		auto AutoLock=TakeLock(&fContext->CS);

		auto IsEmptySet=fContext->UpdateReferenceSet();
		if(fModuleShutdown){

			if(IsEmptySet){
				// all reference deleted

				ufInt8 CurState=fState.Acquire.Load();
				if(CurState==sActive){
					WindowsInterface::SystemFinalize();

					if(fShutdownWaitThreadHandle!=nullptr){
						fState.Release.Store(sNotifyWait);
						::QueueUserAPC(NotifyShutdownAPC,fShutdownWaitThreadHandle,reinterpret_cast<ULONG_PTR>(this));
					}
					else{
						Finalize=true;
					}
				}
				if(CurState==sUnloading){
					Finalize=true;
				}
			}
		}

	}while(fReferenceProcessFlag.Release()==false);

	if(Finalize){
		fContext.Destruct();
		fState.Release.Store(sIdle);
	}
}
//---------------------------------------------------------------------------
