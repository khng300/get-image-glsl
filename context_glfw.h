#ifndef __GETIMAGE_GLFW__
#define __GETIMAGE_GLFW__

#include "common.h" // Params

// glad is a OpenGL loader recommended by GLFW, see GLFW documentation
#include "glad/glad.h"
#include <GLFW/glfw3.h>

typedef struct {
    GLFWwindow* window;
} Context;

#endif
