#ifndef __GETIMAGE_COMMON__
#define __GETIMAGE_COMMON__

#include <stdlib.h> // exit()
#include <stdio.h>  // printf()
#include <string>
/*---------------------------------------------------------------------------*/

typedef enum {
    API_OPENGL,
    API_OPENGL_ES,
} API_TYPE;

typedef struct {
    int width;
    int height;
    int shaderVersion;
    int APIVersion;
    API_TYPE API;
    int delay;
    uint32_t program; // Is GLuint, but missing OpenGL headers here
    std::string fragFilename;
    std::string vertFilename;
    std::string output;
    bool exitCompile;
    bool exitLinking;
    bool persist;
    bool animate;
    std::string timeVarName;
    std::string binOut;
} Params;

/*---------------------------------------------------------------------------*/

#define errcode_crash(errcode, fmt, ...) do {                           \
        printf("%s:%d (%s) ERROR: ", __FILE__, __LINE__, __func__);     \
        printf(fmt, ##__VA_ARGS__);                                     \
        printf("\n");                                                   \
        exit (errcode);                                                 \
    } while (0)

/*---------------------------------------------------------------------------*/

// Hugues: we should just call errcode_crash(EXIT_FAILURE, fmt,
// __VA_ARGS__), but compiler complains.

#define crash(fmt, ...) do {                           \
        printf("%s:%d (%s) ERROR: ", __FILE__, __LINE__, __func__);     \
        printf(fmt, ##__VA_ARGS__);                                     \
        printf("\n");                                                   \
        exit (EXIT_FAILURE);                                            \
    } while (0)

/*---------------------------------------------------------------------------*/

#endif
