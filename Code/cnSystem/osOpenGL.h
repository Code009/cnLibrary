/*- cnSystem - opengl -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-12-18                                          */
/*-------------------------------------------------------------------------*/
#ifdef __cnLibrary_cnSystem_OpenGL_H__
#ifndef __cnLibrary_cnSystem_osOpenGL_H__
#define	__cnLibrary_cnSystem_osOpenGL_H__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnFundamental.h>
#include <cnSystem/cnUI.h>
#include <cnSystem/cnUIGraph.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iOpenGLContext : public iInterface
{
public:
	virtual bool cnLib_FUNC Enter(void)noexcept(true)=0;
	virtual void cnLib_FUNC Leave(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iOpenGLViewPainter : public iUIPainter
{
public:
	virtual void cnLib_FUNC PaintRectChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC NotifyRender(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iOpenGLViewContent : public iUIViewContent
{
public:
	virtual cUIPoint cnLib_FUNC GetPaintSize(void)noexcept(true)=0;
	virtual void cnLib_FUNC NotifyPresent(void)noexcept(true)=0;
	virtual iOpenGLContext* cnLib_FUNC GetOpenGLContext(void)noexcept(true)=0;
	virtual bool cnLib_FUNC GLBindRenderBuffer(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
#endif	/* __cnLibrary_cnSystem_OpenGL_H__ */
