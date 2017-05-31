#ifndef __GETIMAGE_EGL__
#define __GETIMAGE_EGL__

#include <EGL/egl.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include "common.h"

typedef struct {
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface surface;
} Context;

#endif
