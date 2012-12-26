/******************************************************************************
 * JPS Library (Jump Point Search)
 * Recherche de chemin sur une carte en utilisant les algorithem A* (A star) et
 * JPS (Jump Point Search). L’algorithme JPS permet d’éviter de développer
 * certains nœuds inutilement, tous en garantissant que le chemin obtenu est de
 * coup minimal.
 * Sources :
 *      http://harablog.wordpress.com/2011/09/07/jump-point-search/
 *      http://grastien.net/ban/articles/hg-aaai11.pdf
 *      http://github.com/arirahikkala/astar-jps
 */
#ifndef _LIBJPS_H_
#define _LIBJPS_H_

/* Définition des types */
typedef struct map_t map_t;

typedef struct {
    int x;
    int y;
} coord_t;

typedef coord_t * solution;

/* Créer une carte vierge
 * Valeur de retour : -1 en cas d’erreur, 0 sinon */
map_t * map_create(int width, int height);

/* Copier une carte */
map_t * map_copy(map_t * src);

/* Détruire une carte */
void map_destroy(map_t * map);


/* Ajouter ou supprimer un obstacle rectangulaire */
void map_add_rectangle(map_t * map, int x, int y, int width, int height);
void map_remove_rectangle(map_t * map, int x, int y, int width, int height);
/* Ajouter ou supprimer un obstacle circulaire */
void map_add_circle(map_t * map, int x, int y, double radius);
void map_remove_circle(map_t * map, int x, int y, double radius);

/* Afficher une carte dans le terminal */
void map_print(map_t * map);

/* Lancer la recherche d’un chemin
 * Renvoit la longueur de la solution , -1 en cas d’échec
 * Attention : penser à appeler jps_free pour éviter les fuites de mémoire */
int jps_compute(map_t * map, coord_t start, coord_t end, solution * sol);
/* Afficher une solution dans le terminal */
void jps_print(int solLength, solution sol);
/* Détruire une solution */
void jps_free(solution * sol);

#endif
