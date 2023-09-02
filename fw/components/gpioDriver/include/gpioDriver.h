#ifndef _GPIODRIVER_H
#define _GPIODRIVER_H

#include <stdio.h>

/*typedef struct
{
    char*           pinName;
    uint32_t        pinx;
}gpioInfo_t;*/

typedef struct
{
    char*           pinName;
    uint32_t        pinx;
    int             dftLevel;
}gpioOutInfo_t;
#endif