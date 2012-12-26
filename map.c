#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdio.h>

#include "libjps_private.h"

#define cell(m, i, j)   ((m)->map[(j)*(m)->width+(i)])

#define min(x, y)   ((x<y)?x:y)
#define max(x, y)   ((x<y)?y:x)

/** Créer une carte aux dimensions spécifiées */
map_t * map_create(int width, int height)
{
    map_t * m;
    if ((m = malloc(sizeof(map_t))) == NULL) {
        perror("malloc");
        return NULL;
    }
    m->width = width;
    m->height = height;
    if ((m->map = calloc(width * height, sizeof(char))) == NULL) {
        perror("malloc");
        free(m);
        return NULL;
    }
    return m;
}

/** Copier une carte */
map_t * map_copy(map_t * src)
{
    map_t * dest;
    if ((dest = map_create(src->width, src->height)) == NULL) {
        return NULL;
    }
    memcpy(dest->map, src->map, src->width * src->height * sizeof(char));
    return dest;
}

/** Détruire une carte */
void map_destroy(map_t * m)
{
    free(m->map);
    free(m);
}

/** Ajouter ou supprimer un obstacle ponctuel */
static void map_set_point(map_t * m, int x, int y, char value)
{
    cell(m, x, y) = value;
}
void map_add_point(map_t * m, int x, int y)
{
    map_set_point(m, x, y, 1);
}
void map_remove_point(map_t * m, int x, int y)
{
    map_set_point(m, x, y, 0);
}

/** Ajouter ou supprimer un obstacle rectangulaire */
static void map_set_rectangle(map_t * m,
        int x, int y, int width, int height, char value)
{
    int i, j;
    for (i = x ; i < x + width ; i++) {
        for (j = y ; j < y + height ; j++) {
            cell(m, i, j) = value;
        }
    }
}
void map_add_rectangle(map_t * m, int x, int y, int width, int height)
{
    map_set_rectangle(m, x, y, width, height, 1);
}
void map_remove_rectangle(map_t * m, int x, int y, int width, int height)
{
    map_set_rectangle(m, x, y, width, height, 0);
}

/** Ajouter ou supprimer un obstacle circulaire */
static void map_set_circle(map_t * m,
        int x, int y, double radius, char value)
{
    int i, j;
    for (i = max(x-radius, 0) ; i < min(x+radius+1, m->width) ; i++) {
        for (j = max(y-radius, 0) ; j < min(y+radius+1, m->height) ; j++) {
            if (sqrt((x-i)*(x-i) + (y-j)*(y-j)) <= radius) {
                cell(m, i, j) = value;
            }
        }
    }
}
void map_add_circle(map_t * m, int x, int y, double radius)
{
    map_set_circle(m, x, y, radius, 1);
}
void map_remove_circle(map_t * m, int x, int y, double radius)
{
    map_set_circle(m, x, y, radius, 0);
}

/** Afficher une carte */
void map_print(map_t * m)
{
    int i, j;
    for (i = 0 ; i < m->height ; i++) {
        for (j = 0 ; j < m->width ; j++) {
            printf(cell(m, j, i)?"X":"·");
        }
        printf("\n");
    }
}

/** Savoir si des coordonnées sont intérieur à la map */
int map_contains(map_t * m, coord_t coord)
{
    if (coord.x > -1 && coord.x < m->width
            && coord.y > -1 && coord.y < m->height) {
        return 1;
    } else {
        return 0;
    }
}

/** Savoir si une case peut être parcouru */
int map_walkable(map_t * m, coord_t coord)
{
    if (map_contains(m, coord)) {
        return !cell(m, coord.x, coord.y);
    } else {
        return 0;
    }
}
