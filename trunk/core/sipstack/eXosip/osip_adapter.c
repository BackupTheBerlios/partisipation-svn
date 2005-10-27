/**
 * @file osip_adapter.c
 * A sip stack adapter for the eXosip library.
 *
 * @author Enrico Hartung <enrico@iptel.org>
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>

#include <util/threads/thread_management.h>
#include <util/logging/logger.h>
#include <util/config/globals.h>

#include <eXosip2/eXosip.h>
#include <osip2/osip.h>
#include <osipparser2/osip_parser.h>

#include "sipstack/sip_stack_interface.h"
#include "sipstack/sip_listener_interface.h"

int listenerIsActive = 0;

void *sip_listener(void *args) {
	LOG_INFO(SIPSTACK_MSG_PREFIX "Sip listener started");
	while (listenerIsActive == 1) {
		sipstack_event *event = sipstack_receive_event(1);

		/* create debug message on receiving an event */
		if (event) {
			if (event->ack == 1) {
				LOG_INFO(SIPSTACK_MSG_PREFIX "Received sip event (ACK).");
			} else {
				LOG_INFO(SIPSTACK_MSG_PREFIX "Received sip event (%i).",
						 event->statusCode);
			}
			/* send sipstack event to listener */
			sip_listener_receive_event(event);

		}

	}
	thread_terminated();
	return NULL;
}

/* the following methods are part of the sip stack API */

int sipstack_init() {

	/* used to get return codes */
	int rc;

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
	rc = eXosip_listen_addr(IPPROTO_UDP, INADDR_ANY,
							config.sipstack.eXosipAdapter.port, AF_INET,
							0);
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

	/* start the sip listener thread */
	listenerIsActive = 1;
	rc = start_thread(sip_listener, NULL);
	if (rc == 0) {
		log_message(LOG_ERROR,
					SIPSTACK_MSG_PREFIX
					"Sip listener thread could not be started");
	} else {
		log_message(LOG_DEBUG,
					SIPSTACK_MSG_PREFIX "Sip listener thread started");
	}

	/* return success code */
	return 0;
}

void sipstack_quit() {

	/* shut down sip listener */
	listenerIsActive = 0;

	/* wait for sip listener thread to terminate */
	sleep(1);

	log_message(LOG_DEBUG,
				SIPSTACK_MSG_PREFIX "Sip listener is shut down.");

	/* shut down eXosip */
	eXosip_quit();
	log_message(LOG_DEBUG, SIPSTACK_MSG_PREFIX "eXosip is shut down.");
}

sipstack_event *sipstack_map_event(eXosip_event_t * event) {

	/* sip stack event which will be returned */
	sipstack_event *sse;
	sse = (sipstack_event *) malloc(sizeof(sipstack_event));

	/* get response status code */
	sse->statusCode = event->response->status_code;
	/* get response message */
	sse->message = (char *) malloc(strlen(event->textinfo) * sizeof(char));
	strcpy(sse->message, event->textinfo);
	/* get call id */
	sse->callId = event->cid;
	/* get dialog id */
	sse->dialogId = event->did;
	/* get transaction id */
	sse->dialogId = event->tid;

	/* is event an acknowledgment */
	if (event->ack != NULL) {
		sse->ack = 1;
	} else {
		sse->ack = 0;
	}

	/* free memory of eXosip event */
	eXosip_event_free(event);

	/* return event */
	return sse;
}

sipstack_event *sipstack_receive_event(int timeout) {

	/* response */
	eXosip_event_t *je;

	/* sip stack event which will be returned */
	sipstack_event *sse;

	/* wait for response in seconds */
	je = eXosip_event_wait(timeout, 0);

	/* initiate some automatic actions (e.g. react on 3xx, 401/407) */
	//eXosip_automatic_action();

	if (je == NULL) {
		/* no response received in time */
		return NULL;
	} else {
		/* get sip stack event */
		sse = sipstack_map_event(je);
		/* return sip stack event */
		return sse;
	}
}

int sipstack_send_register(char *const identity, char *const registrar,
						   int expire) {

	/* message which is build in this methode */
	osip_message_t *reg = NULL;

	/* registration id */
	int regId;

	/* return value */
	int i;

	/* lock sip stack to avoid conflicts */
	eXosip_lock();
	/* build REGISTER message */
	regId =
		eXosip_register_build_initial_register(identity, registrar, NULL,
											   expire, &reg);

	if (regId < 0) {
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
	i = eXosip_register_send_register(regId, reg);
	/* unlock sip stack for further use */
	eXosip_unlock();

	if (i < 0) {
		/* sending of REGISTER message failed */
		/* return error code */
		return -1;
	}

	/* return registration id */
	return regId;
}

int sipstack_send_unregister(int regId) {

	/* message which is build in this methode */
	osip_message_t *reg = NULL;
	int i;

	/* lock sip stack to avoid conflicts */
	eXosip_lock();

	/* build REGISTER message */
	i = eXosip_register_build_register(regId, 0, &reg);

	if (i < 0) {
		/* building of REGISTER failed */
		/* unlock sip stack for further use */
		eXosip_unlock();
		/* return error code */
		return -1;
	}

	/* send REGISTER message */
	i = eXosip_register_send_register(regId, reg);
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

int sipstack_send_update_register(int regId, int expire) {

	/* message which is build in this methode */
	osip_message_t *reg = NULL;

	int i;

	/* lock sip stack to avoid conflicts */
	eXosip_lock();

	/* build REGISTER message */
	i = eXosip_register_build_register(regId, expire, &reg);

	if (i < 0) {
		/* building of REGISTER failed */

		/* unlock sip stack for further use */
		eXosip_unlock();

		/* return error code */
		return -1;
	}
	/* send REGISTER message */
	i = eXosip_register_send_register(regId, reg);

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

int sipstack_send_invite(char *to, char *from, char *subject) {

	/* message which is build in this methode */
	osip_message_t *invite;

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
	int callId = eXosip_call_send_initial_invite(invite);
	if (callId > 0) {
		void *reference;
		eXosip_call_set_reference(callId, reference);
	}
	/* unlock sip stack for further use */
	eXosip_unlock();

	/* return call id */
	return callId;
}

int sipstack_send_reinvite(int dialogId) {

	/* message which is build in this methode */
	osip_message_t *invite;

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
int sipstack_terminate(int callId, int dialogId) {

	/* lock sip stack to avoid conflicts */
	eXosip_lock();
	/* terminate call: */
	/* send BYE or CANCEL or 603 DECLINE depending on the situation */
	int i = eXosip_call_terminate(callId, dialogId);

	/* unlock sip stack for further use */
	eXosip_unlock();

	/* return return code of message sending */
	return i;
}

int sipstack_bye(int callId, int dialogId) {
	/* call sipstack_terminate_call() because it handles call termination */

	/* return return code of termination */
	return sipstack_terminate(callId, dialogId);
}

int sipstack_cancel(int callId, int dialogId) {
	/* call sipstack_terminate_call() because it handles call termination */

	/* return return code of termination */
	return sipstack_terminate(callId, dialogId);
}

int sipstack_decline(int callId, int dialogId) {
	/* call sipstack_terminate_call() because it handles call termination */

	/* return return code sipstack_terminate_call(of termination */
	return sipstack_terminate(callId, dialogId);
}

int sipstack_send_ok(int dialogId, int transactionId) {

	/* message which is build in this methode */
	osip_message_t *ok = NULL;

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

	/* return return code of message sending */
	return i;
}

int sipstack_send_acknowledgment(int dialogId) {

	/* message which is build in this methode */
	osip_message_t *ack = NULL;

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

	/* return return code of message sending */
	return i;
}

int sipstack_send_status_code(int transactionId, int status_code) {

	/* message which is build in this methode */
	osip_message_t *answer = NULL;

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
