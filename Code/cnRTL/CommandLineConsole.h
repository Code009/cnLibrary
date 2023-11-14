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
	void Reset(const TCharacter *Text,uIntn Length){
		fText=Text;
		fLength=Length;
		i=0;
	}


	bool Run(void){
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
	cArray<const TCharacter>& Value(void){	return fValue;	}

	bool operator () (void){	return Run();	}
	cArray<const TCharacter>& operator * (){	return fValue;	}
	cArray<const TCharacter>* operator -> (){	return &fValue;	}
private:
	cArray<const TCharacter> fValue;


	const TCharacter *fText;
	uIntn fLength;
	uIntn i;

	void ParseIdentifier(bool Separator){
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
	void ParseString(void){
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

	void SetValueText(uIntn TokenStartIndex){
		fValue.Pointer=fText+TokenStartIndex;
		fValue.Length=i-TokenStartIndex;
	}
};
//---------------------------------------------------------------------------
template<class TCharacter>
struct cStringUpperCaseView
{
	const TCharacter *Pointer;
	TCharacter operator *()const{
		TCharacter c=*Pointer;
		if(c>='a' && c<='z'){
			c+=static_cast<TCharacter>('A'-'a');
		}
		return c;
	}
	void operator ++(){
		Pointer++;
	}
	void operator --(){
		Pointer--;
	}
};
//---------------------------------------------------------------------------
class cCommandDispatcher
{
public:
	cCommandDispatcher();
	~cCommandDispatcher();

	void Execute(const uChar16 *Text,uIntn Length);

	void Execute(cCmdTextParser<uChar16> &Parser);

	void ClearCommand(void);
	void ResetCommand(const uChar16 *Name);
	cFunction<void (cCmdTextParser<uChar16>&)>& SetCommand(const uChar16 *Name);

	cFunction<void (cCmdTextParser<uChar16>&)>& operator [](const uChar16 *Name);
protected:
	
	static sfInt8 StringCompare(const cString<uChar16> &Str1,const cArray<const uChar16> &Str2){
		cStringUpperCaseView<uChar16> Str2View={Str2.Pointer};
		if(Str1->Length==Str2.Length)
			return cnString::ViewCompare(Str1->Pointer,Str1->Length,Str2View);
		if(Str1->Length<Str2.Length){
			sfInt8 r=cnString::ViewCompare(Str1->Pointer,Str1->Length,Str2View);
			if(r==0 && Str2.Pointer[Str1->Length]!=0){
				return -1;
			}
			return r;
		}
		else{
			sfInt8 r=cnString::ViewCompare(Str1->Pointer,Str2.Length,Str2View);
			if(r==0)
				return 1;
			return r;
		}
	}

	struct cCmdOrderOperator
	{
		typedef cString<uChar16> tItem;


		template<class TValue>
		static sfInt8 Compare(const tItem &Item,const TValue &Value){
			cStringUpperCaseView<uChar16> ValueView={static_cast<const uChar16*>(Value)};
			return cnString::ViewCompare(Item->Pointer,Item.GetLength()+1,ValueView);
		}
		
		static sfInt8 Compare(const tItem &Item,const cArray<const uChar16> &Array){
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
	virtual void cnLib_FUNC StartPrompt(iArrayReference<uChar16> *Reference)=0;
	virtual void cnLib_FUNC FinishPrompt(void)=0;
	
	virtual void cnLib_FUNC UpdatePrompt(uIntn ChangedOffset)=0;

	virtual uIntn cnLib_FUNC GetPromptCursorPos(void)=0;
	virtual bool cnLib_FUNC SetPromptCursorPos(uIntn Pos)=0;

	virtual bool cnLib_FUNC GetCursorVisible(void)=0;
	virtual bool cnLib_FUNC SetCursorVisible(bool Visible)=0;
};
//---------------------------------------------------------------------------
class cConsoleOutputWithPrompt : public iConsoleOutput, public iConsolePrompt
{
public:
	cConsoleOutputWithPrompt(rPtr<iConsoleOutput> ConsoleOutput);
	~cConsoleOutputWithPrompt();

	typedef iConsoleOutput tReferenceInterface;

	// iConsoleOutput

	virtual cUIPoint cnLib_FUNC GetPageSize(void)override;
	virtual uIntn cnLib_FUNC Print(const uChar16 *Text,uIntn Length)override;
	virtual cUIPoint cnLib_FUNC GetCursorPos(void)override;
	virtual bool cnLib_FUNC SetCursorPos(cUIPoint Pos)override;
	virtual bool cnLib_FUNC GetCursorVisible(void)override;
	virtual bool cnLib_FUNC SetCursorVisible(bool Visible)override;
	virtual void cnLib_FUNC MoveCursor(eDirection Direction,ufInt16 Count)override;
	virtual void cnLib_FUNC MoveCursorLineHead(bool Up,ufInt16 Count)override;
	virtual void cnLib_FUNC EraseLine(void)override;
	virtual void cnLib_FUNC EraseLinePart(bool AfterCursor)override;

	// iConsolePrompt

	virtual void cnLib_FUNC StartPrompt(iArrayReference<uChar16> *Reference)override;
	virtual void cnLib_FUNC FinishPrompt(void)override;

	virtual void cnLib_FUNC UpdatePrompt(uIntn ChangedOffset)override;

	virtual uIntn cnLib_FUNC GetPromptCursorPos(void)override;
	virtual bool cnLib_FUNC SetPromptCursorPos(uIntn Pos)override;

protected:
	rPtr<iConsoleOutput> fConsoleOutput;

	rPtr< iArrayReference<uChar16> > fLineText;
	cUIPoint fPromptPos;
	uIntn fPromptCursorPos;
	uIntn fPromptLength;

	cUIPoint OffsetConsolePos(cUIPoint Pos,sIntn Offset);

	cUIPoint LinePosToConsolePos(uIntn Offset);
	uIntn ConsolePosToLinePos(cUIPoint Pos);

	void ApplyPromptCursorPos(void);


	// clear prompt and restore cursor position
	void ClearPrompt(void);
	// print prompt and set prompt cursor
	void RestorePrompt(const uChar16 *Text,uIntn Length);

	struct cPromptSaveState
	{
		cPromptSaveState(cConsoleOutputWithPrompt *Owner);
		~cPromptSaveState();
		
		cConsoleOutputWithPrompt *Owner;
		bool Cleared;
		cArray<uChar16> LineText;

		operator bool();
	};
};
//---------------------------------------------------------------------------
class bcConsoleLineInputHandler : public iConsoleInputHandler
{
public:
	bcConsoleLineInputHandler(iConsolePrompt *ConsolePrompt);
	~bcConsoleLineInputHandler();

	virtual void cnLib_FUNC KeyInput(eKeyCode KeyCode)override;
	virtual void cnLib_FUNC TextInput(const uChar16 *Text,uIntn Length)override;
protected:

	rPtr<iConsolePrompt> fConsolePrompt;

	virtual void cnLib_FUNC LineInput(const uChar16 *Line,uIntn Length)=0;

private:
	class cStringBufferRef : public cStringBuffer<uChar16>, public iArrayReference<uChar16>
	{
	public:
		virtual uChar16* cnLib_FUNC GetArray(uIntn &Length)noexcept(true) override{
			Length=this->GetLength();
			return this->GetString();
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
