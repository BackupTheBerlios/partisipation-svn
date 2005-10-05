/**
 * @file sip_stack_interface.h
 * Sip Stack Adapter
 *
 * This file provide the API needed to control communication with the sip stack.
 * @author Enrico Hartung <enrico@iptel.org>
 */

/**
 * @defgroup sipstack Sip Stack
 * The API needed to control communication with the sip stack.
 * It is only one dialog per call supported. A call can consist of several transactions
 * but they are queued so that only response for the first transaction in the queue is handled.
 *
 * It can be used to:
 *
 * <ul>
 * <li>send an initial REGISTER</li>
 * <li>send a REGISTER to update registration</li>
 * <li>send a REGISTER to unregister</li>
 * <li>send an initial INVITE</li>
 * <li>send a reINVITE</li>
 * <li>send a 200 OK</li>
 * <li>terminate a call</li>
 * </ul>
 * @{
 */
#ifndef HSIPSTACK_INTERFACE_USED
#define HSIPSTACK_INTERFACE_USED

#include <util/logging/logger.h>
#include <util/config/config_reader.h>

/* prefix for debug and error messages */
#define SIPSTACK_MSG_PREFIX "[sipstack adapter] "

typedef struct {
	int statusCode;
	char *message;
	int callId;
	int transactionId;
	int ack;
} sipstack_event;

/**
 * Init sip stack. This method is supposed to be called before using the sip stack.
 *
 * @param port port to listen on
 */
int sipstack_init(int port);

/**
 * Shut down sip stack. This method is supposed to be called if the sip stack is no more needed.
 * This includes the case that an error occures.
 */
void sipstack_quit();

/**
 * Receive the response of a request. An event of following structure is returned:
 * int event->status_code  the status code of the response
 * char* event->message    the message the response contains
 *
 * @todo Implement transaction termination to support transactions. Transactions shall be
 *			dequeued if their end is reached. Therefore it must be differed between INVITE and
 *			non-INVITE transactions. An INVITE transaction ends with an ACK and a non-INVITE
 			transaction with the OK.
 * @param timeout time to wait for response in seconds
 * @return response event
 */
sipstack_event sipstack_receive_event(int timeout);

/**
 * Send initial register to start a registration.
 * After sending the REGISTER this method waits for a response.
 * If no response is received in 5 seconds or an error is received this methode returns -1.
 *
 * @param identity identity of user (e.g. abc@domain.org)
 * @param registrar server where to register
 * @param expire time to live of registration in seconds
 * @return registration id which can be used to update the registration or to unregister
 */
int sipstack_send_register(char *const identity, char *const registrar, int expire);
/**
 * Updates a registration and sets its expiration time to a given value.
 * After sending the REGISTER this method waits for a response.
 * If no response is received in 5 seconds or an error is received this methode returns -1.
 *
 * @param regId registration id returned by sipstack_send_register()
 * @param expire new expiration time
 * @return result code
 */
int sipstack_send_update_register(int regId, int expire);

/**
 * Deletes a registration. Therefore the expiration time is set to 0.
 * After sending the REGISTER this method waits for a response.
 * If no response is received in 5 seconds or an error is received this methode returns -1.
 *
 * @param regId registration id returned by sipstack_send_register()
 * @return result code
 */
int sipstack_send_unregister(int regId);

/**
 * Send an initial INVITE. It returns a dialog id which is supposed to be used for
 * further messages of this call.
 *
 * @param to sip url of callee
 * @param from sip url of caller
 * @param subject subject of call
 * @return call id (-1 if sending of INVITE failed)
 */
int sipstack_send_invite(char *to, char *from, char *subject);

/**
 * Send an reINVITE. Therefore a call id is needed
 * to identify the call this reINVITE belongs to.
 *
 * @param dialogId dialog id
 * @return return code (-1 if sending of reINVITE failed)
 */
int sipstack_send_reinvite(int dialogId);

/**
 * Terminate a call by sending BYE, CANCEL or DECLINE.
 * There is no need to wait for a 200 OK because the hole call termination process is
 * done by this method.
 *
 * @param callId call id
 * @param dialogId dialog id
 * @return method result code
 */
int sipstack_terminate(int callId, int dialogId) {

/**
 * Terminate a call by sending BYE.
 * There is no need to wait for a 200 OK because the hole call termination process is
 * done by this method.
 *
 * @param callId call id
 * @param dialogId dialog id
 * @return method result code
 */
int sipstack_bye(int callId, int dialogId);

/**
 * Terminate a call by sending CANCEL.
 * There is no need to wait for a 200 OK because the hole call termination process is
 * done by this method.
 *
 * @param callId call id
 * @param dialogId dialog id
 * @return method result code
 */
int sipstack_cancel(int callId, int dialogId);

/**
 * Terminate a call by sending DECLINE.
 * There is no need to wait for a 200 OK because the hole call termination process is
 * done by this method.
 *
 * @param callId call id
 * @param dialogId dialog id
 * @return method result code
 */
int sipstack_decline(int callId, int dialogId);

/**
 * Send a 200 OK.
 * This method sends a 200 OK as a response to an INVITE or BYE.
 *
 * @param dialogId dialog id
 * @param transactionId transaction id
 * @return method result code
 */
int sipstack_send_ok(int dialogId, int transactionId);

/**
 * Build and send a default ACK for a 200 OK received.
 * This method sends a 200 OK as a response to an INVITE or BYE.
 *
 * @param dialogId call id
 * @return method result code
 */
int sipstack_send_acknowledgment(int dialogId);

/**
 * Build and send a status code message.
 *
 * @param transactionId call id
 * @param status_code status code to send in message
 * @return method result code
 */
int sipstack_send_status_code(int transactionId, int status_code);

/**
 * Set sip stack listener.
 * @deprecated
 */
int sipstack_set_listener(int listener);

/** @} */

#endif
