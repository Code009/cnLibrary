#include "WinFileName.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const wchar_t cnWinRTL::Win32FileNameInvalidFileChar[]={'<','>',':','"','/','\\','|','?','*'};
const uIntn cnWinRTL::Win32FileNameInvalidFileCharCount=cnMemory::ArrayLength(Win32FileNameInvalidFileChar);
const uIntn cnWinRTL::Win32FileNameInvalidFilterCharCount=Win32FileNameInvalidFileCharCount-2;
//---------------------------------------------------------------------------
bool cnWinRTL::Win32FileNameCheckIsDot(const wchar_t *Name)
{
	if(Name==nullptr)
		return false;
	if(*Name++!='.')
		return false;
	while(*Name++=='.');
	return *Name==0;
}
//---------------------------------------------------------------------------
DWORD cnWinRTL::Win32FileNameCheckFileName(const wchar_t *Name)
{
	if(Name==nullptr || Name[0]==0)
		return 0;

	// check for dot
	DWORD Index=0;
	while(Name[Index++]=='.');
	if(Name[Index]==0){
		// is dot name, not valid
		return 0;
	}
	// check for invalid symbols
	do{
		if(cnMemory::Search(Win32FileNameInvalidFileChar,Win32FileNameInvalidFileCharCount,Name[Index])!=IndexNotFound){
			// contain invalid character
			return 0;
		}
	}while(Name[++Index]!=0);
	return Index;
}
//---------------------------------------------------------------------------
cStringBuffer<wchar_t> cnWinRTL::NTFileNormalizeName(const wchar_t *Path,uIntn PathLength)
{
	static const wchar_t Prefix[3]={'\\','\\','?'};
	cStringBuffer<wchar_t> Result;
	Result.SetCapacity(PathLength);
	Result.Append(Prefix,ArrayLength(Prefix));
	// file name
	uIntn PathComponent=0;
	auto Process=[&](uIntn i){
		uIntn Length=i-PathComponent;
		switch(Length){
		case 0:
			break;
		case 1:
			if(Path[i]=='.'){
				// ignore the component
				break;
			}
			cnLib_SWITCH_FALLTHROUGH;
		case 2:
			if(Path[i]=='.' && Path[i+1]=='.'){
				// ignore and remove last component
				auto DestString=Result.GetString();
				uIntn DestComponent=Result.GetLength();
				while(DestString[DestComponent]!='\\'){
					if(DestComponent<=4)
						break;
					DestComponent--;
				}
				break;
			}
			cnLib_SWITCH_FALLTHROUGH;
		default:
			// copy path
			Result.AppendChar('\\');
			Result.Append(Path+PathComponent,Length);
		}
		PathComponent=i+1;
	};
	for(uIntn i=0;i<PathLength;i++){
		switch(Path[i]){
		case '\\':
		case '/':
			Process(i);
			break;
		}
	}
	Process(PathLength);
	return Result;
}
//---------------------------------------------------------------------------
cStringBuffer<wchar_t> cnWinRTL::Win32FileMakeName(const wchar_t *BasePath,uIntn BasePathLength,const uChar16*const *Path,uIntn Depth)
{
	cStringBuffer<wchar_t> Dest(BasePath,BasePathLength);
	// get lengths
	{
		auto WriteBuffer=Dest.StreamWriteBuffer();
		for(uIntn i=0;i<Depth;i++){

			auto *CurPath=reinterpret_cast<const wchar_t*>(Path[i]);
			DWORD Length=Win32FileNameStreamAppend(WriteBuffer,CurPath);
			if(Length==0)
				return nullptr;
		}
	}
	return Dest;
}
//---------------------------------------------------------------------------
cStringBuffer<wchar_t> cnWinRTL::Win32FileMakeFindName(const wchar_t *Path,uIntn PathLength,const wchar_t *Filter)
{
	uIntn AcceptableLength=0;
	if(Filter!=nullptr){
		// check for invalid symbols
		for(AcceptableLength=0;Filter[AcceptableLength]!=0;AcceptableLength++){
			if(cnMemory::Search(Win32FileNameInvalidFileChar,Win32FileNameInvalidFilterCharCount,Filter[AcceptableLength])!=IndexNotFound){
				// contain invalid character
				break;
			}
		}
	}
	cStringBuffer<wchar_t> FindPath(Path,PathLength);
	if(AcceptableLength==0){
		static const wchar_t Suffix[2]={'\\','*'};
		FindPath.Append(Suffix,2);
	}
	else{
		FindPath.AppendChar('\\');
		FindPath.Append(Filter,AcceptableLength);
	}

	return FindPath;
}
//---------------------------------------------------------------------------
cStringBuffer<wchar_t> cnWinRTL::Win32FileMakeTemporaryFileName(const wchar_t *FolderName,const wchar_t *Prefix)
{
	wchar_t TempFileName[MAX_PATH+1];
	const wchar_t *TempPrefix=Prefix;
	if(Prefix==nullptr){
		TempPrefix=L"TMP";
	}

	if(GetTempFileNameW(FolderName,TempPrefix,0,TempFileName)==0)
		return nullptr;
	// make copy
	uIntn Length=cnString::FindLength(TempFileName);
	cStringBuffer<wchar_t> Name(TempFileName,Length,Length+1);
	return Name;
}
//---------------------------------------------------------------------------
cStringBuffer<wchar_t> cnWinRTL::NTFileMakeTempFolderName(void)
{
	wchar_t TempPath[MAX_PATH+1];

	uIntn TempPathLength=GetTempPathW(MAX_PATH,TempPath);

	return NTFileNormalizeName(TempPath,TempPathLength);
}
//---------------------------------------------------------------------------
cnRTL::cStringBuffer<wchar_t> cnWinRTL::Win32FileMakeModuleName(HMODULE ModuleHandle)
{
	cStringBuffer<wchar_t> Path;
	uIntn PathLength;
	uIntn Size=cnMath::IntRoundUp(MAX_PATH,256);
	do{
		Path.SetLength(Size);
		PathLength=::GetModuleFileNameW(ModuleHandle,Path->Pointer,static_cast<DWORD>(Size));
		if(PathLength==0){
			// error
			Path.Clear();
			break;
		}
		Size*=2;
	}while(::GetLastError()==ERROR_INSUFFICIENT_BUFFER);

	Path.SetLength(PathLength);
	return Path;
}
//---------------------------------------------------------------------------
cStringBuffer<wchar_t> cnWinRTL::NTFileMakeApplicationFileName(void)
{
	cStringBuffer<wchar_t> Path=Win32FileMakeModuleName(nullptr);
	return NTFileNormalizeName(Path->Pointer,Path->Length);
}
//---------------------------------------------------------------------------
cStringBuffer<wchar_t> cnWinRTL::NTFileMakeApplicationFolderName(void)
{
	cStringBuffer<wchar_t> Path=Win32FileMakeModuleName(nullptr);
	for(uIntn i=Path->Length;i>0;i--){
		if(Path[i]=='\\'){
			Path.SetLength(i);
			break;
		}
	}

	return NTFileNormalizeName(Path->Pointer,Path->Length);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWin32FileNameEnum::cWin32FileNameEnum(cnRTL::cString<wchar_t> Path,cnRTL::cString<wchar_t> FindPath,iPtr<iFileName> (*CreateFileName)(cString<wchar_t> Path,const WIN32_FIND_DATA &CacheFileInfo))
	: fPath(cnVar::MoveCast(Path))
	, fFindPath(cnVar::MoveCast(FindPath))
	, fFindHandle(INVALID_HANDLE_VALUE)
	, fCreateFileName(CreateFileName)
{
}
//---------------------------------------------------------------------------
cWin32FileNameEnum::~cWin32FileNameEnum()
{
	if(fFindHandle!=INVALID_HANDLE_VALUE)
		FindClose(fFindHandle);
}
//---------------------------------------------------------------------------
bool cWin32FileNameEnum::IsDot(const WIN32_FIND_DATA &fd)
{
	if((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0)
		return false;
	
	if(fd.cAlternateFileName[0]!=0)
		return false;
	if(fd.cFileName[0]=='.'){
		if(fd.cFileName[1]=='.'){
			if(fd.cFileName[2]==0){
				return true;
			}
		}
		else if(fd.cFileName[1]==0){
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool cWin32FileNameEnum::IsInvalidFile(const WIN32_FIND_DATA &fd)
{
	if(IsDot(fd))
		return true;
	return false;
}
//---------------------------------------------------------------------------
cnRTL::cString<wchar_t> cWin32FileNameEnum::MakeFileName(const WIN32_FIND_DATA &fd)
{
	// create file name
	auto pFileName=wtou(fd.cFileName);
	return Win32FileMakeName(fPath,fPath.GetLength(),&pFileName,1);
}
//---------------------------------------------------------------------------
bool cWin32FileNameEnum::ProcessFindData(WIN32_FIND_DATA &fd)
{
	// supress . and ..
	while(IsInvalidFile(fd)){
		if(FindNextFileW(fFindHandle,&fd)==FALSE){
			return false;
		}
	}
	return true;
}
//---------------------------------------------------------------------------
bool cWin32FileNameEnum::FindNext(WIN32_FIND_DATA &fd)
{
	if(fFindHandle==INVALID_HANDLE_VALUE){
		fFindHandle=FindFirstFileW(fFindPath,&fd);
		if(fFindHandle!=INVALID_HANDLE_VALUE){
			return ProcessFindData(fd);
		}
	}
	else{
		if(FindNextFileW(fFindHandle,&fd)){
			return ProcessFindData(fd);
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool cWin32FileNameEnum::Fetch(void)
{
	WIN32_FIND_DATA FindData;

	if(FindNext(FindData)==false)
		return false;

	// create file name
	fCurrentFileName=(*fCreateFileName)(MakeFileName(FindData),FindData);

	return true;
}
//---------------------------------------------------------------------------
iFile*	cWin32FileNameEnum::GetCurrentFile(void)
{
	return fCurrentFileName;
}
//---------------------------------------------------------------------------
