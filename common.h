#ifndef __GETIMAGE_COMMON__
#define __GETIMAGE_COMMON__

#include <stdlib.h> // exit()
#include <stdio.h>  // printf()

/*---------------------------------------------------------------------------*/
// Parameters

typedef enum {
    Ctx_EGL,
    Ctx_GLFW,
} ContextProvider;

typedef struct {
    int width;
    int height;
    ContextProvider contextProvider;
} Params;

Params DEFAULT_PARAMS = {
width: 256,
height: 256
};

/*---------------------------------------------------------------------------*/

#define crash(fmt, ...) do {                                            \
        printf("%s:%d (%s) ERROR: ", __FILE__, __LINE__, __func__);     \
        printf(fmt, __VA_ARGS__);                                       \
        printf("\n");                                                   \
        exit (EXIT_FAILURE);                                            \
    } while (0)

/*---------------------------------------------------------------------------*/

#endif
