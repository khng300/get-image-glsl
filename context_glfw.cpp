#include "context_glfw.h"

static void errorCallback(int error, const char* description) {
    if (error == GLFW_VERSION_UNAVAILABLE) {
        // Silent this error, which can happen as we loop over versions
        // to find the highest one available.
        return;
    }
    crash("GLFW Error %d: %s", error, description);
}

/*---------------------------------------------------------------------------*/

void contextInitAndGetAPI(Params& params, Context& ctx) {

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        crash("%s", "glfwInit()");
    }

    // TODO: Must request GLFW_OPENGL_ANY_PROFILE for profiles below
    // 3.2, but we could request core / compat depending on the version.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    // TODO: here, try OpenGL ES API depending on the shader version?
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

    // Create window using the highest possible version
    int versions[] = {
        460, 450, 440, 430, 420, 410, 400,
        330, 320, 310, 300,
        210, 200,
    };

    GLFWwindow* window = NULL;

    for (unsigned i = 0; i < (sizeof(versions) / sizeof(versions[0])); i++) {
        int majorHint = versions[i] / 100;
        int minorHint = (versions[i] % 100) / 10;
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorHint);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorHint);
        window = glfwCreateWindow(params.width, params.height, "get_image_glfw", NULL, NULL);
        if (window != NULL) {
            break;
        }
    }
    if (window == NULL) {
        crash("%s", "glfwCreateWindow()");
    }

    params.API = API_OPENGL;
    int major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    int minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    params.APIVersion = ((int)major * 100) + ((int) minor * 10);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);
    ctx.window = window;
}

/*---------------------------------------------------------------------------*/

bool contextKeepLooping(Context &ctx) {
    return !glfwWindowShouldClose(ctx.window);
}

/*---------------------------------------------------------------------------*/

void contextSwap(Context& ctx) {
    glfwSwapBuffers(ctx.window);
    glfwPollEvents();
}

/*---------------------------------------------------------------------------*/

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    exit(EXIT_SUCCESS);
}

/*---------------------------------------------------------------------------*/

void contextSetKeyCallback(Context& ctx) {
    glfwSetKeyCallback(ctx.window, keyCallback);
}

/*---------------------------------------------------------------------------*/

void contextTerminate(Context& ctx) {
    glfwDestroyWindow(ctx.window);
    glfwTerminate();
}

/*---------------------------------------------------------------------------*/
