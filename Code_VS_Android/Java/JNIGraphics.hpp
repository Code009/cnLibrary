/*- Java - Android - Graphics ---------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-04-25                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <Java/JNIJavaBase.hpp>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace jCPP{
//---------------------------------------------------------------------------
namespace android{
//---------------------------------------------------------------------------
namespace graphics{
//---------------------------------------------------------------------------
struct jcRect : jcObject
{
	static constexpr const char jClassName[]="android/graphics/Rect";
	
	static constexpr const char jname_left[]="left";
	jFieldAccess<jint> left(JNIEnv *env)noexcept{
		return jFieldAccessMaker<jname_left>(env,this);
	}

	static constexpr const char jname_right[]="right";
	jFieldAccess<jint> right(JNIEnv *env)noexcept{
		return jFieldAccessMaker<jname_right>(env,this);
	}
	
	static constexpr const char jname_top[]="top";
	jFieldAccess<jint> top(JNIEnv *env)noexcept{
		return jFieldAccessMaker<jname_top>(env,this);
	}

	static constexpr const char jname_bottom[]="bottom";
	jFieldAccess<jint> bottom(JNIEnv *env)noexcept{
		return jFieldAccessMaker<jname_bottom>(env,this);
	}
	

	static constexpr const char jname_width[]="width";
	jint width(JNIEnv *env)noexcept{
		return jMethodCall<jname_width,&jcRect::width>::Call(env,this);
	}

	static constexpr const char jname_height[]="height";
	jint height(JNIEnv *env)noexcept{
		return jMethodCall<jname_height,&jcRect::height>::Call(env,this);
	}
};
//---------------------------------------------------------------------------
struct jcBitmapConfig : jcObject
{
	static constexpr const char jClassName[]="android/graphics/Bitmap$Config";
	
	static constexpr const char jname_ALPHA_8[]="ALPHA_8";
	static jStaticFieldAccess<jcBitmapConfig*> ALPHA_8(JNIEnv *env)noexcept{
		return jStaticFieldAccessMaker<jcBitmapConfig,jname_ALPHA_8>(env);
	}
};
//---------------------------------------------------------------------------
struct jcBitmap : jcObject
{
	static constexpr const char jClassName[]="android/graphics/Bitmap";

	
	static constexpr const char jname_createBitmap[]="createBitmap";
	static jrLocal<jcBitmap> createBitmap(JNIEnv *env,jint w, jint h, jcBitmapConfig *Config)noexcept{
		return jStaticMethodCall<jcBitmap,jname_createBitmap,&createBitmap>::Call(env,w,h,Config);
	}

	static constexpr const char jname_getDensity[]="getDensity";
	jint getDensity(JNIEnv *env)noexcept{
		return jMethodCall<jname_getDensity,&jcBitmap::getDensity>::Call(env,this);
	}
};
//---------------------------------------------------------------------------
struct jcPaintAlign : jcObject
{
	static constexpr const char jClassName[]="android/graphics/Paint$Align";
	
	
	static constexpr const char jname_CENTER[]="CENTER";
	static jStaticFieldAccess<jcPaintAlign> CENTER(JNIEnv *env)noexcept{
		return jStaticFieldAccessMaker<jcPaintAlign,jname_CENTER>(env);
	}
};
//---------------------------------------------------------------------------
struct jcPaint : jcObject
{
	static constexpr const char jClassName[]="android/graphics/Paint";

	static constexpr jint ANTI_ALIAS_FLAG=1;

	static jrLocal<jcPaint> Make(JNIEnv *env,jint flag)noexcept{
		return jNew<jcPaint>(env,flag);
	}
	
	static constexpr const char jname_setColor[]="setColor";
	void setColor(JNIEnv *env,jint color)noexcept{
		return jMethodCall<jname_setColor,&jcPaint::setColor>::Call(env,this,color);
	}

	static constexpr const char jname_setTextSize[]="setTextSize";
	void setTextSize(JNIEnv *env,jfloat textSize)noexcept{
		return jMethodCall<jname_setTextSize,&jcPaint::setTextSize>::Call(env,this,textSize);
	}

	static constexpr const char jname_setShadowLayer[]="setShadowLayer";
	void setShadowLayer(JNIEnv *env,jfloat radius,jfloat dx,jfloat dy,jint shadowColor)noexcept{
		return jMethodCall<jname_setShadowLayer,&jcPaint::setShadowLayer>::Call(env,this,radius,dx,dy,shadowColor);
	}

	static constexpr const char jname_getTextBounds[]="getTextBounds";
	void getTextBounds(JNIEnv *env,jcString *text, jint start, jint end, jcRect *bounds)noexcept{
		return jMethodCall<jname_getTextBounds,&jcPaint::getTextBounds>::Call(env,this,text,start,end,bounds);
	}

	static constexpr const char jname_setTextAlign[]="setTextAlign";
	void setTextAlign(JNIEnv *env,jcPaintAlign *align)noexcept{
		return jMethodCall<jname_setTextAlign,&jcPaint::setTextAlign>::Call(env,this,align);
	}
};
//---------------------------------------------------------------------------
struct jcCanvas : jcObject
{
	static constexpr const char jClassName[]="android/graphics/Canvas";
	
	static jrLocal<jcCanvas> Make(JNIEnv *env,jcBitmap *bitmap)noexcept{
		return jNew<jcCanvas>(env,bitmap);
	}
	
	static constexpr const char jname_drawText[]="drawText";
	void drawText(JNIEnv *env,jcString *text,jfloat x,jfloat y,jcPaint *Paint)noexcept{
		return jMethodCall<jname_drawText,&jcCanvas::drawText>::Call(env,this,text,x,y,Paint);
	}

	static constexpr const char jname_getDensity[]="getDensity";
	jint getDensity(JNIEnv *env)noexcept{
		return jMethodCall<jname_getDensity,&jcCanvas::getDensity>::Call(env,this);
	}
};
//---------------------------------------------------------------------------
}	// namespace graphics
//---------------------------------------------------------------------------
}	// namespace android
//---------------------------------------------------------------------------
}	// namespace jCPP
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
