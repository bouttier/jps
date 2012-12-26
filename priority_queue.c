/**
 * Original source:
 *   https://github.com/arirahikkala/astar-jps/blob/master/IndexPriorityQueue.c
 * 
 * Copyright 2011 Ari Rahikkala. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *    1. Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 * 
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "priority_queue.h"

static int smallestPowerOfTwoAfter (int x)
{
        if (x < 0)
                return 0;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return x+1;
}

static int makeSpace (pqueue_t *q, size_t size)
{
        unsigned int newAllocated = smallestPowerOfTwoAfter (size * sizeof(item));

        if (newAllocated <= q->allocated)
                return 0;

        q->root = realloc (q->root, newAllocated);
        if (NULL == q->root)
                exit (1); // I'm a bloody Haskell programmer, I don't have any business trying to do anything fancy when realloc fails

        q->allocated = newAllocated;
        return newAllocated;
}

static int placeAtEnd (pqueue_t *q, item item)
{
	makeSpace (q, q->size + 1);
	q->root[q->size] = item;
	return q->size++;
}

static void siftUp (pqueue_t *q, int i)
{
	if (0 == i)
		return;

	int p = (i - 1) / 2;

	if (q->root[p].priority < q->root[i].priority)
		return;

	q->index[q->root[i].value] = p;
	q->index[q->root[p].value] = i;

	item swap = q->root[i];
	q->root[i] = q->root[p];
	q->root[p] = swap;

	return siftUp (q, p);
}

int pqueue_insert (pqueue_t *q, int value, double pri)
{
	item i;
	i.value = value;
	i.priority = pri;

        int newAllocated = smallestPowerOfTwoAfter ((value + 1) * sizeof(int));

        if ((value + 1) * sizeof (int) > q->indexAllocated) {
		q->index = realloc (q->index, newAllocated);
		if (NULL == q->index)
			exit (1);
		for (unsigned int j = q->indexAllocated / sizeof (int); j < newAllocated / sizeof (int); j++)
			q->index[j] = -1;
		q->indexAllocated = newAllocated;
	}

	int p = placeAtEnd (q, i);

	q->index[q->root[p].value] = p;	

	siftUp (q, p);

    return 0;
}

static void siftDown (pqueue_t *q, int i)
{
	int c = 1 + 2 * i;
	if (c >= q->size)
		return;

	if ((c + 1 < q->size) && (q->root[c].priority > q->root[(c+1)].priority))
		c++;

	if (q->root[i].priority < q->root[c].priority)
		return;

	q->index[q->root[c].value] = i;
	q->index[q->root[i].value] = c;

	item swap = q->root[i];
	q->root[i] = q->root[c];
	q->root[c] = swap;

	return siftDown (q, c);
}

static void deleteMin (pqueue_t *q)
{
	if (0 == q->size)
		return;

	q->index[q->root[0].value] = -1;
	q->size--;

	if (0 == q->size)
		return;

	q->index[q->root[q->size].value] = 0;
	q->root[0] = q->root[q->size];

	siftDown (q, 0);
} 

item pqueue_get_min(pqueue_t *q)
{
    item min = q->root[0];
    deleteMin(q);
	return min;
}

void pqueue_change_priority(pqueue_t *q, int ind, double newPriority)
{
	int oldPriority = q->root[q->index[ind]].priority;
	q->root[q->index[ind]].priority = newPriority;
	if (oldPriority < newPriority)
		siftDown (q, q->index[ind]);
	else if (oldPriority > newPriority)
		siftUp (q, q->index[ind]);
}

void pqueue_delete(pqueue_t *q, int ind)
{
	pqueue_change_priority (q, ind, INT_MIN);
	deleteMin (q);
}

int pqueue_priority_of(const pqueue_t *q, int ind)
{
	return q->root[q->index[ind]].priority;
}

int pqueue_exists(const pqueue_t *q, int ind)
{
	return  (q->indexAllocated / sizeof (int) > ind) &&
		(-1 != q->index[ind]) && 
		(q->size > q->index[ind]);
}

int pqueue_create(pqueue_t ** q, int capacity)
{
	pqueue_t *rv = (pqueue_t*) malloc (sizeof (pqueue_t));
	if (NULL == rv)
		exit (1);
	rv->size = 0;
	rv->allocated = 0;
	rv->root = NULL;
	rv->index = NULL;
	rv->indexAllocated = 0;
    *q = rv;
	return 0;
}

void pqueue_destroy (pqueue_t ** q)
{
	free ((*q)->root);
	free ((*q)->index);
    *q = NULL;
}

