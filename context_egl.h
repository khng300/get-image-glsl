#ifndef __GETIMAGE_EGL__
#define __GETIMAGE_EGL__

#include <EGL/egl.h>
#include <EGL/eglext.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES/gl.h>
#include <GLES3/gl3.h>
#include "common.h"

typedef struct {
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface surface;
} Context;

#endif
