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
#include <core/gui_output/gui_callback_sender.h>
#include <core/events/call_status.h>

#define EVENT_DISP_MSG_PREFIX "[event dispatcher] "
#define EVT_DP_EVT_TYPES 2
#define EVT_DP_QUEUE_SIZE 20
#define EVT_DP_GUI_EVTS_ID 0
#define EVT_DP_SIP_EVTS_ID 1

sm_data **queues;
pthread_mutex_t queuesLock;

typedef struct {
	int doShutdown;
	call_trigger *trigger;
} ed_msg;

typedef struct {
	queue msgPool;
	pthread_mutex_t lock;
	pthread_mutex_t wakeUpLock;
	pthread_cond_t wakeUp;
} ed_queue;

ed_queue *ed_queues;

int create_queue(int *pos, int accountId, int callId, int sipCallId) {
	int i;						// counter
	int rc;						// return code

	// lock write access to the queues array
	pthread_mutex_lock(&queuesLock);

	i = 0;
	// find free position in array:
	while (queues[i]) {
		i++;

		if (i == config.core.events.dispatcher.maxCalls) {
			LOG_ERROR(EVENT_DISP_MSG_PREFIX "too many simultanous calls, "
					  "maximum is %d",
					  config.core.events.dispatcher.maxCalls);
			if (callId > 0) {
				rc = go_show_user_event(accountId, "ERROR",
										"Error making call",
										"Too many simultanous calls.",
										"You exceeded the limit of calls "
										"the Core is able to process "
										"according to its configuration.");
			} else {
				rc = go_show_user_event(accountId, "ERROR",
										"Error accepting incoming call",
										"Too many simultanous calls.",
										"You exceeded the limit of calls "
										"the Core is able to process "
										"according to its configuration.");
			}
			if (!rc) {
				LOG_ERROR(EVENT_DISP_MSG_PREFIX "failed to inform the "
						  "GUI");
			}

			if (callId > 0) {
				rc = go_change_call_status(callId,
										   call_status_to_str
										   (CS_TERMINATED));
				if (!rc) {
					LOG_ERROR(EVENT_DISP_MSG_PREFIX "failed to inform "
							  "the GUI");
				}
			}
			// unlock
			pthread_mutex_unlock(&queuesLock);
			return 0;
		}
	}

	LOG_DEBUG(EVENT_DISP_MSG_PREFIX "create queue: position found: %d\n",
			  i);

	// reserve memory for statemachine-specific data
	queues[i] = (sm_data *) malloc(sizeof(sm_data));

	// now the position is marked as used and cannot be overwritten:
	// we can unlock
	rc = pthread_mutex_unlock(&queuesLock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "create queue: failed to release "
				  "lock, error: %d\n", rc);
		return 0;
	}

	queues[i]->callId = callId;
	queues[i]->sipCallId = sipCallId;
	queues[i]->doShutdown = 0;

	// initialize event queue for specific statemachine:
	queues[i]->eventPool =
		queue_create_queue(config.core.events.dispatcher.maxEvents);
	if (!queues[i]->eventPool) {
		LOG_FAILURE(EVENT_DISP_MSG_PREFIX "create queue: failed to "
					"create event pool (not enough memory?)");
		return 0;
	}
	// initialize wakeup-condition-variable for specific statemachine:
	rc = pthread_cond_init(&queues[i]->wakeUp, NULL);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX
				  "create queue: failed to initialize "
				  "wakeup-condition-variable");
		return 0;
	}
	// condition-variable always comes with a mutex lock:
	rc = pthread_mutex_init(&queues[i]->wakeUpLock, NULL);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX
				  "create queue: failed to initialize "
				  "mutex lock for condition variable");
		return 0;
	}
	// initialize mutex lock for event pool:
	rc = pthread_mutex_init(&queues[i]->poolLock, NULL);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX
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
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "destroy queue: failed to release "
				  "wakeup condition variable");
		return 0;
	}
	// release mutex locks:
	rc = pthread_mutex_destroy(&queues[pos]->wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "destroy queue: failed to release "
				  "mutex lock for condition variable");
		return 0;
	}

	rc = pthread_mutex_destroy(&queues[pos]->poolLock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "destroy queue: failed to release "
				  "event pool mutex lock");
		return 0;
	}
	// release array element and remove reference:
	free(queues[pos]);
	queues[pos] = NULL;

	LOG_DEBUG(EVENT_DISP_MSG_PREFIX "destroyed queue");
	// return true for success:
	return 1;
}

int find_pos_by_call_id(int callId) {
	int i;
	int rc;
	int res;

	LOG_DEBUG(EVENT_DISP_MSG_PREFIX "entering find_pos_by_call_id()");

	res = -1;

	// we have to lock, because statemachine may update the call ID atm
	rc = pthread_mutex_lock(&queuesLock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "find pos by call ID: failed to "
				  "gain mutex lock");
		return 0;
	}

	i = 0;
	while (i < config.core.events.dispatcher.maxCalls) {
		if (queues[i]) {
			if (queues[i]->callId == callId) {
				res = i;
				break;
			}
		}
		i++;
	}

	rc = pthread_mutex_unlock(&queuesLock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "find pos by call ID: failed to "
				  "release mutex lock");
		return 0;
	}

	if (res == -1) {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX "leaving find_pos_by_call_id() "
				  "- not found");
	} else {
		LOG_DEBUG(EVENT_DISP_MSG_PREFIX "leaving find_pos_by_call_id() "
				  "- found position");
	}
	return res;
}

int find_pos_by_sip_call_id(int sipCallId) {
	int i;
	int rc;
	int res;

	LOG_DEBUG(EVENT_DISP_MSG_PREFIX "entering find_pos_by_sip_call_id()");

	res = -1;

	// we have to lock, because statemachine may update the SIP call ID atm
	rc = pthread_mutex_lock(&queuesLock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "find pos by SIP call ID: failed "
				  "to gain mutex lock");
		return 0;
	}

	i = 0;
	while (i < config.core.events.dispatcher.maxCalls) {
		if (queues[i]) {
			if (queues[i]->sipCallId == sipCallId) {
				res = i;
				break;
			}
		}
		i++;
	}

	rc = pthread_mutex_unlock(&queuesLock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "find pos by SIP call ID: failed "
				  "to release mutex lock");
		return 0;
	}

	LOG_DEBUG(EVENT_DISP_MSG_PREFIX "leaving find_pos_by_sip_call_id()");

	return res;
}

int enqueue_event(int pos, call_trigger * param) {
	int rc;

	LOG_DEBUG(EVENT_DISP_MSG_PREFIX "entering enqueue_event()");

	rc = pthread_mutex_lock(&queues[pos]->poolLock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "enqueue event: failed to gain "
				  "mutex lock");
		return 0;
	}

	LOG_DEBUG(EVENT_DISP_MSG_PREFIX "trying to enqueue current event");

	rc = queue_enqueue((void *) param, queues[pos]->eventPool);
	if (!rc) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "enqueue event: failed to "
				  "enqueue event (queue full?)");

		// only inform GUI if current event is coming from the GUI
		if (param->trigger == GUI_MAKE_CALL
			|| param->trigger == GUI_ACCEPT_CALL
			|| param->trigger == GUI_END_CALL) {

			rc = go_show_user_event(0, "ERROR",
									"Error performing call control",
									"Failed to perform command.",
									"There were no free ressources to "
									"store your request within the "
									"Core.");
			if (!rc) {
				LOG_ERROR(EVENT_DISP_MSG_PREFIX "failed to inform the "
						  "GUI");
			}
		}

		pthread_mutex_unlock(&queues[pos]->poolLock);
		return 0;
	}

	LOG_DEBUG(EVENT_DISP_MSG_PREFIX "finished queue_enqueue()");

	rc = pthread_mutex_unlock(&queues[pos]->poolLock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "enqueue event: failed to release "
				  "mutex lock");
		return 0;
	}

	LOG_DEBUG(EVENT_DISP_MSG_PREFIX "leaving enqueue_event()");

	return 1;
}

int wake_machine(int pos) {
	int rc;

	LOG_DEBUG(EVENT_DISP_MSG_PREFIX "entering wake_machine()");

	rc = pthread_mutex_lock(&queues[pos]->wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "wake machine: failed to gain "
				  "mutex lock");
		return 0;
	}

	LOG_DEBUG(EVENT_DISP_MSG_PREFIX "trying to send cond_signal");

	rc = pthread_cond_signal(&queues[pos]->wakeUp);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "wake machine: failed to send "
				  "condition signal");
		return 0;
	}

	LOG_DEBUG(EVENT_DISP_MSG_PREFIX "finished sending cond_signal");

	rc = pthread_mutex_unlock(&queues[pos]->wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "wake machine: failed to release "
				  "mutex lock");
		return 0;
	}

	LOG_DEBUG(EVENT_DISP_MSG_PREFIX "leaving wake_machine()");

	return 1;
}

int enqueue_and_wake(int callId, int sipCallId, call_trigger * param) {
	int res;
	int pos;

	if (callId > 0) {
		res = find_pos_by_call_id(callId);
		if (res == -1) {
			LOG_ERROR(EVENT_DISP_MSG_PREFIX "enqueue and wake: failed to "
					  "find statemachine position to given call ID %d",
					  callId);

			res = go_show_user_event(0, "ERROR",
									 "Error performing call control",
									 "Failed to perform either termination "
									 "or acception of a call.",
									 "The given call ID is no longer valid "
									 "which prevents processing your "
									 "command.");
			if (!res) {
				LOG_ERROR(EVENT_DISP_MSG_PREFIX "failed to inform the "
						  "GUI");
			}
			return 0;
		}
	} else if (sipCallId > 0) {
		res = find_pos_by_sip_call_id(sipCallId);
		if (res == -1) {
			LOG_DEBUG(EVENT_DISP_MSG_PREFIX "enqueue and wake: failed to "
					  "find statemachine position to given SIP call ID %d",
					  sipCallId);
			// don't inform GUI because that is probably an unimportant event
			return 0;
		}
	} else {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "neither call ID nor SIP call ID "
				  "given - unable to find statemachine position");
		// don't inform GUI because this means bad usage of this function
		return 0;
	}

	pos = res;

	res = enqueue_event(pos, param);
	if (!res) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX
				  "enqueue and wake: failed to enqueue "
				  "current event (event type %d, call ID %d)",
				  param->trigger, callId);
		// don't inform GUI because this is already done by enqueue_event
		return 0;
	}

	res = wake_machine(pos);
	if (!res) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "enqueue and wake: failed to wake "
				  "statemachine (call ID: %d)", callId);
		return 0;
	}

	return 1;
}

void dispatch_thr_err(call_trigger * param) {
	if (param && param->params) {
		free(param->params);
	}
	if (param) {
		free(param);
	}
}

void *dispatch(void *args) {

	int res;
	int pos;
	int callId;
	int sipCallId;
	int accountId;
	int finished;
	int id;
	call_trigger *param;
	sipstack_event *sipEvt;
	ed_msg *edMsg;

	id = (int) args;
	finished = 0;

	while (!finished) {
		while (!queue_is_empty(ed_queues[id].msgPool)) {
			edMsg =
				(ed_msg *) queue_front_and_dequeue(ed_queues[id].msgPool);
			if (!edMsg) {
				LOG_ERROR(EVENT_DISP_MSG_PREFIX "dispatch thread "
						  "received no parameters");
				// don't inform GUI because this means bad usage of this thread
				thread_terminated();
				return NULL;
			}
			if (edMsg->doShutdown) {
				finished = 1;
				break;
			}
			param = edMsg->trigger;
			if (!param) {
				LOG_ERROR(EVENT_DISP_MSG_PREFIX "dispatch thread received "
						  "no parameters");
				// don't inform GUI because this means bad usage of this thread
				thread_terminated();
				return NULL;
			}
			free(edMsg);

			LOG_DEBUG(EVENT_DISP_MSG_PREFIX "entering dispatch thread "
					  "event type is %d", param->trigger);

			switch (param->trigger) {
				case GUI_MAKE_CALL:
					accountId = (int) param->params[0];
					callId = (int) param->params[2];
					res = create_queue(&pos, accountId, callId, 0);
					if (!res) {
						LOG_ERROR(EVENT_DISP_MSG_PREFIX "create_queue(): "
								  "no free position found");
						// don't inform GUI because this is already done by 
						// create_queue
						dispatch_thr_err(param);
						continue;
					}

					res = enqueue_event(pos, param);
					if (!res) {
						LOG_ERROR(EVENT_DISP_MSG_PREFIX "enqueue_event() "
								  "failed");
						// don't inform GUI because this is already done by 
						// enqueue_event
						dispatch_thr_err(param);
						continue;
					}

					start_thread(sm_start, (void *) pos);

					break;
				case SIPLISTENER_RECEIVE:
					sipEvt = (sipstack_event *) param->params[0];
					if (sipEvt->type == EXOSIP_REGISTRATION_NEW
						|| sipEvt->type == EXOSIP_REGISTRATION_SUCCESS
						|| sipEvt->type == EXOSIP_REGISTRATION_FAILURE
						|| sipEvt->type == EXOSIP_REGISTRATION_REFRESHED
						|| sipEvt->type ==
						EXOSIP_REGISTRATION_TERMINATED) {

						res = rm_receive_register_event(sipEvt);
						if (!res) {
							LOG_ERROR(EVENT_DISP_MSG_PREFIX "dispatch "
									  "thread: failed to deliver "
									  "registration event to registrar "
									  "manager");
							dispatch_thr_err(param);
							continue;
						}
					} else if (sipEvt->type == EXOSIP_CALL_INVITE) {
						sipCallId = sipEvt->callId;
						res = create_queue(&pos, -1, 0, sipCallId);
						if (!res) {
							LOG_ERROR(EVENT_DISP_MSG_PREFIX
									  "create_queue(): no free position "
									  "found");
							// don't inform GUI because this is already done 
							// by create_queue
							dispatch_thr_err(param);
							continue;
						}

						res = enqueue_event(pos, param);
						if (!res) {
							LOG_ERROR(EVENT_DISP_MSG_PREFIX
									  "enqueue_event() failed");
							// don't inform GUI because this is already done 
							// by enqueue_event
							dispatch_thr_err(param);
							continue;
						}

						start_thread(sm_start, (void *) pos);
					} else {
						sipCallId = sipEvt->callId;
						res = enqueue_and_wake(0, sipCallId, param);
						if (!res) {
							LOG_DEBUG(EVENT_DISP_MSG_PREFIX "dispatch "
									  "thread, SipListener.receive: "
									  "enqueue&wake failed (event type: "
									  "%d)", sipEvt->type);
							// don't inform GUI because this is already done
							// by enqueue_and_wake
							dispatch_thr_err(param);
							continue;
						}
					}

					break;
				case GUI_END_CALL:
					callId = (int) param->params[0];
					res = enqueue_and_wake(callId, 0, param);
					if (!res) {
						LOG_DEBUG(EVENT_DISP_MSG_PREFIX "dispatch thread, "
								  "GUI.endCall: enqueue&wake failed");
						// don't inform GUI because this is already done by 
						// enqueue_and_wake
						dispatch_thr_err(param);
						continue;
					}
					break;
				case GUI_ACCEPT_CALL:
					callId = (int) param->params[0];
					res = enqueue_and_wake(callId, 0, param);
					if (!res) {
						LOG_DEBUG(EVENT_DISP_MSG_PREFIX "dispatch thread, "
								  "GUI.acceptCall: enqueue&wake failed");
						// don't inform GUI because this is already done by 
						// enqueue_and_wake
						dispatch_thr_err(param);
						continue;
					}
					break;
				case INVALID_EVENT:
					LOG_ERROR(EVENT_DISP_MSG_PREFIX "dispatch thread: "
							  "received invalid event");
					// don't inform GUI because this is caused by bad usage 
					// of this thread
					dispatch_thr_err(param);
					thread_terminated();
					return NULL;
				default:
					LOG_ERROR(EVENT_DISP_MSG_PREFIX "dispatch thread: "
							  "received event which is not yet implemented");
					// don't inform GUI because this is caused by bad usage 
					// of this thread
					dispatch_thr_err(param);
					thread_terminated();
					return NULL;
			}					// switch
			// don't free event, because it is needed by the statemachine / the
			// registrar manager
			// they are responsible for freeing the event and its parameters

		}						// while !queue_is_empty

		res = pthread_mutex_lock(&ed_queues[id].wakeUpLock);
		if (res != 0) {
			LOG_ERROR(EVENT_DISP_MSG_PREFIX "dispatch thread "
					  "failed to gain lock");
			thread_terminated();
			return NULL;
		}

		res = pthread_cond_wait(&ed_queues[id].wakeUp,
								&ed_queues[id].wakeUpLock);
		if (res != 0) {
			LOG_ERROR(EVENT_DISP_MSG_PREFIX "dispatch thread "
					  "failed to wait for signal");
			thread_terminated();
			return NULL;
		}

		res = pthread_mutex_unlock(&ed_queues[id].wakeUpLock);
		if (res != 0) {
			LOG_ERROR(EVENT_DISP_MSG_PREFIX "dispatch thread "
					  "failed to release lock");
			thread_terminated();
			return NULL;
		}
	}							// while !finished

	thread_terminated();
	return NULL;
}

int event_dispatch(event evt, void **params, int *callId) {

	ed_msg *edMsg;
	int rc;
	int id;

	if (evt == GUI_MAKE_CALL || evt == GUI_ACCEPT_CALL
		|| evt == GUI_END_CALL) {
		id = EVT_DP_GUI_EVTS_ID;
	} else {
		id = EVT_DP_SIP_EVTS_ID;
	}

	rc = pthread_mutex_lock(&ed_queues[id].lock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "event_dispatch: failed to gain "
				  "lock, error: %d", rc);
		return 0;
	}

	edMsg = (ed_msg *) malloc(sizeof(ed_msg));
	edMsg->doShutdown = 0;
	edMsg->trigger = (call_trigger *) malloc(sizeof(call_trigger));

	if (evt == GUI_MAKE_CALL) {
		// no callId is known - has to be generated, returned to gui and
		// passed to the new statemachine
		int accountId;
		char *callee;

		accountId = (int) params[0];
		callee = (char *) params[1];

		if (!callee || strlen(callee) == 0) {
			LOG_ERROR(EVENT_DISP_MSG_PREFIX "GUI.makeCall: callee is "
					  "missing");
			rc = go_show_user_event(accountId, "ERROR",
									"Error making call",
									"Callee is missing.",
									"Please specify whom you want to call.");
			if (!rc) {
				LOG_ERROR(EVENT_DISP_MSG_PREFIX "failed to inform the "
						  "GUI");
			}
			pthread_mutex_unlock(&ed_queues[EVT_DP_GUI_EVTS_ID].lock);
			return 0;
		}

		*callId = cig_generate_call_id();

		edMsg->trigger->params = (void **) malloc(3 * sizeof(void *));
		edMsg->trigger->params[0] = (void *) accountId;
		edMsg->trigger->params[1] =
			(void *) malloc(strlen(callee) * sizeof(char) + 1);
		strcpy(edMsg->trigger->params[1], callee);
		edMsg->trigger->params[2] = (void *) *callId;
		edMsg->trigger->trigger = evt;

	} else {
		edMsg->trigger->trigger = evt;
		edMsg->trigger->params = params;
	}
/*
	// start dispatch thread - it does the real dispatching
	rc = start_thread(dispatch, (void *) threadParam);
	if (!rc) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "failed to start dispatch thread");
		return 0;
	}
*/

	rc = queue_enqueue((void *) edMsg, ed_queues[id].msgPool);
	if (!rc) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "go_set_micro_volume_cb "
				  "failed to enqueue message: queue is full");
		pthread_mutex_unlock(&ed_queues[id].lock);
		return 0;
	}

	rc = pthread_mutex_lock(&ed_queues[id].wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "go_set_micro_volume_cb: "
				  "failed to gain lock");
		return 0;
	}

	rc = pthread_cond_signal(&ed_queues[id].wakeUp);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "go_set_micro_volume_cb: "
				  "failed to send condition signal");
		return 0;
	}

	rc = pthread_mutex_unlock(&ed_queues[id].wakeUpLock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "go_set_micro_volume_cb: "
				  "failed to release lock");
		return 0;
	}

	rc = pthread_mutex_unlock(&ed_queues[id].lock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "event_dispatch: failed to "
				  "release lock, error: %d", rc);
		return 0;
	}

	return 1;
}

int ed_shutdown_all() {

	int i;
	int rc;

	LOG_DEBUG(EVENT_DISP_MSG_PREFIX "now trying to shut down all "
			  "statemachines");

	// we have to lock to prevent dispatching atm:
	rc = pthread_mutex_lock(&queuesLock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "shut down all statemachines: "
				  "failed to gain mutex lock");
		return 0;
	}

	for (i = 0; i < config.core.events.dispatcher.maxCalls; i++) {
		if (queues[i]) {
			queues[i]->doShutdown = 1;
			rc = wake_machine(i);
			if (!rc) {
				LOG_ERROR(EVENT_DISP_MSG_PREFIX "shut down all "
						  "statemachines: failed to wake state machine "
						  "(pos %d)", i);
			}
		}
	}

	// now, statemachines don't care if they receive new events:
	rc = pthread_mutex_unlock(&queuesLock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "shut down all statemachines: "
				  "failed to release mutex lock");
		return 0;
	}

	return 1;
}

int ed_init() {
	int rc;
	int i;

	rc = pthread_mutex_init(&queuesLock, NULL);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "failed to initialize mutex lock");
		return 0;
	}

	queues =
		(sm_data **) calloc(config.core.events.dispatcher.maxCalls,
							sizeof(sm_data *));

	ed_queues = (ed_queue *) malloc(EVT_DP_EVT_TYPES * sizeof(ed_queue));

	for (i = 0; i < EVT_DP_EVT_TYPES; i++) {
		ed_queues[i].msgPool = queue_create_queue(EVT_DP_QUEUE_SIZE);
		if (!ed_queues[i].msgPool) {
			LOG_ERROR(EVENT_DISP_MSG_PREFIX "failed to create "
					  "message pool");
			return 0;
		}

		rc = pthread_mutex_init(&ed_queues[i].lock, NULL);
		if (rc != 0) {
			LOG_ERROR(EVENT_DISP_MSG_PREFIX "failed to initialize "
					  "mutex lock");
			return 0;
		}

		rc = pthread_mutex_init(&ed_queues[i].wakeUpLock, NULL);
		if (rc != 0) {
			LOG_ERROR(EVENT_DISP_MSG_PREFIX "failed to initialize "
					  "mutex lock");
			return 0;
		}

		rc = pthread_cond_init(&ed_queues[i].wakeUp, NULL);
		if (rc != 0) {
			LOG_ERROR(EVENT_DISP_MSG_PREFIX "failed to initialize "
					  "wakeup-condition-variable");
			return 0;
		}
	}

	rc = start_thread(dispatch, (void *) EVT_DP_GUI_EVTS_ID);
	if (!rc) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX
				  "failed to start dispatch thread for GUI events");
		return 0;
	}

	rc = start_thread(dispatch, (void *) EVT_DP_SIP_EVTS_ID);
	if (!rc) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX
				  "failed to start dispatch thread for SIP events");
		return 0;
	}

	return 1;
}

int ed_destroy() {
	int rc;
	int i;

	rc = pthread_mutex_destroy(&queuesLock);
	if (rc != 0) {
		LOG_ERROR(EVENT_DISP_MSG_PREFIX "failed to release mutex lock");
		return 0;
	}

	free(queues);

	for (i = 0; i < EVT_DP_EVT_TYPES; i++) {
		queue_dispose_queue(ed_queues[i].msgPool);

		rc = pthread_mutex_destroy(&ed_queues[i].lock);
		if (rc != 0) {
			LOG_ERROR(EVENT_DISP_MSG_PREFIX "failed to release mutex "
					  "lock");
			return 0;
		}

		rc = pthread_mutex_destroy(&ed_queues[i].wakeUpLock);
		if (rc != 0) {
			LOG_ERROR(EVENT_DISP_MSG_PREFIX "failed to release mutex "
					  "lock");
			return 0;
		}

		rc = pthread_cond_destroy(&ed_queues[i].wakeUp);
		if (rc != 0) {
			LOG_ERROR(EVENT_DISP_MSG_PREFIX "failed to release "
					  "wakeup-condition-variable");
			return 0;
		}
	}

	free(ed_queues);

	return 1;
}
