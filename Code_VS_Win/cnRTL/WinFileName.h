/*- cnRTL - Windows - File Name -------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-03-29                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinCommon.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
extern const wchar_t Win32FileNameInvalidFileChar[];
extern const uIntn Win32FileNameInvalidFileCharCount;
extern const uIntn Win32FileNameInvalidFilterCharCount;
//---------------------------------------------------------------------------
bool Win32FileNameCheckIsDot(const wchar_t *Name);
DWORD Win32FileNameCheckFileName(const wchar_t *Name);
cStringBuffer<wchar_t> NTFileNormalizeName(const wchar_t *Path,uIntn PathLength);
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
typename cnVar::TTypeConditional<DWORD,
	cnVar::TIsSame<wchar_t,typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type::tElement>::Value
>::Type Win32FileNameStreamAppend(TStreamWriteBuffer &&WriteBuffer,const wchar_t *Name)
{
	DWORD NameLength=Win32FileNameCheckFileName(Name);
	if(NameLength==0)
		return 0;
	cMemoryCachedStreamWriteBuffer<typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type> CachedWriteBuffer(&WriteBuffer);
	// copy string
	auto NameBuffer=CachedWriteBuffer.ReserveWriteBuffer(1+NameLength);
	if(NameBuffer.Length<=NameLength){
		CachedWriteBuffer.CommitWriteBuffer(0);
		return 0;	// not enough buffer
	}
	NameBuffer.Pointer[0]='\\';
	cnMemory::Copy(NameBuffer.Pointer+1,Name,NameLength*sizeof(wchar_t));
	CachedWriteBuffer.CommitWriteBuffer(1+NameLength);
	return 1+NameLength;
}
//---------------------------------------------------------------------------
cStringBuffer<wchar_t> Win32FileMakeName(const wchar_t *BasePath,uIntn BasePathLength,const uChar16*const *Path,uIntn Depth);
cStringBuffer<wchar_t> Win32FileMakeFindName(const wchar_t *Path,uIntn PathLength,const wchar_t *Filter);
cStringBuffer<wchar_t> Win32FileMakeTemporaryFileName(const wchar_t *FolderName,const wchar_t *Prefix);
cStringBuffer<wchar_t> NTFileMakeTempFolderName(void);
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
typename cnVar::TTypeConditional<DWORD,
	cnVar::TIsSame<wchar_t,typename TStreamWriteBuffer::tElement>::Value
>::Type Win32FileNameStreamReserveModule(TStreamWriteBuffer&& WriteBuffer,cArray<typename TStreamWriteBuffer::tElement> &NameBuffer,HMODULE ModuleHandle)
{
	uIntn BufferLength=cnMath::IntRoundUp(MAX_PATH,256);
	DWORD PathLength=0;
	cMemoryCachedStreamWriteBuffer<typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type> CachedWriteBuffer(WriteBuffer);
	while((NameBuffer=CachedWriteBuffer.ReserveWriteBuffer(BufferLength)).Length>PathLength){
		PathLength=::GetModuleFileNameW(ModuleHandle,NameBuffer.Pointer,static_cast<DWORD>(NameBuffer.Length));
		if(PathLength==0){
			// error
			CachedWriteBuffer.CommitWriteBuffer(0);
			return 0;
		}
		DWORD LastError=::GetLastError();
		if(LastError==ERROR_SUCCESS){
			break;
		}
		CachedWriteBuffer.CommitWriteBuffer(0);
		if(LastError!=ERROR_INSUFFICIENT_BUFFER){
			// error
			return 0;
		}
	
		// increase buffer and continue;
		BufferLength*=2;
	}
	// no more buffer or successful
	return PathLength;
}
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
typename cnVar::TTypeConditional<DWORD,
	cnVar::TIsSame<wchar_t,typename TStreamWriteBuffer::tElement>::Value
>::Type Win32FileNameStreamWriteModule(TStreamWriteBuffer&& WriteBuffer,HMODULE ModuleHandle)
{
	cArray<typename TStreamWriteBuffer::tElement> NameBuffer;
	DWORD PathLength=Win32FileNameStreamReserveModule(cnVar::Forward<TStreamWriteBuffer>(WriteBuffer),NameBuffer,ModuleHandle);
	WriteBuffer.CommitWriteBuffer(PathLength);
	return PathLength;
}
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
typename cnVar::TTypeConditional<DWORD,
	cnVar::TIsSame<wchar_t,typename TStreamWriteBuffer::tElement>::Value
>::Type Win32FileNameStreamWriteModuleFolder(TStreamWriteBuffer&& WriteBuffer,HMODULE ModuleHandle)
{
	cArray<typename TStreamWriteBuffer::tElement> NameBuffer;
	DWORD PathLength=Win32FileNameStreamReserveModule(cnVar::Forward<TStreamWriteBuffer>(WriteBuffer),NameBuffer,ModuleHandle);

	while(PathLength>0){
		PathLength--;
		if(NameBuffer.Pointer[PathLength]=='\\'){
			break;
		}
	}

	WriteBuffer.CommitWriteBuffer(PathLength);
	return PathLength;
}
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
typename cnVar::TTypeConditional<DWORD,
	cnVar::TIsSame<wchar_t,typename TStreamWriteBuffer::tElement>::Value
>::Type Win32FileNameStreamWriteApplication(TStreamWriteBuffer&& WriteBuffer)
{
	return Win32FileNameStreamWriteModule(cnVar::Forward<TStreamWriteBuffer>(WriteBuffer),nullptr);
}
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
typename cnVar::TTypeConditional<DWORD,
	cnVar::TIsSame<wchar_t,typename TStreamWriteBuffer::tElement>::Value
>::Type Win32FileNameStreamWriteApplicationFolder(TStreamWriteBuffer&& WriteBuffer)
{
	return Win32FileNameStreamWriteModuleFolder(cnVar::Forward<TStreamWriteBuffer>(WriteBuffer),nullptr);
}
//---------------------------------------------------------------------------
cnRTL::cStringBuffer<wchar_t> Win32FileMakeModuleName(HMODULE ModuleHandle);
cStringBuffer<wchar_t> NTFileMakeApplicationFileName(void);
cStringBuffer<wchar_t> NTFileMakeApplicationFolderName(void);
//---------------------------------------------------------------------------
class cWin32FileNameEnum : public iFileEnumerator
{
public:
	cWin32FileNameEnum(cnRTL::cString<wchar_t> Path,cnRTL::cString<wchar_t> FindPath,iPtr<iFileName> (*CreateFileName)(cString<wchar_t> Path,const WIN32_FIND_DATA &CacheFileInfo));
	~cWin32FileNameEnum();

	virtual bool cnLib_FUNC Fetch(void)override;
	virtual iFile*	cnLib_FUNC GetCurrentFile(void)override;

protected:
	static bool IsDot(const WIN32_FIND_DATA &fd);
	static bool IsInvalidFile(const WIN32_FIND_DATA &fd);
	cnRTL::cString<wchar_t> MakeFileName(const WIN32_FIND_DATA &fd);

	bool FindNext(WIN32_FIND_DATA &fd);
	
	iPtr<iFileName> fCurrentFileName;

private:
	HANDLE fFindHandle;
	iPtr<iFileName> (*fCreateFileName)(cString<wchar_t> Path,const WIN32_FIND_DATA &CacheFileInfo);
	cnRTL::cString<wchar_t> fPath;
	cnRTL::cString<wchar_t> fFindPath;
	bool ProcessFindData(WIN32_FIND_DATA &fd);
};
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
