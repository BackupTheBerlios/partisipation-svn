#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <core/gui_input/calls_receiver.h>
#include <remote/callback/gui_callback.h>
#include <core/events/events.h>
#include <core/events/event_dispatcher.h>

int gi_make_call(int const accountId, char *const callee) {
	int callId, rc;
	int len;
	void **params;
	char *s;

	if (!callee) {
		// no callee given
		return -1;
	}

	len = strlen(callee);
	s = (char *) malloc(len * sizeof(char) + 1);
	strcpy(s, callee);

	params = (void **) malloc(2 * sizeof(void *));
	params[0] = (void *) accountId;
	params[1] = (void *) s;
	rc = event_dispatch(GUI_MAKE_CALL, params, &callId);
	if (!rc) {
		// ERROR
		return -1;
	}
	return callId;
}

int gi_end_call(int const callId) {
	int rc;
	void **params;

	params = (void **) malloc(1 * sizeof(void *));
	params[0] = (void *) callId;
	rc = event_dispatch(GUI_END_CALL, params, NULL);
	if (!rc) {
		// ERROR
		return 0;
	}
	return 1;
}

int gi_accept_call(int const callId) {
	int rc;
	void **params;

	params = (void **) malloc(1 * sizeof(void *));
	params[0] = (void *) callId;
	rc = event_dispatch(GUI_ACCEPT_CALL, params, NULL);
	if (!rc) {
		// ERROR
		return 0;
	}
	return 1;
}
