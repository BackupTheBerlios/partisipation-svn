#ifndef HQUEUE_DATATYPE_USED
#define HQUEUE_DATATAPE_USED

#include <events.h>

typedef struct {
    event trigger;
    void** params;
} ElementType;


ElementType 
get_null_element();

#endif
