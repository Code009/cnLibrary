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
bool Win32FileNameCheckIsDot(const wchar_t *Name)noexcept(true);
DWORD Win32FileNameCheckFileName(const wchar_t *Name)noexcept(true);
cStringBuffer<wchar_t> NTFileNormalizeName(const wchar_t *Path,uIntn PathLength)noexcept(true);
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
inline typename cnVar::TTypeConditional<DWORD,
	cnVar::TIsSame<wchar_t,typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type::tElement>::Value
>::Type Win32FileNameStreamAppend(TStreamWriteBuffer &&WriteBuffer,const wchar_t *Name)noexcept(true)
{
	DWORD NameLength=Win32FileNameCheckFileName(Name);
	if(NameLength==0)
		return 0;
	// copy string
	auto NameBuffer=WriteBuffer.ReserveWriteBuffer(1+NameLength);
	if(NameBuffer.Length<=NameLength){
		WriteBuffer.CommitWriteBuffer(0);
		return 0;	// not enough buffer
	}
	NameBuffer.Pointer[0]='\\';
	cnMemory::Copy(NameBuffer.Pointer+1,Name,NameLength*sizeof(wchar_t));
	WriteBuffer.CommitWriteBuffer(1+NameLength);
	return 1+NameLength;
}
//---------------------------------------------------------------------------
cStringBuffer<wchar_t> Win32FileMakeName(const wchar_t *BasePath,uIntn BasePathLength,const uChar16*const *Path,uIntn Depth)noexcept(true);
cStringBuffer<wchar_t> Win32FileMakeFindName(const wchar_t *Path,uIntn PathLength,const wchar_t *Filter)noexcept(true);
cStringBuffer<wchar_t> Win32FileMakeTemporaryFileName(const wchar_t *FolderName,const wchar_t *Prefix)noexcept(true);
cStringBuffer<wchar_t> NTFileMakeTempFolderName(void)noexcept(true);
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
inline typename cnVar::TTypeConditional<DWORD,
	cnVar::TIsSame<wchar_t,typename TStreamWriteBuffer::tElement>::Value
>::Type Win32FileNameStreamReserveModule(TStreamWriteBuffer&& WriteBuffer,cArray<typename TStreamWriteBuffer::tElement> &NameBuffer,HMODULE ModuleHandle)noexcept(true)
{
	uIntn BufferLength=cnMath::IntRoundUp(MAX_PATH,256);
	DWORD PathLength=0;
	while((NameBuffer=WriteBuffer.ReserveWriteBuffer(BufferLength)).Length>PathLength){
		PathLength=::GetModuleFileNameW(ModuleHandle,NameBuffer.Pointer,static_cast<DWORD>(NameBuffer.Length));
		if(PathLength==0){
			// error
			WriteBuffer.CommitWriteBuffer(0);
			return 0;
		}
		DWORD LastError=::GetLastError();
		if(LastError==ERROR_SUCCESS){
			break;
		}
		WriteBuffer.CommitWriteBuffer(0);
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
inline typename cnVar::TTypeConditional<DWORD,
	cnVar::TIsSame<wchar_t,typename TStreamWriteBuffer::tElement>::Value
>::Type Win32FileNameStreamWriteModule(TStreamWriteBuffer&& WriteBuffer,HMODULE ModuleHandle)noexcept(true)
{
	cArray<typename TStreamWriteBuffer::tElement> NameBuffer;
	DWORD PathLength=Win32FileNameStreamReserveModule(cnVar::Forward<TStreamWriteBuffer>(WriteBuffer),NameBuffer,ModuleHandle);
	WriteBuffer.CommitWriteBuffer(PathLength);
	return PathLength;
}
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
inline typename cnVar::TTypeConditional<DWORD,
	cnVar::TIsSame<wchar_t,typename TStreamWriteBuffer::tElement>::Value
>::Type Win32FileNameStreamWriteModuleFolder(TStreamWriteBuffer&& WriteBuffer,HMODULE ModuleHandle)noexcept(true)
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
inline typename cnVar::TTypeConditional<DWORD,
	cnVar::TIsSame<wchar_t,typename TStreamWriteBuffer::tElement>::Value
>::Type Win32FileNameStreamWriteApplication(TStreamWriteBuffer&& WriteBuffer)noexcept(true)
{
	return Win32FileNameStreamWriteModule(cnVar::Forward<TStreamWriteBuffer>(WriteBuffer),nullptr);
}
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
inline typename cnVar::TTypeConditional<DWORD,
	cnVar::TIsSame<wchar_t,typename TStreamWriteBuffer::tElement>::Value
>::Type Win32FileNameStreamWriteApplicationFolder(TStreamWriteBuffer&& WriteBuffer)noexcept(true)
{
	return Win32FileNameStreamWriteModuleFolder(cnVar::Forward<TStreamWriteBuffer>(WriteBuffer),nullptr);
}
//---------------------------------------------------------------------------
cnRTL::cStringBuffer<wchar_t> Win32FileMakeModuleName(HMODULE ModuleHandle)noexcept(true);
cStringBuffer<wchar_t> NTFileMakeApplicationFileName(void)noexcept(true);
cStringBuffer<wchar_t> NTFileMakeApplicationFolderName(void)noexcept(true);
//---------------------------------------------------------------------------
class cWin32FileNameEnum : public iFileEnumerator
{
public:
	cWin32FileNameEnum(cnRTL::cString<wchar_t> Path,cnRTL::cString<wchar_t> FindPath,iPtr<iFileName> (*CreateFileName)(cString<wchar_t> Path,const WIN32_FIND_DATA &CacheFileInfo)noexcept(true))noexcept(true);
	~cWin32FileNameEnum()noexcept(true);

	virtual bool cnLib_FUNC Fetch(void)noexcept(true)override;
	virtual iFile*	cnLib_FUNC GetCurrentFile(void)noexcept(true)override;

protected:
	static bool IsDot(const WIN32_FIND_DATA &fd)noexcept(true);
	static bool IsInvalidFile(const WIN32_FIND_DATA &fd)noexcept(true);
	cnRTL::cString<wchar_t> MakeFileName(const WIN32_FIND_DATA &fd)noexcept(true);

	bool FindNext(WIN32_FIND_DATA &fd)noexcept(true);
	
	iPtr<iFileName> fCurrentFileName;

private:
	HANDLE fFindHandle;
	iPtr<iFileName> (*fCreateFileName)(cString<wchar_t> Path,const WIN32_FIND_DATA &CacheFileInfo)noexcept(true);
	cnRTL::cString<wchar_t> fPath;
	cnRTL::cString<wchar_t> fFindPath;
	bool ProcessFindData(WIN32_FIND_DATA &fd)noexcept(true);
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
