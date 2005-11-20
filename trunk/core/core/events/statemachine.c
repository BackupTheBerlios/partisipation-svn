#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <util/threads/thread_management.h>
#include <util/logging/logger.h>
#include <core/gui_output/gui_callback_sender.h>
#include <sipstack/sip_stack_interface.h>
#include <core/events/events.h>
#include <core/events/event_dispatcher.h>
#include <core/events/statemachine.h>
#include <core/events/info_types.h>
#include <core/events/call_status.h>
#include <core/events/transaction_info.h>
#include <core/callIDs/call_id_generator.h>
#include <accounts/account_core_interface.h>

/**
 * Logging prefix for statemachine.
 */
#define STATEMACHINE_PREFIX "[statemachine] "

extern sm_data **queues;

extern pthread_mutex_t queuesLock;

// forward declaration - it is defined in event_dispatcher.c but shall not be
// available via header file (interface):
int destroy_queue(int pos);

int sm_checking_state_on_entry(local_call_info * callInfo) {
	int rc;
	int success;
	call_trigger *elem;

	success = 1;

	rc = am_is_account_valid(callInfo->accountId);
	if (!rc) {
		LOG_DEBUG(STATEMACHINE_PREFIX "checking.OnEntry: account data of "
				  "account %d is not valid", callInfo->accountId);
		success = 0;
	}

	if (success) {
		rc = am_build_from_and_to(callInfo->accountId, callInfo->callee,
								  &callInfo->from, &callInfo->to);
		if (!rc) {
			LOG_DEBUG(STATEMACHINE_PREFIX "checking.OnEntry: failed to "
					  "build From and To field of account %d",
					  callInfo->accountId);
			success = 0;
		}
	}

	rc = pthread_mutex_lock(&queuesLock);
	if (rc != 0) {
		LOG_DEBUG(STATEMACHINE_PREFIX "checking.OnEntry: failed to gain "
				  "mutex lock");
		return 0;
	}
	// create PSEUDO event to avoid dirty hack in sm_start loop
	elem = (call_trigger *) malloc(sizeof(call_trigger));
	if (success) {
		elem->trigger = ACCOUNT_OK;
	} else {
		elem->trigger = ACCOUNT_ERROR;
	}
	elem->params = NULL;

	rc = queue_enqueue((void *) elem,
					   queues[callInfo->queueId]->eventPool);
	if (!rc) {
		LOG_DEBUG(STATEMACHINE_PREFIX "checking.OnEntry: failed enqueue "
				  "event");
		pthread_mutex_unlock(&queuesLock);
		return 0;
	}

	pthread_mutex_unlock(&queuesLock);
	return 1;
}

int sm_inviting_state_on_entry(local_call_info * callInfo) {
	int rc;
	struct account *acc;

	acc = am_get_account(callInfo->accountId);
	if (!acc) {
		LOG_DEBUG(STATEMACHINE_PREFIX "inviting.OnEntry: failed to get "
				  "account information for account %d",
				  callInfo->accountId);
		return 0;
	}
	// event dispatcher has to know SIP call ID: save it in own statemachine
	// record; lock before actually sending the invite to prevent answer 
	// arriving before sipCallId is set
	rc = pthread_mutex_lock(&queuesLock);
	if (rc != 0) {
		LOG_DEBUG(STATEMACHINE_PREFIX "initial state, "
				  "SipListener.receive: failed to gain mutex lock");
		return 0;
	}

	rc = sipstack_send_invite(callInfo->to, callInfo->from,
							  acc->displayname, "");
	if (rc == -1) {
		LOG_DEBUG(STATEMACHINE_PREFIX "inviting.OnEntry: failed to send "
				  "INVITE via sipstack adapter (call ID: %d)",
				  callInfo->callId);
		return 0;
	}

	callInfo->sipCallId = rc;

	queues[callInfo->queueId]->sipCallId = callInfo->sipCallId;

	rc = pthread_mutex_unlock(&queuesLock);
	if (rc != 0) {
		LOG_DEBUG(STATEMACHINE_PREFIX "initial state, "
				  "SipListener.receive: failed to release mutex " "lock");
		return 0;
	}

	return 1;
}

int sm_connected_state_on_entry(local_call_info * callInfo) {
	int rc;
	rc = go_change_call_status(callInfo->callId,
							   call_status_to_str(CS_ACCEPTED));
	if (!rc) {
		LOG_DEBUG(STATEMACHINE_PREFIX "connected.OnEntry: failed to "
				  "change call status (call ID: %d)", callInfo->callId);
		return 0;
	}
	return 1;
}

int sm_terminating_state_on_exit(local_call_info * callInfo) {
	int rc;
	rc = go_change_call_status(callInfo->callId,
							   call_status_to_str(CS_TERMINATED));
	if (!rc) {
		LOG_DEBUG(STATEMACHINE_PREFIX "terminating.OnExit: failed to "
				  "change call status (call ID: %d)", callInfo->callId);
		return 0;
	}
	return 1;
}

int sm_ringing_state_on_entry(local_call_info * callInfo) {
	int rc;
	int transId;

	transId = find_transaction_by_type(callInfo, INVITE_TRANSACTION);
	if (transId == -1) {
		LOG_DEBUG(STATEMACHINE_PREFIX "ringing.OnEntry: failed to find "
				  "matching INVITE transaction (call ID: %d)",
				  callInfo->callId);
		return 0;
	}

	rc = sipstack_send_status_code(transId, 180);
	if (!rc) {
		LOG_DEBUG(STATEMACHINE_PREFIX "ringing.OnEntry: failed to send "
				  "status code 180 via sipstack adapter (call ID: %d)",
				  callInfo->callId);
		return 0;
	}
	return 1;
}

int sm_connecting_state_on_entry(local_call_info * callInfo) {
	int rc;
	int transId;

	transId = find_transaction_by_type(callInfo, INVITE_TRANSACTION);
	if (transId == -1) {
		LOG_DEBUG(STATEMACHINE_PREFIX "connecting.OnEntry: failed to find "
				  "matching INVITE transaction (call ID: %d)",
				  callInfo->callId);
		return 0;
	}

	rc = sipstack_send_ok(callInfo->dialogId, transId);
	if (!rc) {
		LOG_DEBUG(STATEMACHINE_PREFIX "ringing.OnEntry: failed to send "
				  "status code 200 via sipstack adapter (call ID: %d)",
				  callInfo->callId);
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
			if (!params[1]) {
				LOG_DEBUG(STATEMACHINE_PREFIX "inital state, "
						  "GUI.makeCall: no callee given (callID %d)",
						  callInfo->callId);
				return 0;
			}
			callInfo->callee =
				(char *) malloc(strlen((char *) params[1]) * sizeof(char) +
								1);
			strcpy(callInfo->callee, (char *) params[1]);
			callInfo->caller = NULL;
			callInfo->sipCallId = 0;
			callInfo->dialogId = 0;
			callInfo->from = NULL;
			callInfo->to = NULL;

			rc = sm_checking_state_on_entry(callInfo);
			if (!rc) {
				LOG_DEBUG(STATEMACHINE_PREFIX "inital state, "
						  "GUI.makeCall: failed to perform "
						  "checking.OnEntry (call ID %d)",
						  callInfo->callId);
				return 0;
			}

			*curState = CHECKING;
			break;
		case SIPLISTENER_RECEIVE:
			// incoming call
			sipEvt = (sipstack_event *) params[0];
			if (sipEvt->type != EXOSIP_CALL_INVITE) {
				LOG_DEBUG(STATEMACHINE_PREFIX "inital state, "
						  "SipListener.receive: expected INVITE, but received "
						  "%d - ignoring event", sipEvt->type);
				break;
			}

			LOG_DEBUG(STATEMACHINE_PREFIX "inital state, "
					  "SipListener.receive: To URL: %s", sipEvt->toUrl);
			callInfo->accountId =
				am_get_account_by_callee_uri(sipEvt->toUrl);
			if (callInfo->accountId == -1) {
				LOG_DEBUG(STATEMACHINE_PREFIX "inital state, "
						  "SipListener.receive: no matching account found to "
						  "callee URI %s", sipEvt->toUrl);

				// no response is sent because we want to block broadcast 
				// messages and prevent spamming (firewall concept)
				return 0;
			}
			if (!sipEvt->toUrl) {
				LOG_DEBUG(STATEMACHINE_PREFIX "inital state, "
						  "SipListener.receive: To URL missing");
				return 0;
			}
			if (!sipEvt->fromUrl) {
				LOG_DEBUG(STATEMACHINE_PREFIX "inital state, "
						  "SipListener.receive: From URL missing");
				return 0;
			}

			callInfo->callId = cig_generate_call_id();
			callInfo->callee =
				(char *) malloc(strlen(sipEvt->toUrl) * sizeof(char) + 1);
			strcpy(callInfo->callee, sipEvt->toUrl);
			callInfo->caller =
				(char *) malloc(strlen(sipEvt->fromUrl) * sizeof(char) +
								1);
			strcpy(callInfo->caller, sipEvt->fromUrl);
			callInfo->sipCallId = sipEvt->callId;
			callInfo->dialogId = sipEvt->dialogId;
			callInfo->from = NULL;
			callInfo->to = NULL;

			// event dispatcher has to know generated call ID: save it in
			// own statemachine record
			rc = pthread_mutex_lock(&queuesLock);
			if (rc != 0) {
				LOG_DEBUG(STATEMACHINE_PREFIX "initial state, "
						  "SipListener.receive: failed to gain mutex lock");
				return 0;
			}

			queues[callInfo->queueId]->callId = callInfo->callId;

			rc = pthread_mutex_unlock(&queuesLock);
			if (rc != 0) {
				LOG_DEBUG(STATEMACHINE_PREFIX "initial state, "
						  "SipListener.receive: failed to release mutex "
						  "lock");
				return 0;
			}

			rc = add_transaction(callInfo, sipEvt->transactionId,
								 INVITE_TRANSACTION);
			if (!rc) {
				LOG_DEBUG(STATEMACHINE_PREFIX "inital state, "
						  "SipListener.receive: failed to add INVITE "
						  "transaction");
				return 0;
			}

			rc = go_incoming_call(callInfo->accountId, callInfo->callId,
								  callInfo->caller,
								  sipEvt->fromDisplayName);
			if (!rc) {
				LOG_DEBUG(STATEMACHINE_PREFIX "inital state, "
						  "SipListener.receive: failed to inform the GUI "
						  "about an incoming call (caller: %s)",
						  callInfo->caller);
				return 0;
			}

			rc = sm_ringing_state_on_entry(callInfo);
			if (!rc) {
				LOG_DEBUG(STATEMACHINE_PREFIX "inital state, "
						  "SipListener.receive: failed to perform "
						  "ringing.OnEntry (caller: %s)",
						  callInfo->caller);
				return 0;
			}
			*curState = RINGING;
			break;
		default:
			// ignore event, no transition
			LOG_DEBUG(STATEMACHINE_PREFIX "inital state, received event "
					  "without matching transition: ignoring");
			break;
	}
	return 1;
}

int sm_checking_state(sm_state * curState, event trigger, void **params,
					  local_call_info * callInfo) {
	int rc;

	switch (trigger) {
		case ACCOUNT_OK:
			rc = sm_inviting_state_on_entry(callInfo);
			if (!rc) {
				LOG_DEBUG(STATEMACHINE_PREFIX "checking state, accountOk: "
						  "failed to perform inviting.OnEntry (call ID %d)",
						  callInfo->callId);
				return 0;
			}
			*curState = INVITING;
			break;
		case ACCOUNT_ERROR:
			rc = go_show_user_event(callInfo->accountId, "ERROR",
									"Error checking account data",
									"Account information insufficient or "
									"incorrect.",
									"Check which account you are using, "
									"are all neccessary fields filled in "
									"and correct?");
			if (!rc) {
				LOG_DEBUG(STATEMACHINE_PREFIX "checking state, "
						  "accountError: GUI.showUserEvent returned with an "
						  "error (call ID %d)", callInfo->callId);
				return 0;
			}
			*curState = TERMINATING;
			break;
		default:
			// ignore event, no transition
			LOG_DEBUG(STATEMACHINE_PREFIX "checking state, received event "
					  "without matching transition: ignoring");
			break;
	}
	return 1;
}

// *INDENT-OFF*

int sm_inviting_state(sm_state * curState, event trigger, void **params,
					  local_call_info * callInfo) {
	int rc;
	call_status callSt;
	sipstack_event *sipEvt;
	
	switch (trigger) {
		case SIPLISTENER_RECEIVE:
			sipEvt = (sipstack_event *) params[0];
			update_transaction_and_dialog(callInfo, sipEvt->transactionId, 
										  INVITE_TRANSACTION, 
										  sipEvt->dialogId);
			if ((sipEvt->statusCode >= 100)
				 && (sipEvt->statusCode <= 199)) {
				switch (sipEvt->statusCode) {
					case 180:
						callSt = CS_RINGING; 
						break;
					default:
						callSt = CS_TRYING; 
						break;
				}

				rc = go_change_call_status(callInfo->callId, call_status_to_str(callSt));
				if (!rc) {
					LOG_DEBUG(STATEMACHINE_PREFIX "inviting state, "
						"SipListener.Receive: failed to change "
						"call status (call ID: %d)", callInfo->callId);
					return 0;
				}
				
				// don't leave current state, we're still inviting callee:
				*curState = INVITING;
				break;
			} else if ((sipEvt->statusCode >= 200)
						&& (sipEvt->statusCode <= 299)) {
				int rc;
				rc = sipstack_send_acknowledgment(callInfo->dialogId);
				if (!rc) {
					LOG_DEBUG(STATEMACHINE_PREFIX "inviting state, "
						"SipListener.Receive: failed to send ACK via "
						"sipstack adapter (call ID: %d, dialog ID: %d)", 
						callInfo->callId, callInfo->dialogId);
					return 0;
				}

				// invite transaction was successfull, invoke on entry routine
				// of connected state:
				rc = sm_connected_state_on_entry(callInfo); 
				if (!rc) {
					LOG_DEBUG(STATEMACHINE_PREFIX "inviting state, "
						"SipListener.Receive: failed to perform "
						"connected.OnEntry (call ID: %d)", callInfo->callId);
					return 0;
				}

				*curState = CONNECTED; 
				break;
			} else if ((sipEvt->statusCode >= 300)
						&& (sipEvt->statusCode <= 699)) {
				int rc;
				
				// send ACK is done by sipstack
				
				rc = go_change_call_status(callInfo->callId, call_status_to_str(CS_DECLINED));
				if (!rc) {
					LOG_DEBUG(STATEMACHINE_PREFIX "inviting state, "
							  "SipListener.Receive failed to change call "
							  "status (call ID: %d)", callInfo->callId);
					return 0;
				}

				*curState = TERMINATING; 
				break;
			}

			// ignore event, no transition
			LOG_DEBUG(STATEMACHINE_PREFIX "inviting state, "
				"SipListener.receive: expected status code response, but "
				"received %d - ignoring event", sipEvt->type);
			break; 
		case GUI_END_CALL:
			rc = sipstack_cancel(callInfo->sipCallId, callInfo->dialogId);
			if (!rc) {
					LOG_DEBUG(STATEMACHINE_PREFIX "inviting state, "
						"SipListener.Receive: failed to send terminate call "
						"via sipstack adapter (call ID: %d)", 
						callInfo->callId);
				return 0;
			}
				
			*curState = TERMINATING; 
			break; 
		default:
			// ignore event, no transition
			LOG_DEBUG(STATEMACHINE_PREFIX "inviting state, received event "
					  "without matching transition: ignoring");
			break;
		}
	return 1;
}

// *INDENT-ON*

int sm_ringing_state(sm_state * curState, event trigger,
					 void **params, local_call_info * callInfo) {
	int rc;
//  int transId;
	sipstack_event *sipEvt;

	switch (trigger) {
		case GUI_ACCEPT_CALL:
			rc = sm_connecting_state_on_entry(callInfo);
			if (!rc) {
				LOG_DEBUG(STATEMACHINE_PREFIX "ringing state, "
						  "GUI.acceptCall: failed to perform "
						  "connecting.OnEntry (call ID %d)",
						  callInfo->callId);
				return 0;
			}
			*curState = CONNECTING;
			break;
		case GUI_END_CALL:
			rc = sipstack_decline(callInfo->sipCallId, callInfo->dialogId);
			if (!rc) {
				LOG_DEBUG(STATEMACHINE_PREFIX "ringing state, "
						  "GUI.endCall: failed to send terminate call "
						  "via sipstack adapter (call ID: %d)",
						  callInfo->callId);
				return 0;
			}
			*curState = TERMINATING;
			break;
		case SIPLISTENER_RECEIVE:
			sipEvt = (sipstack_event *) params[0];
			// unfortunately eXosip does not use its own events properly
			// so we have to assume that CALL_CLOSED (BYE) and statusCode 487 
			// is CANCEL
//          if (sipEvt->type != EXOSIP_CALL_CANCELLED) {
			if (sipEvt->type != EXOSIP_CALL_CLOSED
				|| sipEvt->statusCode != 487) {
				LOG_DEBUG(STATEMACHINE_PREFIX "ringing state, "
						  "SipListener.receive: expected CANCEL, but received "
						  "%d - ignoring event", sipEvt->type);
				break;
			}
			// send 487 (INVITE), send OK (CANCEL) is done by sipstack

			rc = go_change_call_status(callInfo->callId,
									   call_status_to_str(CS_CANCELLED));
			if (!rc) {
				LOG_DEBUG(STATEMACHINE_PREFIX "ringing state, "
						  "SipListener.Receive: failed to change call status "
						  "(call ID: %d)", callInfo->callId);
				return 0;
			}

			*curState = TERMINATING;
			break;
		default:
			// ignore event, no transition
			LOG_DEBUG(STATEMACHINE_PREFIX "ringing state, received event "
					  "without matching transition: ignoring");
			break;
	}
	return 1;
}

int sm_connecting_state(sm_state * curState, event trigger,
						void **params, local_call_info * callInfo) {
	int rc;
	int transId;
	sipstack_event *sipEvt;
	switch (trigger) {
		case GUI_ACCEPT_CALL:
			transId =
				find_transaction_by_type(callInfo, INVITE_TRANSACTION);
			if (transId == -1) {
				LOG_DEBUG(STATEMACHINE_PREFIX "connecting state, "
						  "GUI.acceptCall: failed to find matching INVITE "
						  "transaction (call ID: %d)", callInfo->callId);
				return 0;
			}

			rc = sipstack_send_ok(callInfo->dialogId, transId);
			if (!rc) {
				LOG_DEBUG(STATEMACHINE_PREFIX "connecting state: "
						  "GUI.acceptCall: failed to send status code 200 via "
						  "sipstack adapter (call ID: %d)",
						  callInfo->callId);
				return 0;
			}

			*curState = CONNECTING;
			break;
		case GUI_END_CALL:
			rc = sipstack_decline(callInfo->sipCallId, callInfo->dialogId);
			if (!rc) {
				LOG_DEBUG(STATEMACHINE_PREFIX "connecting state, "
						  "GUI.endCall: failed to send terminate call via sipstack "
						  "adapter (call ID: %d)", callInfo->callId);
				return 0;
			}
			*curState = TERMINATING;
			break;
		case SIPLISTENER_RECEIVE:
			sipEvt = (sipstack_event *) params[0];
			if (sipEvt->type == EXOSIP_CALL_ACK) {
				rc = sm_connected_state_on_entry(callInfo);
				if (!rc) {
					LOG_DEBUG(STATEMACHINE_PREFIX "connecting state, "
							  "SipListener.Receive: failed to perform "
							  "connected.OnEntry (call ID: %d)",
							  callInfo->callId);
					return 0;
				}

				*curState = CONNECTED;
				break;

				// unfortunately eXosip does not use its own events properly
				// so we have to assume that CALL_CLOSED (BYE) and statusCode 487 
				// is CANCEL
//          } else if (sipEvt->type == EXOSIP_CALL_CANCELLED) {
			} else if (sipEvt->type == EXOSIP_CALL_CLOSED
					   && sipEvt->statusCode == 487) {

				// send 487 (INVITE), send OK (CANCEL) is done by sipstack

				rc = go_change_call_status(callInfo->callId,
										   call_status_to_str
										   (CS_CANCELLED));
				if (!rc) {
					LOG_DEBUG(STATEMACHINE_PREFIX "connecting state, "
							  "SipListener.Receive: failed to change "
							  "call status (call ID: %d)",
							  callInfo->callId);
					return 0;
				}

				*curState = TERMINATING;
				break;
			}
			// wrong event, ignore
			LOG_DEBUG(STATEMACHINE_PREFIX "connecting state, "
					  "SipListener.receive: expected ACK or CANCEL, but "
					  "received %d - ignoring event", sipEvt->type);
			break;
		default:
			// ignore event, no transition
			LOG_DEBUG(STATEMACHINE_PREFIX
					  "connecting state, received event "
					  "without matching transition: ignoring");
			break;
	}
	return 1;
}

int sm_connected_state(sm_state * curState, event trigger,
					   void **params, local_call_info * callInfo) {
	int rc;
	int transId;
	sipstack_event *sipEvt;
	switch (trigger) {
		case GUI_END_CALL:
			rc = sipstack_bye(callInfo->sipCallId, callInfo->dialogId);
			if (!rc) {
				LOG_DEBUG(STATEMACHINE_PREFIX
						  "connected state, GUI.endCall: "
						  "failed to send terminate call via sipstack adapter "
						  "(call ID: %d)", callInfo->callId);
				return 0;
			}
			*curState = TERMINATING;
			break;
		case SIPLISTENER_RECEIVE:
			sipEvt = (sipstack_event *) params[0];

			if (sipEvt->type != EXOSIP_CALL_CLOSED) {
				LOG_DEBUG(STATEMACHINE_PREFIX "connected state, "
						  "SipListener.receive: expected BYE, but "
						  "received %d - ignoring event", sipEvt->type);
				break;
			}
			// BYE

			update_transaction_and_dialog(callInfo, sipEvt->transactionId,
										  BYE_TRANSACTION,
										  sipEvt->dialogId);

			transId = find_transaction_by_type(callInfo, BYE_TRANSACTION);
			if (transId == -1) {
				LOG_DEBUG(STATEMACHINE_PREFIX "connected state, "
						  "SipListener.receive: failed to find matching "
						  "BYE transaction (call ID: %d)",
						  callInfo->callId);
				return 0;
			}
			// OK is done by sipstack

			*curState = TERMINATING;
			break;
		default:
			// ignore event, no transition
			LOG_DEBUG(STATEMACHINE_PREFIX
					  "connecting state, received event "
					  "without matching transition: ignoring");
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
		LOG_DEBUG(STATEMACHINE_PREFIX
				  "terminating state: failed to perform "
				  "terminating.OnExit (call ID: %d)", callInfo->callId);
		return 0;
	}
	*curState = FINAL;
	return 1;
}

int free_call_trigger(call_trigger * elem) {
	int rc;
	sipstack_event *sipEvt;
	char *callee;

	if (!elem) {
		return 1;
	}

	switch (elem->trigger) {
		case GUI_MAKE_CALL:
			callee = (char *) elem->params[1];
			if (callee) {
				free(callee);
			}
			if (elem->params) {
				free(elem->params);
			}
			break;
		case GUI_END_CALL:
			if (elem->params) {
				free(elem->params);
			}
			break;
		case GUI_ACCEPT_CALL:
			if (elem->params) {
				free(elem->params);
			}
			break;
		case SIPLISTENER_RECEIVE:
			if (!elem->params) {
				break;
			}

			sipEvt = (sipstack_event *) elem->params[0];
			rc = sipstack_event_free(sipEvt);
			if (!rc) {
				LOG_DEBUG(STATEMACHINE_PREFIX "failed to release sipstack "
						  "event");
				free(elem->params);
				free(elem);
				return 0;
			}

			free(elem->params);
			break;
		case ACCOUNT_ERROR:
			break;
		case ACCOUNT_OK:
			break;
		default:
			break;
	}

	free(elem);
	return 1;
}

void *sm_start(void *args) {

	int queueId;
	int rc;
	int finished;
	int errorOccurred;
	sm_state curState;
	local_call_info callInfo;
	queue eventPool;

	init_transactions(&callInfo);

	queueId = (int) args;
	callInfo.queueId = queueId;
	eventPool = queues[queueId]->eventPool;

	curState = INITIAL;
	finished = 0;
	errorOccurred = 0;

	while (!errorOccurred && !finished) {
		while ((curState == TERMINATING) || (!queue_is_empty(eventPool))) {

			call_trigger *elem;

			elem = NULL;
			if (curState != TERMINATING) {
				elem = (call_trigger *) queue_front_and_dequeue(eventPool);
				if (!elem) {
					LOG_DEBUG(STATEMACHINE_PREFIX "failed to dequeue "
							  "event (queue empty?)");
					errorOccurred = 1;
					break;
				}
			}

			switch (curState) {
				case INITIAL:
					rc = sm_initial_state(&curState, elem->trigger,
										  elem->params, &callInfo);
					if (!rc) {
						LOG_DEBUG(STATEMACHINE_PREFIX "inital state could "
								  "not be performed (call ID %d)",
								  callInfo.callId);
						errorOccurred = 1;
						break;
					}
					break;
				case CHECKING:
					rc = sm_checking_state(&curState, elem->trigger,
										   elem->params, &callInfo);
					if (!rc) {
						LOG_DEBUG(STATEMACHINE_PREFIX "checking state "
								  "could not be performed (call ID %d)",
								  callInfo.callId);
						errorOccurred = 1;
						break;
					}
					break;
				case INVITING:
					rc = sm_inviting_state(&curState, elem->trigger,
										   elem->params, &callInfo);
					if (!rc) {
						LOG_DEBUG(STATEMACHINE_PREFIX "inviting state "
								  "could not be performed (call ID %d)",
								  callInfo.callId);
						errorOccurred = 1;
						break;
					}
					break;
				case RINGING:
					rc = sm_ringing_state(&curState, elem->trigger,
										  elem->params, &callInfo);
					if (!rc) {
						LOG_DEBUG(STATEMACHINE_PREFIX "ringing state "
								  "could not be performed (call ID %d)",
								  callInfo.callId);
						errorOccurred = 1;
						break;
					}
					break;
				case CONNECTING:
					rc = sm_connecting_state(&curState, elem->trigger,
											 elem->params, &callInfo);
					if (!rc) {
						LOG_DEBUG(STATEMACHINE_PREFIX "connecting state "
								  "could not be performed (call ID %d)",
								  callInfo.callId);
						errorOccurred = 1;
						break;
					}
					break;
				case CONNECTED:
					rc = sm_connected_state(&curState, elem->trigger,
											elem->params, &callInfo);
					if (!rc) {
						LOG_DEBUG(STATEMACHINE_PREFIX "connected state "
								  "could not be performed (call ID %d)",
								  callInfo.callId);
						errorOccurred = 1;
						break;
					}
					break;
				case TERMINATING:
					rc = sm_terminating_state(&curState, INVALID_EVENT,
											  NULL, &callInfo);
					if (!rc) {
						LOG_DEBUG(STATEMACHINE_PREFIX "terminating state "
								  "could not be performed (call ID %d)",
								  callInfo.callId);
						errorOccurred = 1;
						break;
					}
					finished = 1;
					break;
				case FINAL:
					finished = 1;
					break;
			}

			if (elem) {
				free_call_trigger(elem);
			}

			if (errorOccurred || finished) {
				// leave while loop
				break;
			}

			if (queues[queueId]->doShutdown) {
				// shutdown because of an external reason
				finished = 1;
				break;
			}
		}

		if (!errorOccurred && !finished) {
			// sleep until next event
			LOG_DEBUG(STATEMACHINE_PREFIX "statemachine going to sleep "
					  "until next event arrives (callId: %d)",
					  callInfo.callId);

			rc = pthread_mutex_lock(&queues[queueId]->wakeUpLock);
			if (rc != 0) {
				LOG_DEBUG(STATEMACHINE_PREFIX "main: failed to gain mutex "
						  "lock");
				errorOccurred = 1;
				break;
			}

			rc = pthread_cond_wait(&queues[queueId]->wakeUp,
								   &queues[queueId]->wakeUpLock);
			if (rc != 0) {
				LOG_DEBUG(STATEMACHINE_PREFIX "main: failed to wait for "
						  "signal");
				errorOccurred = 1;
				break;
			}

			rc = pthread_mutex_unlock(&queues[queueId]->wakeUpLock);
			if (rc != 0) {
				LOG_DEBUG(STATEMACHINE_PREFIX "main: failed to release "
						  "mutex lock");
				errorOccurred = 1;
				break;
			}

			LOG_DEBUG(STATEMACHINE_PREFIX "statemachine awaking again "
					  "(callId: %d)", callInfo.callId);

			if (queues[queueId]->doShutdown) {
				// shutdown because of an external reason
				finished = 1;
				break;
			}
		}
	}

	if (errorOccurred) {
		LOG_ERROR(STATEMACHINE_PREFIX "state machine of call with ID %d "
				  "was stopped because of an error. Check the log file for "
				  "reasons.", callInfo.callId);
	}

	rc = destroy_transactions(&callInfo);
	if (!rc) {
		LOG_DEBUG(STATEMACHINE_PREFIX "failed to destroy transaction "
				  "references (call ID %d)", callInfo.callId);
	}

	if (callInfo.callee) {
		free(callInfo.callee);
	}

	if (callInfo.caller) {
		free(callInfo.caller);
	}

	if (callInfo.from) {
		free(callInfo.from);
	}

	if (callInfo.to) {
		free(callInfo.to);
	}

	rc = destroy_queue(queueId);
	if (!rc) {
		LOG_DEBUG(STATEMACHINE_PREFIX "failed to release statemachine "
				  "data for this statemachine (pos: %d)", queueId);
	}

	LOG_DEBUG(STATEMACHINE_PREFIX "statemachine finished.");

	thread_terminated();
	return NULL;
}
