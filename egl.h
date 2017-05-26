#ifndef __GETIMAGE_EGL__
#define __GETIMAGE_EGL__

#include "EGL/egl.h"

void egl_init(int width, int height, EGLDisplay& display, EGLConfig& config, EGLContext& context, EGLSurface& surface);

#endif
