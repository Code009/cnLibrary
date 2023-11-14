/*- cnRTL - Logger --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2007-08-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_Logger_H__
#define __cnLibrary_cnRTL_Logger_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLCore.h>
#include <cnRTL/RuntimeFunction.h>
#include <cnRTL/String.h>
#include <cnRTL/ObjectReference.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
struct cLogContent
{
	uInt8 Level;
	cTime Time;
	cString<uChar16> Name;
	cString<uChar16> Type;
	cString<uChar16> Text;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iLogger : public iReference
{
public:
	virtual void cnLib_FUNC Log(aClsConstRef<cLogContent> Content)=0;
};
//---------------------------------------------------------------------------
class cTextStreamOutputLogger : public iLogger
{
public:
	cTextStreamOutputLogger(iTextStreamOutput *Output);
	virtual void cnLib_FUNC Log(aClsConstRef<cLogContent> Content)override;
protected:
	rPtr<iTextStreamOutput> fOutput;
};
//---------------------------------------------------------------------------
class cLoggerSwitch : public iLogger
{
public:
	cLoggerSwitch();
	~cLoggerSwitch();

	void InsertLogger(iLogger *Logger);
	void RemoveLogger(iLogger *Logger);
	void ClearLogger(void);
	bool IsEmpty(void)const;

	virtual void cnLib_FUNC Log(aClsConstRef<cLogContent> Content)override;
protected:
	rPtr<iSharedMutex> fLoggerListLock;
	cSeqSet< rPtr<iLogger> > fLoggerList;
};
//---------------------------------------------------------------------------
class cLogWriter
{
public:
	cLogWriter()noexcept(true);
	~cLogWriter()noexcept(true);

	iLogger* GetLogger(void)const;
	void SetLogger(iLogger *Logger);

	const cString<uChar16>& GetName(void)const;
	void SetName(cString<uChar16> Name);

	void Write(uInt8 Level,cString<uChar16> Text);
	void Write(uInt8 Level,cString<uChar16> Type,cString<uChar16> Text);

	template<class...TArgs>
	void Write(uInt8 Level,const uChar16 *FormatString,TArgs...Args){
		if(fLogger==nullptr)
			return;
		
		Write(Level,CreateStringFormat<uChar16>(FormatString,Args...));
	}

	class cTextBlock
	{
	public:
		cTextBlock(iLogger *Logger,uInt8 Level,cString<uChar16> Name,cString<uChar16> Type);
		~cTextBlock();

		void Commit(void);

		template<class...TArgs>
		void Write(const uChar16 *FormatString,TArgs...Args){
			StringStream::WriteFormatString(fText.StreamWriteBuffer(),FormatString,Args...);
		}
	private:
		rPtr<iLogger> fLogger;
		cString<uChar16> fName;
		cString<uChar16> fType;
		aClsRef<cLogContent> fContent;
		cStringBuffer<uChar16> fText;
	};
	cTextBlock Block(uInt8 Level);
	cTextBlock Block(uInt8 Level,cString<uChar16> Type);
protected:
	rPtr<iLogger> fLogger;
	cString<uChar16> fName;
};
//---------------------------------------------------------------------------
class cLogVoidWriter
{
public:
	iLogger* GetLogger(void)const{	return nullptr;	}
	void SetLogger(iLogger*){}

	cString<uChar16> GetName(void)const{	return nullptr;	}
	void SetName(const cString<uChar16>&){}

	cString<uChar16> GetType(void)const{	return nullptr;	}
	void SetType(const cString<uChar16>&){}

	template<class...TArgs>
	static void Write(uInt8,TArgs...){}

	class cTextBlock
	{
	public:
		static void Commit(void){}

		template<class...TArgs>
		static void Write(uInt8,TArgs...){}
	};

	cTextBlock Block(uInt8){					return {};	}
	cTextBlock Block(uInt8,cString<uChar16>){	return {};	}
};
//---------------------------------------------------------------------------
}   // namespace cnRTL
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
