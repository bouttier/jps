#include <stdio.h>
#include <stdlib.h>

#include "libjps.h"

int main(int argc, char *argv[])
{
    map_t * m = map_create(80, 20);
    map_add_rectangle(m, 2, 4, 2, 2);
    map_add_circle(m, 20, 10, 7.0);
    coord_t start = { 3, 3 };
    //coord_t end = { 6, 9 };
    coord_t end = { 25, 5 };
    //coord_t end = { 6, 3 };
    map_t * n = map_copy(m);
    map_add_rectangle(n, 50, 10, 13, 6);
    solution sol;
    int solLength = jps_compute(m, start, end, &sol);
    jps_print(solLength, sol);
    jps_free(&sol);
    map_print(m);
    //map_print(n);
    map_destroy(n);
    map_destroy(m);
    return 0;
}
