#include "glfw.h"

static void errorCallback(int error, const char* description) {
    crash("GLFW Error %d: %s", error, description);
}

/*---------------------------------------------------------------------------*/

void glfw_init(const Params& params, AbstractGLFW& abt) {

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

    abt.window = window;

    return;
}

/*---------------------------------------------------------------------------*/

void glfw_render(AbstractGLFW& abt) {
    glfwSwapBuffers(abt.window);
    glfwPollEvents();
}

/*---------------------------------------------------------------------------*/
