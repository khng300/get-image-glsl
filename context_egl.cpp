#include "context_egl.h"

/*---------------------------------------------------------------------------*/

void contextInit(const Params& params, Context& ctx) {

    EGLDisplay& display = ctx.display;
    EGLConfig& config = ctx.config;
    EGLContext& context = ctx.context;
    EGLSurface& surface = ctx.surface;

    const EGLint config_attribute_list[] =
        {
            //EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_RED_SIZE, 4,
            EGL_GREEN_SIZE, 4,
            EGL_BLUE_SIZE, 4,
            EGL_ALPHA_SIZE, 4,

            EGL_CONFORMANT, EGL_OPENGL_ES3_BIT,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
        };

    const EGLint context_attrib_list[] =
        {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
        };

    const EGLint pbuffer_attrib_list[] =
        {
            EGL_WIDTH, params.width,
            EGL_HEIGHT, params.height,
            EGL_TEXTURE_FORMAT,  EGL_NO_TEXTURE,
            EGL_TEXTURE_TARGET, EGL_NO_TEXTURE,
            EGL_LARGEST_PBUFFER, EGL_TRUE,
            EGL_NONE
        };

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint major;
    EGLint minor;

    if(eglInitialize(display, &major, &minor) == EGL_FALSE) {
        crash("%s", "eglInitialize failed.");
    }

    EGLint num_config;
    if(eglChooseConfig(display, config_attribute_list, &config, 1, &num_config) == EGL_FALSE) {
        crash("%s", "eglChooseConfig failed.");
    }

    if(num_config != 1) {
        crash("%s", "eglChooseConfig did not return 1 config.");
    }

    context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attrib_list);
    if(context == EGL_NO_CONTEXT) {
        crash("eglCreateContext failed: %x", eglGetError());
    }

    surface = eglCreatePbufferSurface(display, config, pbuffer_attrib_list);
    if(surface == EGL_NO_SURFACE) {
        crash("eglCreatePbufferSurface failed: %x", eglGetError());
    }

    eglMakeCurrent(display, surface, surface, context);
}

/*---------------------------------------------------------------------------*/

void contextSwap(Context& ctx) {
    eglSwapBuffers(ctx.display, ctx.surface);
}

/*---------------------------------------------------------------------------*/

void contextTerminate(Context& ctx) {
    eglTerminate(ctx.display);
}

/*---------------------------------------------------------------------------*/
