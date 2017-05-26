// TODO: decide of a clang-format

// Design choices: for any error, use the crash() macro. Do not hesitate
// to crash as soon as something goes wrong, rather than passing around
// return values.

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <stdio.h>

/*---------------------------------------------------------------------------*/

typedef enum {
    GLSLv100,
    GLSLv110,
    GLSLv300,
    GLSLv440,
} GLSLVersion;

/*---------------------------------------------------------------------------*/

#define crash(fmt, ...) do {                                            \
        printf("%s:%d (%s) ERROR: ", __FILE__, __LINE__, __func__);     \
        printf(fmt, __VA_ARGS__);                                       \
        printf("\n");                                                   \
        exit (EXIT_FAILURE);                                            \
    } while (0)

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

    exit(EXIT_SUCCESS);
}
