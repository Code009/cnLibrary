#include "OCType.h"
#include <Foundation/Foundation.h>

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
void cnRTL::ocPtrAcquire(id __unsafe_unretained Object)
{
	[Object retain];
}
void cnRTL::ocPtrRelease(id __unsafe_unretained Object)
{
	[Object release];
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
NSString* cnRTL::NSStringFromCStr(const uChar16 *String,uIntn MaxLength)
{
	if(String==nullptr)
		return @"";
	auto Length=cnString::GetLength(String,MaxLength);
	if(Length==0)
		return @"";

	return [[NSString alloc]initWithCharacters:reinterpret_cast<const unichar*>(String) length:Length];
}
//---------------------------------------------------------------------------
uIntn cnRTL::NSStringAddTo(cStringBuffer<uChar16> &Buffer,NSString *str)
{
	uIntn len=str.length;
	Buffer.SetLength(len);
	[str getCharacters:reinterpret_cast<unichar*>(Buffer.GetString())];
	return len;
}
//---------------------------------------------------------------------------
cString<uChar16> cnRTL::NSStringToCStr(NSString *str)
{
	cStringBuffer<uChar16> Buffer;
	NSStringAddTo(Buffer,str);
	return cnVar::MoveCast(Buffer);
}
//---------------------------------------------------------------------------
