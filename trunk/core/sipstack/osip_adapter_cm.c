#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

#include "osip_adapter_cm.h"

/* list of active calls */
sipstack_call calls[SIPSTACK_MAX_CALLS];

/* count of active calls */
int callCounter = 0;

sipstack_event cm_map_event(int callId, eXosip_event_t * event) {

	/* sip stack event which will be returned */
	sipstack_event sse;

	/* sip stack call */
	sipstack_call call;

	/* check whether a call with this call id already exists */
	if (callId > 0 && cm_call_exists(callId)) {
		/* get call with this call id */
		int callIndex = cm_get_call_index_by_call_id(callId);
		/* set sip call id if not set */
		if (calls[callIndex].sipCallId < 1) {
			calls[callIndex].sipCallId = event->cid;
		}
		/* set sip dialog id if not set */
		if (calls[callIndex].sipDialogId < 1) {
			calls[callIndex].sipDialogId = event->did;
		}
		/* add transaction id if not in queue */
		if (!queue_is_element
			((void *) event->tid, calls[callIndex].sipTransactionIds)) {
			/* transaction id is not in queue, so enqueue it */
			queue_enqueue((void *) event->tid,
						  calls[callIndex].sipTransactionIds);
		}
	} else {
		/* build new call */
		queue transactionIds =
			queue_create_queue(SIPSTACK_MAX_TRANSACTIONS);
		queue_enqueue((void *) event->tid, transactionIds);
		/* set transaction type (invite or non-invite) */
		sipstack_transaction_type type = NON_INVITE;
		if (event->type == EXOSIP_CALL_INVITE) {
			type = INVITE;
		}
		call = cm_build_sipstack_call(callId, event->cid, event->did,
									  transactionIds, type);
		cm_add_call(call);
	}
	/* get response status code */
	sse.statusCode = event->response->status_code;
	/* get response message */
	sse.message = event->textinfo;
	/* get dialog id */
	sse.callId = callId;
	/* is event an acknowledgment */
	if (event->ack != NULL) {
		sse.ack = 1;
	} else {
		sse.ack = 0;
	}

	/* return event */
	return sse;
}

/**
 * Check whether a call with this call id exists in the call list.
 *
 * @param callId call id
 */
int cm_call_exists(int callId) {
	int i;
	for (i = 0; i < callCounter; i++) {
		if (calls[i].callId == callId) {
			return 1;
		}
	}
	return 0;
}

/**
 * Return list index of call with this call id.
 *
 * @param callId call id
 */
int cm_get_call_index_by_call_id(int callId) {
	int i;
	for (i = 0; i < callCounter; i++) {
		if (calls[i].callId == callId) {
			return i;
		}
	}
	/* no call with this call id */
	log_message(LOG_ERROR,
				SIPSTACK_MSG_PREFIX
				"No call with this call id (call id = %i)\n", callId);
	return -1;
}

/**
 * Return call with this call id.
 *
 * @param callId call id
 */
sipstack_call cm_get_call_by_call_id(int callId) {
	int i;
	for (i = 0; i < callCounter; i++) {
		if (calls[i].callId == callId) {
			return calls[i];
		}
	}
	/* no call with this call id */
	log_message(LOG_ERROR,
				SIPSTACK_MSG_PREFIX
				"No call with this call id (call id = %i)\n", callId);
	/* return empty call */
	return cm_build_sipstack_call(-1, -1, -1, NULL, 0);
}

/**
 * Return call id of call with this sip dialog id.
 *
 * @param sipDialogId a sip dialog id
 */
int cm_get_call_id_by_sip_dialog_id(int sipDialogId) {
	int i;
	for (i = 0; i < callCounter; i++) {
		if (calls[i].sipDialogId == sipDialogId) {
			return calls[i].callId;
		}
	}
	return -1;
}

/**
 * Return call id of call with this sip transaction id.
 *
 * @param sipDialogId a sip dialog id
 */
int cm_get_call_id_by_sip_transaction_id(int sipTransactionId) {
	int i;
	for (i = 0; i < callCounter; i++) {
		if (queue_is_element
			((void *) sipTransactionId, calls[i].sipTransactionIds)) {
			return calls[i].callId;
		}
	}
	return -1;
}

int cm_get_call_id_by_sip_call_id(int sipCallId) {
	int i;
	for (i = 0; i < callCounter; i++) {
		if (calls[i].sipCallId == sipCallId) {
			return calls[i].callId;
		}
	}
	return -1;
}

int cm_get_sip_dialog_id_by_call_id(int callId) {

	int index = cm_get_call_index_by_call_id(callId);
	if (index < 0) {
		return -1;
	}
	return calls[index].sipDialogId;

}

int cm_get_sip_transaction_id_by_call_id(int callId) {
	int index = cm_get_call_index_by_call_id(callId);
	if (index < 0) {
		return -1;
	}
	if (!queue_is_empty(calls[index].sipTransactionIds)) {
		return (int) queue_front(calls[index].sipTransactionIds);
	}
	return -1;
}

int cm_get_sip_call_id_by_call_id(int callId) {
	int index = cm_get_call_index_by_call_id(callId);
	if (index < 0) {
		return -1;
	}
	return calls[index].sipCallId;
}

int cm_terminate_current_transaction(int callId) {
	if (cm_call_exists(callId)) {
		int callIndex = cm_get_call_index_by_call_id(callId);
		if (!queue_is_empty(calls[callIndex].sipTransactionIds)) {
			queue_dequeue(calls[callIndex].sipTransactionIds);
			return 0;
		}
	}
	return -1;
}

sipstack_call cm_build_sipstack_call(int callId, int sipCallId,
									 int sipDialogId,
									 queue sipTransactionIds,
									 sipstack_transaction_type type) {
	sipstack_call call;
	call.callId = callId;
	call.sipCallId = sipCallId;
	call.sipDialogId = sipDialogId;
	call.sipTransactionIds = sipTransactionIds;
	call.type = type;

	return call;
}

int cm_add_call(sipstack_call call) {
	/* add call */
	calls[callCounter] = call;
	callCounter++;
}

int cm_update_call(sipstack_call call) {
	int i = cm_get_call_index_by_call_id(call.callId);
	if (i > -1) {
		calls[i] = call;
	} else {
		return -1;
	}
	return 0;
}
