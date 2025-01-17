/*- cnRTL - Windows - COM Shell -------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-06-28                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinSDKHeader.h>

#include <cnRTL\AsyncTask.h>
#include <cnRTL\WinCommon.h>
#include <cnRTL\WinCOMIO.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
ufInt16 ShellChildItemIDGetSize(const ITEMID_CHILD *ChildID)noexcept(true);
ITEMID_CHILD* ShellChildItemIDDuplicate(const ITEMID_CHILD *ChildID)noexcept(true);
uIntn ShellItemIDListGetSize(const ITEMIDLIST *IDList)noexcept(true);
ITEMIDLIST* ShellItemIDListMakeCopy(const ITEMIDLIST *IDList,uIntn IDListSize)noexcept(true);
ITEMIDLIST* ShellItemIDListDuplicate(const ITEMIDLIST *IDList,uIntn &IDListSize)noexcept(true);
ITEMIDLIST* ShellItemIDListDuplicateChildID(const ITEMID_CHILD *ChildID)noexcept(true);
//---------------------------------------------------------------------------
class cShellString
{
public:
	cShellString()noexcept(true);
	~cShellString()noexcept(true);
	void Clear(void)noexcept(true);
	LPWSTR* RetPtr(void)noexcept(true);
	operator LPWSTR ()const noexcept(true);
	LPWSTR GetString(void)const noexcept(true);

	static LPWSTR Duplicate(const wchar_t *String)noexcept(true);

	cShellString(const wchar_t *String)noexcept(true);
	cShellString(const cShellString &ID)noexcept(true);
	cShellString(cShellString &&ID)noexcept(true);

	cShellString& operator =(const wchar_t *String)noexcept(true);
	cShellString& operator =(const cShellString &ID)noexcept(true);
	cShellString& operator =(cShellString &&ID)noexcept(true);

	HRESULT FromStrRet(STRRET &&StrRet,PCUITEMID_CHILD ChildItemID)noexcept(true);

	sfInt8 Compare(const cShellString &ID)const noexcept(true);

	bool operator ==(const cShellString &ID)const noexcept(true);
	bool operator !=(const cShellString &ID)const noexcept(true);
	bool operator <(const cShellString &ID)const noexcept(true);
	bool operator <=(const cShellString &ID)const noexcept(true);
	bool operator >(const cShellString &ID)const noexcept(true);
	bool operator >=(const cShellString &ID)const noexcept(true);

protected:
	LPWSTR fString;
};
//---------------------------------------------------------------------------
class cShellChildItemID
{
public:
	cShellChildItemID()noexcept(true);
	~cShellChildItemID()noexcept(true);
	void Clear(void)noexcept(true);
	LPITEMIDLIST* RetPtr(void)noexcept(true);
	operator const ITEMID_CHILD *()const noexcept(true);

	uIntn GetIDSize(void)const noexcept(true);
	static cShellChildItemID TakeChildID(ITEMID_CHILD *ChildID)noexcept(true);
		

	explicit cShellChildItemID(const ITEMID_CHILD *ChildID)noexcept(true);
	cShellChildItemID(const cShellChildItemID &ID)noexcept(true);
	cShellChildItemID(cShellChildItemID &&ID)noexcept(true);

	cShellChildItemID& operator =(ITEMID_CHILD *ChildID)noexcept(true);
	cShellChildItemID& operator =(const cShellChildItemID &ID)noexcept(true);
	cShellChildItemID& operator =(cShellChildItemID &&ID)noexcept(true);

	sfInt8 Compare(const ITEMID_CHILD *ChildID)const noexcept(true);

	bool operator ==(const ITEMID_CHILD *ChildID)const noexcept(true);
	bool operator !=(const ITEMID_CHILD *ChildID)const noexcept(true);
	bool operator <(const ITEMID_CHILD *ChildID)const noexcept(true);
	bool operator <=(const ITEMID_CHILD *ChildID)const noexcept(true);
	bool operator >(const ITEMID_CHILD *ChildID)const noexcept(true);
	bool operator >=(const ITEMID_CHILD *ChildID)const noexcept(true);
protected:
	ITEMID_CHILD *fChildID;

	explicit cShellChildItemID(ITEMID_CHILD &&ChildID)noexcept(true);
};
//---------------------------------------------------------------------------
class cShellItemIDList
{
public:
	cShellItemIDList()noexcept(true);
	~cShellItemIDList()noexcept(true);
	void Clear(void)noexcept(true);
	LPITEMIDLIST* RetPtr(void)noexcept(true);
	operator const ITEMIDLIST *()const noexcept(true);
		
	uIntn GetIDSize(void)const noexcept(true);

	static cShellItemIDList TakeIDList(ITEMIDLIST *IDList)noexcept(true);


	explicit cShellItemIDList(const ITEMIDLIST *IDList)noexcept(true);
	cShellItemIDList(const cShellItemIDList &ID)noexcept(true);
	cShellItemIDList(cShellItemIDList &&ID)noexcept(true);

	cShellItemIDList& operator =(ITEMIDLIST *ChildID)noexcept(true);
	cShellItemIDList& operator =(const cShellItemIDList &ID)noexcept(true);
	cShellItemIDList& operator =(cShellItemIDList &&ID)noexcept(true);

	cShellItemIDList(const cShellChildItemID &ChildID)noexcept(true);
	cShellItemIDList& operator =(const cShellChildItemID &ChildID)noexcept(true);
	

	sfInt8 Compare(const ITEMIDLIST *IDList)const noexcept(true);

	bool operator ==(const ITEMIDLIST *IDList)const noexcept(true);
	bool operator !=(const ITEMIDLIST *IDList)const noexcept(true);
	bool operator <(const ITEMIDLIST *IDList)const noexcept(true);
	bool operator <=(const ITEMIDLIST *IDList)const noexcept(true);
	bool operator >(const ITEMIDLIST *IDList)const noexcept(true);
	bool operator >=(const ITEMIDLIST *IDList)const noexcept(true);

	void AppendChildID(ITEMID_CHILD *ChildID)noexcept(true);

protected:
	ITEMIDLIST *fIDList;
	mutable uIntn fCachedIDListSize;

	explicit cShellItemIDList(ITEMIDLIST &&ChildID)noexcept(true);

};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class cWinShellFileName : public iFileName
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnClass::ImplementCastInterface<iFileName>(this,ID);	}

	cWinShellFileName(iPtr<iWindow> HostWindow,rPtr<iCOMApartmentThreading> Threading,COMPtr<IShellFolder> Parent,cShellItemIDList ChildIDList)noexcept(true);
	~cWinShellFileName();
	
	static iPtr<iFileName> CreateFileNameFromFileSystemFolderItem(IShellFolder *ParentFolder,LPCITEMIDLIST ChildIDList)noexcept(true);
	static iPtr<iFileName> CreateFileNameFromFolderItem(iWindow *HostWindow,iCOMApartmentThreading *Threading,COMPtr<IShellFolder> ParentFolder,cShellItemIDList ChildIDList)noexcept(true);
	static iPtr<iFileName> CreateFileNameFromShellItem(iWindow *HostWindow,iCOMApartmentThreading *Threading,COMPtr<IShellItem> ShellItem)noexcept(true);


	HRESULT SetupFolder(void)noexcept(true);

	rPtr<iFileEnumerator> Enum(void)noexcept(true);


	// iFile

	virtual bool	cnLib_FUNC IsExists(void)noexcept(true)override;
	virtual bool	cnLib_FUNC HasData(void)noexcept(true)override;
	virtual bool	cnLib_FUNC HasFolder(void)noexcept(true)override;
	virtual bool	cnLib_FUNC Delete(void)noexcept(true)override;

	virtual uInt64	cnLib_FUNC GetDataSize(void)noexcept(true)override;
	virtual iPtr<iFileSyncStream>	cnLib_FUNC OpenSequentialStream(eFileAccess AccessMode,eFileCreate CreateFlag)noexcept(true)override;
	virtual iPtr<iFileStream>		cnLib_FUNC OpenFileStream(eFileAccess AccessMode,eFileCreate CreateFlag)noexcept(true)override;

	virtual rPtr<iFileEnumerator>		cnLib_FUNC EnumFolder(void)noexcept(true)override;
	virtual rPtr<iAsyncNotification>	cnLib_FUNC WatchFolder(void)noexcept(true)override;
	virtual rPtr<iFileObserver>			cnLib_FUNC ObserveFolder(void)noexcept(true)override;
	virtual bool	cnLib_FUNC CreateFolder(void)noexcept(true)override;
	virtual uIntn	cnLib_FUNC CreateFolderPath(void)noexcept(true)override;

	// iFileName

	virtual eiOrdering cnLib_FUNC Compare(iFileName *Dest)noexcept(true)override;
	virtual iPtr<iFileName>	cnLib_FUNC ParentFileName(void)noexcept(true)override;
	virtual rPtr<iFileEnumerator>	cnLib_FUNC EnumFileNames(const uChar16 *Filter)noexcept(true)override;
	virtual const uChar16*	cnLib_FUNC GetName(void)noexcept(true)override;
	virtual uIntn			cnLib_FUNC GetNameLength(void)noexcept(true)override;
	virtual iPtr<iFileName>	cnLib_FUNC MakeName(const uChar16*const *Path,uIntn Depth)noexcept(true)override;

protected:
	iPtr<iWindow> fHostWindow;
	rPtr<iCOMApartmentThreading> fThreading;
	COMPtr<IShellFolder> fParentFolder;
	cShellItemIDList fChildIDList;
	cString<uChar16> fName;

	COMPtr<IShellFolder> fChildFolder;

	bool fCachedItemAttributes;
	bool fCachedFileExists;
	bool fCachedFileInfo;
	SFGAOF fItemAttributes;
	FILEDESCRIPTORW fFileInfo;
	void SetupAttributes(void)noexcept(true);
	bool SetupFileInfo(void)noexcept(true);

	void SetupName(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cWinShellFileNameEnum : public iFileEnumerator
{
public:
	cWinShellFileNameEnum(iPtr<iWindow> HostWindow,rPtr<iCOMApartmentThreading> Threading,COMPtr<IShellFolder> Folder,COMPtr<IEnumIDList> Enum)noexcept(true);
	~cWinShellFileNameEnum()noexcept(true);

	virtual bool cnLib_FUNC Fetch(void)noexcept(true)override;
	virtual iFile*	cnLib_FUNC GetCurrentFile(void)noexcept(true)override;
protected:
	iPtr<iWindow> fHostWindow;
	rPtr<iCOMApartmentThreading> fThreading;
	COMPtr<IShellFolder> fFolder;
	COMPtr<IEnumIDList> fEnum;

	cShellItemIDList fCurrentItemIDList;

	iPtr<iFileName> fCurrentFileName;
};
//---------------------------------------------------------------------------
class bcWinShellFolderItemObserver : public bcAsyncQueue, public cDualReference
{
public:
	bcWinShellFolderItemObserver(iPtr<iWindow> HostWindow,rPtr<iCOMApartmentThreading> Threading,COMPtr<IShellFolder> Folder)noexcept(true);
	~bcWinShellFolderItemObserver()noexcept(true);

protected:
	void VirtualStopped(void)noexcept(true);

	iPtr<iWindow> fHostWindow;
	rPtr<iCOMApartmentThreading> fThreading;
	COMPtr<IShellFolder> fFolder;
	cShellItemIDList fFolderIDList;

	static cShellItemIDList MakeFolderAbsoluteIDList(IShellFolder *Folder)noexcept(true);

	iAsyncNotificationCallback *fCallback;
	rPtr<iReference> fCallbackReference;

	virtual iReference* NotificationInnerReference(void)noexcept(true)override;
	virtual void NotificationStarted(void)noexcept(true)override;
	virtual void NotificationStopped(void)noexcept(true)override;
	virtual CycleState NotificationCheckState(void)noexcept(true)override;
	virtual void AsyncQueueNotify(void)noexcept(true)override;


	// shell notification

	class cNotifyWindowHandler : public iWindowMessageHandler
	{
	public:
		virtual void cnLib_FUNC WindowAttached(void)noexcept(true)override;
		virtual void cnLib_FUNC WindowDetached(void)noexcept(true)override;
		virtual bool cnLib_FUNC WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)noexcept(true)override;
		virtual void cnLib_FUNC WindowMessageProcessed(LRESULT Result,const cWindowMessageParam &MsgParam)noexcept(true)override;
	}fNotifyWindowHandler;

	virtual LONG ShellNotifyMask(void)noexcept(true);
	virtual bool ShellNotifyProcess(LONG Event,PIDLIST_ABSOLUTE *List)noexcept(true)=0;


	iPtr<iWindow> fNotifyWindow;
	ULONG fNotifyToken;
	void UpdateFolderMonitor(void)noexcept(true);
	class cCreateNotifyWindowProc : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fCreateNotifyWindowProc;
	void NotifyFolderMonitor(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cWinShellFolderItemWatcher : public iAsyncNotification, public bcWinShellFolderItemObserver
{
public:
	cWinShellFolderItemWatcher(iPtr<iWindow> HostWindow,rPtr<iCOMApartmentThreading> Threading,COMPtr<IShellFolder> Folder)noexcept(true);
	~cWinShellFolderItemWatcher()noexcept(true);

	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept(true)override;
	virtual void cnLib_FUNC StopNotify(void)noexcept(true)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)noexcept(true)override;
	virtual bool cnLib_FUNC IsClosed(void)noexcept(true)override;

protected:

	virtual void NotificationStarted(void)noexcept(true)override;
	virtual void NotificationStopped(void)noexcept(true)override;

	// shell notification

	virtual bool ShellNotifyProcess(LONG Event,PIDLIST_ABSOLUTE *List)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWinShellFolderItemObserver : public iFileObserver, public bcWinShellFolderItemObserver
{
public:
	cWinShellFolderItemObserver(iPtr<iWindow> HostWindow,rPtr<iCOMApartmentThreading> Threading,COMPtr<IShellFolder> Folder)noexcept(true);
	~cWinShellFolderItemObserver()noexcept(true);

	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept(true)override;
	virtual void cnLib_FUNC StopNotify(void)noexcept(true)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)noexcept(true)override;
	virtual bool cnLib_FUNC IsClosed(void)noexcept(true)override;
	virtual iPtr<iFile> cnLib_FUNC FetchFileChange(eFileChange &Change)noexcept(true)override;
	virtual void cnLib_FUNC DiscardChanges(void)noexcept(true)override;
	virtual rPtr<iFileEnumerator> cnLib_FUNC ResetChanges(void)noexcept(true)override;

protected:

	virtual void NotificationStarted(void)noexcept(true)override;
	virtual void NotificationStopped(void)noexcept(true)override;
	virtual void NotificationProcedure(void)noexcept(true)override;

	// shell notification

	virtual bool ShellNotifyProcess(LONG Event,PIDLIST_ABSOLUTE *List)noexcept(true)override;

	// current checkpoint

	cSeqSet< cShellChildItemID > fCheckpointChildren;

	void OnFolderChanged(void)noexcept(true);

	bool fFolderChanged;
	bool fChangeEnumInProgress;
	void ChangeEnumFinished(void)noexcept(true);

	struct cChangeItem
	{
		cShellChildItemID ItemID;
		eFileChange Change;
	};


	class cChangeEnum
	{
	public:
		cChangeEnum(cWinShellFolderItemObserver *Owner)noexcept(true);
		~cChangeEnum()noexcept(true);

		cSeqList< cChangeItem > ChangeList;

		virtual bool cnLib_FUNC Fetch(void)noexcept(true);
		virtual iFile* cnLib_FUNC GetCurrentFile(void)noexcept(true);
		virtual eFileChange cnLib_FUNC GetChange(void)noexcept(true);
	protected:
		rInnerPtr<cWinShellFolderItemObserver> fOwner;
		uIntn fCurrentIndex;
		iPtr<iFileName> fCurrentFileName;
	};
};
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
