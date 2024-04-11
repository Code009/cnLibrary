#include "WinCOM.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

#pragma comment(lib,"OleAut32.lib")

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cBSTROwnerTokenOperator::Release(const BSTR &Token)noexcept
{
	if(Token!=nullptr)
		::SysFreeString(Token);
}
//---------------------------------------------------------------------------
apBSTR cnRTL::MakeBSTR(const wchar_t *Text)noexcept
{
	BSTR Str=::SysAllocString(Text);
	if(Str==nullptr)
		return nullptr;

	return apBSTR::TakeFromManual(Str);
}
//---------------------------------------------------------------------------
BSTR* cnRTL::apBSTRRetPtr(apBSTR &Ptr)noexcept
{
	Ptr=nullptr;
	return &Ptr.Token();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOLEModule::cOLEModule(const wchar_t *ModuleName)noexcept
	: fModuleName(ModuleName)
{
	fModuleHandle=nullptr;
	fLoadFailed=false;
}
//---------------------------------------------------------------------------
cOLEModule::~cOLEModule()noexcept
{
	if(fModuleHandle!=nullptr){
		::FreeLibrary(fModuleHandle);
	}
}
//---------------------------------------------------------------------------
bool cOLEModule::SetupModule(void)noexcept
{
	if(fModuleHandle!=nullptr){
		return true;
	}
	if(fLoadFailed)
		return false;
	fModuleHandle=::LoadLibraryW(fModuleName);
	if(fModuleHandle==nullptr){
		fLoadFailed=true;
		return false;
	}


	fGetClassObjectProc=reinterpret_cast<ProcTypeDllGetClassObject*>(::GetProcAddress(fModuleHandle,"DllGetClassObject"));
	if(fGetClassObjectProc==nullptr){
		::FreeLibrary(fModuleHandle);
		fModuleHandle=nullptr;
		fLoadFailed=true;
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
HRESULT cOLEModule::QueryFactory(REFCLSID rclsid,IClassFactory **Factory)noexcept
{
	if(SetupModule()==false)
		return E_NOTIMPL;

	return fGetClassObjectProc(rclsid,IID_IClassFactory,reinterpret_cast<void**>(Factory));
}
//---------------------------------------------------------------------------
HRESULT cOLEModule::CreateObject(REFCLSID rclsid,IUnknown *pUnkOuter, REFIID riid,LPVOID *ppvObj)noexcept
{
	COMPtr<IClassFactory> Factory;
	HRESULT hr;
	hr=QueryFactory(rclsid,COMRetPtrT(Factory));
	if(FAILED(hr)){
		return hr;
	}
	return Factory->CreateInstance(pUnkOuter,riid,ppvObj);
}
//---------------------------------------------------------------------------
COMPtr<IClassFactory> cOLEModule::QueryFactory(REFCLSID rclsid)noexcept
{
	HRESULT hr;

	COMPtr<IClassFactory> Factory;
	hr=QueryFactory(rclsid,COMRetPtrT(Factory));
	if(FAILED(hr)){
		return nullptr;
	}
	return Factory;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCOMApartmentThreading::cCOMApartmentThreading(aCls<cWinTLS> *TLS,iDispatch *Dispatch)noexcept
	: fTLS(TLS)
	, fDispatch(Dispatch)
{
	fRefCount=1;
}
//---------------------------------------------------------------------------
cCOMApartmentThreading::~cCOMApartmentThreading()noexcept
{
}
//---------------------------------------------------------------------------
void cCOMApartmentThreading::IncreaseReference(void)noexcept
{
	fRefCount.Free++;
}
//---------------------------------------------------------------------------
void cCOMApartmentThreading::DecreaseReference(void)noexcept
{
	if(--fRefCount.Free==0){
		fDispatch->Execute(nullptr,this);
	}
}
//---------------------------------------------------------------------------
iDispatch* cCOMApartmentThreading::GetDispatch(void)noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
void cCOMApartmentThreading::OnDelete(void)noexcept
{
	fTLS->Set(nullptr);
	::CoUninitialize();
}
//---------------------------------------------------------------------------
void cCOMApartmentThreading::Execute(void)noexcept
{
	OnDelete();
	delete this;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCOMApartmentThreadingModule::cThreading::cThreading(aCls<cWinTLS> *TLS,iDispatch *Dispatch)noexcept
	: cCOMApartmentThreading(cnVar::MoveCast(TLS),Dispatch)
{
}
////---------------------------------------------------------------------------
void cCOMApartmentThreadingModule::cThreading::IncreaseReference(void)noexcept(true)
{
	return cCOMApartmentThreading::IncreaseReference();
}
//---------------------------------------------------------------------------
void cCOMApartmentThreadingModule::cThreading::DecreaseReference(void)noexcept(true)
{
	return cCOMApartmentThreading::DecreaseReference();
}
//---------------------------------------------------------------------------
iDispatch* cCOMApartmentThreadingModule::cThreading::GetDispatch(void)noexcept
{
	return cCOMApartmentThreading::GetDispatch();
}
//---------------------------------------------------------------------------
void cCOMApartmentThreadingModule::cThreading::Execute(void)noexcept
{
	OnDelete();
	delete this;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCOMApartmentThreadingModule::cCOMApartmentThreadingModule()noexcept
{
	fTLS=aClsCreate<cWinTLS>();
}
//---------------------------------------------------------------------------
cCOMApartmentThreadingModule::~cCOMApartmentThreadingModule()noexcept
{
}
//---------------------------------------------------------------------------
cCOMApartmentThreadingModule::cThreading* cCOMApartmentThreadingModule::Get(void)noexcept
{
	return static_cast<cThreading*>(fTLS->Get());
}
//---------------------------------------------------------------------------
void cCOMApartmentThreadingModule::cDispatchInitialize::Execute(void)noexcept
{
	HRESULT hr=::CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);
	if(FAILED(hr)){
		NewModule=nullptr;
		return;
	}
	NewModule=new cThreading(TLS,Dispatch);
	TLS->Set(NewModule);
}
//---------------------------------------------------------------------------
rPtr<cCOMApartmentThreadingModule::cThreading> cCOMApartmentThreadingModule::Query(iDispatch *Dispatch)noexcept
{
	void *pThreadModule=fTLS->Get();
	if(pThreadModule!=nullptr){
		return static_cast<cThreading*>(pThreadModule);
	}

	if(Dispatch==nullptr)
		return nullptr;

	cThreading *NewModule;
	if(Dispatch->IsCurrentThread()==false){
		// initialize thread
		cDispatchInitialize Initialize;
		Initialize.TLS=fTLS;
		Initialize.Dispatch=Dispatch;
		Dispatch->ExecuteSync(&Initialize);
		NewModule=Initialize.NewModule;
	}
	else{
		HRESULT hr=::CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);

		if(FAILED(hr))
			return nullptr;
		NewModule=new cThreading(fTLS,Dispatch);
		fTLS->Set(NewModule);
	}
	return rPtr<cThreading>::TakeFromManual(NewModule);
}
//---------------------------------------------------------------------------
cCOMApartmentThreading* cCOMApartmentThreadingModule::GetThreading(void)noexcept
{
	return Get();
}
//---------------------------------------------------------------------------
rPtr<cCOMApartmentThreading> cCOMApartmentThreadingModule::QueryThreading(iDispatch *Dispatch)noexcept
{
	return Query(Dispatch);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinRegisterInstaller::cWinRegisterInstaller()noexcept
	: fKey(nullptr)
	, fErrorCode(ERROR_SUCCESS)
{

}
//---------------------------------------------------------------------------
cWinRegisterInstaller::~cWinRegisterInstaller()noexcept
{
	CloseTree();
}
//---------------------------------------------------------------------------
cWinRegisterInstaller::operator HKEY()const noexcept
{
	return fKey;
}
//---------------------------------------------------------------------------
DWORD cWinRegisterInstaller::GetMakeError(void)const noexcept
{
	return fErrorCode;
}
//---------------------------------------------------------------------------
bool cWinRegisterInstaller::WasnotCreated(void)const noexcept
{
	return fErrorCode==ERROR_ALREADY_EXISTS;
}
//---------------------------------------------------------------------------
LSTATUS cWinRegisterInstaller::Create(HKEY BaseKey,cString<wchar_t> Path,REGSAM samDesired)noexcept
{
	if(fKey!=nullptr)
		return ERROR_ALREADY_EXISTS;
	DWORD CreateOrOpened;
	LSTATUS ret=::RegCreateKeyExW(BaseKey,Path,0,nullptr,REG_OPTION_NON_VOLATILE,samDesired,nullptr,&fKey,&CreateOrOpened);
	if(ret!=ERROR_SUCCESS){
		return ret;
	}
	if(CreateOrOpened==REG_OPENED_EXISTING_KEY){
		ret=ERROR_ALREADY_EXISTS;
	}
	fPath=cnVar::MoveCast(Path);
	fBaseKey=BaseKey;

	return ret;
}
//---------------------------------------------------------------------------
LSTATUS cWinRegisterInstaller::Open(HKEY BaseKey,cString<wchar_t> Path,REGSAM samDesired)noexcept
{
	if(fKey!=nullptr)
		return ERROR_ALREADY_EXISTS;
	LSTATUS ret=::RegOpenKeyExW(BaseKey,Path,0,samDesired,&fKey);
	if(ret!=ERROR_SUCCESS){
		return ret;
	}
	fPath=cnVar::MoveCast(Path);
	fBaseKey=BaseKey;

	return ERROR_SUCCESS;
}
//---------------------------------------------------------------------------
LSTATUS cWinRegisterInstaller::Create(cWinRegisterInstaller &Parent,cString<wchar_t> Path,REGSAM samDesired)noexcept
{
	if(fKey!=nullptr)
		return ERROR_ALREADY_EXISTS;
	DWORD CreateOrOpened;
	LSTATUS ret=::RegCreateKeyExW(Parent,Path,0,nullptr,REG_OPTION_NON_VOLATILE,samDesired,nullptr,&fKey,&CreateOrOpened);
	if(ret!=ERROR_SUCCESS){
		return ret;
	}
	if(CreateOrOpened==REG_OPENED_EXISTING_KEY){
		ret=ERROR_ALREADY_EXISTS;
	}
	Parent.fSubItems.InsertTail(&fLinkItem);
	fPath=cnVar::MoveCast(Path);
	fBaseKey=Parent;

	return ret;
}
//---------------------------------------------------------------------------
LSTATUS cWinRegisterInstaller::Open(cWinRegisterInstaller &Parent,cString<wchar_t> Path,REGSAM samDesired)noexcept
{
	if(fKey!=nullptr)
		return ERROR_ALREADY_EXISTS;
	LSTATUS ret=::RegOpenKeyExW(Parent,Path,0,samDesired,&fKey);
	if(ret!=ERROR_SUCCESS){
		return ret;
	}
	Parent.fSubItems.InsertTail(&fLinkItem);
	fPath=cnVar::MoveCast(Path);
	fBaseKey=Parent;

	return ERROR_SUCCESS;
}
//---------------------------------------------------------------------------
template<>
void cWinRegisterInstaller::Make<false>(HKEY BaseKey,cString<wchar_t> Path)noexcept
{
	fErrorCode=Open(BaseKey,Path);
}
//---------------------------------------------------------------------------
template<>
void cWinRegisterInstaller::Make<true>(HKEY BaseKey,cString<wchar_t> Path)noexcept
{
	fErrorCode=Create(BaseKey,Path);
}
//---------------------------------------------------------------------------
template<>
void cWinRegisterInstaller::Make<false>(cWinRegisterInstaller &Parent,cString<wchar_t> Path)noexcept
{
	fErrorCode=Open(Parent,Path);
}
//---------------------------------------------------------------------------
template<>
void cWinRegisterInstaller::Make<true>(cWinRegisterInstaller &Parent,cString<wchar_t> Path)noexcept
{
	fErrorCode=Create(Parent,Path);
}
//---------------------------------------------------------------------------
void cWinRegisterInstaller::CloseTree(void)noexcept
{
	if(fKey==nullptr){
		return;
	}
	for(auto Item : fSubItems){
		auto Installer=cnMemory::GetObjectFromMemberPointer(Item,&cWinRegisterInstaller::fLinkItem);
		Installer->CloseTree();
	}
	fSubItems.RemoveAll();
	::RegCloseKey(fKey);

	fKey=nullptr;
	fBaseKey=nullptr;
	fPath=nullptr;
}
//---------------------------------------------------------------------------
bool cWinRegisterInstaller::DeleteTree(void)noexcept
{
	if(fKey==nullptr){
		switch(fErrorCode){
		case ERROR_SUCCESS:
		case ERROR_FILE_NOT_FOUND:
			return true;
		}
		return false;
	}
	for(auto Item : fSubItems){
		auto Installer=cnMemory::GetObjectFromMemberPointer(Item,&cWinRegisterInstaller::fLinkItem);
		Installer->DeleteTree();
	}
	fSubItems.RemoveAll();
	::RegCloseKey(fKey);
	bool Success=true;
	if(fErrorCode!=ERROR_ALREADY_EXISTS){
		LSTATUS ret=::RegDeleteKeyW(fBaseKey,fPath);
		if(ret!=ERROR_SUCCESS)
			Success=false;
	}

	fKey=nullptr;
	fBaseKey=nullptr;
	fPath=nullptr;
	return true;
}
//---------------------------------------------------------------------------
