#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <util/threads/thread_management.h>
#include <core/events/events.h>
#include <core/events/statemachine.h>
#include <core/callIDs/call_id_generator.h>
#include <core/events/event_dispatcher.h>
#include <util/config/globals.h>
#include <sipstack/sip_stack_interface.h>
#include <core/sip_output/registrar_manager.h>

#define EVENT_DISP_MSG_PREFIX "[event dispatcher] "

sm_data **queues;
pthread_mutex_t queuesLock;

int ed_init() {
	int rc;

	rc = pthread_mutex_init(&queuesLock, NULL);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX "failed to initialize mutex lock");
		return 0;
	}

	queues =
		(sm_data **) calloc(config.core.events.dispatcher.maxCalls,
							sizeof(sm_data *));

	return 1;
}

int ed_destroy() {
	int rc;

	rc = pthread_mutex_destroy(&queuesLock);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX "failed to release mutex lock");
		return 0;
	}

	free(queues);

	return 1;
}

int create_queue(int *pos, int callId, int sipCallId) {
	int i;						// counter
	int rc;						// return code

	// lock write access to the queues array
	pthread_mutex_lock(&queuesLock);

	i = 0;
	// find free position in array:
	while (queues[i] != 0) {
		i++;

		if (i == config.core.events.dispatcher.maxCalls) {
			LOG_ERROR(EVENT_DISP_MSG_PREFIX "too many simultanous calls, "
					  "maximum is %d",
					  config.core.events.dispatcher.maxCalls);
			// unlock
			pthread_mutex_unlock(&queuesLock);
			return 0;
		}
	}

	LOG_ERROR(EVENT_DISP_MSG_PREFIX "create queue: position found: %d\n",
			  i);

	sm_data *data;

	// reserve memory for statemachine-specific data
	data = (sm_data *) malloc(sizeof(sm_data));
	queues[i] = data;

	// now the position is marked as used and cannot be overwritten:
	// we can unlock
	pthread_mutex_unlock(&queuesLock);

	queues[i]->callId = callId;
	queues[i]->sipCallId = sipCallId;

	// initialize event queue for specific statemachine:
	queues[i]->eventPool =
		queue_create_queue(config.core.events.dispatcher.maxEvents);

	// initialize wakeup-condition-variable for specific statemachine:
	rc = pthread_cond_init(&queues[i]->wakeUp, NULL);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX
				  "create queue: failed to initialize "
				  "wakeup-condition-variable");
		return 0;
	}
	// condition-variable always comes with a mutex lock:
	rc = pthread_mutex_init(&queues[i]->wakeUpLock, NULL);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX
				  "create queue: failed to initialize "
				  "mutex lock for condition variable");
		return 0;
	}
	// initialize mutex lock for event pool:
	rc = pthread_mutex_init(&queues[i]->poolLock, NULL);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX
				  "create queue: failed to initialize "
				  "event pool mutex lock");
		return 0;
	}
	// set the given parameter to the found position
	*pos = i;

	LOG_DEBUG(EVENT_DISP_MSG_PREFIX "created queue");
	// return true for success:
	return 1;
}

int destroy_queue(int pos) {
	int rc;						// return code

	// release queue (event pool):
	queue_dispose_queue(queues[pos]->eventPool);

	// release condition variable:
	rc = pthread_cond_destroy(&queues[pos]->wakeUp);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX "destroy queue: failed to release "
				  "wakeup condition variable");
		return 0;
	}
	// release mutex locks:
	rc = pthread_mutex_destroy(&queues[pos]->wakeUpLock);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX "destroy queue: failed to release "
				  "mutex lock for condition variable");
		return 0;
	}

	rc = pthread_mutex_destroy(&queues[pos]->poolLock);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX "destroy queue: failed to release "
				  "event pool mutex lock");
		return 0;
	}
	// release array element and remove reference:
	free(queues[pos]);
	queues[pos] = 0;

	LOG_DEBUG(EVENT_DISP_MSG_PREFIX "destroyed queue");
	// return true for success:
	return 1;
}

int find_pos_by_call_id(int callId) {
	int i;
	int rc;

	// we have to lock, because statemachine may update the call ID atm
	rc = pthread_mutex_lock(&queuesLock);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX "find pos by call ID: failed to "
				  "gain mutex lock");
		return 0;
	}

	i = 0;
	while (i < config.core.events.dispatcher.maxCalls) {
		if (queues[i]) {
			if (queues[i]->callId == callId) {
				return i;
			}
		}
		i++;
	}

	rc = pthread_mutex_unlock(&queuesLock);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX "find pos by call ID: failed to "
				  "gain mutex lock");
		return 0;
	}

	return -1;
}

int find_pos_by_sip_call_id(int sipCallId) {
	int i;
	int rc;

	// we have to lock, because statemachine may update the SIP call ID atm
	rc = pthread_mutex_lock(&queuesLock);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX
				  "find pos by SIP call ID: failed to " "gain mutex lock");
		return 0;
	}

	i = 0;
	while (i < config.core.events.dispatcher.maxCalls) {
		if (queues[i]) {
			if (queues[i]->sipCallId == sipCallId) {
				return i;
			}
		}
		i++;
	}

	rc = pthread_mutex_unlock(&queuesLock);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX
				  "find pos by SIP call ID: failed to " "gain mutex lock");
		return 0;
	}

	return -1;
}

int enqueue_event(int pos, call_trigger * param) {
	int rc;

	rc = pthread_mutex_lock(&queues[pos]->poolLock);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX
				  "enqueue event: failed to gain mutex" " lock");
		return 0;
	}

	queue_enqueue((void *) param, queues[pos]->eventPool);

	rc = pthread_mutex_unlock(&queues[pos]->poolLock);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX "enqueue event: failed to release "
				  "mutex lock");
		return 0;
	}

	return 1;
}

int wake_machine(int pos) {
	int rc;

	rc = pthread_mutex_lock(&queues[pos]->wakeUpLock);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX
				  "wake machine: failed to gain mutex" " lock");
		return 0;
	}

	rc = pthread_cond_signal(&queues[pos]->wakeUp);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX "wake machine: failed to send "
				  "condition signal");
		return 0;
	}

	rc = pthread_mutex_unlock(&queues[pos]->wakeUpLock);
	if (rc != 0) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX "wake machine: failed to release "
				  "mutex lock");
		return 0;
	}

	return 1;
}

int enqueue_and_wake(int callId, int sipCallId, call_trigger * param) {
	int res;
	int pos;

	if (callId > 0) {
		res = find_pos_by_call_id(callId);
		if (res == -1) {
			LOG_DEBUG(EVENT_DISP_MSG_PREFIX "enqueue and wake: failed to "
					  "find statemachine position to given call ID %d",
					  callId);
			return 0;
		}
	} else if (sipCallId > 0) {
		res = find_pos_by_sip_call_id(sipCallId);
		if (res == -1) {
			LOG_DEBUG(EVENT_DISP_MSG_PREFIX "enqueue and wake: failed to "
					  "find statemachine position to given SIP call ID %d",
					  sipCallId);
			return 0;
		}
	} else {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX "neither call ID nor SIP call ID "
				  "given - unable to find statemachine position");
		return 0;
	}

	pos = res;

	res = enqueue_event(pos, param);
	if (!res) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX
				  "enqueue and wake: failed to enqueue "
				  "current event (event type %d, call ID %d)",
				  param->trigger, callId);
		return 0;
	}

	res = wake_machine(pos);
	if (!res) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX "enqueue and wake: failed to wake "
				  "statemachine (call ID: %d)", callId);
		return 0;
	}

	return 1;
}

void leave_dispatch_thr_err(call_trigger * param) {
	if (param && param->params) {
		free(param->params);
	}
	if (param) {
		free(param);
	}
	thread_terminated();
}

void *dispatch(void *args) {

	int res;
	int pos;
	int callId;
	int sipCallId;
	call_trigger *param;
	sipstack_event *sipEvt;

	param = (call_trigger *) args;
	if (!param) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX "dispatch thread received no "
				  "parameters");
		leave_dispatch_thr_err(param);
		return NULL;
	}

	switch (param->trigger) {
		case GUI_MAKE_CALL:
			callId = (int) param->params[2];
			res = create_queue(&pos, callId, 0);
			if (!res) {
				LOG_DEBUG(EVENT_DISP_MSG_PREFIX
						  "create_queue(): no free position found");
				leave_dispatch_thr_err(param);
				return NULL;
			}

			queue_enqueue((void *) param, queues[pos]->eventPool);

			start_thread(sm_start, (void *) pos);

			break;
		case SIPLISTENER_RECEIVE:
			sipEvt = (sipstack_event *) param->params[0];
			if (sipEvt->type == EXOSIP_REGISTRATION_NEW
				|| sipEvt->type == EXOSIP_REGISTRATION_SUCCESS
				|| sipEvt->type == EXOSIP_REGISTRATION_FAILURE
				|| sipEvt->type == EXOSIP_REGISTRATION_REFRESHED
				|| sipEvt->type == EXOSIP_REGISTRATION_TERMINATED) {

				res = rm_receive_register_event(sipEvt);
				if (!res) {
					LOG_DEBUG(EVENT_DISP_MSG_PREFIX
							  "dispatch thread: failed "
							  "to deliver registration event to registrar "
							  "manager");
					leave_dispatch_thr_err(param);
					return NULL;
				}
			} else if (sipEvt->type == EXOSIP_CALL_INVITE) {
				sipCallId = sipEvt->callId;
				res = create_queue(&pos, 0, sipCallId);
				if (!res) {
					LOG_DEBUG(EVENT_DISP_MSG_PREFIX
							  "create_queue(): no free position found");
					leave_dispatch_thr_err(param);
					return NULL;
				}

				queue_enqueue((void *) param, queues[pos]->eventPool);

				start_thread(sm_start, (void *) pos);
			} else {
				sipCallId = sipEvt->callId;
				res = enqueue_and_wake(sipCallId, 0, param);
				if (!res) {
					LOG_DEBUG(EVENT_DISP_MSG_PREFIX "dispatch thread, "
							  "SipListener.receive: enqueue&wake failed "
							  "(event type: %d)", sipEvt->type);
					leave_dispatch_thr_err(param);
					return NULL;
				}
			}

			break;
		case GUI_END_CALL:
			callId = (int) param->params[0];
			res = enqueue_and_wake(callId, 0, param);
			if (!res) {
				LOG_DEBUG(EVENT_DISP_MSG_PREFIX "dispatch thread, "
						  "GUI.endCall: enqueue&wake failed");
				leave_dispatch_thr_err(param);
				return NULL;
			}
			break;
		case GUI_ACCEPT_CALL:
			callId = (int) param->params[0];
			res = enqueue_and_wake(callId, 0, param);
			if (!res) {
				LOG_DEBUG(EVENT_DISP_MSG_PREFIX "dispatch thread, "
						  "GUI.acceptCall: enqueue&wake failed");
				leave_dispatch_thr_err(param);
				return NULL;
			}
			break;
		case INVALID_EVENT:
			LOG_DEBUG(EVENT_DISP_MSG_PREFIX "dispatch thread, "
					  "GUI.endCall: enqueue&wake failed");
			leave_dispatch_thr_err(param);
			return NULL;
		default:
			LOG_ERROR(EVENT_DISP_MSG_PREFIX "dispatch thread: received "
					  "event which is not yet implemented");
			leave_dispatch_thr_err(param);
			return NULL;
	}

	// don't free event, because it is needed by the statemachine
	// statemachine is responsible for freeing the event and its parameters

	thread_terminated();
	return NULL;
}

int event_dispatch(event evt, void **params, int *callId) {

	call_trigger *threadParam;
	int rc;

	threadParam = (call_trigger *) malloc(sizeof(call_trigger));

	if (evt == GUI_MAKE_CALL) {
		// no callId is known - has to be generated, returned to gui and
		// passed to the new statemachine
		int accountId;
		char *callee;

		*callId = cig_generate_call_id();
		accountId = (int) params[0];
		callee = (char *) params[1];

		threadParam->params = (void **) malloc(3 * sizeof(void *));
		threadParam->params[0] = (void *) accountId;
		threadParam->params[1] =
			(void *) malloc(strlen(callee) * sizeof(char) + 1);
		strcpy(threadParam->params[1], callee);
		threadParam->params[2] = (void *) *callId;
		threadParam->trigger = evt;

	} else {
		threadParam->trigger = evt;
		threadParam->params = params;
	}

	// start dispatch thread - it does the real dispatching
	rc = start_thread(dispatch, (void *) threadParam);
	if (!rc) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "failed to start dispatch thread");
		return 0;
	}

	return 1;
}
