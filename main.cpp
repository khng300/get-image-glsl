// TODO: decide of a clang-format

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "common.h"
// TODO: rename egl.h to getimage_egl.h, etc, to avoid confusion (as EGL
// provides ELG/egl.h)
#include "egl.h"
#include "glfw.h"

#include "lodepng.h"

#include "GLES/gl.h"
#include "GLES2/gl2.h"

#define CHANNELS (4)

/*---------------------------------------------------------------------------*/

static void defaultParams(Params& params) {
    params.width = 256;
    params.height = 256;
    params.output = "output.png";
}

/*---------------------------------------------------------------------------*/

#define GL_SAFECALL(func, ...) do  {                    \
        func(__VA_ARGS__);                              \
        GLenum __err = glGetError();                    \
        if (__err != GL_NO_ERROR) {                     \
            crash("OpenGL failure on: %s()" , #func); \
        }                                               \
    } while (0)

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
        crash("cannot find end-of-line in fragment shader");
    }
    std::string sub = fragContents.substr(0, pos);
    if (std::string::npos == sub.find("#version")) {
        crash("cannot find ``#version'' in first line of fragment shader");
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

void savePNG(Params& params) {
    unsigned int uwidth = (unsigned int) params.width;
    unsigned int uheight = (unsigned int) params.height;
    std::vector<std::uint8_t> data(uwidth * uheight * CHANNELS);
    GL_SAFECALL(glReadPixels, 0, 0, uwidth, uheight, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
    std::vector<std::uint8_t> flipped_data(uwidth * uheight * CHANNELS);
    for (unsigned int h = 0; h < uheight ; h++)
        for (unsigned int col = 0; col < uwidth * CHANNELS; col++)
            flipped_data[h * uwidth * CHANNELS + col] =
                data[(uheight - h - 1) * uwidth * CHANNELS + col];
    unsigned png_error = lodepng::encode(params.output, flipped_data, uwidth, uheight);
    if (png_error) {
        crash("lodepng: %s", lodepng_error_text(png_error));
    }
}

/*---------------------------------------------------------------------------*/

int main(int argc, char* argv[])
{
    std::string fragFilename;
    std::string fragContents;
    AbstractEGL AbtEGL;
    AbstractGLFW AbtGLFW;

    Params params;
    defaultParams(params);

    // parse args
    for (int i = 1; i < argc; i++) {
        std::string arg = std::string(argv[i]);
        if (arg.compare(0, 2, "--") == 0) {
            crash("No -- option yet");
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
        crash("glCreateProgram()");
    }

    const char *temp;
    GLint status = 0;
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    temp = fragContents.c_str();
    GL_SAFECALL(glShaderSource, fragmentShader, 1, &temp, NULL);
    GL_SAFECALL(glCompileShader, fragmentShader);

    GL_SAFECALL(glGetShaderiv, fragmentShader, GL_COMPILE_STATUS, &status);
    if (!status) {
        printShaderError(fragmentShader);
        crash("Fragment shader compilation failed (%s)", fragFilename.c_str());
    }

    GL_SAFECALL(glAttachShader, program, fragmentShader);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GL_SAFECALL(glShaderSource, vertexShader, 1, &vtxstr, NULL);
    GL_SAFECALL(glCompileShader, vertexShader);
    GL_SAFECALL(glGetShaderiv, vertexShader, GL_COMPILE_STATUS, &status);
    if (!status) {
        printShaderError(vertexShader);
        crash("Vertex shader compilation failed");
    }

    GL_SAFECALL(glAttachShader, program, vertexShader);

    GL_SAFECALL(glLinkProgram, program);
    GL_SAFECALL(glGetProgramiv, program, GL_LINK_STATUS, &status);
    if (!status) {
        printProgramError(program);
        crash("glLinkProgram()");
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

    GL_SAFECALL(glViewport, 0, 0, params.width, params.height);

    GL_SAFECALL(glClearColor, 0.0f, 0.0f, 0.0f, 1.0f);
    GL_SAFECALL(glClear, GL_COLOR_BUFFER_BIT);

    GL_SAFECALL(glDrawElements, GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

    GL_SAFECALL(glFlush);

    switch (params.contextProvider) {
    case Ctx_GLFW:
        glfw_render(AbtGLFW);
        break;
    case Ctx_EGL:
        egl_render(AbtEGL);
        break;
    default:
        crash("Invalid context provider: %d", params.contextProvider);
    }

    savePNG(params);

    switch (params.contextProvider) {
    case Ctx_GLFW:
        glfw_terminate(AbtGLFW);
        break;
    case Ctx_EGL:
        egl_terminate(AbtEGL);
        break;
    default:
        crash("Invalid context provider: %d", params.contextProvider);
    }

    // ==============================

    exit(EXIT_SUCCESS);
}
