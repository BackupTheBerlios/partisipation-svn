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

#define Error(Str)        FatalError(Str)
#define FatalError(Str)   fprintf(stderr, "%s\n", Str), exit(1)

#ifndef QUEUE_H
#define QUEUE_H

struct queue_record;

typedef struct queue_record *queue;

/**
 * Check whether a queue is empty.
 *
 * @param queue a queue
 */
int queue_is_empty(queue queue);

/**
 * Check whether a queue is full.
 *
 * @param queue a queue
 */
int queue_is_full(queue queue);

/**
 * Create a new queue. The length of the queue has a maximum which is set
 * by the parameter of this method.
 *
 * @param maxElements maximum count of elements which can be included into the queue
 */
queue queue_create_queue(int maxElements);

/**
 * Dispose a queue.
 *
 * @param queue a queue
 */
void queue_dispose_queue(queue queue);

/**
 * Empty a queue. All elements are deleted.
 *
 * @param queue a queue
 */
void queue_make_empty(queue queue);

/**
 * Insert a new element into a queue.
 *
 * @param element the element to insert
 * @param queue a queue
 */
void queue_enqueue(void *element, queue queue);

/**
 * Returns the first element of the queue.
 *
 * @param queue a queue
 * @return first element
 */
void *queue_front(queue queue);

/**
 * Remove the first element of the queue.
 *
 * @param queue a queue
 */
void queue_dequeue(queue queue);

/**
 * Return the first element of the queue and remove it.
 *
 * @param queue a queue
 * @return first element
 */
void *queue_front_and_dequeue(queue queue);

/**
 * Check whether an element is part of a queue.
 *
 * @param element an queue element
 * @param queue a queue
 * @return
 */
int queue_is_element(void *element, queue queue);

/**
 * Return size of a queue.
 *
 * @param queue a queue
 * @return size of queue
 */
int queue_get_size(queue queue);

#endif							/* QUEUE_H */

/**
 * @}
 */
