#ifndef HEVENT_DISPATCHER_USED
#    define HEVENT_DISPATCHER_USED

#    include <events.h>

//int const MAX_CALLS = 32;

int ed_init();

int ed_destroy();

void event_dispatch(event evt, void **params);

#endif
