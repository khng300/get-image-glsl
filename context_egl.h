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

void context_init(const Params& params, Context& ctx);
void context_render(Context& ctx);
void context_terminate(Context& ctx);

#endif
