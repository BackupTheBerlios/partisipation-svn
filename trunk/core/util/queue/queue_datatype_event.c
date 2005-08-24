#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../core/events.h"
#include "queue_datatype_event.h"

ElementType const get_null_element() {
	ElementType const elemNull = { INVALID_EVENT, NULL };
	return elemNull;
}
