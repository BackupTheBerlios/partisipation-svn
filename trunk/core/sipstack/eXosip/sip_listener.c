#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

#include <util/logging/logger.h>

#include "sipstack/sip_listener_interface.h"

#define SIP_LISTENER_MSG_PREFIX "[sip listener]"

/*
 * Dummy implementation of sip listener.
 * Has to be replaced by real implementation with state machine.
 *
 * caller: SIP Stack
 * receipient: Core (Statemachine)
 */

void sip_listener_receive_event(sipstack_event * event) {
	// dummy implementation
	log_message(LOG_INFO,
				SIP_LISTENER_MSG_PREFIX
				"entering sip_listener_receive_event");
}
