#ifndef __GETIMAGE_GLFW__
#define __GETIMAGE_GLFW__

#include "common.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

typedef struct {
    GLFWwindow* window;
} AbstractGLFW;

void glfw_init(const Params& params, AbstractGLFW& abt);

void glfw_render(AbstractGLFW& abt);

#endif
