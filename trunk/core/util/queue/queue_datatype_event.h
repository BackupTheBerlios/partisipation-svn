#ifndef HQUEUE_DATATYPE_USED
#define HQUEUE_DATATYPE_USED

#include "../../core/events.h"

typedef struct {
	event trigger;
	void **params;
} ElementType;

ElementType get_null_element();

#endif