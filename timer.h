#ifndef __GETIMAGE_TIMER__
#define __GETIMAGE_TIMER__

/*---------------------------------------------------------------------------*/

#ifdef _WIN32 // Windows

typedef struct timeval {
    long tv_sec;
    long tv_usec;
} timeval

#else // Unix platforms

#include <sys/time.h>

#endif

/*---------------------------------------------------------------------------*/

void startTimer(struct timeval *tv);

// Return time in microseconds
long stopTimer(struct timeval *start);

/*---------------------------------------------------------------------------*/

#endif // __GETIMAGE_TIMER__
