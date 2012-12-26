#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "libjps_private.h"

#include "priority_queue.h"

#define VERBOSE 0

// Quelle distance approximative doit être utilisé ?
#define CHEBYSHEV
//#define ENCLIDEAN

typedef unsigned char direction;
typedef unsigned char directionset;
#define NO_DIRECTION        8
#define EMPTY_DIRECTIONSET  0
#define FULL_DIRECTIONSET   255

typedef int node_t;

static node_t get_node(map_t * m, coord_t coord);
static coord_t get_coord(map_t * m, node_t node);
static double distance_estimate(coord_t start, coord_t end);
static double distance_precise(coord_t start, coord_t end);
static directionset natural_neighdirs(direction dir);
static directionset forced_neighdirs(map_t * m, coord_t pos, direction dir);
static coord_t get_neight(coord_t c, direction dir);
static int direction_is_diagonal(direction dir);
static void directionset_add(directionset * dirs, direction dir);
static direction directionset_next(directionset * dirs);
static node_t jump(map_t * m, node_t end, coord_t pos, direction dir);
static direction move_direction(map_t * m, node_t start, node_t end);

int jps_compute(map_t * m, coord_t _start, coord_t _end, solution * sol)
{
#if VERBOSE > 0
    printf("Compute path from (%d, %d) to (%d, %d) on a %dx%d map\n",
            _start.x, _start.y, _end.x, _end.y, m->width, m->height);
#endif
    int start = get_node(m, _start);
    int end = get_node(m, _end);
    int size = m->width * m->height;
    char * closed = calloc(size, sizeof(*closed)); // FIXME
    double *  scores = malloc(size * sizeof(*scores)); // FIXME
    memset(scores, -1, size);
    scores[start] = 0;
    int * cameFrom = malloc(size * sizeof(*cameFrom)); // FIXME
    //memset(cameFrom, -1, size);
    cameFrom[start] = start;

    pqueue_t  * pq;
    pqueue_create(&pq, 10); // FIXME
    pqueue_insert(pq, start, distance_estimate(_start, _end));

    node_t node;
    coord_t coord;
    item i;

    while (pq->size) {
        i = pqueue_get_min(pq);
        node = i.value;
        coord = get_coord(m, node);
#if VERBOSE > 1
        printf("Explore node %d (%d, %d)\n", node, coord.x, coord.y);
#endif
        if (node == end) {
#if VERBOSE > 1
            printf("End !\n");
#endif
            break;
        }
        closed[node] = 1;
        direction from = move_direction(m, cameFrom[node], node);
        directionset natural = natural_neighdirs(from);
        directionset forced = forced_neighdirs(m, coord, from);
        directionset dirs = natural | forced;
#if VERBOSE > 3
        printf("Came from direction : %d\n", from);
        printf("Natural neighbours : %d\n", natural);
        printf("Forced neighbours : %d\n", forced);
#endif
        direction dir;
        for (dir = directionset_next(&dirs) ; dir != NO_DIRECTION ;
                dir = directionset_next(&dirs)) {
            node_t newNode = jump(m, end, coord, dir);
            coord_t newCoord = get_coord(m, newNode);
#if VERBOSE > 2
            printf("Explore direction %d, node %d (%d, %d)\n",
                    dir, newNode, newCoord.x, newCoord.y);
#endif
            if (!map_contains(m, newCoord)) {
#if VERBOSE > 3
                printf("Out of map\n");
#endif
                continue;
            }
            if (closed[newNode]) {
#if VERBOSE > 3
                printf("Already explored\n");
#endif
                continue;
            }
            if (!pqueue_exists(pq, newNode)) {
                cameFrom[newNode] = node;
                scores[newNode] = scores[node] + distance_precise(coord, newCoord);
                pqueue_insert(pq, newNode, scores[newNode]
                        + distance_estimate(newCoord, _end));
#if VERBOSE > 2
                printf("Added to queue\n");
#endif
            } else if (scores[newNode] > scores[node]
                    + distance_precise(coord, newCoord)) {
                cameFrom[newNode] = node;
                int oldScore = scores[newNode];
                scores[newNode] = scores[node] + distance_precise(coord, newCoord);
                double newPri = pqueue_priority_of(pq, newNode)
                    - oldScore
                    + scores[newNode];
                pqueue_change_priority(pq, newNode, newPri);
#if VERBOSE > 3
                printf("Màj prio\n");
#endif
            }
        }
    }
    if (node != end) {
#if VERBOSE > 1
        printf("Ended search because of blocked path\n");
#endif
        return 0;
    }
    pqueue_destroy(&pq);

    *sol = malloc(size * sizeof(**sol));
    node_t pending = end;
    coord_t pendingCoord;
    int k = 0;
    while (pending != start) {
        pendingCoord = get_coord(m, pending);
       (*sol)[k++] = pendingCoord;
       pending = cameFrom[pending];
    }
    pendingCoord = get_coord(m, start);
    (*sol)[k++] = pendingCoord;
    *sol = realloc(*sol, k * sizeof(*sol));

    return k;
}

void jps_print(int length, solution sol)
{
    for (length--; length > -1; length--) {
        printf("(%d, %d)", sol[length].x, sol[length].y);
        if (length > 0) {
            printf(" -> ");
        }
    }
    printf("\n");
}

static direction move_direction(map_t * m, node_t start, node_t end)
{
    int w = m->width;
    if (start - w == end) {
        return 0;
    } else if (start - w + 1 == end) {
        return 1;
    } else if (start + 1 == end) {
        return 2;
    } else if (start + w + 1 == end) {
        return 3;
    } else if (start + w == end) {
        return 4;
    } else if (start + w - 1 == end) {
        return 5;
    } else if (start - 1 == end) {
        return 6;
    } else if (start - w - 1 == end) {
        return 7;
    } else {
        return NO_DIRECTION;
    }
}

static node_t jump(map_t * m, node_t end, coord_t pos, direction dir)
{
    coord_t step = get_neight(pos, dir);
    node_t stepnode = get_node(m, step);
    if (!map_walkable(m, step)) {
        return -1;
    }
    if (stepnode == end) {
        return stepnode;
    }
    if (forced_neighdirs(m, step, dir) != EMPTY_DIRECTIONSET) {
        return stepnode;
    }
    if (direction_is_diagonal(dir)) {
        int i;
        i = jump(m, end, step, (dir + 7) % 8);
        if (i > -1) {
            return stepnode;
        }
        i = jump(m, end, step, (dir + 1) % 8);
        if (i > -1) {
            return stepnode;
        }
    }
    return jump(m, end, step, dir);
}

static direction directionset_next(directionset * dirs)
{
    direction dir;
    for (dir = 0 ; dir < 8 ; dir++) {
        char dirBit = 1 << dir;
        if (*dirs & dirBit) {
            *dirs ^= dirBit;
            return dir;
        }
    }
    return NO_DIRECTION;
}

static directionset forced_neighdirs(map_t * m, coord_t pos, direction dir)
{
    if (dir == NO_DIRECTION) {
        return EMPTY_DIRECTIONSET;
    }
    directionset dirs = EMPTY_DIRECTIONSET;
#define WALKABLE(n) map_walkable(m, get_neight(pos, (dir + (n)) % 8))
    if (direction_is_diagonal(dir)) {
        if (WALKABLE(6) && !WALKABLE(5)) {
            directionset_add(&dirs, (dir + 6) % 8);
        } else if (WALKABLE(2) && !WALKABLE(3)) {
            directionset_add(&dirs, (dir + 2) % 8);
        }
    } else {
        if (WALKABLE(1) && !WALKABLE(2)) {
            directionset_add(&dirs, (dir + 1) % 8);
        } else if (WALKABLE(7) && !WALKABLE(6)) {
            directionset_add(&dirs, (dir + 7) % 8);
        }
    }
#undef WALKABLE
    return dirs;
}

static directionset natural_neighdirs(direction dir)
{
    directionset dirs = EMPTY_DIRECTIONSET;
    if (dir == NO_DIRECTION) {
        return FULL_DIRECTIONSET;
    }
    directionset_add(&dirs, dir);
    if (direction_is_diagonal(dir)) {
        directionset_add(&dirs, (dir + 1) % 8);
        directionset_add(&dirs, (dir + 7) % 8);
    }
    return dirs;
}

static coord_t get_neight(coord_t c, direction dir)
{
    switch (dir % 8) {
        case 0: return (coord_t) {c.x, c.y - 1};
        case 1: return (coord_t) {c.x + 1, c.y - 1};
        case 2: return (coord_t) {c.x + 1, c.y};
        case 3: return (coord_t) {c.x + 1, c.y + 1};
        case 4: return (coord_t) {c.x, c.y + 1};
        case 5: return (coord_t) {c.x - 1, c.y + 1};
        case 6: return (coord_t) {c.x - 1, c.y};
        case 7: return (coord_t) {c.x - 1, c.y - 1};
        default: return (coord_t) {-1, -1};
    }
}

static void directionset_add(directionset * dirs, direction dir)
{
    *dirs |= (1 << dir);
}

static int direction_is_diagonal(direction dir)
{
    return (dir % 2) != 0;
}

static double distance_precise(coord_t start, coord_t end)
{
    if (start.x - end.x != 0 && start.y - end.y != 0) {
        return sqrt(pow(start.x - end.x, 2) +
                pow(start.y - end.y, 2));
    } else {
        return abs(start.x - end.x) + abs(start.y - end.y);
    }
}

static double distance_estimate(coord_t start, coord_t end)
{
#if defined CHEBYSHEV
    return fmax(abs(start.x - end.x), abs(start.y - end.y));
#elif defined ENCLIDEAN
    return distance_precise(start, end);
#endif
}

static node_t get_node(map_t * m, coord_t coord)
{
    return coord.y * m->width + coord.x;
}

static coord_t get_coord(map_t * m, node_t node)
{
    coord_t coord;
    coord.x = node % m->width;
    coord.y = node / m->width;
    return coord;
}

void jps_free(solution * sol)
{
    free(*sol);
    *sol = NULL;
}
