#include <stdlib.h>
#include <stdio.h>

#include <remote/server/calls.h>
#include <core/gui_input/calls_receiver.h>

int make_call(int const accountId, char *const callee) {
	return gi_make_call(accountId, callee);
}

int end_call(int const callId) {
	return gi_end_call(callId);
}

int accept_call(int const callId) {
	return gi_accept_call(callId);
}
