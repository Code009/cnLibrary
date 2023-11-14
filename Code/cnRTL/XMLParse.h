/*- cnRTL - XML Parse -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2022-06-16                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_XMLParse_H__
#define __cnLibrary_cnRTL_XMLParse_H__
//---------------------------------------------------------------------------
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/ObjectReference.h>
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
#endif	// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
template<class TCharacter>
class cTokenAcceptBuffer
{
public:

	void Reset(void){	fBuffer.Clear();	fOutputLine=0;	fOutputCharacter=0;	fLine=0;	fCharacter=0;	}

	void PosMoveLine(uIntn LineCount,uIntn CharIndex=0){
		fLastLine=fLine;	fLastCharacter=fCharacter;
		fLine=fLine+LineCount;	fCharacter=CharIndex;
	}
	void PosMoveCharacter(uIntn CharCount){		fLastLine=fLine;	fLastCharacter=fCharacter; fCharacter=fCharacter+CharCount;	}
	void PosRollback(void){	fLine=fLastLine;	fCharacter=fLastCharacter;	}

	void PosGetLast(uIntn &Line,uIntn &Character){		Line=fLastLine;	Character=fLastCharacter;	}
	void PosGet(uIntn &Line,uIntn &Character){		Line=fLine;	Character=fCharacter;	}

	void PosOutputGet(uIntn &Line,uIntn &Character){		Line=fOutputLine;	Character=fOutputCharacter;	}
	void PosOutputSet(void){		fOutputLine=fLine;	fOutputCharacter=fCharacter;	}
	void PosOutputSetLast(void){	fOutputLine=fLastLine;	fOutputCharacter=fLastCharacter;	}

	uIntn GetLength(void)const{		return fBuffer->Length;	}
	void Accept(TCharacter Char){		fBuffer.AppendChar(Char);	}
	void Accept(const TCharacter *String,uIntn Length){	fBuffer.Append(String,Length);	}
	void Truncate(uIntn Count){		fBuffer.Truncate(Count);	}
	cString<TCharacter> Fetch(void){	return cnVar::MoveCast(fBuffer);	}
	void Discard(void){		fBuffer.Clear();	}

protected:
	cStringBuffer<TCharacter> fBuffer;
	uIntn fOutputLine;
	uIntn fOutputCharacter;
	uIntn fLastLine;
	uIntn fLastCharacter;
	uIntn fLine;
	uIntn fCharacter;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,XMLTokenType)
{
	Error,			// $#@%$#%$#
	Text,			// Plain Text / CDATA
	Comment,		// <!--Comment-->
	TagBegin,		// <Name
	PITagBegin,		// <?Name
	DTDTagBegin,	// <!Name
	CloseTagBegin,	// </Name
	TagEnd,			// >
	EmptyTagEnd,	// />
	PITagEnd,		// ?>
	Attribute,		// AttributeName
	Equ,			// =
	String,			// "String"
}cnLib_ENUM_END(XMLTokenType);
//---------------------------------------------------------------------------
struct cXMLToken
{
	eXMLTokenType Type;
	cString<uChar32> Text;
	uIntn FromLine;
	uIntn FromCharacter;
	uIntn ToLine;
	uIntn ToCharacter;
};
//---------------------------------------------------------------------------
class cXMLLexicalAnalyzer
{
public:
	cFunction<cArray<const uChar32>(cArray<const uChar32>)> TranslateEntity;
	bool PreserveWhitespaceInText		:1;
	bool IgnoreAttribute				:1;
	//bool OutputDetailToken				:1;
	bool AcceptInvalidTagName			:1;
	//bool BypassErrorToken			:1;

	void SetOptionsDefault(void);

	void Reset(void);

	struct cTransition
	{
		bool CharacterStay;
		bool Output;
	};

	cTransition Input(uChar32 Character);
	cTransition InputFinish(void);
	cXMLToken& Output(void);

	struct cXMLCharacterEntity
	{
		const char *Name;
		ufInt8 MatchLength;
		uChar32 Character;
	};
	static const cXMLCharacterEntity XMLCharacterEntities[];
	static cArray<const uChar32> TranslateXMLEntity(cArray<const uChar32> Name);
private:

	enum{
		sText,
		sTextRef,
		sComment,
		sTagBegin,
		sTagDTD,
		sTagComment,
		sTagNameInitial,
		sTagName,
		sTag,
		sTagIgnore,
		sTagEnd,

		sAttribute,
		sString,
		sStringRef,
	}fState;
	enum{
		srStart,
		srName,
		srNumber,
		srNumber10,
		srNumber16,
	}fRefState;

	enum{
		TagBeginType_Normal,
		TagBeginType_PI,
		TagBeginType_DTD,
		TagBeginType_Close,
	};

	enum{
		TagEndType_Normal,
		TagEndType_PI,
		TagEndType_Empty,
	};

	union{
		struct{
			ufInt8 EndLength;
		}fCommentState;
		struct{
			ufInt8 Type;
			bool NameInitial;
		}fTagBeginState;
		struct{
			ufInt8 Type;
		}fTagEndState;
		struct{
			bool InvalidName;
		}fAttributeState;
		struct{
			bool IsDQ;
		}fStringState;
	};

	cXMLToken fValue;
	cStringBuffer<uChar32> fRefText;
	uChar32 fRefNumber;

	struct cParseContext
	{
		cArray<const uChar32> Buffer;
		uIntn ProcessIndex;
	};

	enum{
		CharacterType_Error			=0,
		CharacterType_Whitespace	=1,
		CharacterType_Text			=2,
		CharacterType_Name			=2,
		CharacterType_NameInitial	=3,
		CharacterType_TagLeft		=4,
		CharacterType_TagRight		=5,
		CharacterType_Equ			=6,
		CharacterType_StringQ		=7,
		CharacterType_StringA		=8,
		CharacterType_Reference		=9,
		CharacterType_TagPI			=10,
		CharacterType_TagEnd		=11,
	};

	static const ufInt8 TextCharacterTypeMap[];
	static const uChar32 TextCharacterTypeRangeList[];
	static const ufInt8 TextCharacterTypeRangeListType[];
	static ufInt8 ClassifyTextCharacter(uChar32 c);

	static const ufInt8 TagCharacterClassMap[0x100];
	struct cTagCharTypeRange
	{
		uInt16 Char;
		uInt8 Type;
	};
	static const cTagCharTypeRange TagCharacterTypeRangeList[];
	static ufInt8 ClassifyTagCharacter(uChar32 c);


	cTokenAcceptBuffer<uChar32> fAcceptBuffer;

	bool StateReferenceTranslate(cTransition &Transition,uChar32 c);
	void StateTranslate_Text(cTransition &Transition,uChar32 c);
	void StateTranslate_TextRef(cTransition &Transition,uChar32 c);
	void StateTranslate_Comment(cTransition &Transition,uChar32 c);
	void StateTranslate_TagBegin(cTransition &Transition,uChar32 c);
	void StateTranslate_TagComment(cTransition &Transition,uChar32 c);
	void StateTranslate_TagName(cTransition &Transition,uChar32 c);
	void StateTranslate_Tag(cTransition &Transition,uChar32 c);
	void StateTranslate_TagIgnore(cTransition &Transition,uChar32 c);
	void StateTranslate_Attribute(cTransition &Transition,uChar32 c);
	void StateTranslate_String(cTransition &Transition,uChar32 c);
	void StateTranslate_StringRef(cTransition &Transition,uChar32 c);

	void WriteTagBeginLast(void);
	void OutputTagBeginLast(void);
	void OutputTagEndCur(void);
	void OutputAcceptedLast(eXMLTokenType Type);
	void OutputAcceptedCur(eXMLTokenType Type);
	void WriteAttributeLast(void);
	void OutputAttributeLast(void);
	void OutputEmptyLast(eXMLTokenType Type);
	void OutputEmptyCur(eXMLTokenType Type);
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,XMLSymbolType)
{
	Error,		// @#$#@%$#@
	Text,		// Value
	Comment,	// <!--Value-->
	TagStart,	// <Name...
	TagFinish,	// ...>
	Attribute,	// Name="Value"
}cnLib_ENUM_END(XMLSymbolType);
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,XMLSymbolTagType)
{
	Error,			// $#@%$#%$#
	Normal,			// <Name	>
	PI,				// <?Name	---
	DTD,			// <!Name	---
	Close,			// </Name	/>
}cnLib_ENUM_END(XMLSymbolTagType);
//---------------------------------------------------------------------------
struct cXMLSymbol
{
	eXMLSymbolType Type;
	XMLSymbolTagType TagType;
	cString<uChar32> Name;
	cString<uChar32> Value;
};
//---------------------------------------------------------------------------
class cXMLSyntaxAnalyzer
{
public:
	void Reset(void);

	struct cTransition
	{
		bool Output;
	};

	cTransition Input(const cXMLToken &Token);
	cTransition InputFinish(void);
	cXMLSymbol& Output(void);

private:
	cXMLSymbol fValue;


	enum{
		sText,
		sTag,
		sAttribute,
		sAttributeEqu,
	}fState;

	enum{
		ttNormal,
		ttClose,
		ttDTD,
		ttPI,
	}fTagType;

	cString<uChar32> fAttributeName;

	void StateTranslate_Text(cTransition &Transition,const cXMLToken &Token);
	void StateTranslate_Tag(cTransition &Transition,const cXMLToken &Token);
	void StateTranslate_Attribute(cTransition &Transition,const cXMLToken &Token);
	void StateTranslate_AttributeEqu(cTransition &Transition,const cXMLToken &Token);

	void OutputTextToken(eXMLSymbolType Type,const cXMLToken &Token);
	void OutputTagStart(eXMLSymbolTagType TagType,const cXMLToken &Token);
	void OutputTagFinish(eXMLSymbolTagType TagType);
	void OutputAttributeError(void);
	void OutputAttribute(const cXMLToken &Token);
};
//---------------------------------------------------------------------------
class cXMLTokenParser
{
public:
	iReadBuffer<uChar32> *InputTextStream;
	cXMLLexicalAnalyzer *Lexer;

	bool Run(void);
	cXMLToken& Value(void);

	bool operator () (void);
	cXMLToken& operator * ();
private:

	bool ParseInput(const uChar32 *Text,uIntn Length,uIntn &ProcessedLength);
};
//---------------------------------------------------------------------------
class cXMLSymbolParser
{
public:
	iReadBuffer<uChar32> *InputTextStream;
	cXMLLexicalAnalyzer *Lexer;
	cXMLSyntaxAnalyzer *Syntaxer;

	bool Run(void);
	cXMLSymbol& Value(void);

	bool operator () (void);
	cXMLSymbol& operator * ();
private:
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,XMLNodeElementType)
{
	Element,	// <Element ...>
	Entity,		// <!ENTITY ...>
	PI,			// <?PI ...?>
	Notation,	// <!NOTATION ...>
	DocType,	// <!DOCTYPE ...>
	XMLDecl,	// <?xml ...?>
}cnLib_ENUM_END(XMLNodeElementType);
//---------------------------------------------------------------------------
struct cXMLAttribute
{
	cString<uChar8> Namespace;
	cString<uChar8> Name;
	cString<uChar8> Value;
};
struct cXMLNodeElement
{
	cString<uChar8> Namespace;
	cString<uChar8> Name;
	cSeqMap< cString<uChar8>,cString<uChar8> > Attributes;
};
//---------------------------------------------------------------------------
struct cXMLNode
{
	cString<uChar8> Name;
	cSeqList< aClsRef<cXMLAttribute> > Attributes;


	cSeqList< cString<uChar8> > Texts;
	cSeqList< cString<uChar8> > Comments;
	cSeqList< aClsRef<cXMLNode> > Children;
	cSeqMap< cString<uChar8>,cSeqList< aClsRef<cXMLNode> > > NamedChildren;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,XMLNodeTextType)
{
	Text,
	CDATA,
	Comment,		// <!--Comment-->
}cnLib_ENUM_END(XMLNodeTextType);
//---------------------------------------------------------------------------
struct cXMLNodeText
{
	cString<uChar8> Text;
};
//---------------------------------------------------------------------------
struct cXMLDocument : cXMLNode
{
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,XMLParseOutputType)
{
	Error,
	Element,		// cXMLNode::Name		| <Element...
	ElementDone,	// cXMLNode::Attributes	| <Element />
	ElementBegin,	// cXMLNode::Attributes	| <Element >
	ElementEnd,		// </Element>
	Text,			// Text
	Comment,		// <!--Comment-->
}cnLib_ENUM_END(XMLParseOutputType);
//---------------------------------------------------------------------------
class cXMLParser
{
public:
	void SetInput(cXMLSymbolParser *SymbolParser);
	void Reset(void);

	bool Run(void);
	cXMLToken& Value(void);

	bool operator () (void);
	cXMLToken& operator * ();
private:
	cXMLToken fValue;


	cXMLSymbolParser *fSymbolParser;
	enum{
		sText,
		sElement,
	}fState;

	bool ParseText(cXMLToken &Token);
	bool ParseElement(cXMLToken &Token);


	bool ParseElementID(void);
	bool ParseElementString(void);

	void SetResultID(uIntn Start,uIntn End);

	
	struct cXMLNodeStackItem
	{
		cXMLNodeStackItem *Parent;
		aClsRef<cXMLNode> Node;
	};
	cXMLNodeStackItem *fCurNodeStack;
	aClsRef<cXMLNode> fCurNode;
};
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
