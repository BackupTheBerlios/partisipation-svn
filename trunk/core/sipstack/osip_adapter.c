#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

#include <util/thread_management.h>

#include <eXosip2/eXosip.h>
#include <osip2/osip.h>
#include <osipparser2/osip_parser.h>

#include "sipstack/sip_stack_interface.h"
#include "sipstack/sip_listener_interface.h"
#include "sipstack/osip_adapter_cm.h"

int listenerIsActive = 0;

void *sip_listener(void *args) {
	while (listenerIsActive == 1) {
		sipstack_event event = sipstack_receive_event(1);

		/* dispatch events */
		if (event.statusCode == 200) {
			sip_listener_connect_call(event.callId);
		} else {
			sip_listener_throw(event.callId, event.statusCode);
		}
	}
	int rc = thread_terminated();
	return NULL;
}

/* the following methods are part of the sip stack API */

int sipstack_init(int port) {

	/* used to get return codes */
	int rc;

	/* read config */
	rc = cr_init("../cfg/core_config.xml");
	if (rc == 0) {
		fprintf(stderr,
				SIPSTACK_MSG_PREFIX
				"Config reader could not be initalized.\n");
	}

	/* init logger */
	rc = logger_init();
	if (rc == 0) {
		fprintf(stderr,
				SIPSTACK_MSG_PREFIX "Logging could not be initialized.\n");
	}

	/* initialize trace */
	TRACE_INITIALIZE(6, stdout);

	/* initialize eXosip */
	rc = eXosip_init();
	if (rc != 0) {
		/* initialization of eXosip failed */
		/* print error message */
		log_message(LOG_ERROR,
					SIPSTACK_MSG_PREFIX "Could not initialize eXosip.");
		/* return error code */
		return -1;
	}

	/* listen to given port on every interface, use udp */
	rc = eXosip_listen_addr(IPPROTO_UDP, INADDR_ANY, port, AF_INET, 0);
	if (rc != 0) {
		/* shutdown sip stack */
		eXosip_quit();
		/* print error message */
		log_message(LOG_ERROR,
					SIPSTACK_MSG_PREFIX
					"Could not initialize transport layer.");
		/* return error code */
		return -1;
	} else {
		log_message(LOG_DEBUG,
					SIPSTACK_MSG_PREFIX "Transport layer initialized.");
	}

	/* init thread management for the sip listener thread */
	rc = tm_init();
	if (rc == 0) {
		log_message(LOG_ERROR,
					SIPSTACK_MSG_PREFIX
					"Thread management could not be initialized.");
	}
	/* start the sip listener thread */
	listenerIsActive = 1;
	rc = start_thread(sip_listener, NULL);
	if (rc == 0) {
		log_message(LOG_ERROR,
					SIPSTACK_MSG_PREFIX
					"Sip listener thread could not be started");
	} else {
		log_message(LOG_DEBUG,
					SIPSTACK_MSG_PREFIX
					"Sip listener thread started: id = %i", rc);
	}

	/* return success code */
	return 0;
}

void sipstack_quit() {

	/* used to get return codes */
	int rc;

	/* shut down sip listener */
	listenerIsActive = 0;

	/* wait for sip listener thread to terminate */
	sleep(1);

	/* shut down thread management */
	rc = tm_destroy(0);
	if (rc == 0) {
		log_message(LOG_ERROR,
					SIPSTACK_MSG_PREFIX
					"Thread management could not be released.");
	}
	log_message(LOG_DEBUG,
				SIPSTACK_MSG_PREFIX "Sip listener is shut down.");

	/* shut down eXosip */
	eXosip_quit();
	log_message(LOG_DEBUG, SIPSTACK_MSG_PREFIX "eXosip is shut down.");

	/* shutdown logger */
	rc = logger_destroy();
	if (rc == 0) {
		fprintf(stderr,
				SIPSTACK_MSG_PREFIX "logging could not be shut down.");
	}

	rc = cr_destroy();
	if (rc == 0) {
		fprintf(stderr,
				SIPSTACK_MSG_PREFIX
				"config reader could not be released.");
	}
}

sipstack_event sipstack_receive_event(int timeout) {

	/* response */
	eXosip_event_t *je;

	/* sip stack event which will be returned */
	sipstack_event sse;

	/* wait for response in seconds */
	je = eXosip_event_wait(timeout, 0);

	/* initiate some automatic actions (e.g. react on 3xx, 401/407) */
	//eXosip_automatic_action();

	if (je == NULL) {
		/* no response received in time */
		/* return error code */
		sse.statusCode = -1;
		return sse;
	} else {
		/* if event is not part of a registration */
		if (je->rid == 0) {
			/* get call id for mapping */
			int callId;
			/* check if event has a sip call or dialog id */
			if (je->cid > 0) {
				callId = cm_get_call_id_by_sip_call_id(je->cid);
				if (callId < 1) {
					fprintf(stderr,
							"No call id for sip call id of received event.(dialog id = %i)\n",
							je->cid);
				}
			} else if (je->did > 0) {
				callId = cm_get_call_id_by_sip_dialog_id(je->did);
				if (callId < 1) {
					fprintf(stderr,
							"No call id for sip dialog id of received event.(dialog id = %i)\n",
							je->did);
				}
			}
			/* add/edit call and get sip stack event */
			sse = cm_map_event(callId, je);

			sipstack_call call = cm_get_call_by_call_id(callId);

			/* terminate transaction if neccessary */
			if (call.type == INVITE && sse.ack) {
				cm_terminate_current_transaction(callId);
			} else if (call.type == NON_INVITE && sse.statusCode == 200) {
				cm_terminate_current_transaction(callId);
			}
		} else {
			/* handle registration events */

			/* do not add call to call list but get sip stack event */
			sse = cm_map_event(0, je);
		}
		/* return sip stack event */
		return sse;
	}
}

int sipstack_send_register(char *const identity, char *const registrar,
						   int expire) {

	/* message which is build in this methode */
	osip_message_t *reg = NULL;

	/* registration id */
	int id;

	/* return value */
	int i;

	/* lock sip stack to avoid conflicts */
	eXosip_lock();
	/* build REGISTER message */
	id = eXosip_register_build_initial_register(identity, registrar, NULL,
												expire, &reg);

	if (id < 0) {
		/* building of REGISTER failed */
		/* unlock sip stack for further use */
		eXosip_unlock();
		/* return error code */
		return -1;
	}

	/* set some properties of message */
	osip_message_set_supported(reg, "100rel");
	osip_message_set_supported(reg, "path");

	/* send REGISTER message */
	i = eXosip_register_send_register(id, reg);
	/* unlock sip stack for further use */
	eXosip_unlock();

	if (i < 0) {
		/* sending of REGISTER message failed */
		/* return error code */
		return -1;
	}

	/* return registration id */
	return id;
}

int sipstack_send_unregister(int id) {

	/* message which is build in this methode */
	osip_message_t *reg = NULL;
	int i;

	/* lock sip stack to avoid conflicts */
	eXosip_lock();

	/* build REGISTER message */
	i = eXosip_register_build_register(id, 0, &reg);

	if (i < 0) {
		/* building of REGISTER failed */
		/* unlock sip stack for further use */
		eXosip_unlock();
		/* return error code */
		return -1;
	}

	/* send REGISTER message */
	i = eXosip_register_send_register(id, reg);
	/* unlock sip stack for further use */
	eXosip_unlock();

	if (i < 0) {
		/* sending of REGISTER message failed */
		/* return error code */
		return -1;
	}

	/* return success code */
	return 0;
}

int sipstack_send_update_register(int id, int expire) {

	/* message which is build in this methode */
	osip_message_t *reg = NULL;

	int i;

	/* lock sip stack to avoid conflicts */
	eXosip_lock();

	/* build REGISTER message */
	i = eXosip_register_build_register(id, expire, &reg);

	if (i < 0) {
		/* building of REGISTER failed */

		/* unlock sip stack for further use */
		eXosip_unlock();

		/* return error code */
		return -1;
	}
	/* send REGISTER message */
	i = eXosip_register_send_register(id, reg);

	/* unlock sip stack for further use */
	eXosip_unlock();
	if (i < 0) {
		/* sending REGISTER message failed */
		/* return error code */
		return -1;
	}

	/* return success code */
	return 0;
}

int sipstack_send_invite(int callId, char *to, char *from, char *subject) {

	/* message which is build in this methode */
	osip_message_t *invite;

	/* call data of call created by this method */
	sipstack_call call;

	int i;

	/*build initial INVITE message */
	i = eXosip_call_build_initial_invite(&invite, to, from, NULL, subject);
	if (i != 0) {
		/* building of INVITE message failed */
		/* return error code */
		return -1;
	}

	osip_message_set_supported(invite, "100rel");

	{
		char tmp[4096];
		char localip[128];
		int port = 5061;

		eXosip_guess_localip(AF_INET, localip, 128);

		snprintf(tmp, 4096,
				 "v=0\r\n"
				 "o=josua 0 0 IN IP4 %s\r\n"
				 "s=conversation\r\n"
				 "c=IN IP4 %s\r\n"
				 "t=0 0\r\n"
				 "m=audio %i RTP/AVP 0 8 101\r\n"
				 "a=rtpmap:0 PCMU/8000\r\n"
				 "a=rtpmap:8 PCMA/8000\r\n"
				 "a=rtpmap:101 telephone-event/8000\r\n"
				 "a=fmtp:101 0-11\r\n", localip, localip, port);
		osip_message_set_body(invite, tmp, strlen(tmp));
		osip_message_set_content_type(invite, "application/sdp");
	}

	/* lock sip stack to avoid conflicts */
	eXosip_lock();

	/* send initial INVITE message */
	int sipCallId = eXosip_call_send_initial_invite(invite);
	if (sipCallId > 0) {
		void *reference;
		eXosip_call_set_reference(sipCallId, reference);
	}
	/* unlock sip stack for further use */
	eXosip_unlock();

	/* set transaction type (invite or non-invite) */
	sipstack_transaction_type type = INVITE;

	/* store call data */
	queue transactionIds = queue_create_queue(SIPSTACK_MAX_TRANSACTIONS);
	call =
		cm_build_sipstack_call(callId, sipCallId, -1, transactionIds,
							   type);
	/* add call */
	cm_add_call(call);

	/* return return status code */
	if (sipCallId >= 0) {
		/* sending INVITE succeeded */
		return 0;
	} else {
		/* sending INVITE failed */
		return -1;
	}
}

int sipstack_send_reinvite(int callId) {

	/* message which is build in this methode */
	osip_message_t *invite;

	/* get dialog id of the call */
	int dialogId = cm_get_sip_dialog_id_by_call_id(callId);
	if (dialogId == -1) {
		/* call has no active dialog */
		return -1;
	}

	/* build INVITE message on base of already send initial INVITE message */
	int i = eXosip_call_build_request(dialogId, "INVITE", &invite);

	if (i != 0) {
		/* building of message failed */
		/* return error code */
		return -1;
	}

	/* lock sip stack to avoid conflicts */
	eXosip_lock();

	/* send INVITE message */
	i = eXosip_call_send_request(dialogId, invite);

	/* unlock sip stack for further use */
	eXosip_unlock();

	/* return return code of message sending */
	return i;
}

/**
 * Terminate a call.
 * This method is called by sipstack_bye(), sipstack_cancel() and sipstack_decline()
 * because eXosip2 handles call termination by it self.
 * So a BYE, 603 DECLINE or CANCEL is send depending on the situation.
 * And there is no need to wait for a 200 OK because the hole call termination process is
 * done by this method.
 *
 * @param callId call id
 * @param dialogId dialog id
 * @return method result code
 */
int sipstack_terminate_call(int sipCallId, int sipDialogId) {

	/* lock sip stack to avoid conflicts */
	eXosip_lock();
	/* terminate call: */
	/* send BYE or CANCEL or 603 DECLINE depending on the situation */
	int i = eXosip_call_terminate(sipCallId, sipDialogId);

	/* unlock sip stack for further use */
	eXosip_unlock();

	/* return return code of message sending */
	return i;
}

int sipstack_bye(int callId) {
	/* call sipstack_terminate_call() because it handles call termination */

	/* get sip dialog id of the call */
	int sipDialogId = cm_get_sip_dialog_id_by_call_id(callId);
	/* get sip call id of the call */
	int sipCallId = cm_get_sip_dialog_id_by_call_id(callId);
	/* return return code of termination */
	return sipstack_terminate_call(sipCallId, sipDialogId);
}

int sipstack_cancel(int callId) {
	/* call sipstack_terminate_call() because it handles call termination */

	/* get sip call id of the call */
	int sipCallId = cm_get_sip_call_id_by_call_id(callId);
	/* get sip dialog id of the call */
	int sipDialogId = cm_get_sip_dialog_id_by_call_id(callId);
	/* return return code of termination */
	return sipstack_terminate_call(sipCallId, sipDialogId);
}

int sipstack_decline(int callId) {
	/* call sipstack_terminate_call() because it handles call termination */

	/* get sip dialog id of the call */
	int sipDialogId = cm_get_sip_dialog_id_by_call_id(callId);
	/* get sip call id of the call */
	int sipCallId = cm_get_sip_dialog_id_by_call_id(callId);
	/* return return code sipstack_terminate_call(of termination */
	return sipstack_terminate_call(sipCallId, sipDialogId);
}

int sipstack_send_ok(int callId) {

	/* message which is build in this methode */
	osip_message_t *ok = NULL;

	/* get dialog id of the call */
	int dialogId = cm_get_sip_dialog_id_by_call_id(callId);
	if (dialogId < 1) {
		/* call has no active dialog */
		return -1;
	}

	/* get current transaction id of the call */
	int transactionId = cm_get_sip_transaction_id_by_call_id(callId);
	if (transactionId < 1) {
		/* call has no active transaction */
		return -1;
	}

	/* lock sip stack to avoid conflicts */
	eXosip_lock();

	/* build 200 OK message */
	int i = eXosip_call_build_answer(transactionId, 200, &ok);

	if (i != 0) {
		/* send 400 message */
		eXosip_call_send_answer(transactionId, 400, NULL);
	} else {
		//i = sdp_complete_200ok (dialogId, ok);
		if (i != 0) {
			osip_message_free(ok);
			/* send 415 message */
			i = eXosip_call_send_answer(transactionId, 415, NULL);
		} else {
			/* send 200 OK message */
			i = eXosip_call_send_answer(transactionId, 200, ok);
		}
	}

	/* unlock sip stack for further use */
	eXosip_unlock();

	/*
	   remove transaction from transaction queue
	   if it's a non-invite transaction
	   because an ok always indicates the end of a
	   non-invite transaction
	 */
	sipstack_call call = cm_get_call_by_call_id(callId);
	if (call.type == NON_INVITE) {
		int j = cm_terminate_current_transaction(callId);
		if (j != 0) {
			fprintf(stderr, "Transaction termination failed.");
			return -1;
		}
	}
	/* return return code of message sending */
	return i;
}

int sipstack_send_acknowledgment(int callId) {

	/* message which is build in this methode */
	osip_message_t *ack = NULL;

	/* get dialog id of the call */
	int dialogId = cm_get_sip_dialog_id_by_call_id(callId);
	if (dialogId < 1) {
		/* call has no active dialog */
		fprintf(stderr, "Call has no active dialog (call id = %i)\n",
				callId);
		return -1;
	}

	/* build ACK message */
	int i = eXosip_call_build_ack(dialogId, &ack);

	if (i != 0) {
		/* building of message failed */
		/* return error code */
		return -1;
	}
	/* lock sip stack to avoid conflicts */
	eXosip_lock();

	/* send ACK message */
	i = eXosip_call_send_ack(dialogId, ack);

	/* unlock sip stack for further use */
	eXosip_unlock();

	/*
	   remove transaction from transaction queue
	   because an acknowledgement always indicates the end of a transaction
	 */
	int j = cm_terminate_current_transaction(callId);
	if (j != 0) {
		fprintf(stderr, "Transaction termination failed.");
		return -1;
	}

	/* return return code of message sending */
	return i;
}

int sipstack_send_status_code(int callId, int status_code) {

	/* message which is build in this methode */
	osip_message_t *answer = NULL;

	/* get current transaction id of the call */
	int transactionId = cm_get_sip_transaction_id_by_call_id(callId);
	if (transactionId == -1) {
		/* call has no active transaction */
		return -1;
	}

	/* build message with status code */
	int i = eXosip_call_build_answer(transactionId, status_code, &answer);

	/* lock sip stack to avoid conflicts */
	eXosip_lock();

	/* send message with status code */
	i = eXosip_call_send_answer(transactionId, status_code, answer);

	/* unlock sip stack for further use */
	eXosip_unlock();

	/* return return code of message sending */
	return i;
}

int sipstack_set_listener(int listener) {
	return 0;
}
