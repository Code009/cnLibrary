/*- Mac - Quartz ----------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-03-30                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnUI/UIControl.h>
#include <cnSystem/iOS.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLib{
namespace cnUI{
namespace Quartz{
//---------------------------------------------------------------------------
class cQuartzPainter : public iQuartzPainter
{
protected:
	iQuartzPaintTarget *fPaintTarget;
	CGRect fDrawRect;
public:
	cQuartzPainter();
	~cQuartzPainter();

	virtual void PaintAttach(void)override;
	virtual void PaintDetach(void)override;
	virtual void cnLib_Func PaintRectChanged(void)override;
};
//---------------------------------------------------------------------------
class cButtonPainter : public cQuartzPainter, public iUIControlPainter
{
protected:
	const cButton *fData;

public:
	cButtonPainter(const cButton *Data);
	~cButtonPainter();

	virtual void cnLib_Func DrawRect(CGContextRef Context,const CGRect &Rect)override;

	virtual void cnLib_Func UpdateData(void)override;
};
//---------------------------------------------------------------------------
class cCheckBoxPainter : public cQuartzPainter, public iUIControlPainter
{
protected:
	const cCheckBox *fData;

public:
	cCheckBoxPainter(const cCheckBox *Data);
	~cCheckBoxPainter();

	virtual void cnLib_Func DrawRect(CGContextRef Context,const CGRect &Rect)override;

	virtual void cnLib_Func UpdateData(void)override;
};
//---------------------------------------------------------------------------
class cRadioBoxPainter : public cQuartzPainter, public iUIControlPainter
{
protected:
	const cRadioBox *fData;

public:
	cRadioBoxPainter(const cRadioBox *Data);
	~cRadioBoxPainter();

	virtual void cnLib_Func DrawRect(CGContextRef Context,const CGRect &Rect)override;

	virtual void cnLib_Func UpdateData(void)override;
};
//---------------------------------------------------------------------------
class cTextEditPainter : public cQuartzPainter, public iTextEditPainter
{
	bool fCaretCreated;

	int* GetTextPositions(unsigned int &Length,int &Height);

	void OnUIStateChange(const CGRect &rc);
protected:
	const cTextEdit *fData;
public:
	cTextEditPainter(const cTextEdit *Data);
	~cTextEditPainter();

	virtual void cnLib_Func DrawRect(CGContextRef Context,const CGRect &Rect)override;
	virtual void cnLib_Func PaintRectChanged(void)override;

	virtual void cnLib_Func UpdateData(void)override;
	virtual sintn cnLib_Func CharHitTest(sintn x,sintn y)override;
};
//---------------------------------------------------------------------------
}	// namespace Quartz
}	// namespace UI
}	// namespace cnLib
//---------------------------------------------------------------------------
#endif  /* #ifdef __cplusplus */
/*-------------------------------------------------------------------------*/
