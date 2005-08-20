#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <../util/thread_management.h>
#include <events.h>
#include <event_dispatcher.h>
#include <statemachine.h>

extern sm_data **queues;

int sm_initial_state(sm_state * curState, event trigger, void **params) {
	switch (trigger) {
		case GUI_MAKE_CALL:
			*curState = CHECKING;
			break;
		case SIPLISTENER_INCOMING_CALL:
			// incoming_call
			*curState = INVITED;
			break;
		default:
			// ignore event, no transition
			break;
	}
	return 1;
}

int sm_checking_state(sm_state * curState, event trigger, void **params) {
	int rc;

	// rc = check_account_information
	if (rc != 0) {
		*curState = INVITING;
	} else {
		*curState = TERMINATING;
	}
	return 1;
}

int sm_inviting_state(sm_state * curState, event trigger, void **params) {
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
			*curState = CONNECTED;
			break;
		default:
			// ignore event, no transition
			break;
	}
	return 1;
}

int sm_invited_state(sm_state * curState, event trigger, void **params) {
	switch (trigger) {
		case GUI_END_CALL:
			// sipstack_cancel_call
			*curState = TERMINATING;
			break;
		case SIPLISTENER_CANCEL_CALL:
			*curState = TERMINATING;
			break;
		case GUI_ACCEPT_CALL:
			// sipstack_accept_call
			*curState = CONNECTED;
		default:
			// ignore event, no transition
			break;
	}
	return 1;
}

int sm_connected_state(sm_state * curState, event trigger, void **params) {
	// change_call_status(,"ACCEPTED");
	switch (trigger) {
		case GUI_END_CALL:
			*curState = TERMINATING;
			// sipstack_quit_call
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

int sm_terminating_state(sm_state * curState, event trigger, void **params) {
	// change_call_status(,"TERMINATED");
	*curState = FINAL;
	return 1;
}

void *sm_start(void *args) {

	int queueID, rc, callID;
	sm_state curState, lastState;

	queueID = (int) args;
	curState = INITIAL;

	while (1) {
		Queue eventPool = queues[queueID]->eventPool;

		while (!IsEmpty(eventPool)) {
			ElementType elem = FrontAndDequeue(eventPool);

			lastState = curState;
			switch (curState) {
				case INITIAL:
					rc = sm_initial_state(&curState, elem.trigger,
										  elem.params);
					if (rc != 0) {
						// ERROR
					}
					break;
				case CHECKING:
					rc = sm_checking_state(&curState, elem.trigger,
										   elem.params);
					if (rc != 0) {
						// ERROR
					}
					break;
				case INVITING:
					rc = sm_inviting_state(&curState, elem.trigger,
										   elem.params);
					if (rc != 0) {
						// ERROR
					}
					break;
				case INVITED:
					rc = sm_invited_state(&curState, elem.trigger,
										  elem.params);
					if (rc != 0) {
						// ERROR
					}
					break;
				case CONNECTED:
					rc = sm_connected_state(&curState, elem.trigger,
											elem.params);
					if (rc != 0) {
						// ERROR
					}
					break;
				case TERMINATING:
					rc = sm_terminating_state(&curState, elem.trigger,
											  elem.params);
					if (rc != 0) {
						// ERROR
					}
					break;
				case FINAL:
					break;
			}
			printf("sm_start: %d\n", (int) elem.params[0]);
		}
	}
	thread_terminated(pthread_self());
	pthread_exit(NULL);
}
