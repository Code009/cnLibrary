#ifndef	__cnLibrary_cnSystem_OpenGL_H__
#define	__cnLibrary_cnSystem_OpenGL_H__

#include <TargetConditionals.h>


#if	TARGET_OS_MAC


#if TARGET_OS_IPHONE
	// iOS
#include <OpenGLES/gltypes.h>

#else
	// MacOS
#include <OpenGL/gl.h>

#endif


#endif

#include <cnSystem/osOpenGL.h>

#endif
