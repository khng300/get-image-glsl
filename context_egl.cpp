#include "context_egl.h"
#include <iostream>

/*---------------------------------------------------------------------------*/

static EGLDisplay displayForDevice(EGLDeviceEXT device)
{
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayExt = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
    EGLint attribs[] = { EGL_NONE };
    EGLDisplay display = eglGetPlatformDisplayExt(EGL_PLATFORM_DEVICE_EXT, device, attribs);
    return display;
}

static EGLDisplay getDevice()
{
    PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT = reinterpret_cast<PFNEGLQUERYDEVICESEXTPROC>(eglGetProcAddress("eglQueryDevicesEXT"));
    EGLDeviceEXT devices[32];
    EGLint num_devices;
    if (!eglQueryDevicesEXT(32, devices, &num_devices)) {
	    std::cout << "Failed to query devices." << std::endl << std::endl;
        return EGL_NO_DISPLAY;
    }
    if (num_devices == 0) {
	    std::cout << "Found no devices." << std::endl << std::endl;
        return EGL_NO_DISPLAY;
    }

    EGLDisplay display = displayForDevice(devices[0]);
    if (display == EGL_NO_DISPLAY) {
	    std::cout << "  No attached display." << std::endl;
    }

    return display;
}

void contextInitAndGetAPI(Params& params, Context& ctx) {

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

    display = getDevice();

    EGLint major;
    EGLint minor;

    if(eglInitialize(display, &major, &minor) == EGL_FALSE) {
        crash("%s", "eglInitialize failed.");
    }

    params.APIVersion = ((int)major * 100) + ((int) minor * 10);
    params.API = API_OPENGL_ES;

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

bool contextKeepLooping(Context &ctx) {
    return true;
}

/*---------------------------------------------------------------------------*/

void contextSwap(Context& ctx) {
    eglSwapBuffers(ctx.display, ctx.surface);
}


/*---------------------------------------------------------------------------*/

void contextSetKeyCallback(Context& ctx) {
    printf("Warning: no key callback support with EGL!\n");
}

/*---------------------------------------------------------------------------*/

void contextTerminate(Context& ctx) {
    eglTerminate(ctx.display);
}

/*---------------------------------------------------------------------------*/
