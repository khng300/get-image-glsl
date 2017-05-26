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
    GLSLv300es,
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

void readFile(const std::string& fileName, std::string& contentsOut) {
    std::ifstream ifs(fileName.c_str());
    if(!ifs) {
        crash("File not found: %s", fileName.c_str());
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    contentsOut = ss.str();
    return true;
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
    re

    exit(EXIT_SUCCESS);
}
