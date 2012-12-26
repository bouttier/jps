#ifndef _PRIORITYQUEUE_H_
#define _PRIORITYQUEUE_H_

typedef struct item {
	double priority;
	int value;
} item;

typedef struct queue {
	int size;
	unsigned int allocated;
	item *root;
	int *index;
	unsigned int indexAllocated;
} pqueue_t;

int pqueue_create(pqueue_t ** q, int capacity);
int pqueue_insert(pqueue_t *q, int value, double priority);

int pqueue_exists(const pqueue_t *q, int ind);
void pqueue_delete(pqueue_t *q, int ind);

int pqueue_priority_of(const pqueue_t *q, int ind);
void pqueue_change_priority(pqueue_t *q, int ind, double newPriority);
item pqueue_get_min(pqueue_t *q);

void pqueue_destroy(pqueue_t ** q);

#endif /* _PRIORITYQUEUE_H */
