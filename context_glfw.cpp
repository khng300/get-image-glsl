#include "context_glfw.h"

static void errorCallback(int error, const char* description) {
    crash("GLFW Error %d: %s", error, description);
}

/*---------------------------------------------------------------------------*/

void contextInit(const Params& params, Context& ctx) {

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        crash("%s", "glfwInit()");
    }

    // TODO: Must request GLFW_OPENGL_ANY_PROFILE for profiles below
    // 3.2, but we could request core / compat depending on the version.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    // TODO: here, try OpenGL ES API depending on the version?
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

    // We use to ask for a specific OpenGL API version,
    int major = params.version / 100;
    int minor = (params.version % 100) / 10;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);

    GLFWwindow* window = glfwCreateWindow(params.width, params.height, "Get image", NULL, NULL);
    if (window == NULL) {
        crash("%s", "glfwCreateWindow()");
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);
    ctx.window = window;
}

/*---------------------------------------------------------------------------*/

void contextSwap(Context& ctx) {
    glfwSwapBuffers(ctx.window);
    glfwPollEvents();
}

/*---------------------------------------------------------------------------*/

void contextTerminate(Context& ctx) {
    glfwDestroyWindow(ctx.window);
    glfwTerminate();
}

/*---------------------------------------------------------------------------*/
