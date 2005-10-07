#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <util/threads/thread_management.h>
#include <remote/callback/gui_callback.h>
#include <sipstack/sip_stack_interface.h>
#include <core/events/events.h>
#include <core/events/event_dispatcher.h>
#include <core/events/statemachine.h>

extern sm_data **queues;

typedef struct {
	int callId;
	int accountId;
	char *callee;
	char *caller;
} local_call_info;

int sm_inviting_state_on_entry(local_call_info * callInfo) {
	int rc;
	rc = sipstack_send_invite(callInfo->callee, callInfo->caller, "");
	if (!rc) {
		// ERROR
		return 0;
	}
	return 1;
}

int sm_connected_state_on_entry(local_call_info * callInfo) {
	int rc;
	rc = change_call_status(callInfo->callId, "ACCEPTED");
	if (!rc) {
		// ERROR
		return 0;
	}
	return 1;
}

int sm_terminating_state_on_exit(local_call_info * callInfo) {
	int rc;
	rc = change_call_status(callInfo->callId, "TERMINATING");
	if (!rc) {
		// ERROR
		return 0;
	}
	return 1;
}

int sm_initial_state(sm_state * curState, event trigger, void **params,
					 local_call_info * callInfo) {
	switch (trigger) {
		case GUI_MAKE_CALL:
			callInfo->accountId = (int) params[0];
			callInfo->callId = (int) params[2];
			callInfo->callee = (char *) params[1];
			*curState = CHECKING;
			break;
		case SIPLISTENER_INCOMING_CALL:
			// incoming_call
			callInfo->callId = (int) params[0];
			*curState = INVITED;
			break;
		default:
			// ignore event, no transition
			break;
	}
	return 1;
}

int sm_checking_state(sm_state * curState, event trigger, void **params,
					  local_call_info * callInfo) {
	int rc;

	// rc = check_account_information
	if (rc != 0) {
		rc = sm_inviting_state_on_entry(callInfo);
		if (!rc) {
			// insert error event manually?
		}
		*curState = INVITING;
	} else {
		rc = show_user_event(callInfo->accountId, "ERROR",
							 "Error checking account data",
							 "Account information insufficient or incorrect.",
							 "Check which account you are using, are all"
							 "neccessary fields filled in and correct?");
		if (!rc) {
			// ERROR
		}
		*curState = TERMINATING;
	}
	return 1;
}

int sm_inviting_state(sm_state * curState, event trigger, void **params,
					  local_call_info * callInfo) {
	switch (trigger) {
		case SIPLISTENER_THROW:
			// 3xx-6xx
			*curState = TERMINATING;
			break;
		case SIPLISTENER_DECLINE_CALL:
			*curState = TERMINATING;
			break;
		case GUI_END_CALL:
			*curState = TERMINATING;
			// sipstack_cancel_call
			break;
		case SIPLISTENER_CONNECT_CALL:
			sm_connected_state_on_entry(callInfo);
			*curState = CONNECTED;
			break;
		default:
			// ignore event, no transition
			break;
	}
	return 1;
}

int sm_invited_state(sm_state * curState, event trigger, void **params,
					 local_call_info * callInfo) {
	switch (trigger) {
		case SIPLISTENER_THROW:
			// e.g. timeout
			change_call_status(callInfo->callId, "NOT_ACCEPTED");
			*curState = TERMINATING;
		case GUI_END_CALL:
			// sipstack_cancel_call
			*curState = TERMINATING;
			break;
		case SIPLISTENER_CANCEL_CALL:
			change_call_status(callInfo->callId, "CANCELLED");
			*curState = TERMINATING;
			break;
		case GUI_ACCEPT_CALL:
			// sipstack_accept_call
			sm_connected_state_on_entry(callInfo);
			*curState = CONNECTED;
		default:
			// ignore event, no transition
			break;
	}
	return 1;
}

int sm_connected_state(sm_state * curState, event trigger, void **params,
					   local_call_info * callInfo) {
	switch (trigger) {
		case GUI_END_CALL:
			// sipstack_quit_call
			*curState = TERMINATING;
			break;
		case SIPLISTENER_QUIT_CALL:
			*curState = TERMINATING;
			break;
		default:
			// ignore event, no transition
			break;
	}
	return 1;
}

int sm_terminating_state(sm_state * curState, event trigger, void **params,
						 local_call_info * callInfo) {
	sm_terminating_state_on_exit(callInfo);
	*curState = FINAL;
	return 1;
}

void *sm_start(void *args) {

	int queueID;
	int rc;
	int finished;
	sm_state curState;
	sm_state lastState;
	local_call_info *callInfo;

	callInfo = (local_call_info *) malloc(sizeof(local_call_info));
	queueID = (int) args;
	curState = INITIAL;

	finished = 0;
	while (!finished) {
		queue eventPool = queues[queueID]->eventPool;

		while (!queue_is_empty(eventPool)) {
			call_trigger elem =
				*(call_trigger *) queue_front_and_dequeue(eventPool);

			lastState = curState;
			switch (curState) {
				case INITIAL:
					rc = sm_initial_state(&curState, elem.trigger,
										  elem.params, callInfo);
					if (rc != 0) {
						// ERROR
					}
					break;
				case CHECKING:
					rc = sm_checking_state(&curState, elem.trigger,
										   elem.params, callInfo);
					if (rc != 0) {
						// ERROR
					}
					break;
				case INVITING:
					rc = sm_inviting_state(&curState, elem.trigger,
										   elem.params, callInfo);
					if (rc != 0) {
						// ERROR
					}
					break;
				case INVITED:
					rc = sm_invited_state(&curState, elem.trigger,
										  elem.params, callInfo);
					if (rc != 0) {
						// ERROR
					}
					break;
				case CONNECTED:
					rc = sm_connected_state(&curState, elem.trigger,
											elem.params, callInfo);
					if (rc != 0) {
						// ERROR
					}
					break;
				case TERMINATING:
					rc = sm_terminating_state(&curState, elem.trigger,
											  elem.params, callInfo);
					if (rc != 0) {
						// ERROR
					}
					break;
				case FINAL:
					finished = 1;
					break;
			}
			//printf("sm_start: %d\n", (int) elem.params[0]);
		}
	}

	free(callInfo);
	thread_terminated();
	return NULL;
}
