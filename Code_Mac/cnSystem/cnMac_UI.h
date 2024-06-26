/*--- Mac Interface -------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2017-05-29                                          */
/*-------------------------------------------------------------------------*/
#pragma once
#include <dispatch/dispatch.h>
#include <CoreGraphics/CoreGraphics.h>
#include <QuartzCore/QuartzCore.h>

#ifdef	__cplusplus

#include <cnSystem/cnSystem.h>
#include <cnSystem/cnMac_Sys.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class iCGContextDrawable;
template<> struct TInterfaceID<iCGContextDrawable>{	static iTypeID Value;	};
class cnLib_INTERFACE iCGContextDrawable : public iInterface
{
public:
	virtual void* CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual bool cnLib_FUNC DrawInContext(CGContextRef Context,const CGRect &Rect)=0;
};
//---------------------------------------------------------------------------
class iCFCGImage;
template<> struct TInterfaceID<iCFCGImage>{	static iTypeID Value;	};
class cnLib_INTERFACE iCFCGImage : public iCGContextDrawable
{
public:
	virtual void* CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface<iCGContextDrawable>(this,ID);	}

	virtual CGImageRef cnLib_FUNC GetCGImage(void)=0;
};
//---------------------------------------------------------------------------
#if	0
class cnLib_INTERFACE iQuartzPainter
{
public:
	virtual void PaintAttach(void)=0;
	virtual void PaintDetach(void)=0;
	virtual void cnLib_FUNC PaintRectChanged(void)=0;
	virtual void cnLib_FUNC Paint(CGContextRef Context)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iQuartzPaintTarget : public iInterface
{
public:
	virtual void cnLib_FUNC SetPainter(iQuartzPainter *Painter)=0;
	virtual void cnLib_FUNC InvalidateRect(const CGRect *Rect)=0;
	virtual void cnLib_FUNC GetPaintRect(CGRect &Rect)=0;
};
#endif
//---------------------------------------------------------------------------
//	Objective C
//---------------------------------------------------------------------------
#ifdef	__OBJC__
//---------------------------------------------------------------------------
#endif	// __OBJC__
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// __cplusplus

