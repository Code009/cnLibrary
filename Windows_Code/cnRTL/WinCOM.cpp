#include "WinCOM.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

#pragma comment(lib,"OleAut32.lib")

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cBSTROwnerOperator::Release(BSTR Token)noexcept(true)
{
	::SysFreeString(Token);
}
//---------------------------------------------------------------------------
apBSTR cnRTL::MakeBSTR(const wchar_t *Text)noexcept(true)
{
	BSTR Str=::SysAllocString(Text);
	if(Str==nullptr)
		return nullptr;

	return apBSTR::TakeFromManual(Str);
}
//---------------------------------------------------------------------------
BSTR* cnRTL::apBSTRRetPtr(apBSTR &Ptr)noexcept(true)
{
	Ptr=nullptr;
	return &Ptr.Pointer();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOLEModule::cOLEModule(const wchar_t *ModuleName)noexcept(true)
	: fModuleName(ModuleName)
{
	fModuleHandle=nullptr;
	fLoadFailed=false;
}
//---------------------------------------------------------------------------
cOLEModule::~cOLEModule()noexcept(true)
{
	if(fModuleHandle!=nullptr){
		::FreeLibrary(fModuleHandle);
	}
}
//---------------------------------------------------------------------------
bool cOLEModule::SetupModule(void)noexcept(true)
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
HRESULT cOLEModule::QueryFactory(REFCLSID rclsid,IClassFactory **Factory)noexcept(true)
{
	if(SetupModule()==false)
		return E_NOTIMPL;

	return fGetClassObjectProc(rclsid,IID_IClassFactory,reinterpret_cast<void**>(Factory));
}
//---------------------------------------------------------------------------
HRESULT cOLEModule::CreateObject(REFCLSID rclsid,IUnknown *pUnkOuter, REFIID riid,LPVOID *ppvObj)noexcept(true)
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
COMPtr<IClassFactory> cOLEModule::QueryFactory(REFCLSID rclsid)noexcept(true)
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
cCOMApartmentThreading::cCOMApartmentThreading(aCls<cWinTLS> *TLS,iDispatch *Dispatch)noexcept(true)
	: fTLS(TLS)
	, fDispatch(Dispatch)
{
	fRefCount=1;
}
//---------------------------------------------------------------------------
cCOMApartmentThreading::~cCOMApartmentThreading()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cCOMApartmentThreading::IncreaseReference(void)noexcept(true)
{
	fRefCount.Free++;
}
//---------------------------------------------------------------------------
void cCOMApartmentThreading::DecreaseReference(void)noexcept(true)
{
	if(--fRefCount.Free==0){
		fDispatch->Execute(nullptr,this);
	}
}
//---------------------------------------------------------------------------
iDispatch* cCOMApartmentThreading::GetDispatch(void)noexcept(true)
{
	return fDispatch;
}
//---------------------------------------------------------------------------
void cCOMApartmentThreading::OnDelete(void)noexcept(true)
{
	fTLS->Set(nullptr);
	::CoUninitialize();
}
//---------------------------------------------------------------------------
void cCOMApartmentThreading::Execute(void)noexcept(true)
{
	OnDelete();
	delete this;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCOMApartmentThreadingModule::cThreading::cThreading(aCls<cWinTLS> *TLS,iDispatch *Dispatch)noexcept(true)
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
iDispatch* cCOMApartmentThreadingModule::cThreading::GetDispatch(void)noexcept(true)
{
	return cCOMApartmentThreading::GetDispatch();
}
//---------------------------------------------------------------------------
void cCOMApartmentThreadingModule::cThreading::Execute(void)noexcept(true)
{
	OnDelete();
	delete this;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCOMApartmentThreadingModule::cCOMApartmentThreadingModule()noexcept(true)
{
	fTLS=aClsCreate<cWinTLS>();
}
//---------------------------------------------------------------------------
cCOMApartmentThreadingModule::~cCOMApartmentThreadingModule()noexcept(true)
{
}
//---------------------------------------------------------------------------
cCOMApartmentThreadingModule::cThreading* cCOMApartmentThreadingModule::Get(void)noexcept(true)
{
	return static_cast<cThreading*>(fTLS->Get());
}
//---------------------------------------------------------------------------
void cCOMApartmentThreadingModule::cDispatchInitialize::Execute(void)noexcept(true)
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
rPtr<cCOMApartmentThreadingModule::cThreading> cCOMApartmentThreadingModule::Query(iDispatch *Dispatch)noexcept(true)
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
cCOMApartmentThreading* cCOMApartmentThreadingModule::GetThreading(void)noexcept(true)
{
	return Get();
}
//---------------------------------------------------------------------------
rPtr<cCOMApartmentThreading> cCOMApartmentThreadingModule::QueryThreading(iDispatch *Dispatch)noexcept(true)
{
	return Query(Dispatch);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinRegisterInstaller::cWinRegisterInstaller()noexcept(true)
	: fKey(nullptr)
	, fErrorCode(ERROR_SUCCESS)
{

}
//---------------------------------------------------------------------------
cWinRegisterInstaller::~cWinRegisterInstaller()noexcept(true)
{
	CloseTree();
}
//---------------------------------------------------------------------------
cWinRegisterInstaller::operator HKEY()const noexcept(true)
{
	return fKey;
}
//---------------------------------------------------------------------------
DWORD cWinRegisterInstaller::GetMakeError(void)const noexcept(true)
{
	return fErrorCode;
}
//---------------------------------------------------------------------------
bool cWinRegisterInstaller::WasnotCreated(void)const noexcept(true)
{
	return fErrorCode==ERROR_ALREADY_EXISTS;
}
//---------------------------------------------------------------------------
LSTATUS cWinRegisterInstaller::Create(HKEY BaseKey,cString<wchar_t> Path,REGSAM samDesired)noexcept(true)
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
LSTATUS cWinRegisterInstaller::Open(HKEY BaseKey,cString<wchar_t> Path,REGSAM samDesired)noexcept(true)
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
LSTATUS cWinRegisterInstaller::Create(cWinRegisterInstaller &Parent,cString<wchar_t> Path,REGSAM samDesired)noexcept(true)
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
LSTATUS cWinRegisterInstaller::Open(cWinRegisterInstaller &Parent,cString<wchar_t> Path,REGSAM samDesired)noexcept(true)
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
void cWinRegisterInstaller::Make<false>(HKEY BaseKey,cString<wchar_t> Path)noexcept(true)
{
	fErrorCode=Open(BaseKey,Path);
}
//---------------------------------------------------------------------------
template<>
void cWinRegisterInstaller::Make<true>(HKEY BaseKey,cString<wchar_t> Path)noexcept(true)
{
	fErrorCode=Create(BaseKey,Path);
}
//---------------------------------------------------------------------------
template<>
void cWinRegisterInstaller::Make<false>(cWinRegisterInstaller &Parent,cString<wchar_t> Path)noexcept(true)
{
	fErrorCode=Open(Parent,Path);
}
//---------------------------------------------------------------------------
template<>
void cWinRegisterInstaller::Make<true>(cWinRegisterInstaller &Parent,cString<wchar_t> Path)noexcept(true)
{
	fErrorCode=Create(Parent,Path);
}
//---------------------------------------------------------------------------
void cWinRegisterInstaller::CloseTree(void)noexcept(true)
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
bool cWinRegisterInstaller::DeleteTree(void)noexcept(true)
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
