// TODO: decide of a clang-format

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "common.h"
// TODO: rename egl.h to getimage_egl.h, etc, to avoid confusion (as EGL
// provides ELG/egl.h)
#include "egl.h"
#include "glfw.h"

#include "GLES/gl.h"
#include "GLES2/gl2.h"

// ==================================================
int checkForGLError(const char loc[]) {
  GLenum res = glGetError();
  if(res != GL_NO_ERROR) {
    std::cerr << loc << ": glGetError: " << std::hex << res << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

#define CHECK_ERROR(loc) \
do { \
  if(checkForGLError(loc) == EXIT_FAILURE) { \
    return EXIT_FAILURE; \
  } \
} while(false)
// ==================================================


/*---------------------------------------------------------------------------*/

const Params DEFAULT_PARAMS = {
width: 256,
height: 256
};

/*---------------------------------------------------------------------------*/

const float vertices[] = {
  -1.0f,  1.0f,
  -1.0f, -1.0f,
   1.0f, -1.0f,
   1.0f,  1.0f
};

const GLubyte indices[] = {
  0, 1, 2,
  2, 3, 0
};

/*---------------------------------------------------------------------------*/

void readFile(std::string& contents, const std::string& filename) {
    std::ifstream ifs(filename.c_str());
    if(!ifs) {
        crash("File not found: %s", filename.c_str());
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    contents = ss.str();
}

/*---------------------------------------------------------------------------*/

void printShaderError(GLuint shader) {
    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    // The length includes the NULL character
    std::vector<GLchar> errorLog((size_t) length, 0);
    glGetShaderInfoLog(shader, length, &length, &errorLog[0]);
    if(length > 0) {
        std::string s(&errorLog[0]);
        std::cout << s << std::endl;
    }
}

/*---------------------------------------------------------------------------*/

void printProgramError(GLuint program) {
    GLint length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    // The length includes the NULL character
    std::vector<GLchar> errorLog((size_t) length, 0);
    glGetProgramInfoLog(program, length, &length, &errorLog[0]);
    if(length > 0) {
        std::string s(&errorLog[0]);
        std::cout << s << std::endl;
    }
}

/*---------------------------------------------------------------------------*/

GLSLVersion getVersion(const std::string& fragContents) {
    size_t pos = fragContents.find('\n');
    if (pos == std::string::npos) {
        crash("%s", "cannot find end-of-line in fragment shader");
    }
    std::string sub = fragContents.substr(0, pos);
    if (std::string::npos == sub.find("#version")) {
        crash("%s", "cannot find ``#version'' in first line of fragment shader");
    }
    if (std::string::npos != sub.find("100")) { return GLSLv100; }
    if (std::string::npos != sub.find("110")) { return GLSLv110; }
    if (std::string::npos != sub.find("300")) { return GLSLv300es; }
    if (std::string::npos != sub.find("440")) { return GLSLv440; }
    crash("Cannot find a supported GLSL version in first line of fragment shader: ``%.80s''", sub.c_str());
}

/*---------------------------------------------------------------------------*/

const char* vtxstr =
"#version 110\n"
"attribute vec2 vert2d;\n"
"void main(void) {\n"
"  gl_Position = vec4(vert2d, 0.0, 1.0);\n"
"}\n";

/*---------------------------------------------------------------------------*/

int main(int argc, char* argv[])
{
    Params params = DEFAULT_PARAMS;
    std::string fragFilename;
    std::string fragContents;
    AbstractEGL AbtEGL;
    AbstractGLFW AbtGLFW;

    // parse args
    for (int i = 1; i < argc; i++) {
        std::string arg = std::string(argv[i]);
        if (arg.compare(0, 2, "--") == 0) {
            crash("%s", "No -- option yet");
        }
        if (fragFilename.length() == 0) {
            fragFilename = arg;
        } else {
            crash("Unexpected extra argument: %s", arg.c_str());
        }
    }

    readFile(fragContents, fragFilename);
    params.version = getVersion(fragContents);

    // Init context depepding on version
    switch(params.version) {

        // regular GLSL
    case GLSLv110:
    case GLSLv440:
        glfw_init(params, AbtGLFW);
        params.contextProvider = Ctx_GLFW;
        break;

        // GLSL ES
    case GLSLv100:
    case GLSLv300es:
        egl_init(params, AbtEGL);
        params.contextProvider = Ctx_EGL;
        break;

    default:
        // Should never happen
        crash("No GLSL version? (%d)", params.version);
    }

    // OpenGL part
    GLuint program = glCreateProgram();
    if (program == 0) {
        crash("%s", "glCreateProgram()");
    }

    const char *temp; // necessary for some OpenGL primitives
    GLint status = 0;
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    temp = fragContents.c_str();
    glShaderSource(fragmentShader, 1, &temp, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (!status) {
        printShaderError(fragmentShader);
        crash("Fragment shader compilation failed (%s)", fragFilename.c_str());
    }

    glAttachShader(program, fragmentShader);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vtxstr, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (!status) {
        printShaderError(vertexShader);
        crash("%s", "Vertex shader compilation failed");
    }

    glAttachShader(program, vertexShader);

    glLinkProgram(program);
    glGetProgramiv(vertexShader, GL_LINK_STATUS, &status);
    if (!status) {
        printProgramError(program);
        crash("%s", "glLinkProgram()");
    }

    // ==============================

    GLint posAttribLocationAttempt = glGetAttribLocation(program, "vert2d");
    if(posAttribLocationAttempt == -1) {
        std::cerr << "Error getting vert2d attribute location." << std::endl;
        return EXIT_FAILURE;
    }
    GLuint posAttribLocation = (GLuint) posAttribLocationAttempt;
    glEnableVertexAttribArray(posAttribLocation);

    glUseProgram(program);

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint indicesBuffer;
    glGenBuffers(1, &indicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(posAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);

    glViewport(0, 0, params.width, params.height);
    CHECK_ERROR("After glViewport");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    CHECK_ERROR("After glClearColor");
    glClear(GL_COLOR_BUFFER_BIT);
    CHECK_ERROR("After glClear");

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    CHECK_ERROR("After glDrawElements");

    glFlush();
    CHECK_ERROR("After glFlush");

    glfw_render(AbtGLFW);

    // ==============================

    exit(EXIT_SUCCESS);
}
