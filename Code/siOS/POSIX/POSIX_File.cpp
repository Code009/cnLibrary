#include "POSIX_File.h"
#include "POSIX_Memory.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace siPOSIX;


#if 0


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int siPOSIX::ConvertOpenFileFlags(eFileAccess AccessMode,eFileCreate CreateFlag)
{
	int oflag=0;
	switch(AccessMode&(FileAccess::Read|FileAccess::Write)){
	default:
	case FileAccess::Read:
		oflag|=O_RDONLY;
		break;
	case FileAccess::Write:
		oflag|=O_WRONLY;
		break;
	case FileAccess::Read|FileAccess::Write:
		oflag|=O_RDWR;
		break;
	}

	switch(CreateFlag){
	default:
	case FileCreate::Open:
		break;
	case FileCreate::New:
		oflag|=O_CREAT|O_EXCL;
		break;
	case FileCreate::Create:
		oflag|=O_CREAT;
		break;
	}
	return oflag;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#ifdef	siOS_POSIX_ENABLE_CODESETCONVERT
//---------------------------------------------------------------------------
cICONVTextEncodingConverter::cICONVTextEncodingConverter(iconv_t ConvHandle)
	: fConvHandle(ConvHandle)
{
}
//---------------------------------------------------------------------------
cICONVTextEncodingConverter::~cICONVTextEncodingConverter()
{
	iconv_close(fConvHandle);
}
//---------------------------------------------------------------------------
uIntn cICONVTextEncodingConverter::Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)
{
	size_t inbytes=SrcSize;
	size_t outbytes=DestSize;
	size_t ret=iconv(fConvHandle,reinterpret_cast<char**>(const_cast<void**>(&Src)),&inbytes,reinterpret_cast<char**>(&Dest),&outbytes);
	if(static_cast<sIntn>(ret)<0){
		auto e=errno;
		if(e==E2BIG){
			if(SrcConvertedSize!=nullptr)
				*SrcConvertedSize=SrcSize-inbytes;
			return DestSize;
		}
		return 0;
	}

	if(SrcConvertedSize!=nullptr)
		*SrcConvertedSize=SrcSize-inbytes;
	return DestSize-outbytes;
}
//---------------------------------------------------------------------------
#endif	// siOS_POSIX_ENABLE_CODESETCONVERT
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cFileSeqStream::cFileSeqStream(int FileHandle)
{
	fFileHandle=FileHandle;
	fEndOfReading=false;
}
//---------------------------------------------------------------------------
cFileSeqStream::~cFileSeqStream()
{
}
//---------------------------------------------------------------------------
void cFileSeqStream::Close(void)
{
	close(fFileHandle);
}
//---------------------------------------------------------------------------
bool cFileSeqStream::IsEndOfReading(void)
{
	return fEndOfReading;
}
//---------------------------------------------------------------------------
bool cFileSeqStream::SetEndOfStream(void)
{
	off_t off=lseek(fFileHandle,0,SEEK_CUR);
	if(off<0)
		return false;
	return ftruncate(fFileHandle,off)==0;
}
//---------------------------------------------------------------------------
bool cFileSeqStream::Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)
{
	auto ret=read(fFileHandle,Buffer,Size);
	if(ret<0){
		SizeCompleted=0;
		return false;
	}
	if(ret==0){
		fEndOfReading=true;
	}
	SizeCompleted=ret;
	return true;
}
//---------------------------------------------------------------------------
bool cFileSeqStream::Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)
{
	auto ret=write(fFileHandle,Buffer,Size);
	if(ret>=0){
		SizeCompleted=ret;
		return true;
	}
	SizeCompleted=0;
	return false;
}
//---------------------------------------------------------------------------
uInt64 cFileSeqStream::GetPointer(void)
{
	off_t o=lseek(fFileHandle,0,SEEK_CUR);
	if(o==-1){
		return IndexNotFound;
	}
	return o;
}
//---------------------------------------------------------------------------
bool cFileSeqStream::MovePointerBegin(uInt64 Offset)
{
	off_t o=lseek(fFileHandle,Offset,SEEK_SET);
	return o!=-1;
}
//---------------------------------------------------------------------------
bool cFileSeqStream::MovePointerCurrent(sInt64 Offset)
{
	off_t o=lseek(fFileHandle,Offset,SEEK_CUR);
	return o!=-1;
}
//---------------------------------------------------------------------------
bool cFileSeqStream::MovePointerEnd(sInt64 Offset)
{
	off_t o=lseek(fFileHandle,Offset,SEEK_END);
	return o!=-1;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static cStringBuffer<char> CreateUnixFileName(const char *BasePath,uIntn BasePathLength,const uChar16*const *Path,uIntn Depth)
{

	cStringBuffer<char> FileName(BasePath,BasePathLength);
	cStringBuffer<char> NameBuffer;
	for(uIntn i=0;i<Depth;i++){
		NameBuffer.Clear();
		auto Converter=cnSystem::UnicodeTranscoder(1,2);
		cnString::ConvertEncodingToStream(cnStream::ArrayStreamWriteBuffer(NameBuffer.Storage()),Converter,Path[i],cnString::GetLength(Path[i]));
		uIntn Length8=NameBuffer.GetLength();
		if(Length8!=0){
			uIntn si=FileName.GetLength();
			FileName.Append(1+Length8);
			auto pStr=&FileName[si];
			*pStr='/';
			pStr++;

			cnMemory::Copy(pStr,NameBuffer.GetString(),Length8);
		}
	}
	return FileName;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cStringFileName::cStringFileName(cStringPath FilePath)
	: fFilePath(cnVar::MoveCast(FilePath))
{
}
//---------------------------------------------------------------------------
cStringFileName::~cStringFileName()
{
}
//---------------------------------------------------------------------------
eOrdering cStringFileName::Compare(const iFileName *Dest)const
{
	auto DestFileName=iCast<iPOSIXFileName>(Dest);
	if(DestFileName==nullptr)
		return Ordering::Different;



	sfInt8 Cmp=cnString::Compare(fFilePath.GetPath()->Pointer,DestFileName->GetPOSIXFileName());
	if(Cmp==0)
		return Ordering::Equal;
	if(Cmp<0)
		return Ordering::Less;
	return Ordering::Greater;
}
//---------------------------------------------------------------------------
rPtr<iFileEnumerator> cStringFileName::EnumFolder(void)
{
	auto &Path=fFilePath.GetPath();
	return FileSystemEnum(Path,Path->Length,nullptr);
}
//---------------------------------------------------------------------------
rPtr<iAsyncNotification> cStringFileName::WatchFolder(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iFileObserver> cStringFileName::ObserveFolder(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iFileSyncStream> cStringFileName::OpenSequentialStream(eFileAccess AccessMode,eFileCreate CreateFlag)
{
	return FileSystemOpenSeqStream(fFilePath,AccessMode,CreateFlag);
}
//---------------------------------------------------------------------------
iPtr<iFileStream> cStringFileName::OpenFileStream(eFileAccess AccessMode,eFileCreate CreateFlag)
{
	return POSIXInterface::FSOpenFileStream(fFilePath,AccessMode,CreateFlag);
}
//---------------------------------------------------------------------------
bool cStringFileName::IsExists(void)
{
	return FileSystemIsFileExists(fFilePath);
}
//---------------------------------------------------------------------------
uInt64 cStringFileName::GetDataSize(void)
{
	return FileSystemGetFileSize(fFilePath);
}
//---------------------------------------------------------------------------
bool cStringFileName::HasData(void)
{
	return true;
}
//---------------------------------------------------------------------------
bool cStringFileName::HasFolder(void)
{
	return true;
}
//---------------------------------------------------------------------------
bool cStringFileName::CreateFolder(void)
{
	if(::mkdir(fFilePath,0777)==0)
		return true;

	return false;
}
//---------------------------------------------------------------------------
uIntn cStringFileName::CreateFolderPath(void)
{
#pragma message ("TODO - CreateFolderPath")
	return uintMax;
}
//---------------------------------------------------------------------------
iPtr<iFileName> cStringFileName::ParentFileName(void)
{
	if(fFilePath.GetDepth()<=1)
		return nullptr;	// no parent
	return iCreate<cStringFileName>(fFilePath.MakeParentName());
}
//---------------------------------------------------------------------------
rPtr<iFileEnumerator> cStringFileName::EnumFileNames(const uChar16 *Filter)
{
	auto &Path=fFilePath.GetPath();
	return FileSystemEnum(Path,Path->Length,Filter);
}
//---------------------------------------------------------------------------
const uChar16* cStringFileName::GetName(void)
{
	return fFilePath.GetName();
}
//---------------------------------------------------------------------------
uIntn cStringFileName::GetNameLength(void)
{
	return fFilePath.GetName().GetLength();
}
//---------------------------------------------------------------------------
iPtr<iFileName> cStringFileName::MakeName(const uChar16*const *Path,uIntn Depth)
{
	auto NewFileName=fFilePath.MakeFileName(Path,Depth);
	if(NewFileName.GetPathLength()==0)
		return nullptr;
	return iCreate<cStringFileName>(cnVar::MoveCast(NewFileName));
}
//---------------------------------------------------------------------------
const uChar8* cStringFileName::GetPOSIXFileName(void)
{
	return fFilePath;
}
//---------------------------------------------------------------------------
uIntn cStringFileName::GetPOSIXFileNameLength(void)
{
	return fFilePath.GetPathLength();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cFolderEnum::cFolderEnum(FTS *FindHandle)
{
	fFindHandle=FindHandle;
}
//---------------------------------------------------------------------------
cFolderEnum::~cFolderEnum()
{
	fts_close(fFindHandle);
}
//---------------------------------------------------------------------------
iFile* cFolderEnum::GetCurrentFile(void)
{
	cStringBuffer<char> Name;
	Name.SetString(fCurrentFile->fts_name,fCurrentFile->fts_namelen);

	if(fCurrentFileName==nullptr){
		fCurrentFileName=iCreate<cStringFileName>(cnRTL::cString<char>(Name.Swap()));
	}
	return fCurrentFileName;
}
//---------------------------------------------------------------------------
bool cFolderEnum::Fetch(void)
{
	fCurrentFileName=nullptr;
	fCurrentFile=fts_read(fFindHandle);
	if(fCurrentFile==nullptr)
		return false;

	if(fCurrentFile->fts_info==FTS_DP){
		return Fetch();
	}
	if(fCurrentFile->fts_info==FTS_D){
		fts_set(fFindHandle,fCurrentFile,FTS_SKIP);
		// folder
	}
	else{
		// file
	}

	return true;
}
//---------------------------------------------------------------------------
int siPOSIX::FileSystemOpenFD(const char *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)
{
	int oflag=0;
	switch(AccessMode&(FileAccess::Read|FileAccess::Write)){
	default:
	case FileAccess::Read:
		oflag|=O_RDONLY;
		break;
	case FileAccess::Write:
		oflag|=O_WRONLY;
		break;
	case FileAccess::Read|FileAccess::Write:
		oflag|=O_RDWR;
		break;
	}

	switch(CreateFlag){
	default:
	case FileCreate::Open:
		break;
	case FileCreate::New:
		oflag|=O_CREAT|O_EXCL;
		break;
	case FileCreate::Create:
		oflag|=O_CREAT;
		break;
	}

	return open(FileName,oflag,S_IRWXU);
}
//---------------------------------------------------------------------------
iPtr<iFileSyncStream> siPOSIX::FileSystemOpenSeqStream(const char *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)
{
	int FileHandle=FileSystemOpenFD(FileName,AccessMode,CreateFlag);
	if(FileHandle==-1)
		return nullptr;

	return iCreate<siPOSIX::cFileSeqStream>(FileHandle);
}
//---------------------------------------------------------------------------
uInt64 siPOSIX::FileSystemGetFileSize(const char *FileName)
{
	struct stat fs;
	if(stat(FileName,&fs)!=0){
		// error;
	 	return 0;
	}
	return fs.st_size;
}
//---------------------------------------------------------------------------
bool siPOSIX::FileSystemIsFileExists(const char *FileName)
{
	if(access(FileName,F_OK)==0){
		// exists
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
rPtr<iSwapMemory>		siPOSIX::FileSystemCreateSwapMemoryFromTemporaryFile(uInt64 Size)
{
	static const char TempFileNameValue[]="/tmp/swap_file_XXXXX";
	char TempFileName[sizeof(TempFileNameValue)];

	cnMemory::Copy(TempFileName,TempFileNameValue,sizeof(TempFileNameValue));
	mkstemp(TempFileName);

	int FileOpenFlag=O_RDWR;
	int MemoryProtectFlag=PROT_READ|PROT_WRITE;

	int fh=open(TempFileName,FileOpenFlag,O_CREAT|S_IRWXU);
	if(fh<0)
		return nullptr;
	ftruncate(fh,Size);	// _XOPEN_VERSION should >=600  or increase file size will fail
	unlink(TempFileName);

	return rCreate<cMMapFile>(fh,static_cast<uIntn>(Size),MemoryProtectFlag);

}
//---------------------------------------------------------------------------
rPtr<iSwapMemory>		siPOSIX::FileSystemCreateSwapMemoryFromFile(const char *FileName,uInt64 Size,eFileAccess AccessMode)
{
	if(FileName==nullptr){
		return nullptr;
	}
	int FileOpenFlag;
	int MemoryProtectFlag;
	switch(AccessMode&(FileAccess::Read|FileAccess::Write)){
	default:
	case FileAccess::Read:
		FileOpenFlag=O_RDONLY;
		MemoryProtectFlag=PROT_READ;
		break;
	case FileAccess::Write:
		FileOpenFlag=O_WRONLY;
		MemoryProtectFlag=PROT_WRITE;
		break;
	case FileAccess::Read|FileAccess::Write:
		FileOpenFlag=O_RDWR;
		MemoryProtectFlag=PROT_READ|PROT_WRITE;
		break;
	}

	int fh=open(FileName,FileOpenFlag,0);
	if(fh<0)
		return nullptr;
	struct stat fs;
	fstat(fh,&fs);

	return rCreate<cMMapFile>(fh,static_cast<uIntn>(fs.st_size),MemoryProtectFlag);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rPtr<iFileEnumerator> siPOSIX::FileSystemEnum(const char *FolderName,uIntn FolderNameLength,const uChar16 *Filter)
{
	cString<char> TempName;

	if(Filter!=nullptr && Filter[0]!=0){
		TempName=CreateUnixFileName(FolderName,FolderNameLength,&Filter,1);
		FolderName=TempName;
	}
	char *const DirArray[2]={const_cast<char*>(FolderName),nullptr};
//	static constexpr int SearchOptions=FTS_PHYSICAL|FTS_NOSTAT;
	static constexpr int SearchOptions=FTS_PHYSICAL;
	auto FindHandle=fts_open(DirArray, SearchOptions,nullptr);
	if(FindHandle!=nullptr){
		auto FileInfo=fts_read(FindHandle);
		if(FileInfo->fts_info==FTS_D){
			return rCreate<cFolderEnum>(FindHandle);
		}
		fts_close(FindHandle);
	}

	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iFileName> siPOSIX::FileSystemCreateTemporaryFile(const char *FolderName)
{
	char TempFileTemplate[]="tmpXXXXXX.tmp";
	mkstemp(TempFileTemplate);

	cStringBuffer<char> Name=FolderName;
	Name+=TempFileTemplate;
	return iCreate<cStringFileName>(cString<char>(cnVar::MoveCast(Name)));
}
//---------------------------------------------------------------------------

#endif // 0
