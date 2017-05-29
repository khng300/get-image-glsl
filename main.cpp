// TODO: decide of a clang-format

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "common.h"
// TODO: rename egl.h to getimage_egl.h, etc, to avoid confusion (as EGL
// provides ELG/egl.h)
#include "egl.h"
#include "glfw.h"

#include "GLES/gl.h"
#include "GLES2/gl2.h"

/*---------------------------------------------------------------------------*/

const Params DEFAULT_PARAMS = {
width: 256,
height: 256
};

/*---------------------------------------------------------------------------*/

typedef enum {
    // GLSL (regular)
    GLSLv100,
    GLSLv110,
    GLSLv440,
    // GLSL ES
    GLSLv300,
} GLSLVersion;

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
    if (std::string::npos != sub.find("300")) { return GLSLv300; }
    if (std::string::npos != sub.find("440")) { return GLSLv440; }
    crash("Cannot find a supported GLSL version in first line of fragment shader: ``%.80s''", sub.c_str());
}

/*---------------------------------------------------------------------------*/

int main(int argc, char* argv[])
{
    Params params = DEFAULT_PARAMS;

    std::string fragFilename;

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

    std::string fragContents;
    readFile(fragContents, fragFilename);
    GLSLVersion version = getVersion(fragContents);

    AbstractEGL AbtEGL;

    switch(version) {

        // regular GLSL
    case GLSLv100:
    case GLSLv110:
    case GLSLv440:
        glfw_init(params.width, params.height);
        params.contextProvider = Ctx_GLFW;
        break;

        // GLSL ES
    case GLSLv300:
        egl_init(params.width, params.height, AbtEGL);
        params.contextProvider = Ctx_EGL;
        break;

    default:
        // Should never happen
        crash("No GLSL version? (%d)", version);
    }

    exit(EXIT_SUCCESS);
}
