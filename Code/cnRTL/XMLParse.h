/*- cnRTL - XML Parse -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2022-06-16                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_XMLParse_H__
#define __cnLibrary_cnRTL_XMLParse_H__
//---------------------------------------------------------------------------
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/ObjectReference.h>
#include <cnRTL/UIEvent.h>
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
struct cTokenPosition
{
	uIntn Line;
	uIntn Index;

	void Reset(void)noexcept(true){
		Line=0;
		Index=0;
	}

	void MoveLine(uIntn LineCount,uIntn CharIndex=0)noexcept(true){
		Line=Line+LineCount;	Index=CharIndex;
	}
	void MoveCharacter(uIntn CharCount){		Index=Index+CharCount;	}
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,XMLTokenType)
{
	WhiteSpace,		// white space
	Text,			// Plain Text
	CDATA,			// CDATA section
	Comment,		// <!--Comment-->
	InvalidTag,		// invalid tag <????? ...>
	TagBegin,		// <Name
	CloseTagBegin,	// </Name
	TagEnd,			// >
	CloseTagEnd,	// />
	Attribute,		// AttributeName
	Equ,			// =
	String,			// "String"
}cnLib_ENUM_END(XMLTokenType);
//---------------------------------------------------------------------------
struct cXMLToken
{
	eXMLTokenType Type;
	bool InvalidText		:1;	// invalid text or tag name or attribute name
	bool UnfinishedString	:1;	// unfinished string
	cString<uChar32> Text;
	uIntn FromLine;
	uIntn FromCharacter;
	uIntn ToLine;
	uIntn ToCharacter;
};
//---------------------------------------------------------------------------
class iXMLTagLexicalAnalyzer
{
public:
	enum{
		Recognizing,
		Reject,
		Accept,
	};
	virtual void Start(void)noexcept(true)=0;
	virtual ufInt8 Recognize(ufInt8 TagCharacterType,uChar32 Character)noexcept(true)=0;
	virtual void Discard(void)noexcept(true)=0;
	virtual bool TakeOver(void)noexcept(true)=0;
	struct cTransition
	{
		bool Stay;
		bool Entity;
		bool Finished;
	};
	virtual void Input(cTransition &Transition,uChar32 Character)noexcept(true)=0;
	virtual void InputFinish(void)noexcept(true)=0;

	virtual void InputEscapeEntity(const uChar32 *Text,uIntn Length)noexcept(true)=0;
};
//---------------------------------------------------------------------------
struct cXMLLexicalOptions
{
	bool MergeWhitespaceIntoText		:1;
	bool MergeErrorIntoText				:1;
	bool AcceptInvalidTagName			:1;
	bool AcceptInvalidAttributeName		:1;
	bool InvalidTagAsText				:1;

	void SetDefault(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cXMLLexicalAnalyzer
{
public:
	cXMLLexicalAnalyzer()noexcept(true);
	~cXMLLexicalAnalyzer()noexcept(true);

	cFunction<cArray<const uChar32>(cArray<const uChar32>)> TranslateEntity;

	cXMLLexicalOptions Options;

	void SetOptionsDefault(void)noexcept(true);

	void Reset(void)noexcept(true);

	cOrderedObjectSet<iXMLTagLexicalAnalyzer*,ufInt16> TagAnalyzerSet;

	struct cTransition
	{
		bool Stay;
		bool Output;
	};

	cTransition Input(uChar32 Character)noexcept(true);
	cTransition InputFinish(void)noexcept(true);
	cXMLToken& Output(void)noexcept(true);

	struct cXMLCharacterEntity
	{
		const char *Name;
		ufInt8 MatchLength;
		uChar32 Character;
	};
	static const cXMLCharacterEntity XMLCharacterEntities[];
	static cArray<const uChar32> TranslateXMLEntity(cArray<const uChar32> Name);
private:

	enum eState{
		sSpace,
		sText,
		sTextError,
		sTagRecognize,
		sTagRecognizeWithRollback,
		sTagRecognizeCommited,
		sTagCore,
		sTagExternal,
	}fState;

	bool fTextMergeSpace;
	bool fTextError;
	bool fTextTagRollbackWhitespace;

	ufInt8 fCoreRecognizeCount;
	enum{
		tsBegin,
		tsName,
		tsTag,
		tsCloseTagBegin,
		tsInvalid,
		tsAttribute,
		tsString,
		tsEnd,
		tsComment,
		tsCDATA,
		tsFinish,
		tsIgnore,
		tsIgnoreEnd,
		tsIgnorePI,
		tsIgnorePIEnd,
		tsAcceptWS,
		tsAcceptTag,
		tsAcceptEnd,
		tsReject,
		tsRejectIgnore,
	}fCoreTagState;
	static const char CoreComment_TagName[];
	static const char CoreCData_TagName[];
	static const char CoreComment_TagEnd[];
	static const char CoreCData_TagEnd[];
	ufInt8 fCoreCommentRecognizeState;
	ufInt8 fCoreCDataRecognizeState;
	enum{
		esNone,
		esRef,
		esName,
		esNumber,
		esNumber10,
		esNumber16,
	}fEscapeState;

	enum{
		TagBeginType_Normal,
		TagBeginType_Close,
	};

	enum{
		TagEndType_Close,
	};
	enum{
		TagEndRollback_Tag,
		TagEndRollback_TagName,
		TagEndRollback_Invalid,
		TagEndRollback_Attribute,
	};

	union{
		struct{
			ufInt8 EndLength;
		}fCommentState;
		struct{
			ufInt8 EndLength;
		}fCDATAState;
		struct{
			ufInt8 Type;
			bool InvalidName;
		}fTagBeginState;
		struct{
			bool InvalidName;
		}fAttributeState;
		struct{
			bool IsDQ;
		}fStringState;
	};

	struct{
		ufInt8 Type;
		ufInt8 Rollback;
	}fTagEndState;

	cSeqSet<iXMLTagLexicalAnalyzer*> fTagRecognizeSet;
	cSeqList<iXMLTagLexicalAnalyzer*> fTagRecognizeList;

	cXMLToken fValue;

	uChar32 fEscapeEntityNumber;
	cStringBuffer<uChar32> fEscapeEntityAcceptBuffer;

	uIntn fTextTagRollbackLength;

	iXMLTagLexicalAnalyzer *fExternalTagLexicalAnalyzer;
	cStringBuffer<uChar32> fAcceptBuffer;

	cTokenPosition fLastPos;
	cTokenPosition fCurPos;

	cTokenPosition fTextOutputPos;
	cTokenPosition fTagOutputPos;
	cTokenPosition fTagRollbackPos;


	enum{
		CharacterType_Error			=0,
		CharacterType_Whitespace	=1,
		CharacterType_Text			=2,
		CharacterType_Name			=2,
		CharacterType_NameInitial	=3,
		CharacterType_TagLeft		=4,		// <
		CharacterType_TagRight		=5,		// >
		CharacterType_Equ			=6,		// =
		CharacterType_StringQ		=7,		// "
		CharacterType_StringA		=8,		// '
		CharacterType_Reference		=9,		// &
		CharacterType_TagPI			=10,	// ?
		CharacterType_TagEnd		=11,	// /
		CharacterType_TagDTD		=12,	// !
	};

	static const ufInt8 TextCharacterTypeMap[];
	static const uChar32 TextCharacterTypeRangeList[];
	static const ufInt8 TextCharacterTypeRangeListType[];
	static ufInt8 ClassifyTextCharacter(uChar32 c)noexcept(true);

	static const ufInt8 TagCharacterClassMap[0x100];
	struct cTagCharTypeRange
	{
		uInt16 Char;
		uInt8 Type;
	};
	static const cTagCharTypeRange TagCharacterTypeRangeList[];
	static ufInt8 ClassifyTagCharacter(uChar32 c)noexcept(true);

	enum{
		trRecognizing,
		trAcceptExternal,
		trAcceptCore,
		trReject
	};
	ufInt8 TagRecognize(uChar32 c)noexcept(true);

	void StateTranslate_Space(cTransition &Transition,uChar32 c)noexcept(true);
	void StateTranslate_Space(cTransition &Transition,ufInt8 ct,uChar32 c)noexcept(true);
	void StateTranslate_Text(cTransition &Transition,uChar32 c)noexcept(true);
	void StateTranslate_Text(cTransition &Transition,ufInt8 ct,uChar32 c)noexcept(true);
	void StateTranslate_TextError(cTransition &Transition,uChar32 c)noexcept(true);
	void StateTranslate_TagRecognize(cTransition &Transition,uChar32 c)noexcept(true);
	void StateTranslate_TagRecognizeWithRollback(cTransition &Transition,uChar32 c)noexcept(true);
	bool StateTranslate_TagRecognize_Commit(cTransition &Transition)noexcept(true);
	void StateTranslate_TagRecognize_CoreAccept(cTransition &Transition,uChar32 c)noexcept(true);
	void StateTranslate_TagRecognize_ExternalAccept(cTransition &Transition,uChar32 c)noexcept(true);
	void StateTranslate_TagCore(cTransition &Transition,uChar32 c)noexcept(true);
	void StateTranslate_TagExternal(cTransition &Transition,uChar32 c)noexcept(true);

	void EscapeEntity_Begin(void)noexcept(true);
	bool EscapeEntity_Process(bool &CharacterStay,uChar32 Character)noexcept(true);
	void EscapeEntity_Terminate(void)noexcept(true);
	void EscapeEntity_ApplyText(void)noexcept(true);

	cStringBuffer<uChar32> fCoreTagAcceptBuffer;
	void CoreTag_Start(void)noexcept(true);
	void CoreTag_Discard(void)noexcept(true);
	bool CoreTag_Recognize(ufInt8 TagCharacterType,uChar32 Character)noexcept(true);
	bool CoreTag_TakeOver(void)noexcept(true);
	void CoreTag_Terminate(cTransition &Transition)noexcept(true);
	void CoreTag_CloseTagBegin(cTransition &Transition,uChar32 c)noexcept(true);
	void CoreTag_Name(cTransition &Transition,uChar32 c)noexcept(true);
	bool CoreTag_End(cTransition &Transition,uChar32 c)noexcept(true);
	bool CoreTag_Tag(cTransition &Transition,uChar32 c)noexcept(true);
	bool CoreTag_Invalid(cTransition &Transition,uChar32 c)noexcept(true);
	void CoreTag_Attribute(cTransition &Transition,uChar32 c)noexcept(true);
	void CoreTag_String(cTransition &Transition,uChar32 c)noexcept(true);
	void CoreTag_InputEscapeEntity(void)noexcept(true);
	bool CoreTag_Comment(cTransition &Transition,uChar32 c)noexcept(true);
	bool CoreTag_CDATA(cTransition &Transition,uChar32 c)noexcept(true);

	void OutputToken(eXMLTokenType Type,bool InvalidText,const cTokenPosition &StartPosition,const cTokenPosition &EndingPosition,cStringBuffer<uChar32> &Buffer)noexcept(true);
	void OutputEmpty(eXMLTokenType Type,bool InvalidText,const cTokenPosition &StartPosition,const cTokenPosition &EndingPosition)noexcept(true);
	void OutputTagBegin(const cTokenPosition &StartingPosition,const cTokenPosition &EndingPosition)noexcept(true);
	void OutputTagEnd(const cTokenPosition &StartingPosition,const cTokenPosition &EndingPosition)noexcept(true);
	void OutputUnfinishedString(const cTokenPosition &StartingPosition,const cTokenPosition &EndingPosition)noexcept(true);
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,XMLStatementType){
	Text,			// Text
	TagStart,		// <Name...
	TagNamespace,	// tag namespace change
	Attribute,		// Name="Text"
	TagFinish,		// ...> or .../>
	TagClose,		// </Name...> or />
	TagCloseMismatch,	// close tag without matching tag start
}cnLib_ENUM_END(XMLStatementType);
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,XMLTextStatementType){
	WhiteSpace,		// ...
	Text,			// ...
	CDATA,			// <[CDATA[ ... ]]>
	Comment,		// <!-- ... -->
	InvalidTag,		// <??? ...>
}cnLib_ENUM_END(XMLTextStatementType);
//---------------------------------------------------------------------------
struct cXMLStatement
{
	eXMLStatementType Type;
	union{
		struct{
			eXMLTextStatementType Type;
			bool InvalidText		:1;	// invalid character in Text
		}TextFlags;
		struct{
			bool InvalidName		:1;	// invalid Tag name
		}TagStartFlags;
		struct{
			bool InvalidName		:1;	// invalid character in attribute name
			bool MissingEqualSign	:1;	// missing =
			bool InvalidValue		:1;	// invalid character in attribute value
			bool UnfinishedString	:1;	// unfinished attribute value string
		}AttributeFlags,TagNamespaceFlags;
		struct{
			bool InvalidTagFinish	:1;	// close finish at close tag  </.../>
			bool UnfinishedNode		:1;	// flag is set in TagClose, means the node is unclosed
		}TagCloseFlags;
	
	};
	void ClearFlags(void)noexcept(true);

	cString<uChar32> URI;		// namespace URI
	cString<uChar32> Prefix;	// namespace prefix
	cString<uChar32> Name;		// node name without prefix
	cString<uChar32> Text;		// whitespace, text, CDATA, comment, attribute value
	cSeqList<cXMLToken> Tokens;

};
//---------------------------------------------------------------------------
struct cXMLSyntaxOptions
{
	bool AcceptAttributeNameAsValue				:1;
	bool IgnoreAttributeMultipleEqualSign		:1;
	bool AcceptAttributeWithoutName				:1;
	bool AcceptCloseTagAttributes				:1;
	bool AcceptCloseTagPrefixMismatch			:1;

	void SetDefault(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cXMLSyntaxAnalyzer
{
public:
	cXMLSyntaxAnalyzer()noexcept(true);
	~cXMLSyntaxAnalyzer()noexcept(true);

	void Reset(void);

	cXMLSyntaxOptions Options;
	void SetOptionsDefault(void)noexcept(true);

	struct cTransition
	{
		bool Stay;
		bool Output;
	};

	cTransition Input(const cXMLToken &Token)noexcept(true);
	cTransition InputFinish(void)noexcept(true);
	cXMLStatement& Output(void)noexcept(true);

private:
	cXMLStatement fValue;

	enum{
		sText,
		sTag,
		sCloseTag,
		sCloseTagWithAttribute,
		sMismatchedCloseTag,
		sFinish
	}fState;

	enum{
		tsNone,
		tsAttribute,
		tsAttributeEqu,
		tsAttributeEquA,
	}fTagState;

	cXMLToken fAttributeNameToken;
	cXMLToken fAttributeEquToken;
	cXMLToken fAttributeValueToken;
	cSeqList<cXMLToken> fTagTokenList;
	cStringBuffer<uChar32> fInvalidTagText;

	void StateTranslate_Text(cTransition &Transition,const cXMLToken &Token)noexcept(true);
	void StateTranslate_Tag(cTransition &Transition,const cXMLToken &Token)noexcept(true);
	void StateTranslate_CloseTag(cTransition &Transition,const cXMLToken &Token)noexcept(true);
	void StateTranslate_CloseTagWithAttribute(cTransition &Transition,const cXMLToken &Token)noexcept(true);
	void StateTranslate_MismatchedCloseTag(cTransition &Transition,const cXMLToken &Token)noexcept(true);

	void OpenTag(cString<uChar32> TagName)noexcept(true);
	void CloseTag(void)noexcept(true);

	bool TagStateTranslate(cTransition &Transition,const cXMLToken &Token)noexcept(true);
	bool TagStateTranslate_None(cTransition &Transition,const cXMLToken &Token)noexcept(true);
	void TagStateTranslate_Attribute(cTransition &Transition,const cXMLToken &Token)noexcept(true);
	void TagStateTranslate_AttributeEqu(cTransition &Transition,const cXMLToken &Token)noexcept(true);
	void TagStateTranslate_AttributeEquA(cTransition &Transition,const cXMLToken &Token)noexcept(true);

	bool TagStateTranslate_ProcessFinish(void)noexcept(true);
	bool TagStateTranslate_ProcessAttribute(const cXMLToken &NameToken,const cXMLToken &EquToken,const cXMLToken &ValueToken)noexcept(true);
	bool TagStateTranslate_ProcessAttribute(const cXMLToken &NameToken,const cXMLToken &EquToken)noexcept(true);

	void OutputTextNode(eXMLTextStatementType TextType,const cXMLToken &Token)noexcept(true);
	void OutputTagStart(const cXMLToken &Token)noexcept(true);
	void OutputTagFinish(const cXMLToken &Token)noexcept(true);
	void OutputTagClose(const cXMLToken &Token,bool InvalidTagFinish)noexcept(true);
	void OutputTagClose(const cSeqList<cXMLToken> &TokenList,bool InvalidTagFinish,bool UnfinishedNode)noexcept(true);
	void OutputMismatchedTagClose(cString<uChar32> Text,const cSeqList<cXMLToken> &TokenList)noexcept(true);
	void OutputAttributeNoName(const cXMLToken &EquToken,const cXMLToken &ValueToken)noexcept(true);

	struct cXMLNodeStackItem : cRTLAllocator
	{
		cXMLNodeStackItem *Parent;
		cString<uChar32> TagPrefix;
		cString<uChar32> TagName;
		cString<uChar32> TagNamespace;
		cString<uChar32> DefaultNamespace;
		cSeqMap< cString<uChar32>,cString<uChar32> > NamespacePrefixMap;

		cString<uChar32> FindNamespace(const cString<uChar32> &Prefix)noexcept(true);
		cString<uChar32> QueryName(cString<uChar32> &Prefix,cString<uChar32> &Name)noexcept(true);
	};
	cXMLNodeStackItem *fCurItem;
	cXMLNodeStackItem fRootItem;

	static bool SplitNamespace(cString<uChar32> &Prefix,cString<uChar32> &Name)noexcept(true);
};
//---------------------------------------------------------------------------
class iXMLVisitor
{
public:
	virtual void Close(void)noexcept(true)=0;
	virtual void TagFinish(void)noexcept(true)=0;
	virtual void TagNamespace(cString<uChar16> Namespace)noexcept(true)=0;
	virtual void Attribute(cString<uChar16> Namespace,cString<uChar16> Prefix,cString<uChar16> Name,cString<uChar16> Value)noexcept(true)=0;

	virtual iXMLVisitor* Element(cString<uChar16> Namespace,cString<uChar16> Prefix,cString<uChar16> Name)noexcept(true)=0;
	virtual void Text(eXMLTextStatementType TextType,cString<uChar16> Text)noexcept(true)=0;
	virtual void Comment(eXMLTextStatementType TextType,cString<uChar16> Text)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cXMLParser
{
public:
	cXMLParser()noexcept(true);
	~cXMLParser()noexcept(true);

	cXMLLexicalOptions& RefLexicalOptions(void)noexcept(true);
	cXMLSyntaxOptions& RefSyntaxOptions(void)noexcept(true);

	void Reset(iXMLVisitor *Visitor)noexcept(true);

	template<class TTextGenerator>
	void Input(TTextGenerator &TextGenerator)noexcept(true){
		do{
			auto c=*TextGenerator;
			Input(c);
		}while(TextGenerator());
	}

	void Input(uChar32 c)noexcept(true);

	void Finish(void)noexcept(true);

private:
	cXMLLexicalAnalyzer fLexer;
	cXMLSyntaxAnalyzer fSyntaxer;

	void Syntax(void)noexcept(true);

	void Grammer(void)noexcept(true);

	rPtr<iTextEncodingConverter> fConverter;

	bool fRootNodeFinished=false;

	struct cXMLNodeStackItem
	{
		cXMLNodeStackItem *Parent;
		iXMLVisitor *Visitor;
	};
	cXMLNodeStackItem *fCurItem;
	cXMLNodeStackItem fRootItem;
};
//---------------------------------------------------------------------------
struct cXMLAttribute
{
	cString<uChar16> Namespace;
	cString<uChar16> Prefix;
	cString<uChar16> Name;
	cString<uChar16> Value;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,XMLNodeType){
	Text,			// Text
	Comment,		// comment or other text
	Element,		// xml element
}cnLib_ENUM_END(XMLNodeType);
//---------------------------------------------------------------------------
struct cXMLElement;
//---------------------------------------------------------------------------
struct cXMLNode
{
	eXMLNodeType Type;
	eXMLTextStatementType TextType;
	cString<uChar16> Text;				// Text or Comment
	aClsRef<cXMLElement> Element;		// element
};
//---------------------------------------------------------------------------
struct cXMLElement
{
	cString<uChar16> Namespace;
	cString<uChar16> Prefix;
	cString<uChar16> Name;

	cSeqMap< cString<uChar16>,aClsRef<cXMLAttribute> > AttributeMap;
	cSeqList<cXMLNode> AllNodes;

	cSeqList< cString<uChar16> > Texts;			// texts
	cSeqList< cString<uChar16> > Comments;		// comments
	cSeqMap< cString<uChar16>,cSeqList< aClsRef<cXMLElement> > > Children;	// child elements

	void Visit(iXMLVisitor *Visitor)noexcept(true);
};
//---------------------------------------------------------------------------
class cXMLElementMaker : public iXMLVisitor
{
public:
	cXMLElementMaker()noexcept(true);

	aClsRef<cXMLElement> Fetch(void)noexcept(true);

	virtual void Close(void)noexcept(true)override;
	virtual void TagFinish(void)noexcept(true)override;
	virtual void TagNamespace(cString<uChar16> Namespace)noexcept(true)override;
	virtual void Attribute(cString<uChar16> Namespace,cString<uChar16> Prefix,cString<uChar16> Name,cString<uChar16> Value)noexcept(true)override;

	virtual iXMLVisitor* Element(cString<uChar16> Namespace,cString<uChar16> Prefix,cString<uChar16> Name)noexcept(true)override;
	virtual void Text(eXMLTextStatementType TextType,cString<uChar16> Text)noexcept(true)override;
	virtual void Comment(eXMLTextStatementType TextType,cString<uChar16> Text)noexcept(true)override;

private:
	cXMLElementMaker(cXMLElementMaker *ParentMaker)noexcept(true);

	aClsRef<cXMLElement> fElement;
	cXMLElementMaker *fParentMaker;
	bool fTagFinished;

	void ChildClosed(cXMLElementMaker *SubElementMaker)noexcept(true);
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
