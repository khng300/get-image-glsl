#ifndef __GETIMAGE_OPENGLCONTEXT__
#define __GETIMAGE_OPENGLCONTEXT__

#define CONTEXT_EGL  1
#define CONTEXT_GLFW 2

#if   (GETIMAGE_CONTEXT == CONTEXT_EGL)
#include "context_egl.h"
#elif (GETIMAGE_CONTEXT == CONTEXT_GLFW)
#include "context_glfw.h"
#else
#error Must define a context preprocessor macro!
#endif

#endif
