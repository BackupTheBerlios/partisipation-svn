#include <stdlib.h>

#include <util/queue/queue.h>
#include <util/logging/logger.h>

#define QUEUE_MSG_PREFIX "[queue] "

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
		LOG_FAILURE(QUEUE_MSG_PREFIX "Unable to allocate more memory");
		return NULL;
	}

	queue->array = malloc(sizeof(void *) * maxElements);
	if (queue->array == NULL) {
		LOG_FAILURE(QUEUE_MSG_PREFIX "Unable to allocate more memory");
		return NULL;
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

int queue_enqueue(void *element, queue queue) {

	if (queue_is_full(queue)) {
		LOG_ERROR(QUEUE_MSG_PREFIX "the queue is full");
		return 0;
	}

	queue->size++;
	queue->rear = queue_succ(queue->rear, queue);
	queue->array[queue->rear] = element;
	return 1;
}

void *queue_front(queue queue) {

	if (queue_is_empty(queue)) {
		LOG_ERROR(QUEUE_MSG_PREFIX "the queue is empty");
		return NULL;
	}

	return queue->array[queue->front];
}

int queue_dequeue(queue queue) {

	if (queue_is_empty(queue)) {
		LOG_ERROR(QUEUE_MSG_PREFIX "the queue is empty");
		return 0;
	}

	queue->size--;
	queue->front = queue_succ(queue->front, queue);
	return 1;
}

void *queue_front_and_dequeue(queue queue) {

	void *element = NULL;

	if (queue_is_empty(queue)) {
		LOG_ERROR(QUEUE_MSG_PREFIX "the queue is empty");
		return NULL;
	}

	queue->size--;
	element = queue->array[queue->front];
	queue->front = queue_succ(queue->front, queue);
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
