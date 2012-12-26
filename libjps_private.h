#ifndef _LIBJPS_PRIVATE_H_
#define _LIBJPS_PRIVATE_H_

#include "libjps.h"

struct map_t {
    int width;
    int height;
    char * map;
};

/* Savoir si un point est dans la carte ou à l’extérieur */
int map_contains(map_t * map, coord_t point);

#endif
