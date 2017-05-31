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

/*---------------------------------------------------------------------------*/

#define GL_CHECKERR(strfunc) do {                       \
        GLenum __err = glGetError();                    \
        if (__err != GL_NO_ERROR) {                     \
            crash("OpenGL failure on: %s()" , strfunc); \
        }                                               \
    } while (0)

/*---------------------------------------------------------------------------*/

#define GL_SAFECALL(func, ...) do  {                    \
        func(__VA_ARGS__);                              \
        GL_CHECKERR(#func);                             \
    } while (0)

/*---------------------------------------------------------------------------*/

#endif
