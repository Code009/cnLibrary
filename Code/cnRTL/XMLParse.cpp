//---------------------------------------------------------------------------
#include "XMLParse.h"
#include "TextProcess.h"

using namespace cnLibrary;
using namespace cnRTL;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::SetOptionsDefault(void)
{
	TranslateEntity=TranslateXMLEntity;
	PreserveWhitespaceInText=false;
	IgnoreAttribute=false;
	//OutputDetailToken=false;
	AcceptInvalidTagName=false;
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::Reset(void)
{
	fState=sText;
	fRefState=srStart;
	fAcceptBuffer.Reset();
}
//---------------------------------------------------------------------------
cXMLToken& cXMLLexicalAnalyzer::Output(void){	return fValue;	}
//---------------------------------------------------------------------------
cXMLLexicalAnalyzer::cTransition cXMLLexicalAnalyzer::Input(uChar32 Character)
{
	switch(Character){
	case '\n':
		fAcceptBuffer.PosMoveLine(1);
		break;
	default:
		fAcceptBuffer.PosMoveCharacter(1);
		break;
	}
	cTransition Transition;
	Transition.CharacterStay=false;
	Transition.Output=false;

	switch(fState){
	case sText:
		StateTranslate_Text(Transition,Character);
		break;
	case sTextRef:
		StateTranslate_TextRef(Transition,Character);
		break;
	case sComment:
		StateTranslate_Comment(Transition,Character);
		break;
	case sTagBegin:
		StateTranslate_TagBegin(Transition,Character);
		break;
	case sTagDTD:
		if(Character=='-'){
			fState=sTagComment;
			break;
		}
		fState=sTagName;
		Transition.CharacterStay=true;
		break;
	case sTagComment:
		StateTranslate_TagComment(Transition,Character);
		break;
	case sTagName:
		StateTranslate_TagName(Transition,Character);
		break;
	case sTag:
		if(IgnoreAttribute==false){
			StateTranslate_Tag(Transition,Character);
			break;
		}
		fState=sTagIgnore;
		cnLib_SWITCH_FALLTHROUGH;
	case sTagIgnore:
		StateTranslate_TagIgnore(Transition,Character);
		break;
	case sTagEnd:
		if(Character=='>'){
			OutputTagEndCur();
			fState=sText;
			Transition.Output=true;
		}
		else{
			// error
			fState=sTag;
			Transition.CharacterStay=true;
			fAcceptBuffer.Discard();
			fAcceptBuffer.PosOutputSetLast();
			// reinput as tag
		}
		break;
	case sAttribute:
		StateTranslate_Attribute(Transition,Character);
		break;
	case sString:
		StateTranslate_String(Transition,Character);
		break;
	case sStringRef:
		StateTranslate_StringRef(Transition,Character);
		break;
	}
	if(Transition.CharacterStay){
		fAcceptBuffer.PosRollback();
	}
	return Transition;
}
//---------------------------------------------------------------------------
cXMLLexicalAnalyzer::cTransition cXMLLexicalAnalyzer::InputFinish(void)
{
	fAcceptBuffer.PosMoveLine(1);

	cTransition Transition;
	Transition.CharacterStay=false;
	Transition.Output=false;
	switch(fState){
	case sText:
		if(fAcceptBuffer.GetLength()!=0){
			OutputAcceptedLast(XMLTokenType::Text);
			Transition.Output=true;
		}
		break;
	case sTextRef:
		fAcceptBuffer.Accept(fRefText->Pointer,fRefText->Length);
		if(fAcceptBuffer.GetLength()!=0){
			OutputAcceptedLast(XMLTokenType::Text);
			Transition.Output=true;
		}
		break;
	case sComment:
		// unfinished tag
		OutputAcceptedLast(XMLTokenType::Comment);
		Transition.Output=true;
		break;
	case sTagBegin:
		// unfinished empty tag
		OutputEmptyLast(XMLTokenType::TagBegin);
		Transition.Output=true;
		break;
	case sTagDTD:	// unfinished empty DTD tag
	case sTagComment:// unfinished commit tag '<!-', treat as error DTD
		OutputEmptyLast(XMLTokenType::DTDTagBegin);
		Transition.Output=true;
		break;
	case sTagName:
		// unfinished Tag start
		OutputTagBeginLast();
		Transition.Output=true;
		break;
	case sTag:
	case sTagIgnore:
		// unfinished tag
		break;
	case sTagEnd:
		// expecting tag ending
		break;
	case sAttribute:
		// output attribute name
		OutputAttributeLast();
		Transition.Output=true;
		break;
	case sString:
		// unfinished string
		OutputAcceptedLast(XMLTokenType::Error);
		Transition.Output=true;
		break;
	case sStringRef:
		// unfinished string
		fAcceptBuffer.Accept(fRefText->Pointer,fRefText->Length);
		OutputAcceptedLast(XMLTokenType::Error);
		Transition.Output=true;
		break;
	}
	fState=sText;
	return Transition;
}
//---------------------------------------------------------------------------
const ufInt8 cXMLLexicalAnalyzer::TextCharacterTypeMap[0x3D]={
//	0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F
	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	1,	0,	0,	1,	0,	0,		// 0x10
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 0x20
//		!	"	#	$	%	&	'	(	)	*	+	,	-	.	/
	1,	2,	2,	2,	2,	2,	9,	2,	2,	2,	2,	2,	2,	2,	2,	2,		// 0x30
//	0	1	2	3	4	5	6	7	8	9	:	;	<	=	>	?
	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	3,	2,	4,
};
const uChar32 cXMLLexicalAnalyzer::TextCharacterTypeRangeList[]={
	0xD800,
	0xE000,
	0xFFFE,
	0x10000,
	0x11000
};

const ufInt8 cXMLLexicalAnalyzer::TextCharacterTypeRangeListType[ArrayLength(TextCharacterTypeRangeList)]={
	CharacterType_Text,		// 0xD800,
	CharacterType_Error,	// 0xE000,
	CharacterType_Text,		// 0xFFFE,
	CharacterType_Error,	// 0x10000,
	CharacterType_Text,		// 0x11000
};

ufInt8 cXMLLexicalAnalyzer::ClassifyTextCharacter(uChar32 c)
{
	if(c<ArrayLength(TextCharacterTypeMap)){
		return TextCharacterTypeMap[c];
	}
	const uIntn TextCharacterTypeRangeListCount=ArrayLength(TextCharacterTypeRangeList);
	for(uIntn i=0;i<TextCharacterTypeRangeListCount;i++){
		if(c<TextCharacterTypeRangeList[i]){
			return TextCharacterTypeRangeListType[i];
		}
	}
	return CharacterType_Error;
}
//---------------------------------------------------------------------------
const ufInt8 cXMLLexicalAnalyzer::TagCharacterClassMap[0x100]={
//	0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F

	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	1,	0,	0,	1,	0,	0,		// 0x10
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,		// 0x20
//		!	"	#	$	%	&	'	(	)	*	+	,	-	.	/
	1,	0,	7,	0,	0,	0,	0,	8,	0,	0,	0,	0,	0,	2,	2,	11,		// 0x30
//	0	1	2	3	4	5	6	7	8	9	:	;	<	=	>	?
	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	3,	0,	4,	6,	5,	10,		// 0x40
//	@	A	B	C	D	E	F	G	H	I	J	K	L	M	N	O
	0,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,		// 0x50
//	P	Q	R	S	T	U	V	W	X	Y	Z	[	\	]	^	_
	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	0,	0,	0,	0,	3,		// 0x60
//	`	a	b	c	d	e	f	g	h	i	j	k	l	m	n	o
	0,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,		// 0x70
//	p	q	r	s	t	u	v	w	x	y	z	{	|	}	~	
	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	0,	0,	0,	0,	0,		// 0x80
};
const cXMLLexicalAnalyzer::cTagCharTypeRange cXMLLexicalAnalyzer::TagCharacterTypeRangeList[]={
	// Extender
	{	0xB7,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB8,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xC0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xD7,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xD8,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xF7,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xF8,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x132,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x134,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x13F,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x141,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x149,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x14A,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x17F,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x180,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1C4,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1CD,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1F1,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1F4,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1F6,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1FA,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x218,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x250,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x2A0,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x2BB,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x2C2,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// Extender
	{	0x2D0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x2D2,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x300,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x346,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x360,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x362,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0x386,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x387,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// Extender
	//{	0x387,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x388,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	//{	0x388,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x38B,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x38C,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x38D,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x38E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x3A2,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x3A3,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x3CF,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x3D0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x3D7,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x3DA,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x3DB,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x3DC,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x3DD,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x3DE,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x3DF,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x3E0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x3E1,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x3E2,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x3F4,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x401,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x40D,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x40E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x450,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x451,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x45D,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x45E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x481,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	{	0x483,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x487,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0x490,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x4C5,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x4C7,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x4C9,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x4CB,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x4CD,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x4D0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x4EB,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x4EE,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x4F6,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x4F8,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x4FA,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x531,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x557,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x559,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x55A,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x561,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x587,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	{	0x591,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x5A2,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x5A3,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x5BA,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x5BB,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x5BE,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x5BF,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x5C0,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x5C1,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x5C3,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x5C4,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x5C5,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0x5D0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x5EA,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x5F0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x5F3,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x621,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x63B,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// Extender
	{	0x640,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x641,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	//{	0x641,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x64B,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	//{	0x64B,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x653,	static_cast<uInt8>(CharacterType_Name)			},
	// Digit
	{	0x660,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x66A,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x670,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x671,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	//{	0x671,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x6B8,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x6BA,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x6BF,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x6C0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x6CF,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x6D0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x6D4,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x6D5,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x6D6,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	//{	0x6D6,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x6E5,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	//{	0x6E5,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x6E7,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	//{	0x6E7,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x6E9,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x6EA,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x6EE,	static_cast<uInt8>(CharacterType_Name)			},
	// Digit
	{	0x6F0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x6FA,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x901,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x904,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0x905,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x93A,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	{	0x93C,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x93D,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	//{	0x93D,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x93E,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	//{	0x93E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x94E,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x951,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x955,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0x958,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x962,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	//{	0x962,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x964,	static_cast<uInt8>(CharacterType_Name)			},
	// Digit
	{	0x966,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x96A,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x981,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x984,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0x985,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x98D,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x98F,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x991,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x993,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x9A9,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x9AA,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x9B1,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x9B2,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x9B3,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x9B6,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x9BA,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	{	0x9BC,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x9BD,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x9BE,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x9C5,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x9C7,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x9C9,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x9CB,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x9CE,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x9D7,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x9D8,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0x9DC,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x9DE,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x9DF,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x9E2,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	//{	0x9E2,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x9E4,	static_cast<uInt8>(CharacterType_Name)			},
	// Digit
	{	0x9E6,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x9F0,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	//{	0x9F0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x9F2,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	{	0xA02,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA03,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xA05,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA0B,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xA0F,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA11,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xA13,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA29,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xA2A,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA31,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xA32,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA34,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xA35,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA37,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xA38,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA3A,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	{	0xA3C,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA3D,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xA3E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA43,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xA47,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA49,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xA4B,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA4E,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xA59,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA5D,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xA5E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA5F,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// Digit
	{	0xA66,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA70,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	//{	0xA70,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA72,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	//{	0xA72,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA75,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	{	0xA81,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA84,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xA85,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA8C,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xA8D,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA8E,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xA8F,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xA92,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xA93,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xAA9,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xAAA,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xAB1,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xAB2,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xAB4,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xAB5,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xABA,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	{	0xABC,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xABD,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	//{	0xABD,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xABE,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	//{	0xABE,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xAC6,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xAC7,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xACA,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xACB,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xACE,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xAE0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xAE1,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// Digit
	{	0xAE6,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xAF0,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xB01,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB04,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xB05,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB0D,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xB0F,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB11,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xB13,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB29,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xB2A,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB31,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xB32,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB34,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xB36,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB3A,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	{	0xB3C,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB3D,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	//{	0xB3D,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB3E,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	//{	0xB3E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB44,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xB47,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB49,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xB4B,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB4E,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xB56,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB58,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xB5C,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB5E,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xB5F,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB62,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// Digit
	{	0xB66,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB70,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xB82,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB84,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xB85,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB8B,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xB8E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB91,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xB92,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB96,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xB99,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB9B,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xB9C,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xB9D,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xB9E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xBA0,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xBA3,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xBA5,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xBA8,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xBAB,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xBAE,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xBB6,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xBB7,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xBBA,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	{	0xBBE,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xBC3,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xBC6,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xBC9,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xBCA,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xBCE,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xBD7,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xBD8,	static_cast<uInt8>(CharacterType_Name)			},
	// Digit
	{	0xBE7,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xBF0,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xC01,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xC04,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xC05,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xC0D,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xC0E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xC11,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xC12,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xC29,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xC2A,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xC34,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xC35,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xC3A,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	{	0xC3E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xC45,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xC46,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xC49,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xC4A,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xC4E,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xC55,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xC57,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xC60,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xC62,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// Digit
	{	0xC66,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xC70,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xC82,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xC84,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xC85,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xC8D,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xC8E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xC91,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xC92,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xCA9,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xCAA,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xCB4,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xCB5,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xCBA,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	{	0xCBE,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xCC5,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xCC6,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xCC9,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xCCA,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xCCE,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xCD5,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xCD7,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xCDE,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xCDF,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xCE0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xCE2,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// Digit
	{	0xCE6,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xCF0,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xD02,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xD04,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xD05,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xD0D,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xD0E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xD11,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xD12,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xD29,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xD2A,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xD3A,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	{	0xD3E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xD44,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xD46,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xD49,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xD4A,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xD4E,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xD57,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xD58,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xD60,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xD62,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// Digit
	{	0xD66,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xD70,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xE01,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xE2F,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xE30,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xE31,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	//{	0xE31,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xE32,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	//{	0xE32,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xE34,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	//{	0xE34,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xE3B,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xE40,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xE46,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// Extender
	//{	0xE46,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xE47,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	//{	0xE47,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xE4F,	static_cast<uInt8>(CharacterType_Name)			},
	// Digit
	{	0xE50,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xE5A,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0xE81,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xE83,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xE84,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xE85,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xE87,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xE89,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xE8A,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xE8B,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xE8D,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xE8E,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xE94,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xE98,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xE99,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xEA0,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xEA1,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xEA4,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xEA5,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xEA6,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xEA7,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xEA8,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xEAA,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xEAC,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xEAD,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xEAF,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xEB0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xEB1,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	//{	0xEB1,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xEB2,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	//{	0xEB2,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xEB4,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	//{	0xEB4,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xEBA,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xEBB,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xEBD,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	//{	0xEBD,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xEBE,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xEC0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xEC5,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// Extender
	{	0xEC6,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xEC7,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xEC8,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xECE,	static_cast<uInt8>(CharacterType_Name)			},
	// Digit
	{	0xED0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xEDA,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xF18,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xF1A,	static_cast<uInt8>(CharacterType_Name)			},
	// Digit
	{	0xF20,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xF2A,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xF35,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xF36,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xF37,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xF38,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xF39,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xF3A,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xF3E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xF40,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	//{	0xF40,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xF48,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xF49,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xF6A,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	{	0xF71,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xF85,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xF86,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xF8C,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xF90,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xF96,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xF97,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xF98,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xF99,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xFAE,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xFB1,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xFB8,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0xFB9,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xFBA,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0x10A0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x10C6,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x10D0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x10F7,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1100,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1101,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1102,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1104,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1105,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1108,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1109,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x110A,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x110B,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x110D,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x110E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1113,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x113C,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x113D,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x113E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x113F,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1140,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1141,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x114C,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x114D,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x114E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x114F,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1150,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1151,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1154,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1156,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1159,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x115A,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x115F,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1162,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1163,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1164,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1165,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1166,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1167,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1168,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1169,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x116A,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x116D,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x116F,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1172,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1174,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1175,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1176,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x119E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x11A9,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x11AB,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x11AC,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x11AE,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x11B0,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x11B7,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x11B9,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x11BA,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x11BB,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x11BC,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x11C3,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x11EB,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x11EC,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x11F0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x11F1,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x11F9,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x11FA,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1E00,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1E9C,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1EA0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1EFA,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1F00,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1F16,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1F18,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1F1E,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1F20,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1F46,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1F48,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1F4E,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1F50,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1F58,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1F59,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1F5A,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1F5B,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1F5C,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1F5D,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1F5E,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1F5F,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1F7E,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1F80,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1FB5,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1FB6,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1FBD,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1FBE,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1FBF,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1FC2,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1FC5,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1FC6,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1FCD,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1FD0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1FD4,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1FD6,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1FDC,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1FE0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1FED,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1FF2,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1FF5,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x1FF6,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x1FFD,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	{	0x20D0,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x20DD,	static_cast<uInt8>(CharacterType_Name)			},
	// CombiningChar
	{	0x20E1,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x20E2,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0x2126,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x2127,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x212A,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x212C,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x212E,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x212F,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0x2180,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x2183,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// Extender
	{	0x3005,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x3006,	static_cast<uInt8>(CharacterType_Name)			},
	// Ideographic(Letter)
	{	0x3007,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x3008,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// Ideographic(Letter)
	{	0x3021,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x302A,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	//{	0x302A,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x3030,	static_cast<uInt8>(CharacterType_Name)			},
	// Extender
	{	0x3031,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x3036,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0x3041,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x3095,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// CombiningChar
	{	0x3099,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x309B,	static_cast<uInt8>(CharacterType_Name)			},
	// Extender
	{	0x309D,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x309E,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0x30A1,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x30FB,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// Extender
	{	0x30FC,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x30FF,	static_cast<uInt8>(CharacterType_Name)			},
	// BaseChar(Letter)
	{	0x3105,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x312D,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// Ideographic(Letter)
	{	0x4E00,	static_cast<uInt8>(CharacterType_Error)			},
	{	0x9FA5,	static_cast<uInt8>(CharacterType_NameInitial)	},
	// BaseChar(Letter)
	{	0xAC00,	static_cast<uInt8>(CharacterType_Error)			},
	{	0xD7A4,	static_cast<uInt8>(CharacterType_NameInitial)	},

};

//---------------------------------------------------------------------------
ufInt8 cXMLLexicalAnalyzer::ClassifyTagCharacter(uChar32 c)
{
	const uIntn TextCharacterTypeRangeListCount=ArrayLength(TagCharacterTypeRangeList);
	for(uIntn i=1;i<TextCharacterTypeRangeListCount;i++){
		cnLib_ASSERT(TagCharacterTypeRangeList[i].Char>TagCharacterTypeRangeList[i-1].Char);
	}
	if(c<ArrayLength(TagCharacterClassMap)){
		return TagCharacterClassMap[c];
	}
	for(uIntn i=0;i<TextCharacterTypeRangeListCount;i++){
		if(c<static_cast<uChar32>(TagCharacterTypeRangeList[i].Char)){
			return TagCharacterTypeRangeList[i].Type;
		}
	}
	return CharacterType_Error;
}
//---------------------------------------------------------------------------
const cXMLLexicalAnalyzer::cXMLCharacterEntity cXMLLexicalAnalyzer::XMLCharacterEntities[]={
	{"amp",		0,	'&'	},	//	&	&#38;	&#x26;
	{"apos",	1,	'\''},	//	'	&#39;	&#x27;
	{"gt",		0,	'>'	},	//	>	&#62;	&#x3E;
	{"lt",		0,	'<'	},	//	<	&#60;	&#x3C;
	{"quot",	0,	'\"'},	//	"	&#34;	&#x22;
};
//---------------------------------------------------------------------------
cArray<const uChar32> cXMLLexicalAnalyzer::TranslateXMLEntity(cArray<const uChar32> Name)
{
	cArray<const uChar32> Result;
	uIntn EntityIndex=0;
	uIntn NameCharIndex=0;
	for(;;){
		uChar32 NameChar=Name.Pointer[NameCharIndex];
		auto &XMLCharEnt=XMLCharacterEntities[EntityIndex];
		uChar32 DictChar=XMLCharEnt.Name[NameCharIndex];
		if(DictChar==NameChar){
			// next
			NameCharIndex++;
			if(NameCharIndex>=Name.Length){
				if(XMLCharEnt.Name[NameCharIndex]==0){
					// match
					Result.Pointer=&XMLCharEnt.Character;
					Result.Length=1;
					return Result;
				}
			}
		}
		else if(DictChar<NameChar){
			EntityIndex++;
			if(EntityIndex>=ArrayLength(XMLCharacterEntities))
				break;
			auto &NextEnt=XMLCharacterEntities[EntityIndex];
			if(NextEnt.MatchLength<NameCharIndex)
				break;
		}
		else{
			// not found
			break;
		}
	}
	Result.Pointer=nullptr;
	Result.Length=0;
	return Result;
}
//---------------------------------------------------------------------------
bool cXMLLexicalAnalyzer::StateReferenceTranslate(cTransition &Transition,uChar32 c)
{
	if(c==';'){
		switch(fRefState){
		default:
		case srNumber:
			fRefState=srStart;
			cnLib_SWITCH_FALLTHROUGH;
		case srStart:
			fRefText.AppendChar(c);
			return false;
		case srName:
			// translate name
		{
			auto EntityName=fRefText.GetArray();
			EntityName.Pointer++;
			EntityName.Length--;
			auto EntityText=TranslateEntity(EntityName);
			if(EntityText.Pointer!=nullptr){
				fRefText=EntityText;
			}
			else{
				fRefText.AppendChar(';');
			}
			fRefState=srStart;
		}
			return true;
		case srNumber10:
		case srNumber16:
			// translate number to character
			fRefText.SetLength(1);
			fRefText[0]=fRefNumber;
			fRefState=srStart;
			return true;
		}
	}
	switch(fRefState){
	case srStart:
		switch(ClassifyTagCharacter(c)){
		case CharacterType_Error:
			if(c=='#'){
				fRefNumber=0;
				fRefState=srNumber;
				break;
			}
			cnLib_SWITCH_FALLTHROUGH;
		default:	// error
			// invalid character
			Transition.CharacterStay=true;
			return true;
		case CharacterType_NameInitial:
			fRefState=srName;
			break;
		}
		break;
	case srNumber:
		if(c<'0'){
			// invalid character
			Transition.CharacterStay=true;
			fRefState=srStart;
			return true;
		}
		else if(c<='9'){
			fRefState=srNumber10;
			// ProcessNumber
			fRefNumber*=10;
			fRefNumber+=c-'0';
		}
		else if(c=='x'){
			fRefState=srNumber16;
		}
		else{
			// invalid character
			Transition.CharacterStay=true;
			fRefState=srStart;
			return true;
		}
		break;
	case srNumber10:
		if(c>='0' && c<='9'){
			// ProcessNumber
			fRefNumber*=10;
			fRefNumber+=c-'0';
		}
		else{
			// invalid character
			Transition.CharacterStay=true;
			fRefState=srStart;
			return true;
		}
		break;
	case srNumber16:
		if(c<'0'){
			// invalid character
			Transition.CharacterStay=true;
			fRefState=srStart;
			return true;
		}
		else if(c<='9'){
			// ProcessNumber
			fRefNumber*=16;
			fRefNumber+=c-'0';
		}
		else if(c<'A'){
			// invalid character
			Transition.CharacterStay=true;
			fRefState=srStart;
			return true;
		}
		else if(c<='F'){
			// ProcessNumber
			fRefNumber*=16;
			fRefNumber+=c-'A'+0xA;
		}
		else if(c<'a'){
			// invalid character
			Transition.CharacterStay=true;
			fRefState=srStart;
			return true;
		}
		else if(c<='f'){
			// ProcessNumber
			fRefNumber*=16;
			fRefNumber+=c-'a'+0xA;
		}
		else{
			// invalid character
			Transition.CharacterStay=true;
			fRefState=srStart;
			return true;
		}
		break;
	}
	fRefText.AppendChar(c);
	return false;
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_Text(cTransition &Transition,uChar32 c)
{
	switch(ClassifyTextCharacter(c)){
	default:
	case CharacterType_Error:
		break;
	case CharacterType_Whitespace:
		if(PreserveWhitespaceInText){
			fAcceptBuffer.Accept(c);
		}
		else{
			// output text part
			if(fAcceptBuffer.GetLength()!=0){
				OutputAcceptedLast(XMLTokenType::Text);
				Transition.Output=true;
				break;
			}
			else{
				fAcceptBuffer.PosOutputSet();
			}
		}
		break;
	case CharacterType_Text:
		fAcceptBuffer.Accept(c);
		break;
	case CharacterType_Reference:	// &
		fRefText.SetLength(1);
		fRefText[0]=c;
		fState=sTextRef;
		break;
	case CharacterType_TagLeft:	// <
		IgnoreAttribute=false;
		fState=sTagBegin;
		if(fAcceptBuffer.GetLength()){
			OutputAcceptedLast(XMLTokenType::Text);
			Transition.Output=true;
			break;
		}
		else{
			fAcceptBuffer.Discard();
			fAcceptBuffer.PosOutputSetLast();
		}
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_TextRef(cTransition &Transition,uChar32 c)
{
	if(StateReferenceTranslate(Transition,c)){
		fAcceptBuffer.Accept(fRefText->Pointer,fRefText->Length);
		fState=sText;
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_Comment(cTransition &Transition,uChar32 c)
{
	switch(c){
	case '-':
		fAcceptBuffer.Accept(c);
		fCommentState.EndLength++;
		break;
	case '>':
		if(fCommentState.EndLength>=2){
			// end of comment
			fAcceptBuffer.Truncate(2);
			OutputAcceptedCur(XMLTokenType::Comment);
			fState=sText;
			Transition.Output=true;
			break;
		}
		cnLib_SWITCH_FALLTHROUGH;
	default:
		fAcceptBuffer.Accept(c);
		fCommentState.EndLength=0;
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_TagBegin(cTransition &Transition,uChar32 c)
{
	switch(ClassifyTagCharacter(c)){
	default:
	case CharacterType_Error:
		if(c=='!'){
			fTagBeginState.Type=TagBeginType_DTD;
			fTagBeginState.NameInitial=true;
			fState=sTagDTD;
		}
		else if(AcceptInvalidTagName){
			// treat as correct tag name
			fAcceptBuffer.Accept(c);
			fTagBeginState.Type=TagBeginType_Normal;
			fTagBeginState.NameInitial=false;
			fState=sTagName;
		}
		else{
			// invalid character
			OutputEmptyLast(XMLTokenType::TagBegin);
			fState=sTag;
			Transition.CharacterStay=true;
			Transition.Output=true;
		}
		break;
	case CharacterType_TagLeft:	// <
		// invalid character
		OutputEmptyLast(XMLTokenType::TagBegin);
		fState=sTag;
		Transition.Output=true;
		break;
	case CharacterType_TagEnd:	// /
		fTagBeginState.Type=TagBeginType_Close;
		fTagBeginState.NameInitial=true;
		IgnoreAttribute=true;
		fState=sTagName;
		break;
	case CharacterType_TagPI:	// ?
		fTagBeginState.Type=TagBeginType_PI;
		fTagBeginState.NameInitial=true;
		fState=sTagName;
		break;
	case CharacterType_Name:
		// invalid name
		if(AcceptInvalidTagName==false){
			OutputEmptyLast(XMLTokenType::TagBegin);
			Transition.Output=true;
			Transition.CharacterStay=true;
			fAcceptBuffer.Discard();
			fState=sTag;
			break;
		}
		// treat as correct tag name
		cnLib_SWITCH_FALLTHROUGH;
	case CharacterType_NameInitial:
		fAcceptBuffer.Accept(c);
		fTagBeginState.Type=TagBeginType_Normal;
		fTagBeginState.NameInitial=false;
		fState=sTagName;
		break;
	case CharacterType_TagRight:	// >
		OutputEmptyLast(XMLTokenType::Error);
		fState=sText;
		Transition.Output=true;
		break;
	}

}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_TagComment(cTransition &Transition,uChar32 c)
{
	if(c=='-'){
		fState=sComment;
		fCommentState.EndLength=0;
		return;
	}
	// error dtd tag
	OutputEmptyLast(XMLTokenType::DTDTagBegin);
	//fAcceptBuffer.Accept('-');
	fState=sAttribute;
	Transition.CharacterStay=true;
	Transition.Output=true;
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_TagName(cTransition &Transition,uChar32 c)
{
	switch(ClassifyTagCharacter(c)){
	default:
	case CharacterType_Error:
		if(AcceptInvalidTagName){
			// treat as valid tag name
			fAcceptBuffer.Accept(c);
			break;
		}
		cnLib_SWITCH_FALLTHROUGH;
	case CharacterType_TagLeft:	// <
		// invalid character
		OutputTagBeginLast();
		fState=sTag;
		Transition.Output=true;
		break;
	case CharacterType_TagEnd:	// /
		// output Tag start
		OutputTagBeginLast();
		fTagEndState.Type=TagEndType_Empty;
		fState=sTagEnd;
		Transition.Output=true;
		break;
	case CharacterType_TagPI:	// ?
		// output Tag start
		OutputTagBeginLast();
		fTagEndState.Type=TagEndType_PI;
		fState=sTagEnd;
		Transition.Output=true;
		break;
	case CharacterType_Whitespace:
		// output Tag start
		WriteTagBeginLast();
		fAcceptBuffer.PosOutputSet();
		fState=sTag;
		Transition.Output=true;
		break;
	case CharacterType_Name:
		if(fTagBeginState.NameInitial){
			// error name
			if(AcceptInvalidTagName==false){
				OutputTagBeginLast();
				fAcceptBuffer.Accept(c);
				fAttributeState.InvalidName=true;
				fState=sAttribute;
				Transition.Output=true;
				break;
			}
			// treat as corrent tag name
		}
		cnLib_SWITCH_FALLTHROUGH;
	case CharacterType_NameInitial:
		fAcceptBuffer.Accept(c);
		fTagBeginState.NameInitial=false;
		break;
	case CharacterType_TagRight:	// >
	case CharacterType_Equ:		// =
		// output empty Tag start
		OutputTagBeginLast();
		fState=sTag;
		Transition.CharacterStay=true;
		Transition.Output=true;
		break;
	case CharacterType_StringQ:	// "
		OutputTagBeginLast();
		fStringState.IsDQ=true;
		fState=sString;
		Transition.Output=true;
		break;
	case CharacterType_StringA:	// '
		OutputTagBeginLast();
		fStringState.IsDQ=false;
		fState=sString;
		Transition.Output=true;
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_Tag(cTransition &Transition,uChar32 c)
{
	switch(ClassifyTagCharacter(c)){
	default:
	case CharacterType_Error:
	case CharacterType_TagLeft:	// <
		// ignore error text
		fAcceptBuffer.Discard();
		fAcceptBuffer.PosOutputSet();
		break;
	case CharacterType_Whitespace:
		fAcceptBuffer.Discard();
		fAcceptBuffer.PosOutputSet();
		break;
	case CharacterType_TagPI:	// ?
		fTagEndState.Type=TagEndType_PI;
		fState=sTagEnd;
		break;
	case CharacterType_TagEnd:	// /
		fTagEndState.Type=TagEndType_Empty;
		fState=sTagEnd;
		break;
	case CharacterType_TagRight:	// >
		fTagEndState.Type=TagEndType_Normal;
		OutputEmptyCur(XMLTokenType::TagEnd);
		fState=sText;
		Transition.Output=true;
		break;
	case CharacterType_Name:
		fAcceptBuffer.Accept(c);
		fState=sAttribute;
		fAttributeState.InvalidName=true;
		break;
	case CharacterType_NameInitial:
		fAcceptBuffer.Accept(c);
		fState=sAttribute;
		fAttributeState.InvalidName=false;
		break;
	case CharacterType_Equ:		// =
		OutputEmptyCur(XMLTokenType::Equ);
		Transition.Output=true;
		break;
	case CharacterType_StringQ:	// "
		fStringState.IsDQ=true;
		fState=sString;
		break;
	case CharacterType_StringA:	// '
		fStringState.IsDQ=false;
		fState=sString;
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_TagIgnore(cTransition &Transition,uChar32 c)
{
	switch(ClassifyTagCharacter(c)){
	default:
	case CharacterType_Error:
	case CharacterType_TagLeft:	// <
		// ignore error text
		break;
	case CharacterType_Whitespace:
		fAcceptBuffer.Discard();
		fAcceptBuffer.PosOutputSet();
		break;
	case CharacterType_TagPI:	// ?
		fTagEndState.Type=TagEndType_PI;
		fState=sTagEnd;
		break;
	case CharacterType_TagEnd:	// /
		fTagEndState.Type=TagEndType_Empty;
		fState=sTagEnd;
		break;
	case CharacterType_TagRight:	// >
		fTagEndState.Type=TagEndType_Normal;
		OutputEmptyCur(XMLTokenType::TagEnd);
		fState=sText;
		Transition.Output=true;
		break;
	case CharacterType_Name:
	case CharacterType_NameInitial:
	case CharacterType_Equ:		// =
	case CharacterType_StringQ:	// "
	case CharacterType_StringA:	// '
		// ignore
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_Attribute(cTransition &Transition,uChar32 c)
{
	switch(ClassifyTagCharacter(c)){
	default:
	case CharacterType_Error:
		// invalid character
		cnLib_SWITCH_FALLTHROUGH;
	case CharacterType_TagLeft:	// <
		OutputAttributeLast();

		// invalid character
		fState=sTag;
		Transition.Output=true;
		break;
	case CharacterType_Whitespace:
		WriteAttributeLast();
		fAcceptBuffer.PosOutputSet();

		fState=sTag;
		Transition.Output=true;
		break;
	case CharacterType_TagPI:	// ?
		// output attribute name
		OutputAttributeLast();

		fTagEndState.Type=TagEndType_PI;
		fState=sTagEnd;
		Transition.Output=true;
		break;
	case CharacterType_TagEnd:	// /
		// output attribute name
		OutputAttributeLast();
		fTagEndState.Type=TagEndType_Empty;
		fState=sTagEnd;
		Transition.Output=true;
		break;
	case CharacterType_Name:
	case CharacterType_NameInitial:
		fAcceptBuffer.Accept(c);
		break;
	case CharacterType_Equ:			// =
	case CharacterType_TagRight:	// >
		// output attribute name
		OutputAttributeLast();
		Transition.CharacterStay=true;
		fState=sTag;
		Transition.Output=true;
		break;
	case CharacterType_StringQ:	// "
		OutputAttributeLast();
		fStringState.IsDQ=true;
		fState=sString;
		Transition.Output=true;
		break;
	case CharacterType_StringA:	// '
		OutputAttributeLast();
		fStringState.IsDQ=false;
		fState=sString;
		Transition.Output=true;
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_String(cTransition &Transition,uChar32 c)
{
	switch(c){
	case '&':
		fRefText.SetLength(1);
		fRefText[0]=c;
		fState=sStringRef;
		break;
	case '\"':
		if(fStringState.IsDQ){
			OutputAcceptedCur(XMLTokenType::String);
			fState=sTag;
			Transition.Output=true;
			break;
		}
		fAcceptBuffer.Accept('\"');
		break;
	case '\'':
		if(fStringState.IsDQ==false){
			OutputAcceptedCur(XMLTokenType::String);
			fState=sTag;
			Transition.Output=true;
			break;
		}
		fAcceptBuffer.Accept('\'');
		break;
	default:
		fAcceptBuffer.Accept(c);
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_StringRef(cTransition &Transition,uChar32 c)
{
	if(StateReferenceTranslate(Transition,c)){
		fAcceptBuffer.Accept(fRefText->Pointer,fRefText->Length);
		fState=sString;
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::OutputAcceptedLast(eXMLTokenType Type)
{
	fValue.Type=Type;
	fAcceptBuffer.PosOutputGet(fValue.FromLine,fValue.FromCharacter);
	fAcceptBuffer.PosGetLast(fValue.ToLine,fValue.ToCharacter);
	fValue.Text=fAcceptBuffer.Fetch();
	fAcceptBuffer.PosOutputSetLast();
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::OutputAcceptedCur(eXMLTokenType Type)
{
	fValue.Type=Type;
	fAcceptBuffer.PosOutputGet(fValue.FromLine,fValue.FromCharacter);
	fAcceptBuffer.PosGet(fValue.ToLine,fValue.ToCharacter);
	fValue.Text=fAcceptBuffer.Fetch();
	fAcceptBuffer.PosOutputSet();
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::WriteAttributeLast(void)
{
	if(fAttributeState.InvalidName){
		fValue.Type=XMLTokenType::Error;
	}
	else{
		fValue.Type=XMLTokenType::Attribute;
	}
	fAcceptBuffer.PosOutputGet(fValue.FromLine,fValue.FromCharacter);
	fAcceptBuffer.PosGetLast(fValue.ToLine,fValue.ToCharacter);
	fValue.Text=fAcceptBuffer.Fetch();
	fAcceptBuffer.PosOutputSetLast();
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::OutputAttributeLast(void)
{
	WriteAttributeLast();
	fAcceptBuffer.PosOutputSetLast();
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::OutputEmptyLast(eXMLTokenType Type)
{
	fValue.Type=Type;
	fValue.Text=nullptr;
	fAcceptBuffer.PosOutputGet(fValue.FromLine,fValue.FromCharacter);
	fAcceptBuffer.PosGetLast(fValue.ToLine,fValue.ToCharacter);
	fAcceptBuffer.Discard();
	fAcceptBuffer.PosOutputSetLast();
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::OutputEmptyCur(eXMLTokenType Type)
{
	fValue.Type=Type;
	fValue.Text=nullptr;
	fAcceptBuffer.PosOutputGet(fValue.FromLine,fValue.FromCharacter);
	fAcceptBuffer.PosGet(fValue.ToLine,fValue.ToCharacter);
	fAcceptBuffer.Discard();
	fAcceptBuffer.PosOutputSet();
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::WriteTagBeginLast(void)
{
	switch(fTagBeginState.Type){
	case TagBeginType_Normal:
		fValue.Type=XMLTokenType::TagBegin;
		break;
	case TagBeginType_PI:
		fValue.Type=XMLTokenType::PITagBegin;
		break;
	case TagBeginType_DTD:
		fValue.Type=XMLTokenType::DTDTagBegin;
		break;
	case TagBeginType_Close:
		fValue.Type=XMLTokenType::CloseTagBegin;
		break;
	default:
		fValue.Type=XMLTokenType::Error;
		break;
	}
	fAcceptBuffer.PosOutputGet(fValue.FromLine,fValue.FromCharacter);
	fAcceptBuffer.PosGetLast(fValue.ToLine,fValue.ToCharacter);
	fValue.Text=fAcceptBuffer.Fetch();
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::OutputTagBeginLast(void)
{
	WriteTagBeginLast();
	fAcceptBuffer.PosOutputSetLast();
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::OutputTagEndCur(void)
{
	switch(fTagEndState.Type){
	case TagEndType_Normal:
		fValue.Type=XMLTokenType::TagEnd;
		break;
	case TagEndType_PI:
		fValue.Type=XMLTokenType::PITagEnd;
		break;
	case TagEndType_Empty:
		fValue.Type=XMLTokenType::EmptyTagEnd;
		break;
	}
	fAcceptBuffer.PosOutputGet(fValue.FromLine,fValue.FromCharacter);
	fAcceptBuffer.PosGet(fValue.ToLine,fValue.ToCharacter);
	fValue.Text.Clear();
	fAcceptBuffer.Discard();
	fAcceptBuffer.PosOutputSet();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::Reset(void)
{
	fState=sText;
}
//---------------------------------------------------------------------------
cXMLSymbol& cXMLSyntaxAnalyzer::Output(void)
{
	return fValue;
}
//---------------------------------------------------------------------------
cXMLSyntaxAnalyzer::cTransition cXMLSyntaxAnalyzer::Input(const cXMLToken &Token)
{
	cTransition Transition;
	Transition.Output=false;

	switch(fState){
	case sText:
		StateTranslate_Text(Transition,Token);
		break;
	case sTag:
		StateTranslate_Tag(Transition,Token);
		break;
	case sAttribute:
		StateTranslate_Attribute(Transition,Token);
		break;
	case sAttributeEqu:
		StateTranslate_AttributeEqu(Transition,Token);
		break;
	}

	return Transition;
}
//---------------------------------------------------------------------------
cXMLSyntaxAnalyzer::cTransition cXMLSyntaxAnalyzer::InputFinish(void)
{
	cTransition Transition;
	Transition.Output=false;

	switch(fState){
	case sText:
		break;
	case sTag:
		break;
	case sAttribute:
		OutputAttributeError();
		Transition.Output=true;
		break;
	case sAttributeEqu:
		OutputAttributeError();
		Transition.Output=true;
		break;
	}

	return Transition;
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::StateTranslate_Text(cTransition &Transition,const cXMLToken &Token)
{
	switch(Token.Type){
	case XMLTokenType::Error:
	case XMLTokenType::Comment:
		OutputTextToken(XMLSymbolType::Comment,Token);
		Transition.Output=true;
		break;
	case XMLTokenType::Text:
		OutputTextToken(XMLSymbolType::Text,Token);
		Transition.Output=true;
		break;
	case XMLTokenType::TagBegin:
		OutputTagStart(XMLSymbolTagType::Normal,Token);
		fState=sTag;
		fTagType=ttNormal;
		Transition.Output=true;
		break;
	case XMLTokenType::DTDTagBegin:
		OutputTagStart(XMLSymbolTagType::DTD,Token);
		fState=sTag;
		fTagType=ttDTD;
		Transition.Output=true;
		break;
	case XMLTokenType::CloseTagBegin:
		OutputTagStart(XMLSymbolTagType::Close,Token);
		fState=sTag;
		fTagType=ttClose;
		Transition.Output=true;
		break;
	case XMLTokenType::PITagBegin:
		OutputTagStart(XMLSymbolTagType::PI,Token);
		fState=sTag;
		fTagType=ttPI;
		Transition.Output=true;
		break;
	case XMLTokenType::EmptyTagEnd:
	case XMLTokenType::PITagEnd:
	case XMLTokenType::TagEnd:
	case XMLTokenType::Attribute:
	case XMLTokenType::Equ:
	case XMLTokenType::String:
		// error
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::StateTranslate_Tag(cTransition &Transition,const cXMLToken &Token)
{
	switch(Token.Type){
	case XMLTokenType::Error:
	case XMLTokenType::Text:
	case XMLTokenType::Comment:
	case XMLTokenType::TagBegin:
	case XMLTokenType::DTDTagBegin:
	case XMLTokenType::CloseTagBegin:
	case XMLTokenType::PITagBegin:
		// error
		break;
	case XMLTokenType::EmptyTagEnd:
		switch(fTagType){
		case ttNormal:
			OutputTagFinish(XMLSymbolTagType::Close);
			break;
		case ttDTD:
			// should use normal tag end
			OutputTagFinish(XMLSymbolTagType::Error);
			break;
		case ttClose:
			// should use normal tag end
			OutputTagFinish(XMLSymbolTagType::Error);
			break;
		case ttPI:
			// should use pi tag end
			OutputTagFinish(XMLSymbolTagType::Error);
			break;
		}
		fState=sText;
		Transition.Output=true;
		break;
	case XMLTokenType::PITagEnd:
		if(fTagType==ttPI){
			OutputTagFinish(XMLSymbolTagType::Normal);
		}
		else{
			// cannot use pi tag end
			OutputTagFinish(XMLSymbolTagType::Error);
		}
		fState=sText;
		Transition.Output=true;
		break;
	case XMLTokenType::TagEnd:
		switch(fTagType){
		case ttNormal:
		case ttDTD:
		case ttClose:
			OutputTagFinish(XMLSymbolTagType::Normal);
			break;
		case ttPI:
			// should use pi tag end
			OutputTagFinish(XMLSymbolTagType::Error);
			break;
		}
		fState=sText;
		Transition.Output=true;
		break;
	case XMLTokenType::Attribute:
		fAttributeName=cnVar::MoveCast(Token.Text);
		fState=sAttribute;
		break;
	case XMLTokenType::Equ:
	case XMLTokenType::String:
		// error
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::StateTranslate_Attribute(cTransition &Transition,const cXMLToken &Token)
{
	switch(Token.Type){
	case XMLTokenType::Error:
	case XMLTokenType::Comment:
	case XMLTokenType::Text:
	case XMLTokenType::TagBegin:
	case XMLTokenType::DTDTagBegin:
	case XMLTokenType::CloseTagBegin:
	case XMLTokenType::PITagBegin:
		// error
		fState=sTag;
		break;
	case XMLTokenType::EmptyTagEnd:
		switch(fTagType){
		case ttNormal:
			OutputTagFinish(XMLSymbolTagType::Close);
			break;
		case ttDTD:
			// should use normal tag end
			OutputTagFinish(XMLSymbolTagType::Error);
			break;
		case ttClose:
			// should use normal tag end
			OutputTagFinish(XMLSymbolTagType::Error);
			break;
		case ttPI:
			// should use pi tag end
			OutputTagFinish(XMLSymbolTagType::Error);
			break;
		}
		fState=sText;
		Transition.Output=true;
		break;
	case XMLTokenType::PITagEnd:
		if(fTagType==ttPI){
			OutputTagFinish(XMLSymbolTagType::Normal);
		}
		else{
			// cannot use pi tag end
			OutputTagFinish(XMLSymbolTagType::Error);
		}
		fState=sText;
		Transition.Output=true;
		break;
	case XMLTokenType::TagEnd:
		switch(fTagType){
		case ttNormal:
		case ttDTD:
		case ttClose:
			OutputTagFinish(XMLSymbolTagType::Normal);
			break;
		case ttPI:
			// should use pi tag end
			OutputTagFinish(XMLSymbolTagType::Error);
			break;
		}
		fState=sText;
		Transition.Output=true;
		break;
	case XMLTokenType::Attribute:
		// output error attribute
		OutputAttributeError();
		fAttributeName=cnVar::MoveCast(Token.Text);
		fState=sAttribute;
		break;
	case XMLTokenType::Equ:
		fState=sAttributeEqu;
		break;
	case XMLTokenType::String:
		fState=sTag;
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::StateTranslate_AttributeEqu(cTransition &Transition,const cXMLToken &Token)
{
	switch(Token.Type){
	case XMLTokenType::Error:
	case XMLTokenType::Comment:
	case XMLTokenType::Text:
	case XMLTokenType::TagBegin:
	case XMLTokenType::DTDTagBegin:
	case XMLTokenType::CloseTagBegin:
	case XMLTokenType::PITagBegin:
		// error
		OutputAttributeError();
		fState=sTag;
		Transition.Output=true;
		break;
	case XMLTokenType::EmptyTagEnd:
		switch(fTagType){
		case ttNormal:
			OutputTagFinish(XMLSymbolTagType::Close);
			break;
		case ttDTD:
			// should use normal tag end
			OutputTagFinish(XMLSymbolTagType::Error);
			break;
		case ttClose:
			// should use normal tag end
			OutputTagFinish(XMLSymbolTagType::Error);
			break;
		case ttPI:
			// should use pi tag end
			OutputTagFinish(XMLSymbolTagType::Error);
			break;
		}
		fState=sText;
		Transition.Output=true;
		break;
	case XMLTokenType::PITagEnd:
		if(fTagType==ttPI){
			OutputTagFinish(XMLSymbolTagType::Normal);
		}
		else{
			// cannot use pi tag end
			OutputTagFinish(XMLSymbolTagType::Error);
		}
		fState=sText;
		Transition.Output=true;
		break;
	case XMLTokenType::TagEnd:
		switch(fTagType){
		case ttNormal:
		case ttDTD:
		case ttClose:
			OutputTagFinish(XMLSymbolTagType::Normal);
			break;
		case ttPI:
			// should use pi tag end
			OutputTagFinish(XMLSymbolTagType::Error);
			break;
		}
		fState=sText;
		Transition.Output=true;
		break;
	case XMLTokenType::Attribute:
		// output error attribute
		OutputAttributeError();
		fAttributeName=cnVar::MoveCast(Token.Text);
		fState=sAttribute;
		Transition.Output=true;
		break;
	case XMLTokenType::Equ:
		// error multiple equ
		fState=sAttributeEqu;
		break;
	case XMLTokenType::String:
		// attribute finish
		OutputAttribute(Token);
		fState=sTag;
		Transition.Output=true;
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::OutputTextToken(eXMLSymbolType Type,const cXMLToken &Token)
{
	fValue.Type=Type;
	fValue.TagType=XMLSymbolTagType::Error;
	fValue.Name.Clear();
	fValue.Value=cnVar::MoveCast(Token.Text);
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::OutputTagStart(eXMLSymbolTagType TagType,const cXMLToken &Token)
{
	fValue.Type=XMLSymbolType::TagStart;
	fValue.TagType=TagType;
	fValue.Name=cnVar::MoveCast(Token.Text);
	fValue.Value.Clear();
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::OutputTagFinish(eXMLSymbolTagType TagType)
{
	fValue.Type=XMLSymbolType::TagFinish;
	fValue.TagType=TagType;
	fValue.Name.Clear();
	fValue.Value.Clear();
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::OutputAttributeError(void)
{
	fValue.Type=XMLSymbolType::Attribute;
	fValue.TagType=XMLSymbolTagType::Error;
	fValue.Name=cnVar::MoveCast(fAttributeName);
	fValue.Value.Clear();
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::OutputAttribute(const cXMLToken &Token)
{
	fValue.Type=XMLSymbolType::Attribute;
	fValue.TagType=XMLSymbolTagType::Error;
	fValue.Name=cnVar::MoveCast(fAttributeName);
	fValue.Value=cnVar::MoveCast(Token.Text);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cXMLParser::cXMLParser()noexcept(true)
{
	fLexer.SetOptionsDefault();
	fConverter=UnicodeTranscoder(2,4);
	fCurItem=&fRootItem;
	fRootItem.Parent=nullptr;
}
//---------------------------------------------------------------------------
cXMLParser::~cXMLParser()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cXMLParser::Reset(iXMLVisitor *Visitor)noexcept(true)
{
	fLexer.Reset();
	fSyntaxer.Reset();

	if(fCurItem->Parent!=nullptr){
		auto DeleteItem=fCurItem;
		fCurItem=fCurItem->Parent;

		delete DeleteItem;
	}

	fCurItem=&fRootItem;
	fCurItem->Visitor=Visitor;
}
//---------------------------------------------------------------------------
void cXMLParser::Finish(void)noexcept(true)
{
	{
		auto Transition=fLexer.InputFinish();
		if(Transition.Output){
			Syntax();
		}
	}
	{
		auto Transition=fSyntaxer.InputFinish();
		if(Transition.Output){
			Grammer();
		}
	}
}
//---------------------------------------------------------------------------
bool cXMLParser::Input(uChar32 c)noexcept(true)
{
	auto Transition=fLexer.Input(c);
	if(Transition.Output){
		Syntax();
	}
	return Transition.CharacterStay;
}
//---------------------------------------------------------------------------
void cXMLParser::Syntax(void)noexcept(true)
{

	auto &Token=fLexer.Output();
	auto Transition=fSyntaxer.Input(Token);
	if(Transition.Output){
		Grammer();
	}
}
//---------------------------------------------------------------------------
void cXMLParser::Grammer(void)noexcept(true)
{
	auto &Symbol=fSyntaxer.Output();

	cString<uChar16> SymbolName=cnRTL::CreateStringConvertEncoding<uChar16>(fConverter,Symbol.Name->Pointer,Symbol.Name->Length);
	switch(Symbol.Type){
	case XMLSymbolType::Error:
		break;
	case XMLSymbolType::Comment:
	{
		auto Text=cnRTL::CreateStringConvertEncoding<uChar16>(fConverter,Symbol.Value->Pointer,Symbol.Value->Length);
		fCurItem->Visitor->Comment(cnVar::MoveCast(Text));
	}
		break;
	case XMLSymbolType::Text:
	{
		auto Text=cnRTL::CreateStringConvertEncoding<uChar16>(fConverter,Symbol.Value->Pointer,Symbol.Value->Length);
		fCurItem->Visitor->Text(cnVar::MoveCast(Text));
	}
		break;
	case XMLSymbolType::TagStart:
	{
		switch(Symbol.TagType){
		case XMLSymbolTagType::Normal:
			OpenTag(SymbolName);
			fCurItem->TagType=Symbol.TagType;
			break;
		case XMLSymbolTagType::DTD:
		case XMLSymbolTagType::Error:
		case XMLSymbolTagType::PI:
			break;
		case XMLSymbolTagType::Close:
		{
			// close tag
			if(SymbolName==fCurItem->TagName){
				RootNodeFinished=CloseTag();
			}
			else{
				// invalid close tag
			}
		}
			break;
		}


		Symbol.Type;
		Symbol.TagType;
		Symbol.Name;
		Symbol.Value;
	}
		break;
	case XMLSymbolType::TagFinish:
	{
		switch(fCurItem->TagType){
		case XMLSymbolTagType::Normal:
			switch(Symbol.TagType){
			case XMLSymbolTagType::Normal:
				break;
			case XMLSymbolTagType::Close:
				RootNodeFinished=CloseTag();
				break;
			}
			break;
		case XMLSymbolTagType::DTD:
		case XMLSymbolTagType::Error:
		case XMLSymbolTagType::PI:
		case XMLSymbolTagType::Close:
			break;
		}
	}
		break;
	case XMLSymbolType::Attribute:
	{
		auto Name=cnRTL::CreateStringConvertEncoding<uChar16>(fConverter,Symbol.Name->Pointer,Symbol.Name->Length);
		auto Value=cnRTL::CreateStringConvertEncoding<uChar16>(fConverter,Symbol.Value->Pointer,Symbol.Value->Length);
		fCurItem->Visitor->Attribute(cnVar::MoveCast(Name),cnVar::MoveCast(Value));
	}
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLParser::OpenTag(const cString<uChar16> &Name)noexcept(true)
{
	auto Visitor=fCurItem->Visitor;
	auto NewItem=new cXMLNodeStackItem;
	NewItem->Parent=fCurItem;
	NewItem->Visitor=Visitor->TagStart(Name);
	NewItem->TagName=Name;
	fCurItem=NewItem;
}
//---------------------------------------------------------------------------
bool cXMLParser::CloseTag(void)noexcept(true)
{
	auto Visitor=fCurItem->Visitor;
	auto DeleteItem=fCurItem;
	fCurItem=fCurItem->Parent;
	// finish current node
	if(fCurItem==nullptr){
		// root node ended
		return true;
	}
	fCurItem->Visitor->TagFinish(Visitor);
	delete DeleteItem;
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cXMLNode::Visit(iXMLVisitor *Visitor)noexcept(true)
{
	cString<uChar16> TagName;
	if(Namespace!=nullptr){
		TagName=Namespace->Prefix+u":"+Name;
	}
	else{
		TagName=Name;
	}
	auto TagVisitor=Visitor->TagStart(TagName);

	if(TagVisitor!=nullptr){
		for(auto &Item : AllItems){
			switch(Item.Type){
			case XMLSymbolType::Attribute:
			{
				auto Attribute=Item.Attribute;
				cString<uChar16> AttributeName;
				if(Attribute->Namespace!=nullptr){
					AttributeName=Attribute->Namespace->Prefix+u":"+Attribute->Name;
				}
				else{
					AttributeName=Attribute->Name;
				}
				TagVisitor->Attribute(AttributeName,Attribute->Value);
			}
				break;
			case XMLSymbolType::Comment:
				TagVisitor->Comment(Item.Text);
				break;
			case XMLSymbolType::Text:
				TagVisitor->Text(Item.Text);
				break;
			case XMLSymbolType::TagStart:
				Item.Child->Visit(TagVisitor);
				break;
			}
		}

		Visitor->TagFinish(TagVisitor);
	}

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cXMLNodeMaker::cXMLNodeMaker()noexcept(true)
{
	Node=aClsCreate<cXMLNode>();
}
//---------------------------------------------------------------------------
iXMLVisitor* cXMLNodeMaker::TagStart(const cString<uChar16> &Name)noexcept(true)
{
	auto NewVisitor=new cXMLNodeMaker;
	NewVisitor->Node->Name=Name;
	return NewVisitor;
}
//---------------------------------------------------------------------------
void cXMLNodeMaker::TagFinish(iXMLVisitor *Visitor)noexcept(true)
{
	auto SubNodeMaker=static_cast<cXMLNodeMaker*>(Visitor);
	auto SubNode=SubNodeMaker->Node;
	delete SubNodeMaker;

	auto *NewItem=Node->AllItems.Append();
	NewItem->Type=XMLSymbolType::TagStart;
	NewItem->Child=SubNode;

	auto Pair=Node->Children.InsertPair(SubNode->Name);
	Pair->Value.AppendMake(SubNode);


}
//---------------------------------------------------------------------------
void cXMLNodeMaker::Text(const cString<uChar16> &Text)noexcept(true)
{
	auto *NewItem=Node->AllItems.Append();
	NewItem->Type=XMLSymbolType::Text;
	NewItem->Text=Text;

	Node->Texts.AppendMake(Text);
}
//---------------------------------------------------------------------------
void cXMLNodeMaker::Comment(const cString<uChar16> &Text)noexcept(true)
{
	auto *NewItem=Node->AllItems.Append();
	NewItem->Type=XMLSymbolType::Comment;
	NewItem->Text=Text;

	Node->Comments.AppendMake(Text);
}
//---------------------------------------------------------------------------
void cXMLNodeMaker::Attribute(const cString<uChar16> &Name,const cString<uChar16> &Value)noexcept(true)
{
	auto Attribute=aClsCreate<cXMLAttribute>();
	Attribute->Name=Name;
	Attribute->Value=Value;

	auto *NewItem=Node->AllItems.Append();
	NewItem->Type=XMLSymbolType::Attribute;
	NewItem->Attribute=Attribute;

	auto AttribPair=Node->Attributes.InsertPair(Name);
	AttribPair->Value.AppendMake(Attribute);
}
//---------------------------------------------------------------------------
