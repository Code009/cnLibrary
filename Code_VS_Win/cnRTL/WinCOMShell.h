/*- cnRTL - Windows - COM Shell -------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-06-28                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinSDKHeader.h>

#include <cnRTL\WinCommon.h>
#include <cnRTL\WinCOMIO.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
ufInt16 ShellChildItemIDGetSize(const ITEMID_CHILD *ChildID);
ITEMID_CHILD* ShellChildItemIDDuplicate(const ITEMID_CHILD *ChildID);
uIntn ShellItemIDListGetSize(const ITEMIDLIST *IDList);
ITEMIDLIST* ShellItemIDListMakeCopy(const ITEMIDLIST *IDList,uIntn IDListSize);
ITEMIDLIST* ShellItemIDListDuplicate(const ITEMIDLIST *IDList,uIntn &IDListSize);
ITEMIDLIST* ShellItemIDListDuplicateChildID(const ITEMID_CHILD *ChildID);
//---------------------------------------------------------------------------
class cShellString
{
public:
	cShellString();
	~cShellString();
	void Clear(void);
	LPWSTR* RetPtr(void);
	operator LPWSTR ()const;
	LPWSTR GetString(void)const;

	static LPWSTR Duplicate(const wchar_t *String);

	cShellString(const wchar_t *String);
	cShellString(const cShellString &ID);
	cShellString(cShellString &&ID);

	cShellString& operator =(const wchar_t *String);
	cShellString& operator =(const cShellString &ID);
	cShellString& operator =(cShellString &&ID);

	HRESULT FromStrRet(STRRET &&StrRet,PCUITEMID_CHILD ChildItemID);

	sfInt8 Compare(const cShellString &ID)const;

	bool operator ==(const cShellString &ID)const;
	bool operator !=(const cShellString &ID)const;
	bool operator <(const cShellString &ID)const;
	bool operator <=(const cShellString &ID)const;
	bool operator >(const cShellString &ID)const;
	bool operator >=(const cShellString &ID)const;

protected:
	LPWSTR fString;
};
//---------------------------------------------------------------------------
class cShellChildItemID
{
public:
	cShellChildItemID();
	~cShellChildItemID();
	void Clear(void);
	LPITEMIDLIST* RetPtr(void);
	operator const ITEMID_CHILD *()const;

	uIntn GetIDSize(void)const;
	static cShellChildItemID TakeChildID(ITEMID_CHILD *ChildID);
		

	explicit cShellChildItemID(const ITEMID_CHILD *ChildID);
	cShellChildItemID(const cShellChildItemID &ID);
	cShellChildItemID(cShellChildItemID &&ID);

	cShellChildItemID& operator =(ITEMID_CHILD *ChildID);
	cShellChildItemID& operator =(const cShellChildItemID &ID);
	cShellChildItemID& operator =(cShellChildItemID &&ID);

	sfInt8 Compare(const ITEMID_CHILD *ChildID)const;

	bool operator ==(const ITEMID_CHILD *ChildID)const;
	bool operator !=(const ITEMID_CHILD *ChildID)const;
	bool operator <(const ITEMID_CHILD *ChildID)const;
	bool operator <=(const ITEMID_CHILD *ChildID)const;
	bool operator >(const ITEMID_CHILD *ChildID)const;
	bool operator >=(const ITEMID_CHILD *ChildID)const;
protected:
	ITEMID_CHILD *fChildID;

	explicit cShellChildItemID(ITEMID_CHILD &&ChildID);
};
//---------------------------------------------------------------------------
class cShellItemIDList
{
public:
	cShellItemIDList();
	~cShellItemIDList();
	void Clear(void);
	LPITEMIDLIST* RetPtr(void);
	operator const ITEMIDLIST *()const;
		
	uIntn GetIDSize(void)const;

	static cShellItemIDList TakeIDList(ITEMIDLIST *IDList);


	explicit cShellItemIDList(const ITEMIDLIST *IDList);
	cShellItemIDList(const cShellItemIDList &ID);
	cShellItemIDList(cShellItemIDList &&ID);

	cShellItemIDList& operator =(ITEMIDLIST *ChildID);
	cShellItemIDList& operator =(const cShellItemIDList &ID);
	cShellItemIDList& operator =(cShellItemIDList &&ID);

	cShellItemIDList(const cShellChildItemID &ChildID);
	cShellItemIDList& operator =(const cShellChildItemID &ChildID);
	

	sfInt8 Compare(const ITEMIDLIST *IDList)const;

	bool operator ==(const ITEMIDLIST *IDList)const;
	bool operator !=(const ITEMIDLIST *IDList)const;
	bool operator <(const ITEMIDLIST *IDList)const;
	bool operator <=(const ITEMIDLIST *IDList)const;
	bool operator >(const ITEMIDLIST *IDList)const;
	bool operator >=(const ITEMIDLIST *IDList)const;

	void AppendChildID(ITEMID_CHILD *ChildID);

protected:
	ITEMIDLIST *fIDList;
	mutable uIntn fCachedIDListSize;

	explicit cShellItemIDList(ITEMIDLIST &&ChildID);

};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class cWinShellFileName : public iFileName
{
public:
	cnLib_INTERFACE_DEFINE(cWinShellFileName,iFileName)

	cWinShellFileName(iPtr<iWindow> HostWindow,rPtr<iCOMApartmentThreading> Threading,COMPtr<IShellFolder> Parent,cShellItemIDList ChildIDList);
	~cWinShellFileName();
	
	static iPtr<iFileName> CreateFileNameFromFileSystemFolderItem(IShellFolder *ParentFolder,LPCITEMIDLIST ChildIDList);
	static iPtr<iFileName> CreateFileNameFromFolderItem(iWindow *HostWindow,iCOMApartmentThreading *Threading,COMPtr<IShellFolder> ParentFolder,cShellItemIDList ChildIDList);
	static iPtr<iFileName> CreateFileNameFromShellItem(iWindow *HostWindow,iCOMApartmentThreading *Threading,COMPtr<IShellItem> ShellItem);


	HRESULT SetupFolder(void);

	rPtr<iFileEnumerator> Enum(void);


	// iFile

	virtual bool	cnLib_FUNC IsExists(void)override;
	virtual bool	cnLib_FUNC HasData(void)override;
	virtual bool	cnLib_FUNC HasFolder(void)override;
	virtual bool	cnLib_FUNC Delete(void)override;

	virtual uInt64	cnLib_FUNC GetDataSize(void)override;
	virtual iPtr<iFileSyncStream>	cnLib_FUNC OpenSequentialStream(eFileAccess AccessMode,eFileCreate CreateFlag)override;
	virtual iPtr<iFileStream>		cnLib_FUNC OpenFileStream(eFileAccess AccessMode,eFileCreate CreateFlag)override;

	virtual rPtr<iFileEnumerator>		cnLib_FUNC EnumFolder(void)override;
	virtual rPtr<iAsyncNotification>	cnLib_FUNC WatchFolder(void)override;
	virtual rPtr<iFileObserver>			cnLib_FUNC ObserveFolder(void)override;
	virtual bool	cnLib_FUNC CreateFolder(void)override;
	virtual uIntn	cnLib_FUNC CreateFolderPath(void)override;

	// iFileName

	virtual eiOrdering cnLib_FUNC Compare(iFileName *Dest)override;
	virtual iPtr<iFileName>	cnLib_FUNC ParentFileName(void)override;
	virtual rPtr<iFileEnumerator>	cnLib_FUNC EnumFileNames(const uChar16 *Filter)override;
	virtual const uChar16*	cnLib_FUNC GetName(void)override;
	virtual uIntn			cnLib_FUNC GetNameLength(void)override;
	virtual iPtr<iFileName>	cnLib_FUNC MakeName(const uChar16*const *Path,uIntn Depth)override;

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
	void SetupAttributes(void);
	bool SetupFileInfo(void);

	void SetupName(void);
};
//---------------------------------------------------------------------------
class cWinShellFileNameEnum : public iFileEnumerator
{
public:
	cWinShellFileNameEnum(iPtr<iWindow> HostWindow,rPtr<iCOMApartmentThreading> Threading,COMPtr<IShellFolder> Folder,COMPtr<IEnumIDList> Enum);
	~cWinShellFileNameEnum();

	virtual bool cnLib_FUNC Fetch(void)override;
	virtual iFile*	cnLib_FUNC GetCurrentFile(void)override;
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
	bcWinShellFolderItemObserver(iPtr<iWindow> HostWindow,rPtr<iCOMApartmentThreading> Threading,COMPtr<IShellFolder> Folder);
	~bcWinShellFolderItemObserver();

protected:
	void VirtualStopped(void);

	iPtr<iWindow> fHostWindow;
	rPtr<iCOMApartmentThreading> fThreading;
	COMPtr<IShellFolder> fFolder;
	cShellItemIDList fFolderIDList;

	static cShellItemIDList MakeFolderAbsoluteIDList(IShellFolder *Folder);

	iAsyncNotificationCallback *fCallback;
	rPtr<iReference> fCallbackReference;

	virtual iReference* NotificationInnerReference(void)override;
	virtual void NotificationStarted(void)override;
	virtual void NotificationStopped(void)override;
	virtual CycleState NotificationCheckState(void)override;
	virtual void AsyncQueueNotify(void)override;


	// shell notification

	class cNotifyWindowHandler : public iWindowMessageHandler
	{
	public:
		virtual void cnLib_FUNC WindowAttached(void)override;
		virtual void cnLib_FUNC WindowDetached(void)override;
		virtual bool cnLib_FUNC WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)override;
		virtual void cnLib_FUNC WindowMessageProcessed(LRESULT Result,const cWindowMessageParam &MsgParam)override;
	}fNotifyWindowHandler;

	virtual LONG ShellNotifyMask(void);
	virtual bool ShellNotifyProcess(LONG Event,PIDLIST_ABSOLUTE *List)=0;


	iPtr<iWindow> fNotifyWindow;
	ULONG fNotifyToken;
	void UpdateFolderMonitor(void);
	class cCreateNotifyWindowProc : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)override;
	}fCreateNotifyWindowProc;
	void NotifyFolderMonitor(void);
};
//---------------------------------------------------------------------------
class cWinShellFolderItemWatcher : public iAsyncNotification, public bcWinShellFolderItemObserver
{
public:
	cWinShellFolderItemWatcher(iPtr<iWindow> HostWindow,rPtr<iCOMApartmentThreading> Threading,COMPtr<IShellFolder> Folder);
	~cWinShellFolderItemWatcher();

	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)override;
	virtual void cnLib_FUNC StopNotify(void)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)override;
	virtual bool cnLib_FUNC IsClosed(void)override;

protected:

	virtual void NotificationStarted(void)override;
	virtual void NotificationStopped(void)override;

	// shell notification

	virtual bool ShellNotifyProcess(LONG Event,PIDLIST_ABSOLUTE *List)override;
};
//---------------------------------------------------------------------------
class cWinShellFolderItemObserver : public iFileObserver, public bcWinShellFolderItemObserver
{
public:
	cWinShellFolderItemObserver(iPtr<iWindow> HostWindow,rPtr<iCOMApartmentThreading> Threading,COMPtr<IShellFolder> Folder);
	~cWinShellFolderItemObserver();

	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)override;
	virtual void cnLib_FUNC StopNotify(void)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)override;
	virtual bool cnLib_FUNC IsClosed(void)override;
	virtual void cnLib_FUNC DiscardChanges(void)override;
	virtual rPtr<iFileEnumerator> cnLib_FUNC EnumCurrentFiles(void)override;
	virtual rPtr<iFileChangeEnumerator> cnLib_FUNC FetchFileChange(void)override;

protected:

	virtual void NotificationStarted(void)override;
	virtual void NotificationStopped(void)override;
	virtual void NotificationProcedure(void)override;

	// shell notification

	virtual bool ShellNotifyProcess(LONG Event,PIDLIST_ABSOLUTE *List)override;

	// current checkpoint

	cSeqSet< cShellChildItemID > fCheckpointChildren;

	void OnFolderChanged(void);

	bool fFolderChanged;
	bool fChangeEnumInProgress;
	void ChangeEnumFinished(void);

	struct cChangeItem
	{
		cShellChildItemID ItemID;
		eFileChange Change;
	};

	class cChangeEnum : public iFileChangeEnumerator
	{
	public:
		cChangeEnum(cWinShellFolderItemObserver *Owner);
		~cChangeEnum();

		cSeqList< cChangeItem > ChangeList;

		virtual bool cnLib_FUNC Fetch(void)override;
		virtual iFile* cnLib_FUNC GetCurrentFile(void)override;
		virtual eFileChange cnLib_FUNC GetChange(void)override;
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
