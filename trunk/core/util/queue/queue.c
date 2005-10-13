#include "util/queue/queue.h"
#include <stdlib.h>

struct queue_record {
	int capacity;
	int front;
	int rear;
	int size;
	void **array;
};

int queue_is_empty(queue queue) {
	return queue->size == 0;
}

int queue_is_full(queue queue) {
	return queue->size == queue->capacity;
}

queue queue_create_queue(int maxElements) {
	queue queue;

	queue = malloc(sizeof(struct queue_record));
	if (queue == NULL) {
		FatalError("CreateQueue Error: Unable to allocate more memory.");
	}

	queue->array = malloc(sizeof(void *) * maxElements);
	if (queue->array == NULL) {
		FatalError("CreateQueue Error: Unable to allocate more memory.");
	}

	queue->capacity = maxElements;
	queue_make_empty(queue);

	return queue;
}

void queue_make_empty(queue queue) {

	queue->size = 0;
	queue->front = 1;
	queue->rear = 0;

}

void queue_dispose_queue(queue queue) {
	if (queue != NULL) {
		free(queue->array);
		free(queue);
	}
}

static int queue_succ(int value, queue queue) {
	if (++value == queue->capacity) {
		value = 0;
	}
	return value;
}

void queue_enqueue(void *element, queue queue) {

	if (queue_is_full(queue)) {
		Error("Enqueue Error: The queue is full.");
	} else {
		queue->size++;
		queue->rear = queue_succ(queue->rear, queue);
		queue->array[queue->rear] = element;
	}

}

void *queue_front(queue queue) {

	if (!queue_is_empty(queue)) {
		return queue->array[queue->front];
	}
	Error("Front Error: The queue is empty.");

	/*
	 * Return value to avoid warnings from the compiler
	 */
	return NULL;

}

void queue_dequeue(queue queue) {

	if (queue_is_empty(queue)) {
		Error("Dequeue Error: The queue is empty.");
	} else {
		queue->size--;
		queue->front = queue_succ(queue->front, queue);
	}

}

void *queue_front_and_dequeue(queue queue) {

	void *element = NULL;

	if (queue_is_empty(queue)) {
		Error("Front and Dequeue Error: The queue is empty.");
	} else {
		queue->size--;
		element = queue->array[queue->front];
		queue->front = queue_succ(queue->front, queue);
	}
	return element;

}

int queue_is_element(void *element, queue queue) {
	int i;
	for (i = 0; i < queue->size; i++) {
		if (queue->array[i] == element) {
			return 1;
		}
	}
	return 0;
}

int queue_get_size(queue queue) {
	return queue->size;
}
