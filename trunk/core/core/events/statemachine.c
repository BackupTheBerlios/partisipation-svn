#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <util/threads/thread_management.h>
#include <core/gui_output/gui_callback_sender.h>
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
	int sipCallId;
	int dialogId;
	int transactionId;
} local_call_info;

int sm_connected_state_on_entry(local_call_info * callInfo) {
	int rc;
	rc = go_change_call_status(callInfo->callId, "ACCEPTED");
	if (!rc) {
		// ERROR
		return 0;
	}
	return 1;
}

int sm_terminating_state_on_exit(local_call_info * callInfo) {
	int rc;
	rc = go_change_call_status(callInfo->callId, "TERMINATED");
	if (!rc) {
		// ERROR
		return 0;
	}
	return 1;
}

int sm_ringing_state_on_entry(local_call_info * callInfo) {
	int rc;
	rc = sipstack_send_status_code(callInfo->transactionId, 180);
	if (rc != 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int sm_initial_state(sm_state * curState, event trigger, void **params,
					 local_call_info * callInfo) {
	int rc;
	sipstack_event *sipEvt;

	switch (trigger) {
		case GUI_MAKE_CALL:
			callInfo->accountId = (int) params[0];
			callInfo->callId = (int) params[2];
			callInfo->callee = (char *) params[1];
			*curState = CHECKING;
			break;
		case SIPLISTENER_RECEIVE:
			// incoming call
			sipEvt = (sipstack_event *) params[0];
			if (strcmp(sipEvt->message, "INVITE") != 0) {
				// wrong event
				break;
			}

			rc = sm_ringing_state_on_entry(callInfo);
			if (!rc) {
				// ERROR
				break;
			}
			*curState = RINGING;
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

		rc = sipstack_send_invite(callInfo->callee, callInfo->caller, "");
		if (!rc) {
			// ERROR
			return 0;
		}
		*curState = INVITING;
	} else {
		rc = go_show_user_event(callInfo->accountId, "ERROR",
								"Error checking account data",
								"Account information insufficient or "
								"incorrect.",
								"Check which account you are using, are all"
								"neccessary fields filled in and correct?");
		if (!rc) {
			// ERROR
		}
		*curState = TERMINATING;
	}
	return 1;
}

// *INDENT-OFF*

int sm_inviting_state(sm_state * curState, event trigger, void **params,
					  local_call_info * callInfo) {
	int rc;
	char *msg;
	sipstack_event *sipEvt;
	switch (trigger) {
		case SIPLISTENER_RECEIVE:
			sipEvt = (sipstack_event *) params[0];
			if ((sipEvt->statusCode >= 100)
				 && (sipEvt->statusCode <= 199)) {
				switch (sipEvt->statusCode) {
					case 180:
						msg = "RINGING"; 
						break;
					default:
						msg = "TRYING"; 
						break;
				}

				rc = go_change_call_status(callInfo->callId, msg); 
				if (!rc) {
					// ERROR
					break;
				}
				
				// don't leave current state, we're still inviting callee:
				*curState = INVITING;
			} else if ((sipEvt->statusCode >= 200)
					&& (sipEvt->statusCode <= 299)) {
				int rc;
				rc = sipstack_send_acknowledgment(callInfo->dialogId);
				if (rc != 0) {
				// ERROR
				break;}

				// invite transaction was successfull, invoke on entry routine
				// of connected state:
				rc = sm_connected_state_on_entry(callInfo); if (!rc) {
				// ERROR
				break;}

				*curState = CONNECTED; break;}
				else
				if ((sipEvt->statusCode >= 300)
					&& (sipEvt->statusCode <= 699)) {
				int rc;
				rc = sipstack_send_acknowledgment(callInfo->dialogId);
				if (rc != 0) {
				// ERROR
				break;}

				*curState = TERMINATING; break;}

				// ignore event, no transition
		break; case GUI_END_CALL:
				rc =
				sipstack_terminate(callInfo->sipCallId,
								   callInfo->dialogId); if (rc != 0) {
				// ERROR
				break;}
		*curState = TERMINATING; break; default:
				// ignore event, no transition
				break;}
				return 1;}

// *INDENT-ON*

int sm_ringing_state(sm_state * curState, event trigger,
					 void **params, local_call_info * callInfo) {
	int rc;
	sipstack_event *sipEvt;
	switch (trigger) {
		case GUI_ACCEPT_CALL:
			rc = sipstack_send_ok(callInfo->dialogId,
								  callInfo->transactionId);
			if (rc != 0) {
				// ERROR
				break;
			}
			*curState = CONNECTING;
			break;
		case GUI_END_CALL:
			rc = sipstack_terminate(callInfo->sipCallId,
									callInfo->dialogId);
			if (rc != 0) {
				// ERROR
				break;
			}
			*curState = TERMINATING;
		case SIPLISTENER_RECEIVE:
			sipEvt = (sipstack_event *) params[0];
			if (strcmp(sipEvt->message, "CANCEL") != 0) {
				// wrong event
				break;
			}
			// INVITE
			rc = sipstack_send_status_code(callInfo->transactionId, 487);
			if (rc != 0) {
				// ERROR
				break;
			}
			// CANCEL
			rc = sipstack_send_ok(callInfo->dialogId,
								  callInfo->transactionId);
			if (rc != 0) {
				// ERROR
				break;
			}

			rc = go_change_call_status(callInfo->callId, "CANCELLED");
			if (!rc) {
				// ERROR
				break;
			}

			break;
		default:
			// ignore event, no transition
			break;
	}
	return 1;
}

int sm_connecting_state(sm_state * curState, event trigger,
						void **params, local_call_info * callInfo) {
	int rc;
	sipstack_event *sipEvt;
	switch (trigger) {
		case GUI_ACCEPT_CALL:
			rc = sipstack_send_ok(callInfo->dialogId,
								  callInfo->transactionId);
			if (rc != 0) {
				// ERROR
				break;
			}
			*curState = CONNECTING;
			break;
		case GUI_END_CALL:
			rc = sipstack_terminate(callInfo->sipCallId,
									callInfo->dialogId);
			if (rc != 0) {
				// ERROR
				break;
			}
			*curState = TERMINATING;
		case SIPLISTENER_RECEIVE:
			sipEvt = (sipstack_event *) params[0];
			if (strcmp(sipEvt->message, "ACK") == 0) {
				rc = sm_connected_state_on_entry(callInfo);
				if (!rc) {
					// ERROR
					break;
				}

				*curState = CONNECTED;
				break;
			} else if (strcmp(sipEvt->message, "CANCEL") == 0) {
				// INVITE
				rc = sipstack_send_status_code(callInfo->transactionId,
											   487);
				if (rc != 0) {
					// ERROR
					return 0;
				}
				// CANCEL
				rc = sipstack_send_ok(callInfo->dialogId,
									  callInfo->transactionId);
				if (rc != 0) {
					// ERROR
					break;
				}

				rc = go_change_call_status(callInfo->callId, "CANCELLED");
				if (!rc) {
					// ERROR
					break;
				}

				break;
			} else {
				// wrong event
				break;
			}

			break;
		default:
			// ignore event, no transition
			break;
	}
	return 1;
}

int sm_connected_state(sm_state * curState, event trigger,
					   void **params, local_call_info * callInfo) {
	int rc;
	sipstack_event *sipEvt;
	switch (trigger) {
		case GUI_END_CALL:
			rc = sipstack_terminate(callInfo->sipCallId,
									callInfo->dialogId);
			if (rc != 0) {
				// ERROR
				break;
			}
			*curState = TERMINATING;
			break;
		case SIPLISTENER_RECEIVE:
			sipEvt = (sipstack_event *) params[0];
			if (strcmp(sipEvt->message, "BYE") != 0) {
				// wrong event
				break;
			}

			rc = sipstack_send_ok(callInfo->dialogId,
								  callInfo->transactionId);
			if (rc != 0) {
				// ERROR
				break;
			}
			*curState = TERMINATING;
			break;
		default:
			// ignore event, no transition
			break;
	}
	return 1;
}

int sm_terminating_state(sm_state * curState,
						 event trigger, void **params,
						 local_call_info * callInfo) {
	int rc;
	rc = sm_terminating_state_on_exit(callInfo);
	if (!rc) {
		// ERROR
		return 0;
	}
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
				case RINGING:
					rc = sm_ringing_state(&curState, elem.trigger,
										  elem.params, callInfo);
					if (rc != 0) {
						// ERROR
					}
					break;
				case CONNECTING:
					rc = sm_ringing_state(&curState, elem.trigger,
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
