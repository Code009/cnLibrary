//---------------------------------------------------------------------------
#include "XMLParse.h"
#include "TextProcess.h"

using namespace cnLibrary;
using namespace cnRTL;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cXMLLexicalOptions::SetDefault(void)noexcept(true)
{
	cnMemory::ZeroFill(this,sizeof(cXMLLexicalOptions));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cXMLLexicalAnalyzer::cXMLLexicalAnalyzer()noexcept(true)
{
}
//---------------------------------------------------------------------------
cXMLLexicalAnalyzer::~cXMLLexicalAnalyzer()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::SetOptionsDefault(void)noexcept(true)
{
	TranslateEntity=TranslateXMLEntity;
	Options.SetDefault();
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::Reset(void)noexcept(true)
{
	fEscapeState=esNone;
	fState=sSpace;
	fTextError=false;
	fTextMergeSpace=false;
	fAcceptBuffer.Clear();
	fCurPos.Reset();
	fLastPos.Reset();
	fTextOutputPos.Reset();
}
//---------------------------------------------------------------------------
cXMLToken& cXMLLexicalAnalyzer::Output(void)noexcept(true)
{
	return fValue;
}
//---------------------------------------------------------------------------
cXMLLexicalAnalyzer::cTransition cXMLLexicalAnalyzer::Input(uChar32 Character)noexcept(true)
{
	fLastPos=fCurPos;
	switch(Character){
	case '\n':
		fCurPos.MoveLine(1);
		break;
	default:
		fCurPos.MoveCharacter(1);
		break;
	}
	cTransition Transition;
	Transition.Stay=false;
	Transition.Output=false;

	if(fEscapeState!=esNone){
		bool CharStay=false;
		if(EscapeEntity_Process(CharStay,Character)){
			// finished
			switch(fState){
			case sText:
				EscapeEntity_ApplyText();
				break;
			case sTagCore:
				CoreTag_InputEscapeEntity();
				break;
			case sTagExternal:
				fExternalTagLexicalAnalyzer->InputEscapeEntity(fEscapeEntityAcceptBuffer->Pointer,fEscapeEntityAcceptBuffer->Length);
				break;
			}
		}
		if(CharStay==false)
			return Transition;
	}

	switch(fState){
	case sSpace:
		StateTranslate_Space(Transition,Character);
		break;
	case sText:
		StateTranslate_Text(Transition,Character);
		break;
	case sTextError:
		StateTranslate_TextError(Transition,Character);
		break;
	case sTagRecognize:
		StateTranslate_TagRecognize(Transition,Character);
		break;
	case sTagRecognizeWithRollback:
		StateTranslate_TagRecognizeWithRollback(Transition,Character);
		break;
	case sTagRecognizeCommited:
		if(fExternalTagLexicalAnalyzer!=nullptr){
			StateTranslate_TagRecognize_ExternalAccept(Transition,Character);
		}
		else{
			StateTranslate_TagRecognize_CoreAccept(Transition,Character);
		}
		break;
	case sTagCore:
		StateTranslate_TagCore(Transition,Character);
		break;
	case sTagExternal:
		StateTranslate_TagExternal(Transition,Character);
		break;
	}
	if(Transition.Stay){
		fCurPos=fLastPos;
	}
	return Transition;
}
//---------------------------------------------------------------------------
cXMLLexicalAnalyzer::cTransition cXMLLexicalAnalyzer::InputFinish(void)noexcept(true)
{
	fCurPos.MoveLine(1);

	cTransition Transition;
	Transition.Stay=false;
	Transition.Output=false;

	if(fEscapeState!=esNone){
		switch(fState){
		case sText:
			EscapeEntity_ApplyText();
			break;
		case sTagCore:
			CoreTag_InputEscapeEntity();
			break;
		case sTagExternal:
			fExternalTagLexicalAnalyzer->InputEscapeEntity(fEscapeEntityAcceptBuffer->Pointer,fEscapeEntityAcceptBuffer->Length);
			break;
		}
	}

	switch(fState){
	case sSpace:
		if(fAcceptBuffer.GetLength()!=0){
			OutputToken(XMLTokenType::WhiteSpace,false,fTextOutputPos,fLastPos,fAcceptBuffer);
			Transition.Output=true;
		}
		break;
	case sText:
		if(fAcceptBuffer.GetLength()!=0){
			OutputToken(XMLTokenType::Text,fTextError,fTextOutputPos,fLastPos,fAcceptBuffer);
			Transition.Output=true;
		}
		break;
	case sTagRecognize:
		// ignore incompleted tag
		break;
	case sTagRecognizeWithRollback:
		// output incompleted tag as text
		break;
	case sTagRecognizeCommited:
		if(fExternalTagLexicalAnalyzer!=nullptr){
	case sTagExternal:
			// unfinished empty tag
			fExternalTagLexicalAnalyzer->InputFinish();
		}
		else{
	case sTagCore:
			CoreTag_Terminate(Transition);
		}
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
		2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	4,
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

ufInt8 cXMLLexicalAnalyzer::ClassifyTextCharacter(uChar32 c)noexcept(true)
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
		1,	12,	7,	0,	0,	0,	0,	8,	0,	0,	0,	0,	0,	2,	2,	11,		// 0x30
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
ufInt8 cXMLLexicalAnalyzer::ClassifyTagCharacter(uChar32 c)noexcept(true)
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
void cXMLLexicalAnalyzer::EscapeEntity_Begin(void)noexcept(true)
{
	fEscapeEntityAcceptBuffer.SetLength(1);
	fEscapeEntityAcceptBuffer[0]='&';
	fEscapeState=esRef;
}
//---------------------------------------------------------------------------
bool cXMLLexicalAnalyzer::EscapeEntity_Process(bool &CharacterStay,uChar32 Character)noexcept(true)
{
	if(Character==';'){
		switch(fEscapeState){
		default:
		case esNumber:
			cnLib_SWITCH_FALLTHROUGH;
		case esRef:
			fEscapeEntityAcceptBuffer.AppendChar(Character);
			break;
		case esName:
			// translate name
			{
				auto EntityName=fEscapeEntityAcceptBuffer.GetArray();
				EntityName.Pointer++;
				EntityName.Length--;
				auto EntityText=TranslateEntity(EntityName);
				if(EntityText.Pointer!=nullptr){
					fEscapeEntityAcceptBuffer=EntityText;
				}
				else{
					fEscapeEntityAcceptBuffer.AppendChar(';');
				}
			}
			break;
		case esNumber10:
		case esNumber16:
			// translate number to character
			fEscapeEntityAcceptBuffer.SetLength(1);
			fEscapeEntityAcceptBuffer[0]=fEscapeEntityNumber;
			break;
		}
		fEscapeState=esNone;
		return true;
	}
	switch(fEscapeState){
	case esRef:
		switch(ClassifyTagCharacter(Character)){
		case CharacterType_Error:
			if(Character=='#'){
				fEscapeEntityNumber=0;
				fEscapeState=esNumber;
				break;
			}
			cnLib_SWITCH_FALLTHROUGH;
		default:	// error
			// invalid character
			CharacterStay=true;
			fEscapeState=esNone;
			return true;
		case CharacterType_NameInitial:
			fEscapeState=esName;
			break;
		}
		break;
	case esNumber:
		if(Character<'0'){
			// invalid character
			CharacterStay=true;
			fEscapeState=esNone;
			return true;
		}
		else if(Character<='9'){
			fEscapeState=esNumber10;
			// ProcessNumber
			fEscapeEntityNumber*=10;
			fEscapeEntityNumber+=Character-'0';
		}
		else if(Character=='x'){
			fEscapeState=esNumber16;
		}
		else{
			// invalid character
			CharacterStay=true;
			fEscapeState=esNone;
			return true;
		}
		break;
	case esNumber10:
		if(Character>='0' && Character<='9'){
			// ProcessNumber
			fEscapeEntityNumber*=10;
			fEscapeEntityNumber+=Character-'0';
		}
		else{
			// invalid character
			CharacterStay=true;
			fEscapeState=esNone;
			return true;
		}
		break;
	case esNumber16:
		if(Character<'0'){
			// invalid character
			CharacterStay=true;
			fEscapeState=esNone;
			return true;
		}
		else if(Character<='9'){
			// ProcessNumber
			fEscapeEntityNumber*=16;
			fEscapeEntityNumber+=Character-'0';
		}
		else if(Character<'A'){
			// invalid character
			CharacterStay=true;
			fEscapeState=esNone;
			return true;
		}
		else if(Character<='F'){
			// ProcessNumber
			fEscapeEntityNumber*=16;
			fEscapeEntityNumber+=Character-'A'+0xA;
		}
		else if(Character<'a'){
			// invalid character
			CharacterStay=true;
			fEscapeState=esNone;
			return true;
		}
		else if(Character<='f'){
			// ProcessNumber
			fEscapeEntityNumber*=16;
			fEscapeEntityNumber+=Character-'a'+0xA;
		}
		else{
			// invalid character
			CharacterStay=true;
			fEscapeState=esNone;
			return true;
		}
		break;
	}
	fEscapeEntityAcceptBuffer.AppendChar(Character);
	return false;
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::EscapeEntity_Terminate(void)noexcept(true)
{
	fEscapeState=esNone;
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::EscapeEntity_ApplyText(void)noexcept(true)
{
	fAcceptBuffer.Append(fEscapeEntityAcceptBuffer->Pointer,fEscapeEntityAcceptBuffer->Length);
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_Space(cTransition &Transition,ufInt8 ct,uChar32 c)noexcept(true)
{
	switch(ct){
	default:
		if(Options.MergeWhitespaceIntoText){
			fTextMergeSpace=true;
			fState=sText;
			return StateTranslate_Text(Transition,ct,c);
		}
		// output whitespace part
		if(fAcceptBuffer.GetLength()!=0){
			OutputToken(XMLTokenType::WhiteSpace,false,fTextOutputPos,fLastPos,fAcceptBuffer);
			fTextOutputPos=fLastPos;
			Transition.Output=true;
		}
		fTextMergeSpace=false;
		fTextError=false;
		fState=sText;
		return StateTranslate_Text(Transition,ct,c);
	case CharacterType_Whitespace:
		fAcceptBuffer.AppendChar(c);
		break;
	case CharacterType_TagLeft:	// <
		fTagRecognizeList.Clear();	// make sure the list is clean
		for(auto Analyzer : TagAnalyzerSet){
			fTagRecognizeList.AppendMake(Analyzer);
		}
		if(Options.InvalidTagAsText){
			// delay Tag regonization
			fState=sTagRecognizeWithRollback;
			fTextTagRollbackWhitespace=true;
			fTextTagRollbackLength=fAcceptBuffer.GetLength();
			if(fTextTagRollbackLength!=0){
				if(Options.MergeWhitespaceIntoText){
					fTextMergeSpace=true;
				}
				else{
					OutputToken(XMLTokenType::WhiteSpace,false,fTextOutputPos,fLastPos,fAcceptBuffer);
					fTextOutputPos=fLastPos;
					Transition.Output=true;
					fTextTagRollbackLength=0;
				}
			}
			fAcceptBuffer.AppendChar('<');
		}
		else{
			if(fAcceptBuffer.GetLength()!=0){
				OutputToken(XMLTokenType::WhiteSpace,false,fTextOutputPos,fLastPos,fAcceptBuffer);
				Transition.Output=true;
			}
			fState=sTagRecognize;
		}
		fTagOutputPos=fLastPos;
		CoreTag_Start();
		break;
	}
}

//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_Space(cTransition &Transition,uChar32 c)noexcept(true)
{
	return StateTranslate_Space(Transition,ClassifyTextCharacter(c),c);
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_Text(cTransition &Transition,ufInt8 ct,uChar32 c)noexcept(true)
{
	switch(ct){
	default:
	case CharacterType_Error:
		if(fTextError){
			fAcceptBuffer.AppendChar(c);
		}
		else{
			if(Options.MergeErrorIntoText){
				fTextError=true;
				fAcceptBuffer.AppendChar(c);
				break;
			}
			// output text part
			if(fAcceptBuffer.GetLength()!=0){
				OutputToken(XMLTokenType::Text,false,fTextOutputPos,fLastPos,fAcceptBuffer);
				fTextOutputPos=fLastPos;
				Transition.Output=true;
			}
			fState=sTextError;
		}
		break;
	case CharacterType_Whitespace:
		if(fTextMergeSpace==false){
			// output text part
			if(fAcceptBuffer.GetLength()!=0){
				OutputToken(XMLTokenType::Text,fTextError,fTextOutputPos,fLastPos,fAcceptBuffer);
				fTextOutputPos=fLastPos;
				Transition.Output=true;
			}
			fTextMergeSpace=false;
			fTextError=false;
			fState=sSpace;
			fTextOutputPos=fLastPos;
			fAcceptBuffer.AppendChar(c);
			break;
		}
		cnLib_SWITCH_FALLTHROUGH;
	case CharacterType_Text:
		fAcceptBuffer.AppendChar(c);
		break;
	case CharacterType_Reference:	// &
		EscapeEntity_Begin();
		break;
	case CharacterType_TagLeft:	// <
		fTagRecognizeList.Clear();	// make sure the list is clean
		for(auto Analyzer : TagAnalyzerSet){
			fTagRecognizeList.AppendMake(Analyzer);
		}
		if(Options.InvalidTagAsText){
			// delay Tag regonization
			fState=sTagRecognizeWithRollback;
			fTextTagRollbackLength=fAcceptBuffer.GetLength();
			fTextTagRollbackWhitespace=false;
			fAcceptBuffer.AppendChar('<');
		}
		else{
			if(fAcceptBuffer.GetLength()!=0){
				OutputToken(XMLTokenType::Text,fTextError,fTextOutputPos,fLastPos,fAcceptBuffer);
				Transition.Output=true;
			}
			fTextError=false;
			fTextMergeSpace=false;
			fState=sTagRecognize;
		}
		fTagOutputPos=fLastPos;
		CoreTag_Start();
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_Text(cTransition &Transition,uChar32 c)noexcept(true)
{
	return StateTranslate_Text(Transition,ClassifyTextCharacter(c),c);
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_TextError(cTransition &Transition,uChar32 c)noexcept(true)
{
	ufInt8 ct=ClassifyTextCharacter(c);
	switch(ct){
	case CharacterType_Error:
		fAcceptBuffer.AppendChar(c);
		break;
	case CharacterType_Whitespace:
	case CharacterType_TagLeft:
		// transit to space
		if(fAcceptBuffer.GetLength()!=0){
			// output error part
			OutputToken(XMLTokenType::Text,true,fTextOutputPos,fLastPos,fAcceptBuffer);
			fTextOutputPos=fLastPos;
			Transition.Output=true;
			Transition.Stay=true;
			fState=sSpace;
		}
		else{
			fState=sSpace;
			StateTranslate_Space(Transition,ct,c);
		}
		break;
	default:
		// transit to text
		if(fAcceptBuffer.GetLength()!=0){
			// output error part
			OutputToken(XMLTokenType::Text,true,fTextOutputPos,fLastPos,fAcceptBuffer);
			fTextOutputPos=fLastPos;
			Transition.Output=true;
			Transition.Stay=true;
			fState=sText;
		}
		else{
			fState=sText;
			StateTranslate_Text(Transition,ct,c);
		}
		break;
	}
}
//---------------------------------------------------------------------------
ufInt8 cXMLLexicalAnalyzer::TagRecognize(uChar32 c)noexcept(true)
{
	auto TagCharacterType=ClassifyTagCharacter(c);
	uIntn AnalyzerCount=fTagRecognizeList.GetCount();
	for(uIntn i=0;i<AnalyzerCount;i++){
		auto Analyzer=fTagRecognizeList[i];
		auto Result=Analyzer->Recognize(TagCharacterType,c);
		if(Result==iXMLTagLexicalAnalyzer::Accept){
			// switch to external analyzer
			fExternalTagLexicalAnalyzer=Analyzer;
			// reset other analyzers
			for(uIntn di=0;di<i;di++){
				fTagRecognizeList[di]->Discard();
			}
			for(uIntn di=i+1;di<AnalyzerCount;di++){
				fTagRecognizeList[di]->Discard();
			}
			fTagRecognizeList.Clear();
			// reset core tag
			CoreTag_Discard();

			return trAcceptExternal;
		}
		else if(Result==iXMLTagLexicalAnalyzer::Reject){
			// remove analyzer
			Analyzer->Discard();
			fTagRecognizeList.RemoveAt(i);
			i--;
			AnalyzerCount--;
		}
	}
	if(fCoreRecognizeCount==0){
		if(AnalyzerCount==0){
			return trReject;
		}
	}
	else{
		auto CoreTagRecognized=CoreTag_Recognize(TagCharacterType,c);
		if(CoreTagRecognized){
			// switch to core analyzer
			// reset external analyzers
			fExternalTagLexicalAnalyzer=nullptr;
			for(uIntn i=0;i<AnalyzerCount;i++){
				auto Analyzer=fTagRecognizeList[i];
				Analyzer->Discard();
			}
			fTagRecognizeList.Clear();
			return trAcceptCore;
		}
		else{
			if(AnalyzerCount==0 && fCoreRecognizeCount==0){
				return trReject;
			}
		}
	}
	return trRecognizing;
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_TagRecognize(cTransition &Transition,uChar32 c)noexcept(true)
{
	auto Result=TagRecognize(c);
	switch(Result){
	case trAcceptExternal:
		StateTranslate_TagRecognize_ExternalAccept(Transition,c);
		break;
	case trAcceptCore:
	case trReject:
		StateTranslate_TagRecognize_CoreAccept(Transition,c);
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_TagRecognizeWithRollback(cTransition &Transition,uChar32 c)noexcept(true)
{
	auto Result=TagRecognize(c);
	switch(Result){
	case trAcceptExternal:
		if(StateTranslate_TagRecognize_Commit(Transition)){
			fState=sTagRecognizeCommited;
			return;
		}
		else{
			return StateTranslate_TagRecognize_ExternalAccept(Transition,c);
		}
	case trAcceptCore:
		if(StateTranslate_TagRecognize_Commit(Transition)){
			fState=sTagRecognizeCommited;
			return;
		}
		else{
			return StateTranslate_TagRecognize_CoreAccept(Transition,c);
		}
	case trReject:
		CoreTag_Discard();
		fState=sText;
		Transition.Stay=true;
		return;
	}
	// append buffer for text rollback
	fAcceptBuffer.AppendChar(c);
}
//---------------------------------------------------------------------------
bool cXMLLexicalAnalyzer::StateTranslate_TagRecognize_Commit(cTransition &Transition)noexcept(true)
{
	fAcceptBuffer.SetLength(fTextTagRollbackLength);
	if(fTextTagRollbackLength==0){
		return false;
	}
	
	OutputToken(fTextTagRollbackWhitespace?XMLTokenType::WhiteSpace:XMLTokenType::Text,fTextError,fTextOutputPos,fTagOutputPos,fAcceptBuffer);
	Transition.Output=true;
	Transition.Stay=true;
	return true;
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_TagRecognize_CoreAccept(cTransition &Transition,uChar32 c)noexcept(true)
{
	fState=sTagCore;
	if(CoreTag_TakeOver()){
		return StateTranslate_TagCore(Transition,c);
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_TagRecognize_ExternalAccept(cTransition &Transition,uChar32 c)noexcept(true)
{
	fState=sTagExternal;
	if(fExternalTagLexicalAnalyzer->TakeOver()){
		return StateTranslate_TagExternal(Transition,c);
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_TagCore(cTransition &Transition,uChar32 c)noexcept(true)
{
	switch(fCoreTagState){
	default:
		return;
	case tsBegin:
		// should not happen, this state should be processed in recognition procedure
		fCoreTagState=tsInvalid;
		return;
	case tsCloseTagBegin:
		return CoreTag_CloseTagBegin(Transition,c);
	case tsName:
		return CoreTag_Name(Transition,c);
	case tsTag:
		if(CoreTag_Tag(Transition,c))
			break;
		return;
	case tsEnd:
		if(CoreTag_End(Transition,c))
			break;
		return;
	case tsInvalid:
		if(CoreTag_Invalid(Transition,c))
			break;
		return;
	case tsAttribute:
		return CoreTag_Attribute(Transition,c);
	case tsString:
		return CoreTag_String(Transition,c);
	case tsCDATA:
		if(CoreTag_CDATA(Transition,c))
			break;
		return;
	case tsIgnorePI:
		fCoreTagAcceptBuffer.AppendChar(c);
		if(c=='?'){
			fCoreTagState=tsIgnorePIEnd;
		}
		return;
	case tsIgnorePIEnd:
		fCoreTagAcceptBuffer.AppendChar(c);
		if(c=='>'){
			fCoreTagState=tsFinish;
			OutputToken(XMLTokenType::InvalidTag,false,fTagOutputPos,fCurPos,fCoreTagAcceptBuffer);
			Transition.Output=true;
			break;
		}
		fCoreTagState=tsIgnorePI;
		return;
	case tsIgnore:
		fCoreTagAcceptBuffer.AppendChar(c);
		if(c=='>'){
			fCoreTagState=tsFinish;
			OutputToken(XMLTokenType::InvalidTag,false,fTagOutputPos,fCurPos,fCoreTagAcceptBuffer);
			Transition.Output=true;
			break;
		}
		return;
	case tsComment:
		if(CoreTag_Comment(Transition,c))
			break;
		return;
	case tsAcceptWS:
		OutputTagBegin(fTagOutputPos,fLastPos);
		fTagOutputPos=fCurPos;
		fCoreTagState=tsTag;
		Transition.Stay=false;
		Transition.Output=true;
		return;
	case tsAcceptTag:
		OutputTagBegin(fTagOutputPos,fLastPos);
		fTagOutputPos=fLastPos;
		fCoreTagState=tsTag;
		Transition.Stay=true;
		Transition.Output=true;
		return;
	case tsReject:
		OutputTagBegin(fTagOutputPos,fLastPos);
		fTagOutputPos=fLastPos;
		fCoreTagState=tsTag;
		Transition.Stay=true;
		Transition.Output=true;
		return;
	case tsRejectIgnore:
		fCoreTagState=tsFinish;
		OutputToken(XMLTokenType::InvalidTag,false,fTagOutputPos,fCurPos,fCoreTagAcceptBuffer);
		Transition.Output=true;
		break;
	}
	// core tag ended
	fState=sSpace;
	fTextOutputPos=fCurPos;
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::StateTranslate_TagExternal(cTransition &Transition,uChar32 c)noexcept(true)
{
	iXMLTagLexicalAnalyzer::cTransition TagTransition;
	fExternalTagLexicalAnalyzer->Input(TagTransition,c);

	Transition.Stay=TagTransition.Stay;
	if(TagTransition.Entity){
		fEscapeState=esRef;
	}
	if(TagTransition.Finished){
		fState=sSpace;
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::CoreTag_Start(void)noexcept(true)
{
	fCoreTagState=tsBegin;
	fCoreRecognizeCount=3;
	fCoreCommentRecognizeState=0;
	fCoreCDataRecognizeState=0;
	fTagBeginState.Type=TagBeginType_Normal;
	fTagBeginState.InvalidName=false;
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::CoreTag_Discard(void)noexcept(true)
{
	fCoreTagAcceptBuffer.Clear();
}
//---------------------------------------------------------------------------
const char cXMLLexicalAnalyzer::CoreComment_TagName[]={
	'!','-','-'
};
const char cXMLLexicalAnalyzer::CoreCData_TagName[]={
	'!','[','C','D','A','T','A','['
};
//---------------------------------------------------------------------------
bool cXMLLexicalAnalyzer::CoreTag_Recognize(ufInt8 TagCharacterType,uChar32 c)noexcept(true)
{
	if(fCoreCommentRecognizeState!=0xFF){
		if(static_cast<uChar32>(CoreComment_TagName[fCoreCommentRecognizeState])==c){
			fCoreCommentRecognizeState++;
			if(fCoreCommentRecognizeState>=sizeof(CoreComment_TagName)){
				// match
				fCoreTagState=tsComment;
				fCoreTagAcceptBuffer.Clear();
				//fCoreTagTakeOverStay=false;
				return true;
			}
		}
		else{
			fCoreCommentRecognizeState=0xFF;
			fCoreRecognizeCount--;
		}
	}
	if(fCoreCDataRecognizeState!=0xFF){
		if(static_cast<uChar32>(CoreCData_TagName[fCoreCDataRecognizeState])==c){
			fCoreCDataRecognizeState++;
			if(fCoreCDataRecognizeState>=sizeof(CoreCData_TagName)){
				// match
				fCoreTagState=tsCDATA;
				fCoreTagAcceptBuffer.Clear();
				//fCoreTagTakeOverStay=false;
				return true;
			}
		}
		else{
			fCoreCDataRecognizeState=0xFF;
			fCoreRecognizeCount--;
		}
	}
	switch(fCoreTagState){
	case tsBegin:
		switch(TagCharacterType){
		case CharacterType_TagLeft:		// <
		case CharacterType_Whitespace:
			// <<	| <_
			// invalid character
			fCoreTagState=tsIgnore;
			fCoreTagAcceptBuffer.AppendChar('<');
			fCoreTagAcceptBuffer.AppendChar(c);
			fCoreRecognizeCount--;
			break;
		default:
		case CharacterType_Error:
			if(Options.AcceptInvalidTagName){
				// treat as tag name
				fCoreTagAcceptBuffer.AppendChar(c);
				fTagBeginState.Type=TagBeginType_Normal;
				fTagBeginState.InvalidName=true;
				fCoreTagState=tsName;
			}
			else{
				// invalid character
				fCoreTagState=tsIgnore;
				fCoreTagAcceptBuffer.AppendChar('<');
				fCoreTagAcceptBuffer.AppendChar(c);
				fCoreRecognizeCount--;
			}
			break;
		case CharacterType_TagPI:	// ?
			fCoreTagState=tsIgnorePI;
			fCoreTagAcceptBuffer.AppendChar('<');
			fCoreTagAcceptBuffer.AppendChar('?');
			fCoreRecognizeCount--;
			break;
		case CharacterType_TagDTD:	// !
			fCoreTagState=tsIgnore;
			fCoreTagAcceptBuffer.AppendChar('<');
			fCoreTagAcceptBuffer.AppendChar('!');
			fCoreRecognizeCount--;
			break;
		case CharacterType_TagEnd:	// /
			fTagBeginState.Type=TagBeginType_Close;
			fCoreTagState=tsCloseTagBegin;
			break;
		case CharacterType_Name:
			// invalid name
			if(Options.AcceptInvalidTagName==false){
				fCoreTagState=tsReject;
				fCoreRecognizeCount--;
				break;
			}
			// treat as normal tag name but mark as invalid
			fTagBeginState.InvalidName=true;
			cnLib_SWITCH_FALLTHROUGH;
		case CharacterType_NameInitial:
			fCoreTagAcceptBuffer.AppendChar(c);
			fTagBeginState.Type=TagBeginType_Normal;
			fCoreTagState=tsName;
			break;
		case CharacterType_TagRight:	// >
			fCoreTagState=tsRejectIgnore;
			fCoreTagAcceptBuffer.AppendChar('<');
			fCoreTagAcceptBuffer.AppendChar('>');
			fCoreRecognizeCount--;
			break;
		}
		break;
	case tsCloseTagBegin:
		cnLib_ASSERT(fTagBeginState.InvalidName==false);
		switch(TagCharacterType){
		default:
		case CharacterType_Error:
			// invalid character
			if(Options.AcceptInvalidTagName){
				// treat as tag name
				fTagBeginState.InvalidName=true;
				fCoreTagAcceptBuffer.AppendChar(c);
				fCoreTagState=tsName;
				break;
			}
			// rejected
			fCoreTagState=tsIgnore;
			fCoreTagAcceptBuffer.AppendChar('<');
			fCoreTagAcceptBuffer.AppendChar('/');
			fCoreTagAcceptBuffer.AppendChar(c);
			fCoreRecognizeCount--;
			break;
		case CharacterType_TagEnd:	// /
			// <//
			fCoreTagState=tsIgnore;
			fCoreTagAcceptBuffer.AppendChar('<');
			fCoreTagAcceptBuffer.AppendChar('/');
			fCoreTagAcceptBuffer.AppendChar('/');
			fCoreRecognizeCount--;
			break;
		case CharacterType_TagLeft:	// <
		case CharacterType_Whitespace:
		case CharacterType_Equ:		// =
		case CharacterType_StringQ:	// "
		case CharacterType_StringA:	// '
			// </<
			// </_
			// </=
			// </"
			// </'
			fCoreTagState=tsReject;
			fCoreRecognizeCount--;
			break;
		case CharacterType_Name:
			// error name
			if(Options.AcceptInvalidTagName==false){
				fCoreTagState=tsReject;
				fCoreRecognizeCount--;
				break;
			}
			fTagBeginState.InvalidName=true;
			// treat as corrent tag name but mark as invalid
			cnLib_SWITCH_FALLTHROUGH;
		case CharacterType_NameInitial:
			fCoreTagAcceptBuffer.AppendChar(c);
			fCoreTagState=tsName;
			break;
		case CharacterType_TagRight:	// >
			fCoreTagState=tsAcceptTag;
			return true;
		}
		break;
	case tsEnd:
		if(c=='>'){
			fCoreTagState=tsAcceptEnd;
			return true;
		}
		// invalid tag end
		cnLib_ASSERT(fTagEndState.Rollback==TagEndRollback_TagName);

		switch(fTagEndState.Type){
		case TagEndType_Close:
			fCoreTagAcceptBuffer.AppendChar('/');
			break;
		}
		fTagBeginState.InvalidName=true;
		cnLib_SWITCH_FALLTHROUGH;
	case tsName:
		switch(TagCharacterType){
		default:
		case CharacterType_Error:
		case CharacterType_TagLeft:	// <
			if(fTagBeginState.InvalidName){
				// accept invalid name
				fCoreTagAcceptBuffer.AppendChar(c);
				break;
			}
			// invalid character
			if(Options.AcceptInvalidTagName){
				// treat as tag name
				fTagBeginState.InvalidName=true;
				fCoreTagAcceptBuffer.AppendChar(c);
				break;
			}
			fCoreTagState=tsReject;
			fCoreRecognizeCount--;
			break;
		case CharacterType_TagEnd:	// /
			if(Options.AcceptInvalidTagName){
				fTagEndState.Type=TagEndType_Close;
				fCoreTagState=tsEnd;
				// will merge invalid tag name if tag end is false
				fTagEndState.Rollback=TagEndRollback_TagName;
				fTagRollbackPos=fLastPos;
			}
			else{
				fCoreTagState=tsAcceptTag;
				return true;
			}
			break;
		case CharacterType_Name:
		case CharacterType_NameInitial:
			fCoreTagAcceptBuffer.AppendChar(c);
			break;
		case CharacterType_Whitespace:
			fCoreTagState=tsAcceptWS;
			return true;
		case CharacterType_Equ:			// =
		case CharacterType_StringQ:		// "
		case CharacterType_StringA:		// '
		case CharacterType_TagRight:	// >
			fCoreTagState=tsAcceptTag;
			return true;
		}
		break;
	case tsReject:
		// accept character in case that take over
		fCoreTagAcceptBuffer.AppendChar(c);
		break;
	}
	return false;
}
//---------------------------------------------------------------------------
bool cXMLLexicalAnalyzer::CoreTag_TakeOver(void)noexcept(true)
{
	switch(fCoreTagState){
	case tsReject:
	case tsRejectIgnore:
	case tsAcceptWS:
	case tsAcceptTag:
		return true;
	case tsAcceptEnd:
		fCoreTagState=tsEnd;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::CoreTag_Terminate(cTransition &Transition)noexcept(true)
{
	switch(fCoreTagState){
	case tsBegin:
		fCoreTagAcceptBuffer.AppendChar('<');
		OutputToken(XMLTokenType::InvalidTag,false,fTagOutputPos,fLastPos,fCoreTagAcceptBuffer);
		fTagOutputPos=fLastPos;
		Transition.Output=true;
		break;
	case tsCloseTagBegin:
		fCoreTagAcceptBuffer.AppendChar('<');
		fCoreTagAcceptBuffer.AppendChar('/');
		OutputToken(XMLTokenType::InvalidTag,false,fTagOutputPos,fLastPos,fCoreTagAcceptBuffer);
		fTagOutputPos=fLastPos;
		Transition.Output=true;
		break;
	case tsName:
		// unfinished Tag start
		OutputTagBegin(fTagOutputPos,fLastPos);
		fTagOutputPos=fLastPos;
		Transition.Output=true;
		break;
	case tsTag:
		break;
	case tsInvalid:
		OutputToken(XMLTokenType::Attribute,true,fTagOutputPos,fLastPos,fCoreTagAcceptBuffer);
		fTagOutputPos=fLastPos;
		Transition.Output=true;
		break;
	case tsAttribute:
		OutputToken(XMLTokenType::Attribute,false,fTagOutputPos,fLastPos,fCoreTagAcceptBuffer);
		fTagOutputPos=fLastPos;
		Transition.Output=true;
		break;
	case tsString:
		OutputUnfinishedString(fTagOutputPos,fLastPos);
		fTagOutputPos=fLastPos;
		Transition.Output=true;
		break;
	case tsEnd:
		break;
	case tsComment:
		OutputToken(XMLTokenType::Comment,false,fTagOutputPos,fLastPos,fCoreTagAcceptBuffer);
		fTagOutputPos=fLastPos;
		Transition.Output=true;
		break;
	case tsCDATA:
		OutputToken(XMLTokenType::Text,false,fTagOutputPos,fLastPos,fCoreTagAcceptBuffer);
		fTagOutputPos=fLastPos;
		Transition.Output=true;
		break;
	case tsFinish:
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::CoreTag_CloseTagBegin(cTransition &Transition,uChar32 c)noexcept(true)
{
	cnLib_ASSERT(fTagBeginState.InvalidName==false);
	switch(ClassifyTagCharacter(c)){
	default:
	case CharacterType_Error:
	case CharacterType_TagLeft:	// <
	case CharacterType_TagPI:	// ?
		// invalid character
		if(Options.AcceptInvalidTagName){
			// treat as tag name
			fTagBeginState.InvalidName=true;
			fCoreTagAcceptBuffer.AppendChar(c);
			fCoreTagState=tsName;
			break;
		}
		// invalid tag name, ignore this tag
		fCoreTagState=tsIgnore;
		fCoreTagAcceptBuffer.AppendChar('<');
		fCoreTagAcceptBuffer.AppendChar('/');
		fCoreTagAcceptBuffer.AppendChar('c');
		break;
	case CharacterType_TagEnd:	// /
	case CharacterType_Whitespace:
		// invalid tag name
		fCoreTagState=tsIgnore;
		fCoreTagAcceptBuffer.AppendChar('<');
		fCoreTagAcceptBuffer.AppendChar('/');
		fCoreTagAcceptBuffer.AppendChar('/');
		break;
	case CharacterType_Name:
		// error name
		fTagBeginState.InvalidName=true;
		if(Options.AcceptInvalidTagName==false){
			OutputTagBegin(fTagOutputPos,fLastPos);
			fTagOutputPos=fLastPos;
			fCoreTagAcceptBuffer.AppendChar(c);
			fCoreTagState=tsAttribute;
			Transition.Output=true;
			break;
		}
		// treat as corrent tag name but mark as invalid
		cnLib_SWITCH_FALLTHROUGH;
	case CharacterType_NameInitial:
		fCoreTagAcceptBuffer.AppendChar(c);
		fCoreTagState=tsName;
		break;
	case CharacterType_TagRight:	// >
	case CharacterType_Equ:		// =
		// output empty Tag start
		OutputTagBegin(fTagOutputPos,fLastPos);
		fTagOutputPos=fLastPos;
		fCoreTagState=tsTag;
		Transition.Stay=true;
		Transition.Output=true;
		break;
	case CharacterType_StringQ:	// "
		OutputTagBegin(fTagOutputPos,fLastPos);
		fTagOutputPos=fLastPos;
		fStringState.IsDQ=true;
		fCoreTagState=tsString;
		Transition.Output=true;
		break;
	case CharacterType_StringA:	// '
		OutputTagBegin(fTagOutputPos,fLastPos);
		fTagOutputPos=fLastPos;
		fStringState.IsDQ=false;
		fCoreTagState=tsString;
		Transition.Output=true;
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::CoreTag_Name(cTransition &Transition,uChar32 c)noexcept(true)
{
	switch(ClassifyTagCharacter(c)){
	default:
	case CharacterType_Error:
	case CharacterType_TagLeft:	// <
	case CharacterType_TagPI:	// ?
		if(fTagBeginState.InvalidName){
			// accept invalid name
			fCoreTagAcceptBuffer.AppendChar(c);
			break;
		}
		// invalid character
		if(Options.AcceptInvalidTagName){
			// treat as tag name
			fTagBeginState.InvalidName=true;
			fCoreTagAcceptBuffer.AppendChar(c);
			break;
		}
		OutputTagBegin(fTagOutputPos,fLastPos);
		fTagOutputPos=fLastPos;
		fCoreTagState=tsTag;
		Transition.Output=true;
		break;
	case CharacterType_TagEnd:	// /
		fTagEndState.Type=TagEndType_Close;
		fCoreTagState=tsEnd;
		if(Options.AcceptInvalidTagName){
			// will merge invalid tag name if tag end is false
			fTagEndState.Rollback=TagEndRollback_TagName;
			fTagRollbackPos=fLastPos;
		}
		else{
			// output Tag start
			OutputTagBegin(fTagOutputPos,fLastPos);
			fTagOutputPos=fLastPos;
			fTagEndState.Rollback=TagEndRollback_Tag;
			Transition.Output=true;
		}
		break;
	case CharacterType_Whitespace:
		// output Tag start
		OutputTagBegin(fTagOutputPos,fLastPos);
		fTagOutputPos=fCurPos;
		fCoreTagState=tsTag;
		Transition.Output=true;
		break;
	case CharacterType_Name:
	case CharacterType_NameInitial:
		fCoreTagAcceptBuffer.AppendChar(c);
		break;
	case CharacterType_TagRight:	// >
	case CharacterType_Equ:		// =
		// output empty Tag start
		OutputTagBegin(fTagOutputPos,fLastPos);
		fTagOutputPos=fLastPos;
		fCoreTagState=tsTag;
		Transition.Stay=true;
		Transition.Output=true;
		break;
	case CharacterType_StringQ:	// "
		OutputTagBegin(fTagOutputPos,fLastPos);
		fTagOutputPos=fLastPos;
		fStringState.IsDQ=true;
		fCoreTagState=tsString;
		Transition.Output=true;
		break;
	case CharacterType_StringA:	// '
		OutputTagBegin(fTagOutputPos,fLastPos);
		fTagOutputPos=fLastPos;
		fStringState.IsDQ=false;
		fCoreTagState=tsString;
		Transition.Output=true;
		break;
	}
}
//---------------------------------------------------------------------------
bool cXMLLexicalAnalyzer::CoreTag_End(cTransition &Transition,uChar32 c)noexcept(true)
{
	if(c=='>'){
		switch(fTagEndState.Rollback){
		case TagEndRollback_Tag:
			// nothing remains to output
			OutputTagEnd(fTagOutputPos,fCurPos);
			fTagOutputPos=fCurPos;
			fCoreTagAcceptBuffer.Clear();
			fCoreTagState=tsFinish;
			Transition.Output=true;
			return true;
		case TagEndRollback_TagName:
			OutputTagBegin(fTagOutputPos,fTagRollbackPos);
			fTagOutputPos=fTagRollbackPos;
			break;
		case TagEndRollback_Invalid:
			OutputToken(XMLTokenType::Attribute,true,fTagOutputPos,fTagRollbackPos,fCoreTagAcceptBuffer);
			fTagOutputPos=fTagRollbackPos;
			break;
		case TagEndRollback_Attribute:
			OutputToken(XMLTokenType::Attribute,fAttributeState.InvalidName,fTagOutputPos,fTagRollbackPos,fCoreTagAcceptBuffer);
			fTagOutputPos=fTagRollbackPos;
			break;
		}
		// output remaining token then tag end
		Transition.Output=true;
		Transition.Stay=true;
		fTagEndState.Rollback=TagEndRollback_Tag;
		return false;
	}
	// invalid tag end

	switch(fTagEndState.Type){
	case TagEndType_Close:
		fCoreTagAcceptBuffer.AppendChar('/');
		break;
	}

	switch(fTagEndState.Rollback){
	case TagEndRollback_Tag:
		if(Options.AcceptInvalidAttributeName){
			fCoreTagState=tsAttribute;
			fAttributeState.InvalidName=true;
		}
		else{
			fCoreTagState=tsInvalid;
		}
		Transition.Stay=true;
		break;
	case TagEndRollback_TagName:
		// merge tag end into tag name
		fCoreTagState=tsName;
		fTagBeginState.InvalidName=true;
		Transition.Stay=true;
		break;
	case TagEndRollback_Invalid:
		fCoreTagState=tsInvalid;
		Transition.Stay=true;
		break;
	case TagEndRollback_Attribute:
		fAttributeState.InvalidName=true;
		fCoreTagState=tsAttribute;
		Transition.Stay=true;
		break;
	}
	return false;
}
//---------------------------------------------------------------------------
bool cXMLLexicalAnalyzer::CoreTag_Tag(cTransition &Transition,uChar32 c)noexcept(true)
{
	switch(ClassifyTagCharacter(c)){
	default:
	case CharacterType_Error:
	case CharacterType_TagLeft:	// <
	case CharacterType_TagPI:	// ?
	case CharacterType_Name:
		if(Options.AcceptInvalidAttributeName){
			fCoreTagAcceptBuffer.AppendChar(c);
			fCoreTagState=tsAttribute;
			fAttributeState.InvalidName=true;
			break;
		}
		// invalid token
		fCoreTagAcceptBuffer.AppendChar(c);
		fCoreTagState=tsInvalid;
		break;
	case CharacterType_Whitespace:
		fCoreTagAcceptBuffer.Clear();
		fTagOutputPos=fCurPos;
		break;
	case CharacterType_TagEnd:	// /
		fTagEndState.Type=TagEndType_Close;
		fTagEndState.Rollback=TagEndRollback_Tag;
		fTagRollbackPos=fLastPos;
		fCoreTagState=tsEnd;
		break;
	case CharacterType_TagRight:	// >
		OutputEmpty(XMLTokenType::TagEnd,false,fTagOutputPos,fCurPos);
		fTagOutputPos=fCurPos;
		fCoreTagState=tsFinish;
		Transition.Output=true;
		return true;
	case CharacterType_NameInitial:
		fCoreTagAcceptBuffer.AppendChar(c);
		fCoreTagState=tsAttribute;
		fAttributeState.InvalidName=false;
		break;
	case CharacterType_Equ:		// =
		OutputEmpty(XMLTokenType::Equ,false,fTagOutputPos,fCurPos);
		fTagOutputPos=fCurPos;
		Transition.Output=true;
		break;
	case CharacterType_StringQ:	// "
		fStringState.IsDQ=true;
		fCoreTagState=tsString;
		break;
	case CharacterType_StringA:	// '
		fStringState.IsDQ=false;
		fCoreTagState=tsString;
		break;
	}
	return false;
}
//---------------------------------------------------------------------------
bool cXMLLexicalAnalyzer::CoreTag_Invalid(cTransition &Transition,uChar32 c)noexcept(true)
{
	switch(ClassifyTagCharacter(c)){
	default:
	case CharacterType_Error:
	case CharacterType_TagLeft:	// <
	case CharacterType_TagPI:	// ?
	case CharacterType_Name:
		// continue error text
		fCoreTagAcceptBuffer.AppendChar(c);
		break;
	case CharacterType_TagEnd:	// /
		fTagEndState.Type=TagEndType_Close;
		fTagEndState.Rollback=TagEndRollback_Invalid;
		fTagRollbackPos=fLastPos;
		fCoreTagState=tsEnd;
		break;
	case CharacterType_TagRight:	// >
		OutputToken(XMLTokenType::Attribute,true,fTagOutputPos,fLastPos,fCoreTagAcceptBuffer);
		fTagOutputPos=fLastPos;
		Transition.Output=true;
		fCoreTagState=tsFinish;
		return true;
	case CharacterType_Whitespace:
		OutputToken(XMLTokenType::Attribute,true,fTagOutputPos,fLastPos,fCoreTagAcceptBuffer);
		Transition.Output=true;
		fCoreTagAcceptBuffer.Clear();
		fTagOutputPos=fCurPos;
		fCoreTagState=tsTag;
		break;
	case CharacterType_NameInitial:
	case CharacterType_Equ:		// =
	case CharacterType_StringQ:	// "
	case CharacterType_StringA:	// '
		// ignore
		OutputToken(XMLTokenType::Attribute,true,fTagOutputPos,fLastPos,fCoreTagAcceptBuffer);
		fTagOutputPos=fLastPos;
		Transition.Output=true;
		Transition.Stay=true;
		fCoreTagState=tsTag;
		break;
	}
	return false;
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::CoreTag_Attribute(cTransition &Transition,uChar32 c)noexcept(true)
{
	switch(ClassifyTagCharacter(c)){
	default:
	case CharacterType_Error:
	case CharacterType_TagPI:	// ?
	case CharacterType_TagLeft:	// <
		// invalid character
		if(Options.AcceptInvalidAttributeName){
			fCoreTagAcceptBuffer.AppendChar(c);
			fAttributeState.InvalidName=true;
			break;
		}
		if(fAttributeState.InvalidName){
			// accepted invalid name already
			fCoreTagAcceptBuffer.AppendChar(c);
			break;
		}
		OutputToken(XMLTokenType::Attribute,fAttributeState.InvalidName,fTagOutputPos,fLastPos,fCoreTagAcceptBuffer);
		fTagOutputPos=fLastPos;

		// invalid character
		fCoreTagState=tsInvalid;
		fCoreTagAcceptBuffer.AppendChar(c);
		Transition.Output=true;
		break;
	case CharacterType_Whitespace:
		OutputToken(XMLTokenType::Attribute,fAttributeState.InvalidName,fTagOutputPos,fLastPos,fCoreTagAcceptBuffer);
		fTagOutputPos=fCurPos;

		fCoreTagState=tsTag;
		Transition.Output=true;
		break;
	case CharacterType_TagEnd:	// /
		// output attribute name
		fTagEndState.Type=TagEndType_Close;
		fCoreTagState=tsEnd;
		if(Options.AcceptInvalidAttributeName){
			// will merge invalid attribute name if tag end is false
			fTagEndState.Rollback=TagEndRollback_Attribute;
			fTagRollbackPos=fLastPos;
		}
		else{
			// output attribute
			OutputToken(XMLTokenType::Attribute,fAttributeState.InvalidName,fTagOutputPos,fLastPos,fCoreTagAcceptBuffer);
			fTagOutputPos=fLastPos;
			fTagEndState.Rollback=TagEndRollback_Tag;
			Transition.Output=true;
		}
		break;
	case CharacterType_Name:
	case CharacterType_NameInitial:
		fCoreTagAcceptBuffer.AppendChar(c);
		break;
	case CharacterType_Equ:			// =
	case CharacterType_TagRight:	// >
		// output attribute name
		OutputToken(XMLTokenType::Attribute,fAttributeState.InvalidName,fTagOutputPos,fLastPos,fCoreTagAcceptBuffer);
		fTagOutputPos=fLastPos;
		Transition.Stay=true;
		fCoreTagState=tsTag;
		Transition.Output=true;
		break;
	case CharacterType_StringQ:	// "
		OutputToken(XMLTokenType::Attribute,fAttributeState.InvalidName,fTagOutputPos,fLastPos,fCoreTagAcceptBuffer);
		fTagOutputPos=fLastPos;
		fStringState.IsDQ=true;
		fCoreTagState=tsString;
		Transition.Output=true;
		break;
	case CharacterType_StringA:	// '
		OutputToken(XMLTokenType::Attribute,fAttributeState.InvalidName,fTagOutputPos,fLastPos,fCoreTagAcceptBuffer);
		fTagOutputPos=fLastPos;
		fStringState.IsDQ=false;
		fCoreTagState=tsString;
		Transition.Output=true;
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::CoreTag_String(cTransition &Transition,uChar32 c)noexcept(true)
{
	switch(c){
	case '&':
		EscapeEntity_Begin();
		break;
	case '\"':
		if(fStringState.IsDQ){
			OutputToken(XMLTokenType::String,false,fTagOutputPos,fCurPos,fCoreTagAcceptBuffer);
			fTagOutputPos=fCurPos;
			fCoreTagState=tsTag;
			Transition.Output=true;
			break;
		}
		fCoreTagAcceptBuffer.AppendChar('\"');
		break;
	case '\'':
		if(fStringState.IsDQ==false){
			OutputToken(XMLTokenType::String,false,fTagOutputPos,fCurPos,fCoreTagAcceptBuffer);
			fTagOutputPos=fCurPos;
			fCoreTagState=tsTag;
			Transition.Output=true;
			break;
		}
		fCoreTagAcceptBuffer.AppendChar('\'');
		break;
	default:
		fCoreTagAcceptBuffer.AppendChar(c);
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::CoreTag_InputEscapeEntity(void)noexcept(true)
{
	fCoreTagAcceptBuffer.Append(fEscapeEntityAcceptBuffer->Pointer,fEscapeEntityAcceptBuffer->Length);
}
//---------------------------------------------------------------------------
const char cXMLLexicalAnalyzer::CoreComment_TagEnd[]={
	'-','-','>'
};
const char cXMLLexicalAnalyzer::CoreCData_TagEnd[]={
	']',']','>'
};
//---------------------------------------------------------------------------
bool cXMLLexicalAnalyzer::CoreTag_Comment(cTransition &Transition,uChar32 c)noexcept(true)
{
	if(c==static_cast<uChar32>(CoreComment_TagEnd[fCommentState.EndLength])){
		fCommentState.EndLength++;
		if(fCommentState.EndLength>=sizeof(CoreComment_TagEnd)){
			fCoreTagAcceptBuffer.Truncate(sizeof(CoreComment_TagEnd)-1);
			OutputToken(XMLTokenType::Comment,false,fTagOutputPos,fCurPos,fCoreTagAcceptBuffer);
			fTagOutputPos=fCurPos;
			fCoreTagState=tsFinish;
			Transition.Output=true;
			return true;
		}
	}
	else{
		fCommentState.EndLength=0;
	}
	fCoreTagAcceptBuffer.AppendChar(c);
	return false;
}
//---------------------------------------------------------------------------
bool cXMLLexicalAnalyzer::CoreTag_CDATA(cTransition &Transition,uChar32 c)noexcept(true)
{
	if(c==static_cast<uChar32>(CoreCData_TagEnd[fCDATAState.EndLength])){
		fCDATAState.EndLength++;
		if(fCDATAState.EndLength>=sizeof(CoreCData_TagEnd)){
			fCoreTagAcceptBuffer.Truncate(2);
			OutputToken(XMLTokenType::CDATA,false,fTagOutputPos,fCurPos,fCoreTagAcceptBuffer);
			fTagOutputPos=fCurPos;
			fCoreTagState=tsFinish;
			Transition.Output=true;
			return true;
		}
	}
	else{
		fCDATAState.EndLength=0;
	}
	fCoreTagAcceptBuffer.AppendChar(c);
	return false;
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::OutputToken(eXMLTokenType Type,bool InvalidText,const cTokenPosition &StartPosition,const cTokenPosition &EndingPosition,cStringBuffer<uChar32> &Buffer)noexcept(true)
{
	fValue.Text=cnVar::MoveCast(Buffer);
	fValue.Type=Type;
	fValue.InvalidText=InvalidText;
	fValue.UnfinishedString=false;
	fValue.FromLine=StartPosition.Line;
	fValue.FromCharacter=StartPosition.Index;
	fValue.ToLine=EndingPosition.Line;
	fValue.ToCharacter=EndingPosition.Index;
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::OutputEmpty(eXMLTokenType Type,bool InvalidText,const cTokenPosition &StartPosition,const cTokenPosition &EndingPosition)noexcept(true)
{
	fValue.Text.Clear();
	fValue.Type=Type;
	fValue.InvalidText=InvalidText;
	fValue.UnfinishedString=false;
	fValue.FromLine=StartPosition.Line;
	fValue.FromCharacter=StartPosition.Index;
	fValue.ToLine=EndingPosition.Line;
	fValue.ToCharacter=EndingPosition.Index;
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::OutputTagBegin(const cTokenPosition &StartingPosition,const cTokenPosition &EndingPosition)noexcept(true)
{
	switch(fTagBeginState.Type){
	default:
	case TagBeginType_Normal:
		fValue.Type=XMLTokenType::TagBegin;
		break;
	case TagBeginType_Close:
		fValue.Type=XMLTokenType::CloseTagBegin;
		break;
	}
	fValue.UnfinishedString=false;
	fValue.FromLine=StartingPosition.Line;
	fValue.FromCharacter=StartingPosition.Index;
	fValue.ToLine=EndingPosition.Line;
	fValue.ToCharacter=EndingPosition.Index;
	if(fCoreTagAcceptBuffer.GetLength()==0){
		fValue.InvalidText=true;
		fValue.Text=nullptr;
	}
	else{
		fValue.InvalidText=fTagBeginState.InvalidName;
		fValue.Text=cnVar::MoveCast(fCoreTagAcceptBuffer);
	}
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::OutputTagEnd(const cTokenPosition &StartingPosition,const cTokenPosition &EndingPosition)noexcept(true)
{
	cnLib_ASSERT(fTagEndState.Type==TagEndType_Close);
	//switch(fTagEndState.Type){
	//case TagEndType_Close:
		fValue.Type=XMLTokenType::CloseTagEnd;
	//	break;
	//}
	fValue.InvalidText=false;
	fValue.UnfinishedString=false;
	fValue.FromLine=StartingPosition.Line;
	fValue.FromCharacter=StartingPosition.Index;
	fValue.ToLine=EndingPosition.Line;
	fValue.ToCharacter=EndingPosition.Index;
	fValue.Text.Clear();
}
//---------------------------------------------------------------------------
void cXMLLexicalAnalyzer::OutputUnfinishedString(const cTokenPosition &StartingPosition,const cTokenPosition &EndingPosition)noexcept(true)
{
	fValue.Type=XMLTokenType::String;
	fValue.InvalidText=false;
	fValue.UnfinishedString=true;
	fValue.FromLine=StartingPosition.Line;
	fValue.FromCharacter=StartingPosition.Index;
	fValue.ToLine=EndingPosition.Line;
	fValue.ToCharacter=EndingPosition.Index;
	fValue.Text=cnVar::MoveCast(fCoreTagAcceptBuffer);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cXMLStatement::ClearFlags(void)noexcept(true)
{
	cnMemory::ZeroFill(TextFlags);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cXMLSyntaxOptions::SetDefault(void)noexcept(true)
{
	cnMemory::ZeroFill(this,sizeof(cXMLSyntaxOptions));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cXMLSyntaxAnalyzer::cXMLSyntaxAnalyzer()noexcept(true)
	: fCurItem(&fRootItem)
{
}
//---------------------------------------------------------------------------
cXMLSyntaxAnalyzer::~cXMLSyntaxAnalyzer()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::Reset(void)
{
	while(fCurItem!=&fRootItem){
		auto DeleteItem=fCurItem;
		fCurItem=fCurItem->Parent;
		delete DeleteItem;
	}

	fState=sText;
	fRootItem.Parent=nullptr;
	fRootItem.DefaultNamespace.Clear();
	fRootItem.TagNamespace.Clear();
	fRootItem.NamespacePrefixMap.Clear();
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::SetOptionsDefault(void)noexcept(true)
{
	Options.SetDefault();
}
//---------------------------------------------------------------------------
cXMLStatement& cXMLSyntaxAnalyzer::Output(void)noexcept(true)
{
	return fValue;
}
//---------------------------------------------------------------------------
cXMLSyntaxAnalyzer::cTransition cXMLSyntaxAnalyzer::Input(const cXMLToken &Token)noexcept(true)
{
	cTransition Transition;
	Transition.Stay=false;
	Transition.Output=false;

	switch(fState){
	case sText:
		StateTranslate_Text(Transition,Token);
		break;
	case sTag:
		StateTranslate_Tag(Transition,Token);
		break;
	case sCloseTag:
		StateTranslate_CloseTag(Transition,Token);
		break;
	case sCloseTagWithAttribute:
		StateTranslate_CloseTagWithAttribute(Transition,Token);
		break;
	case sMismatchedCloseTag:
		StateTranslate_MismatchedCloseTag(Transition,Token);
		break;
	}

	return Transition;
}
//---------------------------------------------------------------------------
cXMLSyntaxAnalyzer::cTransition cXMLSyntaxAnalyzer::InputFinish(void)noexcept(true)
{
	cTransition Transition;
	Transition.Stay=false;
	Transition.Output=false;

	switch(fState){
	case sFinish:
		break;
	default:
	case sText:
		if(fCurItem==&fRootItem){
			// no more stack node
			return Transition;
		}
		fState=sFinish;
		fTagTokenList.Clear();
		break;
	case sTag:
		if(TagStateTranslate_ProcessFinish()){
			Transition.Output=true;
			Transition.Stay=true;
			return Transition;
		}
		// let tag stack to terminate the node
		cnLib_ASSERT(fCurItem!=&fRootItem);
		fState=sFinish;
		fTagTokenList.Clear();
		break;
	case sCloseTag:
		if(TagStateTranslate_ProcessFinish()){
			Transition.Output=true;
		}
		else{
			// unfinished close tag
			OutputTagClose(fTagTokenList,true,true);
			Transition.Output=true;
			CloseTag();
			if(fCurItem==&fRootItem){
				// no more stack node
				return Transition;
			}
			fState=sFinish;
			fTagTokenList.Clear();
		}
		Transition.Stay=true;
		return Transition;
	case sMismatchedCloseTag:
		// unfinished mismatched close tag
		OutputMismatchedTagClose(cnVar::MoveCast(fInvalidTagText),fTagTokenList);
		Transition.Output=true;
		Transition.Stay=true;
		// let tag stack to terminate the node
		cnLib_ASSERT(fCurItem!=&fRootItem);
		fState=sFinish;
		fTagTokenList.Clear();
		return Transition;
	}

	// terminate stack
	cnLib_ASSERT(fCurItem!=&fRootItem);
	CloseTag();
	OutputTagClose(fTagTokenList,false,true);

	Transition.Output=true;
	Transition.Stay=fCurItem!=&fRootItem;

	return Transition;
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::StateTranslate_Text(cTransition &Transition,const cXMLToken &Token)noexcept(true)
{
	switch(Token.Type){
	default:
		// never happens
		break;
	case XMLTokenType::Comment:
		OutputTextNode(XMLTextStatementType::Comment,Token);
		Transition.Output=true;
		break;
	case XMLTokenType::Text:
		OutputTextNode(XMLTextStatementType::Text,Token);
		Transition.Output=true;
		break;
	case XMLTokenType::WhiteSpace:
		OutputTextNode(XMLTextStatementType::WhiteSpace,Token);
		Transition.Output=true;
		break;
	case XMLTokenType::CDATA:
		OutputTextNode(XMLTextStatementType::CDATA,Token);
		Transition.Output=true;
		break;
	case XMLTokenType::InvalidTag:
		OutputTextNode(XMLTextStatementType::InvalidTag,Token);
		Transition.Output=true;
		break;
	case XMLTokenType::TagBegin:
		fState=sTag;
		OpenTag(Token.Text);
		OutputTagStart(Token);
		Transition.Output=true;
		fTagState=tsNone;
		break;
	case XMLTokenType::CloseTagBegin:
		{
			cString<uChar32> TagPrefix;
			cString<uChar32> TagName=Token.Text;
			auto Namespace=fCurItem->QueryName(TagPrefix,TagName);

			fTagTokenList.Clear();
			fTagTokenList.AppendMake(Token);
			if(TagName!=fCurItem->TagName || 
				(Options.AcceptCloseTagPrefixMismatch?Namespace!=fCurItem->TagNamespace:TagPrefix!=fCurItem->TagPrefix))
			{
				// mismatched close tag
				fState=sMismatchedCloseTag;
				fInvalidTagText.AppendChar('<');
				fInvalidTagText.AppendChar('/');
				fInvalidTagText.Append(Token.Text,Token.Text->Length);
			}
			else{
				if(Options.AcceptCloseTagAttributes){
					fState=sCloseTagWithAttribute;
				}
				else{
					fState=sCloseTag;
				}
			}
		}
		fTagState=tsNone;
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::StateTranslate_Tag(cTransition &Transition,const cXMLToken &Token)noexcept(true)
{
	if(TagStateTranslate(Transition,Token))
		return;

	switch(Token.Type){
	default:
		// never happens
		break;
	case XMLTokenType::CloseTagEnd:
		CloseTag();
		OutputTagClose(Token,false);
		fState=sText;
		Transition.Output=true;
		break;
	case XMLTokenType::TagEnd:
		OutputTagFinish(Token);
		fState=sText;
		Transition.Output=true;
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::StateTranslate_CloseTag(cTransition &Transition,const cXMLToken &Token)noexcept(true)
{
	bool InvalidTagFinish=false;
	switch(Token.Type){
	default:
		// never happens
		break;
	case XMLTokenType::CloseTagEnd:
		// should use normal tag end
		InvalidTagFinish=true;
		cnLib_SWITCH_FALLTHROUGH;
	case XMLTokenType::TagEnd:
		fTagTokenList.AppendMake(Token);
		OutputTagClose(fTagTokenList,InvalidTagFinish,false);
		CloseTag();
		fState=sText;
		Transition.Output=true;
		break;
	case XMLTokenType::Attribute:
	case XMLTokenType::Equ:
	case XMLTokenType::String:
		// just ignore attributes
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::StateTranslate_CloseTagWithAttribute(cTransition &Transition,const cXMLToken &Token)noexcept(true)
{
	if(TagStateTranslate(Transition,Token))
		return;

	bool InvalidTagFinish=false;
	switch(Token.Type){
	default:
		// never happens
		break;
	case XMLTokenType::CloseTagEnd:
		// should use normal tag end
		InvalidTagFinish=true;
		cnLib_SWITCH_FALLTHROUGH;
	case XMLTokenType::TagEnd:
		fTagTokenList.AppendMake(Token);
		OutputTagClose(fTagTokenList,InvalidTagFinish,false);
		CloseTag();
		fState=sText;
		Transition.Output=true;
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::StateTranslate_MismatchedCloseTag(cTransition &Transition,const cXMLToken &Token)noexcept(true)
{
	switch(Token.Type){
	default:
		// never happens
		fInvalidTagText.AppendChar(' ');
		fInvalidTagText.Append(Token.Text);
		break;
	case XMLTokenType::CloseTagEnd:
		// should use normal tag end
		fInvalidTagText.AppendChar('/');
		cnLib_SWITCH_FALLTHROUGH;
	case XMLTokenType::TagEnd:
		fTagTokenList.AppendMake(Token);
		fInvalidTagText.AppendChar('>');
		OutputMismatchedTagClose(cnVar::MoveCast(fInvalidTagText),fTagTokenList);
		fState=sText;
		Transition.Output=true;
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::OpenTag(cString<uChar32> TagName)noexcept(true)
{
	cString<uChar32> Prefix;
	auto Namespace=fCurItem->QueryName(Prefix,TagName);

	auto NewItem=new cXMLNodeStackItem;
	NewItem->Parent=fCurItem;
	NewItem->TagNamespace=cnVar::MoveCast(Namespace);
	NewItem->TagPrefix=cnVar::MoveCast(Prefix);
	NewItem->TagName=cnVar::MoveCast(TagName);
	NewItem->DefaultNamespace=fCurItem->DefaultNamespace;
	fCurItem=NewItem;
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::CloseTag(void)noexcept(true)
{
	if(fCurItem==&fRootItem){
		// cannot close root item
		return;
	}
	auto DeleteItem=fCurItem;
	fCurItem=fCurItem->Parent;
	delete DeleteItem;
}
//---------------------------------------------------------------------------
bool cXMLSyntaxAnalyzer::TagStateTranslate(cTransition &Transition,const cXMLToken &Token)noexcept(true)
{
	switch(fTagState){
	default:
	case tsNone:
		return TagStateTranslate_None(Transition,Token);
	case tsAttribute:
		TagStateTranslate_Attribute(Transition,Token);
		break;
	case tsAttributeEqu:
		TagStateTranslate_AttributeEqu(Transition,Token);
		break;
	case tsAttributeEquA:
		TagStateTranslate_AttributeEquA(Transition,Token);
		break;
	}
	return true;
}
//---------------------------------------------------------------------------
bool cXMLSyntaxAnalyzer::TagStateTranslate_ProcessFinish(void)noexcept(true)
{
	switch(fTagState){
	default:
	case tsNone:
		return false;
	case tsAttribute:
		// terminate attribute
		fTagState=tsNone;
		return false;
	case tsAttributeEqu:
		// attribute without value
		fAttributeValueToken.Type=XMLTokenType::WhiteSpace;
		fTagState=tsNone;
		return TagStateTranslate_ProcessAttribute(fAttributeNameToken,fAttributeEquToken,fAttributeValueToken);
	case tsAttributeEquA:
		fTagState=tsNone;
		return TagStateTranslate_ProcessAttribute(fAttributeNameToken,fAttributeEquToken,fAttributeValueToken);
	}
}
//---------------------------------------------------------------------------
bool cXMLSyntaxAnalyzer::TagStateTranslate_None(cTransition &Transition,const cXMLToken &Token)noexcept(true)
{
	switch(Token.Type){
	default:
		return false;
	case XMLTokenType::Attribute:
		fTagState=tsAttribute;
		fAttributeNameToken=cnVar::MoveCast(Token);
		break;
	case XMLTokenType::Equ:
		fAttributeNameToken.Type=XMLTokenType::WhiteSpace;
		fTagState=tsAttributeEqu;
		break;
	case XMLTokenType::String:
		// error
		if(Options.AcceptAttributeWithoutName){
			fAttributeEquToken.Type=XMLTokenType::WhiteSpace;
			OutputAttributeNoName(fAttributeEquToken,Token);
			Transition.Output=true;
		}
		break;
	}
	return true;
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::TagStateTranslate_Attribute(cTransition &Transition,const cXMLToken &Token)noexcept(true)
{
	switch(Token.Type){
	default:
	case XMLTokenType::TagBegin:
	case XMLTokenType::CloseTagBegin:
	case XMLTokenType::CloseTagEnd:
	case XMLTokenType::TagEnd:
		// terminate attribute
		fTagState=tsNone;
		Transition.Stay=true;
		break;
	case XMLTokenType::Attribute:
		// output error attribute
		fAttributeEquToken.Type=XMLTokenType::WhiteSpace;
		Transition.Output=TagStateTranslate_ProcessAttribute(fAttributeNameToken,fAttributeEquToken);
		fTagState=tsAttribute;
		fAttributeNameToken=Token;
		break;
	case XMLTokenType::Equ:
		fAttributeEquToken=Token;
		fTagState=tsAttributeEqu;
		break;
	case XMLTokenType::String:
		fAttributeEquToken.Type=XMLTokenType::WhiteSpace;
		Transition.Output=TagStateTranslate_ProcessAttribute(fAttributeNameToken,fAttributeEquToken,Token);
		fTagState=tsNone;
		break;
	}
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::TagStateTranslate_AttributeEqu(cTransition &Transition,const cXMLToken &Token)noexcept(true)
{
	switch(Token.Type){
	default:
	case XMLTokenType::TagBegin:
	case XMLTokenType::CloseTagBegin:
	case XMLTokenType::CloseTagEnd:
	case XMLTokenType::TagEnd:
		// terminate attribute
		fAttributeValueToken.Type=XMLTokenType::WhiteSpace;
		Transition.Output=TagStateTranslate_ProcessAttribute(fAttributeNameToken,fAttributeEquToken,fAttributeValueToken);
		fTagState=tsNone;
		Transition.Stay=true;
		break;
	case XMLTokenType::Attribute:
		if(Options.AcceptAttributeNameAsValue){
			fAttributeValueToken=Token;
			fTagState=tsAttributeEquA;
		}
		else{
			// output error attribute
			Transition.Output=TagStateTranslate_ProcessAttribute(fAttributeNameToken,fAttributeEquToken);
			fTagState=tsAttribute;
		}
		break;
	case XMLTokenType::Equ:
		if(Options.IgnoreAttributeMultipleEqualSign){
			// ignore multiple equal sign
		}
		else{
			Transition.Output=TagStateTranslate_ProcessAttribute(fAttributeNameToken,fAttributeEquToken);
			// error multiple equ
			fAttributeNameToken.Type=XMLTokenType::WhiteSpace;
			fAttributeEquToken=Token;
		}
		break;
	case XMLTokenType::String:
		// attribute finish
		Transition.Output=TagStateTranslate_ProcessAttribute(fAttributeNameToken,fAttributeEquToken,Token);
		fTagState=tsNone;
		break;
	}
}

//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::TagStateTranslate_AttributeEquA(cTransition &Transition,const cXMLToken &Token)noexcept(true)
{
	switch(Token.Type){
	default:
	case XMLTokenType::CloseTagEnd:
	case XMLTokenType::TagEnd:
	case XMLTokenType::Attribute:
		Transition.Output=TagStateTranslate_ProcessAttribute(fAttributeNameToken,fAttributeEquToken,fAttributeValueToken);
		Transition.Stay=true;
		fTagState=tsNone;
		break;
	case XMLTokenType::Equ:
		// output last attribute without value
		Transition.Output=TagStateTranslate_ProcessAttribute(fAttributeNameToken,fAttributeEquToken);
		// next attribute
		fTagState=tsAttributeEqu;
		fAttributeNameToken=cnVar::MoveCast(fAttributeValueToken);
		fAttributeEquToken=Token;
		break;
	case XMLTokenType::String:
		Transition.Output=TagStateTranslate_ProcessAttribute(fAttributeNameToken,fAttributeEquToken,fAttributeValueToken);
		fAttributeNameToken=cnVar::MoveCast(fAttributeValueToken);
		if(Transition.Output){
			fTagState=tsAttribute;
			Transition.Stay=true;
		}
		else{
			// output attribute without equ sign
			fAttributeEquToken.Type=XMLTokenType::WhiteSpace;
			Transition.Output=TagStateTranslate_ProcessAttribute(fAttributeNameToken,fAttributeEquToken,Token);
			fTagState=tsNone;
		}
		break;
	}
}
//---------------------------------------------------------------------------
bool cXMLSyntaxAnalyzer::TagStateTranslate_ProcessAttribute(const cXMLToken &NameToken,const cXMLToken &EquToken,const cXMLToken &ValueToken)noexcept(true)
{
	bool MissingEquSign=EquToken.Type!=XMLTokenType::Equ;
	bool InvalidValue;
	bool UnfinishedString;
	if(ValueToken.Type!=XMLTokenType::String){
		InvalidValue=true;
		UnfinishedString=false;
	}
	else{
		InvalidValue=ValueToken.InvalidText;
		UnfinishedString=ValueToken.UnfinishedString;
	}
	cString<uChar32> Prefix;
	cString<uChar32> Name=NameToken.Text;
	if(SplitNamespace(Prefix,Name)){
		if(Prefix==U"xmlns"){
			// declare namespace prefix
			cString<uChar32> Namespace=ValueToken.Text;
			fCurItem->NamespacePrefixMap.Insert(Name,Namespace);
			if(Name==fCurItem->TagPrefix){
				// change current tag's namespace
				fCurItem->TagNamespace=Namespace;
				fValue.Type=XMLStatementType::TagNamespace;
				fValue.ClearFlags();
				fValue.TagNamespaceFlags.InvalidName=NameToken.InvalidText;
				fValue.TagNamespaceFlags.InvalidValue=InvalidValue;
				fValue.TagNamespaceFlags.UnfinishedString=UnfinishedString;
				fValue.TagNamespaceFlags.MissingEqualSign=MissingEquSign;
				fValue.URI=cnVar::MoveCast(Namespace);
				fValue.Prefix.Clear();
				fValue.Name.Clear();
				fValue.Text.Clear();
				fValue.Tokens.Clear();
				fValue.Tokens.AppendMake(NameToken);
				if(!MissingEquSign)
					fValue.Tokens.AppendMake(EquToken);
				if(ValueToken.Type==XMLTokenType::String)
					fValue.Tokens.AppendMake(ValueToken);
				return true;
			}
		}
		else{
			// attribute with namespace
			auto Namespace=fCurItem->FindNamespace(Prefix);

			fValue.Type=XMLStatementType::Attribute;
			fValue.ClearFlags();
			fValue.AttributeFlags.InvalidName=NameToken.InvalidText;
			fValue.AttributeFlags.InvalidValue=InvalidValue;
			fValue.AttributeFlags.UnfinishedString=UnfinishedString;
			fValue.AttributeFlags.MissingEqualSign=MissingEquSign;
			fValue.URI=cnVar::MoveCast(Namespace);
			fValue.Prefix=cnVar::MoveCast(Prefix);
			fValue.Name=cnVar::MoveCast(Name);
			fValue.Text=ValueToken.Text;
			fValue.Tokens.Clear();
			fValue.Tokens.AppendMake(NameToken);
			if(!MissingEquSign)
				fValue.Tokens.AppendMake(EquToken);
			if(ValueToken.Type==XMLTokenType::String)
				fValue.Tokens.AppendMake(ValueToken);
			return true;
		}
	}
	else{
		if(Name==U"xmlns"){
			// set default namespace
			cString<uChar32> Namespace=ValueToken.Text;
			fCurItem->DefaultNamespace=Namespace;
			if(fCurItem->TagPrefix->Length==0){
				// change current tag's namespace
				fCurItem->TagNamespace=Namespace;
				fValue.Type=XMLStatementType::TagNamespace;
				fValue.ClearFlags();
				fValue.TagNamespaceFlags.InvalidName=false;
				fValue.TagNamespaceFlags.InvalidValue=InvalidValue;
				fValue.TagNamespaceFlags.UnfinishedString=UnfinishedString;
				fValue.TagNamespaceFlags.MissingEqualSign=MissingEquSign;
				fValue.URI=cnVar::MoveCast(Namespace);
				fValue.Prefix.Clear();
				fValue.Name.Clear();
				fValue.Text.Clear();
				fValue.Tokens.Clear();
				fValue.Tokens.AppendMake(NameToken);
				if(!MissingEquSign)
					fValue.Tokens.AppendMake(EquToken);
				if(ValueToken.Type==XMLTokenType::String)
					fValue.Tokens.AppendMake(ValueToken);
				return true;
			}
		}
		else{
			// attribute with default namespace
			fValue.Type=XMLStatementType::Attribute;
			fValue.ClearFlags();
			fValue.AttributeFlags.InvalidName=NameToken.InvalidText;
			fValue.AttributeFlags.InvalidValue=InvalidValue;
			fValue.AttributeFlags.UnfinishedString=UnfinishedString;
			fValue.AttributeFlags.MissingEqualSign=MissingEquSign;
			fValue.URI=fCurItem->DefaultNamespace;
			fValue.Prefix.Clear();
			fValue.Name=cnVar::MoveCast(Name);
			fValue.Text=ValueToken.Text;
			fValue.Tokens.Clear();
			fValue.Tokens.AppendMake(NameToken);
			if(!MissingEquSign)
				fValue.Tokens.AppendMake(EquToken);
			if(ValueToken.Type==XMLTokenType::String)
				fValue.Tokens.AppendMake(ValueToken);
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool cXMLSyntaxAnalyzer::TagStateTranslate_ProcessAttribute(const cXMLToken &NameToken,const cXMLToken &EquToken)noexcept(true)
{
	cXMLToken EmptyToken;
	EmptyToken.Type=XMLTokenType::WhiteSpace;
	return TagStateTranslate_ProcessAttribute(NameToken,EquToken,EmptyToken);
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::OutputTextNode(eXMLTextStatementType TextType,const cXMLToken &Token)noexcept(true)
{
	fValue.Type=XMLStatementType::Text;
	fValue.ClearFlags();
	fValue.TextFlags.Type=TextType;
	fValue.TextFlags.InvalidText=Token.InvalidText;
	fValue.URI.Clear();
	fValue.Prefix.Clear();
	fValue.Name.Clear();
	fValue.Text=Token.Text;
	fValue.Tokens.Clear();
	fValue.Tokens.AppendMake(Token);
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::OutputTagStart(const cXMLToken &Token)noexcept(true)
{
	fValue.Type=XMLStatementType::TagStart;
	fValue.ClearFlags();
	fValue.TagStartFlags.InvalidName=Token.InvalidText;
	fValue.URI=fCurItem->TagNamespace;
	fValue.Prefix=fCurItem->TagPrefix;
	fValue.Name=fCurItem->TagName;
	fValue.Text.Clear();
	fValue.Tokens.Clear();
	fValue.Tokens.AppendMake(Token);
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::OutputTagFinish(const cXMLToken &Token)noexcept(true)
{
	fValue.Type=XMLStatementType::TagFinish;
	fValue.ClearFlags();
	fValue.URI.Clear();
	fValue.Prefix.Clear();
	fValue.Name.Clear();
	fValue.Text.Clear();
	fValue.Tokens.Clear();
	fValue.Tokens.AppendMake(Token);
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::OutputTagClose(const cXMLToken &Token,bool InvalidTagFinish)noexcept(true)
{
	fValue.Type=XMLStatementType::TagClose;
	fValue.ClearFlags();
	fValue.TagCloseFlags.InvalidTagFinish=InvalidTagFinish;
	fValue.TagCloseFlags.UnfinishedNode=false;
	fValue.URI.Clear();
	fValue.Prefix.Clear();
	fValue.Name.Clear();
	fValue.Text.Clear();
	fValue.Tokens.Clear();
	fValue.Tokens.AppendMake(Token);	
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::OutputTagClose(const cSeqList<cXMLToken> &TokenList,bool InvalidTagFinish,bool UnfinishedNode)noexcept(true)
{
	fValue.Type=XMLStatementType::TagClose;
	fValue.ClearFlags();
	fValue.TagCloseFlags.InvalidTagFinish=InvalidTagFinish;
	fValue.TagCloseFlags.UnfinishedNode=UnfinishedNode;
	fValue.URI.Clear();
	fValue.Prefix.Clear();
	fValue.Name.Clear();
	fValue.Text.Clear();
	fValue.Tokens=TokenList;
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::OutputMismatchedTagClose(cString<uChar32> Text,const cSeqList<cXMLToken> &TokenList)noexcept(true)
{
	fValue.Type=XMLStatementType::TagCloseMismatch;
	fValue.ClearFlags();
	fValue.URI.Clear();
	fValue.Prefix.Clear();
	fValue.Name.Clear();
	fValue.Text=cnVar::MoveCast(Text);
	fValue.Tokens=TokenList;
}
//---------------------------------------------------------------------------
void cXMLSyntaxAnalyzer::OutputAttributeNoName(const cXMLToken &EquToken,const cXMLToken &ValueToken)noexcept(true)
{
	bool MissingEquSign=EquToken.Type!=XMLTokenType::Equ;

	fValue.Type=XMLStatementType::Attribute;
	fValue.ClearFlags();
	fValue.AttributeFlags.InvalidName=true;
	fValue.AttributeFlags.InvalidValue=ValueToken.InvalidText;
	fValue.AttributeFlags.UnfinishedString=ValueToken.UnfinishedString;
	fValue.AttributeFlags.MissingEqualSign=MissingEquSign;
	fValue.URI.Clear();
	fValue.Prefix.Clear();
	fValue.Name.Clear();
	fValue.Text=ValueToken.Text;
	fValue.Tokens.Clear();
	if(!MissingEquSign){
		fValue.Tokens.AppendMake(EquToken);
	}
	fValue.Tokens.AppendMake(ValueToken);
}
//---------------------------------------------------------------------------
bool cXMLSyntaxAnalyzer::SplitNamespace(cString<uChar32> &Prefix,cString<uChar32> &Name)noexcept(true)
{
	auto NamespaceIndex=cnString::Search(Name->Pointer,Name->Length,U':');
	if(NamespaceIndex==IndexNotFound){
		// no namespace prefix, use default namespace
		return false;
	}
	else{
		Prefix=Name.SubString(0,NamespaceIndex);
		Name=Name.SubString(NamespaceIndex+1,Name->Length-NamespaceIndex-1);
		return true;
	}
}
//---------------------------------------------------------------------------
cString<uChar32> cXMLSyntaxAnalyzer::cXMLNodeStackItem::FindNamespace(const cString<uChar32> &Prefix)noexcept(true)
{
	auto PrefixPair=NamespacePrefixMap.GetPair(Prefix);
	if(PrefixPair!=nullptr){
		return PrefixPair->Value;
	}
	if(Parent!=nullptr){
		return Parent->FindNamespace(Prefix);
	}
	// new namespace
	return cString<uChar32>();
}
//---------------------------------------------------------------------------
cString<uChar32> cXMLSyntaxAnalyzer::cXMLNodeStackItem::QueryName(cString<uChar32> &Prefix,cString<uChar32> &Name)noexcept(true)
{
	if(SplitNamespace(Prefix,Name)){
		return FindNamespace(Prefix);
	}
	else{
		return DefaultNamespace;
	}
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
cXMLLexicalOptions& cXMLParser::RefLexicalOptions(void)noexcept(true)
{
	return fLexer.Options;
}
//---------------------------------------------------------------------------
cXMLSyntaxOptions& cXMLParser::RefSyntaxOptions(void)noexcept(true)
{
	return fSyntaxer.Options;
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

	// insert default namespace
	fCurItem=&fRootItem;
	fCurItem->Visitor=Visitor;
}
//---------------------------------------------------------------------------
void cXMLParser::Finish(void)noexcept(true)
{
	{
		cXMLLexicalAnalyzer::cTransition Transition;
		do{
			Transition=fLexer.InputFinish();
			if(Transition.Output){
				Syntax();
			}
		}while(Transition.Stay);
	}
	{
		cXMLSyntaxAnalyzer::cTransition Transition;
		do{
			Transition=fSyntaxer.InputFinish();
			if(Transition.Output){
				Grammer();
			}
		}while(Transition.Stay);
	}
}
//---------------------------------------------------------------------------
void cXMLParser::Input(uChar32 c)noexcept(true)
{
	cXMLLexicalAnalyzer::cTransition Transition;
	do{
		Transition=fLexer.Input(c);
		if(Transition.Output){
			Syntax();
		}
	}while(Transition.Stay);
}
//---------------------------------------------------------------------------
void cXMLParser::Syntax(void)noexcept(true)
{
	cXMLSyntaxAnalyzer::cTransition Transition;
	auto &Token=fLexer.Output();
	do{
		Transition=fSyntaxer.Input(Token);
		if(Transition.Output){
			Grammer();
		}
	}while(Transition.Stay);
}
//---------------------------------------------------------------------------
void cXMLParser::Grammer(void)noexcept(true)
{
	auto &Statement=fSyntaxer.Output();

	switch(Statement.Type){
	case XMLStatementType::Text:
		{
			auto Text=cnRTL::CreateStringConvertEncoding<uChar16>(fConverter,Statement.Text->Pointer,Statement.Text->Length);
			switch(Statement.TextFlags.Type){
			case XMLTextStatementType::Text:
			case XMLTextStatementType::WhiteSpace:
			case XMLTextStatementType::CDATA:
				fCurItem->Visitor->Text(Statement.TextFlags.Type,cnVar::MoveCast(Text));
				break;
			case XMLTextStatementType::Comment:
			case XMLTextStatementType::InvalidTag:
				fCurItem->Visitor->Comment(Statement.TextFlags.Type,cnVar::MoveCast(Text));
				break;
			}
		}
		break;
	case XMLStatementType::TagStart:
		{
			auto Namespace=cnRTL::CreateStringConvertEncoding<uChar16>(fConverter,Statement.URI->Pointer,Statement.URI->Length);
			auto Prefix=cnRTL::CreateStringConvertEncoding<uChar16>(fConverter,Statement.Prefix->Pointer,Statement.Prefix->Length);
			auto NodeName=cnRTL::CreateStringConvertEncoding<uChar16>(fConverter,Statement.Name->Pointer,Statement.Name->Length);

			auto NewItem=new cXMLNodeStackItem;
			NewItem->Parent=fCurItem;
			NewItem->Visitor=fCurItem->Visitor->Element(cnVar::MoveCast(Namespace),cnVar::MoveCast(Prefix),cnVar::MoveCast(NodeName));
			fCurItem=NewItem;
		}
		break;
	case XMLStatementType::TagFinish:
		{
			fCurItem->Visitor->TagFinish();
		}
		break;
	case XMLStatementType::TagNamespace:
		{
			auto Namespace=cnRTL::CreateStringConvertEncoding<uChar16>(fConverter,Statement.URI->Pointer,Statement.URI->Length);
			fCurItem->Visitor->TagNamespace(cnVar::MoveCast(Namespace));
		}
		break;
	case XMLStatementType::TagClose:
		{
			auto Visitor=fCurItem->Visitor;
			auto DeleteItem=fCurItem;
			fCurItem=fCurItem->Parent;
			// finish current node
			if(fCurItem==nullptr){
				// root node ended
				fRootNodeFinished=true;
			}
			Visitor->Close();
			delete DeleteItem;
			fRootNodeFinished=false;
		}
		break;
	case XMLStatementType::Attribute:
		{
			// attribute with namespace
			auto Namespace=cnRTL::CreateStringConvertEncoding<uChar16>(fConverter,Statement.URI->Pointer,Statement.URI->Length);
			auto Prefix=cnRTL::CreateStringConvertEncoding<uChar16>(fConverter,Statement.Prefix->Pointer,Statement.Prefix->Length);
			auto NodeName=cnRTL::CreateStringConvertEncoding<uChar16>(fConverter,Statement.Name->Pointer,Statement.Name->Length);
			auto Text=cnRTL::CreateStringConvertEncoding<uChar16>(fConverter,Statement.Text->Pointer,Statement.Text->Length);
			fCurItem->Visitor->Attribute(cnVar::MoveCast(Namespace),cnVar::MoveCast(Prefix),cnVar::MoveCast(NodeName),cnVar::MoveCast(Text));
		}
		break;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cXMLElement::Visit(iXMLVisitor *Visitor)noexcept(true)
{
	auto TagVisitor=Visitor->Element(Namespace,Prefix,Name);

	if(TagVisitor!=nullptr){
		for(auto &AttributePair : AttributeMap){
			auto Attribute=AttributePair.Value;
			TagVisitor->Attribute(Attribute->Namespace,Attribute->Prefix,Attribute->Name,Attribute->Value);
		}
		for(auto &Node : AllNodes){
			switch(Node.Type){
			case XMLNodeType::Text:
				TagVisitor->Text(Node.TextType,Node.Text);
				break;
			case XMLNodeType::Comment:
				TagVisitor->Comment(Node.TextType,Node.Text);
				break;
			case XMLNodeType::Element:
				Node.Element->Visit(TagVisitor);
				break;
			}
		}

		TagVisitor->Close();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cXMLElementMaker::cXMLElementMaker()noexcept(true)
	: fElement(aClsCreate<cXMLElement>())
	, fParentMaker(nullptr)
	, fTagFinished(false)
{
}
//---------------------------------------------------------------------------
cXMLElementMaker::cXMLElementMaker(cXMLElementMaker *ParentMaker)noexcept(true)
	: fElement(aClsCreate<cXMLElement>())
	, fParentMaker(ParentMaker)
	, fTagFinished(false)
{
}
//---------------------------------------------------------------------------
aClsRef<cXMLElement> cXMLElementMaker::Fetch(void)noexcept(true)
{
	return cnVar::MoveCast(fElement);
}
//---------------------------------------------------------------------------
void cXMLElementMaker::Close(void)noexcept(true)
{
	if(fParentMaker!=nullptr){
		fParentMaker->ChildClosed(this);
	}
}
//---------------------------------------------------------------------------
void cXMLElementMaker::ChildClosed(cXMLElementMaker *SubElementMaker)noexcept(true)
{
	auto SubElement=cnVar::MoveCast(SubElementMaker->fElement);
	delete SubElementMaker;

	auto *NewNode=fElement->AllNodes.Append();
	NewNode->Type=XMLNodeType::Element;
	NewNode->Element=SubElement;

	auto Pair=fElement->Children.InsertPair(SubElement->Name);
	Pair->Value.AppendMake(cnVar::MoveCast(SubElement));
}
//---------------------------------------------------------------------------
iXMLVisitor* cXMLElementMaker::Element(cString<uChar16> Namespace,cString<uChar16> Prefix,cString<uChar16> Name)noexcept(true)
{
	auto MakingElement=new cXMLElementMaker(this);
	MakingElement->fElement->Namespace=cnVar::MoveCast(Namespace);
	MakingElement->fElement->Prefix=cnVar::MoveCast(Prefix);
	MakingElement->fElement->Name=cnVar::MoveCast(Name);
	return MakingElement;
}
//---------------------------------------------------------------------------
void cXMLElementMaker::TagFinish(void)noexcept(true)
{
	fTagFinished=true;
}
//---------------------------------------------------------------------------
void cXMLElementMaker::TagNamespace(cString<uChar16> Namespace)noexcept(true)
{
	fElement->Namespace=cnVar::MoveCast(Namespace);
}
//---------------------------------------------------------------------------
void cXMLElementMaker::Attribute(cString<uChar16> Namespace,cString<uChar16> Prefix,cString<uChar16> Name,cString<uChar16> Value)noexcept(true)
{
	if(fTagFinished)
		return;	// ignore attribute in close tag

	auto Attribute=aClsCreate<cXMLAttribute>();
	Attribute->Namespace=cnVar::MoveCast(Namespace);
	Attribute->Prefix=cnVar::MoveCast(Prefix);
	Attribute->Name=cnVar::MoveCast(Name);
	Attribute->Value=cnVar::MoveCast(Value);

	fElement->AttributeMap.Insert(Name,cnVar::MoveCast(Attribute));
}
//---------------------------------------------------------------------------
void cXMLElementMaker::Text(eXMLTextStatementType TextType,cString<uChar16> Text)noexcept(true)
{
	auto *NewNode=fElement->AllNodes.Append();
	NewNode->Type=XMLNodeType::Text;
	NewNode->TextType=TextType;
	NewNode->Text=Text;

	fElement->Texts.AppendMake(Text);
}
//---------------------------------------------------------------------------
void cXMLElementMaker::Comment(eXMLTextStatementType TextType,cString<uChar16> Text)noexcept(true)
{
	auto *NewNode=fElement->AllNodes.Append();
	NewNode->Type=XMLNodeType::Comment;
	NewNode->TextType=TextType;
	NewNode->Text=Text;

	fElement->Comments.AppendMake(Text);
}
//---------------------------------------------------------------------------
