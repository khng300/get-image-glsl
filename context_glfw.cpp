#include "context_glfw.h"

static void errorCallback(int error, const char* description) {
    crash("GLFW Error %d: %s", error, description);
}

/*---------------------------------------------------------------------------*/

void context_init(const Params& params, Context& ctx) {

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        crash("%s", "glfwInit()");
    }

    // TODO: here, try OpenGL ES API
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

    switch (params.version) {
    case GLSLv110:
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        break;
    case GLSLv440:
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
        break;
    default:
        crash("Invalid GLSL version: %d", params.version);
    }

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

void context_render(Context& ctx) {
    glfwSwapBuffers(ctx.window);
    glfwPollEvents();
}

/*---------------------------------------------------------------------------*/

void context_terminate(Context& ctx) {
    glfwDestroyWindow(ctx.window);
    glfwTerminate();
}

/*---------------------------------------------------------------------------*/
