/**
 * @file queue.h
 * Queue
 *
 * Define queue record structure and all forward declarations.
 *
 * @author Jeffrey Hunter (http://www.iDevelopment.info)
 * @author Enrico Hartung <enrico@iptel.org>
 */

/**
 * @defgroup utils Utilities
 * Utility collection
 * @}
 */

/**
 * @defgroup queue Queue
 * @ingroup utils
 * @{
 */

#include <stdio.h>
#include <stdlib.h>

#ifndef QUEUE_DATATYPE
#include "queue_datatype_int.h"
#endif	/*
 */
#ifdef QUEUE_DATATYPE
#include QUEUE_DATATYPE_H
#endif	/*
 */

#define Error(Str)        FatalError(Str)
#define FatalError(Str)   fprintf(stderr, "%s\n", Str), exit(1)

#ifndef _Queue_h
#define _Queue_h

struct QueueRecord;

typedef struct QueueRecord *Queue;


int IsEmpty(Queue Q);

int IsFull(Queue Q);

Queue CreateQueue(int MaxElements);

void DisposeQueue(Queue Q);

void MakeEmpty(Queue Q);

void Enqueue(ElementType X, Queue Q);

ElementType Front(Queue Q);

void Dequeue(Queue Q);

ElementType FrontAndDequeue(Queue Q);

int IsElement(ElementType X, Queue Q);


#endif	/* _Queue_h */

/**
 * @}
 */
