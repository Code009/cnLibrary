#include <cnRTL\WinSDKHeader.h>
#include <objidl.h>

#include "WinCOMShell.h"

#pragma comment(lib,"Shlwapi.lib")

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

#pragma message (cnLib_FILE_LINE " : TODO - Fix or remove ItemID Compare")

cnLib_INTERFACE_LOCALID_DEFINE(cWinShellFileName);

//---------------------------------------------------------------------------
ufInt16 cnWinRTL::ShellChildItemIDGetSize(const ITEMID_CHILD *ChildID)noexcept
{
	return ChildID->mkid.cb;
}
//---------------------------------------------------------------------------
ITEMID_CHILD* cnWinRTL::ShellChildItemIDDuplicate(const ITEMID_CHILD *ChildID)noexcept
{
	if(ChildID==nullptr)
		return nullptr;
	uIntn IDSize=ShellChildItemIDGetSize(ChildID);
	auto *DupID=static_cast<ITEMID_CHILD*>(::CoTaskMemAlloc(IDSize+2));
	cnMemory::Copy(DupID,ChildID,IDSize);
	cnMemory::PointerAddByteOffset(DupID,IDSize)->mkid.cb=0;
	return DupID;
}
//---------------------------------------------------------------------------
uIntn cnWinRTL::ShellItemIDListGetSize(const ITEMIDLIST *IDList)noexcept
{
	uIntn TotalSize=0;
	uInt16 CurSize;
	while((CurSize=IDList->mkid.cb)!=0){
		TotalSize+=CurSize;

		IDList=cnMemory::PointerAddByteOffset(IDList,CurSize);
	}
	return TotalSize;
}
//---------------------------------------------------------------------------
ITEMIDLIST* cnWinRTL::ShellItemIDListMakeCopy(const ITEMIDLIST *IDList,uIntn IDListSize)noexcept
{
	auto *DupID=static_cast<ITEMIDLIST*>(::CoTaskMemAlloc(IDListSize+2));
	cnMemory::Copy(DupID,IDList,IDListSize);
	cnMemory::PointerAddByteOffset(DupID,IDListSize)->mkid.cb=0;
	return DupID;
}
//---------------------------------------------------------------------------
ITEMIDLIST* cnWinRTL::ShellItemIDListDuplicate(const ITEMIDLIST *IDList,uIntn &IDListSize)noexcept
{
	if(IDList==nullptr){
		IDListSize=0;
		return nullptr;
	}
	IDListSize=ShellItemIDListGetSize(IDList);
	return ShellItemIDListMakeCopy(IDList,IDListSize);
}
//---------------------------------------------------------------------------
ITEMIDLIST* cnWinRTL::ShellItemIDListDuplicateChildID(const ITEMID_CHILD *ChildID)noexcept
{
	if(ChildID==nullptr)
		return nullptr;
	return ShellItemIDListMakeCopy(ChildID,ChildID->mkid.cb);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cShellString::cShellString()noexcept
	: fString(nullptr)
{
}
//---------------------------------------------------------------------------
cShellString::~cShellString()noexcept
{
	Clear();
}
//---------------------------------------------------------------------------
void cShellString::Clear(void)noexcept
{
	if(fString!=nullptr){
		::CoTaskMemFree(fString);
		fString=nullptr;
	}
}
//---------------------------------------------------------------------------
LPWSTR* cShellString::RetPtr(void)noexcept
{
	Clear();
	return &fString;
}
//---------------------------------------------------------------------------
cShellString::operator LPWSTR ()const noexcept
{
	return fString;
}
//---------------------------------------------------------------------------
LPWSTR cShellString::GetString(void)const noexcept
{
	return fString;
}
//---------------------------------------------------------------------------
LPWSTR cShellString::Duplicate(const wchar_t *String)noexcept
{
	if(String==nullptr)
		return nullptr;
	uIntn StrLength=cnString::FindLength(String);
	auto *DupStr=static_cast<LPWSTR>(::CoTaskMemAlloc(StrLength*2));
	cnMemory::Copy(DupStr,String,StrLength);
	return DupStr;
}
//---------------------------------------------------------------------------
cShellString::cShellString(const wchar_t *String)noexcept
	: fString(Duplicate(String))
{
}
//---------------------------------------------------------------------------
cShellString::cShellString(const cShellString &ShellString)noexcept
	: fString(Duplicate(ShellString.fString))
{
}
//---------------------------------------------------------------------------
cShellString::cShellString(cShellString &&ShellString)noexcept
	: fString(ShellString.fString)
{
	ShellString.fString=nullptr;
}
//---------------------------------------------------------------------------
cShellString& cShellString::operator =(const wchar_t *String)noexcept
{
	if(fString!=nullptr){
		::CoTaskMemFree(fString);
	}
	fString=Duplicate(String);
	return *this;
}
//---------------------------------------------------------------------------
cShellString& cShellString::operator =(const cShellString &ShellString)noexcept
{
	if(fString!=nullptr){
		::CoTaskMemFree(fString);
	}
	fString=Duplicate(ShellString.fString);
	return *this;
}
//---------------------------------------------------------------------------
cShellString& cShellString::operator =(cShellString &&ShellString)noexcept
{
	if(fString!=nullptr){
		::CoTaskMemFree(fString);
	}
	fString=ShellString.fString;
	ShellString.fString=nullptr;
	return *this;
}
//---------------------------------------------------------------------------
HRESULT cShellString::FromStrRet(STRRET &&StrRet,PCUITEMID_CHILD ChildItemID)noexcept
{
	Clear();
	return StrRetToStrW(&StrRet,ChildItemID,&fString);
}
//---------------------------------------------------------------------------
sfInt8 cShellString::Compare(const cShellString &ShellString)const noexcept
{
	if(fString==nullptr){
		if(ShellString.fString==nullptr)
			return 0;
		return -1;
	}
	if(ShellString.fString==nullptr)
		return 1;
	return cnString::Compare(fString,ShellString);
}
//---------------------------------------------------------------------------
bool cShellString::operator ==(const cShellString &ShellString)const noexcept
{
	return Compare(ShellString)==0;
}
//---------------------------------------------------------------------------
bool cShellString::operator !=(const cShellString &ShellString)const noexcept
{
	return Compare(ShellString)!=0;
}
//---------------------------------------------------------------------------
bool cShellString::operator <(const cShellString &ShellString)const noexcept
{
	return Compare(ShellString)<0;
}
//---------------------------------------------------------------------------
bool cShellString::operator <=(const cShellString &ShellString)const noexcept
{
	return Compare(ShellString)<=0;
}
//---------------------------------------------------------------------------
bool cShellString::operator >(const cShellString &ShellString)const noexcept
{
	return Compare(ShellString)>0;
}
//---------------------------------------------------------------------------
bool cShellString::operator >=(const cShellString &ShellString)const noexcept
{
	return Compare(ShellString)>=0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cShellChildItemID::cShellChildItemID()noexcept
	: fChildID(nullptr)
{
}
//---------------------------------------------------------------------------
cShellChildItemID::~cShellChildItemID()noexcept
{
	Clear();
}
//---------------------------------------------------------------------------
cShellChildItemID::cShellChildItemID(ITEMID_CHILD &&ChildID)noexcept
	: fChildID(&ChildID)
{
}
//---------------------------------------------------------------------------
cShellChildItemID cShellChildItemID::TakeChildID(ITEMID_CHILD *ChildID)noexcept
{
	return cShellChildItemID(static_cast<ITEMID_CHILD&&>(*ChildID));
}
//---------------------------------------------------------------------------
void cShellChildItemID::Clear(void)noexcept
{
	if(fChildID!=nullptr){
		::CoTaskMemFree(fChildID);
		fChildID=nullptr;
	}
}
//---------------------------------------------------------------------------
LPITEMIDLIST* cShellChildItemID::RetPtr(void)noexcept
{
	Clear();
	return &fChildID;
}
//---------------------------------------------------------------------------
cShellChildItemID::operator const ITEMID_CHILD *()const noexcept
{
	return fChildID;
}
//---------------------------------------------------------------------------
uIntn cShellChildItemID::GetIDSize(void)const noexcept
{
	if(fChildID==nullptr)
		return 0;
	return fChildID->mkid.cb;
}
//---------------------------------------------------------------------------
cShellChildItemID::cShellChildItemID(const ITEMID_CHILD *ChildID)noexcept
	: fChildID(ShellChildItemIDDuplicate(ChildID))
{
}
//---------------------------------------------------------------------------
cShellChildItemID::cShellChildItemID(const cShellChildItemID &ID)noexcept
	: fChildID(ShellChildItemIDDuplicate(ID.fChildID))
{
}
//---------------------------------------------------------------------------
cShellChildItemID::cShellChildItemID(cShellChildItemID &&ID)noexcept
	: fChildID(ID.fChildID)
{
	ID.fChildID=nullptr;
}
//---------------------------------------------------------------------------
cShellChildItemID& cShellChildItemID::operator =(ITEMID_CHILD *ChildID)noexcept
{
	if(fChildID!=nullptr){
		::CoTaskMemFree(fChildID);
	}
	fChildID=ShellChildItemIDDuplicate(ChildID);
	return *this;
}
//---------------------------------------------------------------------------
cShellChildItemID& cShellChildItemID::operator =(const cShellChildItemID &ID)noexcept
{
	if(fChildID!=nullptr){
		::CoTaskMemFree(fChildID);
	}
	fChildID=ShellChildItemIDDuplicate(ID.fChildID);
	return *this;
}
//---------------------------------------------------------------------------
cShellChildItemID& cShellChildItemID::operator =(cShellChildItemID &&ID)noexcept
{
	if(fChildID!=nullptr){
		::CoTaskMemFree(fChildID);
	}
	fChildID=ID.fChildID;
	ID.fChildID=nullptr;
	return *this;
}
//---------------------------------------------------------------------------
sfInt8 cShellChildItemID::Compare(const ITEMID_CHILD *ChildID)const noexcept
{
	if(fChildID==nullptr){
		if(ChildID==nullptr)
			return 0;
		return -1;
	}
	if(ChildID==nullptr){
		return 1;
	}

	uIntn Size=ShellChildItemIDGetSize(fChildID);
	uIntn CmpSize=ShellChildItemIDGetSize(ChildID);
	if(Size<CmpSize)
		return -1;
	if(Size>CmpSize)
		return 1;
	return cnMemory::Compare(fChildID,ChildID,Size);
}
//---------------------------------------------------------------------------
bool cShellChildItemID::operator ==(const ITEMID_CHILD *ChildID)const noexcept
{
	return Compare(ChildID)==0;
}
//---------------------------------------------------------------------------
bool cShellChildItemID::operator !=(const ITEMID_CHILD *ChildID)const noexcept
{
	return Compare(ChildID)!=0;
}
//---------------------------------------------------------------------------
bool cShellChildItemID::operator <(const ITEMID_CHILD *ChildID)const noexcept
{
	return Compare(ChildID)<0;
}
//---------------------------------------------------------------------------
bool cShellChildItemID::operator <=(const ITEMID_CHILD *ChildID)const noexcept
{
	return Compare(ChildID)<=0;
}
//---------------------------------------------------------------------------
bool cShellChildItemID::operator >(const ITEMID_CHILD *ChildID)const noexcept
{
	return Compare(ChildID)>0;
}
//---------------------------------------------------------------------------
bool cShellChildItemID::operator >=(const ITEMID_CHILD *ChildID)const noexcept
{
	return Compare(ChildID)>=0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cShellItemIDList::cShellItemIDList()noexcept
	: fIDList(nullptr)
	, fCachedIDListSize(IndexNotFound)
{
}
//---------------------------------------------------------------------------
cShellItemIDList::~cShellItemIDList()noexcept
{
	Clear();
}
//---------------------------------------------------------------------------
cShellItemIDList::cShellItemIDList(ITEMIDLIST &&IDList)noexcept
	: fIDList(&IDList)
	, fCachedIDListSize(IndexNotFound)
{
}
//---------------------------------------------------------------------------
cShellItemIDList cShellItemIDList::TakeIDList(ITEMIDLIST *IDList)noexcept
{
	return cShellItemIDList(static_cast<ITEMIDLIST&&>(*IDList));
}
//---------------------------------------------------------------------------
void cShellItemIDList::Clear(void)noexcept
{
	if(fIDList!=nullptr){
		::CoTaskMemFree(fIDList);
		fCachedIDListSize=0;
		fIDList=nullptr;
	}
}
//---------------------------------------------------------------------------
LPITEMIDLIST* cShellItemIDList::RetPtr(void)noexcept
{
	Clear();
	fCachedIDListSize=IndexNotFound;
	return &fIDList;
}
//---------------------------------------------------------------------------
cShellItemIDList::operator const ITEMIDLIST* ()const noexcept
{
	return fIDList;
}
//---------------------------------------------------------------------------
uIntn cShellItemIDList::GetIDSize(void)const noexcept
{
	if(fCachedIDListSize==IndexNotFound){
		if(fIDList==nullptr){
			fCachedIDListSize=0;
		}
		else{
			fCachedIDListSize=ShellItemIDListGetSize(fIDList);
		}
	}
	return fCachedIDListSize;
}
//---------------------------------------------------------------------------
cShellItemIDList::cShellItemIDList(const ITEMIDLIST *IDList)noexcept
	: fIDList(ShellItemIDListDuplicate(IDList,fCachedIDListSize))
{
}
//---------------------------------------------------------------------------
cShellItemIDList::cShellItemIDList(const cShellItemIDList &ID)noexcept
{
	if(ID.fIDList!=nullptr){
		fCachedIDListSize=ID.fCachedIDListSize;
		if(fCachedIDListSize==IndexNotFound){
			fIDList=ShellItemIDListDuplicate(ID.fIDList,fCachedIDListSize);
		}
		else{
			fIDList=ShellItemIDListMakeCopy(ID.fIDList,fCachedIDListSize);
		}
	}
	else{
		fIDList=nullptr;
		fCachedIDListSize=0;
	}
}
//---------------------------------------------------------------------------
cShellItemIDList::cShellItemIDList(cShellItemIDList &&ID)noexcept
	: fIDList(ID.fIDList)
	, fCachedIDListSize(ID.fCachedIDListSize)
{
	ID.fIDList=nullptr;
}
//---------------------------------------------------------------------------
cShellItemIDList& cShellItemIDList::operator =(ITEMIDLIST *IDList)noexcept
{
	if(fIDList!=nullptr){
		::CoTaskMemFree(fIDList);
	}
	fIDList=ShellItemIDListDuplicate(IDList,fCachedIDListSize);
	return *this;
}
//---------------------------------------------------------------------------
cShellItemIDList& cShellItemIDList::operator =(const cShellItemIDList &ID)noexcept
{
	if(fIDList!=nullptr){
		::CoTaskMemFree(fIDList);
	}
	if(ID.fIDList!=nullptr){
		fCachedIDListSize=ID.fCachedIDListSize;
		if(fCachedIDListSize==IndexNotFound){
			fIDList=ShellItemIDListDuplicate(ID.fIDList,fCachedIDListSize);
		}
		else{
			fIDList=ShellItemIDListMakeCopy(ID.fIDList,fCachedIDListSize);
		}
	}
	else{
		fIDList=nullptr;
	}
	return *this;
}
//---------------------------------------------------------------------------
cShellItemIDList& cShellItemIDList::operator =(cShellItemIDList &&ID)noexcept
{
	if(fIDList!=nullptr){
		::CoTaskMemFree(fIDList);
	}
	fCachedIDListSize=ID.fCachedIDListSize;
	fIDList=ID.fIDList;
	ID.fIDList=nullptr;
	return *this;
}
//---------------------------------------------------------------------------
cShellItemIDList::cShellItemIDList(const cShellChildItemID &ChildID)noexcept
	: fIDList(ShellItemIDListDuplicateChildID(ChildID))
	, fCachedIDListSize(ChildID.GetIDSize())
{
}
//---------------------------------------------------------------------------
cShellItemIDList& cShellItemIDList::operator =(const cShellChildItemID &ChildID)noexcept
{
	if(fIDList!=nullptr){
		::CoTaskMemFree(fIDList);
	}
	if(ChildID!=nullptr){
		fIDList=ShellItemIDListDuplicateChildID(ChildID);
		fCachedIDListSize=ChildID.GetIDSize();
	}
	else{
		fIDList=nullptr;
		fCachedIDListSize=0;
	}
	return *this;
}
//---------------------------------------------------------------------------
sfInt8 cShellItemIDList::Compare(const ITEMIDLIST *IDList)const noexcept
{
	if(fIDList==nullptr){
		if(IDList==nullptr)
			return 0;
		return -1;
	}
	if(IDList==nullptr){
		return 1;
	}

	uIntn ThisIDSize=GetIDSize();
	uIntn DestIDSize=ShellItemIDListGetSize(IDList);
	if(ThisIDSize<DestIDSize)
		return -1;
	if(ThisIDSize>DestIDSize)
		return 1;
	return cnMemory::Compare(fIDList,IDList,ThisIDSize);
}
//---------------------------------------------------------------------------
bool cShellItemIDList::operator ==(const ITEMIDLIST *IDList)const noexcept
{
	return Compare(IDList)==0;
}
//---------------------------------------------------------------------------
bool cShellItemIDList::operator !=(const ITEMIDLIST *IDList)const noexcept
{
	return Compare(IDList)!=0;
}
//---------------------------------------------------------------------------
bool cShellItemIDList::operator <(const ITEMIDLIST *IDList)const noexcept
{
	return Compare(IDList)<0;
}
//---------------------------------------------------------------------------
bool cShellItemIDList::operator <=(const ITEMIDLIST *IDList)const noexcept
{
	return Compare(IDList)<=0;
}
//---------------------------------------------------------------------------
bool cShellItemIDList::operator >(const ITEMIDLIST *IDList)const noexcept
{
	return Compare(IDList)>0;
}
//---------------------------------------------------------------------------
bool cShellItemIDList::operator >=(const ITEMIDLIST *IDList)const noexcept
{
	return Compare(IDList)>=0;
}
//---------------------------------------------------------------------------
void cShellItemIDList::AppendChildID(ITEMID_CHILD *ChildID)noexcept
{
	auto CurSize=GetIDSize();
	uIntn NewSize=CurSize+ChildID->mkid.cb;
	fIDList=static_cast<ITEMIDLIST*>(::CoTaskMemRealloc(fIDList,NewSize+2));
	cnMemory::Copy(cnMemory::PointerAddByteOffset(fIDList,CurSize),ChildID,ChildID->mkid.cb);
	cnMemory::PointerAddByteOffset(fIDList,NewSize)->mkid.cb=0;
	fCachedIDListSize=NewSize;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinShellFileName::cWinShellFileName(iPtr<iWindow> HostWindow,rPtr<iCOMApartmentThreading> Threading,COMPtr<IShellFolder> Parent,cShellItemIDList ChildIDList)noexcept
	: fHostWindow(HostWindow)
	, fThreading(cnVar::MoveCast(Threading))
	, fParentFolder(cnVar::MoveCast(Parent))
	, fChildIDList(cnVar::MoveCast(ChildIDList))
{
	fCachedItemAttributes=false;
	fCachedFileInfo=false;
}
//---------------------------------------------------------------------------
cWinShellFileName::~cWinShellFileName()noexcept
{
}
//---------------------------------------------------------------------------
eiOrdering cWinShellFileName::Compare(iFileName *Dest)noexcept
{
	auto DestShellFileName=iCast<cWinShellFileName>(Dest);
	if(DestShellFileName==nullptr)
		return iOrdering::Different;

	HRESULT hr=fParentFolder->CompareIDs(0,fChildIDList,DestShellFileName->fChildIDList);
	if(FAILED(hr))
		return iOrdering::Different;
		
	
	SHORT Result=HRESULT_CODE(hr);
	if(Result==0)
		return iOrdering::Equal;
	if(Result<0)
		return iOrdering::Less;
	return iOrdering::Greater;
}
//---------------------------------------------------------------------------
HRESULT cWinShellFileName::SetupFolder(void)noexcept
{
	if(fChildFolder!=nullptr)
		return S_OK;

	HRESULT hr;

	hr=fParentFolder->BindToObject(fChildIDList,nullptr,IID_IShellFolder,COMRetPtr(fChildFolder));

	return hr;
}
//---------------------------------------------------------------------------
rPtr<iFileEnumerator> cWinShellFileName::Enum(void)noexcept
{
	if(fParentFolder==nullptr)
		return nullptr;

	HRESULT hr;

	hr=SetupFolder();
	if(FAILED(hr))
		return nullptr;

	COMPtr<IEnumIDList> IDEnum;
	SHCONTF Flags=SHCONTF_FOLDERS|SHCONTF_NONFOLDERS;
	HWND HostWindowHandle;
	if(fHostWindow!=nullptr){
		HostWindowHandle=fHostWindow->GetWindowHandle();
	}
	else{
		HostWindowHandle=nullptr;
	}
	hr=fChildFolder->EnumObjects(HostWindowHandle,Flags,COMRetPtrT(IDEnum));
	if(hr==S_OK){
		return rCreate<cWinShellFileNameEnum>(fHostWindow,fThreading,fChildFolder,cnVar::MoveCast(IDEnum));
	}

	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iFileEnumerator> cWinShellFileName::EnumFolder(void)noexcept
{
	return Enum();
}
//---------------------------------------------------------------------------
rPtr<iAsyncNotification> cWinShellFileName::WatchFolder(void)noexcept
{
	if(fParentFolder==nullptr)
		return nullptr;

	HRESULT hr;
	hr=SetupFolder();
	if(FAILED(hr))
		return nullptr;

	return rCreate<cWinShellFolderItemWatcher>(fHostWindow,fThreading,fChildFolder);
}
//---------------------------------------------------------------------------
rPtr<iFileObserver> cWinShellFileName::ObserveFolder(void)noexcept
{
	if(fParentFolder==nullptr)
		return nullptr;

	HRESULT hr;
	hr=SetupFolder();
	if(FAILED(hr))
		return nullptr;

	return rCreate<cWinShellFolderItemObserver>(fHostWindow,fThreading,fChildFolder);
}
//---------------------------------------------------------------------------
iPtr<iFileSyncStream>	cWinShellFileName::OpenSequentialStream(eFileAccess AccessMode,eFileCreate CreateFlag)noexcept
{
	if(fParentFolder==nullptr)
		return nullptr;

	auto ThreadingDispatch=fThreading->GetDispatch();
	if(ThreadingDispatch->IsCurrentThread()){
		HRESULT hr;
		COMPtr<IStream> FileStream;
		hr=fParentFolder->BindToObject(fChildIDList,nullptr,IID_IStream,COMRetPtr(FileStream));
		if(hr==S_OK){
			return iCreate<cCOMSyncStream>(cnVar::MoveCast(FileStream));
		}

		return nullptr;
	}

	auto CurThreading=cnWindows::COMApartmentThreadingGet();
	if(CurThreading==nullptr)
		return nullptr;

	IStream *MarshallStream;
	cFunction<void(void)noexcept> MarshallProc=(ThreadingDispatch,[this,&MarshallStream]{
		::CoMarshalInterThreadInterfaceInStream(IID_IShellFolder,fParentFolder,&MarshallStream);
	});
	ThreadingDispatch->ExecuteSync(MarshallProc);

	if(MarshallStream==nullptr)
		return nullptr;

	COMPtr<IShellFolder> FolderThreaded;
	::CoGetInterfaceAndReleaseStream(MarshallStream,IID_IShellFolder,COMRetPtr(FolderThreaded));
	if(FolderThreaded==nullptr)
		return nullptr;


	HRESULT hr;
	COMPtr<IStream> FileStream;
	hr=fParentFolder->BindToStorage(fChildIDList,nullptr,IID_IStream,COMRetPtr(FileStream));
	if(hr==S_OK){
		return iCreate<cCOMSyncStream>(cnVar::MoveCast(FileStream));
	}


	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iFileStream>		cWinShellFileName::OpenFileStream(eFileAccess AccessMode,eFileCreate CreateFlag)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cWinShellFileName::IsExists(void)noexcept
{
	SetupAttributes();
	return fCachedFileExists;
}
//---------------------------------------------------------------------------
uInt64 cWinShellFileName::GetDataSize(void)noexcept
{
	if(SetupFileInfo()==false)
		return 0;

	uInt64 FileSize=fFileInfo.nFileSizeHigh;
	FileSize<<=32;
	FileSize+=fFileInfo.nFileSizeLow;
	return FileSize;
}
//---------------------------------------------------------------------------
void cWinShellFileName::SetupAttributes(void)noexcept
{
	HRESULT hr;

	if(fCachedItemAttributes==false){
		LPCITEMIDLIST IDList=fChildIDList;
		SFGAOF Attributes=SFGAO_STREAM|SFGAO_FOLDER|SFGAO_FILESYSTEM;
		hr=fParentFolder->GetAttributesOf(1,&IDList,&Attributes);
		if(FAILED(hr)){
			fItemAttributes=0;
			fCachedFileExists=false;
		}
		else{
			fCachedFileExists=true;
		}
		fItemAttributes=Attributes;

		fCachedItemAttributes=true;
	}
}
//---------------------------------------------------------------------------
bool cWinShellFileName::SetupFileInfo(void)noexcept
{
	if(fCachedFileInfo)
		return true;

	HRESULT hr;
	COMPtr<IDataObject> DataObj;
	LPCITEMIDLIST IDList=fChildIDList;
	hr=fParentFolder->GetUIObjectOf(fHostWindow->GetWindowHandle(),1,&IDList,IID_IDataObject,nullptr,COMRetPtr(DataObj));
	if(FAILED(hr)){
		return false;
	}
	FORMATETC fmc;


	fmc.cfFormat=::RegisterClipboardFormatW(CFSTR_FILEDESCRIPTORW);
	fmc.ptd=nullptr;
	fmc.dwAspect=DVASPECT_CONTENT;
	fmc.lindex=-1;
	fmc.tymed=TYMED_HGLOBAL;

	hr=DataObj->QueryGetData(&fmc);

	STGMEDIUM fninfo;
	cnMemory::ZeroFill(fninfo);
	hr=DataObj->GetData(&fmc,&fninfo);

	auto p=static_cast<LPFILEGROUPDESCRIPTORW>(GlobalLock(fninfo.hGlobal));
	if(p!=nullptr){
		fFileInfo=p->fgd[0];
		fCachedFileInfo=true;

		GlobalUnlock(fninfo.hGlobal);
	}

	ReleaseStgMedium(&fninfo);
	return fCachedFileInfo;
}
//---------------------------------------------------------------------------
bool cWinShellFileName::HasData(void)noexcept
{
	SetupAttributes();
	if(fCachedFileExists==false)
		return false;
	return (fItemAttributes&SFGAO_STREAM)!=0;
}
//---------------------------------------------------------------------------
bool cWinShellFileName::HasFolder(void)noexcept
{
	SetupAttributes();
	if(fCachedFileExists==false)
		return false;
	return (fItemAttributes&SFGAO_FOLDER)!=0;
}
//---------------------------------------------------------------------------
bool cWinShellFileName::Delete(void)noexcept
{
#pragma message (cnLib_FILE_LINE ": TODO - Delete file")
	return false;
}
//---------------------------------------------------------------------------
iPtr<iFileName>	cWinShellFileName::ParentFileName(void)noexcept
{
	if(fParentFolder==nullptr)
		return nullptr;
	HRESULT hr;
	COMPtr<IShellFolder> ParentFolder;
	COMPtr<IPersistFolder2> PersistFolder2;
	hr=fParentFolder->QueryInterface(COMRetPtrT(PersistFolder2));
	if(SUCCEEDED(hr)){
		// get parent by absolute id
		cShellItemIDList DirIDList;
		hr=PersistFolder2->GetCurFolder(DirIDList.RetPtr());
		if(SUCCEEDED(hr)){
			PCUITEMID_CHILD ChildID;
			hr=SHBindToParent(DirIDList,IID_IShellFolder,COMRetPtr(ParentFolder),&ChildID);
			if(SUCCEEDED(hr)){
				return iCreate<cWinShellFileName>(fHostWindow,fThreading,cnVar::MoveCast(ParentFolder),cShellItemIDList(ChildID));
			}
		}
	}
	COMPtr<IParentAndItem> Parent;
	hr=fParentFolder->QueryInterface(COMRetPtrT(Parent));
	if(FAILED(hr))
		return nullptr;

	cShellItemIDList FolderID;
	hr=Parent->GetParentAndItem(nullptr,COMRetPtrT(ParentFolder),FolderID.RetPtr());
	if(FAILED(hr))
		return nullptr;

	return iCreate<cWinShellFileName>(fHostWindow,fThreading,cnVar::MoveCast(ParentFolder),cnVar::MoveCast(FolderID));
}
//---------------------------------------------------------------------------
rPtr<iFileEnumerator>	cWinShellFileName::EnumFileNames(const uChar16 *Filter)noexcept
{
	if(Filter!=nullptr)
		return nullptr;
	return Enum();
}
//---------------------------------------------------------------------------
void cWinShellFileName::SetupName(void)noexcept
{
	if(fName->Length!=0)
		return;

	if(SetupFileInfo()==false){
		return;
	}

	fName.SetString(wtou(fFileInfo.cFileName));
}
//---------------------------------------------------------------------------
const uChar16*	cWinShellFileName::GetName(void)noexcept
{
	SetupName();
	return fName;
}
//---------------------------------------------------------------------------
uIntn			cWinShellFileName::GetNameLength(void)noexcept
{
	SetupName();
	return fName->Length;
}
//---------------------------------------------------------------------------
iPtr<iFileName>	cWinShellFileName::MakeName(const uChar16*const *FilePath,uIntn Depth)noexcept
{
	return nullptr;
	// create file name
	//auto FileName=CreateWinFileName(fFileName,fFileName.GetLength(),FilePath,Depth);
	//if(FileName.GetLength()==0)
	//	return nullptr;
	//
	//return iCreate<cWinFileName>(FileName.Swap());
}
//---------------------------------------------------------------------------
bool cWinShellFileName::CreateFolder(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
uIntn cWinShellFileName::CreateFolderPath(void)noexcept
{
	return IndexNotFound;
}
//---------------------------------------------------------------------------
iPtr<iFileName> cWinShellFileName::CreateFileNameFromFileSystemFolderItem(IShellFolder *ParentFolder,LPCITEMIDLIST ChildIDList)noexcept
{
	HRESULT hr;
	STRRET Name;
	hr=ParentFolder->GetDisplayNameOf(ChildIDList,SHGDN_FORPARSING,&Name);
	if(FAILED(hr))
		return nullptr;

	cShellString FilePath;
	hr=FilePath.FromStrRet(cnVar::MoveCast(Name),ChildIDList);
	if(FAILED(hr)){
		return nullptr;
	}

	return cnWindows::CreateFileName(FilePath);
}
//---------------------------------------------------------------------------
iPtr<iFileName> cWinShellFileName::CreateFileNameFromFolderItem(iWindow *HostWindow,iCOMApartmentThreading *Threading,COMPtr<IShellFolder> ParentFolder,cShellItemIDList ChildIDList)noexcept
{
	HRESULT hr;

	SFGAOF Attribute=SFGAO_FILESYSTEM;
	LPCITEMIDLIST ItemID=ChildIDList;
	hr=ParentFolder->GetAttributesOf(1,&ItemID,&Attribute);
	if(SUCCEEDED(hr)){
		if(Attribute!=0){
			// create as native file
			auto FSFileName=CreateFileNameFromFileSystemFolderItem(ParentFolder,ChildIDList);
			if(FSFileName!=nullptr){
				return FSFileName;
			}
		}
	}
	// create shell file name

	return iCreate<cWinShellFileName>(HostWindow,Threading,cnVar::MoveCast(ParentFolder),cnVar::MoveCast(ChildIDList));
}
//---------------------------------------------------------------------------
iPtr<iFileName> cWinShellFileName::CreateFileNameFromShellItem(iWindow *HostWindow,iCOMApartmentThreading *Threading,COMPtr<IShellItem> ShellItem)noexcept
{
	HRESULT hr;
	COMPtr<IParentAndItem> ItemParent;
	hr=ShellItem->QueryInterface(COMRetPtrT(ItemParent));
	if(FAILED(hr))
		return nullptr;
	
	COMPtr<IShellFolder> ParentFolder;
	cShellItemIDList ChildIDList;
	hr=ItemParent->GetParentAndItem(nullptr,COMRetPtrT(ParentFolder),ChildIDList.RetPtr());
	if(FAILED(hr))
		return nullptr;

	return CreateFileNameFromFolderItem(HostWindow,Threading,cnVar::MoveCast(ParentFolder),cnVar::MoveCast(ChildIDList));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinShellFileNameEnum::cWinShellFileNameEnum(iPtr<iWindow> HostWindow,rPtr<iCOMApartmentThreading> Threading,COMPtr<IShellFolder> Folder,COMPtr<IEnumIDList> Enum)noexcept
	: fHostWindow(cnVar::MoveCast(HostWindow))
	, fThreading(cnVar::MoveCast(Threading))
	, fFolder(cnVar::MoveCast(Folder))
	, fEnum(cnVar::MoveCast(Enum))
{
}
//---------------------------------------------------------------------------
cWinShellFileNameEnum::~cWinShellFileNameEnum()noexcept
{
}
//---------------------------------------------------------------------------
bool cWinShellFileNameEnum::Fetch(void)noexcept
{
	HRESULT hr;
	ULONG Fetched;
	hr=fEnum->Next(1,fCurrentItemIDList.RetPtr(),&Fetched);
	if(hr!=S_OK){
		return false;
	}

	fCurrentFileName=cWinShellFileName::CreateFileNameFromFolderItem(fHostWindow,fThreading,fFolder,fCurrentItemIDList);

	return true;
}
//---------------------------------------------------------------------------
iFile* cWinShellFileNameEnum::GetCurrentFile(void)noexcept
{
	return fCurrentFileName;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcWinShellFolderItemObserver::bcWinShellFolderItemObserver(iPtr<iWindow> HostWindow,rPtr<iCOMApartmentThreading> Threading,COMPtr<IShellFolder> Folder)noexcept
	: fHostWindow(cnVar::MoveCast(HostWindow))
	, fThreading(cnVar::MoveCast(Threading))
	, fFolder(cnVar::MoveCast(Folder))
	, fFolderIDList(MakeFolderAbsoluteIDList(fFolder))
{
	fCallback=nullptr;
}
//---------------------------------------------------------------------------
bcWinShellFolderItemObserver::~bcWinShellFolderItemObserver()noexcept
{
}
//---------------------------------------------------------------------------
void bcWinShellFolderItemObserver::VirtualStopped(void)noexcept
{
	CloseQueue();

	InnerDecReference('self');
}
//---------------------------------------------------------------------------
cShellItemIDList bcWinShellFolderItemObserver::MakeFolderAbsoluteIDList(IShellFolder *Folder)noexcept
{
	cShellItemIDList AbsoluteID;
	// get absolute id for this folder
	HRESULT hr;
	
	COMPtr<IPersistFolder2> PersistFolder2;
	hr=Folder->QueryInterface(COMRetPtrT(PersistFolder2));
	if(FAILED(hr)){
		return AbsoluteID;
	}

	PersistFolder2->GetCurFolder(AbsoluteID.RetPtr());

	return AbsoluteID;
}
//---------------------------------------------------------------------------
void bcWinShellFolderItemObserver::cNotifyWindowHandler::WindowAttached(void)noexcept
{
}
//---------------------------------------------------------------------------
void bcWinShellFolderItemObserver::cNotifyWindowHandler::WindowDetached(void)noexcept
{
}
//---------------------------------------------------------------------------
bool bcWinShellFolderItemObserver::cNotifyWindowHandler::WindowMessage(LRESULT&,const cWindowMessageParam &)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
void bcWinShellFolderItemObserver::cNotifyWindowHandler::WindowMessageProcessed(LRESULT,const cWindowMessageParam &MsgParam)noexcept
{
	if(MsgParam.Code==WM_USER+1){
		// shell notification
		auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcWinShellFolderItemObserver::fNotifyWindowHandler);

		PIDLIST_ABSOLUTE *List;
		LONG Event;
		auto LockHandle=SHChangeNotification_Lock(reinterpret_cast<HANDLE>(MsgParam.wParam),static_cast<DWORD>(MsgParam.lParam),&List,&Event);
		
#if 1

		OutputDebugStringW(L"Notification:\n");

		for(uIntn li=0;li<2 && List[li]!=nullptr;li++){
			if(List[li]==nullptr)
				break;
			COMPtr<IShellItem> FileItem;
			SHCreateItemFromIDList(List[li],IID_IShellItem,COMRetPtr(FileItem));

			LPWSTR TempStr=nullptr;
			LPWSTR TempStr2=nullptr;
			LPCWSTR FileNameStr=L"";
			LPCWSTR FullFileNameStr=L"";
			if(FileItem!=nullptr){
				FileItem->GetDisplayName(SIGDN_NORMALDISPLAY,&TempStr);
				FileItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING,&TempStr2);
			}
			if(TempStr!=nullptr){
				FileNameStr=TempStr;
			}
			if(TempStr2!=nullptr){
				FullFileNameStr=TempStr2;
			}
			//cShellString TempStr;
			//STRRET FileName;
			//PIDLIST_RELATIVE ChildID;
			//PIDLIST_RELATIVE TempID;
			//ChildID=TempID=List[li];
			//while(TempID->mkid.cb!=0){
			//	ChildID=TempID;
			//	TempID=cnMemory::PointerAddByteOffset(TempID,TempID->mkid.cb);
			//}
			//Host->fFolder->GetDisplayNameOf(ChildID,SHGDN_NORMAL,&FileName);
			//TempStr.FromStrRet(cnVar::MoveCast(FileName),ChildID);
			auto FileMsg=CreateStringFormat(L"	File %d : %s (%s)\n",li,FileNameStr,FullFileNameStr);
			//OutputDebugStringW(FileMsg->Pointer);
			if(TempStr!=nullptr){
				::CoTaskMemFree(TempStr);
			}
			if(TempStr2!=nullptr){
				::CoTaskMemFree(TempStr2);
			}
		}
		
		if(Event&SHCNE_RENAMEITEM){
			OutputDebugStringW(L"	SHCNE_RENAMEITEM\n");
		}
		if(Event&SHCNE_CREATE){
			OutputDebugStringW(L"	SHCNE_CREATE\n");
		}
		if(Event&SHCNE_DELETE){
			OutputDebugStringW(L"	SHCNE_DELETE\n");
		}
		if(Event&SHCNE_MKDIR){
			OutputDebugStringW(L"	SHCNE_MKDIR\n");
		}
		if(Event&SHCNE_RMDIR){
			OutputDebugStringW(L"	SHCNE_RMDIR\n");
		}
		if(Event&SHCNE_MEDIAINSERTED){
			OutputDebugStringW(L"	SHCNE_MEDIAINSERTED\n");
		}
		if(Event&SHCNE_MEDIAREMOVED){
			OutputDebugStringW(L"	SHCNE_MEDIAREMOVED\n");
		}
		if(Event&SHCNE_DRIVEREMOVED){
			OutputDebugStringW(L"	SHCNE_DRIVEREMOVED\n");
		}
		if(Event&SHCNE_DRIVEADD){
			OutputDebugStringW(L"	SHCNE_DRIVEADD\n");
		}
		if(Event&SHCNE_NETSHARE){
			OutputDebugStringW(L"	SHCNE_NETSHARE\n");
		}
		if(Event&SHCNE_NETUNSHARE){
			OutputDebugStringW(L"	SHCNE_NETUNSHARE\n");
		}
		if(Event&SHCNE_ATTRIBUTES){
			OutputDebugStringW(L"	SHCNE_ATTRIBUTES\n");
		}
		if(Event&SHCNE_UPDATEDIR){
			OutputDebugStringW(L"	SHCNE_UPDATEDIR\n");
		}
		if(Event&SHCNE_UPDATEITEM){
			OutputDebugStringW(L"	SHCNE_UPDATEITEM\n");
		}
		if(Event&SHCNE_SERVERDISCONNECT){
			OutputDebugStringW(L"	SHCNE_SERVERDISCONNECT\n");
		}
		if(Event&SHCNE_UPDATEIMAGE){
			OutputDebugStringW(L"	SHCNE_UPDATEIMAGE\n");
		}
		if(Event&SHCNE_DRIVEADDGUI){
			OutputDebugStringW(L"	SHCNE_DRIVEADDGUI\n");
		}
		if(Event&SHCNE_RENAMEFOLDER){
			OutputDebugStringW(L"	SHCNE_RENAMEFOLDER\n");
		}
		if(Event&SHCNE_FREESPACE){
			OutputDebugStringW(L"	SHCNE_FREESPACE\n");
		}
		OutputDebugStringW(L"--------\n");
#endif // 0
		bool NeedUpdateState=Host->ShellNotifyProcess(Event,List);
		SHChangeNotification_Unlock(LockHandle);

		if(NeedUpdateState){
			Host->UpdateQueueState(false);
		}
	}
}
//---------------------------------------------------------------------------
void bcWinShellFolderItemObserver::UpdateFolderMonitor(void)noexcept
{
	if(IsNotificationStarted()){

		if(fFolderIDList!=nullptr){

			// create message window
			fNotifyWindow=cnWindows::CreateWindowHandle(HWND_MESSAGE,nullptr,0,0,0,0,0,0,0);
			if(fNotifyWindow!=nullptr){

				SHChangeNotifyEntry cne;
				cne.fRecursive=false;
				cne.pidl=fFolderIDList;
				LONG NotifyEvents=ShellNotifyMask();
				fNotifyToken=SHChangeNotifyRegister(fNotifyWindow->GetWindowHandle(),SHCNRF_ShellLevel|SHCNRF_NewDelivery,NotifyEvents,WM_USER+1,1,&cne);
				if(fNotifyToken==0){
					// registration failed
					fNotifyWindow=nullptr;
				}
				else{
					// register window message
					fNotifyWindow->InsertMessageHandler(&fNotifyWindowHandler);
				}
			}
		}
	}
	else{
		if(fNotifyWindow!=nullptr){
			fNotifyWindow->RemoveMessageHandler(&fNotifyWindowHandler);
			SHChangeNotifyDeregister(fNotifyToken);
			fNotifyWindow=nullptr;
		}
	}
}
//---------------------------------------------------------------------------
LONG bcWinShellFolderItemObserver::ShellNotifyMask(void)noexcept
{
	return SHCNE_RENAMEITEM|SHCNE_CREATE|SHCNE_DELETE|SHCNE_UPDATEITEM|SHCNE_ATTRIBUTES|
		SHCNE_MKDIR|SHCNE_RMDIR|SHCNE_RENAMEFOLDER|SHCNE_UPDATEDIR;
}
//---------------------------------------------------------------------------
void bcWinShellFolderItemObserver::cCreateNotifyWindowProc::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcWinShellFolderItemObserver::fCreateNotifyWindowProc);
	Host->UpdateFolderMonitor();
}
//---------------------------------------------------------------------------
void bcWinShellFolderItemObserver::NotifyFolderMonitor(void)noexcept
{
	if(fHostWindow!=nullptr){
		auto Thread=fHostWindow->GetUIThread();
		if(Thread->IsCurrentThread()){
			UpdateFolderMonitor();
		}
		else{
			auto Dispatch=Thread->GetDispatch(false);
			Dispatch->ExecuteAsync(&fInnerReference,&fCreateNotifyWindowProc);
		}
	}
}
//---------------------------------------------------------------------------
iReference* bcWinShellFolderItemObserver::NotificationInnerReference(void)noexcept
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void bcWinShellFolderItemObserver::NotificationStarted(void)noexcept
{
	bcAsyncQueue::NotificationStarted();

	fCallback->AsyncStarted();

	NotifyFolderMonitor();
}
//---------------------------------------------------------------------------
void bcWinShellFolderItemObserver::NotificationStopped(void)noexcept
{
	NotifyFolderMonitor();

	auto Reference=cnVar::MoveCast(fCallbackReference);
	auto Callback=fCallback;
	fCallback=nullptr;
	Callback->AsyncStopped();

	bcAsyncQueue::NotificationStopped();
}
//---------------------------------------------------------------------------
bcWinShellFolderItemObserver::CycleState bcWinShellFolderItemObserver::NotificationCheckState(void)noexcept
{
	return CycleState::Normal;
}
//---------------------------------------------------------------------------
void bcWinShellFolderItemObserver::AsyncQueueNotify(void)noexcept
{
	fCallback->AsyncNotify();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinShellFolderItemWatcher::cWinShellFolderItemWatcher(iPtr<iWindow> HostWindow,rPtr<iCOMApartmentThreading> Threading,COMPtr<IShellFolder> Folder)noexcept
	: bcWinShellFolderItemObserver(cnVar::MoveCast(HostWindow),cnVar::MoveCast(Threading),cnVar::MoveCast(Folder))
{
}
//---------------------------------------------------------------------------
cWinShellFolderItemWatcher::~cWinShellFolderItemWatcher()
{
}
//---------------------------------------------------------------------------
bool cWinShellFolderItemWatcher::cWinShellFolderItemWatcher::StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept
{
	if(PrepareStartNotify()==false)
		return false;

	fCallbackReference=Reference;
	fCallback=Callback;

	ConfirmStartNotify();
	UpdateQueueState(false);
	return true;
}
//---------------------------------------------------------------------------
void cWinShellFolderItemWatcher::cWinShellFolderItemWatcher::StopNotify(void)noexcept
{
	if(bcAsyncQueue::StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void cWinShellFolderItemWatcher::cWinShellFolderItemWatcher::NotifyCallback(bool IdleNotify)noexcept
{
	return NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool cWinShellFolderItemWatcher::cWinShellFolderItemWatcher::IsClosed(void)noexcept
{
	return IsNotificationClosed();
}
//---------------------------------------------------------------------------
void cWinShellFolderItemWatcher::NotificationStarted(void)noexcept
{
	rIncReference(this,'fiqu');

	bcWinShellFolderItemObserver::NotificationStarted();
}
//---------------------------------------------------------------------------
void cWinShellFolderItemWatcher::NotificationStopped(void)noexcept
{
	bcWinShellFolderItemObserver::NotificationStopped();
	
	rDecReference(this,'fiqu');
}
//---------------------------------------------------------------------------
bool cWinShellFolderItemWatcher::ShellNotifyProcess(LONG Event,PIDLIST_ABSOLUTE *List)noexcept
{
	const LONG CheckEvents=SHCNE_CREATE|SHCNE_DELETE|SHCNE_RENAMEITEM|SHCNE_UPDATEITEM|SHCNE_ATTRIBUTES
		|SHCNE_MKDIR|SHCNE_RMDIR|SHCNE_RENAMEFOLDER|SHCNE_UPDATEDIR;
	if(Event&CheckEvents){
		AsyncQueueSetAvailable(false);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinShellFolderItemObserver::cChangeEnum::cChangeEnum(cWinShellFolderItemObserver *Owner)noexcept
	: fOwner(Owner)
	, fCurrentIndex(IndexNotFound)
{
}
//---------------------------------------------------------------------------
cWinShellFolderItemObserver::cChangeEnum::~cChangeEnum()noexcept
{
	fOwner->ChangeEnumFinished();
}
//---------------------------------------------------------------------------
bool cWinShellFolderItemObserver::cChangeEnum::Fetch(void)noexcept
{
	uIntn NextIndex=fCurrentIndex+1;
	if(NextIndex>=ChangeList.GetCount()){
		return false;
	}
	fCurrentIndex=NextIndex;
	fCurrentFileName=nullptr;
	return true;
}
//---------------------------------------------------------------------------
iFile* cWinShellFolderItemObserver::cChangeEnum::GetCurrentFile(void)noexcept
{
	if(fCurrentFileName==nullptr){
		auto &Item=ChangeList[fCurrentIndex];
		fCurrentFileName=cWinShellFileName::CreateFileNameFromFolderItem(fOwner->fHostWindow,fOwner->fThreading,fOwner->fFolder,cnVar::MoveCast(Item.ItemID));
	}
	return fCurrentFileName;
}
//---------------------------------------------------------------------------
eFileChange cWinShellFolderItemObserver::cChangeEnum::GetChange(void)noexcept
{
	auto &Item=ChangeList[fCurrentIndex];
	return Item.Change;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinShellFolderItemObserver::cWinShellFolderItemObserver(iPtr<iWindow> HostWindow,rPtr<iCOMApartmentThreading> Threading,COMPtr<IShellFolder> Folder)noexcept
	: bcWinShellFolderItemObserver(cnVar::MoveCast(HostWindow),cnVar::MoveCast(Threading),cnVar::MoveCast(Folder))
{
	fChangeEnumInProgress=false;
}
//---------------------------------------------------------------------------
cWinShellFolderItemObserver::~cWinShellFolderItemObserver()noexcept
{
}
//---------------------------------------------------------------------------
bool cWinShellFolderItemObserver::StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept
{
	if(PrepareStartNotify()==false)
		return false;

	fCallbackReference=Reference;
	fCallback=Callback;

	ConfirmStartNotify();
	UpdateQueueState(false);
	return true;
}
//---------------------------------------------------------------------------
void cWinShellFolderItemObserver::StopNotify(void)noexcept
{
	if(bcAsyncQueue::StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void cWinShellFolderItemObserver::NotifyCallback(bool IdleNotify)noexcept
{
	return NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool cWinShellFolderItemObserver::IsClosed(void)noexcept
{
	return IsNotificationClosed();
}
//---------------------------------------------------------------------------
void cWinShellFolderItemObserver::DiscardChanges(void)noexcept
{
	//EnumFileDiscardChange();
}
//---------------------------------------------------------------------------
rPtr<iFileEnumerator> cWinShellFolderItemObserver::ResetChanges(void)noexcept
{
	if(fChangeEnumInProgress)
		return nullptr;

	HRESULT hr;
	SHCONTF Flags=SHCONTF_FOLDERS|SHCONTF_NONFOLDERS;
	COMPtr<IEnumIDList> FolderEnum;
	HWND HostWindowHandle;
	if(fHostWindow!=nullptr){
		HostWindowHandle=fHostWindow->GetWindowHandle();
	}
	else{
		HostWindowHandle=nullptr;
	}
	hr=fFolder->EnumObjects(HostWindowHandle,Flags,COMRetPtrT(FolderEnum));
	if(FAILED(hr)){
		// error
		return nullptr;
	}
	
	ULONG Fetched;
	PITEMID_CHILD ChildItemID[4];
	while(SUCCEEDED(hr=FolderEnum->Next(4,ChildItemID,&Fetched))){
		for(ULONG i=0;i<Fetched;i++){
			fCheckpointChildren.Insert(cShellChildItemID::TakeChildID(ChildItemID[i]));
		}
		if(hr==S_FALSE)	// no more items
			break;
	}

	hr=FolderEnum->Reset();
	if(SUCCEEDED(hr)){
		return rCreate<cWinShellFileNameEnum>(fHostWindow,fThreading,fFolder,cnVar::MoveCast(FolderEnum));
	}

	// TODO : return enumeration to checkpoint children
	return rCreate<cWinShellFileNameEnum>(fHostWindow,fThreading,fFolder,cnVar::MoveCast(FolderEnum));
}
//---------------------------------------------------------------------------
iPtr<iFile> cWinShellFolderItemObserver::FetchFileChange(eFileChange &Change)noexcept
{
	if(fChangeEnumInProgress)
		return nullptr;

	HRESULT hr;
	SHCONTF Flags=SHCONTF_FOLDERS|SHCONTF_NONFOLDERS;
	COMPtr<IEnumIDList> FolderEnum;
	HWND HostWindowHandle;
	if(fHostWindow!=nullptr){
		HostWindowHandle=fHostWindow->GetWindowHandle();
	}
	else{
		HostWindowHandle=nullptr;
	}
	hr=fFolder->EnumObjects(HostWindowHandle,Flags,COMRetPtrT(FolderEnum));
	if(FAILED(hr)){
		// error
		return nullptr;
	}
	
	fChangeEnumInProgress=true;
	/*
	auto ChangeEnum=rCreate<cChangeEnum>(this);

	auto CheckpointChildren=cnVar::MoveCast(fCheckpointChildren);

	ULONG Fetched;
	PITEMID_CHILD ChildItemID[4];
	while(SUCCEEDED(hr=FolderEnum->Next(4,ChildItemID,&Fetched))){
		for(ULONG i=0;i<Fetched;i++){
			uIntn ExistingItemIndex=CheckpointChildren.IndexOf(ChildItemID[i]);
			if(ExistingItemIndex==IndexNotFound){
				// new file
				fCheckpointChildren.Insert(cShellChildItemID(ChildItemID[i]));

				auto NewChangeItem=ChangeEnum->ChangeList.Append();
				NewChangeItem->Change=FileChange::Insert;
				NewChangeItem->ItemID=cShellChildItemID::TakeChildID(ChildItemID[i]);
			}
			else{
				// existing file
				CheckpointChildren.RemoveAt(ExistingItemIndex);
			
				fCheckpointChildren.Insert(cShellChildItemID::TakeChildID(ChildItemID[i]));
			}
		}
		if(hr==S_FALSE)	// no more items
			break;
	}

	auto RemovedItemCount=CheckpointChildren.GetCount();
	auto RemovedItemChanges=ChangeEnum->ChangeList.Append_temp(RemovedItemCount);
	for(uIntn i=0;i<RemovedItemCount;i++){
		auto &RemovedChild=CheckpointChildren[i];
		auto &RemoveChangeItem=RemovedItemChanges[i];
		// removed item
		RemoveChangeItem.Change=FileChange::Remove;
		RemoveChangeItem.ItemID=cnVar::MoveCast(RemovedChild);
	}*/
	return nullptr;
}
//---------------------------------------------------------------------------
bool cWinShellFolderItemObserver::ShellNotifyProcess(LONG Event,PIDLIST_ABSOLUTE *List)noexcept
{
	if(Event&SHCNE_CREATE){
		// create file or virtual item
		OnFolderChanged();
		return true;
	}
	else if(Event&SHCNE_DELETE){
		// delete file or virtual item
		OnFolderChanged();
		return true;
	}
	else if(Event&SHCNE_RENAMEITEM){
		// rename file or virtual item
		OnFolderChanged();
		return true;
	}
	else if(Event&SHCNE_UPDATEITEM){
		// file or virtual item time changed
		OnFolderChanged();
		return true;
	}
	else if(Event&SHCNE_ATTRIBUTES){
		OnFolderChanged();
		return true;
	}
	else if(Event&SHCNE_MKDIR){
		// create folder
		OnFolderChanged();
		return true;
	}
	else if(Event&SHCNE_RMDIR){
		// delete folder
		OnFolderChanged();
		return true;
	}
	else if(Event&SHCNE_RENAMEFOLDER){
		// rename folder
		OnFolderChanged();
		return true;
	}
	else if(Event&SHCNE_UPDATEDIR){
		// update folder contents
		OnFolderChanged();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void cWinShellFolderItemObserver::NotificationStarted(void)noexcept
{
	rIncReference(this,'fiqu');

	bcWinShellFolderItemObserver::NotificationStarted();
	AsyncQueueSetAvailable(false);
}
//---------------------------------------------------------------------------
void cWinShellFolderItemObserver::NotificationStopped(void)noexcept
{
	bcWinShellFolderItemObserver::NotificationStopped();
	rDecReference(this,'fiqu');
}
//---------------------------------------------------------------------------
void cWinShellFolderItemObserver::NotificationProcedure(void)noexcept
{
	if(fChangeEnumInProgress)
		return;

	bcWinShellFolderItemObserver::NotificationProcedure();
}
//---------------------------------------------------------------------------
void cWinShellFolderItemObserver::OnFolderChanged(void)noexcept
{
	if(fFolderChanged==false){
		fFolderChanged=true;
		AsyncQueueSetAvailable(false);
	}
}
//---------------------------------------------------------------------------
void cWinShellFolderItemObserver::ChangeEnumFinished(void)noexcept
{
	fChangeEnumInProgress=false;
	UpdateQueueState(false);
}
//---------------------------------------------------------------------------
