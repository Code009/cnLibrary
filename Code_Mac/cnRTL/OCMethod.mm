#include "OCMethod.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cOCMethodTypeMatching::Reset(Method m)
{
	TypeEncoding=method_getTypeEncoding(m);
}
//---------------------------------------------------------------------------
void cOCMethodTypeMatching::Advance(uIntn EncodingLength){
	TypeEncoding+=EncodingLength;
	while(*TypeEncoding>='0' && *TypeEncoding<='9'){
		TypeEncoding++;
	}
}
//---------------------------------------------------------------------------
