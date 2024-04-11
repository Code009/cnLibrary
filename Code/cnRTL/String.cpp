//---------------------------------------------------------------------------
#include "String.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnString;


#pragma message (cnLib_FILE_LINE ": TODO - format string - lower case of hex digits")
#pragma message (cnLib_FILE_LINE ": TODO - format string - conversion alternative form")
#pragma message (cnLib_FILE_LINE ": TODO - format string - zero padding")
#pragma message (cnLib_FILE_LINE ": TODO - format string - unsigned output from signed integer")
#pragma message (cnLib_FILE_LINE ": TODO - ConvertEncodingToStream - total written")

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ufInt8 cnRTL::UnicodeChar8To32(uChar32 &Dest,const uChar8 *Src,uIntn SrcLength)noexcept
{
	if(SrcLength==0)
		return 0;

	uChar32 c;
	ufInt8 b0=Src[0];
	if(b0<0x80){
		// single character
		Dest=static_cast<uChar32>(b0);
		return 1;
	}
//	bytes	first byte
//	1		0-7F
// 	Tail	80-BF
//	2		C0-DF
//	3		E0-EF
//	4		F0-F7
//
	if(b0<0xC0){
		// tailing bytes along
		Dest='?';
		return 1;
	}

	ufInt8 CharLen;
	if(b0<0xE0){
		// 2 bytes
		CharLen=2;
		c=b0-0xC0;
	}
	else if(b0<0xF0){
		// 3 bytes
		CharLen=3;
		c=b0-0xE0;
	}
	else if(b0<0xF8){
		// 4 bytes
		CharLen=4;
		c=b0-0xF0;
	}
	else{	// b0>=0xF8
		// error
		Dest='?';
		return 1;
	}
	if(CharLen>SrcLength){
		// not enough length
		return 0;
	}
	// process following bytes
	for(ufInt8 i=1;i<CharLen;i++){
		ufInt8 bFollow=static_cast<ufInt8>(Src[i]-0x80);
		if(bFollow>=0x40){
			// error following byte
			Dest='?';
			return i;
		}
		c<<=6;
		c|=bFollow;
	}
	Dest=c;
	return CharLen;
}
//---------------------------------------------------------------------------
ufInt8 cnRTL::UnicodeChar16To32(uChar32 &Dest,const uChar16 *Src,uIntn SrcLength)noexcept
{
	if(SrcLength==0)
		return 0;
	
	uChar16 SurrogateChar=Src[0]-0xD800;
	if(SurrogateChar>=0x400){
		// single character or tailing surrogate
		// output single character
		Dest=static_cast<uChar32>(Src[0]);
		return 1;
	}
	// heading surrogate
	if(SrcLength==1){
		// need following character
		return 0;
	}
	// check tailing surrogate
	uChar16 SurrogateTailChar=Src[1]-0xDC00;
	if(SurrogateTailChar>=0x400){
		// invalid tailing surroage
		// output single character
		Dest=static_cast<uChar32>(Src[0]);
		return 1;
	}
	uChar32 c;
	c=static_cast<uChar32>(SurrogateChar)<<10;
	c+=static_cast<uChar32>(SurrogateTailChar)|0x10000;
	Dest=c;
	return 2;
}
//---------------------------------------------------------------------------
ufInt8 cnRTL::UnicodeChar8From32(uChar8 *Dest,uIntn DestLength,uChar32 Src)noexcept
{
	if(DestLength==0){
		return 0;
	}
	uInt32 t=Src;
	if(t<0x80){
		// 1 byte
		Dest[0]=static_cast<uChar8>(t);
		return 1;
	}
	if(t<0x800){
		// 2 bytes
		if(DestLength<2){
			return 0;
		}
		Dest[0]=static_cast<uChar8>(0xC0| (t>>6));
		Dest[1]=static_cast<uChar8>(0x80| (t&0x3F));
		return 2;
	}
	if(t<0x10000){
		// 3 bytes
		if(DestLength<3){
			return 0;
		}
		Dest[0]=static_cast<uChar8>(0xE0| (t>>12));
		Dest[1]=static_cast<uChar8>(0x80| ((t>>6)&0x3F));
		Dest[2]=static_cast<uChar8>(0x80| (t&0x3F));
		return 3;
	}
	if(t<=0x10FFFF){
		// 4 bytes
		if(DestLength<4){
			return 0;
		}
		Dest[0]=static_cast<uChar8>(0xF0| (t>>18));
		Dest[1]=static_cast<uChar8>(0x80| ((t>>12)&0x3F));
		Dest[2]=static_cast<uChar8>(0x80| ((t>>6)&0x3F));
		Dest[3]=static_cast<uChar8>(0x80| (t&0x3F));
		return 4;
	}
	// error
	Dest[0]='?';
	return 1;
}
//---------------------------------------------------------------------------
ufInt8 cnRTL::UnicodeChar16From32(uChar16 *Dest,uIntn DestLength,uChar32 Src)noexcept
{
	if(DestLength==0)
		return 0;

	uInt32 t=static_cast<uInt32>(Src);
	if(t<=0xFFFF){
		Dest[0]=static_cast<uChar16>(t);
		return 1;
	}

	if(t>0x10FFFF){
		Dest[0]=u'?';
		return 1;
	}
	// t<=0x10FFFF
	if(DestLength<2)
		return 0;
	uChar16 c[2];
	t-=0x10000;
	c[0]=static_cast<uChar16>(0x3FF & (t>>10));
	c[0]|=0xD800;
	c[1]=static_cast<uChar16>(0x3FF & t);
	c[1]|=0xDC00;
	Dest[0]=c[0];
	Dest[1]=c[1];
	return 2;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cFormatStringTokenParameter::SetDefault(void)noexcept
{
	TokenType=FormatStringTokenType::Literal;
	SizeModifier=FormatStringArgumentSize::Normal;
	WidthArgumentIndex=ArgIndexDefault;
	PrecisionArgumentIndex=ArgIndexDefault;

	AlignLeft=false;
	PositiveSign=false;
	ReserveSignSpace=false;
	ConversionAlternativeForm=false;
	ConversionUpperCase=false;
	PadLeftZero=false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cFormatStringParseStateTransistion::Reset(void)noexcept
{
	fState=sStr;
	fArgumentIndex=0;
}
//---------------------------------------------------------------------------
const cFormatStringParseStateTransistion::eCharType cFormatStringParseStateTransistion::FormatOutput_CharMap[CharMapLen]={
//	0			1			2			3			4			5			6			7
//	8			9			A			B			C			D			E			F
	ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,	// 0x00 - 0x07
	ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,	// 0x08 - 0x0F
	ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,	// 0x10 - 0x17
	ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,	// 0x18 - 0x1F
//				!			"			#			$			%			&			'
	ctFlag,		ctEnd,		ctEnd,		ctFlag,		ctEnd,		ctFmt,		ctEnd,		ctEnd,	// 0x20 - 0x27
// 	(			)			*			+			,			-			.			/
	ctEnd,		ctEnd,		ctVarNum,	ctFlag,		ctEnd,		ctFlag,		ctPrec,		ctEnd,	// 0x28 - 0x2F
//	0			1			2			3			4			5			6			7
	ctZero,		ctNum,		ctNum,		ctNum,		ctNum,		ctNum,		ctNum,		ctNum,	// 0x30 - 0x37
//	8			9			:			;			<			=			>			?
	ctNum,		ctNum,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,	// 0x38 - 0x3F
//	@			A			B			C			D			E			F			G
	ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctType,		ctType,		ctType,	// 0x40 - 0x47
//	H			I			J			K			L			M			N			O
	ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctSize,		ctEnd,		ctEnd,		ctEnd,	// 0x48 - 0x4F
//	P			Q			R			S			T			U			V			W
	ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,	// 0x50 - 0x57
//	X			Y			Z			[			\			]			^			_
	ctType,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,		ctEnd,	// 0x58 - 0x5F
//	`			a			b			c			d			e			f			g
	ctEnd,		ctEnd,		ctEnd,		ctType,		ctType,		ctType,		ctType,		ctType,	// 0x60 - 0x67
//	h			i			j			k			l			m			n			o
	ctSize,		ctType,		ctSize,		ctEnd,		ctSize,		ctEnd,		ctType,		ctType,	// 0x68 - 0x6F
//	p			q			r			s			t			u			v			w
	ctType,		ctEnd,		ctEnd,		ctType,		ctSize,		ctType,		ctEnd,		ctEnd,	// 0x70 - 0x77
//	x			y			z			{			|			}			~
	ctType,		ctEnd,		ctSize,																// 0x78 - 0x7F
};
//---------------------------------------------------------------------------
auto cFormatStringParseStateTransistion::CharType(uIntn c)noexcept -> eCharType
{

	if(c>=CharMapLen)
        return ctEnd;
    return FormatOutput_CharMap[c];
}
//---------------------------------------------------------------------------
const cFormatStringParseStateTransistion::cStateTransitionItem cFormatStringParseStateTransistion::FormatStringAnalyseTransistion[sMax][ctMax]={
{	// sStr
	{sStr,		aNone},			// End
	{sFmt,		aFmtStart},		// Fmt
	{sStr,		aNone},			// Flag
	{sStr,		aNone},			// Num
	{sStr,		aNone},			// VarNum
	{sStr,		aNone},			// Prec
	{sStr,		aNone},			// Size
	{sStr,		aNone},			// Type
	{sStr,		aNone},			// Zero
},
{	// sFmt
	{sStr,		aFmtError},		// End
	{sStr,		aFmtChar},		// Fmt
	{sFlag,		aFlag},			// Flag
	{sWidth,	aSetWidth},		// Num
	{sWidth,	aVarWidth},		// VarNum
	{sPrec,		aSetPrec},		// Prec
	{sSize,		aSize},			// Size
	{sStr,		aFmtEnd},		// Type
	{sFlag,		aFlag},			// Zero
},
{	// sFlag
	{sStr,		aFmtError},		// End
	{sStr,		aFmtError},		// Fmt
	{sFlag,		aFlag},			// Flag
	{sWidth,	aSetWidth},		// Num
	{sWidth,	aVarWidth},		// VarNum
	{sPrec,		aSetPrec},		// Prec
	{sSize,		aSize},			// Size
	{sStr,		aFmtEnd},		// Type
	{sFlag,		aFlag},			// Zero
},
{	// sWidth
	{sStr,		aFmtError},		// End
	{sStr,		aFmtError},		// Fmt
	{sStr,		aFmtError},		// Flag
	{sWidth,	aWidth},		// Num
	{sWidth,	aVarWidth},		// VarNum
	{sPrec,		aSetPrec},		// Prec
	{sSize,		aSize},			// Size
	{sStr,		aFmtEnd},		// Type
	{sWidth,	aWidth},		// Zero
},
{	// sPrec
	{sStr,		aFmtError},		// End
	{sStr,		aFmtError},		// Fmt
	{sStr,		aFmtError},		// Flag
	{sPrec,		aPrec},			// Num
	{sPrec,		aVarPrec},		// VarNum
	{sStr,		aFmtEnd},		// Prec
	{sSize,		aSize},			// Size
	{sStr,		aFmtEnd},		// Type
	{sPrec,		aPrec},			// Zero
},
{	// sSize
	{sStr,		aFmtError},		// End
	{sStr,		aFmtError},		// Fmt
	{sStr,		aFmtError},		// Flag
	{sStr,		aFmtError},		// Num
	{sStr,		aFmtError},		// VarNum
	{sStr,		aFmtError},		// Prec
	{sSize,		aSize},			// Size
	{sStr,		aFmtEnd},		// Type
	{sStr,		aFmtError},		// Zero
}
};
//---------------------------------------------------------------------------
const cFormatStringParseStateAction cFormatStringParseStateTransistion::ActionNone={
	false,false
};
const cFormatStringParseStateAction cFormatStringParseStateTransistion::ActionOutput={
	true,false
};
const cFormatStringParseStateAction cFormatStringParseStateTransistion::ActionOutputInclude={
	true,true
};
//---------------------------------------------------------------------------
cFormatStringParseStateAction cFormatStringParseStateTransistion::Process(uIntn Char,cFormatStringTokenParameter &TokenParameter)noexcept
{
	auto Type=CharType(Char);
	auto &Trans=FormatStringAnalyseTransistion[fState][Type];
	fState=Trans.Next;

	switch(Trans.Action){
	default:
	case aNone:
		break;
	case aFmtChar:
		TokenParameter.TokenType=FormatStringTokenType::Escape;
		return ActionOutputInclude;
	case aFlag:
		switch(Char){
		case '-':
			TokenParameter.AlignLeft=true;
			break;
		case '+':
			TokenParameter.PositiveSign=true;
			break;
		case ' ':
			TokenParameter.ReserveSignSpace=true;
			break;
		case '0':
			TokenParameter.PadLeftZero=true;
			break;
		case '#':
			TokenParameter.ConversionAlternativeForm=true;
			break;
		}
		break;
	case aSetWidth:
		TokenParameter.WidthArgumentIndex=TokenParameter.ArgIndexLiteral;
        TokenParameter.Width=Char-'0';
		break;
	case aWidth:
		TokenParameter.Width*=10;
        TokenParameter.Width+=Char-'0';
		break;
	case aVarWidth:
		TokenParameter.WidthArgumentIndex=fArgumentIndex;
		fArgumentIndex++;
		break;
	case aSetPrec:
		TokenParameter.PrecisionArgumentIndex=TokenParameter.ArgIndexLiteral;
		TokenParameter.Precision=0;
		break;
	case aPrec:
        TokenParameter.Precision*=10;
        TokenParameter.Precision+=static_cast<uIntn>(Char-'0');
		break;
	case aVarPrec:
		TokenParameter.PrecisionArgumentIndex=fArgumentIndex;
		fArgumentIndex++;
		break;
	case aSize:
		switch(Char){
		case 'h':
			if(TokenParameter.SizeModifier==FormatStringArgumentSize::Half)
				TokenParameter.SizeModifier=FormatStringArgumentSize::Small;
			else
				TokenParameter.SizeModifier=FormatStringArgumentSize::Half;
			break;
		case 'l':
			if(TokenParameter.SizeModifier==FormatStringArgumentSize::Long)
				TokenParameter.SizeModifier=FormatStringArgumentSize::Long2;
			else
				TokenParameter.SizeModifier=FormatStringArgumentSize::Long;
			break;
		case 'L':
			TokenParameter.SizeModifier=FormatStringArgumentSize::Double;
			break;
		case 'z':
			TokenParameter.SizeModifier=FormatStringArgumentSize::NatureWord;
			break;
		case 'j':
			TokenParameter.SizeModifier=FormatStringArgumentSize::Maximum;
			break;
		case 't':
			TokenParameter.SizeModifier=FormatStringArgumentSize::Offset;
			break;
		}
		break;
	case aFmtStart:
		TokenParameter.SetDefault();
		return ActionOutput;
	case aFmtError:
		TokenParameter.TokenType=FormatStringTokenType::Literal;
		return ActionOutput;
	case aFmtEnd:
		switch(Char){
        case 'd': case 'i':	// decimal int
			TokenParameter.TokenType=FormatStringTokenType::SignedDecimal;
            break;
        case 'o':	// octex int
			TokenParameter.TokenType=FormatStringTokenType::UnsignedOctal;
            break;
        case 'u':	// unsigned int
			TokenParameter.TokenType=FormatStringTokenType::UnsignedDecimal;
            break;
        case 'x':	// hex int
			TokenParameter.TokenType=FormatStringTokenType::UnsignedHex;
			TokenParameter.ConversionUpperCase=false;
            break;
        case 'X':	// hex int
			TokenParameter.TokenType=FormatStringTokenType::UnsignedHex;
			TokenParameter.ConversionUpperCase=true;
            break;
		case 'f': case 'F':	// float - normal form
			TokenParameter.TokenType=FormatStringTokenType::FloatDecimal;
			break;
        case 'e': // float - scientific form
			TokenParameter.TokenType=FormatStringTokenType::ExponentDecimal;
			TokenParameter.ConversionUpperCase=false;
			break;
        case 'E':	// float - scientific form
			TokenParameter.TokenType=FormatStringTokenType::ExponentDecimal;
			TokenParameter.ConversionUpperCase=true;
			break;
        case 'g':	// float
			TokenParameter.TokenType=FormatStringTokenType::FloatCompact;
			TokenParameter.ConversionUpperCase=false;
			break;
        case  'G':	// float
			TokenParameter.TokenType=FormatStringTokenType::FloatCompact;
			TokenParameter.ConversionUpperCase=true;
			break;
        case 'a':
			TokenParameter.TokenType=FormatStringTokenType::FloatHex;
			TokenParameter.ConversionUpperCase=false;
			break;
		case 'A':
			TokenParameter.TokenType=FormatStringTokenType::FloatHex;
			TokenParameter.ConversionUpperCase=true;
			break;
        case 'c':	// character
			TokenParameter.TokenType=FormatStringTokenType::Character;
            break;
        case 's':	// string
			TokenParameter.TokenType=FormatStringTokenType::String;
            break;
		case 'p':	// address
			TokenParameter.TokenType=FormatStringTokenType::Pointer;
            break;
		case 'n':
			TokenParameter.TokenType=FormatStringTokenType::OutputLength;
			break;
        }
		TokenParameter.ValueArgumentIndex=fArgumentIndex;
		fArgumentIndex++;
		return ActionOutputInclude;
	}
	return ActionNone;
}
//---------------------------------------------------------------------------
void cFormatStringParseStateTransistion::ProcessEnd(cFormatStringTokenParameter &TokenParameter)noexcept
{
	TokenParameter.TokenType=FormatStringTokenType::Literal;
}
//---------------------------------------------------------------------------

const char StringStream::HexBytesWrite_HexTextMap[]=
{
	'0',	'1',	'2',	'3',	'4',	'5',	'6',	'7'	,
	'8',	'9',	'A',	'B',	'C',	'D',	'E',	'F'	,
};

//---------------------------------------------------------------------------

