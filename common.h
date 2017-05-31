#ifndef __GETIMAGE_COMMON__
#define __GETIMAGE_COMMON__

#include <stdlib.h> // exit()
#include <stdio.h>  // printf()
#include <string>

/*---------------------------------------------------------------------------*/

typedef struct {
    int width;
    int height;
    int version;
    std::string fragFilename;
    std::string output;
} Params;

/*---------------------------------------------------------------------------*/

#define crash(fmt, ...) do {                                            \
        printf("%s:%d (%s) ERROR: ", __FILE__, __LINE__, __func__);     \
        printf(fmt, ##__VA_ARGS__);                                     \
        printf("\n");                                                   \
        exit (EXIT_FAILURE);                                            \
    } while (0)

/*---------------------------------------------------------------------------*/

#endif
