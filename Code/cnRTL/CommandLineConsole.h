/*- cnRTL - Command Line Mode Console -------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-10-04                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_CommandLineConsole_H__
#define __cnLibrary_cnRTL_CommandLineConsole_H__
//---------------------------------------------------------------------------
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/String.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnRTL_CONSTVAR	constexpr
#define	cnRTL_CONSTEXPR_FUNC constexpr
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnRTL_CONSTEXPR_FUNC
#define	cnRTL_CONSTVAR	const
#endif // cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
template<class TCharacter>
class cCmdTextParser
{
public:
	void Reset(const TCharacter *Text,uIntn Length)noexcept(true){
		fText=Text;
		fLength=Length;
		i=0;
	}


	bool Run(void)noexcept(true){
		// skip whitespaces
		while(i<fLength){
			switch(fText[i]){
			case ' ':
			case '\t':
				// white space
				break;
			case '\'':
			case '\"':
				// string
				ParseString();
				return true;
			case '/':
			case '-':
				ParseIdentifier(true);
				return true;
			default:
				ParseIdentifier(false);
				return true;
			}
			i++;
		}
		return false;
	}
	cArray<const TCharacter>& Value(void)noexcept(true){	return fValue;	}

	bool operator () (void)noexcept(true){	return Run();	}
	cArray<const TCharacter>& operator * ()noexcept(true){	return fValue;	}
	cArray<const TCharacter>* operator -> ()noexcept(true){	return &fValue;	}
private:
	cArray<const TCharacter> fValue;


	const TCharacter *fText;
	uIntn fLength;
	uIntn i;

	void ParseIdentifier(bool Separator)noexcept(true){
		uIntn TokenStartIndex=i;
		i+=Separator;
		do{
			switch(fText[i]){
			case ' ':
			case '\t':
				// white space
				SetValueText(TokenStartIndex);
				i++;
				return;
			case '\'':
			case '\"':
				// string
				SetValueText(TokenStartIndex);
				return;
			case '-':
			case '/':
				// token separator
				SetValueText(TokenStartIndex);
				return;
			default:
				break;
			}
			i++;
		}while(i<fLength);
		// finish current token
		SetValueText(TokenStartIndex);
	}
	void ParseString(void)noexcept(true){
		TCharacter StringQuoteChar=fText[i];
		i++;
		uIntn TokenStartIndex=i;
		while(i<fLength){
			uIntn CurIndex=i++;
			TCharacter c=fText[CurIndex];
			if(c==StringQuoteChar){
				fValue.Pointer=fText+TokenStartIndex;
				fValue.Length=CurIndex-TokenStartIndex;
				return;
			}
		}
		// finish unfinished string
		fValue.Pointer=fText+TokenStartIndex;
		fValue.Length=i-TokenStartIndex;
	}

	void SetValueText(uIntn TokenStartIndex)noexcept(true){
		fValue.Pointer=fText+TokenStartIndex;
		fValue.Length=i-TokenStartIndex;
	}
};
//---------------------------------------------------------------------------
template<class TCharacter>
struct cStringUpperCaseView
{
	const TCharacter *Pointer;
	TCharacter operator *()const noexcept(true){
		TCharacter c=*Pointer;
		if(c>='a' && c<='z'){
			c+=static_cast<TCharacter>('A'-'a');
		}
		return c;
	}
	void operator ++()noexcept(true){
		Pointer++;
	}
	void operator --()noexcept(true){
		Pointer--;
	}
};
//---------------------------------------------------------------------------
class cCommandDispatcher
{
public:
	cCommandDispatcher()noexcept(true);
	~cCommandDispatcher()noexcept(true);

	void Execute(const uChar16 *Text,uIntn Length)noexcept(true);

	void Execute(cCmdTextParser<uChar16> &Parser)noexcept(true);

	void ClearCommand(void)noexcept(true);
	void ResetCommand(const uChar16 *Name)noexcept(true);
	cFunction<void (cCmdTextParser<uChar16>&)>& SetCommand(const uChar16 *Name)noexcept(true);

	cFunction<void (cCmdTextParser<uChar16>&)>& operator [](const uChar16 *Name)noexcept(true);
protected:
	
	static sfInt8 StringCompare(const cString<uChar16> &Str1,const cArray<const uChar16> &Str2)noexcept(true){
		cStringUpperCaseView<uChar16> Str2View={Str2.Pointer};
		return cnString::ViewCompare(Str1->Pointer,Str1->Length,Str2View,Str2.Length);
		/*if(Str1->Length==Str2.Length)
		if(Str1->Length<Str2.Length){
			sfInt8 r=cnString::ViewCompare(Str1->Pointer,Str2View,Str1->Length);
			if(r==0 && Str2.Pointer[Str1->Length]!=0){
				return -1;
			}
			return r;
		}
		else{
			sfInt8 r=cnString::ViewCompare(Str1->Pointer,Str2View,Str2.Length);
			if(r==0)
				return 1;
			return r;
		}*/
	}

	struct cCmdOrderOperator
	{
		typedef cString<uChar16> tItem;


		template<class TValue>
		static sfInt8 Compare(const tItem &Item,const TValue &Value)noexcept(true){
			cStringUpperCaseView<uChar16> ValueView={static_cast<const uChar16*>(Value)};
			return cnString::ViewCompare(Item->Pointer,ValueView,Item->Length+1);
		}
		
		static sfInt8 Compare(const tItem &Item,const cArray<const uChar16> &Array)noexcept(true){
			return StringCompare(Item,Array);
		}
	};
	cSeqMap<cString<uChar16>,cFunction<void (cCmdTextParser<uChar16>&)>,cCmdOrderOperator> fCmdMap;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class cnLib_INTERFACE iConsolePrompt : public iReference
{
public:
	virtual void cnLib_FUNC StartPrompt(iStringReference *Reference)noexcept(true)=0;
	virtual void cnLib_FUNC FinishPrompt(void)noexcept(true)=0;
	
	virtual void cnLib_FUNC UpdatePrompt(uIntn ChangedOffset)noexcept(true)=0;

	virtual uIntn cnLib_FUNC GetPromptCursorPos(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetPromptCursorPos(uIntn Pos)noexcept(true)=0;

	virtual bool cnLib_FUNC GetCursorVisible(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetCursorVisible(bool Visible)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cConsoleOutputWithPrompt : public iConsoleOutput, public iConsolePrompt
{
public:
	cConsoleOutputWithPrompt(rPtr<iConsoleOutput> ConsoleOutput)noexcept(true);
	~cConsoleOutputWithPrompt()noexcept(true);

	typedef iConsoleOutput tReferenceInterface;

	// iConsoleOutput

	virtual cUIPoint cnLib_FUNC GetPageSize(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC Print(const uChar16 *Text,uIntn Length)noexcept(true)override;
	virtual cUIPoint cnLib_FUNC GetCursorPos(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetCursorPos(cUIPoint Pos)noexcept(true)override;
	virtual bool cnLib_FUNC GetCursorVisible(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetCursorVisible(bool Visible)noexcept(true)override;
	virtual void cnLib_FUNC MoveCursor(eDirection Direction,ufInt16 Count)noexcept(true)override;
	virtual void cnLib_FUNC MoveCursorLineHead(bool Up,ufInt16 Count)noexcept(true)override;
	virtual void cnLib_FUNC EraseLine(void)noexcept(true)override;
	virtual void cnLib_FUNC EraseLinePart(bool AfterCursor)noexcept(true)override;

	// iConsolePrompt

	virtual void cnLib_FUNC StartPrompt(iStringReference *Reference)noexcept(true)override;
	virtual void cnLib_FUNC FinishPrompt(void)noexcept(true)override;

	virtual void cnLib_FUNC UpdatePrompt(uIntn ChangedOffset)noexcept(true)override;

	virtual uIntn cnLib_FUNC GetPromptCursorPos(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetPromptCursorPos(uIntn Pos)noexcept(true)override;

protected:
	rPtr<iConsoleOutput> fConsoleOutput;

	rPtr<iStringReference> fLineText;
	cUIPoint fPromptPos;
	uIntn fPromptCursorPos;
	uIntn fPromptLength;

	cUIPoint OffsetConsolePos(cUIPoint Pos,sIntn Offset)noexcept(true);

	cUIPoint LinePosToConsolePos(uIntn Offset)noexcept(true);
	uIntn ConsolePosToLinePos(cUIPoint Pos)noexcept(true);

	void ApplyPromptCursorPos(void)noexcept(true);


	// clear prompt and restore cursor position
	void ClearPrompt(void)noexcept(true);
	// print prompt and set prompt cursor
	void RestorePrompt(const uChar16 *Text,uIntn Length)noexcept(true);

	struct cPromptSaveState
	{
		cPromptSaveState(cConsoleOutputWithPrompt *Owner)noexcept(true);
		~cPromptSaveState()noexcept(true);
		
		cConsoleOutputWithPrompt *Owner;
		bool Cleared;
		cArray<uChar16> LineText;

		operator bool()noexcept(true);
	};
};
//---------------------------------------------------------------------------
class bcConsoleLineInputHandler : public iConsoleInputHandler
{
public:
	bcConsoleLineInputHandler(iConsolePrompt *ConsolePrompt)noexcept(true);
	~bcConsoleLineInputHandler()noexcept(true);

	virtual void cnLib_FUNC KeyInput(eKeyCode KeyCode)noexcept(true)override;
	virtual void cnLib_FUNC TextInput(const uChar16 *Text,uIntn Length)noexcept(true)override;
protected:

	rPtr<iConsolePrompt> fConsolePrompt;

	virtual void cnLib_FUNC LineInput(const uChar16 *Line,uIntn Length)noexcept(true)=0;

private:
	class cStringBufferRef : public cStringBuffer<uChar16>, public iStringReference
	{
	public:
		virtual cArray<const uChar16> cnLib_FUNC Get()noexcept(true) override{
			cArray<const uChar16> Ret;
			Ret.Pointer=this->GetString();
			Ret.Length=this->GetLength();
			return Ret;
		}
	};
	rPtr<cStringBufferRef> fLineBuffer;
	cSeqList<uIntn> fLineTextPos;
	cUIPoint fLineEchoPos;
	uIntn fEditPos;
};
//---------------------------------------------------------------------------
#if 0
class bcConsoleLineInputHandler : public iConsoleInputHandler
{
public:
	virtual void cnLib_FUNC KeyInput(eKeyCode KeyCode)override;
	virtual void cnLib_FUNC TextInpuChar16uChar8 *Text,uIntn Length)override;
protected:
	virtual iConsolePrompt* LineConsolePrompt(void)=0;
	virtual void cnLib_FUNC LineInput(const uChar16 *Line,uIntn Length)=0;

	void ClearLineEcho(void);
	void RestoreLineEcho(void);
private:
	cStringBuffer<uChar16> fLineBuffer;
	cSeqList<uIntn> fLineTextPos;
	cUIPoint fLineEchoPos;
	uIntn fEditPos;

	static cUIPoint CalculateOffsetPos(cUIPoint Pos,cUIPoint Size,sIntn Offset);
	void UpdateCaretPos(sIntn Offset);
	void UpdateLineEcho(void);
};
#endif // 0
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#undef	cnRTL_CONSTEXPR_FUNC
#undef	cnRTL_CONSTVAR
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
