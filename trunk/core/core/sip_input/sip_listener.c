#include <stdlib.h>
#include <stdio.h>

#include <util/logging/logger.h>
#include <sipstack/sip_listener_interface.h>
#include <core/events/events.h>
#include <core/events/event_dispatcher.h>

#define SIP_LISTENER_MSG_PREFIX "[sip listener]"

void sip_listener_receive_event(sipstack_event * event) {
	int rc;
	void **params;
	params = (void **) malloc(1 * sizeof(void *));
	params[0] = (void *) event;
	rc = event_dispatch(SIPLISTENER_RECEIVE, params, NULL);
	if (!rc) {
		// ERROR
	}
}
