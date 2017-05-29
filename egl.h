#ifndef __GETIMAGE_EGL__
#define __GETIMAGE_EGL__

#include <EGL/egl.h>
#include "common.h"

// This type is named Abstract rather than Context since the EGL library
// already defines a EGLContext type.
typedef struct {
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface surface;
} AbstractEGL;

void egl_init(const Params& params, AbstractEGL& abt);

#endif
