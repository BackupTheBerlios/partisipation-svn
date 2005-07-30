#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <events.h>
#include <queue/queue_datatype.h>

ElementType const
get_null_element() {
    ElementType const elemNull = { INVALID_EVENT, NULL };
    return elemNull;
}
